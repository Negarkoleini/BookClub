#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTabWidget>
#include<QDialog>
#include <QDialog>
#include <QLabel>
#include <QResizeEvent>
#include <QJsonObject>
#include <QJsonObject>
#include "CommandType.h"


class LoginWindow : public QDialog {
    Q_OBJECT
private:
    // تب ورود
    QLineEdit* txtLoginUsername;
    QLineEdit* txtLoginPassword;
    QPushButton* btnLogin;
    QPushButton* btnForgotPassword;
    QLabel* lblLoginStatus;

    //  تب ثبت نام
    QLineEdit* txtRegUsername;
    QLineEdit* txtRegPassword;
    QLineEdit* txtRegEmail;
    QComboBox* comboRole;
    QLineEdit* txtPublisherName;
    QLineEdit* txtCorporateId;
    QLineEdit* txtSecurityQuestion;
    QLineEdit* txtSecurityAnswer;
    QPushButton* btnRegister;
    QLabel* lblRegisterStatus;
     QLabel *bgLabel;

    QTabWidget* tabs;

    QString forgotPasswordUsername;

    void buildUi();
    void doForgotPasswordFlow();

protected:
     void resizeEvent(QResizeEvent *event) override; // اضافه کردن این

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override = default;

private slots:
    void handleLoginSubmit();
    void handleRegisterSubmit();
    void onRoleChanged(int index);
    void onNetworkReply(CommandType commandType, QJsonObject payload, bool ok);
    void onNetworkError(QString message);

signals:
    void loginSucceeded(int userId, QString role);
};
