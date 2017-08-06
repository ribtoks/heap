#ifndef LOGGER_H
#define LOGGER_H

#include <QStringList>
#include <QString>
#include <QWaitCondition>
#include <QMutex>

class Logger
{
public:
    static Logger& getInstance()
    {
        static Logger instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

public:
    void setLogFilePath(const QString &filepath) {
        m_LogFilepath = filepath;
    }

    QString getLogFilePath() const { return m_LogFilepath; }

    void log(const QString &message);
    void flush();
    void stop();

private:
    void doLog(const QString &message);
    void flushStream(QStringList *logItems);

private:
    Logger() {
        m_QueueLogTo = &m_LogsStorage[0];
        m_QueueFlushFrom = &m_LogsStorage[1];
        m_Stopped = false;
    }

    Logger(Logger const&);
    void operator=(Logger const&);

private:
    QString m_LogFilepath;
    QStringList m_LogsStorage[2];
    QStringList *m_QueueFlushFrom;
    QStringList *m_QueueLogTo;
    QMutex m_LogMutex;
    QMutex m_FlushMutex;
    QWaitCondition m_AnyLogsToFlush;
    volatile bool m_Stopped;
};

#endif // LOGGER_H
