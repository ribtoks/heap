#include "loggingworker.h"
#include "logger.h"
#include <QThread>
#include <iostream>
#include "defines.h"

LoggingWorker::LoggingWorker(QObject *parent) :
    QObject(parent),
    m_Cancel(false)
{
}

void LoggingWorker::process() {
    Logger &logger = Logger::getInstance();
    const int sleepTimeout = 700;

    while (!m_Cancel) {
        logger.flush();
        QThread::msleep(sleepTimeout);
    }

    emit stopped();
}

void LoggingWorker::cancel() {
    m_Cancel = true;

    Logger &logger = Logger::getInstance();
    logger.stop();
}
