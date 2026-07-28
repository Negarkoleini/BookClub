#include <QStackedWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

#include "ClientNetworkManager.h"
#include "DashboardController.h"

namespace {
QString readServerPortFromFile() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    QFile file(QDir(dir).filePath("server_port.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "5555";
    }

    QTextStream in(&file);
    QString portText = in.readLine().trimmed();
    file.close();
    return portText.isEmpty() ? "5555" : portText;
}
}

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

    const quint16 port = static_cast<quint16>(readServerPortFromFile().toUShort());

    if (!ClientNetworkManager::getInstance().establishConnection(serverIp, port)) {
        QMessageBox::critical(
            nullptr,
            "خطا",
            QString("اتصال به سرور روی پورت %1 برقرار نشد.").arg(port));
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