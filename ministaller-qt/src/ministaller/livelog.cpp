/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "livelog.h"
#include <iostream>
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
#define MINISTALLER_USERDATA_PATH QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
#define MINISTALLER_USERDATA_PATH QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif

void LiveLog::log(const QString &message) {
    m_JoinedLog.append(message);
    m_JoinedLog.append(QChar::LineSeparator);

#ifdef QT_DEBUG
    std::cout << message.toStdString() << std::endl;
#endif

    emit logContentsChanged();
}

void LiveLog::saveLog() {
    QString appDataPath = MINISTALLER_USERDATA_PATH;

    const QString &logFileDir = QDir::cleanPath(appDataPath + QDir::separator() + "logs");
    if (!logFileDir.isEmpty()) {
        QDir dir(logFileDir);
        if (!dir.exists()) {
            bool created = QDir().mkpath(logFileDir);
            Q_UNUSED(created);
        }

        QString time = QDateTime::currentDateTimeUtc().toString("ddMMyyyy-hhmmss-zzz");
        QString filename = QString("ministaller-log-%1.log").arg(time);

        QString logFilePath = dir.filePath(filename);

        QFile file(logFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_JoinedLog.toUtf8());
        }
    }
}
