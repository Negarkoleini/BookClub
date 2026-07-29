#pragma once
#include <QObject>
#include "AppNotification.h"

class NotificationBroadcaster : public QObject {
    Q_OBJECT
public:
    explicit NotificationBroadcaster(QObject *parent = nullptr);
    ~NotificationBroadcaster() override = default;

    void sendToUser(const AppNotification &notif);

    // برای اعلان‌های همگانی (مثلاً اطلاع‌رسانی سیستمی)
    void broadcastToAll(const AppNotification &notif, const QList<int> &allUserIds);

signals:
    void logRequired(QString log);
};
