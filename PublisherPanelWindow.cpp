#include "PublisherPanelWindow.h"
#include "ClientNetworkManager.h"
#include "ClientUtils.h"
#include "Genre.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QLabel>
#include <QJsonArray>

PublisherPanelWindow::PublisherPanelWindow(int publisherId, QWidget *parent)
    : QMainWindow(parent), currentPublisherId(publisherId) {
    buildUi();

    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::serverReplyReceived,
            this, &PublisherPanelWindow::onNetworkReply);
    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::pushNotificationArrived,
            this, &PublisherPanelWindow::onPushNotification);

    requestMyBooks();

    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetNotifications, req);
}

void PublisherPanelWindow::buildUi() {
    setWindowTitle("BookClub - پنل ناشر");
    resize(950, 650);

    auto* central = new QWidget();
    auto* outerLayout = new QVBoxLayout(central);

    auto* topBar = new QHBoxLayout();
    btnProfile = new QPushButton("👤 پروفایل");
    btnLogout = new QPushButton("🚪 خروج از حساب");
    btnOpenNotifications = new QPushButton("🔔 اعلان‌ها");
    topBar->addStretch(1);
    topBar->addWidget(btnProfile);
    topBar->addWidget(btnLogout);
    topBar->addWidget(btnOpenNotifications);
    outerLayout->addLayout(topBar);

    auto* mainLayout = new QHBoxLayout();
    outerLayout->addLayout(mainLayout);

    // ---- ستون چپ: جدول کتاب‌های من ----
    auto* leftBox = new QGroupBox("کتاب‌های منتشرشده‌ی من");
    auto* leftLayout = new QVBoxLayout(leftBox);
    lblMyBooksHeader = new QLabel("کتاب‌های منتشرشده‌ی من");
    lblMyBooksHeader->setAlignment(Qt::AlignCenter);
    lblMyBooksHeader->setStyleSheet("font-weight: bold; font-size: 14px; padding: 4px;");
    tableMyBooks = new QTableWidget(0, 5);
    tableMyBooks->setHorizontalHeaderLabels({"عنوان", "قیمت", "میانگین امتیاز", "فروش", "وضعیت"});
    tableMyBooks->horizontalHeader()->setStretchLastSection(true);
    tableMyBooks->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableMyBooks->setSelectionMode(QAbstractItemView::SingleSelection);
    tableMyBooks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableMyBooks->setStyleSheet(
        "QTableWidget::item:selected { font-weight: bold; background: palette(highlight); color: palette(highlighted-text); }");

    auto* tableButtons = new QHBoxLayout();
    btnEditSelected = new QPushButton("ویرایشِ انتخاب‌شده");
    btnDeleteSelected = new QPushButton("حذف/غیرفعال‌کردن");
    btnOpenAnalytics = new QPushButton("📊 آمار و نمودار");
    btnManageDiscounts = new QPushButton("🏷️ مدیریتِ تخفیف");
    btnViewBookDetails = new QPushButton("👁️ جزئیاتِ کتاب (نظرات و امتیاز)");
    tableButtons->addWidget(btnEditSelected);
    tableButtons->addWidget(btnDeleteSelected);
    tableButtons->addWidget(btnOpenAnalytics);
    tableButtons->addWidget(btnManageDiscounts);
    tableButtons->addWidget(btnViewBookDetails);

    leftLayout->addWidget(lblMyBooksHeader);
    leftLayout->addWidget(tableMyBooks);
    leftLayout->addLayout(tableButtons);

    auto* rightBox = new QGroupBox("افزودنِ کتابِ جدید");
    auto* rightForm = new QFormLayout(rightBox);
    txtNewBookTitle = new QLineEdit();
    txtNewBookAuthor = new QLineEdit();
    comboNewBookGenre = new QComboBox();
    comboNewBookGenre->addItem("داستانی", static_cast<int>(Genre::Fiction));
    comboNewBookGenre->addItem("غیرداستانی", static_cast<int>(Genre::NonFiction));
    comboNewBookGenre->addItem("علمی‌تخیلی", static_cast<int>(Genre::SciFi));
    comboNewBookGenre->addItem("فانتزی", static_cast<int>(Genre::Fantasy));
    comboNewBookGenre->addItem("درسی", static_cast<int>(Genre::darsi));
    comboNewBookGenre->addItem("معمایی", static_cast<int>(Genre::Mystery));
    comboNewBookGenre->addItem("عاشقانه", static_cast<int>(Genre::Romance));
    comboNewBookGenre->addItem("زندگینامه", static_cast<int>(Genre::Biography));
    comboNewBookGenre->addItem("خوددرمانی", static_cast<int>(Genre::SelfHelp));
    comboNewBookGenre->addItem("فلسفی", static_cast<int>(Genre::Philosophy));
    comboNewBookGenre->addItem("شعر", static_cast<int>(Genre::Poetry));
    comboNewBookGenre->addItem("کودک", static_cast<int>(Genre::Children));
    comboNewBookGenre->addItem("تاریخی", static_cast<int>(Genre::History));
    txtNewBookDescription = new QTextEdit();
    txtNewBookDescription->setMaximumHeight(80);
    spinNewBookPrice = new QDoubleSpinBox();
    spinNewBookPrice->setRange(0, 10000000);
    spinNewBookPrice->setDecimals(0);
    txtNewBookCoverPath = new QLineEdit();
    txtNewBookCoverPath->setPlaceholderText("مسیر فایلِ عکسِ جلد...");
    btnBrowseCoverPath = new QPushButton("انتخاب فایل");
    txtNewBookPdfPath = new QLineEdit();
    txtNewBookPdfPath->setPlaceholderText("مسیر فایلِ PDF...");
    btnBrowsePdfPath = new QPushButton("انتخاب فایل");
    btnUploadBook = new QPushButton("انتشارِ کتاب");

    auto* coverRow = new QHBoxLayout();
    coverRow->addWidget(txtNewBookCoverPath);
    coverRow->addWidget(btnBrowseCoverPath);

    auto* pdfRow = new QHBoxLayout();
    pdfRow->addWidget(txtNewBookPdfPath);
    pdfRow->addWidget(btnBrowsePdfPath);

    rightForm->addRow("عنوان:", txtNewBookTitle);
    rightForm->addRow("نویسنده:", txtNewBookAuthor);
    rightForm->addRow("ژانر:", comboNewBookGenre);
    rightForm->addRow("توضیحات:", txtNewBookDescription);
    rightForm->addRow("قیمت:", spinNewBookPrice);
    rightForm->addRow("عکسِ جلد:", coverRow);
    rightForm->addRow("فایلِ PDF:", pdfRow);
    rightForm->addRow(btnUploadBook);

    mainLayout->addWidget(leftBox, /*stretch=*/2);
    mainLayout->addWidget(rightBox, /*stretch=*/1);

    setCentralWidget(central);

    connect(btnUploadBook, &QPushButton::clicked, this, &PublisherPanelWindow::handleUploadSubmit);
    connect(btnEditSelected, &QPushButton::clicked, this, &PublisherPanelWindow::handleEditSubmit);
    connect(btnDeleteSelected, &QPushButton::clicked, this, &PublisherPanelWindow::handleDeleteSubmit);
    connect(btnOpenAnalytics, &QPushButton::clicked, this, &PublisherPanelWindow::openAnalyticsWindow);
    connect(btnManageDiscounts, &QPushButton::clicked, this, &PublisherPanelWindow::openDiscountWindow);
    connect(btnOpenNotifications, &QPushButton::clicked, this, &PublisherPanelWindow::onOpenNotificationsClicked);
    connect(btnProfile, &QPushButton::clicked, this, &PublisherPanelWindow::onProfileClicked);
    connect(btnLogout, &QPushButton::clicked, this, &PublisherPanelWindow::onLogoutClicked);
    connect(btnViewBookDetails, &QPushButton::clicked, this, &PublisherPanelWindow::onViewBookDetailsClicked);
    connect(btnBrowseCoverPath, &QPushButton::clicked, this, &PublisherPanelWindow::onBrowseCoverPath);
    connect(btnBrowsePdfPath, &QPushButton::clicked, this, &PublisherPanelWindow::onBrowsePdfPath);
    connect(tableMyBooks, &QTableWidget::itemSelectionChanged, this, &PublisherPanelWindow::onMyBooksSelectionChanged);
}

void PublisherPanelWindow::requestMyBooks() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetPublisherAnalytics, req);
}

void PublisherPanelWindow::loadPublisherBooks() {
    requestMyBooks();
}

void PublisherPanelWindow::refreshTable() {
    tableMyBooks->setRowCount(myBooksCache.size());
    for (int row = 0; row < myBooksCache.size(); ++row) {
        const Book &b = myBooksCache[row];
        // نکته‌ی مهم: قیمت/امتیاز/فروش از Book (که فقط id/title/isActive را دارد) خوانده نمی‌شود
        // بلکه از پاسخِ خامِ سرور (myBooksAnalyticsCache) گرفته می‌شود؛ همینه که تا قبل از این
        // اصلاح، بعد از هر ویرایش این ستون‌ها همیشه یک مقدارِ ثابت/صفر نشان می‌دادن.
        QJsonObject stats = (row < myBooksAnalyticsCache.size()) ? myBooksAnalyticsCache[row].toObject() : QJsonObject();

        tableMyBooks->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getTitle())));
        tableMyBooks->setItem(row, 1, new QTableWidgetItem(QString::number(stats.value("basePrice").toDouble())));
        tableMyBooks->setItem(row, 2, new QTableWidgetItem(QString::number(stats.value("averageRating").toDouble(), 'f', 1)));
        tableMyBooks->setItem(row, 3, new QTableWidgetItem(QString::number(stats.value("salesCount").toInt())));
        tableMyBooks->setItem(row, 4, new QTableWidgetItem(b.getIsActive() ? "فعال" : "غیرفعال"));
        tableMyBooks->item(row, 0)->setData(Qt::UserRole, b.getId());
    }
}

void PublisherPanelWindow::handleUploadSubmit() {
    if (txtNewBookTitle->text().isEmpty() || txtNewBookAuthor->text().isEmpty()) {
        QMessageBox::warning(this, "خطا", "عنوان و نویسنده نمی‌توانند خالی باشند.");
        return;
    }
    QJsonObject req;
    req["title"] = txtNewBookTitle->text();
    req["author"] = txtNewBookAuthor->text();
    req["genre"] = comboNewBookGenre->currentData().toInt();
    req["description"] = txtNewBookDescription->toPlainText();
    req["basePrice"] = spinNewBookPrice->value();
    req["coverImagePath"] = txtNewBookCoverPath->text();
    req["pdfFileName"] = txtNewBookPdfPath->text();
    ClientNetworkManager::getInstance().sendRequest(CommandType::AddBook, req);
}

void PublisherPanelWindow::handleEditSubmit() {
    auto selected = tableMyBooks->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "توجه", "لطفاً ابتدا یک کتاب را از لیست انتخاب کنید.");
        return;
    }
    int bookId = tableMyBooks->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    QJsonObject req;
    req["bookId"] = bookId;
    if (!txtNewBookTitle->text().isEmpty()) req["title"] = txtNewBookTitle->text();
    if (!txtNewBookAuthor->text().isEmpty()) req["author"] = txtNewBookAuthor->text();
    if (!txtNewBookDescription->toPlainText().isEmpty()) req["description"] = txtNewBookDescription->toPlainText();
    if (spinNewBookPrice->value() > 0) req["basePrice"] = spinNewBookPrice->value();

    if (req.keys().size() <= 1) {
        QMessageBox::warning(this, "خطا", "هیچ اطلاعاتی برای ویرایش وارد نشده است. حداقل یکی از فیلدهای عنوان، نویسنده، توضیحات یا قیمت را وارد کنید.");
        return;
    }
    ClientNetworkManager::getInstance().sendRequest(CommandType::EditBook, req);
}

void PublisherPanelWindow::handleDeleteSubmit() {
    auto selected = tableMyBooks->selectedItems();
    if (selected.isEmpty()) return;
    int bookId = tableMyBooks->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    // find the book in local cache
    Book foundBook;
    bool haveBook = false;
    for (const auto &b : myBooksCache) {
        if (b.getId() == bookId) { foundBook = b; haveBook = true; break; }
    }

    if (haveBook && !foundBook.getIsActive()) {
        if (QMessageBox::question(this, "فعالسازی کتاب", "این کتاب در حال حاضر غیرفعال است. آیا آن را فعال کنم؟") != QMessageBox::Yes) return;
        QJsonObject req;
        req["bookId"] = bookId;
        req["isActive"] = true;
        ClientNetworkManager::getInstance().sendRequest(CommandType::EditBook, req);
        return;
    }

    if (QMessageBox::question(this, "تایید حذف/غیرفعال‌سازی", "آیا می‌خواهید این کتاب را غیرفعال (یا حذف نرم) کنید؟") != QMessageBox::Yes) return;
    QJsonObject req;
    req["bookId"] = bookId;
    req["isActive"] = false;
    ClientNetworkManager::getInstance().sendRequest(CommandType::EditBook, req);
}

void PublisherPanelWindow::openAnalyticsWindow() {
    if (!chartWidget) {
        chartWidget = new AnalyticsChartWidget();
        chartWidget->setWindowTitle("آمار و نمودار فروش");
        chartWidget->resize(700, 500);
    }
    requestMyBooks();
    chartWidget->show();
    chartWidget->raise();
}

void PublisherPanelWindow::openDiscountWindow() {
    if (!discountWidget) {
        discountWidget = new DiscountManagerWidget();
        discountWidget->setWindowTitle("مدیریتِ تخفیف");
        discountWidget->resize(400, 300);
    }
    discountWidget->setAvailableBooks(myBooksCache);
    discountWidget->show();
    discountWidget->raise();
}

void PublisherPanelWindow::onOpenNotificationsClicked() {
    if (!notificationCenter) {
        notificationCenter = new NotificationCenterWidget();
        notificationCenter->setWindowTitle("اعلان‌های من (فروش، نظر و امتیازِ کتاب‌ها)");
        notificationCenter->resize(400, 500);
    }
    notificationCenter->show();
    notificationCenter->raise();
}

void PublisherPanelWindow::onProfileClicked() {
    profileDialog = new QDialog(this);
    profileDialog->setWindowTitle("پروفایلِ ناشر");
    profileDialog->resize(400, 280);
    auto* layout = new QVBoxLayout(profileDialog);

    auto* form = new QFormLayout();
    profileUsernameField = new QLineEdit();
    profileUsernameField->setReadOnly(true);
    profileEmailField = new QLineEdit();
    form->addRow("نام کاربری:", profileUsernameField);
    form->addRow("ایمیل:", profileEmailField);
    layout->addLayout(form);

    auto* btnSave = new QPushButton("ذخیره‌ی تغییراتِ ایمیل");
    layout->addWidget(btnSave);

    layout->addWidget(new QLabel("---- تغییرِ رمزِ عبور ----"));
    auto* passForm = new QFormLayout();
    profileOldPasswordField = new QLineEdit();
    profileOldPasswordField->setEchoMode(QLineEdit::Password);
    profileNewPasswordField = new QLineEdit();
    profileNewPasswordField->setEchoMode(QLineEdit::Password);
    passForm->addRow("رمزِ فعلی:", profileOldPasswordField);
    passForm->addRow("رمزِ جدید:", profileNewPasswordField);
    layout->addLayout(passForm);

    auto* btnChange = new QPushButton("تغییرِ رمزِ عبور");
    layout->addWidget(btnChange);

    connect(btnSave, &QPushButton::clicked, this, &PublisherPanelWindow::onSaveProfileClicked);
    connect(btnChange, &QPushButton::clicked, this, &PublisherPanelWindow::onChangePasswordClicked);

    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetProfile, req);
    profileDialog->exec();
}

void PublisherPanelWindow::onSaveProfileClicked() {
    QJsonObject req;
    req["email"] = profileEmailField->text();
    ClientNetworkManager::getInstance().sendRequest(CommandType::UpdateProfile, req);
}

void PublisherPanelWindow::onChangePasswordClicked() {
    QString oldPass = profileOldPasswordField->text();
    QString newPass = profileNewPasswordField->text();
    if (oldPass.isEmpty() || newPass.isEmpty()) {
        QMessageBox::warning(profileDialog, "خطا", "رمزِ فعلی و رمزِ جدید را وارد کنید.");
        return;
    }
    QJsonObject req;
    req["oldPassword"] = oldPass;
    req["newPassword"] = newPass;
    ClientNetworkManager::getInstance().sendRequest(CommandType::ChangePassword, req);
}

void PublisherPanelWindow::onLogoutClicked() {
    emit logoutRequested();
}

void PublisherPanelWindow::onPushNotification(QJsonObject payload) {
    QString message = payload.value("message").toString();
    auto* toast = new InAppNotificationWidget(this);
    toast->popToastMessage(message);

    if (notificationCenter) {
        AppNotification n = AppNotification::fromStorage(
            payload.value("id").toInt(),
            static_cast<NotificationType>(payload.value("type").toInt()),
            message.toStdString(), currentPublisherId, false,
            payload.value("timestamp").toString().toStdString());
        notificationCenter->addNotification(n);
    }
}

void PublisherPanelWindow::onViewBookDetailsClicked() {
    auto selected = tableMyBooks->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "توجه", "ابتدا یک کتاب را از لیست انتخاب کنید.");
        return;
    }
    int bookId = tableMyBooks->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();
    QJsonObject req;
    req["bookId"] = bookId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetBookDetails, req);
}

void PublisherPanelWindow::onMyBooksSelectionChanged() {
    auto selected = tableMyBooks->selectedItems();
    if (selected.isEmpty()) return;

    int row = selected.first()->row();
    if (row < 0) return;

    QTableWidgetItem* titleItem = tableMyBooks->item(row, 0);
    QTableWidgetItem* priceItem = tableMyBooks->item(row, 1);
    if (titleItem) {
        txtNewBookTitle->setText(titleItem->text());
    }
    if (priceItem) {
        spinNewBookPrice->setValue(priceItem->text().toDouble());
    }
}

void PublisherPanelWindow::onBrowseCoverPath() {
    QString fileName = QFileDialog::getOpenFileName(this, "انتخاب عکس جلد", QString(), "تصاویر (*.png *.jpg *.jpeg *.bmp);;همه فایل‌ها (*)");
    if (!fileName.isEmpty()) {
        txtNewBookCoverPath->setText(fileName);
    }
}

void PublisherPanelWindow::onBrowsePdfPath() {
    QString fileName = QFileDialog::getOpenFileName(this, "انتخاب فایل PDF", QString(), "PDF (*.pdf);;همه فایل‌ها (*)");
    if (!fileName.isEmpty()) {
        txtNewBookPdfPath->setText(fileName);
    }
}

void PublisherPanelWindow::onNetworkReply(CommandType commandType, QJsonObject payload, bool ok) {
    if (!ok) {
        QMessageBox::warning(this, "خطا", payload.value("error").toString());
        return;
    }

    if (commandType == CommandType::GetProfile) {
        if (profileDialog && profileUsernameField && profileEmailField) {
            profileUsernameField->setText(payload.value("username").toString());
            profileEmailField->setText(payload.value("email").toString());
        }
        return;
    }

    if (commandType == CommandType::GetPublisherAnalytics) {
        myBooksCache.clear();
        myBooksAnalyticsCache = payload.value("books").toArray();
        for (const auto &v : myBooksAnalyticsCache) {
            QJsonObject bo = v.toObject();
            // پاسخِ این دستور شکلِ متفاوتی از GetBooks دارد (bookId به‌جای id، و فیلدهای کمتر)؛
            // برای همین یک Book سبک با مقادیرِ موجود می‌سازیم (بقیه‌ی فیلدها خالی می‌مانند
            // چون این پاسخ برایِ آمار است نه برای فرمِ ویرایشِ کامل).
            Book b("", "", currentPublisherId, Genre::Fiction, "", 0, "", "", "");
            b.setId(bo.value("bookId").toInt());
            b.setTitle(bo.value("title").toString().toStdString());
            b.setIsActive(bo.value("isActive").toBool());
            myBooksCache.push_back(b);
        }
        refreshTable();
        if (chartWidget && chartWidget->isVisible()) {
            chartWidget->drawRevenueBarChart(payload);
        }
        return;
    }

    if (commandType == CommandType::AddBook || commandType == CommandType::EditBook
        || commandType == CommandType::DeleteBook) {
        QMessageBox::information(this, "موفق", "عملیات با موفقیت انجام شد.");
        requestMyBooks();
        return;
    }

    if (commandType == CommandType::GetNotifications) {
        if (!notificationCenter) {
            notificationCenter = new NotificationCenterWidget();
            notificationCenter->setWindowTitle("اعلان‌های من (فروش، نظر و امتیازِ کتاب‌ها)");
            notificationCenter->resize(400, 500);
        }
        QVector<AppNotification> notifs;
        for (const auto &v : payload.value("notifications").toArray()) {
            QJsonObject no = v.toObject();
            notifs.push_back(AppNotification::fromStorage(
                no.value("id").toInt(),
                static_cast<NotificationType>(no.value("type").toInt()),
                no.value("message").toString().toStdString(),
                currentPublisherId,
                no.value("isRead").toBool(),
                no.value("timestamp").toString().toStdString()));
        }
        notificationCenter->loadNotifications(notifs);
        return;
    }

    if (commandType == CommandType::GetBookDetails) {
        // فروش/درآمد در پاسخِ GetBookDetails نیست؛ از کشِ محلیِ آمار (که همین الان از سرور گرفته‌ایم) می‌خوانیم
        int salesCount = 0;
        double revenue = 0.0;
        for (const auto &v : myBooksAnalyticsCache) {
            QJsonObject bo = v.toObject();
            if (bo.value("bookId").toInt() == payload.value("id").toInt()) {
                salesCount = bo.value("salesCount").toInt();
                revenue = bo.value("revenue").toDouble();
                break;
            }
        }

        QString text;
        text += "عنوان: " + payload.value("title").toString() + "\n";
        text += "نویسنده: " + payload.value("author").toString() + "\n";
        text += "توضیحات: " + payload.value("description").toString() + "\n\n";
        text += "قیمتِ نهایی: " + QString::number(payload.value("finalPrice").toDouble()) + " تومان\n";
        text += "میانگینِ امتیاز: " + QString::number(payload.value("averageRating").toDouble(), 'f', 2) + "\n";
        text += "تعدادِ فروش: " + QString::number(salesCount) + "   |   درآمد: " + QString::number(revenue) + " تومان\n";

        QJsonArray comments = payload.value("comments").toArray();
        text += QString("\n--- نظراتِ کاربران (%1 مورد) ---\n").arg(comments.size());
        if (comments.isEmpty()) {
            text += "هنوز نظری ثبت نشده است.\n";
        }
        for (const auto &v : comments) {
            QJsonObject co = v.toObject();
            bool approved = co.value("isApproved").toBool();
            text += QString("• %1%2: %3\n")
                        .arg(co.value("username").toString())
                        .arg(approved ? "" : " [در انتظارِ تاییدِ ادمین]")
                        .arg(co.value("text").toString());
        }

        QMessageBox::information(this, "جزئیاتِ کتاب", text);
        return;
    }
}