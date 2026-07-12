#include "ServerLogManager.h"
#include <QDateTime>
#include <QMutexLocker>
#include <QFileInfo>

ServerLogManager::ServerLogManager(QString filename, qint64 maxSize)
    : logFileName(std::move(filename)), maxFileSizeBytes(maxSize) {
}

void ServerLogManager::rotateLogFiles() {
    QString backupName = logFileName + "." + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QFile::rename(logFileName, backupName);
}

void ServerLogManager::writeLog(const QString &category, const QString &message) {
    QMutexLocker locker(&logMutex);

    QFileInfo info(logFileName);
    if (info.exists() && info.size() > maxFileSizeBytes) {
        rotateLogFiles();
    }

    QFile file(logFileName);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
        << " [" << category << "] " << message << "\n";
}
