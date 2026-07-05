#include "ReadingSession.h"
#include<algorithm>

ReadingSession::ReadingSession(int sessionId, int bookId, int hostUserId):
    sessionId(sessionId),bookId(bookId),hostUserId(hostUserId){
    syncedCurrentPage=1;
    participantUserIds.push_back(hostUserId);
}
int ReadingSession::getSessionId() const{
    return sessionId;
}
int ReadingSession::getBookId() const{
    return bookId;
}
int ReadingSession::getHostUserId() const{
    return hostUserId;
}

void ReadingSession::joinSession(int userId){
    if(!participantUserIds[userId]){
        participantUserIds.push_back(userId);
    }
}
void ReadingSession::leaveSession(int userId){
    auto it=std::find(participantUserIds.begin(),participantUserIds.end(),userId);
    if(it!=participantUserIds.end()){
        participantUserIds.erase(it);
    }
}

void ReadingSession::syncPage(int newPage){
    if(newPage>=1){
        syncedCurrentPage=newPage;
    }
}
int ReadingSession::getSyncedPage() const{
    return syncedCurrentPage;
}

std::vector<int> ReadingSession::getParticipants() const{
    return participantUserIds;
}
bool ReadingSession::isParticipant(int userId) const{
    return std::find(participantUserIds.begin(),participantUserIds.end(),userId)!=participantUserIds.end();
}