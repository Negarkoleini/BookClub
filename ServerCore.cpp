#include "ServerCore.h"
#include "RequestProcessor.h"

ServerCore::ServerCore(RequestProcessor* processor, QObject *parent)
    : QTcpServer(parent), requestProcessor(processor) {
}

ServerCore::~ServerCore() {
    stop();
}

bool ServerCore::start(quint16 port) {
    if (isRunning) return true;
    if (!listen(QHostAddress::Any, port)) {
        emit logGenerated(QString("خطا در راه‌اندازی سرور روی پورت %1: %2").arg(port).arg(errorString()));
        return false;
    }
    listeningPort = port;
    isRunning = true;
    emit logGenerated(QString("سرور روی پورت %1 راه‌اندازی شد.").arg(port));
    return true;
}

void ServerCore::stop() {
    if (!isRunning) return;
    close();
    isRunning = false;
    emit logGenerated("سرور متوقف شد.");
}

bool ServerCore::getIsRunning() const { return isRunning; }
int ServerCore::getConnectedClientsCount() const { return connectedClientsCount; }

void ServerCore::incomingConnection(qintptr socketDescriptor) {
    auto* thread = new ClientHandlerThread(socketDescriptor, this);

    connect(thread, &ClientHandlerThread::logRequired, this, &ServerCore::logGenerated);

    connect(thread, &ClientHandlerThread::workerReady, this,
            [this](ClientSocketWorker* worker) {
                connect(worker, &ClientSocketWorker::dataReadyForProcessing,
                        requestProcessor, &RequestProcessor::handleRequest);
            });

    connect(thread, &QThread::finished, this, [this, thread]() {
        connectedClientsCount--;
        emit clientCountChanged(connectedClientsCount);
        thread->deleteLater();
    });

    connectedClientsCount++;
    emit clientCountChanged(connectedClientsCount);

    thread->start();
}