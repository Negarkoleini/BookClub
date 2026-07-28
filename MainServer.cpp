#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>
#include <QTcpServer>
#include "DatabaseManager.h"
#include "NotificationBroadcaster.h"
#include "RequestProcessor.h"
#include "ServerCore.h"

static NotificationBroadcaster* broadcaster = nullptr;
static RequestProcessor* processor = nullptr;
static ServerCore* server = nullptr;

namespace {
QString getAppDataDir() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir;
}

QString getPortFilePath() {
    return QDir(getAppDataDir()).filePath("server_port.txt");
}

bool writeChosenPort(int port) {
    QFile file(getPortFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << port;
    file.close();
    return true;
}
}

bool startServer(QString* errorMessage = nullptr)
{
    QString dbPath = getAppDataDir();
    QString fullDbPath = QDir(dbPath).filePath("bookclub.db");
    qDebug() << "Database path:" << fullDbPath;

    if (!DatabaseManager::getInstance().initialize(fullDbPath)) {
        QString msg = QString("خطا در راه‌اندازی دیتابیس در مسیر: %1").arg(fullDbPath);
        if (errorMessage) *errorMessage = msg;
        qCritical() << msg;
        return false;
    }

    broadcaster = new NotificationBroadcaster();
    processor = new RequestProcessor(broadcaster);
    server = new ServerCore(processor);

    if (!server->start(0)) {
        QString msg = QString("سرور راه‌اندازی نشد: %1").arg(server->errorString());
        if (errorMessage) *errorMessage = msg;
        qCritical() << msg;
        return false;
    }

    const int chosenPort = server->serverPort();
    if (!writeChosenPort(chosenPort)) {
        qWarning() << "Unable to save chosen server port to file.";
    }

    qDebug() << "Server started on port" << chosenPort;
    return true;
}
