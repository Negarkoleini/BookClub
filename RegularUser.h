#ifndef REGULARUSER_H
#define REGULARUSER_H
#pragma once
#include "User.h"
#include "Genre.h"
#include "Bookshelf.h"
#include<vector>
#include<map>

class RegularUser:public User{
private:
    double WalletBalance;
    std::vector<int> purchasedBookIds;
    std::vector<int> savedBookIds;
    std::vector<Genre> favorateGeners;
    std::vector<BookShelf>personalShelves;
    std::map<int,int>BookmarkPages;
public:
    RegularUser(int id,std::string username, std::string passwordHash, std::string email,
                std::string registrationDate, double initialBalance=0.0);

    double getWalletBalance() const;
    void depositMoney(double amount);
    bool withdrawMoney(double amount);

    std::vector<int> getPurchasedBookIds() const;
    void addPurchasedBook(int bookId);
    bool hasBook(int bookId) const;

    void setFavoriteGenres(const std::vector<Genre> &genres);
    std::vector<Genre> getFavoriteGenres() const;

    void addToSaved(int bookId);
    void removeFromSaved(int bookId);
    std::vector<int> getSavedBookIds() const;

    void createShelf(const std::string &shelfName);
    void deleteShelf(int shelfId);
    std::vector<Bookshelf>& getShelves();

    void savePageLocation(int bookId, int pageNum);
    int getPageLocation(int bookId) const;


};
#endif // REGULARUSER_H
