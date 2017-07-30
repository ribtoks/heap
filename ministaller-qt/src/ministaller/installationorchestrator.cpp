/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "installationorchestrator.h"
#include <memory>
#include <QFileInfo>
#include <QDir>
#include <QThread>
#include <QProcess>
#include <QTemporaryDir>
#include <QCoreApplication>
#include "fshelpers.h"
#include "packageparser.h"
#include "../common/diffgeneratorbase.h"
#include "packageinstaller.h"
#include "../common/ifilesprovider.h"
#include "livelog.h"

QString findMeaningfulDir(const QString &startingDir) {
    QDir dir(startingDir);
    QFileInfoList fileInfos;
    bool canContinue = false;

    do {
        fileInfos = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        canContinue = (fileInfos.length() == 1) && (fileInfos.first().isDir());
        if (canContinue) {
            dir.cd(fileInfos.first().fileName());
        }
    } while(canContinue);

    return dir.absolutePath();
}

InstallationOrchestrator::InstallationOrchestrator(const ParsedOptions &options, UIHelper &uiHelper, QObject *parent) :
    QObject(parent),
    m_Options(options),
    m_UIHelper(uiHelper)
{
}

void InstallationOrchestrator::process() {
    qDebug() << "#";

    try {
        doWork();
    } catch(...) {

    }

    auto &liveLog = LiveLog::getInstance();
    liveLog.saveLog();

    emit finished();

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void InstallationOrchestrator::doWork() {
    m_UIHelper.systemMessage(tr("Preparing the installation..."));

    QTemporaryDir extractDir;
    if (!extractDir.isValid()) {
        qWarning() << "Failed to create temp dir";
        return;
    }

    QString extractedDirPath = extractDir.path();
    if (!extractPackage(m_Options.m_PackagePath, extractedDirPath)) {
        qWarning() << "Failed to extract archive";
        return;
    }

    QTemporaryDir backupDir;
    if (!backupDir.isValid()) {
        qWarning() << "Failed to create backup directory";
        return;
    }

    if (m_Options.m_FindUsefulDirs) {
        extractedDirPath = findMeaningfulDir(extractedDirPath);
        qInfo() << "Using" << extractedDirPath << "as package dir";
    }

    if (m_UIHelper.getShouldCancel()) { return; }

    std::shared_ptr<IFilesProvider> filesProvider;
    if (m_Options.m_GenerateDiff) {
        qDebug() << "Generating directories differences...";
        std::shared_ptr<DiffGeneratorBase> diffGenerator(new DiffGeneratorBase(m_Options.m_InstallDir, extractedDirPath,
                                                                               m_Options.m_ForceUpdate, m_Options.m_KeepMissing));
        diffGenerator->generateDiffs();
        filesProvider = std::dynamic_pointer_cast<IFilesProvider>(diffGenerator);
    } else {
        qDebug() << "Parsing package config...";
        std::shared_ptr<PackageParser> packageParser(new PackageParser(m_Options.m_PackageConfigPath));

        if (!packageParser->parsePackage()) {
            qWarning() << "Cannot parse package config";
            return;
        }

        filesProvider = std::dynamic_pointer_cast<IFilesProvider>(packageParser);
    }

#ifdef QT_DEBUG
    bool alwaysFail = m_Options.m_FailTheInstall;
#else
    bool alwaysFail = false;
#endif

    PackageInstaller packageInstaller(filesProvider, m_UIHelper, alwaysFail);
    packageInstaller.setInstallDir(m_Options.m_InstallDir);
    packageInstaller.setPackageDir(extractedDirPath);
    packageInstaller.setBackupDir(backupDir.path());

    if (m_UIHelper.getShouldCancel()) { return; }

    if (packageInstaller.install()) {
        QDir installDir = m_Options.m_InstallDir;

        if (!m_Options.m_LaunchExePath.isEmpty()) {
            QString exePath = installDir.filePath(m_Options.m_LaunchExePath);
            QFileInfo executableFI(exePath);
            if (executableFI.exists() && executableFI.isFile()) {
                qInfo() << "Starting" << exePath;
                QProcess::startDetached(exePath);
            } else {
                qWarning() << exePath << "not found or not file";
            }
        }
    }
}
