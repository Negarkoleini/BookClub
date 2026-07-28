#include "Book.h"
#include <algorithm>
#include <numeric>
#include"Genre.h"

using namespace std;

int Book:: nextId = 1;

void Book::seedNextId(int startId) { if (startId >= nextId) nextId = startId; }

Book::Book(std::string title, std::string author, int publisherId, Genre genre,
           std::string description, double basePrice, std::string coverImagePath,
           std::string pdfFileName, std::string publishDate)
    : id(nextId), title(std::move(title)), author(std::move(author)), publisherId(publisherId),
    genre(genre), description(std::move(description)), basePrice(basePrice),
    coverImagePath(std::move(coverImagePath)), pdfFileName(std::move(pdfFileName)),
    publishDate(std::move(publishDate)), isActive(true), isDeleted(false), averageRating(0.0) {
    ++nextId;
}

int Book::getId() const { return id; }
std::string Book::getTitle() const { return title; }
std::string Book::getAuthor() const { return author; }
int Book::getPublisherId() const { return publisherId; }
Genre Book::getGenre() const { return genre; }
std::string Book::getDescription() const { return description; }
double Book::getBasePrice() const{ return basePrice; }
std::string Book::getCoverImagePath() const { return coverImagePath; }
std::string Book::getPdfFileName() const { return pdfFileName; }
std::string Book::getPublishDate() const { return publishDate; }

bool Book::isFree() const { return basePrice <= 0.0; }

bool Book::getIsActive() const { return isActive; }
void Book::setIsActive(bool active) { isActive = active; }
bool Book::getIsDeleted() const { return isDeleted; }
void Book::setIsDeleted(bool deleted) { isDeleted = deleted; }

void Book::setTitle(const std::string &newTitle) { title = newTitle; }
void Book::setAuthor(const std::string &newAuthor) { author = newAuthor; }
void Book::setGenre(Genre newGenre) { genre = newGenre; }
void Book::setDescription(const std::string &newDescription) { description = newDescription; }
void Book::setBasePrice(double newPrice) { if (newPrice >= 0) basePrice = newPrice; }
void Book::setCoverImagePath(const std::string &path) { coverImagePath = path; }
void Book::setPdfFileName(const std::string &fileName) { pdfFileName = fileName; }

void Book::setId(int newId)
{
    id = newId;

    if (nextId <= newId)
        nextId = newId + 1;
}

void Book::addRating(const Rating &rating) {
    userRatings.push_back(rating);
    updateAverageRating();
}

void Book::addcomment(int userId , Comment newComment){
    userComments.push_back(newComment);
}

bool Book::updateUserRating(int userId, int newScore) {
    for (auto &r : userRatings) {
        if (r.getUserId() == userId) {
            r.setScore(newScore);
            updateAverageRating();
            return true;
        }
    }
    return false;
}

std::vector<Rating> Book::getRatings() const { return userRatings; }

vector<Comment> Book::getComments() const{ return userComments; }

double Book::getAverageRating() const { return averageRating; }

void Book::updateAverageRating() {
    if (userRatings.empty()) {
        averageRating = 0.0;
        return;
    }
    int sum = std::accumulate(userRatings.begin(), userRatings.end(), 0,
                              [](int acc, const Rating &r) { return acc + r.getScore(); });
    averageRating = static_cast<double>(sum) / static_cast<double>(userRatings.size());
}

int Book::getRatingCount() const {
    return static_cast<int>(userRatings.size());
}


bool Book::removeDiscount(int discountId)
{
    auto it = std::remove_if(discounts.begin(), discounts.end(),[discountId](const TimedDiscount& d)
                             {
                                 return d.getDiscountId() == discountId;
                             });

    if (it == discounts.end())
        return false;

    discounts.erase(it, discounts.end());
    return true;
}

void Book::addDiscount(const TimedDiscount& discount) {
    discounts.push_back(discount);
}

double Book::getFinalPrice(const std::string& currentSystemTime) const {
    double finalPrice = basePrice;

    for (const auto& discount : discounts) {
        if (discount.isActiveNow(currentSystemTime)) {
            if (discount.getDiscountType() == DiscountType::Percentage) {
                // اعمال تخفیف درصدی (مثلاً ۲۰ درصد تخفیف)
                finalPrice -= (basePrice * (discount.getDiscountValue() / 100.0));
            } else if (discount.getDiscountType() == DiscountType::Cash) {
                // اعمال تخفیف مبلغی (مثلاً ۵۰ هزار تومان تخفیف)
                finalPrice -= discount.getDiscountValue();
            }
            // اگر داک گفته فقط یک تخفیف همزمان اعمال شود:
            break;
        }
    }

    // قیمت نباید منفی شود
    return finalPrice < 0.0 ? 0.0 : finalPrice;
}

Book::Book() {}

void Book::setAverageRating (double newavgrate){
    averageRating=newavgrate;
}
