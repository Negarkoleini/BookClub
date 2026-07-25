#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTabWidget>
#include <QJsonObject>
#include "CommandType.h"


class LoginWindow : public QWidget {
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

    QTabWidget* tabs;

    QString forgotPasswordUsername;

    void buildUi();
    void doForgotPasswordFlow();

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
