#ifndef SERVERDASHBOARDWINDOW_H
#define SERVERDASHBOARDWINDOW_H
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QTimer>
#include "serverCore.h"
#include "ServerLogManager.h"

class ServerDashboardWindow : public QWidget {
    Q_OBJECT
private:
    ServerCore* m_server;
    ServerLogManager m_logManager;

    QPushButton* btnToggleServer;
    QTextEdit* textEditLogs;
    QLabel* lblConnectionStatus;
    QLabel* lblActiveClients;
    QLabel* lblOnlineUsers;
    QLabel* lblServerHealth;
    QTimer* statusTimer;

    void buildUi();
    void updateHealthLabel(int connectedClients);

public:
    explicit ServerDashboardWindow(ServerCore* server, QWidget *parent = nullptr);
    ~ServerDashboardWindow() override = default;

private slots:
    void onStartStopClicked();
    void handleNewLog(const QString &text);
    void handleConnectionUpdate(int totalCount);
    void refreshOnlineUsersLabel();
};

#endif // SERVERDASHBOARDWINDOW_H
