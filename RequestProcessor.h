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
    // این متد به سیگنال ClientSocketWorker::dataReadyForProcessing وصل می‌شود (در ServerCore)
    void handleRequest(CommandType commandType, const QByteArray &payload, ClientSocketWorker* sender);

private:
    NotificationBroadcaster* broadcaster; // برای اطلاع‌رسانیِ خودکار بعد از رویدادها (کتاب جدید، فروش و ...)

    // ---- احراز هویت ----
    void processLogin(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processRegister(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processForgotPasswordRequest(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processResetPassword(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processChangePassword(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetProfile(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processUpdateProfile(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processSetFavoriteGenres(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processDepositMoney(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- کتاب‌ها ----
    void processGetBooks(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetBookDetails(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetSuggestedBooks(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetPopularBooks(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetBestsellingBooks(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processAddBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processEditBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processDeleteBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- خرید ----
    void processBuyBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- کتابخانه شخصی ----
    void processGetLibrary(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processSaveBookForLater(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processRemoveSavedBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processCreateShelf(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processDeleteShelf(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processAddBookToShelf(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processSavePageLocation(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetPageLocation(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- نظر/امتیاز ----
    void processAddComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processAddRating(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processApproveComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetPendingComments(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processRejectComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processDeleteComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processEditComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender); // فقط برای صاحبِ نظر
    void processGetAllComments(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- اعلان‌ها ----
    void processGetNotifications(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processMarkNotificationRead(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- ناشر ----
    void processApplyDiscount(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetPublisherAnalytics(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- ادمین: کاربران ----
    void processGetAllUsers(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processBanUser(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processUnbanUser(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processDisableUserTemporarily(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processChangeUserRole(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processDeleteUserAccount(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processGetUserDetails(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- ادمین: کتاب‌ها ----
    void processGetAllBooksAdmin(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- ادمین: تخفیف‌ها ----
    void processGetPendingDiscounts(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processApproveDiscount(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processRejectDiscount(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- ادمین: محدودیت‌های سیستمی ----
    void processGetLimits(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);
    void processSetLimits(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender);

    // ---- کمکی ----
    bool isRequesterAdmin(ClientSocketWorker* sender) const; // برای چک‌کردنِ سطحِ دسترسی قبل از عملیاتِ حساس
    void sendError(ClientSocketWorker* sender, CommandType cmd, const QString &message);
    void sendOk(ClientSocketWorker* sender, CommandType cmd, const QJsonObject &extra = QJsonObject());
    static std::string currentTimestamp();
};

#endif // REQUESTPROCESSOR_H
