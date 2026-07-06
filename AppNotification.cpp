#include "AppNotification.h"
#include <QDateTime>

std::string AppNotification::getCurrentTime()
{
    return QDateTime::currentDateTime()
    .toString("yyyy-MM-dd HH:mm:ss")
        .toStdString();
}


AppNotification::AppNotification(int id, NotificationType type, std::string message, int targetUserId)
    : id(id),
    type(type),
    message(std::move(message)),
    isRead(false),
    timestamp(getCurrentTime()),
    targetUserId(targetUserId) {
}

void AppNotification::markAsRead() { isRead = true; }
bool AppNotification::getIsRead() const { return isRead; }
int AppNotification::getId() const { return id; }
NotificationType AppNotification::getType() const { return type; }
std::string AppNotification::getMessage() const { return message; }
std::string AppNotification::getTimestamp() const { return timestamp; }
int AppNotification::getTargetUserId() const { return targetUserId; }

AppNotification AppNotification::createNewBookNotification(int id, int targetUserId,
                                                           const std::string &bookTitle) {
    std::string msg = "کتاب جدیدی با عنوان \"" + bookTitle + "\" در یکی از ژانرهای مورد علاقه‌ی شما منتشر شد.";
    return AppNotification(id, NotificationType::NewBookInFavoriteGenre, msg, targetUserId);
}

AppNotification AppNotification::createDiscountNotification(int id, int targetUserId,
                                                            const std::string &bookTitle,
                                                            double discountPercentage) {
    std::string msg = "تخفیف " + std::to_string(static_cast<int>(discountPercentage)) +
                      "% روی کتاب \"" + bookTitle + "\" اعمال شد.";
    return AppNotification(id, NotificationType::DiscountApplied, msg, targetUserId);
}

AppNotification AppNotification::createNewSaleNotification(int id, int publisherUserId,
                                                           const std::string &bookTitle) {
    std::string msg = "کتاب \"" + bookTitle + "\" شما به فروش رسید.";
    return AppNotification(id, NotificationType::NewSale, msg, publisherUserId);
}

AppNotification AppNotification::createNewReviewNotification(int id, int publisherUserId,
                                                             const std::string &bookTitle) {
    std::string msg = "نظر یا امتیاز جدیدی برای کتاب \"" + bookTitle + "\" شما ثبت شد.";
    return AppNotification(id, NotificationType::NewReview, msg, publisherUserId);
}

