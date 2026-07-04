#ifndef USER_H
#define USER_H
#pragma once
#include<string>
enum class AccountStatus{
    Active,
    Blocked,
    TemporarilyDisabled,
};
class User{
private:
    int id;
    std::string username;
    std::string passwordHash;
    std::string email;
    std::string roleString;
    AccountStatus status;
    std::string registrationDate;
    std::string securityQuestion;
    std::string securityAnswerHash;
public:
    User(int id, std::string username, std::string passwordHash, std::string email,
        std::string roleString, std::string registrationDate);
    virtual ~User()=default;

    int getId()const;
    void setId(const int &newid);

    std::string getUsername()const;
    void setUsername(const std::string &uname);

    std::string getPasswordHash()const;
    void setPasswordHash(const std::string &pass);

    std::string getEmail()const;
    void setEmail(const std::string &newemail);

    std::string getRoleString()const;
    std::string getRegistrationDate() const;

    AccountStatus getStatus() const;
    void setStatus(AccountStatus s);

    std::string getSecurityQuestion()const;
    void setSecurityQuestion(const std::string &question,const std::string &answerHash);
    bool verifySecurityAnswer(const std::string &answerHash)const;
};
#endif // USER_H
