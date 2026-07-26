#include <QStackedWidget>
#include <QMessageBox>
#include <QInputDialog>

#include "ClientNetworkManager.h"
#include "DashboardController.h"

void startClient()
{
    bool okPressed = false;

    QString serverIp = QInputDialog::getText(
        nullptr,
        "اتصال به سرور",
        "آدرس IP سرور:",
        QLineEdit::Normal,
        "127.0.0.1",
        &okPressed
        );

    if (!okPressed)
        return;

    if (!ClientNetworkManager::getInstance().establishConnection(serverIp, 5555)) {
        QMessageBox::critical(
            nullptr,
            "خطا",
            "اتصال به سرور برقرار نشد."
            );
        return;
    }

    QStackedWidget* mainStack = new QStackedWidget();

    mainStack->setWindowTitle("BookClub");
    mainStack->resize(950, 650);

    DashboardController* controller =
        new DashboardController(mainStack);

    Q_UNUSED(controller);

    mainStack->show();
}