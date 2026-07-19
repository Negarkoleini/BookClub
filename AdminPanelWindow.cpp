#include "AdminPanelWindow.h"
#include "ClientNetworkManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonArray>

AdminPanelWindow::AdminPanelWindow(int adminUserId, QWidget *parent)
    : QMainWindow(parent), currentAdminId(adminUserId) {
    buildUi();

    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::serverReplyReceived,
            this, &AdminPanelWindow::onNetworkReply);

    fetchSystemReports();
}

void AdminPanelWindow::buildUi() {
    setWindowTitle("BookClub - پنل مدیر سیستم");
    resize(950, 650);

    auto* central = new QWidget();
    auto* mainLayout = new QVBoxLayout(central);

    // ---- بخش کاربران ----
    auto* usersBox = new QGroupBox("مدیریتِ کاربران");
    auto* usersLayout = new QVBoxLayout(usersBox);
    tableAllUsersReport = new QTableWidget(0, 4);
    tableAllUsersReport->setHorizontalHeaderLabels({"نام کاربری", "نقش", "وضعیت", "تاریخِ ثبت‌نام"});
    tableAllUsersReport->horizontalHeader()->setStretchLastSection(true);// کش آمدن ستون آخر
    tableAllUsersReport->setSelectionBehavior(QAbstractItemView::SelectRows);//با کلیک بر خانه سطر انتخاب میشه
    tableAllUsersReport->setEditTriggers(QAbstractItemView::NoEditTriggers);//نمیتوان ادیت کرد

    auto* userButtons = new QHBoxLayout();
    btnBanUser = new QPushButton("مسدودسازی");
    btnUnbanUser = new QPushButton("رفعِ مسدودیت");
    btnDisableTemporarily = new QPushButton("غیرفعال‌سازیِ موقت");
    userButtons->addWidget(btnBanUser);
    userButtons->addWidget(btnUnbanUser);
    userButtons->addWidget(btnDisableTemporarily);

    usersLayout->addWidget(tableAllUsersReport);
    usersLayout->addLayout(userButtons);

    //---بخش نظرات در انتظار تایید---
    auto* commentsBox = new QGroupBox("نظراتِ در انتظارِ تأیید");
    auto* commentsLayout = new QVBoxLayout(commentsBox);
    tablePendingComments = new QTableWidget(0, 3);
    tablePendingComments->setHorizontalHeaderLabels({"کاربر", "متنِ نظر", "کتاب"});
    tablePendingComments->horizontalHeader()->setStretchLastSection(true);
    tablePendingComments->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablePendingComments->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto* commentButtons = new QHBoxLayout();
    btnApproveComment = new QPushButton("تأییدِ نظر");
    btnRejectComment = new QPushButton("ردِ نظر");
    commentButtons->addWidget(btnApproveComment);
    commentButtons->addWidget(btnRejectComment);

    commentsLayout->addWidget(tablePendingComments);
    commentsLayout->addLayout(commentButtons);

    btnRefreshAll = new QPushButton("🔄 رفرشِ کامل");

    mainLayout->addWidget(usersBox);
    mainLayout->addWidget(commentsBox);
    mainLayout->addWidget(btnRefreshAll);

    setCentralWidget(central);

    connect(btnBanUser, &QPushButton::clicked, this, &AdminPanelWindow::executeBanAction);
    connect(btnUnbanUser, &QPushButton::clicked, this, &AdminPanelWindow::executeUnbanAction);
    connect(btnDisableTemporarily, &QPushButton::clicked, this, &AdminPanelWindow::executeDisableTemporarily);
    connect(btnApproveComment, &QPushButton::clicked, this, &AdminPanelWindow::executeCommentApproval);
    connect(btnRejectComment, &QPushButton::clicked, this, &AdminPanelWindow::executeCommentRejection);
    connect(btnRefreshAll, &QPushButton::clicked, this, &AdminPanelWindow::fetchSystemReports);
}

void AdminPanelWindow::fetchSystemReports() {
    requestAllUsers();
    requestPendingComments();
}

void AdminPanelWindow::requestAllUsers() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetAllUsers, req);
}

void AdminPanelWindow::requestPendingComments() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetPendingComments, req);
}
void AdminPanelWindow::executeBanAction() {
    auto selected = tableAllUsersReport->selectedItems();
    if (selected.isEmpty()) return;
    int userId = tableAllUsersReport->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    if (QMessageBox::question(this, "تایید", "این کاربر مسدود شود؟") != QMessageBox::Yes) return;
    QJsonObject req;
    req["userId"] = userId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::BanUser, req);
}

void AdminPanelWindow::executeUnbanAction() {
    auto selected = tableAllUsersReport->selectedItems();
    if (selected.isEmpty()) return;
    int userId = tableAllUsersReport->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    QJsonObject req;
    req["userId"] = userId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::UnbanUser, req);
}

void AdminPanelWindow::executeDisableTemporarily() {
    auto selected = tableAllUsersReport->selectedItems();
    if (selected.isEmpty()) return;
    int userId = tableAllUsersReport->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    QJsonObject req;
    req["userId"] = userId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::DisableUserTemporarily, req);
}

void AdminPanelWindow::executeCommentApproval() {
    auto selected = tablePendingComments->selectedItems();
    if (selected.isEmpty()) return;
    int commentId = tablePendingComments->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    QJsonObject req;
    req["commentId"] = commentId;
    req["approve"] = true;
    ClientNetworkManager::getInstance().sendRequest(CommandType::ApproveComment, req);
}

void AdminPanelWindow::executeCommentRejection() {
    auto selected = tablePendingComments->selectedItems();
    if (selected.isEmpty()) return;
    int commentId = tablePendingComments->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    QJsonObject req;
    req["commentId"] = commentId;
    req["approve"] = false;
    ClientNetworkManager::getInstance().sendRequest(CommandType::ApproveComment, req);
}


void AdminPanelWindow::onNetworkReply(CommandType commandType, QJsonObject payload, bool ok) {
    if (!ok) {
        QMessageBox::warning(this, "خطا", payload.value("error").toString());
        return;
    }

    if (commandType == CommandType::GetAllUsers) {
        auto users = payload.value("users").toArray();
        tableAllUsersReport->setRowCount(users.size());
        for (int row = 0; row < users.size(); ++row) {
            QJsonObject u = users[row].toObject();
            auto* usernameItem = new QTableWidgetItem(u.value("username").toString());
            usernameItem->setData(Qt::UserRole, u.value("id").toInt());
            tableAllUsersReport->setItem(row, 0, usernameItem);
            tableAllUsersReport->setItem(row, 1, new QTableWidgetItem(u.value("role").toString()));

            int statusInt = u.value("status").toInt();
            QString statusText = (statusInt == 0) ? "فعال" : (statusInt == 1) ? "مسدود" : "غیرفعالِ موقت";
            tableAllUsersReport->setItem(row, 2, new QTableWidgetItem(statusText));
            tableAllUsersReport->setItem(row, 3, new QTableWidgetItem(u.value("registrationDate").toString()));
        }
        return;
    }

    if (commandType == CommandType::GetPendingComments) {
        auto comments = payload.value("comments").toArray();
        tablePendingComments->setRowCount(comments.size());
        for (int row = 0; row < comments.size(); ++row) {
            QJsonObject c = comments[row].toObject();
            auto* userItem = new QTableWidgetItem(c.value("username").toString());
            userItem->setData(Qt::UserRole, c.value("commentId").toInt());
            tablePendingComments->setItem(row, 0, userItem);
            tablePendingComments->setItem(row, 1, new QTableWidgetItem(c.value("text").toString()));
            tablePendingComments->setItem(row, 2, new QTableWidgetItem(QString::number(c.value("bookId").toInt())));
        }
        return;
    }

    if (commandType == CommandType::BanUser || commandType == CommandType::UnbanUser
        || commandType == CommandType::DisableUserTemporarily) {
        requestAllUsers();
        return;
    }

    if (commandType == CommandType::ApproveComment) {
        requestPendingComments();
        return;
    }
}


