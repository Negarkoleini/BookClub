#ifndef ADMINPANELWINDOW_H
#define ADMINPANELWINDOW_H
#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QJsonObject>
#include "CommandType.h"
#include "InAppNotificationWidget.h"


class AdminPanelWindow : public QMainWindow {
    Q_OBJECT
private:
    QTableWidget* tableAllUsersReport;
    QTableWidget* tablePendingComments;

    QPushButton* btnBanUser;
    QPushButton* btnUnbanUser;
    QPushButton* btnDisableTemporarily;
    QPushButton* btnApproveComment;
    QPushButton* btnRejectComment;
    QPushButton* btnRefreshAll;

    void buildUi();
    void requestAllUsers();
    void requestPendingComments();

public:
    explicit AdminPanelWindow(int adminUserId, QWidget *parent = nullptr);
    ~AdminPanelWindow() override = default;

private slots:
    void fetchSystemReports();
    void executeBanAction();
    void executeUnbanAction();
    void executeDisableTemporarily();
    void executeCommentApproval();
    void executeCommentRejection();
    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);

private:
    int currentAdminId;
};
#endif // ADMINPANELWINDOW_H
