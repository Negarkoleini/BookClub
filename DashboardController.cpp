#include "DashboardController.h"
#include "UserPanelWindow.h"
#include "PublisherPanelWindow.h"
#include "AdminPanelWindow.h"

DashboardController::DashboardController(QStackedWidget* stack)
    : mainStackedWidget(stack) {
    loginWindow = new LoginWindow();
    mainStackedWidget->addWidget(loginWindow); // ایندکس 0

    connect(loginWindow, &LoginWindow::loginSucceeded, this, &DashboardController::onLoginSucceeded);

    switchToLogin();
}

void DashboardController::onLoginSucceeded(int userId, QString role) {
    currentUserId = userId;
    currentRole = role;

    if (role == "RegularUser") {
        switchToUserDashboard(userId);
    } else if (role == "Publisher") {
        switchToPublisherDashboard(userId);
    } else if (role == "Admin") {
        switchToAdminDashboard(userId);
    }
}

void DashboardController::switchToLogin() {
    mainStackedWidget->setCurrentWidget(loginWindow);
}

void DashboardController::switchToUserDashboard(int userId) {
    if (!userPanel) {
        userPanel = new UserPanelWindow(userId);
        mainStackedWidget->addWidget(userPanel);
    }
    mainStackedWidget->setCurrentWidget(userPanel);
}

void DashboardController::switchToPublisherDashboard(int userId) {
    if (!publisherPanel) {
        publisherPanel = new PublisherPanelWindow(userId);
        mainStackedWidget->addWidget(publisherPanel);
    }
    mainStackedWidget->setCurrentWidget(publisherPanel);
}

void DashboardController::switchToAdminDashboard(int userId) {
    if (!adminPanel) {
        adminPanel = new AdminPanelWindow(userId);
        mainStackedWidget->addWidget(adminPanel);
    }
    mainStackedWidget->setCurrentWidget(adminPanel);
}
