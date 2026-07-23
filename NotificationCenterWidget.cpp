#include "NotificationCenterWidget.h"
#include "ClientNetworkManager.h"
#include <QVBoxLayout>
#include <QJsonObject>
#include <QFont>

NotificationCenterWidget::NotificationCenterWidget(QWidget *parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    listNotifications = new QListWidget();
    layout->addWidget(listNotifications);

    connect(listNotifications, &QListWidget::itemClicked, this, &NotificationCenterWidget::onNotificationClicked);
}

void NotificationCenterWidget::addNotification(const AppNotification &notif) {
    cachedNotifications.push_front(notif); // جدیدترین بالای لیست
    refreshList();
}

void NotificationCenterWidget::loadNotifications(const QVector<AppNotification> &notifs) {
    cachedNotifications = notifs; // سرور از قبل به‌ترتیبِ جدید-به-قدیم برمی‌گرداند
    refreshList();
}

void NotificationCenterWidget::refreshList() {
    listNotifications->clear();
    for (int i = 0; i < cachedNotifications.size(); ++i) {
        const AppNotification &n = cachedNotifications[i];
        QString text = QString::fromStdString(n.getTimestamp()) + "  —  " + QString::fromStdString(n.getMessage());
        auto* item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, n.getId());
        if (!n.getIsRead()) {
            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
        }
        listNotifications->addItem(item);
    }
}

void NotificationCenterWidget::onNotificationClicked(QListWidgetItem* item) {
    int notifId = item->data(Qt::UserRole).toInt();

    for (auto &n : cachedNotifications) {
        if (n.getId() == notifId && !n.getIsRead()) {
            n.markAsRead();
        }
    }
    refreshList();

    QJsonObject req;
    req["notificationId"] = notifId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::MarkNotificationRead, req);
}
