#pragma once
#include <string>
#include <vector>

// نمایانگر یک قفسه‌ی شخصی برای دسته‌بندی کتاب‌ها در کتابخانه‌ی هر کاربر
class Bookshelf {
private:
    int shelfId;
    std::string shelfName;
    std::vector<int> bookIds;

public:
    Bookshelf(int shelfId, std::string shelfName);

    int getShelfId() const;
    std::string getShelfName() const;
    void renameShelf(const std::string &newName);

    void addBook(int bookId);
    void removeBook(int bookId);
    bool containsBook(int bookId) const;
    int getBookCount() const;
    std::vector<int> getBookIds() const;
};
