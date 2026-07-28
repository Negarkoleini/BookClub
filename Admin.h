#ifndef ADMIN_H
#define ADMIN_H
#pragma once
#include "User.h"
#include "Comment.h"
#include<vector>

class Admin:public User{
private:
    int SecurityLevel;
public:
    Admin(int id,std::string username,std::string passwordHash,
          std::string email,std::string registrationDate,int SecurityLevel=1);

    int getSecurityLevel()const;
    void setSecurityLevel(int Level);

    bool verifyComment(Comment &comment,bool approve);
    bool banUserAccount(User &user);
    bool unbanUserAccount(User &user);
    bool temporarilyDisableAccount(User &user);
};
#endif // ADMIN_H