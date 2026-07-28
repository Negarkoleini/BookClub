#include "LoginWindow.h"
#include "ClientNetworkManager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QCoreApplication>
#include <QLabel>
#include <QPixmap>
#include <QCoreApplication>
#include <QPixmap>
#include <QResizeEvent>
#include<QDialog>

LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent) {
    buildUi();

    // ۱. ساخت لیبل پس‌زمینه
    bgLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/img/1785233222108.png";
    QPixmap pixmap(imagePath);

    if (!pixmap.isNull()) {
        bgLabel->setPixmap(pixmap);
        bgLabel->setScaledContents(true); // فیت شدن کامل عکس
        bgLabel->lower();                // فرستادن به زیر بقیه اجزا
    }

    // ۲. شفاف کردن پس‌زمینه تب‌ویجت و ویجت‌های داخلی تا عکس از زیرشان دیده شود
    this->setStyleSheet(
        "QTabWidget::pane { background: transparent; }" // حذف پس‌زمینه کادر تب
        "QWidget { background: transparent; }"          // شفاف کردن ویجت‌های داخلی صفحه ورود/ثبت‌نام
        );

    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::serverReplyReceived,
            this, &LoginWindow::onNetworkReply);
    connect(&ClientNetworkManager::getInstance(), &ClientNetworkManager::connectionError,
            this, &LoginWindow::onNetworkError);
}

// ۳. تابع برای اینکه عکس همیشه اندازه پنجره بماند و نصفه نشود
void LoginWindow::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    if (bgLabel) {
        bgLabel->setGeometry(this->rect()); // پوشش کامل ابعاد پنجره در هر لحظه
    }
}

void LoginWindow::buildUi() {
    setWindowTitle("BookClub - ورود به سامانه");
    resize(420, 380);

    tabs = new QTabWidget(this);
    tabs->setFixedSize(420, 430);

    // ================= تب ورود =================
    auto* loginPage = new QWidget();
    auto* loginForm = new QFormLayout();
    loginForm->setContentsMargins(20,15,20,15);
    loginForm->setSpacing(10);
    txtLoginUsername = new QLineEdit();
    txtLoginPassword = new QLineEdit();
    txtLoginPassword->setEchoMode(QLineEdit::Password);
    btnLogin = new QPushButton("ورود");
    btnForgotPassword = new QPushButton("رمز عبور را فراموش کرده‌ام");

    QString buttonStyle = R"(
QPushButton {
    background-color: #D2A679;
    color: black;
    border: 2px solid #8B5E3C;
    border-radius: 8px;
    padding: 8px 15px;
    font-size: 14px;
    font-weight: bold;
}

QPushButton:hover {
    background-color: #E8C39E;
    border: 2px solid #6B4226;
}

QPushButton:pressed {
    background-color: #B8865A;
}
)";

    btnLogin->setStyleSheet(buttonStyle);
    btnForgotPassword->setStyleSheet(buttonStyle);
    lblLoginStatus = new QLabel();
    lblLoginStatus->setStyleSheet("color:red;");

    loginForm->addRow("نام کاربری:", txtLoginUsername);
    loginForm->addRow("رمز عبور:", txtLoginPassword);
    auto* loginButtonLayout = new QHBoxLayout();
    loginButtonLayout->addStretch();
    loginButtonLayout->addWidget(btnLogin);
    loginButtonLayout->addStretch();

    loginForm->addRow(loginButtonLayout);


    auto* forgotButtonLayout = new QHBoxLayout();
    forgotButtonLayout->addStretch();
    forgotButtonLayout->addWidget(btnForgotPassword);
    forgotButtonLayout->addStretch();

    loginForm->addRow(forgotButtonLayout);
    loginForm->addRow(lblLoginStatus);
    loginPage->setLayout(loginForm);

    // ================= تب ثبت‌نام =================
    auto* registerPage = new QWidget();
    auto* regForm = new QFormLayout();
    regForm->setContentsMargins(30, 25, 30, 25);
    regForm->setSpacing(12);
    txtRegUsername = new QLineEdit();
    txtRegPassword = new QLineEdit();
    txtRegPassword->setEchoMode(QLineEdit::Password);
    txtRegEmail = new QLineEdit();
    comboRole = new QComboBox();
    comboRole->addItem("کاربر عادی", "RegularUser");
    comboRole->addItem("ناشر", "Publisher");
    comboRole->addItem("مدیر سیستم", "Admin");
    txtPublisherName = new QLineEdit();
    txtCorporateId = new QLineEdit();
    txtSecurityQuestion = new QLineEdit();
    txtSecurityQuestion->setPlaceholderText("مثلاً: نام نویسنده‌ی مورد علاقه‌تان؟");
    txtSecurityAnswer = new QLineEdit();
    QList<QLineEdit*> inputs = {
        txtLoginUsername,
        txtLoginPassword,
        txtRegUsername,
        txtRegPassword,
        txtRegEmail,
        txtPublisherName,
        txtCorporateId,
        txtSecurityQuestion,
        txtSecurityAnswer
    };

    for (auto input : inputs) {
        input->setFixedWidth(170);
        input->setFixedHeight(28);
    }
    btnRegister = new QPushButton("ثبت‌نام");
    btnLogin->setFixedSize(130, 45);
    btnForgotPassword->setFixedSize(200, 45);
    btnRegister->setFixedSize(130, 45);
    lblRegisterStatus = new QLabel();
    lblRegisterStatus->setStyleSheet("color:red;");

    regForm->addRow("نام کاربری:", txtRegUsername);
    regForm->addRow("رمز عبور:", txtRegPassword);
    regForm->addRow("ایمیل:", txtRegEmail);
    regForm->addRow("نقش:", comboRole);
    regForm->addRow("نام ناشر:", txtPublisherName);
    regForm->addRow("شناسه‌ی شرکتی:", txtCorporateId);
    regForm->addRow("سوال امنیتی:", txtSecurityQuestion);
    regForm->addRow("پاسخ امنیتی:", txtSecurityAnswer);
    auto* registerButtonLayout = new QHBoxLayout();
    registerButtonLayout->addStretch();
    registerButtonLayout->addWidget(btnRegister);
    registerButtonLayout->addStretch();

    regForm->addRow(registerButtonLayout);
    regForm->addRow(lblRegisterStatus);
    registerPage->setLayout(regForm);

    // فیلدهای مخصوص ناشر پیش‌فرض مخفی‌اند (فقط وقتی role=Publisher انتخاب شود نشان داده می‌شوند)
    txtPublisherName->setVisible(false);
    txtCorporateId->setVisible(false);
    regForm->labelForField(txtPublisherName)->setVisible(false);
    regForm->labelForField(txtCorporateId)->setVisible(false);

    tabs->addTab(loginPage, "ورود");
    tabs->addTab(registerPage, "ثبت‌نام");

    // استایل تب‌ها
    tabs->setStyleSheet(R"(
QTabWidget::pane {
    border: 2px solid #8B5E3C;
    border-radius: 10px;
    background: rgba(255,255,255,80);
}

QTabBar::tab {
    background: #D2A679;
    color: black;
    padding: 6px 18px;
    border-radius: 6px;
    margin: 2px;
}

QTabBar::tab:selected {
    background: #E8C39E;
    font-weight: bold;
}
)");


    // وسط چین کردن
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addWidget(tabs, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    btnLogin->setStyleSheet(buttonStyle);
    btnForgotPassword->setStyleSheet(buttonStyle);
    btnRegister->setStyleSheet(buttonStyle);

    setLayout(mainLayout);

    connect(btnLogin, &QPushButton::clicked, this, &LoginWindow::handleLoginSubmit);
    connect(btnRegister, &QPushButton::clicked, this, &LoginWindow::handleRegisterSubmit);
    connect(btnForgotPassword, &QPushButton::clicked, this, &LoginWindow::doForgotPasswordFlow);
    connect(comboRole, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LoginWindow::onRoleChanged);
}

void LoginWindow::onRoleChanged(int /*index*/) {
    bool isPublisher = (comboRole->currentData().toString() == "Publisher");
    txtPublisherName->setVisible(isPublisher);
    txtCorporateId->setVisible(isPublisher);
    auto* regForm = qobject_cast<QFormLayout*>(tabs->widget(1)->layout());
    if (regForm) {
        regForm->labelForField(txtPublisherName)->setVisible(isPublisher);
        regForm->labelForField(txtCorporateId)->setVisible(isPublisher);
    }
}

void LoginWindow::handleLoginSubmit() {
    if (txtLoginUsername->text().isEmpty() || txtLoginPassword->text().isEmpty()) {
        lblLoginStatus->setText("نام کاربری و رمز عبور را وارد کنید.");
        return;
    }
    QJsonObject req;
    req["username"] = txtLoginUsername->text();
    req["password"] = txtLoginPassword->text();
    ClientNetworkManager::getInstance().sendRequest(CommandType::Login, req);
}

void LoginWindow::handleRegisterSubmit() {
    if (txtRegUsername->text().isEmpty() || txtRegPassword->text().isEmpty()) {
        lblRegisterStatus->setText("نام کاربری و رمز عبور نمی‌توانند خالی باشند.");
        return;
    }
    QJsonObject req;
    req["username"] = txtRegUsername->text();
    req["password"] = txtRegPassword->text();
    req["email"] = txtRegEmail->text();
    req["role"] = comboRole->currentData().toString();
    req["securityQuestion"] = txtSecurityQuestion->text();
    req["securityAnswer"] = txtSecurityAnswer->text();
    if (comboRole->currentData().toString() == "Publisher") {
        req["publisherName"] = txtPublisherName->text();
        req["corporateId"] = txtCorporateId->text();
    }
    ClientNetworkManager::getInstance().sendRequest(CommandType::Register, req);
}

void LoginWindow::doForgotPasswordFlow() {
    bool okPressed = false;
    QString username = QInputDialog::getText(this, "فراموشی رمز عبور", "نام کاربری خود را وارد کنید:",
                                             QLineEdit::Normal, "", &okPressed);
    if (!okPressed || username.isEmpty()) return;

    forgotPasswordUsername = username;
    QJsonObject req;
    req["username"] = username;
    ClientNetworkManager::getInstance().sendRequest(CommandType::ForgotPasswordRequest, req);
}

void LoginWindow::onNetworkReply(CommandType commandType, QJsonObject payload, bool ok) {
    if (!ok) {
        QString err = payload.value("error").toString();
        lblLoginStatus->setText(err);
        lblRegisterStatus->setText(err);
        return;
    }

    switch (commandType) {
    case CommandType::Login: {
        int userId = payload.value("userId").toInt();
        QString role = payload.value("role").toString();
        emit loginSucceeded(userId, role);
        break;
    }
    case CommandType::Register: {
        lblRegisterStatus->setStyleSheet("color:green;");
        lblRegisterStatus->setText("ثبت نام با موفقیت انجام شد؛ اکنون میتوانید وارد شوید.");
        tabs->setCurrentIndex(0);
        break;
    }
    case CommandType::ForgotPasswordRequest: {
        // ادامه‌ی جریانِ فراموشیِ رمز: سوال را نشان بده، پاسخ و رمزِ جدید را بگیر
        QString question = payload.value("securityQuestion").toString();
        bool okPressed = false;
        QString answer = QInputDialog::getText(this, "سوال امنیتی", question,
                                               QLineEdit::Normal, "", &okPressed);
        if (!okPressed) return;

        QString newPassword = QInputDialog::getText(this, "رمز عبور جدید", "رمز عبور جدید را وارد کنید:",
                                                    QLineEdit::Password, "", &okPressed);
        if (!okPressed || newPassword.isEmpty()) return;

        QJsonObject req;
        req["username"] = forgotPasswordUsername;
        req["securityAnswer"] = answer;
        req["newPassword"] = newPassword;
        ClientNetworkManager::getInstance().sendRequest(CommandType::ResetPassword, req);
        break;
    }
    case CommandType::ResetPassword: {
        QMessageBox::information(this, "موفق", "رمز عبور با موفقیت تغییر کرد؛ اکنون وارد شوید.");
        break;
    }
    default:
        break;
    }
}

void LoginWindow::onNetworkError(QString message) {
    lblLoginStatus->setText(message);
}
