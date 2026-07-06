#include "Transaction.h"

Transaction::Transaction(int transactionId, int buyerUserId, int sellerPublisherId, int purchasedBookId,
 double finalAmountPaid, std::string transactionTime):
    transactionId(transactionId),
    buyerUserId(buyerUserId),
    sellerPublisherId(sellerPublisherId),
    purchasedBookId(purchasedBookId),
    finalAmountPaid(finalAmountPaid>=0.0?finalAmountPaid:0.0),
    transactionTime(transactionTime){}

int Transaction::getTransactionId() const{
    return transactionId;
}
int Transaction::getBuyerUserId() const{
    return buyerUserId;
}
int Transaction::getSellerPublisherId() const{
    return sellerPublisherId;
}
int Transaction::getPurchasedBookId() const{
    return purchasedBookId;
}
double Transaction::getFinalAmountPaid() const{
    return finalAmountPaid;
}
std::string Transaction::getTransactionTime() const{
    return transactionTime;
}