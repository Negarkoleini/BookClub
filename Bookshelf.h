#pragma once
#include <string>
#include <vector>
#include "Book.h"

class Bookshelf {
private:
    int shelfId;
    std::string shelfName;
    std::vector<int> bookIds;


    std::vector<Book> myBooks;
    std::vector<int> savedBooksIds;
    std::vector<Bookshelf> customSubShelves;
    int nextSubShelfId = 1;

public:
    Bookshelf(int shelfId, std::string shelfName);

    int getShelfId() const;
    std::string getShelfName() const;
    void renameShelf(const std::string &newName);

    // مدیریت کتاب در قفسه
    void addBook(int bookId);
    void removeBook(int bookId);
    bool containsBook(int bookId) const;
    int getBookCount() const;
    std::vector<int> getBookIds() const;

    //  بخش کتاب‌های من
    void addPurchasedBook(const Book &book);
    std::vector<Book> getPurchasedBooks() const;
    bool isBookPurchased(int bookId) const;

    //  بخش کتاب‌های ذخیره شده
    void saveBookForLater(int bookId);
    void removeFromFileLater(int bookId);
    std::vector<int> getSavedBookIds() const;

    // ج) قفسه ها و دسته بندی های شخصی
    void createCustomShelf(const std::string &name);
    void deleteCustomShelf(int subShelfId);
    void renameCustomShelf(int subShelfId, const std::string &newName);
    bool addBookToCustomShelf(int subShelfId, int bookId);
    bool removeBookFromCustomShelf(int subShelfId, int bookId);
    bool moveBookBetweenCustomShelves(int fromShelfId, int toShelfId, int bookId);
    std::vector<Bookshelf> getCustomShelves() const;
};