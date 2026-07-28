#ifndef USERPANELWINDOW_H
#define USERPANELWINDOW_H
#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QDialog>
#include <QTextEdit>
#include <QPixmap>
#include "BookSearchEngine.h"
#include "PdfReaderWidget.h"
#include "InAppNotificationWidget.h"
#include "NotificationCenterWidget.h"
#include "Book.h"
#include "ShoppingCart.h"
#include "CommandType.h"


struct PurchaseHistoryEntry {
    int bookId = -1;
    QString purchasedAt;
};

class UserPanelWindow : public QMainWindow {
    Q_OBJECT
private:
    bool hasPromptedGenres = false;
    QJsonArray m_lastSavedGenres;
    // ---- بالای پنجره ----
    QLabel* lblBalance;
    QLineEdit* txtSearch;
    QComboBox* comboGenreFilter;
    QPushButton* btnChargeWallet;
    QPushButton* btnPurchaseHistory;
    QPushButton* btnProfile;
    QPushButton* btnLogout;
    QPushButton* btnOpenNotifications;

    // ---- تب فروشگاه ----
    QListWidget* listWidgetCatalog;
    QPushButton* btnBuy;
    QPushButton* btnAddToCart;
    QPushButton* btnSaveForLater;
    QPushButton* btnViewDetails; // جایگزینِ دابل‌کلیک -- تا دیده بشه و کاربر بفهمه هست

    // ---- تب سبدخرید ----
    QListWidget* listWidgetCart;
    QLabel* lblCartTotal;
    QPushButton* btnCheckout;
    QPushButton* btnRemoveFromCart;

    // ---- تب کتابخانه من ----
    QListWidget* listWidgetMyLibrary;
    QPushButton* btnRead;
    QPushButton* btnAddToShelf;

    // ---- تب ذخیره‌شده‌ها ----
    QListWidget* listWidgetSaved;
    QPushButton* btnRemoveSaved;
    QPushButton* btnReadSaved;

    // ---- تب قفسه‌های شخصی ----
    QComboBox* comboShelfSelector;
    QPushButton* btnCreateShelf;
    QPushButton* btnRenameShelf;
    QPushButton* btnDeleteShelf;
    QListWidget* listWidgetShelfBooks;
    QComboBox* comboMoveTargetShelf;
    QPushButton* btnMoveToShelf;
    QPushButton* btnRemoveFromShelf;
    QJsonArray shelvesCache; // آخرین پاسخِ GetShelves: [{shelfId, shelfName, bookIds:[...]}]

    int currentUserId;
    int selectedCatalogBookId = -1;
    BookSearchEngine searchEngine;
    ShoppingCart myCart;
    QVector<Book> availableBooksCache;
    QVector<Book> suggestedBooksCache;
    QVector<Book> popularBooksCache;
    QVector<Book> bestsellingBooksCache;
    QVector<Book> freeBooksCache;
    QVector<Book> newestBooksCache;
    QVector<Book> myLibraryCache;
    QVector<Book> savedBooksCache;
    QVector<PurchaseHistoryEntry> purchaseHistoryCache;

    PdfReaderWidget* pdfReader = nullptr;
    NotificationCenterWidget* notificationCenter = nullptr;

    // ---- دیالوگِ جزئیاتِ کتاب (نظرات + امتیاز) ----
    QDialog* bookDetailsDialog = nullptr;
    QLabel* dialogDescriptionLabel = nullptr;
    QLabel* dialogCoverLabel = nullptr;
    QListWidget* dialogCommentsList = nullptr;
    QTextEdit* dialogNewCommentText = nullptr;
    QComboBox* dialogRatingCombo = nullptr;
    QPushButton* dialogSubmitCommentBtn = nullptr;
    QPushButton* dialogSubmitRatingBtn = nullptr;
    QPushButton* dialogEditCommentBtn = nullptr;
    QPushButton* dialogDeleteCommentBtn = nullptr;
    int currentDetailsBookId = -1;
    QJsonArray currentDetailsComments;

    // ---- دیالوگِ پروفایل ----
    QDialog* profileDialog = nullptr;
    QDialog* purchaseHistoryDialog = nullptr;
    QListWidget* purchaseHistoryDialogList = nullptr;
    QLineEdit* profileUsernameField = nullptr;
    QLineEdit* profileEmailField = nullptr;
    QLineEdit* profileOldPasswordField = nullptr;
    QLineEdit* profileNewPasswordField = nullptr;
    QListWidget* profilePurchaseHistoryList = nullptr;

    void buildUi();
    void requestCatalog();
    void requestLibrary();
    void requestProfile();
    void refreshCatalogListWidget(const QVector<Book> &books);
    void refreshCartListWidget();
    void refreshProfileHistoryList();
    QVector<Book> getBooksForCurrentViewFromCache() const;
    void applyCurrentBookViewFilter();
    Book* findCachedBookById(int bookId);
    QPixmap loadCoverOrPlaceholder(const std::string &coverPath, const std::string &title, const QSize &size);

    // ---- قفسه‌های شخصی ----
    void requestShelves();
    void refreshShelfSelector();
    void refreshShelfBooksList();

    void promptFavoriteGenresIfNeeded(const QJsonArray &currentGenres);
    void openBookDetailsDialog(int bookId);
    void refreshDialogCommentsList();
    void requestBooksForCurrentView();
    void openProfileDialog();

public:
    explicit UserPanelWindow(int userId, QWidget *parent = nullptr);
    ~UserPanelWindow() override = default;

private slots:
    void onSearchTextChanged(const QString &text);
    void onGenreFilterChanged(int index);
    void onBuyBookClicked();
    void onAddToCartClicked();
    void onSaveForLaterClicked();
    void onCheckoutClicked();
    void onRemoveFromCartClicked();
    void onReadBookClicked();
    void onReadSavedBookClicked();
    void onRemoveSavedClicked();
    void onOpenNotificationsClicked();

    // ---- قفسه‌های شخصی ----
    void onAddToShelfFromLibraryClicked();
    void onShelfSelectionChanged(int index);
    void onCreateShelfClicked();
    void onRenameShelfClicked();
    void onDeleteShelfClicked();
    void onMoveBookToShelfClicked();
    void onRemoveBookFromShelfClicked();

    void onViewDetailsClicked();
    void onCatalogItemDoubleClicked(QListWidgetItem* item);
    void onSubmitCommentClicked();
    void onSubmitRatingClicked();
    void onEditSelectedCommentClicked();
    void onDeleteSelectedCommentClicked();

    void onChargeWalletClicked();
    void onPurchaseHistoryClicked();
    void onProfileClicked();
    void onLogoutClicked();
    void onSaveProfileClicked();
    void onChangePasswordClicked();

    void updateWalletDisplay(double currentBalance);
    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);
    void onPushNotification(QJsonObject payload);
    void openEditGenresDialog();

signals:
    void logoutRequested();
};

#endif // USERPANELWINDOW_H





