#pragma once
#include <string>

enum class NotificationType {
    NewBookInFavoriteGenre,
    DiscountApplied,
    NewSale,
    NewReview
};

class AppNotification {
private:
    int id;
    NotificationType type;
    std::string message;
    bool isRead;
    std::string timestamp;   // همیشه توسط خودِ کلاس، هنگام ساخت، پر می‌شود
    int targetUserId;

    // زمان لحظه‌ی حال سیستم را به‌صورت رشته‌ی قابل‌ذخیره برمی‌گرداند (مثل "2026-07-05 14:32:07")
    static std::string getCurrentTimestamp();

public:
    // توجه: پارامتر timestamp دیگر گرفته نمی‌شود؛ خودِ سازنده آن را ست می‌کند
    AppNotification(int id, NotificationType type, std::string message, int targetUserId);

    void markAsRead();
    bool getIsRead() const;
    int getId() const;
    NotificationType getType() const;
    std::string getMessage() const;
    std::string getTimestamp() const;
    int getTargetUserId() const;

    // متدهای کارخانه‌ای (Factory) برای ساخت آسان انواع اعلان‌های ذکرشده در سند (بخش ۵)
    // این‌ها هم دیگر timestamp نمی‌گیرند؛ همان لحظه‌ی فراخوانی به‌صورت خودکار ثبت می‌شود
    static AppNotification createNewBookNotification(int id, int targetUserId, const std::string &bookTitle);
    static AppNotification createDiscountNotification(int id, int targetUserId, const std::string &bookTitle,
                                                      double discountPercentage);
    static AppNotification createNewSaleNotification(int id, int publisherUserId, const std::string &bookTitle);
    static AppNotification createNewReviewNotification(int id, int publisherUserId, const std::string &bookTitle);
};
