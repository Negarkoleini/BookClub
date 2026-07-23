#include "InAppNotificationWidget.h"
#include <QVBoxLayout>
#include <QTimer>

InAppNotificationWidget::InAppNotificationWidget(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::ToolTip); // بدون فریم، همیشه روی بقیه‌ی ویجت‌ها
    setAttribute(Qt::WA_ShowWithoutActivating);
    setStyleSheet("background-color: #323232; border-radius: 8px;");

    auto* layout = new QVBoxLayout(this);
    lblNotifMessage = new QLabel();
    lblNotifMessage->setStyleSheet("color: white; padding: 10px; font-size: 13px;");
    lblNotifMessage->setWordWrap(true);
    layout->addWidget(lblNotifMessage);

    setFixedWidth(280);
}

void InAppNotificationWidget::popToastMessage(const QString &message) {
    lblNotifMessage->setText(message);
    adjustSize();

    if (parentWidget()) {
        // گوشه‌ی پایین-راستِ پنجره‌ی والد
        QPoint parentBottomRight = parentWidget()->mapToGlobal(
            QPoint(parentWidget()->width(), parentWidget()->height()));
        move(parentBottomRight.x() - width() - 20, parentBottomRight.y() - height() - 20);
    }

    show();
    raise();

    // بعد از ۴ ثانیه شروع به محوشدن کن
    QTimer::singleShot(4000, this, [this]() {
        slideAnimation = new QPropertyAnimation(this, "windowOpacity");
        slideAnimation->setDuration(600);
        slideAnimation->setStartValue(1.0);
        slideAnimation->setEndValue(0.0);
        connect(slideAnimation, &QPropertyAnimation::finished, this, &InAppNotificationWidget::onAnimationFinished);
        slideAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void InAppNotificationWidget::onAnimationFinished() {
    close();
    deleteLater();
}
