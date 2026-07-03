#include "Book.h"
#include "CartItem.h"
#include "ShoppingCart.h"
#include <algorithm>

ShoppingCart::ShoppingCart(int ownerUserId) : ownerUserId(ownerUserId) {
}

void ShoppingCart::addItem(const Book &book, int quantity) {
    for (auto &item : items) {
        if (item.getBook().getId() == book.getId()) {
            item.setQuantity(item.getQuantity() + quantity);
            return;
        }
    }
    items.emplace_back(book, quantity);
}

void ShoppingCart::removeItem(int bookId) {
    items.erase(std::remove_if(items.begin(), items.end(),
                               [bookId](const CartItem &item) { return item.getBook().getId() == bookId; }),
                items.end());
}

void ShoppingCart::clearAll() {
    items.clear();
}

std::vector<CartItem> ShoppingCart::getItems() const { return items; }

int ShoppingCart::getItemCount() const {
    return static_cast<int>(items.size());
}

double ShoppingCart::calculateTotal() const {
    double total = 0.0;
    for (const auto &item : items) {
        total += item.getSubtotal();
    }
    return total;
}

double ShoppingCart::calculateTotalWithDiscounts(const std::vector<TimedDiscount> &availableDiscounts,
                                                 const std::string &currentSystemTime) const {
    double total = 0.0;
    for (const auto &item : items) {
        double itemSubtotal = item.getSubtotal();
        for (const auto &discount : availableDiscounts) {
            if (discount.getTargetBookId() == item.getBook().getId() &&
                discount.isActiveNow(currentSystemTime)) {
                itemSubtotal = item.getSubtotalWithDiscount(discount, currentSystemTime);
                break;
            }
        }
        total += itemSubtotal;
    }
    return total;
}

double ShoppingCart::getTotalDiscountAmount(const std::vector<TimedDiscount> &availableDiscounts,
                                            const std::string &currentSystemTime) const {
    double withoutDiscount = calculateTotal();
    double withDiscount = calculateTotalWithDiscounts(availableDiscounts, currentSystemTime);
    return withoutDiscount - withDiscount;
}