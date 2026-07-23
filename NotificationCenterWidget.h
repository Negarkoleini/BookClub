#ifndef NOTIFICATIONCENTERWIDGET_H
#define NOTIFICATIONCENTERWIDGET_H
#pragma once
#include "AppNotification.h"
#include <QWidget>
#include <QListWidget>
#include <QVector>

class NotificationCenterWidget : public QWidget {
    Q_OBJECT
private:
    QListWidget* listNotifications;
    QVector<AppNotification> cachedNotifications;

    void refreshList();

public:
    explicit NotificationCenterWidget(QWidget *parent = nullptr);
    ~NotificationCenterWidget() override = default;

    void addNotification(const AppNotification &notif);   // یک اعلانِ تازه (push) به بالای لیست اضافه می‌شود
    void loadNotifications(const QVector<AppNotification> &notifs); // بارگذاریِ اولیه از سرور (GetNotifications)

private slots:
    void onNotificationClicked(QListWidgetItem* item);
};

#endif // NOTIFICATIONCENTERWIDGET_H
