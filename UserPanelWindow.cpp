#include "UserPanelWindow.h"
#include "ClientNetworkManager.h"
#include "ClientUtils.h"
#include "Genre.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
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
    requestShelves();

    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetNotifications, req);
}

void UserPanelWindow::buildUi() {
    setWindowTitle("BookClub - پنل کاربری");
    resize(950, 650);

    auto* central = new QWidget();
    auto* mainLayout = new QVBoxLayout(central);

    // ---- نوار بالا ----
    auto* topBar = new QHBoxLayout();
    lblBalance = new QLabel("موجودی: ...");
    btnChargeWallet = new QPushButton("💳 شارژِ کیف پول");
    btnProfile = new QPushButton("👤 پروفایل");
    txtSearch = new QLineEdit();
    txtSearch->setPlaceholderText("جستجو بر اساس نام کتاب یا نویسنده...");

    comboGenreFilter = new QComboBox();
    comboGenreFilter->addItem("همه‌ی ژانرها", -1);
    comboGenreFilter->addItem("داستانی", static_cast<int>(Genre::Fiction));
    comboGenreFilter->addItem("غیرداستانی", static_cast<int>(Genre::NonFiction));
    comboGenreFilter->addItem("علمی‌تخیلی", static_cast<int>(Genre::SciFi));
    comboGenreFilter->addItem("فانتزی", static_cast<int>(Genre::Fantasy));
    comboGenreFilter->addItem("درسی", static_cast<int>(Genre::darsi));
    comboGenreFilter->addItem("معمایی", static_cast<int>(Genre::Mystery));
    comboGenreFilter->addItem("عاشقانه", static_cast<int>(Genre::Romance));
    comboGenreFilter->addItem("زندگینامه", static_cast<int>(Genre::Biography));
    comboGenreFilter->addItem("خوددرمانی", static_cast<int>(Genre::SelfHelp));
    comboGenreFilter->addItem("فلسفی", static_cast<int>(Genre::Philosophy));
    comboGenreFilter->addItem("شعر", static_cast<int>(Genre::Poetry));
    comboGenreFilter->addItem("کودک", static_cast<int>(Genre::Children));
    comboGenreFilter->addItem("تاریخی", static_cast<int>(Genre::History));

    comboBookView = new QComboBox();
    comboBookView->addItem("همه‌ی کتاب‌ها");
    comboBookView->addItem("پیشنهادی برای من");
    comboBookView->addItem("محبوب‌ترین‌ها");
    comboBookView->addItem("پرفروش‌ترین‌ها");

    btnOpenNotifications = new QPushButton("🔔 اعلان‌ها");

    topBar->addWidget(lblBalance);
    topBar->addWidget(btnChargeWallet);
    topBar->addWidget(btnProfile);
    topBar->addWidget(txtSearch, /*stretch=*/1);
    topBar->addWidget(comboGenreFilter);
    topBar->addWidget(comboBookView);
    topBar->addWidget(btnOpenNotifications);
    mainLayout->addLayout(topBar);

    auto* tabs = new QTabWidget();

    // ---- تب فروشگاه ----
    auto* shopTab = new QWidget();
    auto* shopLayout = new QVBoxLayout(shopTab);
    listWidgetCatalog = new QListWidget();
    auto* shopButtons = new QHBoxLayout();
    btnViewDetails = new QPushButton("جزئیات / نظرات / امتیاز");
    btnBuy = new QPushButton("خرید مستقیم");
    btnAddToCart = new QPushButton("افزودن به سبد خرید");
    btnSaveForLater = new QPushButton("ذخیره برای بعد");
    shopButtons->addWidget(btnViewDetails);
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
    auto* libraryButtons = new QHBoxLayout();
    btnRead = new QPushButton("مطالعه‌ی کتاب");
    btnAddToShelf = new QPushButton("📚 افزودن به قفسه...");
    libraryButtons->addWidget(btnRead);
    libraryButtons->addWidget(btnAddToShelf);
    libraryLayout->addWidget(listWidgetMyLibrary);
    libraryLayout->addLayout(libraryButtons);
    tabs->addTab(libraryTab, "کتابخانه‌ی من");

    // ---- تب ذخیره‌شده‌ها ----
    auto* savedTab = new QWidget();
    auto* savedLayout = new QVBoxLayout(savedTab);
    listWidgetSaved = new QListWidget();
    btnRemoveSaved = new QPushButton("حذف از ذخیره‌شده‌ها");
    savedLayout->addWidget(listWidgetSaved);
    savedLayout->addWidget(btnRemoveSaved);
    tabs->addTab(savedTab, "ذخیره‌شده‌ها");

    // ---- تب قفسه‌های شخصی ----
    auto* shelvesTab = new QWidget();
    auto* shelvesLayout = new QVBoxLayout(shelvesTab);

    auto* shelfTopBar = new QHBoxLayout();
    comboShelfSelector = new QComboBox();
    btnCreateShelf = new QPushButton("➕ قفسه‌ی جدید");
    btnRenameShelf = new QPushButton("✏️ تغییرِ نام");
    btnDeleteShelf = new QPushButton("🗑️ حذفِ قفسه");
    shelfTopBar->addWidget(new QLabel("قفسه:"));
    shelfTopBar->addWidget(comboShelfSelector, /*stretch=*/1);
    shelfTopBar->addWidget(btnCreateShelf);
    shelfTopBar->addWidget(btnRenameShelf);
    shelfTopBar->addWidget(btnDeleteShelf);

    listWidgetShelfBooks = new QListWidget();

    auto* shelfBookButtons = new QHBoxLayout();
    comboMoveTargetShelf = new QComboBox();
    btnMoveToShelf = new QPushButton("➡️ انتقال به قفسه‌ی انتخاب‌شده");
    btnRemoveFromShelf = new QPushButton("حذف از این قفسه");
    shelfBookButtons->addWidget(new QLabel("انتقال به:"));
    shelfBookButtons->addWidget(comboMoveTargetShelf, /*stretch=*/1);
    shelfBookButtons->addWidget(btnMoveToShelf);
    shelfBookButtons->addWidget(btnRemoveFromShelf);

    shelvesLayout->addLayout(shelfTopBar);
    shelvesLayout->addWidget(new QLabel("کتاب‌های این قفسه (برای سازماندهیِ کتابخانه، مطابقِ سلیقه‌ی خودتان قفسه بسازید):"));
    shelvesLayout->addWidget(listWidgetShelfBooks);
    shelvesLayout->addLayout(shelfBookButtons);
    tabs->addTab(shelvesTab, "قفسه‌های من");

    mainLayout->addWidget(tabs);
    setCentralWidget(central);

    connect(txtSearch, &QLineEdit::textChanged, this, &UserPanelWindow::onSearchTextChanged);
    connect(comboGenreFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UserPanelWindow::onGenreFilterChanged);
    connect(comboBookView, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UserPanelWindow::onBookViewChanged);
    connect(listWidgetCatalog, &QListWidget::itemDoubleClicked, this, &UserPanelWindow::onCatalogItemDoubleClicked);
    connect(btnViewDetails, &QPushButton::clicked, this, &UserPanelWindow::onViewDetailsClicked);
    connect(btnBuy, &QPushButton::clicked, this, &UserPanelWindow::onBuyBookClicked);
    connect(btnAddToCart, &QPushButton::clicked, this, &UserPanelWindow::onAddToCartClicked);
    connect(btnSaveForLater, &QPushButton::clicked, this, &UserPanelWindow::onSaveForLaterClicked);
    connect(btnCheckout, &QPushButton::clicked, this, &UserPanelWindow::onCheckoutClicked);
    connect(btnRemoveFromCart, &QPushButton::clicked, this, &UserPanelWindow::onRemoveFromCartClicked);
    connect(btnRead, &QPushButton::clicked, this, &UserPanelWindow::onReadBookClicked);
    connect(btnRemoveSaved, &QPushButton::clicked, this, &UserPanelWindow::onRemoveSavedClicked);
    connect(btnOpenNotifications, &QPushButton::clicked, this, &UserPanelWindow::onOpenNotificationsClicked);
    connect(btnAddToShelf, &QPushButton::clicked, this, &UserPanelWindow::onAddToShelfFromLibraryClicked);
    connect(comboShelfSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UserPanelWindow::onShelfSelectionChanged);
    connect(btnCreateShelf, &QPushButton::clicked, this, &UserPanelWindow::onCreateShelfClicked);
    connect(btnRenameShelf, &QPushButton::clicked, this, &UserPanelWindow::onRenameShelfClicked);
    connect(btnDeleteShelf, &QPushButton::clicked, this, &UserPanelWindow::onDeleteShelfClicked);
    connect(btnMoveToShelf, &QPushButton::clicked, this, &UserPanelWindow::onMoveBookToShelfClicked);
    connect(btnRemoveFromShelf, &QPushButton::clicked, this, &UserPanelWindow::onRemoveBookFromShelfClicked);
    connect(btnChargeWallet, &QPushButton::clicked, this, &UserPanelWindow::onChargeWalletClicked);
    connect(btnProfile, &QPushButton::clicked, this, &UserPanelWindow::onProfileClicked);
}

// =========================================================================
// درخواست‌های شبکه
// =========================================================================
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

void UserPanelWindow::requestBooksForCurrentView() {
    QJsonObject req;
    switch (comboBookView->currentIndex()) {
    case 1: ClientNetworkManager::getInstance().sendRequest(CommandType::GetSuggestedBooks, req); break;
    case 2: ClientNetworkManager::getInstance().sendRequest(CommandType::GetPopularBooks, req); break;
    case 3: ClientNetworkManager::getInstance().sendRequest(CommandType::GetBestsellingBooks, req); break;
    default: ClientNetworkManager::getInstance().sendRequest(CommandType::GetBooks, req); break;
    }
}

// =========================================================================
// رندر لیست‌ها
// =========================================================================
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
    for (auto &b : myLibraryCache) {
        if (b.getId() == bookId) return &b;
    }
    for (auto &b : savedBooksCache) {
        if (b.getId() == bookId) return &b;
    }
    return nullptr;
}

// =========================================================================
// قفسه‌های شخصی
// =========================================================================
void UserPanelWindow::requestShelves() {
    QJsonObject req;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetShelves, req);
}

void UserPanelWindow::refreshShelfSelector() {
    int previousShelfId = comboShelfSelector->currentData().toInt();

    comboShelfSelector->blockSignals(true);
    comboMoveTargetShelf->blockSignals(true);
    comboShelfSelector->clear();
    comboMoveTargetShelf->clear();

    for (const auto &v : shelvesCache) {
        QJsonObject so = v.toObject();
        comboShelfSelector->addItem(so.value("shelfName").toString(), so.value("shelfId").toInt());
        comboMoveTargetShelf->addItem(so.value("shelfName").toString(), so.value("shelfId").toInt());
    }

    int idx = comboShelfSelector->findData(previousShelfId);
    comboShelfSelector->setCurrentIndex(idx >= 0 ? idx : (comboShelfSelector->count() > 0 ? 0 : -1));
    comboShelfSelector->blockSignals(false);
    comboMoveTargetShelf->blockSignals(false);

    refreshShelfBooksList();
}

void UserPanelWindow::refreshShelfBooksList() {
    listWidgetShelfBooks->clear();
    int shelfId = comboShelfSelector->currentData().toInt();
    if (comboShelfSelector->currentIndex() < 0) return;

    for (const auto &v : shelvesCache) {
        QJsonObject so = v.toObject();
        if (so.value("shelfId").toInt() != shelfId) continue;

        for (const auto &bidVal : so.value("bookIds").toArray()) {
            int bookId = bidVal.toInt();
            QString title = QString("کتابِ شماره‌ی %1").arg(bookId);
            if (Book* b = findCachedBookById(bookId)) {
                title = QString::fromStdString(b->getTitle());
            }
            auto* item = new QListWidgetItem(title);
            item->setData(Qt::UserRole, bookId);
            listWidgetShelfBooks->addItem(item);
        }
        break;
    }
}

void UserPanelWindow::onAddToShelfFromLibraryClicked() {
    auto selected = listWidgetMyLibrary->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "توجه", "ابتدا یک کتاب را از «کتابخانه‌ی من» انتخاب کنید.");
        return;
    }
    if (shelvesCache.isEmpty()) {
        QMessageBox::information(this, "توجه", "ابتدا از تبِ «قفسه‌های من» یک قفسه بسازید.");
        return;
    }
    int bookId = selected.first()->data(Qt::UserRole).toInt();

    QStringList names;
    for (const auto &v : shelvesCache) names << v.toObject().value("shelfName").toString();

    bool ok = false;
    QString chosen = QInputDialog::getItem(this, "افزودن به قفسه", "قفسه را انتخاب کنید:", names, 0, false, &ok);
    if (!ok || chosen.isEmpty()) return;

    int shelfId = -1;
    for (const auto &v : shelvesCache) {
        QJsonObject so = v.toObject();
        if (so.value("shelfName").toString() == chosen) { shelfId = so.value("shelfId").toInt(); break; }
    }
    if (shelfId == -1) return;

    QJsonObject req;
    req["shelfId"] = shelfId;
    req["bookId"] = bookId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::AddBookToShelf, req);
}

void UserPanelWindow::onShelfSelectionChanged(int /*index*/) {
    refreshShelfBooksList();
}

void UserPanelWindow::onCreateShelfClicked() {
    bool ok = false;
    QString name = QInputDialog::getText(this, "قفسه‌ی جدید", "نامِ قفسه:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    QJsonObject req;
    req["shelfName"] = name.trimmed();
    ClientNetworkManager::getInstance().sendRequest(CommandType::CreateShelf, req);
}

void UserPanelWindow::onRenameShelfClicked() {
    if (comboShelfSelector->currentIndex() < 0) {
        QMessageBox::information(this, "توجه", "ابتدا یک قفسه انتخاب کنید.");
        return;
    }
    bool ok = false;
    QString newName = QInputDialog::getText(this, "تغییرِ نامِ قفسه", "نامِ جدید:",
                                            QLineEdit::Normal, comboShelfSelector->currentText(), &ok);
    if (!ok || newName.trimmed().isEmpty()) return;

    QJsonObject req;
    req["shelfId"] = comboShelfSelector->currentData().toInt();
    req["shelfName"] = newName.trimmed();
    ClientNetworkManager::getInstance().sendRequest(CommandType::RenameShelf, req);
}

void UserPanelWindow::onDeleteShelfClicked() {
    if (comboShelfSelector->currentIndex() < 0) {
        QMessageBox::information(this, "توجه", "ابتدا یک قفسه انتخاب کنید.");
        return;
    }
    if (QMessageBox::question(this, "تاییدِ حذف",
                              QString("قفسه‌ی «%1» حذف شود؟").arg(comboShelfSelector->currentText())) != QMessageBox::Yes) {
        return;
    }
    QJsonObject req;
    req["shelfId"] = comboShelfSelector->currentData().toInt();
    ClientNetworkManager::getInstance().sendRequest(CommandType::DeleteShelf, req);
}

void UserPanelWindow::onMoveBookToShelfClicked() {
    auto selected = listWidgetShelfBooks->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "توجه", "ابتدا یک کتاب را از قفسه‌ی جاری انتخاب کنید.");
        return;
    }
    if (comboMoveTargetShelf->currentIndex() < 0) return;

    int fromShelfId = comboShelfSelector->currentData().toInt();
    int toShelfId = comboMoveTargetShelf->currentData().toInt();
    if (fromShelfId == toShelfId) {
        QMessageBox::information(this, "توجه", "قفسه‌ی مقصد باید متفاوت از قفسه‌ی جاری باشد.");
        return;
    }

    QJsonObject req;
    req["fromShelfId"] = fromShelfId;
    req["toShelfId"] = toShelfId;
    req["bookId"] = selected.first()->data(Qt::UserRole).toInt();
    ClientNetworkManager::getInstance().sendRequest(CommandType::MoveBookBetweenShelves, req);
}

void UserPanelWindow::onRemoveBookFromShelfClicked() {
    auto selected = listWidgetShelfBooks->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "توجه", "ابتدا یک کتاب را از قفسه‌ی جاری انتخاب کنید.");
        return;
    }
    QJsonObject req;
    req["shelfId"] = comboShelfSelector->currentData().toInt();
    req["bookId"] = selected.first()->data(Qt::UserRole).toInt();
    ClientNetworkManager::getInstance().sendRequest(CommandType::RemoveBookFromShelf, req);
}

// =========================================================================
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

void UserPanelWindow::onBookViewChanged(int /*index*/) {
    requestBooksForCurrentView();
}

void UserPanelWindow::onViewDetailsClicked() {
    auto* item = listWidgetCatalog->currentItem();
    if (!item) {
        QMessageBox::information(this, "راهنما", "اول یک کتاب را از لیست انتخاب کنید.");
        return;
    }
    openBookDetailsDialog(item->data(Qt::UserRole).toInt());
}

void UserPanelWindow::onCatalogItemDoubleClicked(QListWidgetItem* item) {
    if (!item) return;
    openBookDetailsDialog(item->data(Qt::UserRole).toInt());
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

// =========================================================================
// دیالوگِ جزئیاتِ کتاب: توضیحات + نظرات (با ویرایش/حذفِ نظرِ خودِ کاربر) + امتیاز
// =========================================================================
void UserPanelWindow::openBookDetailsDialog(int bookId) {
    currentDetailsBookId = bookId;

    bookDetailsDialog = new QDialog(this);
    bookDetailsDialog->setWindowTitle("جزئیاتِ کتاب");
    bookDetailsDialog->resize(500, 550);
    auto* layout = new QVBoxLayout(bookDetailsDialog);

    dialogDescriptionLabel = new QLabel();
    dialogDescriptionLabel->setWordWrap(true);
    layout->addWidget(dialogDescriptionLabel);

    auto* ratingRow = new QHBoxLayout();
    ratingRow->addWidget(new QLabel("امتیازِ شما:"));
    dialogRatingCombo = new QComboBox();
    dialogRatingCombo->addItems({"1", "2", "3", "4", "5"});
    dialogSubmitRatingBtn = new QPushButton("ثبتِ امتیاز");
    ratingRow->addWidget(dialogRatingCombo);
    ratingRow->addWidget(dialogSubmitRatingBtn);
    layout->addLayout(ratingRow);

    layout->addWidget(new QLabel("نظرات:"));
    dialogCommentsList = new QListWidget();
    layout->addWidget(dialogCommentsList);

    auto* commentButtonsRow = new QHBoxLayout();
    dialogEditCommentBtn = new QPushButton("ویرایشِ نظرِ انتخابی");
    dialogDeleteCommentBtn = new QPushButton("حذفِ نظرِ انتخابی");
    commentButtonsRow->addWidget(dialogEditCommentBtn);
    commentButtonsRow->addWidget(dialogDeleteCommentBtn);
    layout->addLayout(commentButtonsRow);

    dialogNewCommentText = new QTextEdit();
    dialogNewCommentText->setPlaceholderText("نظرِ خود را بنویسید...");
    dialogNewCommentText->setMaximumHeight(70);
    layout->addWidget(dialogNewCommentText);
    dialogSubmitCommentBtn = new QPushButton("ثبتِ نظر");
    layout->addWidget(dialogSubmitCommentBtn);

    connect(dialogSubmitRatingBtn, &QPushButton::clicked, this, &UserPanelWindow::onSubmitRatingClicked);
    connect(dialogSubmitCommentBtn, &QPushButton::clicked, this, &UserPanelWindow::onSubmitCommentClicked);
    connect(dialogEditCommentBtn, &QPushButton::clicked, this, &UserPanelWindow::onEditSelectedCommentClicked);
    connect(dialogDeleteCommentBtn, &QPushButton::clicked, this, &UserPanelWindow::onDeleteSelectedCommentClicked);

    QJsonObject req;
    req["bookId"] = bookId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::GetBookDetails, req);

    bookDetailsDialog->exec();
}

void UserPanelWindow::refreshDialogCommentsList() {
    if (!dialogCommentsList) return;
    dialogCommentsList->clear();
    for (const auto &v : currentDetailsComments) {
        QJsonObject c = v.toObject();
        bool isMine = (c.value("userId").toInt() == currentUserId);
        bool isPending = !c.value("isApproved").toBool();
        QString text = QString("%1%2%3: %4")
                           .arg(c.value("username").toString())
                           .arg(isMine ? " (شما)" : "")
                           .arg(isPending ? " [در انتظارِ تاییدِ ادمین - فقط شما آن را می‌بینید]" : "")
                           .arg(c.value("text").toString());
        auto* item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, c.value("commentId").toInt());
        item->setData(Qt::UserRole + 1, isMine);
        dialogCommentsList->addItem(item);
    }
}

void UserPanelWindow::onSubmitRatingClicked() {
    if (currentDetailsBookId == -1) return;
    QJsonObject req;
    req["bookId"] = currentDetailsBookId;
    req["score"] = dialogRatingCombo->currentText().toInt();
    ClientNetworkManager::getInstance().sendRequest(CommandType::AddRating, req);
}

void UserPanelWindow::onSubmitCommentClicked() {
    if (currentDetailsBookId == -1) return;
    QString text = dialogNewCommentText->toPlainText().trimmed();
    if (text.isEmpty()) return;

    QJsonObject req;
    req["bookId"] = currentDetailsBookId;
    req["text"] = text;
    ClientNetworkManager::getInstance().sendRequest(CommandType::AddComment, req);
    dialogNewCommentText->clear();
}

void UserPanelWindow::onEditSelectedCommentClicked() {
    auto* item = dialogCommentsList->currentItem();
    if (!item) return;
    if (!item->data(Qt::UserRole + 1).toBool()) {
        QMessageBox::warning(this, "اجازه نیست", "فقط می‌توانید نظرِ خودتان را ویرایش کنید.");
        return;
    }
    int commentId = item->data(Qt::UserRole).toInt();

    bool okPressed = false;
    QString newText = QInputDialog::getMultiLineText(this, "ویرایشِ نظر", "متنِ جدید:", "", &okPressed);
    if (!okPressed || newText.trimmed().isEmpty()) return;

    QJsonObject req;
    req["commentId"] = commentId;
    req["text"] = newText.trimmed();
    ClientNetworkManager::getInstance().sendRequest(CommandType::EditComment, req);
}

void UserPanelWindow::onDeleteSelectedCommentClicked() {
    auto* item = dialogCommentsList->currentItem();
    if (!item) return;
    if (!item->data(Qt::UserRole + 1).toBool()) {
        QMessageBox::warning(this, "اجازه نیست", "فقط می‌توانید نظرِ خودتان را حذف کنید.");
        return;
    }
    if (QMessageBox::question(this, "تایید", "این نظر حذف شود؟") != QMessageBox::Yes) return;

    QJsonObject req;
    req["commentId"] = item->data(Qt::UserRole).toInt();
    ClientNetworkManager::getInstance().sendRequest(CommandType::DeleteComment, req);
}

// =========================================================================
// اسلات‌های سبد خرید
// =========================================================================
void UserPanelWindow::onRemoveFromCartClicked() {
    auto* item = listWidgetCart->currentItem();
    if (!item) return;
    myCart.removeItem(item->data(Qt::UserRole).toInt());
    refreshCartListWidget();
}

void UserPanelWindow::onCheckoutClicked() {
    if (myCart.getItems().empty()) {
        QMessageBox::information(this, "سبد خالی", "سبدِ خریدِ شما خالی است.");
        return;
    }
    for (const auto &item : myCart.getItems()) {
        QJsonObject req;
        req["bookId"] = item.getBook().getId();
        ClientNetworkManager::getInstance().sendRequest(CommandType::BuyBook, req);
    }
    myCart.clearAll();
    refreshCartListWidget();
    QMessageBox::information(this, "تسویه حساب", "درخواستِ خریدِ همه ی کتاب های سبد ارسال شد.");
}

// =========================================================================
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

// =========================================================================
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

// =========================================================================
// شارژِ کیف پول
// =========================================================================
void UserPanelWindow::onChargeWalletClicked() {
    bool okPressed = false;
    double amount = QInputDialog::getDouble(this, "شارژِ کیف پول", "مبلغِ شارژ (تومان):",
                                            10000, 1, 100000000, 0, &okPressed);
    if (!okPressed) return;

    QJsonObject req;
    req["amount"] = amount;
    ClientNetworkManager::getInstance().sendRequest(CommandType::DepositMoney, req);
}

// =========================================================================
// پروفایل: مشاهده/ویرایشِ اطلاعات + تغییرِ رمز عبور
// =========================================================================
void UserPanelWindow::onProfileClicked() {
    openProfileDialog();
}

void UserPanelWindow::openProfileDialog() {
    profileDialog = new QDialog(this);
    profileDialog->setWindowTitle("پروفایلِ من");
    profileDialog->resize(400, 300);
    auto* layout = new QVBoxLayout(profileDialog);

    auto* form = new QFormLayout();
    profileUsernameField = new QLineEdit();
    profileUsernameField->setReadOnly(true); // نام کاربری قابل‌تغییر نیست
    profileEmailField = new QLineEdit();
    form->addRow("نام کاربری:", profileUsernameField);
    form->addRow("ایمیل:", profileEmailField);
    layout->addLayout(form);

    auto* btnSaveProfile = new QPushButton("ذخیره‌ی تغییراتِ ایمیل");
    layout->addWidget(btnSaveProfile);

    auto* btnEditGenres = new QPushButton(" تغییر ژانرهای موردعلاقه");
    layout->addWidget(btnEditGenres);

    connect(btnEditGenres, &QPushButton::clicked, this, [this]() {
        hasPromptedGenres = false;
        promptFavoriteGenresIfNeeded(QJsonArray());
    });

    layout->addWidget(new QLabel("---- تغییرِ رمزِ عبور ----"));
    auto* passForm = new QFormLayout();
    profileOldPasswordField = new QLineEdit();
    profileOldPasswordField->setEchoMode(QLineEdit::Password);
    profileNewPasswordField = new QLineEdit();
    profileNewPasswordField->setEchoMode(QLineEdit::Password);
    passForm->addRow("رمزِ فعلی:", profileOldPasswordField);
    passForm->addRow("رمزِ جدید:", profileNewPasswordField);
    layout->addLayout(passForm);

    auto* btnChangePass = new QPushButton("تغییرِ رمزِ عبور");
    layout->addWidget(btnChangePass);

    connect(btnSaveProfile, &QPushButton::clicked, this, &UserPanelWindow::onSaveProfileClicked);
    connect(btnChangePass, &QPushButton::clicked, this, &UserPanelWindow::onChangePasswordClicked);

    // درخواستِ اطلاعاتِ تازه از سرور (پاسخش توی onNetworkReply پر می‌شه)
    requestProfile();

    profileDialog->exec();
}

void UserPanelWindow::onSaveProfileClicked() {
    QJsonObject req;
    req["email"] = profileEmailField->text();
    ClientNetworkManager::getInstance().sendRequest(CommandType::UpdateProfile, req);
}

void UserPanelWindow::onChangePasswordClicked() {
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

// =========================================================================
// پاسخ‌های شبکه
// =========================================================================
void UserPanelWindow::onNetworkReply(CommandType commandType, QJsonObject payload, bool ok) {
    if (!ok) {
        QMessageBox::warning(this, "خطا", payload.value("error").toString());
        return;
    }

    switch (commandType) {
    case CommandType::GetBooks:
    case CommandType::GetSuggestedBooks:
    case CommandType::GetPopularBooks:
    case CommandType::GetBestsellingBooks: {
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
        // اگه دیالوگِ پروفایل باز باشه، فیلدهاش رو با اطلاعاتِ تازه پر کن
        if (profileDialog && profileUsernameField) {
            profileUsernameField->setText(payload.value("username").toString());
            profileEmailField->setText(payload.value("email").toString());
        }
        // اگه هنوز ژانرِ موردعلاقه انتخاب نکرده (اولین ورود)، دیالوگِ انتخابِ ژانر رو نشون بده
        m_lastSavedGenres = payload.value("favoriteGenres").toArray();
        promptFavoriteGenresIfNeeded(m_lastSavedGenres);
        break;
    }
    case CommandType::UpdateProfile: {
        QMessageBox::information(this, "موفق", "پروفایل بروزرسانی شد.");
        break;
    }
    case CommandType::ChangePassword: {
        QMessageBox::information(this, "موفق", "رمزِ عبور تغییر کرد.");
        if (profileOldPasswordField) profileOldPasswordField->clear();
        if (profileNewPasswordField) profileNewPasswordField->clear();
        break;
    }
    case CommandType::DepositMoney: {
        if (payload.contains("newWalletBalance")) {
            updateWalletDisplay(payload.value("newWalletBalance").toDouble());
            QMessageBox::information(this, "موفق", "کیف‌پول شارژ شد.");
        }
        break;
    }
    case CommandType::SetFavoriteGenres: {
        QMessageBox::information(this, "موفق", "ژانرهای موردعلاقه ذخیره شد.");
        requestBooksForCurrentView();
        break;
    }
    case CommandType::BuyBook: {
        if (payload.contains("newWalletBalance")) {
            updateWalletDisplay(payload.value("newWalletBalance").toDouble());
        }
        requestLibrary();
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
        requestLibrary();
        break;
    }
    case CommandType::GetShelves: {
        shelvesCache = payload.value("shelves").toArray();
        refreshShelfSelector();
        break;
    }
    case CommandType::CreateShelf:
    case CommandType::DeleteShelf:
    case CommandType::RenameShelf:
    case CommandType::AddBookToShelf:
    case CommandType::RemoveBookFromShelf:
    case CommandType::MoveBookBetweenShelves: {
        requestShelves();
        break;
    }
    case CommandType::GetPageLocation: {
        if (pdfReader && payload.contains("pageNum")) {
            int page = payload.value("pageNum").toInt();
            pdfReader->jumpToPage(page);
        }
        break;
    }
    case CommandType::GetBookDetails: {
        currentDetailsComments = payload.value("comments").toArray();
        if (dialogDescriptionLabel) {
            QString text = QString("%1 — %2\n\n%3\n\nمیانگینِ امتیاز: %4")
                               .arg(payload.value("title").toString())
                               .arg(payload.value("author").toString())
                               .arg(payload.value("description").toString())
                               .arg(payload.value("averageRating").toDouble(), 0, 'f', 1);
            dialogDescriptionLabel->setText(text);
        }
        refreshDialogCommentsList();
        break;
    }
    case CommandType::AddComment:
    case CommandType::EditComment:
    case CommandType::DeleteComment: {
        if (currentDetailsBookId != -1) {
            QJsonObject req;
            req["bookId"] = currentDetailsBookId;
            ClientNetworkManager::getInstance().sendRequest(CommandType::GetBookDetails, req);
        }
        break;
    }
    case CommandType::AddRating: {
        if (payload.contains("newAverage")) {
            double newAvg = payload.value("newAverage").toDouble();
            QMessageBox::information(this, "موفق", QString("امتیازِ شما ثبت شد. میانگینِ جدید: %1")
                                                       .arg(newAvg, 0, 'f', 1));

            if (currentDetailsBookId != -1) {
                Book* b = findCachedBookById(currentDetailsBookId);
                if (b) {
                    b->setAverageRating(newAvg); // آپدیت مقدار در حافظه کلاینت
                }
            }

            refreshCatalogListWidget(availableBooksCache);

            // درخواست لیست تازه از سرور (برای همگام‌سازی کامل)
            requestBooksForCurrentView();

            if (bookDetailsDialog && dialogDescriptionLabel && currentDetailsBookId != -1) {
                QJsonObject req;
                req["bookId"] = currentDetailsBookId;
                ClientNetworkManager::getInstance().sendRequest(CommandType::GetBookDetails, req);
            }
        }
        break;
    }
    }
}

// =========================================================================
// دیالوگِ انتخابِ ژانرهای موردعلاقه (اولین ورود)
// =========================================================================
void UserPanelWindow::promptFavoriteGenresIfNeeded(const QJsonArray &currentGenres) {
    // ۱. اگر در این نشست قبلاً دیالوگ نشان داده شده یا کاربر قبلاً ژانرهایی داشته، خارج شو
    if (hasPromptedGenres || !currentGenres.isEmpty()) {
        return;
    }

    // ۲. پرچم را true کن تا در فراخوانی‌های بعدی GetProfile دوباره اجرا نشود
    hasPromptedGenres = true;

    QStringList genreNames = {"داستانی", "غیرداستانی", "علمی‌تخیلی", "فانتزی", "معمایی",
                              "عاشقانه", "تاریخی", "زندگی‌نامه", "خوددرمانی", "فلسفی", "شعر", "کودک","درسی"};

    auto* dialog = new QDialog(this);
    dialog->setWindowTitle("انتخابِ ژانرهای موردعلاقه");
    auto* layout = new QVBoxLayout(dialog);
    layout->addWidget(new QLabel("لطفاً ۱ تا ۳ ژانرِ موردعلاقه‌تان را انتخاب کنید:"));

    auto* list = new QListWidget();
    list->setSelectionMode(QAbstractItemView::MultiSelection);
    for (int i = 0; i < genreNames.size(); ++i) {
        auto* item = new QListWidgetItem(genreNames[i]);
        item->setData(Qt::UserRole, i);

        for (int j = 0; j < currentGenres.size(); ++j) {
            if (currentGenres.at(j).toInt() == i) {
                item->setSelected(true);
                break;
            }
        }

        list->addItem(item);
    }
    layout->addWidget(list);

    auto* btnConfirm = new QPushButton("تأیید");
    layout->addWidget(btnConfirm);

    connect(btnConfirm, &QPushButton::clicked, this, [this, list, dialog]() {
        auto selected = list->selectedItems();
        if (selected.isEmpty() || selected.size() > 3) {
            QMessageBox::warning(dialog, "خطا", "باید بینِ ۱ تا ۳ ژانر انتخاب کنید.");
            return;
        }
        QJsonArray genresArr;
        for (auto* item : selected) genresArr.append(item->data(Qt::UserRole).toInt());

        QJsonObject req;
        req["genres"] = genresArr;
        ClientNetworkManager::getInstance().sendRequest(CommandType::SetFavoriteGenres, req);
        dialog->accept();
    });

    dialog->exec();
}
void UserPanelWindow::openEditGenresDialog() {
    hasPromptedGenres = false;
    promptFavoriteGenresIfNeeded(m_lastSavedGenres);
}