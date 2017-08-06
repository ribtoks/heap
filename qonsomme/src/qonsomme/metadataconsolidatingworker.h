#ifndef METADATACONSOLIDATINGWORKER_H
#define METADATACONSOLIDATINGWORKER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <deque>
#include <memory>
#include "consolidaterequest.h"
#include "consolidationresult.h"
#include "readerwriterqueue.h"

class MetadataConsolidatingWorker : public QObject
{
    Q_OBJECT
public:
    explicit MetadataConsolidatingWorker(ReaderWriterQueue<ConsolidationResult> *queueOut, QObject *parent = 0);

public:
    void submitItem(const std::shared_ptr<ConsolidateRequest> &item);
    void submitItems(const std::vector<std::shared_ptr<ConsolidateRequest> > &items);
    void cancelCurrentBatch();

    bool isCancelled() const { return m_Cancel; }
    bool isRunning() const { return m_IsRunning; }

    void doWork();
    void stopWorking(bool immediately=true);

public slots:
     void process() { doWork(); }
     void cancel() { stopWorking(); }

 signals:
     void stopped();
     void queueIsEmpty();

private:
    bool initWorker();
    void processOneItem(std::shared_ptr<ConsolidateRequest> &item);

    void onQueueIsEmpty() { emit queueIsEmpty(); }
    void workerStopped() { emit stopped(); }

    void runWorkerLoop();

private:
    ReaderWriterQueue<ConsolidationResult> *m_ResultsQueue;
    QWaitCondition m_WaitAnyItem;
    QMutex m_QueueMutex;
    std::deque<std::shared_ptr<ConsolidateRequest> > m_Queue;
    volatile bool m_Cancel;
    volatile bool m_IsRunning;
};

#endif // METADATACONSOLIDATINGWORKER_H
