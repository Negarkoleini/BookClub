#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QVector>
#include <QJsonObject>
#include "Book.h"
#include "CommandType.h"
#include "AnalyticsChartWidget.h"
#include "DiscountManagerWidget.h"


class PublisherPanelWindow : public QMainWindow {
    Q_OBJECT
private:
    int currentPublisherId;

    QTableWidget* tableMyBooks;

    // فرمِ افزودنِ کتابِ جدید
    QLineEdit* txtNewBookTitle;
    QLineEdit* txtNewBookAuthor;
    QComboBox* comboNewBookGenre;
    QTextEdit* txtNewBookDescription;
    QDoubleSpinBox* spinNewBookPrice;
    QLineEdit* txtNewBookCoverPath;
    QLineEdit* txtNewBookPdfPath;
    QPushButton* btnUploadBook;

    QPushButton* btnEditSelected;
    QPushButton* btnDeleteSelected;
    QPushButton* btnOpenAnalytics;
    QPushButton* btnManageDiscounts;

    QVector<Book> myBooksCache;
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
    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);
};