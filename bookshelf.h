#pragma once
#include <string>
#include <vector>

// نمایانگر سیستم مدیریت کتابخانه شخصی و قفسه‌بندی کاربر
class Bookshelf {
private:
    int shelfId;                                // شناسه اصلی کتابخانه/قفسه
    std::string shelfName;                      // نام قفسه
    std::vector<int> bookIds;                   // کتاب‌های موجود در این قفسه خاص

    // --- بخش‌های اضافه شده برای پوشش کامل داک پروژه ---
    std::vector<int> myBooksIds;                // الف) لیست شناسه‌های کتاب‌های خریداری‌شده (کتاب‌های من)
    std::vector<int> savedBooksIds;             // ب) لیست شناسه‌های کتاب‌های ذخیره‌شده (علاقه‌مندی‌ها)
    std::vector<Bookshelf> customSubShelves;    // ج) لیست قفسه‌ها و دسته‌بندی‌های شخصی ایجاد شده توسط کاربر
    int nextSubShelfId = 1;                     // برای تولید شناسه قفسه‌های جدید

public:
    Bookshelf(int shelfId, std::string shelfName);

    int getShelfId() const;
    std::string getShelfName() const;
    void renameShelf(const std::string &newName);

    // --- مدیریت کتاب‌ها در قفسه جاری ---
    void addBook(int bookId);
    void removeBook(int bookId);
    bool containsBook(int bookId) const;
    int getBookCount() const;
    std::vector<int> getBookIds() const;

    // --- الف) بخش کتاب‌های من (خریداری‌شده) ---
    void addPurchasedBook(int bookId);
    std::vector<int> getPurchasedBookIds() const;
    bool isBookPurchased(int bookId) const;

    // --- ب) بخش کتاب‌های ذخیره‌شده (علاقه‌مندی‌ها) ---
    void saveBookForLater(int bookId);
    void removeFromFileLater(int bookId);
    std::vector<int> getSavedBookIds() const;

    // --- ج) مدیریت قفسه‌ها و دسته‌بندی‌های شخصی ---
    void createCustomShelf(const std::string &name);
    void deleteCustomShelf(int subShelfId);
    void renameCustomShelf(int subShelfId, const std::string &newName);
    bool addBookToCustomShelf(int subShelfId, int bookId);
    bool removeBookFromCustomShelf(int subShelfId, int bookId);
    bool moveBookBetweenCustomShelves(int fromShelfId, int toShelfId, int bookId);
    std::vector<Bookshelf> getCustomShelves() const;
};