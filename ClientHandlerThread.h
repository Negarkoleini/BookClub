#pragma once
#include <QThread>
#include "ClientSocketWorker.h"

class ClientHandlerThread : public QThread {
    Q_OBJECT
private:
    qintptr m_socketDescriptor;
    ClientSocketWorker* m_worker = nullptr;

public:
    explicit ClientHandlerThread(qintptr descriptor, QObject *parent = nullptr);
    ~ClientHandlerThread() override;

protected:
    void run() override;

private slots:
    void onWorkerDisconnected(ClientSocketWorker* worker);

signals:
    void workerReady(ClientSocketWorker* worker);
    void logRequired(QString log);
};
