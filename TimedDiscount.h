#ifndef TIMEDDISCOUNT_H
#define TIMEDDISCOUNT_H
#include<string>

enum class DiscountType{
    Percentage,Cash
};

class TimedDiscount{
private:
    int DiscountId;
    static int NextDiscountId;
    int targetBookId;
    DiscountType discountType;
    double discountValue;
    std::string startDateTime;
    std::string endDateTime;
public:
    TimedDiscount(int targetBookId, double discountValue, std::string startDateTime, std::string endDateTime);
    std::string getStartDateTime() const;
    std::string getEndDateTime() const;
    int getDiscountId() const;
    int getTargetBookId() const;
    DiscountType getDiscountType() const;
    double getDiscountValue() const;
    bool isActiveNow(const std::string &currentSystemTime) const;
    double getDiscountedPrice(double originalPrice) const;

};
#endif // TIMEDDISCOUNT_H
