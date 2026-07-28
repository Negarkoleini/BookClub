#include "SecurityUtils.h"
#include<QCryptographicHash>
#include<QString>
#include<QByteArray>

const char ENCRYPTION_KEY = 0x5A;

std::string SecurityUtils::oneWayHash(const std::string &plainText){
    QByteArray data = QByteArray::fromStdString(plainText);
    QByteArray hashedData = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hashedData.toHex().toStdString();
}

bool SecurityUtils::verifyoneWayHash(const std::string &plainText,const std::string &hashed){
    return oneWayHash(plainText)==hashed;
}
std::string SecurityUtils::twoWayEncrypt(const std::string &plainText){
    QByteArray result;
    result.reserve(static_cast<int>(plainText.size()));

    for (char c : plainText) {
        result.append(c ^ ENCRYPTION_KEY);
    }

    return result.toHex().toStdString();
}
std::string SecurityUtils::twoWayDecrypt(const std::string &cipherText){
    QByteArray encryptedData = QByteArray::fromHex(QByteArray::fromStdString(cipherText));
    std::string plainText;
    plainText.reserve(encryptedData.size());

    for (int i = 0; i < encryptedData.size(); ++i) {
        plainText.push_back(encryptedData.at(i) ^ ENCRYPTION_KEY);
    }

    return plainText;
}