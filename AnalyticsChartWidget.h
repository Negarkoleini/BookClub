#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QtCharts/QChartView>

// -----------------------------------------------------------------------
// AnalyticsChartWidget: نمودارهای آمار فروش/عملکردِ ناشر با Qt Charts
// (بخش امتیازی ۴). ورودی‌اش مستقیماً همان QJsonObject ای است که
// RequestProcessor::processGetPublisherAnalytics برمی‌گرداند؛ چون کلاینت
// دیگر یک AnalyticsData کاملِ سمتِ سرور نمی‌سازد (آن کلاس سمتِ سرور برای
// نگهداریِ آمار در Publisher استفاده می‌شود، نه برای انتقالِ شبکه‌ای).
// -----------------------------------------------------------------------
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