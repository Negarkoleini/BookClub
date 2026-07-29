// ===================================================================
// نقطه ورود برنامه سرور (Server.exe)
// این برنامه کاملا مجزا از کلاینت اجرا می‌شود، روی یک پورت مشخص Listen
// می‌کند و از طریق سوکت (TCP) با برنامه‌های کلاینت در ارتباط است.
// ===================================================================
#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#include "AppTheme.h"
#include "DatabaseManager.h"
#include "NotificationBroadcaster.h"
#include "RequestProcessor.h"
#include "ServerCore.h"
#include "ServerDashboardWindow.h"

namespace {

QString getAppDataDir()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir;
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet(appMainStyleSheet());

    // پورت سرور: به‌صورت پیش‌فرض پورت ثابت kDefaultServerPort است تا کلاینت‌ها
    // (که ممکن است روی سیستم دیگری اجرا شوند) از قبل بدانند به کدام پورت وصل شوند.
    // در صورت نیاز می‌توان با آرگومان خط فرمان آن را override کرد:
    //     Server.exe 6000
    quint16 port = kDefaultServerPort;
    if (argc > 1) {
        bool ok = false;
        quint16 customPort = QString(argv[1]).toUShort(&ok);
        if (ok && customPort > 0) {
            port = customPort;
        }
    }

    QString dbPath = getAppDataDir();
    QString fullDbPath = QDir(dbPath).filePath("bookclub.db");
    qDebug() << "Database path:" << fullDbPath;

    if (!DatabaseManager::getInstance().initialize(fullDbPath)) {
        QString msg = QString("خطا در راه‌اندازی دیتابیس در مسیر: %1").arg(fullDbPath);
        qCritical() << msg;
        QMessageBox::critical(nullptr, "خطا", msg);
        return 1;
    }

    auto* broadcaster = new NotificationBroadcaster(&app);
    auto* processor = new RequestProcessor(broadcaster, &app);
    auto* server = new ServerCore(processor, &app);

    // رویدادهای اعلان‌ها هم به همان کانال لاگ سرور (که داشبورد به آن گوش می‌دهد) وصل می‌شوند
    QObject::connect(broadcaster, &NotificationBroadcaster::logRequired,
                      server, &ServerCore::logGenerated);

    if (!server->start(port)) {
        QString msg = QString("سرور روی پورت %1 راه‌اندازی نشد: %2")
                          .arg(port)
                          .arg(server->errorString());
        qCritical() << msg;
        QMessageBox::critical(nullptr, "خطا", msg);
        return 1;
    }

    qDebug() << "Server started on port" << server->serverPort();

    // نمایش پنجره داشبورد سرور: وضعیت اتصال، تعداد کلاینت‌های آنلاین و لاگ‌ها
    ServerDashboardWindow dashboard(server);
    dashboard.setWindowTitle(QString("BookClub Server — Port %1").arg(server->serverPort()));
    dashboard.resize(950, 650);
    dashboard.show();

    return app.exec();
}
