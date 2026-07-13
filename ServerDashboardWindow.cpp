#include "ServerDashboardWindow.h"
#include "SessionManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDateTime>

ServerDashboardWindow::ServerDashboardWindow(ServerCore* server, QWidget *parent)
    : QWidget(parent), m_server(server), m_logManager("server.log") {
    buildUi();

    connect(btnToggleServer, &QPushButton::clicked, this, &ServerDashboardWindow::onStartStopClicked);
    connect(m_server, &ServerCore::logGenerated, this, &ServerDashboardWindow::handleNewLog);
    connect(m_server, &ServerCore::clientCountChanged, this, &ServerDashboardWindow::handleConnectionUpdate);

    statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &ServerDashboardWindow::refreshOnlineUsersLabel);
    statusTimer->start(2000); // هر ۲ ثانیه رفرش (Real-Time طبق سند، بدون فشار زیاد به UI)
}

void ServerDashboardWindow::buildUi() {
    setWindowTitle("BookClub Server Dashboard");
    resize(700, 500);

    auto* mainLayout = new QVBoxLayout(this);

    // ---- بخش کنترل و وضعیت ----
    auto* statusBox = new QGroupBox("وضعیت سرور");
    auto* firstRow=new QHBoxLayout();
    auto* statusLayout = new QVBoxLayout(statusBox);

    btnToggleServer = new QPushButton("راه‌اندازی سرور");
    lblConnectionStatus = new QLabel("متوقف");
    lblActiveClients = new QLabel("کلاینت‌های متصل: 0");
    lblOnlineUsers = new QLabel("کاربران آنلاین: 0");
    lblServerHealth = new QLabel("سلامت: -");

    firstRow->addWidget(btnToggleServer);
    firstRow->addWidget(lblConnectionStatus);
    firstRow->addStretch();
    statusLayout->addLayout(firstRow);
    statusLayout->addWidget(lblActiveClients);
    statusLayout->addWidget(lblOnlineUsers);
    statusLayout->addWidget(lblServerHealth);

    // ---- بخش لاگ ----
    auto* logBox = new QGroupBox("لاگِ زنده‌ی سرور");
    auto* logLayout = new QVBoxLayout(logBox);
    textEditLogs = new QTextEdit();
    textEditLogs->setReadOnly(true);
    logLayout->addWidget(textEditLogs);

    mainLayout->addWidget(statusBox);
    mainLayout->addWidget(logBox);
}

void ServerDashboardWindow::onStartStopClicked() {
    if (m_server->getIsRunning()) {
        m_server->stop();
        btnToggleServer->setText("راه‌اندازی سرور");
        lblConnectionStatus->setText("متوقف");
    } else {
        if (m_server->start(5555)) {
            btnToggleServer->setText("متوقف‌کردن سرور");
            lblConnectionStatus->setText("در حال اجرا (پورت 5555)");
        }
    }
}

void ServerDashboardWindow::handleNewLog(const QString &text) {
    QString line = QString("[%1] %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss"), text);
    textEditLogs->append(line);
    m_logManager.writeLog("SERVER", text);
}

void ServerDashboardWindow::handleConnectionUpdate(int totalCount) {
    lblActiveClients->setText(QString("کلاینت‌های متصل: %1").arg(totalCount));
    updateHealthLabel(totalCount);
}

void ServerDashboardWindow::updateHealthLabel(int connectedClients) {
    QString health;
    if (connectedClients < 10) health = "Healthy";
    else if (connectedClients < 50) health = "Busy";
    else health = "Overloaded";
    lblServerHealth->setText("سلامت: " + health);
}

void ServerDashboardWindow::refreshOnlineUsersLabel() {
    int count = SessionManager::getInstance().getOnlineUserCount();
    lblOnlineUsers->setText(QString("کاربران آنلاین: %1").arg(count));
}
