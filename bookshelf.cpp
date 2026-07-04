#include "Bookshelf.h"
#include <algorithm>

Bookshelf::Bookshelf(int shelfId, std::string shelfName)
    : shelfId(shelfId), shelfName(std::move(shelfName)) {
}

int Bookshelf::getShelfId() const { return shelfId; }
std::string Bookshelf::getShelfName() const { return shelfName; }
void Bookshelf::renameShelf(const std::string &newName) { shelfName = newName; }

void Bookshelf::addBook(int bookId) {
    if (!containsBook(bookId)) {
        bookIds.push_back(bookId);
    }
}

void Bookshelf::removeBook(int bookId) {
    bookIds.erase(std::remove(bookIds.begin(), bookIds.end(), bookId), bookIds.end());
}

bool Bookshelf::containsBook(int bookId) const {
    return std::find(bookIds.begin(), bookIds.end(), bookId) != bookIds.end();
}

int Bookshelf::getBookCount() const {
    return static_cast<int>(bookIds.size());
}

std::vector<int> Bookshelf::getBookIds() const { return bookIds; }
