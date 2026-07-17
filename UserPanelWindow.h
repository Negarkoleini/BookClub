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
    QPushButton* btnOpenNotifications;

    // ---- تب فروشگاه ----
    QListWidget* listWidgetCatalog;
    QPushButton* btnBuy;
    QPushButton* btnAddToCart;
    QPushButton* btnSaveForLater;

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

    void updateWalletDisplay(double currentBalance);
    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);
    void onPushNotification(QJsonObject payload);
};

#endif // USERPANELWINDOW_H
