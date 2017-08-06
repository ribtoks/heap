#include "requestscoordinator.h"
#include <QLocalSocket>
#include <QDataStream>
#include <QThread>
#include "metadataconsolidatingworker.h"
#include "resultssendingworker.h"
#include "consolidaterequest.h"
#include "defines.h"

#define MIN_WORKING_THREADS 2

RequestsCoordinator::RequestsCoordinator(QLocalServer *server, QObject *parent) :
    QObject(parent),
    m_ResultsSendingWorker(nullptr),
    m_Server(server),
    m_BlockSize(0),
    m_LastWorkerIndex(0)
{
    Q_ASSERT(server != nullptr);
    QObject::connect(m_Server, &QLocalServer::newConnection, this, &RequestsCoordinator::handleConnection);
}

void RequestsCoordinator::startWorkers() {
    int threadsCount = qMax(QThread::idealThreadCount(), MIN_WORKING_THREADS);
    LOG_DEBUG << "Starting" << threadsCount << "thread(s)";
    int i = 0;

    while (threadsCount--) {
        MetadataConsolidatingWorker *worker = new MetadataConsolidatingWorker(&m_QueueOut);

        QThread *thread = new QThread();
        worker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, worker, &MetadataConsolidatingWorker::process);
        QObject::connect(worker, &MetadataConsolidatingWorker::stopped, thread, &QThread::quit);

        QObject::connect(worker, &MetadataConsolidatingWorker::stopped, worker, &MetadataConsolidatingWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(this, &RequestsCoordinator::stopRequest, worker, &MetadataConsolidatingWorker::cancel);

        thread->start();

        LOG_INFO << "Started worker" << i++;

        m_Workers.push_back(worker);
    }
}

void RequestsCoordinator::stopWorkers() {
    for (auto &worker: m_Workers) {
        worker->stopWorking();
    }
}

void RequestsCoordinator::startSendingResults(QLocalSocket *socket) {
    Q_ASSERT(m_ResultsSendingWorker == nullptr);

    m_ResultsSendingWorker = new ResultsSendingWorker(&m_QueueOut, socket);

    QThread *thread = new QThread();
    m_ResultsSendingWorker->moveToThread(thread);

    QObject::connect(thread, &QThread::started, m_ResultsSendingWorker, &ResultsSendingWorker::process);
    QObject::connect(m_ResultsSendingWorker, &ResultsSendingWorker::stopped, thread, &QThread::quit);

    QObject::connect(m_ResultsSendingWorker, &ResultsSendingWorker::stopped, m_ResultsSendingWorker, &ResultsSendingWorker::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    QObject::connect(this, &RequestsCoordinator::stopRequest, m_ResultsSendingWorker, &ResultsSendingWorker::cancel);

    thread->start();
}

void RequestsCoordinator::handleConnection() {
    QLocalSocket *clientConnection = m_Server->nextPendingConnection();

    QObject::connect(clientConnection, &QLocalSocket::disconnected,
                     clientConnection, &QLocalSocket::deleteLater);

    QObject::connect(clientConnection, &QLocalSocket::readyRead,
                     this, &RequestsCoordinator::handleReadyRead);

    startSendingResults(clientConnection);
}

void RequestsCoordinator::handleReadyRead() {
    QLocalSocket *socket = qobject_cast<QLocalSocket*>(sender());
    Q_ASSERT(socket != nullptr);
    if (socket == nullptr) { return; }

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_6);

    if (m_BlockSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint32)) { return; }
        in >> m_BlockSize;
    }

    if ((socket->bytesAvailable() < m_BlockSize) || in.atEnd()) { return; }
    QString nextFilepath;
    in >> nextFilepath;

    if (nextFilepath == "quit") {
        socket->flush();
        socket->disconnectFromServer();
        m_BlockSize = 0;
        emit stopRequest();
        LOG_INFO << "Stop requested";
        return;
    }

    std::shared_ptr<ConsolidateRequest> request(new ConsolidateRequest(nextFilepath));

    auto *worker = getNextWorker();
    worker->submitItem(request);
}

MetadataConsolidatingWorker *RequestsCoordinator::getNextWorker() {
    Q_ASSERT(!m_Workers.empty());

    auto *worker = m_Workers[m_LastWorkerIndex];
    int nextIndex = m_LastWorkerIndex + 1;
    m_LastWorkerIndex = nextIndex % m_Workers.size();
    return worker;
}
