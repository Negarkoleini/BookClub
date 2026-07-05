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

// ShoppingCart.cpp

// ۱. قیمت کل بدون هیچ تخفیفی (قیمت پایه کتاب‌ها)
double ShoppingCart::calculateTotal() const {
    double total = 0.0;
    for (const auto &item : items) {
        total += item.getBook().getBasePrice() * item.getQuantity();
    }
    return total;
}

// ۲. قیمت کل با اعمال آنی تخفیف‌های درون کتاب‌ها
double ShoppingCart::calculateTotalWithDiscounts(const std::string &currentSystemTime) const {
    double total = 0.0;
    for (const auto &item : items) {
        // درخواست قیمت نهایی مستقیم از خود کتاب
        double finalBookPrice = item.getBook().getFinalPrice(currentSystemTime);
        total += finalBookPrice * item.getQuantity();
    }
    return total;
}

// ۳. مجموع سود کاربر از تخفیف‌ها
double ShoppingCart::getTotalDiscountAmount(const std::string &currentSystemTime) const {
    double withoutDiscount = calculateTotal();
    double withDiscount = calculateTotalWithDiscounts(currentSystemTime);
    return withoutDiscount - withDiscount;
}