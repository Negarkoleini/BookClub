#include "Admin.h"

Admin::Admin(int id,std::string username,std::string passwordHash,
             std::string email,std::string registrationDate,int SecurityLevel):User(id,username,passwordHash,email,"Admin",registrationDate),
    SecurityLevel(SecurityLevel){}

int Admin::getSecurityLevel()const{
    return SecurityLevel;
}
void Admin::setSecurityLevel(int Level){
    SecurityLevel=Level;
}

bool Admin::verifyComment(Comment &comment,bool approve){
    comment.setApproved(approve);
    return true;
}
bool Admin::banUserAccount(User &user) {
    if (user.getRoleString() == "Admin") {
        return false;
    }
    user.setStatus(AccountStatus::Blocked);
    return true;
}
bool Admin::unbanUserAccount(User &user){
    if(user.getStatus()==AccountStatus::Blocked){
        user.setStatus(AccountStatus::Active);
        return true;
    }
    return false;
}
bool Admin::temporarilyDisableAccount(User &user){
    if(user.getRoleString()=="Admin"){
        return false;
    }
    user.setStatus(AccountStatus::TemporarilyDisabled);
    return true;
}