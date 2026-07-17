#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QJsonObject>
#include "NetworkMessage.h"
#include "CommandType.h"

class ClientNetworkManager : public QObject {
    Q_OBJECT
private:
    ClientNetworkManager();
    QTcpSocket* socket;
    QByteArray incomingBuffer;

public:
    ClientNetworkManager(const ClientNetworkManager&) = delete;
    ClientNetworkManager& operator=(const ClientNetworkManager&) = delete;

    static ClientNetworkManager& getInstance();

    bool establishConnection(const QString &ip, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;


    void sendRequest(CommandType commandType, const QJsonObject &payload);

signals:
    void connectedToServer();
    void disconnectedFromServer();
    void connectionError(QString errorMessage);

    void serverReplyReceived(CommandType commandType, QJsonObject payload, bool ok);

    void pushNotificationArrived(QJsonObject payload);

private slots:
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);
};