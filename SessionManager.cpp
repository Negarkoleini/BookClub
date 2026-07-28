#include "SessionManager.h"
#include "ClientSocketWorker.h"
#include <QMutexLocker>

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;
    return instance;
}

void SessionManager::registerSession(int userId, ClientSocketWorker* worker) {
    QMutexLocker locker(&sessionMutex);
    onlineUsers[userId] = worker;
}

void SessionManager::removeSession(ClientSocketWorker* worker) {
    QMutexLocker locker(&sessionMutex);
    auto it = onlineUsers.begin();
    while (it != onlineUsers.end()) {
        if (it.value() == worker) {
            it = onlineUsers.erase(it);
        } else {
            ++it;
        }
    }
}

void SessionManager::removeSessionByUserId(int userId) {
    QMutexLocker locker(&sessionMutex);
    onlineUsers.remove(userId);
}

ClientSocketWorker* SessionManager::getWorkerForUser(int userId) const {
    QMutexLocker locker(&sessionMutex);
    return onlineUsers.value(userId, nullptr);
}

bool SessionManager::isUserOnline(int userId) const {
    QMutexLocker locker(&sessionMutex);
    return onlineUsers.contains(userId);
}

int SessionManager::getOnlineUserCount() const {
    QMutexLocker locker(&sessionMutex);
    return onlineUsers.size();
}

QList<int> SessionManager::getOnlineUserIds() const {
    QMutexLocker locker(&sessionMutex);
    return onlineUsers.keys();
}