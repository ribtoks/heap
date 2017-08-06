#include <QCoreApplication>
#include <QLocalServer>
#include <QStandardPaths>
#include <QString>
#include <QThread>
#include <iostream>
#include "requestscoordinator.h"
#include "exiv2inithelper.h"
#include "logger.h"
#include "loggingworker.h"

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString logLine = qFormatLogMessage(type, context, msg);
#else
    QString msgType;
    switch (type) {
        case QtDebugMsg:
            msgType = "debug";
            break;
        case QtWarningMsg:
            msgType = "warning";
            break;
        case QtCriticalMsg:
            msgType = "critical";
            break;
        case QtFatalMsg:
            msgType = "fatal";
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 1))
        case QtInfoMsg:
            msgType = "info";
            break;
#endif
    }

    // %{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}
    QString time = QDateTime::currentDateTimeUtc().toString("hh:mm:ss.zzz");
    QString logLine = QString("%1 %2 T#%3 %4 - %5")
                          .arg(time).arg(msgType)
                          .arg(0).arg(context.function)
                          .arg(msg);
#endif

    Logger &logger = Logger::getInstance();
    logger.log(logLine);

    if (type == QtFatalMsg) {
        abort();
    }
}

void startLogging(LoggingWorker *worker) {
    QThread *loggingThread = new QThread();
    worker->moveToThread(loggingThread);

    QObject::connect(loggingThread, &QThread::started, worker, &LoggingWorker::process);
    QObject::connect(worker, &LoggingWorker::stopped, loggingThread, &QThread::quit);

    QObject::connect(worker, &LoggingWorker::stopped, worker, &LoggingWorker::deleteLater);
    QObject::connect(loggingThread, &QThread::finished, loggingThread, &QThread::deleteLater);

    loggingThread->start();
}

int main(int argc, char *argv[]) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    qSetMessagePattern("%{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}");
#endif

    qInstallMessageHandler(myMessageHandler);

    Exiv2InitHelper exiv2Init;
    Q_UNUSED(exiv2Init);

    QString appDataPath = QONSOMME_USERDATA_PATH;
#ifdef WITH_LOGS
    const QString &logFileDir = QDir::cleanPath(appDataPath + QDir::separator() + "logs");
    if (!logFileDir.isEmpty()) {
        QDir dir(logFileDir);
        if (!dir.exists()) {
            bool created = QDir().mkpath(logFileDir);
            Q_UNUSED(created);
        }

        QString time = QDateTime::currentDateTimeUtc().toString("ddMMyyyy-hhmmss-zzz");
        QString logFilename = QString("qonsomme-%1.log").arg(time);

        QString logFilePath = dir.filePath(logFilename);

        Helpers::Logger &logger = Helpers::Logger::getInstance();
        logger.setLogFilePath(logFilePath);
    }
#endif

    LoggingWorker loggingWorker;
    startLogging(&loggingWorker);

    QCoreApplication app(argc, argv);

    QString serverName;
    QStringList arguments = app.arguments();

    if (arguments.size() > 2) {
        std::cerr << "Wrong arguments number:" << arguments.size();
        return 1;
    }

    if ((arguments.size() <= 1) || arguments.at(1).isEmpty()) {
        serverName = "qonsomme";
    } else {
        serverName = arguments.at(1);
    }

    QLocalServer server;
    RequestsCoordinator coordinator(&server);

    QObject::connect(&coordinator, &RequestsCoordinator::stopRequest,
                     &loggingWorker, &LoggingWorker::cancel);
    QObject::connect(&coordinator, &RequestsCoordinator::stopRequest,
                     &app, &QCoreApplication::quit);

    coordinator.startWorkers();

    if (!server.listen(serverName)) {
        return 2;
    }

    LOG_INFO << "Listening to" << serverName;

    return app.exec();
}
