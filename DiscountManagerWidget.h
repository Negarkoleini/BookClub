#pragma once
#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QVector>
#include "Book.h"

// -----------------------------------------------------------------------
// DiscountManagerWidget: فرمِ گرافیکیِ اعمالِ تخفیفِ زمان‌دار روی یکی از
// کتاب‌های خودِ ناشر. اطلاعاتِ فرم را به QJsonObject تبدیل و با
// CommandType::ApplyDiscount به سرور می‌فرستد.
// -----------------------------------------------------------------------
class DiscountManagerWidget : public QWidget {
    Q_OBJECT
private:
    QComboBox* comboBooks;
    QComboBox* comboDiscountType;
    QDoubleSpinBox* spinDiscountValue;
    QDateTimeEdit* dateTimeStart;
    QDateTimeEdit* dateTimeEnd;
    QPushButton* btnSubmitDiscount;

    QVector<Book> myBooks; // برای پرکردنِ comboBooks؛ از بیرون (PublisherPanelWindow) ست می‌شود

public:
    explicit DiscountManagerWidget(QWidget *parent = nullptr);
    ~DiscountManagerWidget() override = default;

    void setAvailableBooks(const QVector<Book> &books);

private slots:
    void sendDiscountToServer();
};