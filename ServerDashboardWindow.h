#ifndef SERVERDASHBOARDWINDOW_H
#define SERVERDASHBOARDWINDOW_H
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QTimer>
#include <QProgressBar>
#include "ServerCore.h"
#include "ServerLogManager.h"
#include "SystemResourceMonitor.h"

class ServerDashboardWindow : public QWidget {
    Q_OBJECT
private:
    ServerCore* m_server;
    ServerLogManager m_logManager;
    SystemResourceMonitor m_resourceMonitor;
    quint16 m_configuredPort = 0;

    // نوار بالا: کنترل و وضعیتِ کلی
    QPushButton* btnToggleServer;
    QLabel* lblConnectionStatus;
    QLabel* lblHealthBadge;

    // کارت‌های آماری
    QLabel* lblOnlineUsersValue;
    QLabel* lblActiveClientsValue;
    QProgressBar* cpuBar;
    QProgressBar* ramBar;

    // پنل‌های لاگ
    QTextEdit* textEditRequestsLog;
    QTextEdit* textEditEventsLog;

    QTimer* statusTimer;

    void buildUi();
    QWidget* buildStatCard(const QString &title, QLabel* valueLabel);
    void updateHealthBadge(int connectedClients);
    void appendToRequestsLog(const QString &text);
    void appendToEventsLog(const QString &text);

public:
    explicit ServerDashboardWindow(ServerCore* server, QWidget *parent = nullptr);
    ~ServerDashboardWindow() override = default;

private slots:
    void onStartStopClicked();
    void handleNewLog(const QString &text);
    void handleConnectionUpdate(int totalCount);
    void refreshRealtimeStats();
};

#endif // SERVERDASHBOARDWINDOW_H
