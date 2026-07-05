#ifndef READINGSESSION_H
#define READINGSESSION_H
#include<vector>

class ReadingSession{
private:
    int sessionId;
    int bookId;
    int hostUserId;
    std::vector<int> participantUserIds;
    int syncedCurrentPage;
public:
    ReadingSession(int sessionId, int bookId, int hostUserId);
    int getSessionId() const;
    int getBookId() const;
    int getHostUserId() const;

    void joinSession(int userId);
    void leaveSession(int userId);

    void syncPage(int newPage);
    int getSyncedPage() const;

    std::vector<int> getParticipants() const;
    bool isParticipant(int userId) const;
};
#endif // READINGSESSION_H
