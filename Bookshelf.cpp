#include "Bookshelf.h"
#include <algorithm>
#include <iostream>

// سازنده کلاس
Bookshelf::Bookshelf(int shelfId, std::string shelfName)
    : shelfId(shelfId), shelfName(shelfName) {
}

int Bookshelf::getShelfId() const { return shelfId; }
std::string Bookshelf::getShelfName() const { return shelfName; }

void Bookshelf::renameShelf(const std::string &newName) {
    if (!newName.empty()) {
        shelfName = newName;
    }
}

// --- مدیریت کتاب‌ها در قفسه جاری ---
void Bookshelf::addBook(int bookId) {
    if (!containsBook(bookId)) {
        bookIds.push_back(bookId);
    }
}

void Bookshelf::removeBook(int bookId) {
    auto it = std::remove(bookIds.begin(), bookIds.end(), bookId);
    if (it != bookIds.end()) {
        bookIds.erase(it, bookIds.end());
    }
}

bool Bookshelf::containsBook(int bookId) const {
    return std::find(bookIds.begin(), bookIds.end(), bookId) != bookIds.end();
}

int Bookshelf::getBookCount() const { return static_cast<int>(bookIds.size()); }
std::vector<int> Bookshelf::getBookIds() const { return bookIds; }


// --- الف) بخش کتاب‌های من (خریداری‌شده) ---
void Bookshelf::addPurchasedBook(const Book &book) {
    if (!isBookPurchased(book.getId())) {
        myBooks.push_back(book);
    }
}

std::vector<Book> Bookshelf::getPurchasedBooks() const {
    return myBooks;
}

bool Bookshelf::isBookPurchased(int bookId) const {
    for (const auto &book : myBooks) {
        if (book.getId() == bookId) return true;
    }
    return false;
}


// --- ب) بخش کتاب‌های ذخیره‌شده (علاقه‌مندی‌ها) ---
void Bookshelf::saveBookForLater(int bookId) {
    if (std::find(savedBooksIds.begin(), savedBooksIds.end(), bookId) == savedBooksIds.end()) {
        savedBooksIds.push_back(bookId);
    }
}

void Bookshelf::removeFromFileLater(int bookId) {
    auto it = std::remove(savedBooksIds.begin(), savedBooksIds.end(), bookId);
    if (it != savedBooksIds.end()) {
        savedBooksIds.erase(it, savedBooksIds.end());
    }
}

std::vector<int> Bookshelf::getSavedBookIds() const { return savedBooksIds; }


// --- ج) مدیریت قفسه‌ها و دسته‌بندی‌های شخصی ---
void Bookshelf::createCustomShelf(const std::string &name) {
    customSubShelves.emplace_back(nextSubShelfId++, name);
}

void Bookshelf::deleteCustomShelf(int subShelfId) {
    for (auto it = customSubShelves.begin(); it != customSubShelves.end(); ++it) {
        if (it->getShelfId() == subShelfId) {
            customSubShelves.erase(it);
            break;
        }
    }
}

void Bookshelf::renameCustomShelf(int subShelfId, const std::string &newName) {
    for (auto &shelf : customSubShelves) {
        if (shelf.getShelfId() == subShelfId) {
            shelf.renameShelf(newName);
            break;
        }
    }
}

bool Bookshelf::addBookToCustomShelf(int subShelfId, int bookId) {
    if (!isBookPurchased(bookId)) {
        return false; // فقط کتاب‌های خریداری شده اجازه ورود به قفسه شخصی را دارند
    }

    for (auto &shelf : customSubShelves) {
        if (shelf.getShelfId() == subShelfId) {
            shelf.addBook(bookId);
            return true;
        }
    }
    return false;
}

bool Bookshelf::removeBookFromCustomShelf(int subShelfId, int bookId) {
    for (auto &shelf : customSubShelves) {
        if (shelf.getShelfId() == subShelfId) {
            shelf.removeBook(bookId);
            return true;
        }
    }
    return false;
}

bool Bookshelf::moveBookBetweenCustomShelves(int fromShelfId, int toShelfId, int bookId) {
    Bookshelf* fromShelf = nullptr;
    Bookshelf* toShelf = nullptr;

    for (auto &shelf : customSubShelves) {
        if (shelf.getShelfId() == fromShelfId) fromShelf = &shelf;
        if (shelf.getShelfId() == toShelfId) toShelf = &shelf;
    }

    if (fromShelf && toShelf && fromShelf->containsBook(bookId)) {
        fromShelf->removeBook(bookId);
        toShelf->addBook(bookId);
        return true;
    }
    return false;
}

std::vector<Bookshelf> Bookshelf::getCustomShelves() const {
    return customSubShelves;
}