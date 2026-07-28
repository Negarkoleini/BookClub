#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <atomic>
#include "NetworkMessage.h"


class ClientSocketWorker : public QObject {
    Q_OBJECT
private:
    QTcpSocket* m_socket = nullptr;
    qintptr m_descriptor;
    QByteArray incomingBuffer;
    std::atomic<int> associatedUserId{-1};

public:
    explicit ClientSocketWorker(qintptr descriptor, QObject *parent = nullptr);
    ~ClientSocketWorker() override;

    void setAssociatedUserId(int userId);
    int getAssociatedUserId() const;

public slots:
    void initSocket();
    void readClientData();
    void sendResponse(CommandType type, const QByteArray &jsonPayload);
    void handleDisconnect();

signals:
    void dataReadyForProcessing(CommandType commandType, const QByteArray &payload, ClientSocketWorker* sender);
    void logRequired(QString log);
    void workerDisconnected(ClientSocketWorker* self);
};