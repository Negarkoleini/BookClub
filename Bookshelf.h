#pragma once
#include <string>
#include <vector>
#include "Book.h" // اضافه شدن شیء کتاب برای خواندن جزئیات و فایل

class Bookshelf {
private:
    int shelfId;
    std::string shelfName;
    std::vector<int> bookIds; // شناسه‌های کتاب داخل این قفسه خاص

    // بخش‌های سراسری کتابخانه شخصی کاربر
    std::vector<Book> myBooks;                // الف) بخش کتاب‌های من (دارای فایل و جزئیات)
    std::vector<int> savedBooksIds;           // ب) بخش کتاب‌های ذخیره‌شده
    std::vector<Bookshelf> customSubShelves;   // ج) قفسه‌های شخصی کاربر
    int nextSubShelfId = 1;

public:
    Bookshelf(int shelfId, std::string shelfName);

    int getShelfId() const;
    std::string getShelfName() const;
    void renameShelf(const std::string &newName);

    // مدیریت کتاب‌ها در قفسه جاری
    void addBook(int bookId);
    void removeBook(int bookId);
    bool containsBook(int bookId) const;
    int getBookCount() const;
    std::vector<int> getBookIds() const;

    // الف) بخش کتاب‌های من
    void addPurchasedBook(const Book &book);
    std::vector<Book> getPurchasedBooks() const;
    bool isBookPurchased(int bookId) const;

    // ب) بخش کتاب‌های ذخیره‌شده
    void saveBookForLater(int bookId);
    void removeFromFileLater(int bookId);
    std::vector<int> getSavedBookIds() const;

    // ج) قفسه‌ها و دسته‌بندی‌های شخصی
    void createCustomShelf(const std::string &name);
    void deleteCustomShelf(int subShelfId);
    void renameCustomShelf(int subShelfId, const std::string &newName);
    bool addBookToCustomShelf(int subShelfId, int bookId);
    bool removeBookFromCustomShelf(int subShelfId, int bookId);
    bool moveBookBetweenCustomShelves(int fromShelfId, int toShelfId, int bookId);
    std::vector<Bookshelf> getCustomShelves() const;
};