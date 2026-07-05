#ifndef TIMEDDISCOUNT_H
#define TIMEDDISCOUNT_H
#include<string>

class TimedDiscount{
private:
    int targetBookId;
    double discountPercentage;
    std::string startDateTime;
    std::string endDateTime;
public:
    TimedDiscount(int targetBookId, double discountPercentage, std::string startDateTime, std::string endDateTime);
    int getTargetBookId() const;
    double getDiscountPercentage() const;
    bool isActiveNow(const std::string &currentSystemTime) const;
    double getDiscountedPrice(double originalPrice) const;

};
#endif // TIMEDDISCOUNT_H
