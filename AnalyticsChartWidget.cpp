#include "AnalyticsChartWidget.h"
#include <QVBoxLayout>
#include <QJsonArray>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QLegend>

AnalyticsChartWidget::AnalyticsChartWidget(QWidget *parent) : QWidget(parent) {
    chartViewContainer = new QChartView(new QChart(), this);
    chartViewContainer->setRenderHint(QPainter::Antialiasing);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(chartViewContainer);
}

void AnalyticsChartWidget::drawRevenueBarChart(const QJsonObject &analyticsData) {
    auto* set = new QBarSet("فروش (تعداد)");
    QStringList categories;

    for (const QJsonValue &v : analyticsData.value("books").toArray()) {
        QJsonObject bookObj = v.toObject();
        categories << bookObj.value("title").toString();
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
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartViewContainer->setChart(chart);
}

void AnalyticsChartWidget::drawBookSharePieChart(const QJsonObject &analyticsData) {
    auto* series = new QPieSeries();
    for (const QJsonValue &v : analyticsData.value("books").toArray()) {
        QJsonObject bookObj = v.toObject();
        double revenue = bookObj.value("revenue").toDouble();
        if (revenue > 0) {
            series->append(bookObj.value("title").toString(), revenue);
        }
    }
    if (series->count() > 0) {
        series->slices().first()->setLabelVisible(true);
    }
    for (auto* slice : series->slices()) {
        slice->setLabelVisible(true);
    }

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("سهمِ هر کتاب از کل درآمد");

    chartViewContainer->setChart(chart);
}

void AnalyticsChartWidget::drawRatingBarChart(const QJsonObject &analyticsData) {
    auto* set = new QBarSet("میانگین امتیاز");
    QStringList categories;

    for (const QJsonValue &v : analyticsData.value("books").toArray()) {
        QJsonObject bookObj = v.toObject();
        categories << bookObj.value("title").toString();
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
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    axisY->setRange(0, 5);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartViewContainer->setChart(chart);
}
