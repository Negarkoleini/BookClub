#ifndef BOOK_H
#define BOOK_H

#pragma once
#include "Rating.h"
#include"Comment.h"
#include "Genre.h"
#include <string>
#include <vector>
#include "TimedDiscount"

using namespace std;

class Book {
private:
    int id;
    static int nextId;
    std::string title;
    std::string author;
    int publisherId;
    Genre genre;
    std::string description;
    double basePrice;
    std::string coverImagePath;
    std::string pdfFileName;
    std::string publishDate;
    bool isActive;
    bool isDeleted;
    std::vector<Rating> userRatings;
    vector<Comment> userComments;
    double averageRating;
    std::vector<TimedDiscount> discounts;

public:
    Book(std::string title, std::string author, int publisherId, Genre genre,
         std::string description, double basePrice, std::string coverImagePath,
         std::string pdfFileName, std::string publishDate);

    int getId() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    int getPublisherId() const;
    Genre getGenre() const;
    std::string getDescription() const;
    std::string getCoverImagePath() const;
    std::string getPdfFileName() const;
    std::string getPublishDate() const;
    double getBasePrice() const;
    bool isFree() const;
    void setTitle(const std::string &newTitle);
    void setAuthor(const std::string &newAuthor);
    void setGenre(Genre newGenre);
    void setDescription(const std::string &newDescription);
    void setBasePrice(double newPrice);
    void setCoverImagePath(const std::string &path);
    void setPdfFileName(const std::string &fileName);
    void addRating(const Rating &rating);
    bool updateUserRating(int userId, int newScore);
    void addcomment(int userId , Comment newComment);
    std::vector<Rating> getRatings() const ;
    vector<Comment> getComments() const;

    bool getIsActive() const;
    void setIsActive(bool active);
    bool getIsDeleted() const;
    void setIsDeleted(bool deleted);

    double getAverageRating() const;
    void updateAverageRating();
    int getRatingCount() const ;
    bool removeDiscount(int discountId);
    void addDiscount(const TimedDiscount& discount);
    double getFinalPrice(const std::string& currentSystemTime) const;
};

#endif // BOOK_H
