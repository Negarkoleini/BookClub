#ifndef ANALYTICSDATA_H
#define ANALYTICSDATA_H
#include<string>
#include<map>

class AnalyticsData{
private:
    int publisherId;
    double totalRevenue;
    std::map<int, int> bookSalesVolume;
    std::map<std::string, double> dailyRevenueMap;
    std::map<int, int> commentCountPerBook;
    std::map<int, int> libraryAddCountPerBook;
public:
    AnalyticsData(int publisherId=0);

    void registerSale(int bookId, double price, const std::string &date);
    void registerComment(int bookId);
    void registerLibraryAdd(int bookId);

    double getTotalRevenue() const;
    int getPublisherId() const;
    std::map<int, int> getBookSalesVolume() const;
    std::map<std::string, double> getDailyRevenueMap() const;
    std::map<int, int> getCommentCountPerBook() const;
    std::map<int, int> getLibraryAddCountPerBook() const;
};
#endif // ANALYTICSDATA_H
