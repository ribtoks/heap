#ifndef RESULTSSENDINGWORKER_H
#define RESULTSSENDINGWORKER_H

#include <QObject>
#include "readerwriterqueue.h"
#include "consolidationresult.h"

class QLocalSocket;

class ResultsSendingWorker : public QObject
{
    Q_OBJECT
public:
    explicit ResultsSendingWorker(ReaderWriterQueue<ConsolidationResult> *queueOut, QLocalSocket *socket, QObject *parent = 0);

public slots:
    void process();
    void cancel();

signals:
    void stopped();

private:
     void doWork();
     void sendResult(const std::shared_ptr<ConsolidationResult> &result);

private:
    QLocalSocket *m_Socket;
    ReaderWriterQueue<ConsolidationResult> *m_QueueOut;
    volatile bool m_Cancel;
};

#endif // RESULTSSENDINGWORKER_H
