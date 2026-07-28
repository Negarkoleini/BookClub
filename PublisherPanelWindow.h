#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QVector>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include "Book.h"
#include "CommandType.h"
#include "AnalyticsChartWidget.h"
#include "DiscountManagerWidget.h"
#include "NotificationCenterWidget.h"
#include "InAppNotificationWidget.h"


class PublisherPanelWindow : public QMainWindow {
    Q_OBJECT
private:
    int currentPublisherId;

    QTableWidget* tableMyBooks;
    QLabel* lblMyBooksHeader;
    QPushButton* btnOpenNotifications;
    NotificationCenterWidget* notificationCenter = nullptr;

    // فرمِ افزودنِ کتابِ جدید
    QLineEdit* txtNewBookTitle;
    QLineEdit* txtNewBookAuthor;
    QComboBox* comboNewBookGenre;
    QTextEdit* txtNewBookDescription;
    QDoubleSpinBox* spinNewBookPrice;
    QLineEdit* txtNewBookCoverPath;
    QLineEdit* txtNewBookPdfPath;
    QPushButton* btnBrowseCoverPath;
    QPushButton* btnBrowsePdfPath;
    QPushButton* btnUploadBook;

    QPushButton* btnEditSelected;
    QPushButton* btnDeleteSelected;
    QPushButton* btnOpenAnalytics;
    QPushButton* btnManageDiscounts;
    QPushButton* btnViewBookDetails;

    QVector<Book> myBooksCache;
    QJsonArray myBooksAnalyticsCache; // اطلاعاتِ خامِ آمار (قیمت، امتیاز، فروش) برای نمایشِ دقیق در جدول
    AnalyticsChartWidget* chartWidget = nullptr;
    DiscountManagerWidget* discountWidget = nullptr;

    void buildUi();
    void requestMyBooks();
    void refreshTable();

public:
    explicit PublisherPanelWindow(int publisherId, QWidget *parent = nullptr);
    ~PublisherPanelWindow() override = default;

private slots:
    void loadPublisherBooks();
    void handleUploadSubmit();
    void handleEditSubmit();
    void handleDeleteSubmit();
    void openAnalyticsWindow();
    void openDiscountWindow();
    void onOpenNotificationsClicked();
    void onPushNotification(QJsonObject payload);
    void onViewBookDetailsClicked();
    void onBrowseCoverPath();
    void onBrowsePdfPath();
    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);
};