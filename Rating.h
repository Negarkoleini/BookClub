#ifndef RATING_H
#define RATING_H

class Rating {
private:
    int bookId;
    int userId;
    int score;

public:
    Rating(int bookId, int userId, int score);
    int getBookId() const;
    int getUserId() const;
    int getScore() const;
    void setScore(int newScore);

    bool isValidScore(int score);
};
#endif // RATING_H
