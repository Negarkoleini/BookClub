#ifndef DASHBOARDCONTROLLER_H
#define DASHBOARDCONTROLLER_H
#pragma once
#include <QObject>
#include <QStackedWidget>
#include "LoginWindow.h"

class UserPanelWindow;
class PublisherPanelWindow;
class AdminPanelWindow;

class DashboardController : public QObject {
    Q_OBJECT
private:
    QStackedWidget* mainStackedWidget;
    LoginWindow* loginWindow;
    UserPanelWindow* userPanel = nullptr;
    PublisherPanelWindow* publisherPanel = nullptr;
    AdminPanelWindow* adminPanel = nullptr;

    int currentUserId = -1;
    QString currentRole;

public:
    explicit DashboardController(QStackedWidget* stack);
    ~DashboardController() override = default;

    void switchToLogin();
    void switchToUserDashboard(int userId);
    void switchToPublisherDashboard(int userId);
    void switchToAdminDashboard(int userId);

private slots:
    void onLoginSucceeded(int userId, QString role);
    void onLogoutRequested();
};

#endif // DASHBOARDCONTROLLER_H