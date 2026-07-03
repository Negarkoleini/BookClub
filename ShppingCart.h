#pragma once
#include "CartItem.h"
#include "TimedDiscount.h"
#include <vector>

// سبد خرید کاربر - نگهداری اقلام، محاسبه‌ی مجموع و اعمال تخفیف‌های فعال
class ShoppingCart {
private:
    int ownerUserId;
    std::vector<CartItem> items;

public:
    ShoppingCart(int ownerUserId);
    void addItem(const Book &book, int quantity = 1);
    void removeItem(int bookId);
    void clearAll();
    std::vector<CartItem> getItems() const;
    int getItemCount() const;

    double calculateTotal() const; // بدون در نظر گرفتن تخفیف

    // مجموع نهایی با اعمال تخفیف‌های فعال روی هر کتاب (بخش ۲-۴ سند)
    double calculateTotalWithDiscounts(const std::vector<TimedDiscount> &availableDiscounts,
                                       const std::string &currentSystemTime) const;
    double getTotalDiscountAmount(const std::vector<TimedDiscount> &availableDiscounts,
                                  const std::string &currentSystemTime) const;
};
