#include "RegularUser.h"
#include <algorithm>

RegularUser::RegularUser(int id,std::string username, std::string passwordHash, std::string email,
                         std::string registrationDate, double initialBalance):User
    (id,username,passwordHash,email,"RegularUser",registrationDate),WalletBalance(initialBalance>=0.0?initialBalance:0.0){}

double RegularUser::getWalletBalance() const{
    return WalletBalance;
}
void RegularUser::depositMoney(double amount){
    if(amount>0.0){
        WalletBalance+=amount;
    }
}
bool RegularUser:: withdrawMoney(double amount){
    if(amount>0.0 && WalletBalance>=amount){
        WalletBalance-=amount;
        return true;
    }
    return false;
}

std::vector<int> RegularUser::getPurchasedBookIds() const{
    return purchasedBookIds;
}
void RegularUser::addPurchasedBook(int bookId){
    if(!hasBook(bookId)){
        purchasedBookIds.push_back(bookId);
    }
}
bool RegularUser::hasBook(int bookId) const{
    return std::find(purchasedBookIds.begin(),purchasedBookIds.end(),bookId)!=purchasedBookIds.end();
}

void RegularUser::setFavoriteGenres(const std::vector<Genre> &genres){
    favorateGeners=genres;
}
std::vector<Genre> RegularUser::getFavoriteGenres() const{
    return favorateGeners;
}

void RegularUser::addToSaved(int bookId){
    if(std::find(savedBookIds.begin(),savedBookIds.end(),bookId)==savedBookIds.end()){
        savedBookIds.push_back(bookId);
    }
}
void RegularUser::removeFromSaved(int bookId){
    auto it =std::find(savedBookIds.begin(),savedBookIds.end(),bookId);
    if(it!=savedBookIds.end()){
        savedBookIds.erase(it);
    }
}
std::vector<int> RegularUser::getSavedBookIds() const{
    return savedBookIds;
}

void RegularUser::createShelf(const std::string &shelfName){
    int newShelfId=static_cast<int>(personalShelves.size())+1;
    personalShelves.push_back(Bookshelf(newShelfId,shelfName));

}
void RegularUser::deleteShelf(int shelfId){
    auto it = std::remove_if(personalShelves.begin(), personalShelves.end(),//in tabe amade onsor ra be enteha montaghel mikonad
                             [shelfId](const Bookshelf &shelf) {
                                 return shelf.getShelfId() == shelfId;
                             });
    if (it != personalShelves.end()) {
        personalShelves.erase(it, personalShelves.end());//az it ta enteha ra hazf mikonad
    }
}
std::vector<Bookshelf>& RegularUser::getShelves(){
    return personalShelves;
}

void RegularUser::savePageLocation(int bookId, int pageNum){
    if(pageNum>1){
        BookmarkPages[bookId]=pageNum;
    }
}
int RegularUser::getPageLocation(int bookId) const{
    auto it=BookmarkPages.find(bookId);
    if(it!=BookmarkPages.end()){
        return it->second;
    }
    return 1;//ghabla baz nashode pas az safhe 1 shoro mishe
}