#include "User.h"

User::User(int id, std::string username, std::string passwordHash, std::string email,
           std::string roleString, std::string registrationDate)
    : id(id), username(username), passwordHash(passwordHash), email(email),
    roleString(roleString), registrationDate(registrationDate)
    {status=AccountStatus::Active;
    securityQuestion="";
    securityAnswerHash="";
    }

int User::getId() const {
    return id;
}

void User::setId(const int &newId) {
    id = newId;
}

std::string User::getUsername() const {
    return username;
}

void User::setUsername(const std::string &uname) {
    username = uname;
}

std::string User::getPasswordHash() const {
    return passwordHash;
}

void User::setPasswordHash(const std::string &pass) {
    passwordHash = pass;
}

std::string User::getEmail() const {
    return email;
}

void User::setEmail(const std::string &newEmail) {
    email = newEmail;
}

std::string User::getRoleString() const {
    return roleString;
}

std::string User::getRegistrationDate() const {
    return registrationDate;
}

AccountStatus User::getStatus() const {
    return status;
}

void User::setStatus(AccountStatus s) {
    status = s;
}

void User::setSecurityQuestion(const std::string &question, const std::string &answerHash) {
    securityQuestion = question;
    securityAnswerHash = answerHash;
}

std::string User::getSecurityQuestion() const {
    return securityQuestion;
}

std::string User::getSecurityAnswerHash() const {
    return securityAnswerHash;
}

bool User::verifySecurityAnswer(const std::string &answerHash) const {
    return securityAnswerHash == answerHash;
}
