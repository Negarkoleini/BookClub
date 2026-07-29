#include "ServerDashboardWindow.h"
#include "SessionManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>
#include <QDateTime>
#include <QFont>

namespace {
// رنگ‌های نشان سلامت سرور
const char* HEALTH_HEALTHY_STYLE = "background-color:#2E8B57; color:white; border-radius:10px; padding:4px 14px; font-weight:bold;";
const char* HEALTH_BUSY_STYLE    = "background-color:#E0A000; color:white; border-radius:10px; padding:4px 14px; font-weight:bold;";
const char* HEALTH_OVERLOAD_STYLE= "background-color:#C0392B; color:white; border-radius:10px; padding:4px 14px; font-weight:bold;";

const char* CARD_STYLE =
    "QFrame#statCard {"
    "  background-color:#FFFFFF;"
    "  border:1px solid #CDBA9F;"
    "  border-radius:10px;"
    "}";
}

ServerDashboardWindow::ServerDashboardWindow(ServerCore* server, QWidget *parent)
    : QWidget(parent), m_server(server), m_logManager("server.log") {
    buildUi();

    connect(btnToggleServer, &QPushButton::clicked, this, &ServerDashboardWindow::onStartStopClicked);
    connect(m_server, &ServerCore::logGenerated, this, &ServerDashboardWindow::handleNewLog);
    connect(m_server, &ServerCore::clientCountChanged, this, &ServerDashboardWindow::handleConnectionUpdate);

    // مقداردهی اولیه‌ی وضعیت با توجه به اینکه سرور معمولا از قبل توسط main راه‌اندازی شده
    if (m_server->getIsRunning()) {
        m_configuredPort = m_server->serverPort();
        btnToggleServer->setText("متوقف‌کردن سرور");
        lblConnectionStatus->setText(QString("در حال اجرا — پورت %1").arg(m_configuredPort));
    }
    updateHealthBadge(m_server->getConnectedClientsCount());

    statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &ServerDashboardWindow::refreshRealtimeStats);
    statusTimer->start(2000); // هر ۲ ثانیه رفرش (Real-Time طبق سند، بدون فشار زیاد به UI)
    refreshRealtimeStats();
}

QWidget* ServerDashboardWindow::buildStatCard(const QString &title, QLabel* valueLabel) {
    auto* card = new QFrame();
    card->setObjectName("statCard");
    card->setStyleSheet(CARD_STYLE);

    auto* layout = new QVBoxLayout(card);
    auto* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color:#8C6239; font-weight:bold; font-size:11px;");

    QFont valueFont = valueLabel->font();
    valueFont.setPointSize(18);
    valueFont.setBold(true);
    valueLabel->setFont(valueFont);
    valueLabel->setStyleSheet("color:#4A3B32;");

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    return card;
}

void ServerDashboardWindow::buildUi() {
    setWindowTitle("BookClub Server — داشبورد پایش سیستم");
    resize(950, 650);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    // ---------------------------------------------------------------
    // نوار بالا: کنترل سرور + وضعیت اتصال + نشان سلامت
    // ---------------------------------------------------------------
    auto* topBar = new QHBoxLayout();
    btnToggleServer = new QPushButton("متوقف‌کردن سرور");
    lblConnectionStatus = new QLabel("در حال اجرا");
    lblConnectionStatus->setStyleSheet("font-weight:bold; color:#2E8B57;");

    lblHealthBadge = new QLabel("Healthy");
    lblHealthBadge->setAlignment(Qt::AlignCenter);
    lblHealthBadge->setStyleSheet(HEALTH_HEALTHY_STYLE);

    topBar->addWidget(btnToggleServer);
    topBar->addSpacing(12);
    topBar->addWidget(new QLabel("وضعیت:"));
    topBar->addWidget(lblConnectionStatus);
    topBar->addStretch();
    topBar->addWidget(new QLabel("سلامت سرور:"));
    topBar->addWidget(lblHealthBadge);

    // ---------------------------------------------------------------
    // کارت‌های آماری: کاربران آنلاین / کلاینت‌های متصل / CPU / RAM
    // ---------------------------------------------------------------
    auto* statsGrid = new QGridLayout();
    statsGrid->setSpacing(12);

    lblOnlineUsersValue = new QLabel("0");
    lblActiveClientsValue = new QLabel("0");

    statsGrid->addWidget(buildStatCard("کاربران آنلاین", lblOnlineUsersValue), 0, 0);
    statsGrid->addWidget(buildStatCard("کلاینت‌های متصل", lblActiveClientsValue), 0, 1);

    auto* cpuCard = new QFrame();
    cpuCard->setObjectName("statCard");
    cpuCard->setStyleSheet(CARD_STYLE);
    auto* cpuLayout = new QVBoxLayout(cpuCard);
    auto* cpuTitle = new QLabel("میزان استفاده از CPU");
    cpuTitle->setStyleSheet("color:#8C6239; font-weight:bold; font-size:11px;");
    cpuBar = new QProgressBar();
    cpuBar->setRange(0, 100);
    cpuBar->setFormat("%p%");
    cpuLayout->addWidget(cpuTitle);
    cpuLayout->addWidget(cpuBar);
    statsGrid->addWidget(cpuCard, 0, 2);

    auto* ramCard = new QFrame();
    ramCard->setObjectName("statCard");
    ramCard->setStyleSheet(CARD_STYLE);
    auto* ramLayout = new QVBoxLayout(ramCard);
    auto* ramTitle = new QLabel("میزان استفاده از RAM");
    ramTitle->setStyleSheet("color:#8C6239; font-weight:bold; font-size:11px;");
    ramBar = new QProgressBar();
    ramBar->setRange(0, 100);
    ramBar->setFormat("%p%");
    ramLayout->addWidget(ramTitle);
    ramLayout->addWidget(ramBar);
    statsGrid->addWidget(ramCard, 0, 3);

    for (int col = 0; col < 4; ++col) statsGrid->setColumnStretch(col, 1);

    // ---------------------------------------------------------------
    // دو پنل لاگ کنار هم: درخواست‌ها/پاسخ‌ها  |  اعلان‌ها/رویدادهای سیستمی
    // ---------------------------------------------------------------
    auto* logsRow = new QHBoxLayout();

    auto* requestsBox = new QGroupBox("لاگِ زنده‌ی درخواست‌ها و پاسخ‌ها");
    auto* requestsLayout = new QVBoxLayout(requestsBox);
    textEditRequestsLog = new QTextEdit();
    textEditRequestsLog->setReadOnly(true);
    textEditRequestsLog->setStyleSheet("background-color:#FBF7F2; font-family:Consolas, monospace; font-size:11px;");
    requestsLayout->addWidget(textEditRequestsLog);

    auto* eventsBox = new QGroupBox("اعلان‌ها و رویدادهای سیستمی");
    auto* eventsLayout = new QVBoxLayout(eventsBox);
    textEditEventsLog = new QTextEdit();
    textEditEventsLog->setReadOnly(true);
    textEditEventsLog->setStyleSheet("background-color:#FBF7F2; font-family:Consolas, monospace; font-size:11px;");
    eventsLayout->addWidget(textEditEventsLog);

    logsRow->addWidget(requestsBox);
    logsRow->addWidget(eventsBox);

    mainLayout->addLayout(topBar);
    mainLayout->addLayout(statsGrid);
    mainLayout->addLayout(logsRow, /*stretch=*/1);
}

void ServerDashboardWindow::onStartStopClicked() {
    if (m_server->getIsRunning()) {
        m_server->stop();
        btnToggleServer->setText("راه‌اندازی سرور");
        lblConnectionStatus->setText("متوقف");
        lblConnectionStatus->setStyleSheet("font-weight:bold; color:#C0392B;");
    } else {
        if (m_server->start(m_configuredPort)) {
            btnToggleServer->setText("متوقف‌کردن سرور");
            lblConnectionStatus->setText(QString("در حال اجرا — پورت %1").arg(m_configuredPort));
            lblConnectionStatus->setStyleSheet("font-weight:bold; color:#2E8B57;");
        }
    }
    updateHealthBadge(m_server->getConnectedClientsCount());
}

void ServerDashboardWindow::appendToRequestsLog(const QString &text) {
    QString line = QString("[%1] %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss"), text);
    textEditRequestsLog->append(line);
}

void ServerDashboardWindow::appendToEventsLog(const QString &text) {
    QString line = QString("[%1] %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss"), text);
    textEditEventsLog->append(line);
}

void ServerDashboardWindow::handleNewLog(const QString &text) {
    // مسیریابیِ لاگ بر اساس برچسب: درخواست/پاسخ در یک پنل، بقیه رویدادها در پنل دیگر
    if (text.startsWith("[REQ]") || text.startsWith("[RES]")) {
        appendToRequestsLog(text);
    } else {
        appendToEventsLog(text);
    }
    m_logManager.writeLog("SERVER", text);
}

void ServerDashboardWindow::handleConnectionUpdate(int totalCount) {
    lblActiveClientsValue->setText(QString::number(totalCount));
    updateHealthBadge(totalCount);
}

void ServerDashboardWindow::updateHealthBadge(int connectedClients) {
    if (connectedClients < 10) {
        lblHealthBadge->setText("Healthy");
        lblHealthBadge->setStyleSheet(HEALTH_HEALTHY_STYLE);
    } else if (connectedClients < 50) {
        lblHealthBadge->setText("Busy");
        lblHealthBadge->setStyleSheet(HEALTH_BUSY_STYLE);
    } else {
        lblHealthBadge->setText("Overloaded");
        lblHealthBadge->setStyleSheet(HEALTH_OVERLOAD_STYLE);
    }
}

void ServerDashboardWindow::refreshRealtimeStats() {
    int onlineCount = SessionManager::getInstance().getOnlineUserCount();
    lblOnlineUsersValue->setText(QString::number(onlineCount));

    double cpu = m_resourceMonitor.sampleCpuUsagePercent();
    double ram = m_resourceMonitor.currentRamUsagePercent();

    cpuBar->setValue(cpu >= 0 ? static_cast<int>(cpu) : 0);
    cpuBar->setFormat(cpu >= 0 ? "%p%" : "نامشخص");

    ramBar->setValue(ram >= 0 ? static_cast<int>(ram) : 0);
    ramBar->setFormat(ram >= 0 ? "%p%" : "نامشخص");
}
