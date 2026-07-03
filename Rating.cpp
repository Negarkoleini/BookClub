#include "Rating.h"
#include <algorithm>

Rating::Rating(int bookId, int userId, int score)
    : bookId(bookId), userId(userId), score(std::clamp(score, 1, 5)) {
}

int Rating::getBookId() const { return bookId; }
int Rating::getUserId() const { return userId; }
int Rating::getScore() const { return score; }

void Rating::setScore(int newScore) {
    if (isValidScore(newScore)) {
        score = newScore;
    }
}

bool Rating::isValidScore(int score) {
    return score >= 1 && score <= 5;
}
