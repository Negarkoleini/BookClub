#ifndef TIMEDDISCOUNT_H
#define TIMEDDISCOUNT_H
#include<string>

enum class DiscountType{
    Percentage,Cash
};

class TimedDiscount{
private:
    int targetBookId;
    DiscountType discountType;
    double discountValue;
    std::string startDateTime;
    std::string endDateTime;
public:
    TimedDiscount(int targetBookId, double discountValue, std::string startDateTime, std::string endDateTime);
    int getTargetBookId() const;
    DiscountType getDiscountType() const;
    double getDiscountValue() const;
    bool isActiveNow(const std::string &currentSystemTime) const;
    double getDiscountedPrice(double originalPrice) const;

};
#endif // TIMEDDISCOUNT_H
