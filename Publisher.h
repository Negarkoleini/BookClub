#ifndef PUBLISHER_H
#define PUBLISHER_H
#pragma once
#include "User.h"
#include "Book.h"
#include "AnalyticsData.h"
#include "TimedDiscount.h"
#include <vector>
#include <string>

class Publisher : public User {
private:
    std::string publisherName;
    std::string corporateId;
    std::vector<Book> publishedBooks;
    std::vector<TimedDiscount> activeDiscounts;
    AnalyticsData publisherAnalytics;

public:
    Publisher(int id, std::string username, std::string passwordHash, std::string email,
              std::string registrationDate, std::string publisherName, std::string corporateId);

    std::string getPublisherName() const;
    std::string getCorporateId() const;

    void addPublishedBook(const Book &book);
    std::vector<Book> getPublishedBooks() const;
    bool updateBookStatus(int bookId, bool active);
    bool softDeleteBook(int bookId);

    void createDiscount(const TimedDiscount &discount);
    std::vector<TimedDiscount> getActiveDiscounts() const;

    AnalyticsData getAnalytics() const;
    void updateAnalytics(const AnalyticsData &newData);
};
#endif // PUBLISHER_H
