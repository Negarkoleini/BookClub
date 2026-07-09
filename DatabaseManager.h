#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#pragma once
#include <QString>
#include <QMutex>
#include <QVector>
#include <QSqlDatabase>
#include <memory>

#include "User.h"
#include "RegularUser.h"
#include "Publisher.h"
#include "Admin.h"
#include "Book.h"
#include "Comment.h"
#include "Transaction.h"
#include "TimedDiscount.h"
#include "AppNotification.h"
#include "Bookshelf.h"

struct UserSummary { //ساختار برای نمایش خلاصه کاربر
    int id = -1;
    std::string username;
    std::string passwordHash;
    std::string email;
    std::string role;
    AccountStatus status = AccountStatus::Active;
    std::string registrationDate;
};

class DatabaseManager{
    private:
        DatabaseManager() = default; // به علت اینکه فقط یک شی از کلاس ایجاد شود در پرایوت انده (singlton)
        QSqlDatabase db;
        mutable QMutex dbMutex; // جلوگیری از وصل شدن چند ترد همزمان به دیتا بیس  ___ میو تیبل میکوید این متغیر حتی در تواابع کانست هم قابل تغییر است
        bool createSchema();// تابعی برای ساخت جدول
    public:
        DatabaseManager(const DatabaseManager&) = delete;
        DatabaseManager& operator=(const DatabaseManager&) = delete;
        static DatabaseManager& getInstance();
        bool initialize(const QString &dbFilePath = "bookclub.db");
                   //مدیریت کاربران//
        bool usernameExists(const std::string &username) const;
        int getNextUserId() const;
        bool addRegularUser(const RegularUser &user);
        bool addPublisher(const Publisher &publisher);
        bool addAdmin(const Admin &admin);
                   //لاگین//
        bool findUserSummaryByUsername(const std::string &username, UserSummary &out) const;
        bool findUserSummaryById(int userId, UserSummary &out) const;
        bool updateUserStatus(int userId, AccountStatus status);
        bool updateUserRole(int userId, const std::string &newRole);
        bool updatePasswordHash(int userId, const std::string &newPasswordHash);
        bool updateWalletBalance(int userId, double newBalance);
        bool getSecurityQuestion(const std::string &username, std::string &question, std::string &answerHashOut) const;
        bool deleteUserAccount(int userId);
        QVector<UserSummary> getAllUsers() const;
              //بارگذاری کامل ابجکت//
        std::unique_ptr<RegularUser> loadRegularUser(int userId) const;
        std::unique_ptr<Publisher> loadPublisher(int userId) const;
                  //کتاب ها//
        int addBook(const Book &book);
        bool updateBook(const Book &book);
        bool setBookActive(int bookId, bool active);
        bool softDeleteBook(int bookId);
        QVector<Book> getAllActiveBooks() const;
        bool getBookById(int bookId, Book &out) const;
        QVector<Book> getBooksByPublisher(int publisherId) const;
                 //امتیاز و نظرات//
        bool upsertRating(int bookId, int userId, int score);
        double getAverageRating(int bookId) const;
        int addComment(const Comment &comment);
        QVector<Comment> getCommentsForBook(int bookId) const;
        QVector<Comment> getPendingComments() const;
        bool setCommentApproved(int commentId, bool approved);
        bool deleteComment(int commentId);
               //خرید و تراکنش//
        bool logTransaction(const Transaction &tx);
        bool addBookToLibrary(int userId, int bookId);
        bool isBookInLibrary(int userId, int bookId) const;
        QVector<int> getPurchasedBookIds(int userId) const;
        //کتابخانه شخصی//
        bool saveBookForLater(int userId, int bookId);
        bool removeSavedBook(int userId, int bookId);
        QVector<int> getSavedBookIds(int userId) const;
        int createShelf(int userId, const std::string &shelfName);
        bool deleteShelf(int shelfId);
        bool addBookToShelf(int shelfId, int bookId);
        bool removeBookFromShelf(int shelfId, int bookId);
        bool savePageLocation(int userId, int bookId, int pageNum);
        int getPageLocation(int userId, int bookId) const;
        //تخفیف//
        int addDiscount(const TimedDiscount &discount);
        bool removeDiscount(int discountId);
        QVector<TimedDiscount> getActiveDiscountsForBook(int bookId, const std::string &currentSystemTime) const;
        //اعلان ها//
        bool saveNotification(const AppNotification &notif);
        QVector<AppNotification> getNotificationsForUser(int userId) const;
        bool markNotificationRead(int notificationId);


};
#endif // DATABASEMANAGER_H
