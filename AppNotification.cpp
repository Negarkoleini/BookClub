#include "AppNotification.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

// ----------------------------------------------------------------------
// زمان لحظه‌ی حال سیستم (system clock) را می‌گیرد و به فرمت "YYYY-MM-DD HH:MM:SS"
// تبدیل می‌کند. این متد private است چون فقط خودِ کلاس باید صدایش بزند؛
// هیچ کد بیرونی نباید مستقیماً زمان بسازد و به AppNotification تحمیل کند.
// ----------------------------------------------------------------------
std::string AppNotification::getCurrentTimestamp() {
    using namespace std::chrono;

    const auto now = system_clock::now();
    const std::time_t nowAsTimeT = system_clock::to_time_t(now);

    std::tm localTm{};
#if defined(_WIN32)
    localtime_s(&localTm, &nowAsTimeT);
#else
    localtime_r(&nowAsTimeT, &localTm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

AppNotification::AppNotification(int id, NotificationType type, std::string message, int targetUserId)
    : id(id),
    type(type),
    message(std::move(message)),
    isRead(false),
    timestamp(getCurrentTimestamp()),   // <-- همین‌جا، لحظه‌ی ساخت آبجکت، زمان سیستم ثبت می‌شود
    targetUserId(targetUserId) {
}

void AppNotification::markAsRead() { isRead = true; }
bool AppNotification::getIsRead() const { return isRead; }
int AppNotification::getId() const { return id; }
NotificationType AppNotification::getType() const { return type; }
std::string AppNotification::getMessage() const { return message; }
std::string AppNotification::getTimestamp() const { return timestamp; }
int AppNotification::getTargetUserId() const { return targetUserId; }

// ----------------------------------------------------------------------
// متدهای کارخانه‌ای -- دیگر timestamp نمی‌گیرند، چون سازنده خودش می‌سازدش
// ----------------------------------------------------------------------
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

