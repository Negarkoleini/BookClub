#include "AdminPanelWindow.h"
#include "ClientNetworkManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QJsonArray>
#include <QLabel>
#include <QDialog>

AdminPanelWindow::AdminPanelWindow(int adminUserId, QWidget *parent)
    : QMainWindow(parent), currentAdminId(adminUserId) {
    buildUi();

    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::serverReplyReceived,
            this, &AdminPanelWindow::onNetworkReply);

    fetchSystemReports();
}

// =========================================================================
// ساختِ رابط کاربری
// =========================================================================
void AdminPanelWindow::buildUi() {
    setWindowTitle("BookClub - پنل مدیر سیستم");
    resize(1050, 700);

    auto* tabs = new QTabWidget();
    tabs->addTab(buildUsersTab(), "کاربران");
    tabs->addTab(buildBooksTab(), "کتاب ها");
    tabs->addTab(buildCommentsTab(), "نظرات");
    tabs->addTab(buildDiscountsTab(), "تخفیف ها");
    tabs->addTab(buildSettingsTab(), "محدودیت های سیستمی");

    setCentralWidget(tabs);
}

QWidget* AdminPanelWindow::buildUsersTab() {
    auto* tab = new QWidget();
    auto* layout = new QVBoxLayout(tab);

    // ---- نوارِ جستجو/فیلتر ----
    auto* filterBar = new QHBoxLayout();
    txtUserSearch = new QLineEdit();
    txtUserSearch->setPlaceholderText("جستجو بر اساسِ نامِ کاربری...");
    comboRoleFilter = new QComboBox();
    comboRoleFilter->addItem("همه‌ی نقش‌ها", "");
    comboRoleFilter->addItem("کاربرِ عادی", "RegularUser");
    comboRoleFilter->addItem("ناشر", "Publisher");
    comboRoleFilter->addItem("مدیر", "Admin");
    comboStatusFilter = new QComboBox();
    comboStatusFilter->addItem("همه‌ی وضعیت‌ها", -1);
    comboStatusFilter->addItem("فعال", 0);
    comboStatusFilter->addItem("مسدود", 1);
    comboStatusFilter->addItem("غیرفعالِ موقت", 2);

    filterBar->addWidget(txtUserSearch, /*stretch=*/1);
    filterBar->addWidget(comboRoleFilter);
    filterBar->addWidget(comboStatusFilter);
    layout->addLayout(filterBar);

    tableAllUsersReport = new QTableWidget(0, 4);
    tableAllUsersReport->setHorizontalHeaderLabels({"نام کاربری", "نقش", "وضعیت", "تاریخِ ثبت‌نام"});
    tableAllUsersReport->horizontalHeader()->setStretchLastSection(true);
    tableAllUsersReport->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableAllUsersReport->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(tableAllUsersReport);

    auto* buttonBar = new QHBoxLayout();
    btnBanUser = new QPushButton("مسدودسازی");
    btnUnbanUser = new QPushButton("رفعِ مسدودیت");
    btnDisableTemporarily = new QPushButton("غیرفعال‌سازیِ موقت");
    btnChangeRole = new QPushButton("تغییرِ نقش");
    btnDeleteUser = new QPushButton("حذفِ حساب");
    btnViewUserDetails = new QPushButton("مشاهده‌ی جزئیات");
    buttonBar->addWidget(btnBanUser);
    buttonBar->addWidget(btnUnbanUser);
    buttonBar->addWidget(btnDisableTemporarily);
    buttonBar->addWidget(btnChangeRole);
    buttonBar->addWidget(btnDeleteUser);
    buttonBar->addWidget(btnViewUserDetails);
    layout->addLayout(buttonBar);

    connect(txtUserSearch, &QLineEdit::textChanged, this, &AdminPanelWindow::applyUserFilters);
    connect(comboRoleFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminPanelWindow::applyUserFilters);
    connect(comboStatusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminPanelWindow::applyUserFilters);
    connect(btnBanUser, &QPushButton::clicked, this, &AdminPanelWindow::executeBanAction);
    connect(btnUnbanUser, &QPushButton::clicked, this, &AdminPanelWindow::executeUnbanAction);
    connect(btnDisableTemporarily, &QPushButton::clicked, this, &AdminPanelWindow::executeDisableTemporarily);
    connect(btnChangeRole, &QPushButton::clicked, this, &AdminPanelWindow::executeChangeRole);
    connect(btnDeleteUser, &QPushButton::clicked, this, &AdminPanelWindow::executeDeleteUser);
    connect(btnViewUserDetails, &QPushButton::clicked, this, &AdminPanelWindow::executeViewUserDetails);

    return tab;
}

QWidget* AdminPanelWindow::buildBooksTab() {
    auto* tab = new QWidget();
    auto* layout = new QVBoxLayout(tab);

    tableAllBooks = new QTableWidget(0, 6);
    tableAllBooks->setHorizontalHeaderLabels({"عنوان", "نویسنده", "ناشر", "قیمت", "امتیاز", "وضعیت"});
    tableAllBooks->horizontalHeader()->setStretchLastSection(true);
    tableAllBooks->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableAllBooks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(tableAllBooks);

    auto* buttonBar = new QHBoxLayout();
    btnToggleBookActive = new QPushButton("تغییرِ وضعیتِ فعال/غیرفعال");
    btnDeleteBookAdmin = new QPushButton("حذفِ کتاب (نامعتبر/مغایرِ قوانین)");
    buttonBar->addWidget(btnToggleBookActive);
    buttonBar->addWidget(btnDeleteBookAdmin);
    layout->addLayout(buttonBar);

    connect(btnToggleBookActive, &QPushButton::clicked, this, &AdminPanelWindow::executeToggleBookActive);
    connect(btnDeleteBookAdmin, &QPushButton::clicked, this, &AdminPanelWindow::executeDeleteBookAdmin);

    return tab;
}

QWidget* AdminPanelWindow::buildCommentsTab() {
    auto* tab = new QWidget();
    auto* layout = new QVBoxLayout(tab);

    auto* filterBar = new QHBoxLayout();
    txtCommentBookFilter = new QLineEdit();
    txtCommentBookFilter->setPlaceholderText("فیلتر بر اساسِ شناسه‌ی کتاب (خالی = همه)");
    txtCommentUserFilter = new QLineEdit();
    txtCommentUserFilter->setPlaceholderText("فیلتر بر اساسِ شناسه‌ی کاربر (خالی = همه)");
    btnApplyCommentFilter = new QPushButton("اعمالِ فیلتر");
    filterBar->addWidget(txtCommentBookFilter);
    filterBar->addWidget(txtCommentUserFilter);
    filterBar->addWidget(btnApplyCommentFilter);
    layout->addLayout(filterBar);

    tableAllComments = new QTableWidget(0, 5);
    tableAllComments->setHorizontalHeaderLabels({"کاربر", "کتاب", "متنِ نظر", "تاریخ", "وضعیتِ تأیید"});
    tableAllComments->horizontalHeader()->setStretchLastSection(true);
    tableAllComments->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableAllComments->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(tableAllComments);

    auto* buttonBar = new QHBoxLayout();
    btnApproveComment = new QPushButton("تأییدِ نظر");
    btnRejectComment = new QPushButton("ردِ نظر");
    btnDeleteComment = new QPushButton("حذفِ کاملِ نظر");
    buttonBar->addWidget(btnApproveComment);
    buttonBar->addWidget(btnRejectComment);
    buttonBar->addWidget(btnDeleteComment);
    layout->addLayout(buttonBar);

    connect(btnApplyCommentFilter, &QPushButton::clicked, this, &AdminPanelWindow::requestAllComments);
    connect(btnApproveComment, &QPushButton::clicked, this, &AdminPanelWindow::executeCommentApproval);
    connect(btnRejectComment, &QPushButton::clicked, this, &AdminPanelWindow::executeCommentRejection);
    connect(btnDeleteComment, &QPushButton::clicked, this, &AdminPanelWindow::executeDeleteComment);

    return tab;
}

QWidget* AdminPanelWindow::buildDiscountsTab() {
    auto* tab = new QWidget();
    auto* layout = new QVBoxLayout(tab);

    auto* info = new QLabel("تخفیف‌هایی که ناشران ثبت کرده‌اند و هنوز تأیید نشده‌اند "
                            "(، تخفیف تا وقتی تأیید نشود روی قیمتِ نهایی اثر نمی‌گذارد):");
    info->setWordWrap(true);
    layout->addWidget(info);

    tablePendingDiscounts = new QTableWidget(0, 5);
    tablePendingDiscounts->setHorizontalHeaderLabels({"کتاب", "نوعِ تخفیف", "مقدار", "شروع", "پایان"});
    tablePendingDiscounts->horizontalHeader()->setStretchLastSection(true);
    tablePendingDiscounts->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablePendingDiscounts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(tablePendingDiscounts);

    auto* buttonBar = new QHBoxLayout();
    btnApproveDiscount = new QPushButton("تأییدِ تخفیف");
    btnRejectDiscount = new QPushButton("ردِ تخفیف");
    buttonBar->addWidget(btnApproveDiscount);
    buttonBar->addWidget(btnRejectDiscount);
    layout->addLayout(buttonBar);

    connect(btnApproveDiscount, &QPushButton::clicked, this, &AdminPanelWindow::executeApproveDiscount);
    connect(btnRejectDiscount, &QPushButton::clicked, this, &AdminPanelWindow::executeRejectDiscount);

    return tab;
}

QWidget* AdminPanelWindow::buildSettingsTab() {
    auto* tab = new QWidget();
    auto* form = new QFormLayout(tab);

    auto* info = new QLabel("محدودیتِ تعدادِ خرید و نظرِ هر کاربر در طولِ یک روز :");
    info->setWordWrap(true);
    form->addRow(info);

    spinMaxPurchasesPerDay = new QSpinBox();
    spinMaxPurchasesPerDay->setRange(1, 100000);
    spinMaxCommentsPerDay = new QSpinBox();
    spinMaxCommentsPerDay->setRange(1, 100000);
    btnSaveLimits = new QPushButton("ذخیره‌ی تنظیمات");

    form->addRow("حداکثرِ خرید در روز:", spinMaxPurchasesPerDay);
    form->addRow("حداکثرِ نظر در روز:", spinMaxCommentsPerDay);
    form->addRow(btnSaveLimits);

    connect(btnSaveLimits, &QPushButton::clicked, this, &AdminPanelWindow::executeSaveLimits);

    return tab;
}

// =========================================================================
// درخواست‌های شبکه
// =========================================================================
void AdminPanelWindow::fetchSystemReports() {
    requestAllUsers();
    requestAllBooks();
    requestAllComments();
    requestPendingDiscounts();
    requestLimits();
}

void AdminPanelWindow::requestAllUsers() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetAllUsers, req);
}

void AdminPanelWindow::requestAllBooks() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetAllBooksAdmin, req);
}

void AdminPanelWindow::requestAllComments() {
    QJsonObject req;
    QString bookText = txtCommentBookFilter->text().trimmed();
    QString userText = txtCommentUserFilter->text().trimmed();
    if (!bookText.isEmpty()) req["bookId"] = bookText.toInt();
    if (!userText.isEmpty()) req["userId"] = userText.toInt();
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetAllComments, req);
}

void AdminPanelWindow::requestPendingDiscounts() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetPendingDiscounts, req);
}

void AdminPanelWindow::requestLimits() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetLimits, req);
}

// =========================================================================
// فیلترِ محلی کاربران (روی همان دیتایی که از GetAllUsers گرفتیم)
// =========================================================================
void AdminPanelWindow::applyUserFilters() {
    QString searchText = txtUserSearch->text().trimmed().toLower();
    QString roleFilter = comboRoleFilter->currentData().toString();
    int statusFilter = comboStatusFilter->currentData().toInt();

    auto users = allUsersRaw.value("users").toArray();
    QJsonArray filtered;
    for (const auto &v : users) {
        QJsonObject u = v.toObject();
        if (!searchText.isEmpty() && !u.value("username").toString().toLower().contains(searchText)) continue;
        if (!roleFilter.isEmpty() && u.value("role").toString() != roleFilter) continue;
        if (statusFilter != -1 && u.value("status").toInt() != statusFilter) continue;
        filtered.append(u);
    }

    tableAllUsersReport->setRowCount(filtered.size());
    for (int row = 0; row < filtered.size(); ++row) {
        QJsonObject u = filtered[row].toObject();
        auto* usernameItem = new QTableWidgetItem(u.value("username").toString());
        usernameItem->setData(Qt::UserRole, u.value("id").toInt());
        tableAllUsersReport->setItem(row, 0, usernameItem);
        tableAllUsersReport->setItem(row, 1, new QTableWidgetItem(u.value("role").toString()));

        int statusInt = u.value("status").toInt();
        QString statusText = (statusInt == 0) ? "فعال" : (statusInt == 1) ? "مسدود" : "غیرفعالِ موقت";
        tableAllUsersReport->setItem(row, 2, new QTableWidgetItem(statusText));
        tableAllUsersReport->setItem(row, 3, new QTableWidgetItem(u.value("registrationDate").toString()));
    }
}

// =========================================================================
// اکشن‌های تبِ کاربران
// =========================================================================
static int selectedUserId(QTableWidget* table) {
    auto selected = table->selectedItems();
    if (selected.isEmpty()) return -1;
    return table->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();
}

void AdminPanelWindow::executeBanAction() {
    int userId = selectedUserId(tableAllUsersReport);
    if (userId < 0) return;
    if (QMessageBox::question(this, "تایید", "این کاربر مسدود شود؟") != QMessageBox::Yes) return;
    QJsonObject req;
    req["userId"] = userId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::BanUser, req);
}

void AdminPanelWindow::executeUnbanAction() {
    int userId = selectedUserId(tableAllUsersReport);
    if (userId < 0) return;
    QJsonObject req;
    req["userId"] = userId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::UnbanUser, req);
}

void AdminPanelWindow::executeDisableTemporarily() {
    int userId = selectedUserId(tableAllUsersReport);
    if (userId < 0) return;
    QJsonObject req;
    req["userId"] = userId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::DisableUserTemporarily, req);
}

void AdminPanelWindow::executeChangeRole() {
    int userId = selectedUserId(tableAllUsersReport);
    if (userId < 0) return;

    QStringList roles = {"RegularUser", "Publisher", "Admin"};
    bool okPressed = false;
    QString newRole = QInputDialog::getItem(this, "تغییرِ نقش", "نقشِ جدید را انتخاب کنید:",
                                            roles, 0, false, &okPressed);
    if (!okPressed) return;

    QJsonObject req;
    req["userId"] = userId;
    req["newRole"] = newRole;
    ClientNetworkManager::getInstance().sendRequest(CommandType::ChangeUserRole, req);
}

void AdminPanelWindow::executeDeleteUser() {
    int userId = selectedUserId(tableAllUsersReport);
    if (userId < 0) return;
    if (QMessageBox::warning(this, "تاییدِ حذفِ کامل",
                             "این حسابِ کاربری و همه‌ی اطلاعاتِ مرتبط با آن حذف می‌شود. این عملیات قابلِ بازگشت نیست. ادامه می‌دهید؟",
                             QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    QJsonObject req;
    req["userId"] = userId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::DeleteUserAccount, req);
}

void AdminPanelWindow::executeViewUserDetails() {
    int userId = selectedUserId(tableAllUsersReport);
    if (userId < 0) return;
    QJsonObject req;
    req["userId"] = userId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetUserDetails, req);
}

// =========================================================================
// اکشن‌های تبِ کتاب‌ها
// =========================================================================
void AdminPanelWindow::executeToggleBookActive() {
    auto selected = tableAllBooks->selectedItems();
    if (selected.isEmpty()) return;
    int row = selected.first()->row();
    int bookId = tableAllBooks->item(row, 0)->data(Qt::UserRole).toInt();
    bool currentlyActive = (tableAllBooks->item(row, 5)->text() == "فعال");

    QJsonObject req;
    req["bookId"] = bookId;
    req["isActive"] = !currentlyActive;
    ClientNetworkManager::getInstance().sendRequest(CommandType::EditBook, req);
}

void AdminPanelWindow::executeDeleteBookAdmin() {
    auto selected = tableAllBooks->selectedItems();
    if (selected.isEmpty()) return;
    int bookId = tableAllBooks->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    if (QMessageBox::question(this, "تایید حذف", "این کتاب به‌عنوانِ نامعتبر/مغایرِ قوانین حذف شود؟") != QMessageBox::Yes) return;
    QJsonObject req;
    req["bookId"] = bookId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::DeleteBook, req);
}

// =========================================================================
// اکشن‌های تبِ نظرات
// =========================================================================
static int selectedCommentId(QTableWidget* table) {
    auto selected = table->selectedItems();
    if (selected.isEmpty()) return -1;
    return table->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();
}

void AdminPanelWindow::executeCommentApproval() {
    int commentId = selectedCommentId(tableAllComments);
    if (commentId < 0) return;
    QJsonObject req;
    req["commentId"] = commentId;
    req["approve"] = true;
    ClientNetworkManager::getInstance().sendRequest(CommandType::ApproveComment, req);
}

void AdminPanelWindow::executeCommentRejection() {
    int commentId = selectedCommentId(tableAllComments);
    if (commentId < 0) return;
    QJsonObject req;
    req["commentId"] = commentId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::RejectComment, req);
}

void AdminPanelWindow::executeDeleteComment() {
    int commentId = selectedCommentId(tableAllComments);
    if (commentId < 0) return;
    if (QMessageBox::question(this, "تایید", "این نظر به‌طورِ کامل حذف شود؟") != QMessageBox::Yes) return;
    QJsonObject req;
    req["commentId"] = commentId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::DeleteComment, req);
}

// =========================================================================
// اکشن‌های تبِ تخفیف‌ها
// =========================================================================
void AdminPanelWindow::executeApproveDiscount() {
    auto selected = tablePendingDiscounts->selectedItems();
    if (selected.isEmpty()) return;
    int discountId = tablePendingDiscounts->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();
    QJsonObject req;
    req["discountId"] = discountId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::ApproveDiscount, req);
}

void AdminPanelWindow::executeRejectDiscount() {
    auto selected = tablePendingDiscounts->selectedItems();
    if (selected.isEmpty()) return;
    int discountId = tablePendingDiscounts->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();
    QJsonObject req;
    req["discountId"] = discountId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::RejectDiscount, req);
}

// =========================================================================
// اکشنِ تبِ تنظیمات
// =========================================================================
void AdminPanelWindow::executeSaveLimits() {
    QJsonObject req;
    req["maxPurchasesPerDay"] = spinMaxPurchasesPerDay->value();
    req["maxCommentsPerDay"] = spinMaxCommentsPerDay->value();
    ClientNetworkManager::getInstance().sendRequest(CommandType::SetLimits, req);
}

// =========================================================================
// دیسپچرِ پاسخ‌های شبکه
// =========================================================================
void AdminPanelWindow::onNetworkReply(CommandType commandType, QJsonObject payload, bool ok) {
    if (!ok) {
        QMessageBox::warning(this, "خطا", payload.value("error").toString());
        return;
    }

    switch (commandType) {
    case CommandType::GetAllUsers: {
        allUsersRaw = payload;
        applyUserFilters();
        break;
    }
    case CommandType::GetAllBooksAdmin: {
        auto books = payload.value("books").toArray();
        tableAllBooks->setRowCount(books.size());
        for (int row = 0; row < books.size(); ++row) {
            QJsonObject b = books[row].toObject();
            auto* titleItem = new QTableWidgetItem(b.value("title").toString());
            titleItem->setData(Qt::UserRole, b.value("id").toInt());
            tableAllBooks->setItem(row, 0, titleItem);
            tableAllBooks->setItem(row, 1, new QTableWidgetItem(b.value("author").toString()));
            tableAllBooks->setItem(row, 2, new QTableWidgetItem(b.value("publisherUsername").toString()));
            tableAllBooks->setItem(row, 3, new QTableWidgetItem(QString::number(b.value("basePrice").toDouble())));
            tableAllBooks->setItem(row, 4, new QTableWidgetItem(QString::number(b.value("averageRating").toDouble(), 'f', 1)));
            bool isDeleted = b.value("isDeleted").toBool();
            bool isActive = b.value("isActive").toBool();
            QString status = isDeleted ? "حذف‌شده" : (isActive ? "فعال" : "غیرفعال");
            tableAllBooks->setItem(row, 5, new QTableWidgetItem(status));
        }
        break;
    }
    case CommandType::GetAllComments: {
        auto comments = payload.value("comments").toArray();
        tableAllComments->setRowCount(comments.size());
        for (int row = 0; row < comments.size(); ++row) {
            QJsonObject c = comments[row].toObject();
            auto* userItem = new QTableWidgetItem(c.value("username").toString());
            userItem->setData(Qt::UserRole, c.value("commentId").toInt());
            tableAllComments->setItem(row, 0, userItem);
            tableAllComments->setItem(row, 1, new QTableWidgetItem(QString::number(c.value("bookId").toInt())));
            tableAllComments->setItem(row, 2, new QTableWidgetItem(c.value("text").toString()));
            tableAllComments->setItem(row, 3, new QTableWidgetItem(c.value("timestamp").toString()));
            tableAllComments->setItem(row, 4, new QTableWidgetItem(c.value("isApproved").toBool() ? "تأییدشده" : "در انتظار/رد‌شده"));
        }
        break;
    }
    case CommandType::GetPendingDiscounts: {
        auto discounts = payload.value("discounts").toArray();
        tablePendingDiscounts->setRowCount(discounts.size());
        for (int row = 0; row < discounts.size(); ++row) {
            QJsonObject d = discounts[row].toObject();
            auto* bookItem = new QTableWidgetItem(d.value("bookTitle").toString());
            bookItem->setData(Qt::UserRole, d.value("discountId").toInt());
            tablePendingDiscounts->setItem(row, 0, bookItem);
            int type = d.value("discountType").toInt();
            tablePendingDiscounts->setItem(row, 1, new QTableWidgetItem(type == 0 ? "درصدی" : "مبلغ ثابت"));
            tablePendingDiscounts->setItem(row, 2, new QTableWidgetItem(QString::number(d.value("discountValue").toDouble())));
            tablePendingDiscounts->setItem(row, 3, new QTableWidgetItem(d.value("startDateTime").toString()));
            tablePendingDiscounts->setItem(row, 4, new QTableWidgetItem(d.value("endDateTime").toString()));
        }
        break;
    }
    case CommandType::GetLimits: {
        spinMaxPurchasesPerDay->setValue(payload.value("maxPurchasesPerDay").toInt(1000));
        spinMaxCommentsPerDay->setValue(payload.value("maxCommentsPerDay").toInt(1000));
        break;
    }
    case CommandType::GetUserDetails: {
        QString text;
        text += "نام کاربری: " + payload.value("username").toString() + "\n";
        text += "ایمیل: " + payload.value("email").toString() + "\n";
        text += "نقش: " + payload.value("role").toString() + "\n";
        text += "تاریخِ ثبت‌نام: " + payload.value("registrationDate").toString() + "\n";
        if (payload.contains("walletBalance")) {
            text += "موجودیِ کیف‌پول: " + QString::number(payload.value("walletBalance").toDouble()) + "\n";
        }

        if (payload.contains("publisherStats")) {
            // برای ناشر: آمارهای واقعیِ کتاب‌های خودش (نه شمارنده‌های خرید/نظرِ کاربرِ عادی که همیشه صفر بود)
            QJsonObject ps = payload.value("publisherStats").toObject();
            text += "\n--- آمارِ ناشر ---\n";
            text += "تعدادِ کتاب‌های منتشرشده: " + QString::number(ps.value("publishedBooksCount").toInt()) + "\n";
            text += "تعدادِ کلِ فروش: " + QString::number(ps.value("totalSales").toInt()) + "\n";
            text += "درآمدِ کل: " + QString::number(ps.value("totalRevenue").toDouble()) + " تومان\n";
            text += "تعدادِ نظراتِ دریافت‌شده روی کتاب‌ها: " + QString::number(ps.value("totalCommentsReceived").toInt()) + "\n";
            text += "میانگینِ امتیازِ کتاب‌ها: " + QString::number(ps.value("averageRatingAcrossBooks").toDouble(), 'f', 2) + "\n";

            text += "\n--- کتاب‌های ناشر ---\n";
            for (const auto &v : ps.value("books").toArray()) {
                QJsonObject bo = v.toObject();
                text += QString("• %1  |  فروش: %2  |  نظرات: %3  |  امتیاز: %4\n")
                            .arg(bo.value("title").toString())
                            .arg(bo.value("salesCount").toInt())
                            .arg(bo.value("commentsCount").toInt())
                            .arg(QString::number(bo.value("averageRating").toDouble(), 'f', 1));
            }
        } else {
            text += "\nتعدادِ کتاب‌های خریداری‌شده: " + QString::number(payload.value("purchasedBookIds").toArray().size()) + "\n";
            text += "تعدادِ نظراتِ ثبت‌شده: " + QString::number(payload.value("comments").toArray().size()) + "\n";
        }

        text += "\n--- تاریخچه‌ی ورود (۲۰ موردِ اخیر) ---\n";
        for (const auto &v : payload.value("loginHistory").toArray()) {
            text += v.toString() + "\n";
        }

        auto* dialog = new QDialog(this);
        dialog->setWindowTitle("جزئیاتِ کاربر");
        dialog->resize(450, 500);
        auto* layout = new QVBoxLayout(dialog);
        auto* view = new QTextEdit();
        view->setReadOnly(true);
        view->setPlainText(text);
        layout->addWidget(view);
        dialog->exec();
        break;
    }
    case CommandType::BanUser:
    case CommandType::UnbanUser:
    case CommandType::DisableUserTemporarily:
    case CommandType::ChangeUserRole:
    case CommandType::DeleteUserAccount:
        requestAllUsers();
        break;

    case CommandType::EditBook: // برای toggle فعال/غیرفعال هم از همین دستور استفاده می‌کنیم
    case CommandType::DeleteBook:
        requestAllBooks();
        break;

    case CommandType::ApproveComment:
    case CommandType::RejectComment:
    case CommandType::DeleteComment:
        requestAllComments();
        break;

    case CommandType::ApproveDiscount:
    case CommandType::RejectDiscount:
        requestPendingDiscounts();
        break;

    case CommandType::SetLimits:
        QMessageBox::information(this, "موفق", "تنظیمات ذخیره شد.");
        break;

    default:
        break;
    }
}

