// ===================================================================
// نقطه ورود برنامه کلاینت (Client.exe)
// این برنامه کاملا مجزا از سرور اجرا می‌شود و از طریق سوکت (TCP) به
// سروری که از قبل در حال اجراست متصل می‌شود. اگر سرور در دسترس نباشد
// (اجرا نشده یا بسته شده باشد) کلاینت پیغام خطا نمایش می‌دهد و اجرا
// نمی‌شود / قطع می‌شود.
// ===================================================================
#include <QApplication>
#include <QStackedWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>

#include "AppTheme.h"
#include "ClientNetworkManager.h"
#include "DashboardController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet(appMainStyleSheet());

    bool okPressed = false;
    QString serverIp = QInputDialog::getText(
        nullptr,
        "اتصال به سرور",
        "آدرس IP سرور:",
        QLineEdit::Normal,
        "127.0.0.1",
        &okPressed);

    if (!okPressed)
        return 0;

    QString portText = QInputDialog::getText(
        nullptr,
        "اتصال به سرور",
        "پورت سرور:",
        QLineEdit::Normal,
        QString::number(kDefaultServerPort),
        &okPressed);

    if (!okPressed)
        return 0;

    bool portOk = false;
    quint16 port = portText.toUShort(&portOk);
    if (!portOk || port == 0) {
        port = kDefaultServerPort;
    }

    // تلاش برای اتصال به سرور از طریق سوکت. اگر سرور در حال اجرا نباشد
    // (Server.exe اجرا نشده یا بسته شده باشد) این اتصال با خطا مواجه
    // می‌شود و کلاینت جدید نمی‌تواند وارد سیستم شود.
    if (!ClientNetworkManager::getInstance().establishConnection(serverIp, port)) {
        QMessageBox::critical(
            nullptr,
            "خطا",
            QString("اتصال به سرور %1:%2 برقرار نشد.\nمطمئن شوید Server.exe در حال اجراست.")
                .arg(serverIp)
                .arg(port));
        return 1;
    }

    QStackedWidget mainStack;
    mainStack.setWindowTitle("BookClub");
    mainStack.resize(950, 650);

    DashboardController controller(&mainStack);
    Q_UNUSED(controller);

    mainStack.show();

    // اگر ارتباط با سرور در حین اجرا قطع شود (مثلا سرور بسته شود)،
    // به کاربر اطلاع داده می‌شود.
    QObject::connect(&ClientNetworkManager::getInstance(),
                      &ClientNetworkManager::disconnectedFromServer,
                      &mainStack,
                      [&mainStack]() {
                          QMessageBox::warning(&mainStack,
                                               "قطع ارتباط",
                                               "ارتباط با سرور قطع شد. برنامه بسته می‌شود.");
                          qApp->quit();
                      });

    return app.exec();
}
