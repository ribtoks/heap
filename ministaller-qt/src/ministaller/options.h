/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>

struct ParsedOptions {
public:
    ParsedOptions():
        m_PidWaitFor(0),
        m_GenerateDiff(false),
        m_KeepMissing(false),
        m_ForceUpdate(false),
        m_FindUsefulDirs(false)
    {
#ifdef QT_DEBUG
        m_FailTheInstall = false;
#endif
    }

public:
    QString m_PackageConfigPath;
    QString m_InstallDir;
    QString m_PackagePath;
    QString m_LaunchExePath;
    int m_PidWaitFor;
    bool m_GenerateDiff;
    bool m_KeepMissing;
    bool m_ForceUpdate;
    bool m_FindUsefulDirs;
#ifdef QT_DEBUG
    bool m_FailTheInstall;
#endif
};

#endif // OPTIONS_H
