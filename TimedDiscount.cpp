#include "TimedDiscount.h"

TimedDiscount::TimedDiscount(int targetBookId, double discountPercentage, std::string startDateTime, std::string endDateTime):
targetBookId(targetBookId),
discountPercentage(discountPercentage>=0.0 && discountPercentage<=100?discountPercentage:0.0),
startDateTime(startDateTime),
endDateTime(endDateTime){}

int TimedDiscount::getTargetBookId() const{
    return targetBookId;
}
double TimedDiscount::getDiscountPercentage() const{
    return discountPercentage;
}
bool TimedDiscount::isActiveNow(const std::string &currentSystemTime) const{
    return currentSystemTime>=startDateTime && currentSystemTime<=endDateTime;
}
double TimedDiscount::getDiscountedPrice(double originalPrice) const{
    if (originalPrice < 0.0) {
        return 0.0;
    }
    double discountamount=originalPrice*(discountPercentage/100.0);
    return originalPrice-discountamount;
}