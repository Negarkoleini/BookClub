#include "ClientHandlerThread.h"

ClientHandlerThread::ClientHandlerThread(qintptr descriptor, QObject *parent)
    : QThread(parent), m_socketDescriptor(descriptor) {
}

ClientHandlerThread::~ClientHandlerThread() {
}

void ClientHandlerThread::run() {
    m_worker = new ClientSocketWorker(m_socketDescriptor);
    connect(m_worker, &ClientSocketWorker::workerDisconnected,
            this, &ClientHandlerThread::onWorkerDisconnected, Qt::DirectConnection);
    connect(m_worker, &ClientSocketWorker::logRequired,
            this, &ClientHandlerThread::logRequired, Qt::DirectConnection);

    m_worker->initSocket();

    emit workerReady(m_worker);

    exec();

    delete m_worker;
    m_worker = nullptr;
}

void ClientHandlerThread::onWorkerDisconnected(ClientSocketWorker* /*worker*/) {
    quit(); // به exec() میگوید event loop