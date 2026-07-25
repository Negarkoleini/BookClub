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

    void popToastMessage(const QString &message);

private slots:
    void onAnimationFinished();
};
