#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QLabel>
#include <QTableWidget>
#include <QTabWidget>
#include <QtCharts/QChartView>

class AnalyticsChartWidget : public QWidget {
    Q_OBJECT
private:
    QTabWidget* chartTabs;
    QChartView* revenueChartView;   // فروش هر کتاب (میله‌ای)
    QChartView* shareChartView;     // سهمِ هر کتاب از کل فروش (دایره‌ای)
    QChartView* ratingChartView;    // میانگین امتیاز هر کتاب (میله‌ای)

    QLabel* lblTotalRevenue;
    QLabel* lblTotalBooks;
    QLabel* lblTotalSales;
    QTableWidget* tableAverageRatings;
    QTableWidget* tableTopBooks;
    QTableWidget* tableLeastBooks;

    void setupTable(QTableWidget* table, const QStringList &headers);

public:
    explicit AnalyticsChartWidget(QWidget *parent = nullptr);
    ~AnalyticsChartWidget() override = default;

    void populatePublisherDashboard(const QJsonObject &analyticsData);
    void drawRevenueBarChart(const QJsonObject &analyticsData);   // فروش هر کتاب (میله‌ای)
    void drawBookSharePieChart(const QJsonObject &analyticsData); // سهمِ هر کتاب از کل فروش (دایره‌ای)
    void drawRatingBarChart(const QJsonObject &analyticsData);    // میانگین امتیاز هر کتاب (میله‌ای)
};
