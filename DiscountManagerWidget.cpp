#include "DiscountManagerWidget.h"
#include "ClientNetworkManager.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QJsonObject>

DiscountManagerWidget::DiscountManagerWidget(QWidget *parent) : QWidget(parent) {
    auto* form = new QFormLayout(this);

    comboBooks = new QComboBox();
    comboDiscountType = new QComboBox();
    comboDiscountType->addItem("درصدی (%)", 0);   // 0 == DiscountType::Percentage
    comboDiscountType->addItem("مبلغ ثابت", 1);   // 1 == DiscountType::Cash

    spinDiscountValue = new QDoubleSpinBox();
    spinDiscountValue->setRange(0, 1000000);
    spinDiscountValue->setDecimals(0);

    dateTimeStart = new QDateTimeEdit(QDateTime::currentDateTime());
    dateTimeStart->setCalendarPopup(true);
    dateTimeEnd = new QDateTimeEdit(QDateTime::currentDateTime().addDays(7));
    dateTimeEnd->setCalendarPopup(true);

    btnSubmitDiscount = new QPushButton("اعمالِ تخفیف");

    form->addRow("کتاب:", comboBooks);
    form->addRow("نوع تخفیف:", comboDiscountType);
    form->addRow("مقدار تخفیف:", spinDiscountValue);
    form->addRow("شروع:", dateTimeStart);
    form->addRow("پایان:", dateTimeEnd);
    form->addRow(btnSubmitDiscount);

    connect(btnSubmitDiscount, &QPushButton::clicked, this, &DiscountManagerWidget::sendDiscountToServer);

    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::serverReplyReceived,
            this, &DiscountManagerWidget::onServerReply);
}

void DiscountManagerWidget::setAvailableBooks(const QVector<Book> &books) {
    myBooks = books;
    comboBooks->clear();
    for (const auto &b : myBooks) {
        comboBooks->addItem(QString::fromStdString(b.getTitle()), b.getId());
    }
}

void DiscountManagerWidget::sendDiscountToServer() {
    if (comboBooks->count() == 0) {
        QMessageBox::warning(this, "خطا", "ابتدا باید حداقل یک کتاب منتشر کرده باشید.");
        return;
    }
    if (dateTimeEnd->dateTime() <= dateTimeStart->dateTime()) {
        QMessageBox::warning(this, "خطا", "تاریخِ پایان باید بعد از تاریخِ شروع باشد.");
        return;
    }

    QJsonObject req;
    req["bookId"] = comboBooks->currentData().toInt();
    req["discountType"] = comboDiscountType->currentData().toInt();
    req["discountValue"] = spinDiscountValue->value();
    req["startDateTime"] = dateTimeStart->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    req["endDateTime"] = dateTimeEnd->dateTime().toString("yyyy-MM-dd HH:mm:ss");

    ClientNetworkManager::getInstance().sendRequest(CommandType::ApplyDiscount, req);
}

void DiscountManagerWidget::onServerReply(CommandType commandType, QJsonObject payload, bool ok) {
    if (commandType != CommandType::ApplyDiscount) return;

    if (!ok) {
        QMessageBox::warning(this, "خطا", payload.value("error").toString());
        return;
    }

    QMessageBox::information(this, "ارسال شد", "درخواستِ تخفیف ثبت شد. پس از تأییدِ مدیر سیستم، تخفیف برای کاربران قابل مشاهده خواهد شد.");
    this->close();
}
