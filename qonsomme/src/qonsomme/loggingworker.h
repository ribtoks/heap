#ifndef LOGGINGWORKER_H
#define LOGGINGWORKER_H

#include <QObject>

class LoggingWorker : public QObject
{
    Q_OBJECT
public:
    explicit LoggingWorker(QObject *parent = 0);
    virtual ~LoggingWorker() {}

signals:
    void stopped();

public slots:
    void process();
    void cancel();

private:
    volatile bool m_Cancel;
};

#endif // LOGGINGWORKER_H
