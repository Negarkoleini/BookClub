#include <QDebug>
#include "DatabaseManager.h"
#include "NotificationBroadcaster.h"
#include "RequestProcessor.h"
#include "ServerCore.h"

static NotificationBroadcaster* broadcaster = nullptr;
static RequestProcessor* processor = nullptr;
static ServerCore* server = nullptr;

bool startServer()
{
    if (!DatabaseManager::getInstance().initialize("bookclub.db")) {
        qCritical() << "خطا در راه‌اندازی دیتابیس";
        return false;
    }

    broadcaster = new NotificationBroadcaster();
    processor = new RequestProcessor(broadcaster);
    server = new ServerCore(processor);

    if (!server->start(5555)) {
        qCritical() << "سرور نتوانست روی پورتِ 5555 راه‌اندازی شود "
                       "(احتمالاً پورت قبلاً توسطِ برنامه‌ی دیگری اشغال شده).";
        return false;
    }

    qDebug() << "Server started on port 5555";
    return true;
}
