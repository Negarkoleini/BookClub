#pragma once
#include <string>

enum class Genre {
    Fiction, NonFiction, SciFi, Fantasy, Mystery,
    Romance, History, Biography, SelfHelp, Philosophy,
    Poetry, Children
};

// ابزارهای کمکی برای تبدیل ژانر به رشته و برعکس
// (برای ذخیره‌سازی در فایل/دیتابیس و نمایش در UI لازم است)
class GenreUtils {
public:
    GenreUtils() = delete;
    static std::string toString(Genre genre);
    static Genre fromString(const std::string &text);
    static std::string toPersianString(Genre genre);
};
