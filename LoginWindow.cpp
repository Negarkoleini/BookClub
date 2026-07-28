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
#include <QResizeEvent>
#include <QDialog>

LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent) {
    buildUi();

    // ۱. ساخت لیبل پس‌زمینه
    bgLabel = new QLabel(this);
    bgLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    bgLabel->setStyleSheet("background: transparent;");

    QString imagePath = QCoreApplication::applicationDirPath() + "/img/1785233222108.png";
    QPixmap pixmap(imagePath);

    if (!pixmap.isNull()) {
        bgLabel->setPixmap(pixmap);
        bgLabel->setScaledContents(true);
        bgLabel->lower();
    }

    // ۲. حفظ ظاهر طبیعی کنترل‌ها و فقط شفاف‌سازی پشت‌زمینه تب‌ها
    this->setStyleSheet(
        "QTabWidget::pane { background: transparent; }"
        "QTabWidget { background: transparent; }"
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

    // ================= تب ورود =================
    auto* loginPage = new QWidget();
    auto* loginForm = new QFormLayout();
    txtLoginUsername = new QLineEdit();
    txtLoginPassword = new QLineEdit();
    txtLoginPassword->setEchoMode(QLineEdit::Password);
    btnLogin = new QPushButton("ورود");
    btnForgotPassword = new QPushButton("رمز عبور را فراموش کرده‌ام");
    lblLoginStatus = new QLabel();
    lblLoginStatus->setStyleSheet("color:red;");

    loginForm->addRow("نام کاربری:", txtLoginUsername);
    loginForm->addRow("رمز عبور:", txtLoginPassword);
    loginForm->addRow(btnLogin);
    loginForm->addRow(btnForgotPassword);
    loginForm->addRow(lblLoginStatus);
    loginPage->setLayout(loginForm);

    // ================= تب ثبت‌نام =================
    auto* registerPage = new QWidget();
    auto* regForm = new QFormLayout();
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
    btnRegister = new QPushButton("ثبت‌نام");
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
    regForm->addRow(btnRegister);
    regForm->addRow(lblRegisterStatus);
    registerPage->setLayout(regForm);

    // فیلدهای مخصوص ناشر پیش‌فرض مخفی‌اند (فقط وقتی role=Publisher انتخاب شود نشان داده می‌شوند)
    txtPublisherName->setVisible(false);
    txtCorporateId->setVisible(false);
    regForm->labelForField(txtPublisherName)->setVisible(false);
    regForm->labelForField(txtCorporateId)->setVisible(false);

    tabs->addTab(loginPage, "ورود");
    tabs->addTab(registerPage, "ثبت‌نام");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabs);

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
