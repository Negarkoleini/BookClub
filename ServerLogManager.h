#ifndef SERVERLOGMANAGER_H
#define SERVERLOGMANAGER_H
#pragma once
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class ServerLogManager {
private:
    QString logFileName;
    qint64 maxFileSizeBytes;
    mutable QMutex logMutex;

    void rotateLogFiles();

public:
    explicit ServerLogManager(QString filename = "server.log", qint64 maxSize = 1024 * 1024);
    void writeLog(const QString &category, const QString &message);
};

#endif // SERVERLOGMANAGER_H
