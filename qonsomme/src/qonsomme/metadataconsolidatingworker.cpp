#include "metadataconsolidatingworker.h"
#include "exiv2consolidator.h"
#include "defines.h"

MetadataConsolidatingWorker::MetadataConsolidatingWorker(ReaderWriterQueue<ConsolidationResult> *queueOut, QObject *parent) :
    QObject(parent),
    m_ResultsQueue(queueOut),
    m_Cancel(false),
    m_IsRunning(false)
{
    Q_ASSERT(queueOut != nullptr);
}

void MetadataConsolidatingWorker::submitItem(const std::shared_ptr<ConsolidateRequest> &item) {
    if (m_Cancel) {
        return;
    }

    m_QueueMutex.lock();
    {
        bool wasEmpty = m_Queue.empty();
        m_Queue.push_back(item);

        if (wasEmpty) {
            m_WaitAnyItem.wakeOne();
        }
    }
    m_QueueMutex.unlock();
}

void MetadataConsolidatingWorker::submitItems(const std::vector<std::shared_ptr<ConsolidateRequest> > &items) {
    if (m_Cancel) {
        return;
    }

    m_QueueMutex.lock();
    {
        bool wasEmpty = m_Queue.empty();

        size_t size = items.size();
        for (size_t i = 0; i < size; ++i) {
            auto &item = items.at(i);
            m_Queue.push_back(item);
        }

        if (wasEmpty) {
            m_WaitAnyItem.wakeOne();
        }
    }
    m_QueueMutex.unlock();
}

void MetadataConsolidatingWorker::cancelCurrentBatch() {
    m_QueueMutex.lock();
    {
        m_Queue.clear();
    }
    m_QueueMutex.unlock();

    onQueueIsEmpty();
}

void MetadataConsolidatingWorker::doWork() {
    if (initWorker()) {
        m_IsRunning = true;
        runWorkerLoop();
        m_IsRunning = false;
    } else {
        m_Cancel = true;
    }

    workerStopped();
}

void MetadataConsolidatingWorker::stopWorking(bool immediately) {
    m_Cancel = true;

    m_QueueMutex.lock();
    {
        if (immediately) {
            m_Queue.clear();
        }

        m_Queue.emplace_back(std::shared_ptr<ConsolidateRequest>());
        m_WaitAnyItem.wakeOne();
    }
    m_QueueMutex.unlock();
}

bool MetadataConsolidatingWorker::initWorker() {
    return true;
}

void MetadataConsolidatingWorker::processOneItem(std::shared_ptr<ConsolidateRequest> &item) {
    Exiv2Consolidator consolidator(item);
    auto result = consolidator.consolidate();
    if (result) {
        m_ResultsQueue->push(result);
    }
}

void MetadataConsolidatingWorker::runWorkerLoop() {
    for (;;) {
        if (m_Cancel) {
            LOG_INFO << "Cancelled. Exiting...";
            break;
        }

        bool noMoreItems = false;

        m_QueueMutex.lock();

        while (m_Queue.empty()) {
            bool waitResult = m_WaitAnyItem.wait(&m_QueueMutex);
            if (!waitResult) {
                LOG_WARNING << "Waiting failed for new items";
            }
        }

        std::shared_ptr<ConsolidateRequest> item = m_Queue.front();
        m_Queue.pop_front();

        noMoreItems = m_Queue.empty();

        m_QueueMutex.unlock();

        if (item.get() == nullptr) { break; }

        try {
            processOneItem(item);
        }
        catch (...) {
            LOG_WARNING << "Exception while processing item!";
        }

        if (noMoreItems) {
            onQueueIsEmpty();
        }
    }
}
