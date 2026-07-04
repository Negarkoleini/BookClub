#include "Publisher.h"
#include<algorithm>

Publisher::Publisher(int id, std::string username, std::string passwordHash, std::string email,
std::string registrationDate, std::string publisherName, std::string corporateId):
User(id,username,passwordHash,email,"Publisher",registrationDate),publisherName(publisherName),corporateId(corporateId){}

std::string Publisher::getPublisherName() const{
    return publisherName;
}
std::string Publisher::getCorporateId() const{
    return corporateId;
}
void Publisher::addPublishedBook(const Book &book){
    publishedBooks.push_back(book);
}
std::vector<Book> Publisher::getPublishedBooks() const{
    return publishedBooks;
}
bool Publisher::updateBookStatus(int bookId, bool active){
    for(auto &book:publishedBooks){
        if(book.getId()==bookId){
            book.setIsActive(active);
            return true;
        }
    }
    return false;
}
bool Publisher::softDeleteBook(int bookId){
    for(auto &book:publishedBooks){
        if(book.getId()==bookId){
            book.setDeleted(true);
            book.setActive(false);
            return true;
        }
    }
    return false;
}

void Publisher::createDiscount(const TimedDiscount &discount){
    activeDiscounts.push_back(discount);
}
std::vector<TimedDiscount> Publisher::getActiveDiscounts(){
    return activeDiscounts;
}

AnalyticsData Publisher::getAnalytics() const{
    return publisherAnalytics;
}
void Publisher::updateAnalytics(const AnalyticsData &newData){
    publisherAnalytics=newDate
        ;
}