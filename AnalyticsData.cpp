#include "AnalyticsData.h"

AnalyticsDate::AnalyticsDate(int publisherId):publisherId(publisherId),totalRevenue(0.0){}

void AnalyticsDate::registerSale(int bookId, double price, const std::string &date){
    if(price<0.0){
        return;
    }
    totalRevenue+=price;
    bookSalesVolume[bookId]++;
    dailyRevenueMap[date]+=price;
}
void AnalyticsDate::registerComment(int bookId){
    commentCountPerBook[bookId]++;
}
void AnalyticsDate::registerLibraryAdd(int bookId){
    libraryAddCountPerBook[bookId]++;
}

double AnalyticsDate::getTotalRevenue() const{
    return totalRevenue;
}
int AnalyticsDate::getPublisherId() const{
    return publisherId;
}
std::map<int, int> AnalyticsDate::getBookSalesVolume() const{
    return bookSalesVolume;
}
std::map<std::string, double> AnalyticsDate::getDailyRevenueMap() const{
    return dailyRevenueMap;
}
std::map<int, int> AnalyticsDate::getCommentCountPerBook() const{
    return commentCountPerBook;
}
std::map<int, int> AnalyticsDate::getLibraryAddCountPerBook() const{
    return libraryAddCountPerBook;
}