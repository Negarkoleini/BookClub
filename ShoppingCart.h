#pragma once

#include "CartItem.h"
#include <vector>
#include <string>

class Bookshelf;

class ShoppingCart {
private:
    int ownerUserId;
    std::vector<CartItem> items;

public:
    ShoppingCart(int ownerUserId);

    void addItem(const Book &book, int quantity = 1);
    void removeItem(int bookId);
    void clearAll();

    const std::vector<CartItem>& getItems() const;
    int getItemCount() const;

    double calculateTotal() const;

    double calculateTotalWithDiscounts(const std::string &currentSystemTime) const;

    double getTotalDiscountAmount(const std::string &currentSystemTime) const;

    bool checkout(Bookshelf &library);
};