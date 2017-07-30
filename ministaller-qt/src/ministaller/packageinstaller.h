/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef PACKAGEINSTALLER_H
#define PACKAGEINSTALLER_H

#include <memory>
#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>
#include <QDir>
#include "platform.h"
#include "uihelper.h"
#include "../common/fileentry.h"
#include "../common/ifilesprovider.h"

#ifdef Q_OS_WIN
// for the platform_pid DWORD
#include "windows.h"
#endif

class PackageInstaller : public QObject
{
    Q_OBJECT
public:
    PackageInstaller(const std::shared_ptr<IFilesProvider> &filesProvider, UIHelper &uiHelper, bool alwaysFail=false);

public:
    void setInstallDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_InstallDir = dir; }
    void setPackageDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_PackageDir = dir; }
    void setBackupDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_BackupDir = dir; }
    void setPidWaitFor(PLATFORM_PID pid) { m_PidWaitFor = pid; }

public:
    bool install();

private:
    void beforeInstall();
    bool installPackage();
    void afterSuccess();
    void afterFailure();

private:
    bool addFilesToAdd();
    bool updateFilesToUpdate();
    bool removeFilesToRemove();
    void removeFilesToAdd();
    void backupPath(const QDir &fileDir, const QString &fullpath);
    void removeBackups();
    void restoreBackups();
    void ensureDirectoryExistsForFile(const QString &filepath);

private:
    QString m_InstallDir;
    QString m_PackageDir;
    QString m_BackupDir;
    UIHelper &m_UIHelper;
    bool m_AlwaysFail;
    PLATFORM_PID m_PidWaitFor;
    std::shared_ptr<IFilesProvider> m_FilesProvider;
    QHash<QString, QString> m_BackupPaths;
};

#endif // PACKAGEINSTALLER_H
