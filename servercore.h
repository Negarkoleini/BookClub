#pragma once
#include <QTcpServer>
#include <QObject>
#include "ClientHandlerThread.h"

class RequestProcessor;

// -----------------------------------------------------------------------
// ServerCore: هسته‌ی گوش‌دادن روی پورت TCP.
// به ازای هر اتصالِ جدید، یک ClientHandlerThread می‌سازد (نه این‌که خودش
// مستقیم داده بخواند)، و سیگنال‌های آن ترد را به RequestProcessor وصل می‌کند.
// -----------------------------------------------------------------------
class ServerCore : public QTcpServer {
    Q_OBJECT
private:
    quint16 listeningPort = 0;
    bool isRunning = false;
    RequestProcessor* requestProcessor;
    int connectedClientsCount = 0;

public:
    explicit ServerCore(RequestProcessor* processor, QObject *parent = nullptr);
    ~ServerCore() override;

    bool start(quint16 port);
    void stop();
    bool getIsRunning() const;
    int getConnectedClientsCount() const;

signals:
    void logGenerated(const QString &logText);
    void clientCountChanged(int totalConnected);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};