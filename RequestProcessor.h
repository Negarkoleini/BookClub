#ifndef REQUESTPROCESSOR_H
#define REQUESTPROCESSOR_H
#pragma once
#include <QObject>
#include <QByteArray>
#include "CommandType.h"
#include "ClientSocketWorker.h"
#include "NotificationBroadcaster.h"
#include <QJsonObject>


class RequestProcessor : public QObject {
    Q_OBJECT
public:
    explicit RequestProcessor(NotificationBroadcaster* broadcaster, QObject *parent = nullptr);

public slots:
    void handleRequest(CommandType commandType, const QByteArray &payload, ClientSocketWorker* sender);

private:
    NotificationBroadcaster* broadcaster; // برای اطلاع‌رسانیِ خودکار بعد از رویدادها (کتاب جدید، فروش و ...)

    // احراز هویت
    void processLogin(const QByteArray &data, ClientSocketWorker* sender);
    void processRegister(const QByteArray &data, ClientSocketWorker* sender);
    void processForgotPasswordRequest(const QByteArray &data, ClientSocketWorker* sender);
    void processResetPassword(const QByteArray &data, ClientSocketWorker* sender);
    void processChangePassword(const QByteArray &data, ClientSocketWorker* sender);

    // کتاب‌ها
    void processGetBooks(const QByteArray &data, ClientSocketWorker* sender);
    void processGetBookDetails(const QByteArray &data, ClientSocketWorker* sender);
    void processAddBook(const QByteArray &data, ClientSocketWorker* sender);
    void processEditBook(const QByteArray &data, ClientSocketWorker* sender);
    void processDeleteBook(const QByteArray &data, ClientSocketWorker* sender);

    // خرید
    void processBuyBook(const QByteArray &data, ClientSocketWorker* sender);

    // نظر/امتیاز
    void processAddComment(const QByteArray &data, ClientSocketWorker* sender);
    void processAddRating(const QByteArray &data, ClientSocketWorker* sender);
    void processApproveComment(const QByteArray &data, ClientSocketWorker* sender);
    void processGetPendingComments(const QByteArray &data, ClientSocketWorker* sender);

    //  ناشر
    void processApplyDiscount(const QByteArray &data, ClientSocketWorker* sender);
    void processGetPublisherAnalytics(const QByteArray &data, ClientSocketWorker* sender);

    //  ادمین
    void processGetAllUsers(const QByteArray &data, ClientSocketWorker* sender);
    void processBanUser(const QByteArray &data, ClientSocketWorker* sender);
    void processUnbanUser(const QByteArray &data, ClientSocketWorker* sender);
    void processDisableUserTemporarily(const QByteArray &data, ClientSocketWorker* sender);

    void sendError(ClientSocketWorker* sender, const QString &message);
    void sendOk(ClientSocketWorker* sender, const QJsonObject &extra = QJsonObject());
    static std::string currentTimestamp();
};

#endif // REQUESTPROCESSOR_H
