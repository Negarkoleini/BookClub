#include "AnalyticsData.h"

AnalyticsData::AnalyticsData(int publisherId):publisherId(publisherId),totalRevenue(0.0){}

void AnalyticsData::registerSale(int bookId, double price, const std::string &date){
    if(price<0.0){
        return;
    }
    totalRevenue+=price;
    bookSalesVolume[bookId]++;
    dailyRevenueMap[date]+=price;
}
void AnalyticsData::registerComment(int bookId){
    commentCountPerBook[bookId]++;
}
void AnalyticsData::registerLibraryAdd(int bookId){
    libraryAddCountPerBook[bookId]++;
}

double AnalyticsData::getTotalRevenue() const{
    return totalRevenue;
}
int AnalyticsData::getPublisherId() const{
    return publisherId;
}
std::map<int, int> AnalyticsData::getBookSalesVolume() const{
    return bookSalesVolume;
}
std::map<std::string, double> AnalyticsData::getDailyRevenueMap() const{
    return dailyRevenueMap;
}
std::map<int, int> AnalyticsData::getCommentCountPerBook() const{
    return commentCountPerBook;
}
std::map<int, int> AnalyticsData::getLibraryAddCountPerBook() const{
    return libraryAddCountPerBook;
}