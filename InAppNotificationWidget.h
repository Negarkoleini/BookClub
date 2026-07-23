#pragma once
#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QString>


class InAppNotificationWidget : public QWidget {
    Q_OBJECT
private:
    QLabel* lblNotifMessage;
    QPropertyAnimation* slideAnimation = nullptr;

public:
    explicit InAppNotificationWidget(QWidget *parent = nullptr);
    ~InAppNotificationWidget() override = default;

    // پیام را نشان می‌دهد؛ گوشه‌ی پایین-راستِ parent قرار می‌گیرد و بعد از
    // چند ثانیه با انیمیشن محو و خودش حذف می‌شود (deleteLater)
    void popToastMessage(const QString &message);

private slots:
    void onAnimationFinished();
};
