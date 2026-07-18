#include "UserPanelWindow.h"
#include "ClientNetworkManager.h"
#include "ClientUtils.h"
#include "Genre.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QDialog>
#include <QJsonArray>
#include <QInputDialog>

UserPanelWindow::UserPanelWindow(int userId, QWidget *parent)
    : QMainWindow(parent), currentUserId(userId), myCart(userId) {
    buildUi();

    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::serverReplyReceived,
            this, &UserPanelWindow::onNetworkReply);
    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::pushNotificationArrived,
            this, &UserPanelWindow::onPushNotification);

    requestProfile();
    requestCatalog();
    requestLibrary();

    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetNotifications, req);
}
void UserPanelWindow::buildUi() {
    setWindowTitle("BookClub - پنل کاربری");
    resize(900, 600);

    auto* central = new QWidget();
    auto* mainLayout = new QVBoxLayout(central);

    // ---- نوار بالا ----
    auto* topBar = new QHBoxLayout();
    lblBalance = new QLabel("موجودی: ...");
    txtSearch = new QLineEdit();
    txtSearch->setPlaceholderText("جستجو بر اساس نام کتاب یا نویسنده...");
    comboGenreFilter = new QComboBox();
    comboGenreFilter->addItem("همه‌ی ژانرها", -1);
    comboGenreFilter->addItem("داستانی", static_cast<int>(Genre::Fiction));
    comboGenreFilter->addItem("غیرداستانی", static_cast<int>(Genre::NonFiction));
    comboGenreFilter->addItem("علمی‌تخیلی", static_cast<int>(Genre::SciFi));
    comboGenreFilter->addItem("فانتزی", static_cast<int>(Genre::Fantasy));
    btnOpenNotifications = new QPushButton("🔔 اعلان‌ها");

    topBar->addWidget(lblBalance);
    topBar->addWidget(txtSearch, /*stretch=*/1);
    topBar->addWidget(comboGenreFilter);
    topBar->addWidget(btnOpenNotifications);
    mainLayout->addLayout(topBar);

    auto* tabs = new QTabWidget();

    // ---- تب فروشگاه ----
    auto* shopTab = new QWidget();
    auto* shopLayout = new QVBoxLayout(shopTab);
    listWidgetCatalog = new QListWidget();
    auto* shopButtons = new QHBoxLayout();
    btnBuy = new QPushButton("خرید مستقیم");
    btnAddToCart = new QPushButton("افزودن به سبد خرید");
    btnSaveForLater = new QPushButton("ذخیره برای بعد");
    shopButtons->addWidget(btnBuy);
    shopButtons->addWidget(btnAddToCart);
    shopButtons->addWidget(btnSaveForLater);
    shopLayout->addWidget(listWidgetCatalog);
    shopLayout->addLayout(shopButtons);
    tabs->addTab(shopTab, "فروشگاه");

    // ---- تب سبد خرید ----
    auto* cartTab = new QWidget();
    auto* cartLayout = new QVBoxLayout(cartTab);
    listWidgetCart = new QListWidget();
    lblCartTotal = new QLabel("مبلغ کل: 0");
    auto* cartButtons = new QHBoxLayout();
    btnRemoveFromCart = new QPushButton("حذف از سبد");
    btnCheckout = new QPushButton("تسویه‌حساب نهایی");
    cartButtons->addWidget(btnRemoveFromCart);
    cartButtons->addWidget(btnCheckout);
    cartLayout->addWidget(listWidgetCart);
    cartLayout->addWidget(lblCartTotal);
    cartLayout->addLayout(cartButtons);
    tabs->addTab(cartTab, "سبد خرید");

    // ---- تب کتابخانه من ----
    auto* libraryTab = new QWidget();
    auto* libraryLayout = new QVBoxLayout(libraryTab);
    listWidgetMyLibrary = new QListWidget();
    btnRead = new QPushButton("مطالعه‌ی کتاب");
    libraryLayout->addWidget(listWidgetMyLibrary);
    libraryLayout->addWidget(btnRead);
    tabs->addTab(libraryTab, "کتابخانه‌ی من");

    // ---- تب ذخیره‌شده‌ها ----
    auto* savedTab = new QWidget();
    auto* savedLayout = new QVBoxLayout(savedTab);
    listWidgetSaved = new QListWidget();
    btnRemoveSaved = new QPushButton("حذف از ذخیره‌شده‌ها");
    savedLayout->addWidget(listWidgetSaved);
    savedLayout->addWidget(btnRemoveSaved);
    tabs->addTab(savedTab, "ذخیره‌شده‌ها");

    mainLayout->addWidget(tabs);
    setCentralWidget(central);

    connect(txtSearch, &QLineEdit::textChanged, this, &UserPanelWindow::onSearchTextChanged);
    connect(comboGenreFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UserPanelWindow::onGenreFilterChanged);
    connect(btnBuy, &QPushButton::clicked, this, &UserPanelWindow::onBuyBookClicked);
    connect(btnAddToCart, &QPushButton::clicked, this, &UserPanelWindow::onAddToCartClicked);
    connect(btnSaveForLater, &QPushButton::clicked, this, &UserPanelWindow::onSaveForLaterClicked);
    connect(btnCheckout, &QPushButton::clicked, this, &UserPanelWindow::onCheckoutClicked);
    connect(btnRemoveFromCart, &QPushButton::clicked, this, &UserPanelWindow::onRemoveFromCartClicked);
    connect(btnRead, &QPushButton::clicked, this, &UserPanelWindow::onReadBookClicked);
    connect(btnRemoveSaved, &QPushButton::clicked, this, &UserPanelWindow::onRemoveSavedClicked);
    connect(btnOpenNotifications, &QPushButton::clicked, this, &UserPanelWindow::onOpenNotificationsClicked);
}

// درخواست‌های شبکه
void UserPanelWindow::requestCatalog() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetBooks, req);
}

void UserPanelWindow::requestLibrary() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetLibrary, req);
}

void UserPanelWindow::requestProfile() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetProfile, req);
}

// رندر لیست‌ها
void UserPanelWindow::refreshCatalogListWidget(const QVector<Book> &books) {
    listWidgetCatalog->clear();
    for (const auto &b : books) {
        QString text = QString("%1 — %2  |  %3 تومان  |  ★ %4")
                           .arg(QString::fromStdString(b.getTitle()))
                           .arg(QString::fromStdString(b.getAuthor()))
                           .arg(b.getBasePrice())
                           .arg(b.getAverageRating(), 0, 'f', 1);
        auto* item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, b.getId());
        listWidgetCatalog->addItem(item);
    }
}

void UserPanelWindow::refreshCartListWidget() {
    listWidgetCart->clear();
    for (const auto &item : myCart.getItems()) {
        QString text = QString("%1 × %2 = %3 تومان")
                           .arg(QString::fromStdString(item.getBook().getTitle()))
                           .arg(item.getQuantity())
                           .arg(item.getSubtotal());
        auto* w = new QListWidgetItem(text);
        w->setData(Qt::UserRole, item.getBook().getId());
        listWidgetCart->addItem(w);
    }
    lblCartTotal->setText(QString("مبلغ کل: %1 تومان").arg(myCart.calculateTotal()));
}

Book* UserPanelWindow::findCachedBookById(int bookId) {
    for (auto &b : availableBooksCache) {
        if (b.getId() == bookId) return &b;
    }
    return nullptr;
}

// اسلات‌های فروشگاه
// =========================================================================
void UserPanelWindow::onSearchTextChanged(const QString &text) {
    auto filtered = searchEngine.filterByTitleOrAuthor(text, availableBooksCache);
    refreshCatalogListWidget(filtered);
}

void UserPanelWindow::onGenreFilterChanged(int /*index*/) {
    int genreValue = comboGenreFilter->currentData().toInt();
    if (genreValue < 0) {
        refreshCatalogListWidget(availableBooksCache);
    } else {
        refreshCatalogListWidget(searchEngine.filterByGenre(static_cast<Genre>(genreValue), availableBooksCache));
    }
}

void UserPanelWindow::onBuyBookClicked() {
    auto* item = listWidgetCatalog->currentItem();
    if (!item) return;
    int bookId = item->data(Qt::UserRole).toInt();

    if (QMessageBox::question(this, "تایید خرید", "آیا از خریدِ این کتاب مطمئنید؟") != QMessageBox::Yes) return;

    QJsonObject req;
    req["bookId"] = bookId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::BuyBook, req);
}

void UserPanelWindow::onAddToCartClicked() {
    auto* item = listWidgetCatalog->currentItem();
    if (!item) return;
    int bookId = item->data(Qt::UserRole).toInt();

    Book* b = findCachedBookById(bookId);
    if (!b) return;
    myCart.addItem(*b, 1);
    refreshCartListWidget();
}

void UserPanelWindow::onSaveForLaterClicked() {
    auto* item = listWidgetCatalog->currentItem();
    if (!item) return;
    QJsonObject req;
    req["bookId"] = item->data(Qt::UserRole).toInt();
    ClientNetworkManager::getInstance().sendRequest(CommandType::SaveBookForLater, req);
}

// اسلات‌های سبد خرید
// =========================================================================
void UserPanelWindow::onRemoveFromCartClicked() {
    auto* item = listWidgetCart->currentItem();
    if (!item) return;
    myCart.removeItem(item->data(Qt::UserRole).toInt());
    refreshCartListWidget();
}

void UserPanelWindow::onCheckoutClicked() {
    if (myCart.getItems().isEmpty()) {
        QMessageBox::information(this, "سبد خالی", "سبدِ خریدِ شما خالی است.");
        return;
    }
    // چون سرور دستورِ Checkout جداگانه ندارد (طبق طراحیِ فعلی)، به‌ازای هر آیتمِ سبد
    // یک BuyBook جداگانه می‌فرستیم. اگر هرکدام خطا بدهد (مثلاً موجودی کافی نیست)،
    // در پاسخِ BuyBook با ok=false مطلع می‌شویم (نگاه کن به onNetworkReply).
    for (const auto &item : myCart.getItems()) {
        QJsonObject req;
        req["bookId"] = item.getBook().getId();
        ClientNetworkManager::getInstance().sendRequest(CommandType::BuyBook, req);
    }
    myCart.clearAll();
    refreshCartListWidget();
    QMessageBox::information(this, "تسویه حساب", "درخواستِ خریدِ همه ی کتاب های سبد ارسال شد.");
}

// اسلات‌های کتابخانه‌ی من
// =========================================================================
void UserPanelWindow::onReadBookClicked() {
    auto* item = listWidgetMyLibrary->currentItem();
    if (!item) return;
    int bookId = item->data(Qt::UserRole).toInt();
    Book* b = nullptr;
    for (auto &book : myLibraryCache) {
        if (book.getId() == bookId) { b = &book; break; }
    }
    if (!b) return;

    QJsonObject req;
    req["bookId"] = bookId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetPageLocation, req);

    auto* dialog = new QDialog(this);
    dialog->setWindowTitle(QString::fromStdString(b->getTitle()));
    dialog->resize(800, 900);
    auto* layout = new QVBoxLayout(dialog);

    pdfReader = new PdfReaderWidget(dialog);
    layout->addWidget(pdfReader);
    pdfReader->openFile(QString::fromStdString(b->getPdfFileName()), bookId, 1);

    connect(pdfReader, &PdfReaderWidget::pageChanged, this, [](int bId, int newPage) {
        QJsonObject r;
        r["bookId"] = bId;
        r["pageNum"] = newPage;
        ClientNetworkManager::getInstance().sendRequest(CommandType::SavePageLocation, r);
    });

    dialog->exec();
}

// اسلات‌های ذخیره‌شده‌ها
// =========================================================================
void UserPanelWindow::onRemoveSavedClicked() {
    auto* item = listWidgetSaved->currentItem();
    if (!item) return;
    QJsonObject req;
    req["bookId"] = item->data(Qt::UserRole).toInt();
    ClientNetworkManager::getInstance().sendRequest(CommandType::RemoveSavedBook, req);
}

void UserPanelWindow::onOpenNotificationsClicked() {
    if (!notificationCenter) {
        notificationCenter = new NotificationCenterWidget();
        notificationCenter->setWindowTitle("اعلان‌های من");
        notificationCenter->resize(400, 500);
    }
    notificationCenter->show();
    notificationCenter->raise();
}

void UserPanelWindow::updateWalletDisplay(double currentBalance) {
    lblBalance->setText(QString("موجودی: %1 تومان").arg(currentBalance));
}

void UserPanelWindow::onPushNotification(QJsonObject payload) {
    QString message = payload.value("message").toString();
    auto* toast = new InAppNotificationWidget(this);
    toast->popToastMessage(message);

    if (notificationCenter) {
        AppNotification n = AppNotification::fromStorage(
            payload.value("id").toInt(),
            static_cast<NotificationType>(payload.value("type").toInt()),
            message.toStdString(), currentUserId, false,
            payload.value("timestamp").toString().toStdString());
        notificationCenter->addNotification(n);
    }
}

//  پاسخ‌های شبکه
// =========================================================================
void UserPanelWindow::onNetworkReply(CommandType commandType, QJsonObject payload, bool ok) {
    if (!ok) {
        if (commandType == CommandType::BuyBook) {
            QMessageBox::warning(this, "خطا در خرید", payload.value("error").toString());
        }
        return;
    }

    switch (commandType) {
    case CommandType::GetBooks: {
        availableBooksCache.clear();
        for (const auto &v : payload.value("books").toArray()) {
            availableBooksCache.push_back(ClientUtils::bookFromJson(v.toObject()));
        }
        refreshCatalogListWidget(availableBooksCache);
        break;
    }
    case CommandType::GetProfile: {
        if (payload.contains("walletBalance")) {
            updateWalletDisplay(payload.value("walletBalance").toDouble());
        }
        break;
    }
    case CommandType::BuyBook: {
        if (payload.contains("newWalletBalance")) {
            updateWalletDisplay(payload.value("newWalletBalance").toDouble());
        }
        requestLibrary(); // کتابخانه رو رفرش کن تا کتابِ تازه‌خریده‌شده نشون داده بشه
        break;
    }
    case CommandType::GetLibrary: {
        myLibraryCache.clear();
        listWidgetMyLibrary->clear();
        for (const auto &v : payload.value("purchasedBookIds").toArray()) {
            int bookId = v.toInt();
            Book* b = findCachedBookById(bookId);
            if (b) {
                myLibraryCache.push_back(*b);
                auto* item = new QListWidgetItem(QString::fromStdString(b->getTitle()));
                item->setData(Qt::UserRole, bookId);
                listWidgetMyLibrary->addItem(item);
            }
        }
        savedBooksCache.clear();
        listWidgetSaved->clear();
        for (const auto &v : payload.value("savedBookIds").toArray()) {
            int bookId = v.toInt();
            Book* b = findCachedBookById(bookId);
            if (b) {
                savedBooksCache.push_back(*b);
                auto* item = new QListWidgetItem(QString::fromStdString(b->getTitle()));
                item->setData(Qt::UserRole, bookId);
                listWidgetSaved->addItem(item);
            }
        }
        break;
    }
    case CommandType::GetNotifications: {
        if (!notificationCenter) {
            notificationCenter = new NotificationCenterWidget();
            notificationCenter->setWindowTitle("اعلان‌های من");
            notificationCenter->resize(400, 500);
        }
        QVector<AppNotification> notifs;
        for (const auto &v : payload.value("notifications").toArray()) {
            QJsonObject no = v.toObject();
            notifs.push_back(AppNotification::fromStorage(
                no.value("id").toInt(),
                static_cast<NotificationType>(no.value("type").toInt()),
                no.value("message").toString().toStdString(),
                currentUserId,
                no.value("isRead").toBool(),
                no.value("timestamp").toString().toStdString()));
        }
        notificationCenter->loadNotifications(notifs);
        break;
    }
    case CommandType::SaveBookForLater:
    case CommandType::RemoveSavedBook: {
        requestLibrary(); // دوباره لیستِ ذخیره‌شده‌ها رو بگیر
        break;
    }
    case CommandType::GetPageLocation: {
            if (pdfReader && payload.contains("pageNum")) {
                int page = payload.value("pageNum").toInt();
                pdfReader->jumpToPage(page);
        }
        break;
    }
    default:
        break;
    }
}

