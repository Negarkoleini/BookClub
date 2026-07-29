#include "AnalyticsChartWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QJsonArray>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>
#include <QtCharts/QLegend>
#include <algorithm>
#include <QHeaderView>

namespace {
// عنوان‌های طولانی روی محورِ نمودار با هم قاطی می‌شدند؛ برای خوانا ماندنِ محور، عنوان‌های
// بلند را کوتاه می‌کنیم (برچسبِ چرخانده‌شده هم در drawRevenueBarChart/drawRatingBarChart تنظیم شده).
QString shortenTitle(const QString &title) {
    constexpr int maxLen = 14;
    if (title.size() <= maxLen) return title;
    return title.left(maxLen) + "...";
}
}

AnalyticsChartWidget::AnalyticsChartWidget(QWidget *parent) : QWidget(parent) {
    setWindowFlag(Qt::Window);

    revenueChartView = new QChartView(new QChart());
    shareChartView = new QChartView(new QChart());
    ratingChartView = new QChartView(new QChart());
    revenueChartView->setRenderHint(QPainter::Antialiasing);
    shareChartView->setRenderHint(QPainter::Antialiasing);
    ratingChartView->setRenderHint(QPainter::Antialiasing);

    lblTotalRevenue = new QLabel("0 تومان");
    lblTotalBooks = new QLabel("0");
    lblTotalSales = new QLabel("0");
    for (QLabel* lbl : {lblTotalRevenue, lblTotalBooks, lblTotalSales}) {
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("font-size: 16px; font-weight: bold;");
    }

    tableAverageRatings = new QTableWidget();
    tableTopBooks = new QTableWidget();
    tableLeastBooks = new QTableWidget();

    // ---- لایه‌ی بیرونی: یک ScrollArea تا با هر تعداد کتاب، محتوا از پنجره بیرون نزند/بریده نشود ----
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* scrollContent = new QWidget();
    auto* layout = new QVBoxLayout(scrollContent);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(12);

    // ---- ردیفِ خلاصه‌ی آمار: سه کارتِ کنارِ هم ----
    auto makeStatCard = [](const QString &title, QLabel* valueLabel) {
        auto* box = new QGroupBox(title);
        auto* lay = new QVBoxLayout(box);
        lay->addWidget(valueLabel);
        box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        return box;
    };
    auto* summaryRow = new QHBoxLayout();
    summaryRow->setSpacing(12);
    summaryRow->addWidget(makeStatCard("کل درآمد", lblTotalRevenue));
    summaryRow->addWidget(makeStatCard("تعداد کل کتاب‌های منتشرشده", lblTotalBooks));
    summaryRow->addWidget(makeStatCard("تعداد کل فروش", lblTotalSales));
    layout->addLayout(summaryRow);

    // ---- ردیفِ جدول‌ها: امتیازها | پرفروش‌ترین‌ها | کم‌فروش‌ترین‌ها، هر سه هم‌عرض ----
    // نکته: عنوانِ توکارِ QGroupBox در این استایل درست رندر نمی‌شد و پشتِ جدول می‌رفت،
    // برای همین به‌جای تکیه بر آن، یک QLabel صریح بالای هر جدول قرار می‌گیرد.
    auto* tablesGrid = new QGridLayout();
    tablesGrid->setSpacing(12);

    auto makeTableTitle = [](const QString &text) {
        auto* lbl = new QLabel(text);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("font-size: 14px; font-weight: bold; padding: 4px;");
        return lbl;
    };

    auto* ratingsBox = new QGroupBox();
    auto* ratingsLayout = new QVBoxLayout(ratingsBox);
    setupTable(tableAverageRatings, {"کتاب", "میانگین امتیاز"});
    ratingsLayout->addWidget(makeTableTitle("میانگین امتیاز کتاب‌ها"));
    ratingsLayout->addWidget(tableAverageRatings);

    auto* topBox = new QGroupBox();
    auto* topLayout = new QVBoxLayout(topBox);
    setupTable(tableTopBooks, {"کتاب", "فروش", "درآمد"});
    topLayout->addWidget(makeTableTitle("۵ کتاب پرفروش"));
    topLayout->addWidget(tableTopBooks);

    auto* leastBox = new QGroupBox();
    auto* leastLayout = new QVBoxLayout(leastBox);
    setupTable(tableLeastBooks, {"کتاب", "فروش", "درآمد"});
    leastLayout->addWidget(makeTableTitle("۵ کتاب کم‌فروش"));
    leastLayout->addWidget(tableLeastBooks);

    for (QGroupBox* box : {ratingsBox, topBox, leastBox}) {
        box->setMinimumHeight(220);
        box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    tablesGrid->addWidget(ratingsBox, 0, 0);
    tablesGrid->addWidget(topBox, 0, 1);
    tablesGrid->addWidget(leastBox, 0, 2);
    tablesGrid->setColumnStretch(0, 1);
    tablesGrid->setColumnStretch(1, 1);
    tablesGrid->setColumnStretch(2, 1);
    layout->addLayout(tablesGrid);

    // ---- ردیفِ نمودارها: هر سه نمودار داخلِ تب، به‌جای این‌که فقط یکی نمایش داده شود ----
    chartTabs = new QTabWidget();
    chartTabs->addTab(revenueChartView, "فروش هر کتاب");
    chartTabs->addTab(shareChartView, "سهم از درآمد");
    chartTabs->addTab(ratingChartView, "میانگین امتیاز");
    chartTabs->setMinimumHeight(320);
    chartTabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(chartTabs, /*stretch=*/1);

    scrollArea->setWidget(scrollContent);
    outerLayout->addWidget(scrollArea);

    resize(820, 640);
}

void AnalyticsChartWidget::setupTable(QTableWidget* table, const QStringList &headers) {
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    table->verticalHeader()->setVisible(false);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    table->setRowCount(0);
}

void AnalyticsChartWidget::populatePublisherDashboard(const QJsonObject &analyticsData) {
    lblTotalRevenue->setText(QString::number(analyticsData.value("totalRevenue").toDouble(), 'f', 0) + " تومان");
    lblTotalBooks->setText(QString::number(analyticsData.value("totalBooks").toInt()));
    lblTotalSales->setText(QString::number(analyticsData.value("totalSales").toInt()));

    QVector<QJsonObject> books;
    for (const QJsonValue &v : analyticsData.value("books").toArray()) {
        books.push_back(v.toObject());
    }

    std::sort(books.begin(), books.end(), [](const QJsonObject &a, const QJsonObject &b) {
        return a.value("salesCount").toInt() > b.value("salesCount").toInt();
    });

    tableAverageRatings->setRowCount(books.size());
    for (int row = 0; row < books.size(); ++row) {
        const QJsonObject &bookObj = books[row];
        tableAverageRatings->setItem(row, 0, new QTableWidgetItem(bookObj.value("title").toString()));
        tableAverageRatings->setItem(row, 1, new QTableWidgetItem(QString::number(bookObj.value("averageRating").toDouble(), 'f', 1) + " / ۵"));
    }

    int topCount = std::min(5, static_cast<int>(books.size()));
    tableTopBooks->setRowCount(topCount);
    for (int row = 0; row < topCount; ++row) {
        const QJsonObject &bookObj = books[row];
        tableTopBooks->setItem(row, 0, new QTableWidgetItem(bookObj.value("title").toString()));
        tableTopBooks->setItem(row, 1, new QTableWidgetItem(QString::number(bookObj.value("salesCount").toInt())));
        tableTopBooks->setItem(row, 2, new QTableWidgetItem(QString::number(bookObj.value("revenue").toDouble(), 'f', 0) + " تومان"));
    }

    QVector<QJsonObject> leastBooks = books;
    std::sort(leastBooks.begin(), leastBooks.end(), [](const QJsonObject &a, const QJsonObject &b) {
        return a.value("salesCount").toInt() < b.value("salesCount").toInt();
    });
    int leastCount = std::min(5, static_cast<int>(leastBooks.size()));
    tableLeastBooks->setRowCount(leastCount);
    for (int row = 0; row < leastCount; ++row) {
        const QJsonObject &bookObj = leastBooks[row];
        tableLeastBooks->setItem(row, 0, new QTableWidgetItem(bookObj.value("title").toString()));
        tableLeastBooks->setItem(row, 1, new QTableWidgetItem(QString::number(bookObj.value("salesCount").toInt())));
        tableLeastBooks->setItem(row, 2, new QTableWidgetItem(QString::number(bookObj.value("revenue").toDouble(), 'f', 0) + " تومان"));
    }

    // هر سه نمودار به‌روزرسانی می‌شوند تا با تغییرِ تب، اطلاعاتِ آن هم‌زمان و درست نمایش داده شود
    drawRevenueBarChart(analyticsData);
    drawBookSharePieChart(analyticsData);
    drawRatingBarChart(analyticsData);
}

void AnalyticsChartWidget::drawRevenueBarChart(const QJsonObject &analyticsData) {
    auto* set = new QBarSet("فروش (تعداد)");
    QStringList categories;

    for (const QJsonValue &v : analyticsData.value("books").toArray()) {
        QJsonObject bookObj = v.toObject();
        categories << shortenTitle(bookObj.value("title").toString());
        *set << bookObj.value("salesCount").toInt();
    }

    auto* series = new QBarSeries();
    series->append(set);

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("تعداد فروش هر کتاب");
    chart->legend()->setVisible(false);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsAngle(-45); // برای جلوگیری از روی‌همافتادنِ عنوان‌های کتاب وقتی تعدادشان زیاد است
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    revenueChartView->setChart(chart);
}

void AnalyticsChartWidget::drawBookSharePieChart(const QJsonObject &analyticsData) {
    auto* series = new QPieSeries();
    for (const QJsonValue &v : analyticsData.value("books").toArray()) {
        QJsonObject bookObj = v.toObject();
        double revenue = bookObj.value("revenue").toDouble();
        if (revenue > 0) {
            series->append(shortenTitle(bookObj.value("title").toString()), revenue);
        }
    }
    for (auto* slice : series->slices()) {
        slice->setLabelVisible(true);
    }

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("سهمِ هر کتاب از کل درآمد");
    chart->legend()->setAlignment(Qt::AlignRight);

    shareChartView->setChart(chart);
}

void AnalyticsChartWidget::drawRatingBarChart(const QJsonObject &analyticsData) {
    auto* set = new QBarSet("میانگین امتیاز");
    QStringList categories;

    for (const QJsonValue &v : analyticsData.value("books").toArray()) {
        QJsonObject bookObj = v.toObject();
        categories << shortenTitle(bookObj.value("title").toString());
        *set << bookObj.value("averageRating").toDouble();
    }

    auto* series = new QBarSeries();
    series->append(set);

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("میانگین امتیازِ هر کتاب (از ۵)");
    chart->legend()->setVisible(false);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsAngle(-45);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    axisY->setRange(0, 5);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ratingChartView->setChart(chart);
}