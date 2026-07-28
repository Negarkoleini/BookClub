#pragma once
#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QVector>
#include "Book.h"
#include <QJsonObject>
#include "CommandType.h"


class DiscountManagerWidget : public QWidget {
    Q_OBJECT
private:
    QComboBox* comboBooks;
    QComboBox* comboDiscountType;
    QDoubleSpinBox* spinDiscountValue;
    QDateTimeEdit* dateTimeStart;
    QDateTimeEdit* dateTimeEnd;
    QPushButton* btnSubmitDiscount;

    QVector<Book> myBooks;

public:
    explicit DiscountManagerWidget(QWidget *parent = nullptr);
    ~DiscountManagerWidget() override = default;

    void setAvailableBooks(const QVector<Book> &books);

private slots:
    void sendDiscountToServer();
    void onServerReply(CommandType commandType, QJsonObject payload, bool ok);
};