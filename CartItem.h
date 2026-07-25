#pragma once
#include "Book.h"
#include "TimedDiscount.h"

class CartItem {
private:
    Book book;
    int quantity;

public:
    CartItem(const Book &book, int quantity = 1);
    double getSubtotal() const; // بر اساس قیمت پایه
    double getSubtotalWithDiscount(const TimedDiscount &discount, const std::string &currentSystemTime) const;
    Book getBook() const;
    int getQuantity() const;
    void setQuantity(int qty);
};
