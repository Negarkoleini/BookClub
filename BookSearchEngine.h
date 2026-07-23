#ifndef BOOKSEARCHENGINE_H
#define BOOKSEARCHENGINE_H
#pragma once
#include "Book.h"
#include <QVector>
#include <QString>
#include <QMap>

class BookSearchEngine{
public:
    BookSearchEngine() = default;

    QVector<Book> filterByTitleOrAuthor(const QString &textQuery, const QVector<Book> &fullCatalog) const;

    QVector<Book> filterByPublisherName(const QString &publisherNameQuery, const QVector<Book> &fullCatalog,
                                        const QMap<int, QString> &publisherIdToName) const;

    QVector<Book> filterByGenre(Genre genre, const QVector<Book> &fullCatalog) const;
    QVector<Book> getFreeBooks(const QVector<Book> &fullCatalog) const;

    QVector<Book> sortBooksByRating(QVector<Book> booksList) const;      // نزولی
    QVector<Book> sortBooksByNewest(QVector<Book> booksList) const;      // بر اساس publishDate، نزولی
};

#endif // BOOKSEARCHENGINE_H
