#ifndef ADMINPANELWINDOW_H
#define ADMINPANELWINDOW_H
#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QJsonObject>
#include "CommandType.h"



class AdminPanelWindow : public QMainWindow {
    Q_OBJECT
private:
    int currentAdminId;

    // ---- تب کاربران ----
    QLineEdit* txtUserSearch;
    QComboBox* comboRoleFilter;
    QComboBox* comboStatusFilter;
    QTableWidget* tableAllUsersReport;
    QPushButton* btnBanUser;
    QPushButton* btnUnbanUser;
    QPushButton* btnDisableTemporarily;
    QPushButton* btnChangeRole;
    QPushButton* btnDeleteUser;
    QPushButton* btnViewUserDetails;
    QJsonObject allUsersRaw; // برای فیلترِ محلی، پاسخِ خامِ GetAllUsers را نگه می‌داریم

    // ---- تب کتاب‌ها ----
    QTableWidget* tableAllBooks;
    QPushButton* btnToggleBookActive;
    QPushButton* btnDeleteBookAdmin;

    // ---- تب نظرات ----
    QLineEdit* txtCommentBookFilter;
    QLineEdit* txtCommentUserFilter;
    QPushButton* btnApplyCommentFilter;
    QTableWidget* tableAllComments;
    QPushButton* btnApproveComment;
    QPushButton* btnRejectComment;
    QPushButton* btnDeleteComment;

    // ---- تب تخفیف‌ها ----
    QTableWidget* tablePendingDiscounts;
    QPushButton* btnApproveDiscount;
    QPushButton* btnRejectDiscount;

    // ---- تب تنظیمات/محدودیت‌ها ----
    QSpinBox* spinMaxPurchasesPerDay;
    QSpinBox* spinMaxCommentsPerDay;
    QPushButton* btnSaveLimits;

    void buildUi();
    QWidget* buildUsersTab();
    QWidget* buildBooksTab();
    QWidget* buildCommentsTab();
    QWidget* buildDiscountsTab();
    QWidget* buildSettingsTab();

    void requestAllUsers();
    void requestAllBooks();
    void requestAllComments();
    void requestPendingDiscounts();
    void requestLimits();
    void applyUserFilters();

public:
    explicit AdminPanelWindow(int adminUserId, QWidget *parent = nullptr);
    ~AdminPanelWindow() override = default;

private slots:
    void fetchSystemReports();

    void executeBanAction();
    void executeUnbanAction();
    void executeDisableTemporarily();
    void executeChangeRole();
    void executeDeleteUser();
    void executeViewUserDetails();

    void executeToggleBookActive();
    void executeDeleteBookAdmin();

    void executeCommentApproval();
    void executeCommentRejection();
    void executeDeleteComment();

    void executeApproveDiscount();
    void executeRejectDiscount();

    void executeSaveLimits();

    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);
};
#endif // ADMINPANELWINDOW_H
