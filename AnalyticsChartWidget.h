#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QtCharts/QChartView>

class AnalyticsChartWidget : public QWidget {
    Q_OBJECT
private:
    QChartView* chartViewContainer;

public:
    explicit AnalyticsChartWidget(QWidget *parent = nullptr);
    ~AnalyticsChartWidget() override = default;

    void drawRevenueBarChart(const QJsonObject &analyticsData);   // فروش هر کتاب (میله‌ای)
    void drawBookSharePieChart(const QJsonObject &analyticsData); // سهمِ هر کتاب از کل فروش (دایره‌ای)
    void drawRatingBarChart(const QJsonObject &analyticsData);    // میانگین امتیاز هر کتاب (میله‌ای)
};