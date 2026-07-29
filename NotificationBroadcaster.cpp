#include "NotificationBroadcaster.h"
#include "SessionManager.h"
#include "ClientSocketWorker.h"
#include "DatabaseManager.h"
#include "JsonPayLoad.h"
#include <QJsonObject>

NotificationBroadcaster::NotificationBroadcaster(QObject *parent) : QObject(parent) {
}

void NotificationBroadcaster::sendToUser(const AppNotification &notif) {
    // همیشه اول در دیتابیس ذخیره می‌شود (تا وقتی کاربر بعداً لاگین کرد هم ببیندش)
    int dbId = DatabaseManager::getInstance().saveNotification(notif);

    ClientSocketWorker* worker = SessionManager::getInstance().getWorkerForUser(notif.getTargetUserId());
    if (worker == nullptr) {
        emit logRequired(QString("[EVENT] اعلان برای کاربر %1 در دیتابیس ذخیره شد (کاربر آفلاین است)")
                              .arg(notif.getTargetUserId()));
        return;
    }

    emit logRequired(QString("[EVENT] اعلان به کاربر %1 ارسال شد: %2")
                          .arg(notif.getTargetUserId())
                          .arg(QString::fromStdString(notif.getMessage())));

    QJsonObject json;
    json["id"] = dbId; // <--- قرار دادن آی‌دی واقعی دیتابیس در پکت شبکه
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