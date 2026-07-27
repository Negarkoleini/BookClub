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
#include<QJsonArray>
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
    // ---- بالای پنجره ----
    QLabel* lblBalance;
    QLineEdit* txtSearch;
    QComboBox* comboGenreFilter;
    QComboBox* comboBookView; // همه / پیشنهادی / محبوب / پرفروش
    QPushButton* btnOpenNotifications;

    // ---- تب فروشگاه ----
    QListWidget* listWidgetCatalog;
    QPushButton* btnBuy;
    QPushButton* btnAddToCart;
    QPushButton* btnSaveForLater;

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
    QJsonArray currentDetailsComments; // برای پیداکردنِ userId هر نظر موقعِ ویرایش/حذف

    // ---- تب سبدخرید ----
    QListWidget* listWidgetCart;
    QLabel* lblCartTotal;
    QPushButton* btnCheckout;
    QPushButton* btnRemoveFromCart;

    // ---- تب کتابخانه من ----
    QListWidget* listWidgetMyLibrary;
    QPushButton* btnRead;

    // ---- تب ذخیره‌شده‌ها ----
    QListWidget* listWidgetSaved;
    QPushButton* btnRemoveSaved;

    int currentUserId;
    BookSearchEngine searchEngine;
    ShoppingCart myCart;
    QVector<Book> availableBooksCache;
    QVector<Book> myLibraryCache;
    QVector<Book> savedBooksCache;

    PdfReaderWidget* pdfReader = nullptr;
    NotificationCenterWidget* notificationCenter = nullptr;

    void buildUi();
    void requestCatalog();
    void requestLibrary();
    void requestProfile();
    void refreshCatalogListWidget(const QVector<Book> &books);
    void refreshCartListWidget();
    Book* findCachedBookById(int bookId);

    // ----  ژانرِ موردعلاقه (اولین ورود) و جزئیاتِ کتاب/نظرات ----
    void promptFavoriteGenresIfNeeded(const QJsonArray &currentGenres);
    void openBookDetailsDialog(int bookId);
    void refreshDialogCommentsList();
    void requestBooksForCurrentView();

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

    void onBookViewChanged(int index);
    void onCatalogItemDoubleClicked(QListWidgetItem* item);
    void onSubmitCommentClicked();
    void onSubmitRatingClicked();
    void onEditSelectedCommentClicked();
    void onDeleteSelectedCommentClicked();

    void updateWalletDisplay(double currentBalance);
    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);
    void onPushNotification(QJsonObject payload);
};

#endif // USERPANELWINDOW_H



