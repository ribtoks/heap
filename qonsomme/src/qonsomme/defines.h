#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>

#if (QT_VERSION <= QT_VERSION_CHECK(5, 4, 2))
#define qInfo qDebug
#endif

#define LOG_DEBUG qDebug()
#define LOG_INFO qInfo()
#define LOG_WARNING qWarning()

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
#define QONSOMME_DATA_LOCATION_TYPE QStandardPaths::AppDataLocation
#define QONSOMME_USERDATA_PATH QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
#define QONSOMME_DATA_LOCATION_TYPE QStandardPaths::DataLocation
#define QONSOMME_USERDATA_PATH QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif

#endif // DEFINES_H
