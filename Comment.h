#pragma once
#include <string>

// نمایانگر یک نظر ثبت‌شده برای یک کتاب
class Comment {
private:
    int commentId;
    int bookId;
    int userId;
    std::string senderUsername;
    std::string textContent;
    std::string timestamp;
    bool isApprovedByAdmin;
    bool isEdited;
    bool isDeleted; // حذف نرم (Soft Delete) توسط ادمین یا خود کاربر

public:
    Comment(int commentId, int bookId, int userId, std::string senderUsername,
            std::string textContent, std::string timestamp);

    int getCommentId() const;
    int getBookId() const;
    int getUserId() const;
    std::string getSenderUsername() const;
    std::string getTextContent() const;
    std::string getTimestamp() const;
    bool getIsApproved() const;
    void setApproved(bool approved);

    // ویرایش نظر توسط صاحب آن
    void editContent(const std::string &newText, const std::string &editTimestamp);
    bool getIsEdited() const;

    // حذف نظر توسط کاربر یا ادمین (بدون از بین بردن رکورد)
    void markDeleted();
    bool getIsDeleted() const;
};