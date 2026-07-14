#include "NotificationBroadcaster.h"
#include "SessionManager.h"
#include "ClientSocketWorker.h"
#include "DatabaseManager.h"
#include "JsonPayload.h"
#include <QJsonObject>

NotificationBroadcaster::NotificationBroadcaster(QObject *parent) : QObject(parent) {
}

void NotificationBroadcaster::sendToUser(const AppNotification &notif) {
    // همیشه اول در دیتابیس ذخیره می‌شود (تا وقتی کاربر بعداً لاگین کرد هم ببیندش)
    DatabaseManager::getInstance().saveNotification(notif);

    ClientSocketWorker* worker = SessionManager::getInstance().getWorkerForUser(notif.getTargetUserId());
    if (worker == nullptr) {
        return; // کاربر آفلاین است
    }

    QJsonObject json;
    json["id"] = notif.getId();
    json["type"] = static_cast<int>(notif.getType());
    json["message"] = QString::fromStdString(notif.getMessage());
    json["timestamp"] = QString::fromStdString(notif.getTimestamp());
    json["isRead"] = notif.getIsRead();


    QMetaObject::invokeMethod(worker, "sendResponse", Qt::QueuedConnection,
                              Q_ARG(CommandType, CommandType::Notification),
                              Q_ARG(QByteArray, JsonPayload::toQByteArray(json)));
}

void NotificationBroadcaster::broadcastToAll(const AppNotification &notif, const QList<int> &allUserIds) {
    for (int uid : allUserIds) {
        AppNotification copy = AppNotification::fromStorage(
            notif.getId(), notif.getType(), notif.getMessage(), uid,
            notif.getIsRead(), notif.getTimestamp());
        sendToUser(copy);
    }
}