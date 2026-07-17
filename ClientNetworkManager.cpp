#include "ClientNetworkManager.h"
#include "JsonPayload.h"
#include "JsonPayLoad.h"
#include <QJsonDocument>
#include <cstring>
#include <stdexcept>

ClientNetworkManager::ClientNetworkManager() : socket(new QTcpSocket(this)) {
    connect(socket, &QTcpSocket::readyRead, this, &ClientNetworkManager::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &ClientNetworkManager::connectedToServer);
    connect(socket, &QTcpSocket::disconnected, this, &ClientNetworkManager::disconnectedFromServer);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientNetworkManager::onSocketError);
}

ClientNetworkManager& ClientNetworkManager::getInstance() {
    static ClientNetworkManager instance;
    return instance;
}

bool ClientNetworkManager::establishConnection(const QString &ip, quint16 port) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }
    socket->connectToHost(ip, port);
    return socket->waitForConnected(5000); // برای سادگیِ فاز اول، اتصال را همزمان (blocking) منتظر می‌مانیم
}

void ClientNetworkManager::disconnectFromServer() {
    socket->disconnectFromHost();
}

bool ClientNetworkManager::isConnected() const {
    return socket->state() == QAbstractSocket::ConnectedState;
}

void ClientNetworkManager::sendRequest(CommandType commandType, const QJsonObject &payload) {
    if (!isConnected()) {
        emit connectionError("به سرور متصل نیستید.");
        return;
    }
    QByteArray json = JsonPayload::toQByteArray(payload);
    std::vector<char> payloadVec(json.begin(), json.end());
    NetworkMessage msg(commandType, payloadVec);
    std::vector<char> raw = msg.wrap();
    socket->write(raw.data(), static_cast<qint64>(raw.size()));
}

void ClientNetworkManager::onReadyRead() {
    incomingBuffer.append(socket->readAll());

    while (incomingBuffer.size() >= NetworkMessage::HEADER_SIZE) {
        int cmdTypeRaw = 0, length = 0;
        std::memcpy(&cmdTypeRaw, incomingBuffer.constData(), sizeof(int));
        std::memcpy(&length, incomingBuffer.constData() + sizeof(int), sizeof(int));

        if (length < 0) {
            incomingBuffer.clear();
            emit connectionError("پیامِ خراب از سرور دریافت شد.");
            return;
        }

        const int totalSize = NetworkMessage::HEADER_SIZE + length;
        if (incomingBuffer.size() < totalSize) {
            break;
        }

        QByteArray oneMessage = incomingBuffer.left(totalSize);
        incomingBuffer.remove(0, totalSize);

        std::vector<char> rawVec(oneMessage.begin(), oneMessage.end());
        try {
            NetworkMessage msg = NetworkMessage::unwrap(rawVec);
            QByteArray payloadBytes(msg.getPayload().data(), static_cast<int>(msg.getPayload().size()));
            QJsonObject obj = JsonPayload::fromBytes(payloadBytes);

            if (msg.getCommandType() == CommandType::Notification) {
                emit pushNotificationArrived(obj);
            } else {
                bool ok = obj.value("ok").toBool(false);
                emit serverReplyReceived(msg.getCommandType(), obj, ok);
            }
        } catch (const std::exception &ex) {
            emit connectionError(QString("خطا در پردازش پاسخ سرور: %1").arg(ex.what()));
        }
    }
}

void ClientNetworkManager::onSocketError(QAbstractSocket::SocketError /*error*/) {
    emit connectionError(socket->errorString());
}
