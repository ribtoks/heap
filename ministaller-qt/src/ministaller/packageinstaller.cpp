/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "packageinstaller.h"
#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <string.h>
#include <cerrno>
#include "livelog.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

bool waitForProcess(PLATFORM_PID pid) {
#if defined(Q_OS_UNIX)
    pid_t result = ::waitpid(pid, 0, 0);
    if (result < 0) {
        qWarning() << "waitpid() failed with error:" << QString::fromLocal8Bit(strerror(errno));
    }

    return result > 0;

#elif defined(Q_OS_WIN)
    HANDLE hProc;

    if (!(hProc = OpenProcess(SYNCHRONIZE, FALSE, pid))) {
        qWarning() << "Unable to get process handle for pid" << pid << "last error" << GetLastError();
        return false;
    }

    DWORD dwRet = WaitForSingleObject(hProc, INFINITE);
    CloseHandle(hProc);

    if (dwRet == WAIT_FAILED) {
        qWarning() << "WaitForSingleObject failed with error" << GetLastError();
    }

    return (dwRet == WAIT_OBJECT_0);
#endif
}

void cleanupEmptyDirectories(const QString &baseDirectory) {
    qInfo() << baseDirectory;

    QDirIterator it(baseDirectory, QDir::NoDotAndDotDot | QDir::Dirs, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        QString subdirPath = it.next();

        cleanupEmptyDirectories(subdirPath);

        QDir subdir(subdirPath);
        if (subdir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).empty()) {
            qInfo() << "Removing empty directory:" << subdirPath;
            bool removeResult = subdir.removeRecursively();
            if (!removeResult) {
                qWarning() << "Failed to remove directory:" << subdirPath;
            }
        }
    }
}

QString joinPath(const QString& path1, const QString& path2) {
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

bool moveFile(const QString &from, const QString &to) {
    bool success = false;

    QFile destination(to);
    if (destination.exists()) {
        if (!destination.remove()) {
            return success;
        }
    }

    QFile source(from);
    if (source.exists()) {
        success = source.rename(to);
    }

    return success;
}

PackageInstaller::PackageInstaller(const std::shared_ptr<IFilesProvider> &filesProvider, UIHelper &uiHelper, bool alwaysFail):
    m_PidWaitFor(0),
    m_FilesProvider(filesProvider),
    m_UIHelper(uiHelper),
    m_AlwaysFail(alwaysFail)
{
}

bool PackageInstaller::install() {
    bool success = false;
    qDebug() << "#";

    beforeInstall();

    success = installPackage();

    if (success) {
        afterSuccess();
    } else {
        afterFailure();
    }

    return success;
}

void PackageInstaller::beforeInstall() {
    qDebug() << "#";

    if (m_PidWaitFor != 0) {
        m_UIHelper.systemMessage(tr("Waiting for the app to exit..."));

        qInfo() << "Waiting for pid:" << m_PidWaitFor;
        waitForProcess(m_PidWaitFor);
    }
}

bool PackageInstaller::installPackage() {
    qDebug() << "#";

    bool success = false;

    do {
        m_UIHelper.systemMessage(tr("Removing components..."));
        if (!removeFilesToRemove()) {
            break;
        }

        if (m_UIHelper.getShouldCancel()) { break; }

        m_UIHelper.systemMessage(tr("Updating components..."));
        if (!updateFilesToUpdate()) {
            break;
        }

        if (m_UIHelper.getShouldCancel()) { break; }

        m_UIHelper.systemMessage(tr("Adding components..."));
        if (!addFilesToAdd()) {
            break;
        }

        if (m_UIHelper.getShouldCancel()) { break; }

#ifdef QT_DEBUG
        if (m_AlwaysFail) { break; }
#endif

        success = true;
    } while (false);

    return success;
}

void PackageInstaller::afterSuccess() {
    qDebug() << "#";
    m_UIHelper.systemMessage(tr("Finishing the installation..."));

    cleanupEmptyDirectories(m_InstallDir);
    removeBackups();
}

void PackageInstaller::afterFailure() {
    qDebug() << "#";
    m_UIHelper.systemMessage(tr("Cleaning up..."));

    removeFilesToAdd();
    restoreBackups();
    cleanupEmptyDirectories(m_InstallDir);
    removeBackups();
}

bool PackageInstaller::addFilesToAdd() {
    qDebug() << "#";
    int addedFilesCount = 0,
            failedToAddFilesCount = 0;

    auto &itemsToAdd = m_FilesProvider->getItemsToAdd();

    for (auto &item: itemsToAdd) {
        auto &path = item.m_Filepath;
        QString fullPathToAdd = joinPath(m_PackageDir, path);
        QString possibleExistingPath = joinPath(m_InstallDir, path);

        Q_ASSERT(QFileInfo(fullPathToAdd).exists());
        Q_ASSERT(!QFileInfo(possibleExistingPath).exists());

        ensureDirectoryExistsForFile(possibleExistingPath);

        if (moveFile(fullPathToAdd, possibleExistingPath)) {
            addedFilesCount++;
        } else {
            failedToAddFilesCount++;
            qWarning() << "Failed to install file" << fullPathToAdd;
        }
    }

    qInfo() << "Added" << addedFilesCount << "files";
    qInfo() << "Failed to add" << failedToAddFilesCount << "files";

    return (failedToAddFilesCount == 0);
}

bool PackageInstaller::updateFilesToUpdate() {
    qDebug() << "#";
    int updatedFilesCount = 0,
            failedToUpdateCount = 0;

    auto &itemsToUpdate = m_FilesProvider->getItemsToUpdate();
    QDir installDir(m_InstallDir);

    for (auto &item: itemsToUpdate) {
        auto &path = item.m_Filepath;
        QString fullPathToUpdate = joinPath(m_PackageDir, path);
        QString existingPath = joinPath(m_InstallDir, path);

        Q_ASSERT(QFileInfo(fullPathToUpdate).exists());
        Q_ASSERT(QFileInfo(existingPath).exists());

        backupPath(installDir, existingPath);
        ensureDirectoryExistsForFile(existingPath);

        if (moveFile(fullPathToUpdate, existingPath)) {
            updatedFilesCount++;
        } else {
            failedToUpdateCount++;
            qWarning() << "Failed to update file" << existingPath;
        }
    }

    qInfo() << "Updated" << updatedFilesCount << "files";
    qInfo() << "Failed to update" << failedToUpdateCount << "files";

    return (failedToUpdateCount == 0);
}

bool PackageInstaller::removeFilesToRemove() {
    qDebug() << "#";
    int removedFilesCount = 0,
            failedToRemoveCount = 0;

    auto &itemsToRemove = m_FilesProvider->getItemsToRemove();
    QDir installDir(m_InstallDir);

    for (auto &item: itemsToRemove) {
        auto &path = item.m_Filepath;
        QString fullPathToRemove = joinPath(m_InstallDir, path);

        backupPath(installDir, fullPathToRemove);

        if (QFile::remove(fullPathToRemove)) {
            removedFilesCount++;
        } else {
            if (QFileInfo(fullPathToRemove).exists()) {
                failedToRemoveCount++;
                qWarning() << "Failed to remove:" << fullPathToRemove;
            } else {
                qInfo() << fullPathToRemove << "does not exist anymore";
            }
        }
    }

    qInfo() << "Removed" << removedFilesCount << "files";
    qInfo() << "Failed to remove" << failedToRemoveCount << "files";

    return (failedToRemoveCount == 0);
}

void PackageInstaller::removeFilesToAdd() {
    qDebug() << "#";

    int removedFilesCount = 0,
            failedToRemoveCount = 0;

    auto &itemsToAdd = m_FilesProvider->getItemsToAdd();

    for (auto &item: itemsToAdd) {
        auto &path = item.m_Filepath;
        QString fullPathToRemove = joinPath(m_InstallDir, path);

        if (QFile::remove(fullPathToRemove)) {
            removedFilesCount++;
        } else {
            failedToRemoveCount++;
            qWarning() << "Failed to remove:" << fullPathToRemove;
        }
    }

    qInfo() << "Removed" << removedFilesCount << "files";
    qInfo() << "Failed to remove" << failedToRemoveCount << "files";
}

void PackageInstaller::backupPath(const QDir &fileDir, const QString &fullpath) {
    qInfo() << fullpath;

    Q_ASSERT(!m_BackupPaths.contains(fullpath));

    if (!QFileInfo(fullpath).exists()) {
        return;
    }

    QString relativePath = fileDir.relativeFilePath(fullpath);

    QString backupPath = joinPath(m_BackupDir, relativePath + ".bak");
    ensureDirectoryExistsForFile(backupPath);

    {
        QFile backupFile(backupPath);
        if (backupFile.exists()) {
            backupFile.remove();
        }
        backupFile.close();
    }

    if (QFile::copy(fullpath, backupPath)) {
        m_BackupPaths[fullpath] = backupPath;
    } else {
        qWarning() << "Failed to backup" << fullpath;
    }
}

void PackageInstaller::removeBackups() {
    qDebug() << "#";
    QHashIterator<QString, QString> it(m_BackupPaths);
    int removedCount = 0;

    while (it.hasNext()) {
        it.next();
        const QString &backupPath = it.value();

        QFile backupFile(backupPath);
        if (backupFile.exists()) {
            if (backupFile.remove()) {
                removedCount++;
            } else {
                qWarning() << "Failed to cleanup backup:" << backupPath;
            }
        } else {
            qDebug() << "Backup path does not exist anymore:" << backupPath;
        }
    }

    qInfo() << removedCount << "files removed";

    if (QDir(m_BackupDir).removeRecursively()) {
        qInfo() << "Removed" << m_BackupDir;
    } else {
        qWarning() << "Failed to remove" << m_BackupDir;
    }
}

void PackageInstaller::restoreBackups() {
    qDebug() << "#";
    QHashIterator<QString, QString> it(m_BackupPaths);
    int restoredCount = 0;

    while (it.hasNext()) {
        it.next();

        const QString &originalFilepath = it.key();

        QFile originalFile(originalFilepath);
        if (originalFile.exists()) {
            originalFile.remove();
        }

        const QString &backupFilepath = it.value();
        if (QFileInfo(backupFilepath).exists()) {
            if (moveFile(backupFilepath, originalFilepath)) {
                restoredCount++;
            } else {
                qWarning() << "Failed to restore file:" << originalFilepath;
            }
        } else {
            qDebug() << "Backup path does not exist anymore:" << backupFilepath;
        }
    }

    qInfo() << restoredCount << "out of" << m_BackupPaths.size() << "files restored";
}

void PackageInstaller::ensureDirectoryExistsForFile(const QString &filepath) {
    QFileInfo fi(filepath);

    if (fi.exists()) { return; }

    QDir filesDir = fi.absoluteDir();
    if (!filesDir.exists()) {
        if (!filesDir.mkpath(".")) {
            qWarning() << "Failed to create a path:" << filesDir.path();
        }
    }
}
