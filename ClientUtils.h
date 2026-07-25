#ifndef CLIENTUTILS_H
#define CLIENTUTILS_H
#pragma once
#include <QJsonObject>
#include "Book.h"
#include "Genre.h"

namespace ClientUtils {

inline Book bookFromJson(const QJsonObject &o) {
    Book b(
        o.value("title").toString().toStdString(),
        o.value("author").toString().toStdString(),
        o.value("publisherId").toInt(),
        static_cast<Genre>(o.value("genre").toInt()),
        o.value("description").toString().toStdString(),
        o.value("basePrice").toDouble(),
        o.value("coverImagePath").toString().toStdString(),
        o.value("pdfFileName").toString().toStdString(),
        o.value("publishDate").toString().toStdString()
        );
    b.setId(o.value("id").toInt());//شناسه وتقعی سرور را جایگزین شناسه خودکار سازنده میکنیم
    return b;
}


inline double bookFinalPriceFromJson(const QJsonObject &o) {
    return o.contains("finalPrice") ? o.value("finalPrice").toDouble() : o.value("basePrice").toDouble();
}

} // namespace ClientUtils

#endif // CLIENTUTILS_H
