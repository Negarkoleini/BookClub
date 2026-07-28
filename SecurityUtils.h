#ifndef SECURITYUTILS_H
#define SECURITYUTILS_H
#include<string>

class SecurityUtils{
public:
    SecurityUtils()=delete;
    static std::string oneWayHash(const std::string &plainText);
    static bool verifyoneWayHash(const std::string &plainText,const std::string &hashed);
    static std::string twoWayEncrypt(const std::string &plainText);
    static std::string twoWayDecrypt(const std::string &cipherText);
};
#endif // SECURITYUTILS_H
