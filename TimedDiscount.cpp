#include "TimedDiscount.h"

int TimedDiscount::NextDiscountId = 1;
TimedDiscount::TimedDiscount(int targetBookId, DiscountType discountType, double discountValue, std::string startDateTime, std::string endDateTime):
    targetBookId(targetBookId),
    discountType(discountType),
    discountValue(discountValue),
    startDateTime(startDateTime),
    endDateTime(endDateTime){
    DiscountId=NextDiscountId;
    NextDiscountId++;
}

int TimedDiscount::getTargetBookId() const{
    return targetBookId;
}
DiscountType TimedDiscount::getDiscountType() const {
    return discountType;
}
double TimedDiscount::getDiscountValue() const{
    return discountValue;
}

int TimedDiscount::getDiscountId() const{
    return DiscountId;
}
void TimedDiscount::setDiscountId(int id) {
    DiscountId = id;
}
std::string TimedDiscount::getStartDateTime() const
{
    return startDateTime;
}

std::string TimedDiscount::getEndDateTime() const
{
    return endDateTime;
}
bool TimedDiscount::isActiveNow(const std::string &currentSystemTime) const{
    return currentSystemTime>=startDateTime && currentSystemTime<=endDateTime;
}
double TimedDiscount::getDiscountedPrice(double originalPrice) const{
    double finalPrice = originalPrice;

    switch (discountType) {
    case DiscountType::Percentage: {
        double clampedPercentage = std::min(std::max(discountValue, 0.0), 100.0);
        finalPrice = originalPrice - (originalPrice * (clampedPercentage / 100.0));
        break;
    }
    case DiscountType::Cash: {
        finalPrice = originalPrice - discountValue;
        break;
    }
    }

    if (finalPrice < 0.0) {
        finalPrice = 0.0;
    }

    return finalPrice;
}