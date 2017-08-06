#include "resultssendingworker.h"
#include <QByteArray>
#include <QDataStream>
#include <QLocalSocket>

ResultsSendingWorker::ResultsSendingWorker(ReaderWriterQueue<ConsolidationResult> *queueOut, QLocalSocket *socket, QObject *parent) :
    QObject(parent),
    m_Socket(socket),
    m_QueueOut(queueOut),
    m_Cancel(false)
{
    Q_ASSERT(socket != nullptr);
    Q_ASSERT(queueOut != nullptr);
}

void ResultsSendingWorker::process() {
    doWork();
    emit stopped();
}

void ResultsSendingWorker::cancel() {
    m_Cancel = true;
    m_QueueOut->pushThombstone();
}

void ResultsSendingWorker::doWork() {
    for (;;) {
        if (m_Cancel) { break; }

        std::shared_ptr<ConsolidationResult> item = m_QueueOut->blockingPop();
        if (!item) { continue; }

        sendResult(item);
    }
}

void ResultsSendingWorker::sendResult(const std::shared_ptr<ConsolidationResult> &result) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);
    out << (quint32)0;

    QHash<QString, QVariant> resultsHash;
    resultsHash["title"] = result->m_Title;
    resultsHash["description"] = result->m_Description;
    resultsHash["keywords"] = result->m_Keywords;
    resultsHash["filepath"] = result->m_Filepath;

    out << resultsHash;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    m_Socket->write(block);
    m_Socket->flush();
}
