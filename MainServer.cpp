#include <QDebug>
#include "DatabaseManager.h"
#include "NotificationBroadcaster.h"
#include "RequestProcessor.h"
#include "ServerCore.h"

static NotificationBroadcaster* broadcaster = nullptr;
static RequestProcessor* processor = nullptr;
static ServerCore* server = nullptr;

void startServer()
{
    if (!DatabaseManager::getInstance().initialize("bookclub.db")) {
        qCritical() << "خطا در راه‌اندازی دیتابیس";
        return;
    }

    broadcaster = new NotificationBroadcaster();
    processor = new RequestProcessor(broadcaster);
    server = new ServerCore(processor);

    qDebug() << "Server started";
}