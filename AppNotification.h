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
    std::string timestamp;
    int targetUserId;

    static std::string getCurrentTime();


public:

    AppNotification(int id, NotificationType type, std::string message, int targetUserId);

    void markAsRead();
    bool getIsRead() const;
    int getId() const;
    NotificationType getType() const;
    std::string getMessage() const;
    std::string getTimestamp() const;
    int getTargetUserId() const;

    // متدهای کارخانه‌ای (Factory)
    static AppNotification createNewBookNotification(int id, int targetUserId, const std::string &bookTitle);
    static AppNotification createDiscountNotification(int id, int targetUserId, const std::string &bookTitle,
                                                      double discountPercentage);
    static AppNotification createNewSaleNotification(int id, int publisherUserId, const std::string &bookTitle);
    static AppNotification createNewReviewNotification(int id, int publisherUserId, const std::string &bookTitle);
};
