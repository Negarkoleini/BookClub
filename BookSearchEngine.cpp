#include "BookSearchEngine.h"
#include <algorithm>
#include <QString>

QVector<Book> BookSearchEngine::filterByTitleOrAuthor(const QString &textQuery, const QVector<Book> &fullCatalog) const {
    if (textQuery.trimmed().isEmpty()) return fullCatalog;
    QString q = textQuery.trimmed().toLower();

    QVector<Book> result;
    for (const auto &b : fullCatalog) {
        QString title = QString::fromStdString(b.getTitle()).toLower();
        QString author = QString::fromStdString(b.getAuthor()).toLower();
        if (title.contains(q) || author.contains(q)) {
            result.push_back(b);
        }
    }
    return result;
}

QVector<Book> BookSearchEngine::filterByPublisherName(const QString &publisherNameQuery, const QVector<Book> &fullCatalog,
                                                      const QMap<int, QString> &publisherIdToName) const {
    if (publisherNameQuery.trimmed().isEmpty()) return fullCatalog;
    QString q = publisherNameQuery.trimmed().toLower();

    QVector<Book> result;
    for (const auto &b : fullCatalog) {
        QString publisherName = publisherIdToName.value(b.getPublisherId(), "").toLower();
        if (publisherName.contains(q)) {
            result.push_back(b);
        }
    }
    return result;
}

QVector<Book> BookSearchEngine::filterByGenre(Genre genre, const QVector<Book> &fullCatalog) const {
    QVector<Book> result;
    for (const auto &b : fullCatalog) {
        if (b.getGenre() == genre) result.push_back(b);
    }
    return result;
}

QVector<Book> BookSearchEngine::getFreeBooks(const QVector<Book> &fullCatalog) const {
    QVector<Book> result;
    for (const auto &b : fullCatalog) {
        if (b.isFree()) result.push_back(b);
    }
    return result;
}

QVector<Book> BookSearchEngine::sortBooksByRating(QVector<Book> booksList) const {
    std::sort(booksList.begin(), booksList.end(), [](const Book &a, const Book &b) {
        return a.getAverageRating() > b.getAverageRating();
    });
    return booksList;
}

QVector<Book> BookSearchEngine::sortBooksByNewest(QVector<Book> booksList) const {
    std::sort(booksList.begin(), booksList.end(), [](const Book &a, const Book &b) {
        return a.getPublishDate() > b.getPublishDate();
    });
    return booksList;
}
