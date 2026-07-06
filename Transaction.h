#ifndef TRANSACTION_H
#define TRANSACTION_H
#include<string>

class Transaction{
private:
    int transactionId;
    int buyerUserId;
    int sellerPublisherId;
    int purchasedBookId;
    double finalAmountPaid;
    std::string transactionTime;
public:
    Transaction(int transactionId, int buyerUserId, int sellerPublisherId, int purchasedBookId,
                double finalAmountPaid, std::string transactionTime);

    int getTransactionId() const;
    int getBuyerUserId() const;
    int getSellerPublisherId() const;
    int getPurchasedBookId() const;
    double getFinalAmountPaid() const;
    std::string getTransactionTime() const;
};
#endif // TRANSACTION_H
