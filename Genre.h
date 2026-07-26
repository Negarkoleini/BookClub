#pragma once
#include <string>

enum class Genre {
    Fiction, NonFiction, SciFi, Fantasy, Mystery,
    Romance, History, Biography, SelfHelp, Philosophy,
    Poetry, Children,darsi,
};


class GenreUtils {
public:
    GenreUtils() = delete;
    static std::string toString(Genre genre);
    static Genre fromString(const std::string &text);
    static std::string toPersianString(Genre genre);
};
