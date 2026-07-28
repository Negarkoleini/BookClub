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
#include "BookSearchEngine.h"
#include "PdfReaderWidget.h"
#include "InAppNotificationWidget.h"
#include "NotificationCenterWidget.h"
#include "Book.h"
#include "ShoppingCart.h"
#include "CommandType.h"


class UserPanelWindow : public QMainWindow {
    Q_OBJECT
private:
    bool hasPromptedGenres = false;
    QJsonArray m_lastSavedGenres;
    // ---- بالای پنجره ----
    QLabel* lblBalance;
    QLineEdit* txtSearch;
    QComboBox* comboGenreFilter;
    QComboBox* comboBookView; // همه / پیشنهادی / محبوب / پرفروش
    QPushButton* btnChargeWallet;
    QPushButton* btnProfile;
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
    BookSearchEngine searchEngine;
    ShoppingCart myCart;
    QVector<Book> availableBooksCache;
    QVector<Book> myLibraryCache;
    QVector<Book> savedBooksCache;

    PdfReaderWidget* pdfReader = nullptr;
    NotificationCenterWidget* notificationCenter = nullptr;

    // ---- دیالوگِ جزئیاتِ کتاب (نظرات + امتیاز) ----
    QDialog* bookDetailsDialog = nullptr;
    QLabel* dialogDescriptionLabel = nullptr;
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
    QLineEdit* profileUsernameField = nullptr;
    QLineEdit* profileEmailField = nullptr;
    QLineEdit* profileOldPasswordField = nullptr;
    QLineEdit* profileNewPasswordField = nullptr;

    void buildUi();
    void requestCatalog();
    void requestLibrary();
    void requestProfile();
    void refreshCatalogListWidget(const QVector<Book> &books);
    void refreshCartListWidget();
    Book* findCachedBookById(int bookId);

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

    void onBookViewChanged(int index);
    void onViewDetailsClicked();
    void onCatalogItemDoubleClicked(QListWidgetItem* item);
    void onSubmitCommentClicked();
    void onSubmitRatingClicked();
    void onEditSelectedCommentClicked();
    void onDeleteSelectedCommentClicked();

    void onChargeWalletClicked();
    void onProfileClicked();
    void onSaveProfileClicked();
    void onChangePasswordClicked();

    void updateWalletDisplay(double currentBalance);
    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);
    void onPushNotification(QJsonObject payload);
    void openEditGenresDialog();
};

#endif // USERPANELWINDOW_H





