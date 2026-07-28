#include "CartItem.h"
#include"Book.h"

CartItem::CartItem(const Book &book, int quantity)
    : book(book), quantity(quantity > 0 ? quantity : 1) {
}

double CartItem::getSubtotal() const {
    return book.getBasePrice() * quantity;
}

double CartItem::getSubtotalWithDiscount(const TimedDiscount &discount, const std::string &currentSystemTime) const {
    if (discount.getTargetBookId() != book.getId() || !discount.isActiveNow(currentSystemTime)) {
        return getSubtotal();
    }
    return discount.getDiscountedPrice(book.getBasePrice()) * quantity;
}

Book CartItem::getBook() const { return book; }
int CartItem::getQuantity() const { return quantity; }

void CartItem::setQuantity(int qty) {
    if (qty > 0) {
        quantity = qty;
    }
}