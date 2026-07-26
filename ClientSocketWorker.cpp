#include "ClientSocketWorker.h"
#include "SessionManager.h"
#include <cstring>
#include <stdexcept>

ClientSocketWorker::ClientSocketWorker(qintptr descriptor, QObject *parent)
    : QObject(parent), m_descriptor(descriptor) {
}

ClientSocketWorker::~ClientSocketWorker() {
    if (m_socket) {
        m_socket->deleteLater();
    }
}

void ClientSocketWorker::setAssociatedUserId(int userId) { associatedUserId = userId; }
int ClientSocketWorker::getAssociatedUserId() const { return associatedUserId; }

void ClientSocketWorker::initSocket() {

    m_socket = new QTcpSocket(this);
    if (!m_socket->setSocketDescriptor(m_descriptor)) {
        emit logRequired(QString("خطا در اتصال به سوکت: %1").arg(m_socket->errorString()));
        emit workerDisconnected(this);
        return;
    }

    connect(m_socket, &QTcpSocket::readyRead, this, &ClientSocketWorker::readClientData);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientSocketWorker::handleDisconnect);

    emit logRequired(QString("کلاینت جدید متصل شد (descriptor=%1)").arg(m_descriptor));
}

void ClientSocketWorker::readClientData() {
    incomingBuffer.append(m_socket->readAll());

    while (incomingBuffer.size() >= NetworkMessage::HEADER_SIZE) {
        int cmdTypeRaw = 0, length = 0;
        std::memcpy(&cmdTypeRaw, incomingBuffer.constData(), sizeof(int));
        std::memcpy(&length, incomingBuffer.constData() + sizeof(int), sizeof(int));
        //آدرس خونه اولو نشون میده constData()

        if (length < 0) {
            emit logRequired("پیامِ خراب دریافت شد (طول منفی)؛ اتصال قطع می‌شود.");
            m_socket->disconnectFromHost();
            return;
        }

        const int totalMessageSize = NetworkMessage::HEADER_SIZE + length;
        if (incomingBuffer.size() < totalMessageSize) {
            break;
            //  پیام هنوز کامل نرسیده؛ صبر میکنیم تا کامل شودreadyRead بعدی
        }

        QByteArray oneMessage = incomingBuffer.left(totalMessageSize);
        incomingBuffer.remove(0, totalMessageSize);

        std::vector<char> rawVec(oneMessage.begin(), oneMessage.end());
        try {
            NetworkMessage msg = NetworkMessage::unwrap(rawVec);
            QByteArray payload(msg.getPayload().data(), static_cast<int>(msg.getPayload().size()));
            emit dataReadyForProcessing(msg.getCommandType(), payload, this);
        } catch (const std::exception &ex) {
            emit logRequired(QString("خطا در unwrap پیام: %1").arg(ex.what()));
        }
    }
}

void ClientSocketWorker::sendResponse(CommandType type, const QByteArray &jsonPayload) {
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        return;
    }
    std::vector<char> payloadVec(jsonPayload.begin(), jsonPayload.end());
    NetworkMessage msg(type, payloadVec);
    std::vector<char> raw = msg.wrap();
    m_socket->write(raw.data(), static_cast<qint64>(raw.size()));
}

void ClientSocketWorker::handleDisconnect() {
    if (associatedUserId != -1) {
        SessionManager::getInstance().removeSession(this);
    }
    emit logRequired(QString("اتصال قطع شد (userId=%1)")
                         .arg(associatedUserId.load()));
    emit workerDisconnected(this);
}
