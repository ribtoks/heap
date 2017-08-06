#ifndef REQUESTSCOORDINATOR_H
#define REQUESTSCOORDINATOR_H

#include <QObject>
#include <QLocalServer>
#include <vector>
#include "readerwriterqueue.h"
#include "consolidationresult.h"

class MetadataConsolidatingWorker;
class ResultsSendingWorker;

class RequestsCoordinator : public QObject
{
    Q_OBJECT
public:
    explicit RequestsCoordinator(QLocalServer *server, QObject *parent = 0);

public:
    void startWorkers();
    void stopWorkers();
    void startSendingResults(QLocalSocket *socket);

signals:
    void stopRequest();

public slots:
    void handleConnection();
    void handleReadyRead();

private:
    MetadataConsolidatingWorker *getNextWorker();

private:
    std::vector<MetadataConsolidatingWorker*> m_Workers;
    ResultsSendingWorker *m_ResultsSendingWorker;
    ReaderWriterQueue<ConsolidationResult> m_QueueOut;
    QLocalServer *m_Server;
    // this version is designed to work with 1 client only so it has just 1 block size
    int m_BlockSize;
    int m_LastWorkerIndex;
};

#endif // REQUESTSCOORDINATOR_H
