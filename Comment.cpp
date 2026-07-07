#include "Comment.h"

Comment::Comment(int commentId, int bookId, int userId, std::string senderUsername,
                 std::string textContent, std::string timestamp)
    : commentId(commentId), bookId(bookId), userId(userId),
    senderUsername(std::move(senderUsername)), textContent(std::move(textContent)),
    timestamp(std::move(timestamp)), isApprovedByAdmin(false),
    isEdited(false), isDeleted(false) {
}

int Comment::getCommentId() const { return commentId; }
int Comment::getBookId() const { return bookId; }
int Comment::getUserId() const { return userId; }
std::string Comment::getSenderUsername() const { return senderUsername; }
std::string Comment::getTextContent() const { return textContent; }
std::string Comment::getTimestamp() const { return timestamp; }
bool Comment::getIsApproved() const { return isApprovedByAdmin; }
void Comment::setApproved(bool approved) { isApprovedByAdmin = approved; }

void Comment::editContent(const std::string &newText, const std::string &editTimestamp) {
    if (!newText.empty()) {
        textContent = newText;
        timestamp = editTimestamp;
        isEdited = true;
    }
}

bool Comment::getIsEdited() const { return isEdited; }

void Comment::markDeleted() { isDeleted = true; }
bool Comment::getIsDeleted() const { return isDeleted; }
