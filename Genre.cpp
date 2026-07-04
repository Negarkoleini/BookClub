#include "Genre.h"
#include <array>
#include <utility>

namespace {
const std::array<std::pair<Genre, const char*>, 12> genreTable = {{
    {Genre::Fiction,    "Fiction"},
    {Genre::NonFiction, "NonFiction"},
    {Genre::SciFi,      "SciFi"},
    {Genre::Fantasy,    "Fantasy"},
    {Genre::Mystery,    "Mystery"},
    {Genre::Romance,    "Romance"},
    {Genre::History,    "History"},
    {Genre::Biography,  "Biography"},
    {Genre::SelfHelp,   "SelfHelp"},
    {Genre::Philosophy, "Philosophy"},
    {Genre::Poetry,     "Poetry"},
    {Genre::Children,   "Children"}
}};

const std::array<std::pair<Genre, const char*>, 12> genreTablePersian = {{
    {Genre::Fiction,    "داستانی"},
    {Genre::NonFiction, "غیرداستانی"},
    {Genre::SciFi,      "علمی-تخیلی"},
    {Genre::Fantasy,    "فانتزی"},
    {Genre::Mystery,    "معمایی"},
    {Genre::Romance,    "عاشقانه"},
    {Genre::History,    "تاریخی"},
    {Genre::Biography,  "زندگی‌نامه"},
    {Genre::SelfHelp,   "خودیاری"},
    {Genre::Philosophy, "فلسفه"},
    {Genre::Poetry,     "شعر"},
    {Genre::Children,   "کودک"}
}};
}

std::string GenreUtils::toString(Genre genre) {
    for (const auto &entry : genreTable) {
        if (entry.first == genre) return entry.second;
    }
    return "Fiction";
}

Genre GenreUtils::fromString(const std::string &text) {
    for (const auto &entry : genreTable) {
        if (text == entry.second) return entry.first;
    }
    return Genre::Fiction;
}

std::string GenreUtils::toPersianString(Genre genre) {
    for (const auto &entry : genreTablePersian) {
        if (entry.first == genre) return entry.second;
    }
    return "داستانی";
}