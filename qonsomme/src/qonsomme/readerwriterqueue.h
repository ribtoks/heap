#ifndef READERWRITERQUEUE_H
#define READERWRITERQUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <vector>
#include <memory>
#include "defines.h"

// queue optimized for "many writers - 1 reader" case
template<typename T>
class ReaderWriterQueue
{
public:
    ReaderWriterQueue() {}

public:
    void push(const std::shared_ptr<T> &item) {
        bool wasEmpty = false;

        QMutexLocker writeLocker(&m_WriteMutex);
        Q_UNUSED(writeLocker);

        wasEmpty = m_WriteQueue.empty();

        m_WriteQueue.push_back(item);

        if (wasEmpty) {
            m_WaitAnyItem.wakeOne();
        }
    }

    void pushThombstone() {
        QMutexLocker writeLocker(&m_WriteMutex);
        Q_UNUSED(writeLocker);

        std::shared_ptr<T> emptyItem;
        m_WriteQueue.push_back(emptyItem);

        m_WaitAnyItem.wakeOne();
    }

    std::shared_ptr<T> blockingPop() {
        std::shared_ptr<T> result;

        {
            QMutexLocker readLocker(&m_ReadMutex);
            Q_UNUSED(readLocker);

            if (m_ReadQueue.empty()) {
                QMutexLocker writeLocker(&m_WriteMutex);
                Q_UNUSED(writeLocker);

                rebalanceUnsafe();

                while (m_ReadQueue.empty()) {
                    bool waitResult = m_WaitAnyItem.wait(&m_WriteMutex);
                    if (!waitResult) {
                        LOG_WARNING << "Waiting failed for new items";
                    }

                    rebalanceUnsafe();
                }
            }

            Q_ASSERT(!m_ReadQueue.empty());

            result = m_ReadQueue.back();
            m_ReadQueue.pop_back();
        }

        return result;
    }

    void rebalance() {
        QMutexLocker readLocker(&m_ReadMutex);
        Q_UNUSED(readLocker);

        {
            QMutexLocker writeLocker(&m_WriteMutex);
            Q_UNUSED(writeLocker);

            rebalanceUnsafe();
        }
    }

private:
    void rebalanceUnsafe() {
        Q_ASSERT(m_ReadQueue.empty());

        const size_t size = m_WriteQueue.size();
        if (size == 0) { return; }

        m_ReadQueue.reserve(size);

        // reverse in order to use pop_back() in read
        for (size_t i = size; i >= 1; i--) {
            m_ReadQueue.push_back(m_WriteQueue[i - 1]);
        }

        m_WriteQueue.clear();
    }

private:
    QMutex m_ReadMutex;
    QMutex m_WriteMutex;
    QWaitCondition m_WaitAnyItem;
    std::vector<std::shared_ptr<T> > m_ReadQueue;
    std::vector<std::shared_ptr<T> > m_WriteQueue;
};

#endif // READERWRITERQUEUE_H
