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
#include <QJsonArray>

PublisherPanelWindow::PublisherPanelWindow(int publisherId, QWidget *parent)
    : QMainWindow(parent), currentPublisherId(publisherId) {
    buildUi();

    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::serverReplyReceived,
            this, &PublisherPanelWindow::onNetworkReply);

    requestMyBooks();
}

void PublisherPanelWindow::buildUi() {
    setWindowTitle("BookClub - پنل ناشر");
    resize(950, 650);

    auto* central = new QWidget();
    auto* mainLayout = new QHBoxLayout(central);

    // ---- ستون چپ: جدول کتاب‌های من ----
    auto* leftBox = new QGroupBox("کتاب‌های منتشرشده‌ی من");
    auto* leftLayout = new QVBoxLayout(leftBox);
    tableMyBooks = new QTableWidget(0, 5);
    tableMyBooks->setHorizontalHeaderLabels({"عنوان", "قیمت", "میانگین امتیاز", "فروش", "وضعیت"});
    tableMyBooks->horizontalHeader()->setStretchLastSection(true);
    tableMyBooks->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableMyBooks->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto* tableButtons = new QHBoxLayout();
    btnEditSelected = new QPushButton("ویرایشِ انتخاب‌شده");
    btnDeleteSelected = new QPushButton("حذف/غیرفعال‌کردن");
    btnOpenAnalytics = new QPushButton("📊 آمار و نمودار");
    btnManageDiscounts = new QPushButton("🏷️ مدیریتِ تخفیف");
    tableButtons->addWidget(btnEditSelected);
    tableButtons->addWidget(btnDeleteSelected);
    tableButtons->addWidget(btnOpenAnalytics);
    tableButtons->addWidget(btnManageDiscounts);

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
    comboNewBookGenre->addItem("تاریخی", static_cast<int>(Genre::History));
    txtNewBookDescription = new QTextEdit();
    txtNewBookDescription->setMaximumHeight(80);
    spinNewBookPrice = new QDoubleSpinBox();
    spinNewBookPrice->setRange(0, 10000000);
    spinNewBookPrice->setDecimals(0);
    txtNewBookCoverPath = new QLineEdit();
    txtNewBookCoverPath->setPlaceholderText("مسیر فایلِ عکسِ جلد...");
    txtNewBookPdfPath = new QLineEdit();
    txtNewBookPdfPath->setPlaceholderText("مسیر فایلِ PDF...");
    btnUploadBook = new QPushButton("انتشارِ کتاب");

    rightForm->addRow("عنوان:", txtNewBookTitle);
    rightForm->addRow("نویسنده:", txtNewBookAuthor);
    rightForm->addRow("ژانر:", comboNewBookGenre);
    rightForm->addRow("توضیحات:", txtNewBookDescription);
    rightForm->addRow("قیمت:", spinNewBookPrice);
    rightForm->addRow("عکسِ جلد:", txtNewBookCoverPath);
    rightForm->addRow("فایلِ PDF:", txtNewBookPdfPath);
    rightForm->addRow(btnUploadBook);

    mainLayout->addWidget(leftBox, /*stretch=*/2);
    mainLayout->addWidget(rightBox, /*stretch=*/1);

    setCentralWidget(central);

    connect(btnUploadBook, &QPushButton::clicked, this, &PublisherPanelWindow::handleUploadSubmit);
    connect(btnEditSelected, &QPushButton::clicked, this, &PublisherPanelWindow::handleEditSubmit);
    connect(btnDeleteSelected, &QPushButton::clicked, this, &PublisherPanelWindow::handleDeleteSubmit);
    connect(btnOpenAnalytics, &QPushButton::clicked, this, &PublisherPanelWindow::openAnalyticsWindow);
    connect(btnManageDiscounts, &QPushButton::clicked, this, &PublisherPanelWindow::openDiscountWindow);
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
        tableMyBooks->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getTitle())));
        tableMyBooks->setItem(row, 1, new QTableWidgetItem(QString::number(b.getBasePrice())));
        tableMyBooks->setItem(row, 2, new QTableWidgetItem(QString::number(b.getAverageRating(), 'f', 1)));
        tableMyBooks->setItem(row, 3, new QTableWidgetItem("-")); // تعدادِ فروش را از پاسخِ آمار جدا نگه می‌داریم
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
    if (selected.isEmpty()) return;
    int bookId = tableMyBooks->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    QJsonObject req;
    req["bookId"] = bookId;
    if (!txtNewBookTitle->text().isEmpty()) req["title"] = txtNewBookTitle->text();
    if (!txtNewBookAuthor->text().isEmpty()) req["author"] = txtNewBookAuthor->text();
    if (!txtNewBookDescription->toPlainText().isEmpty()) req["description"] = txtNewBookDescription->toPlainText();
    if (spinNewBookPrice->value() > 0) req["basePrice"] = spinNewBookPrice->value();
    ClientNetworkManager::getInstance().sendRequest(CommandType::EditBook, req);
}

void PublisherPanelWindow::handleDeleteSubmit() {
    auto selected = tableMyBooks->selectedItems();
    if (selected.isEmpty()) return;
    int bookId = tableMyBooks->item(selected.first()->row(), 0)->data(Qt::UserRole).toInt();

    if (QMessageBox::question(this, "تایید حذف", "این کتاب حذف/غیرفعال شود؟") != QMessageBox::Yes) return;

    QJsonObject req;
    req["bookId"] = bookId;
    ClientNetworkManager::getInstance().sendRequest(CommandType::DeleteBook, req);
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

void PublisherPanelWindow::onNetworkReply(CommandType commandType, QJsonObject payload, bool ok) {
    if (!ok) {
        QMessageBox::warning(this, "خطا", payload.value("error").toString());
        return;
    }

    if (commandType == CommandType::GetPublisherAnalytics) {
        myBooksCache.clear();
        for (const auto &v : payload.value("books").toArray()) {
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
}
