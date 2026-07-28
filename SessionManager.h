#pragma once
#include <QMap>
#include <QMutex>

class ClientSocketWorker;

class SessionManager {
private:
    SessionManager() = default;
    QMap<int, ClientSocketWorker*> onlineUsers; // userId -> worker
    mutable QMutex sessionMutex;

public:
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    static SessionManager& getInstance();

    void registerSession(int userId, ClientSocketWorker* worker);
    void removeSession(ClientSocketWorker* worker);
    void removeSessionByUserId(int userId);

    ClientSocketWorker* getWorkerForUser(int userId) const;
    bool isUserOnline(int userId) const;
    int getOnlineUserCount() const;
    QList<int> getOnlineUserIds() const;
};
