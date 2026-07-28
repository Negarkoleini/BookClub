#include "RequestProcessor.h"
#include "DatabaseManager.h"
#include "SessionManager.h"
#include "JsonPayLoad.h"

#include "SecurityUtils.h"
#include "RegularUser.h"
#include "Publisher.h"
#include "Admin.h"
#include "Book.h"
#include "Comment.h"
#include "Rating.h"
#include "Transaction.h"
#include "TimedDiscount.h"
#include "AppNotification.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

RequestProcessor::RequestProcessor(NotificationBroadcaster* broadcaster, QObject *parent)
    : QObject(parent), broadcaster(broadcaster) {
}

std::string RequestProcessor::currentTimestamp() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString();
}

bool RequestProcessor::isRequesterAdmin(ClientSocketWorker* sender) const {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) return false;
    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryById(userId, summary)) return false;
    return summary.role == "Admin";
}

void RequestProcessor::sendError(ClientSocketWorker* sender, CommandType cmd, const QString &message) {
    QJsonObject resp;
    resp["ok"] = false;
    resp["error"] = message;
    QMetaObject::invokeMethod(sender, "sendResponse", Qt::QueuedConnection,
                              Q_ARG(CommandType, cmd),
                              Q_ARG(QByteArray, JsonPayload::toQByteArray(resp)));
}

void RequestProcessor::sendOk(ClientSocketWorker* sender, CommandType cmd, const QJsonObject &extra) {
    QJsonObject resp = extra;
    resp["ok"] = true;
    QMetaObject::invokeMethod(sender, "sendResponse", Qt::QueuedConnection,
                              Q_ARG(CommandType, cmd),
                              Q_ARG(QByteArray, JsonPayload::toQByteArray(resp)));
}

// =========================================================================
// دیسپچر مرکزی: بر اساس commandType، متد process مربوطه را صدا می‌زند
// =========================================================================
void RequestProcessor::handleRequest(CommandType commandType, const QByteArray &payload, ClientSocketWorker* sender) {
    switch (commandType) {
    case CommandType::Login:                    processLogin(commandType, payload, sender); break;
    case CommandType::Register:                 processRegister(commandType, payload, sender); break;
    case CommandType::ForgotPasswordRequest:     processForgotPasswordRequest(commandType, payload, sender); break;
    case CommandType::ResetPassword:             processResetPassword(commandType, payload, sender); break;
    case CommandType::ChangePassword:            processChangePassword(commandType, payload, sender); break;
    case CommandType::GetProfile:                processGetProfile(commandType, payload, sender); break;
    case CommandType::UpdateProfile:              processUpdateProfile(commandType, payload, sender); break;
    case CommandType::SetFavoriteGenres:          processSetFavoriteGenres(commandType, payload, sender); break;
    case CommandType::DepositMoney:                processDepositMoney(commandType, payload, sender); break;


    case CommandType::GetBooks:                  processGetBooks(commandType, payload, sender); break;
    case CommandType::GetBookDetails:            processGetBookDetails(commandType, payload, sender); break;
    case CommandType::GetSuggestedBooks:          processGetSuggestedBooks(commandType, payload, sender); break;
    case CommandType::GetPopularBooks:            processGetPopularBooks(commandType, payload, sender); break;
    case CommandType::GetBestsellingBooks:        processGetBestsellingBooks(commandType, payload, sender); break;
    case CommandType::AddBook:                   processAddBook(commandType, payload, sender); break;
    case CommandType::EditBook:                  processEditBook(commandType, payload, sender); break;
    case CommandType::DeleteBook:                processDeleteBook(commandType, payload, sender); break;

    case CommandType::BuyBook:                   processBuyBook(commandType, payload, sender); break;

    case CommandType::GetLibrary:                processGetLibrary(commandType, payload, sender); break;
    case CommandType::SaveBookForLater:          processSaveBookForLater(commandType, payload, sender); break;
    case CommandType::RemoveSavedBook:           processRemoveSavedBook(commandType, payload, sender); break;
    case CommandType::CreateShelf:               processCreateShelf(commandType, payload, sender); break;
    case CommandType::DeleteShelf:               processDeleteShelf(commandType, payload, sender); break;
    case CommandType::AddBookToShelf:            processAddBookToShelf(commandType, payload, sender); break;
    case CommandType::RemoveBookFromShelf:       processRemoveBookFromShelf(commandType, payload, sender); break;
    case CommandType::GetShelves:                processGetShelves(commandType, payload, sender); break;
    case CommandType::RenameShelf:               processRenameShelf(commandType, payload, sender); break;
    case CommandType::MoveBookBetweenShelves:    processMoveBookBetweenShelves(commandType, payload, sender); break;
    case CommandType::SavePageLocation:          processSavePageLocation(commandType, payload, sender); break;
    case CommandType::GetPageLocation:           processGetPageLocation(commandType, payload, sender); break;

    case CommandType::AddComment:                processAddComment(commandType, payload, sender); break;
    case CommandType::AddRating:                 processAddRating(commandType, payload, sender); break;
    case CommandType::ApproveComment:            processApproveComment(commandType, payload, sender); break;
    case CommandType::GetPendingComments:        processGetPendingComments(commandType, payload, sender); break;
    case CommandType::RejectComment:             processRejectComment(commandType, payload, sender); break;
    case CommandType::DeleteComment:             processDeleteComment(commandType, payload, sender); break;
    case CommandType::EditComment:                processEditComment(commandType, payload, sender); break;
    case CommandType::GetAllComments:            processGetAllComments(commandType, payload, sender); break;

    case CommandType::GetNotifications:          processGetNotifications(commandType, payload, sender); break;
    case CommandType::MarkNotificationRead:      processMarkNotificationRead(commandType, payload, sender); break;

    case CommandType::ApplyDiscount:             processApplyDiscount(commandType, payload, sender); break;
    case CommandType::GetPublisherAnalytics:     processGetPublisherAnalytics(commandType, payload, sender); break;

    case CommandType::GetAllUsers:               processGetAllUsers(commandType, payload, sender); break;
    case CommandType::BanUser:                   processBanUser(commandType, payload, sender); break;
    case CommandType::UnbanUser:                 processUnbanUser(commandType, payload, sender); break;
    case CommandType::DisableUserTemporarily:    processDisableUserTemporarily(commandType, payload, sender); break;
    case CommandType::ChangeUserRole:            processChangeUserRole(commandType, payload, sender); break;
    case CommandType::DeleteUserAccount:         processDeleteUserAccount(commandType, payload, sender); break;
    case CommandType::GetUserDetails:            processGetUserDetails(commandType, payload, sender); break;

    case CommandType::GetAllBooksAdmin:          processGetAllBooksAdmin(commandType, payload, sender); break;

    case CommandType::GetPendingDiscounts:       processGetPendingDiscounts(commandType, payload, sender); break;
    case CommandType::ApproveDiscount:           processApproveDiscount(commandType, payload, sender); break;
    case CommandType::RejectDiscount:            processRejectDiscount(commandType, payload, sender); break;

    case CommandType::GetLimits:                 processGetLimits(commandType, payload, sender); break;
    case CommandType::SetLimits:                 processSetLimits(commandType, payload, sender); break;

    default:
        sendError(sender, commandType, "این دستور هنوز در سرور پیاده‌سازی نشده است.");
        break;
    }
}

// =========================================================================
// احراز هویت
// =========================================================================
void RequestProcessor::processLogin(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string username = req["username"].toString().toStdString();
    std::string password = req["password"].toString().toStdString();

    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryByUsername(username, summary)) {
        sendError(sender, cmd, "نام کاربری یا رمز عبور اشتباه است.");
        return;
    }
    if (!SecurityUtils::verifyoneWayHash(password, summary.passwordHash)) {
        sendError(sender, cmd, "نام کاربری یا رمز عبور اشتباه است.");
        return;
    }
    if (summary.status == AccountStatus::Blocked) {
        sendError(sender, cmd, "این حساب کاربری مسدود شده است.");
        return;
    }
    if (summary.status == AccountStatus::TemporarilyDisabled) {
        sendError(sender, cmd, "این حساب کاربری موقتاً غیرفعال شده است.");
        return;
    }

    sender->setAssociatedUserId(summary.id);
    SessionManager::getInstance().registerSession(summary.id, sender);
    DatabaseManager::getInstance().logLoginEvent(summary.id, currentTimestamp());

    QJsonObject resp;
    resp["userId"] = summary.id;
    resp["role"] = QString::fromStdString(summary.role);
    resp["email"] = QString::fromStdString(SecurityUtils::twoWayDecrypt(summary.email));
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processRegister(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string username = req["username"].toString().toStdString();
    std::string password = req["password"].toString().toStdString();
    std::string email = req["email"].toString().toStdString();
    std::string role = req["role"].toString().toStdString(); // "RegularUser" | "Publisher" | "Admin"
    std::string securityQuestion = req["securityQuestion"].toString().toStdString();
    std::string securityAnswer = req["securityAnswer"].toString().toStdString();

    if (username.empty() || password.empty()) {
        sendError(sender, cmd, "نام کاربری و رمز عبور نمی‌توانند خالی باشند.");
        return;
    }
    if (DatabaseManager::getInstance().usernameExists(username)) {
        sendError(sender, cmd, "این نام کاربری قبلاً ثبت شده است.");
        return;
    }

    // طبق داک: رمز عبور هش یک‌طرفه، بقیه‌ی اطلاعات (مثل ایمیل) هش دوطرفه (قابل بازگشت)
    std::string passwordHash = SecurityUtils::oneWayHash(password);
    std::string encryptedEmail = SecurityUtils::twoWayEncrypt(email);
    std::string answerHash = SecurityUtils::oneWayHash(securityAnswer); // مثل رمز عبور، فقط یک‌طرفه لازم است

    int newId = DatabaseManager::getInstance().getNextUserId();
    std::string regDate = currentTimestamp();

    bool ok = false;
    if (role == "Publisher") {
        std::string publisherName = req["publisherName"].toString().toStdString();
        std::string corporateId = req["corporateId"].toString().toStdString();
        Publisher pub(newId, username, passwordHash, encryptedEmail, regDate, publisherName, corporateId);
        pub.setSecurityQuestion(securityQuestion, answerHash);
        ok = DatabaseManager::getInstance().addPublisher(pub);
    } else if (role == "Admin") {
        Admin admin(newId, username, passwordHash, encryptedEmail, regDate);
        admin.setSecurityQuestion(securityQuestion, answerHash);
        ok = DatabaseManager::getInstance().addAdmin(admin);
    } else {
        RegularUser user(newId, username, passwordHash, encryptedEmail, regDate);
        user.setSecurityQuestion(securityQuestion, answerHash);
        ok = DatabaseManager::getInstance().addRegularUser(user);
    }

    if (!ok) {
        sendError(sender, cmd, "خطا در ثبت‌نام؛ لطفاً دوباره تلاش کنید.");
        return;
    }

    QJsonObject resp;
    resp["userId"] = newId;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processForgotPasswordRequest(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string username = req["username"].toString().toStdString();

    std::string question, answerHash;
    if (!DatabaseManager::getInstance().getSecurityQuestion(username, question, answerHash) || question.empty()) {
        sendError(sender, cmd, "کاربری با این نام یافت نشد یا سوال امنیتی ثبت نکرده است.");
        return;
    }
    QJsonObject resp;
    resp["securityQuestion"] = QString::fromStdString(question);
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processResetPassword(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string username = req["username"].toString().toStdString();
    std::string answer = req["securityAnswer"].toString().toStdString();
    std::string newPassword = req["newPassword"].toString().toStdString();

    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryByUsername(username, summary)) {
        sendError(sender, cmd, "کاربر یافت نشد.");
        return;
    }
    std::string question, storedAnswerHash;
    DatabaseManager::getInstance().getSecurityQuestion(username, question, storedAnswerHash);

    if (!SecurityUtils::verifyoneWayHash(answer, storedAnswerHash)) {
        sendError(sender, cmd, "پاسخ امنیتی صحیح نیست.");
        return;
    }
    std::string newHash = SecurityUtils::oneWayHash(newPassword);
    if (!DatabaseManager::getInstance().updatePasswordHash(summary.id, newHash)) {
        sendError(sender, cmd, "خطا در تغییر رمز عبور.");
        return;
    }
    sendOk(sender, cmd);
}

void RequestProcessor::processChangePassword(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string oldPassword = req["oldPassword"].toString().toStdString();
    std::string newPassword = req["newPassword"].toString().toStdString();

    UserSummary summary;
    DatabaseManager::getInstance().findUserSummaryById(sender->getAssociatedUserId(), summary);
    if (!SecurityUtils::verifyoneWayHash(oldPassword, summary.passwordHash)) {
        sendError(sender, cmd, "رمز عبور فعلی اشتباه است.");
        return;
    }
    DatabaseManager::getInstance().updatePasswordHash(summary.id, SecurityUtils::oneWayHash(newPassword));
    sendOk(sender, cmd);
}
void RequestProcessor::processGetProfile(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }

    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryById(userId, summary)) {
        sendError(sender, cmd, "کاربر یافت نشد.");
        return;
    }

    QJsonObject resp;
    resp["username"] = QString::fromStdString(summary.username);
    resp["email"] = QString::fromStdString(SecurityUtils::twoWayDecrypt(summary.email));
    resp["role"] = QString::fromStdString(summary.role);
    resp["registrationDate"] = QString::fromStdString(summary.registrationDate);

    if (summary.role == "RegularUser") {
        auto regUser = DatabaseManager::getInstance().loadRegularUser(userId);
        if (regUser) {
            resp["walletBalance"] = regUser->getWalletBalance();


            QJsonArray genresArr;
            for (Genre g : regUser->getFavoriteGenres()) {
                genresArr.append(static_cast<int>(g));
            }
            resp["favoriteGenres"] = genresArr;
            // ----------------------------------------------
        }
    }
    sendOk(sender, cmd, resp);
}
void RequestProcessor::processUpdateProfile(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }

    QJsonObject req = JsonPayload::fromBytes(data);
    if (req.contains("email")) {
        std::string encryptedEmail = SecurityUtils::twoWayEncrypt(req["email"].toString().toStdString());
        if (!DatabaseManager::getInstance().updateEmail(userId, encryptedEmail)) {
            sendError(sender, cmd, "خطا در بروزرسانیِ ایمیل.");
            return;
        }
    }
    sendOk(sender, cmd);
}

void RequestProcessor::processSetFavoriteGenres(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }

    QJsonObject req = JsonPayload::fromBytes(data);
    QJsonArray arr = req["genres"].toArray();

    if (arr.size() < 1 || arr.size() > 3) {
        sendError(sender, cmd, "باید بین ۱ تا ۳ ژانر انتخاب کنید.");
        return;
    }

    std::vector<Genre> genres;
    for (int i = 0; i < arr.size(); ++i) {
        genres.push_back(static_cast<Genre>(arr.at(i).toInt()));
    }

    if (!DatabaseManager::getInstance().updateFavoriteGenres(userId, genres)) {
        sendError(sender, cmd, "خطا در ذخیره‌ی ژانرها.");
        return;
    }
    sendOk(sender, cmd);
}
void RequestProcessor::processDepositMoney(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }

    QJsonObject req = JsonPayload::fromBytes(data);
    double amount = req["amount"].toDouble();
    if (amount <= 0.0) {
        sendError(sender, cmd, "مبلغِ شارژ باید بیشتر از صفر باشد.");
        return;
    }

    auto regUser = DatabaseManager::getInstance().loadRegularUser(userId);
    if (!regUser) {
        sendError(sender, cmd, "خطا در بارگذاریِ حساب کاربری.");
        return;
    }
    regUser->depositMoney(amount);
    if (!DatabaseManager::getInstance().updateWalletBalance(userId, regUser->getWalletBalance())) {
        sendError(sender, cmd, "خطا در ذخیره‌ی موجودی.");
        return;
    }

    QJsonObject resp;
    resp["newWalletBalance"] = regUser->getWalletBalance();
    sendOk(sender, cmd, resp);
}

// =========================================================================
// کتاب‌ها
// =========================================================================
static QJsonObject bookToJsonObject(const Book &b, const std::string &now) {
    QJsonObject bo;
    bo["id"] = b.getId();
    bo["title"] = QString::fromStdString(b.getTitle());
    bo["author"] = QString::fromStdString(b.getAuthor());
    bo["genre"] = static_cast<int>(b.getGenre());
    bo["basePrice"] = b.getBasePrice();
    bo["finalPrice"] = b.getFinalPrice(now);
    bo["coverImagePath"] = QString::fromStdString(b.getCoverImagePath());
    bo["averageRating"] = b.getAverageRating();
    bo["isFree"] = b.isFree();
    return bo;
}

void RequestProcessor::processGetBooks(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    QVector<Book> books = DatabaseManager::getInstance().getAllActiveBooks();
    QJsonArray arr;
    for (const auto &b : books) {
        arr.append(bookToJsonObject(b, currentTimestamp()));
    }
    QJsonObject resp;
    resp["books"] = arr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processGetSuggestedBooks(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }

    QJsonArray arr;
    for (const auto &b : DatabaseManager::getInstance().getSuggestedBooksForUser(userId)) {
        arr.append(bookToJsonObject(b, currentTimestamp()));
    }
    QJsonObject resp;
    resp["books"] = arr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processGetPopularBooks(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    QJsonArray arr;
    for (const auto &b : DatabaseManager::getInstance().getPopularBooks(20)) {
        arr.append(bookToJsonObject(b, currentTimestamp()));
    }
    QJsonObject resp;
    resp["books"] = arr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processGetBestsellingBooks(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    QJsonArray arr;
    for (const auto &b : DatabaseManager::getInstance().getBestsellingBooks(20)) {
        arr.append(bookToJsonObject(b, currentTimestamp()));
    }
    QJsonObject resp;
    resp["books"] = arr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processGetBookDetails(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();

    Book b;
    if (!DatabaseManager::getInstance().getBookById(bookId, b)) {
        sendError(sender, cmd, "کتاب یافت نشد.");
        return;
    }
    int viewerUserId = sender->getAssociatedUserId();
    QJsonArray commentsArr;
    for (const auto &c : DatabaseManager::getInstance().getCommentsForBook(bookId, viewerUserId)) {
        QJsonObject co;
        co["commentId"] = c.getCommentId();
        co["userId"] = c.getUserId();
        co["username"] = QString::fromStdString(c.getSenderUsername());
        co["text"] = QString::fromStdString(c.getTextContent());
        co["timestamp"] = QString::fromStdString(c.getTimestamp());
        co["isApproved"] = c.getIsApproved(); // اگر false باشد یعنی این نظر فقط برای صاحبش (در انتظار تایید) نمایش داده می‌شود
        commentsArr.append(co);
    }

    QJsonObject resp;
    resp["id"] = b.getId();
    resp["title"] = QString::fromStdString(b.getTitle());
    resp["author"] = QString::fromStdString(b.getAuthor());
    resp["description"] = QString::fromStdString(b.getDescription());
    resp["genre"] = static_cast<int>(b.getGenre());
    resp["finalPrice"] = b.getFinalPrice(currentTimestamp());
    resp["averageRating"] = b.getAverageRating();
    resp["comments"] = commentsArr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processAddBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    Book newBook(
        req["title"].toString().toStdString(),
        req["author"].toString().toStdString(),
        sender->getAssociatedUserId(),
        static_cast<Genre>(req["genre"].toInt()),
        req["description"].toString().toStdString(),
        req["basePrice"].toDouble(),
        req["coverImagePath"].toString().toStdString(),
        req["pdfFileName"].toString().toStdString(),
        currentTimestamp()
        );
    int newId = DatabaseManager::getInstance().addBook(newBook);
    if (newId < 0) {
        sendError(sender, cmd, "خطا در ثبت کتاب.");
        return;
    }

    // نکته‌ی مهم: اعلان باید برای *همه‌ی* کاربرانی که این ژانر جزو ژانرهای موردعلاقه‌شان است
    // ساخته و در دیتابیس ذخیره شود (چه در این لحظه آنلاین باشند چه نباشند)؛ در غیر این صورت
    // کاربرانی که هنگام انتشار کتاب آنلاین نبوده‌اند، بعداً هم که وارد شوند چیزی نخواهند دید.
    AppNotification notif = AppNotification::createNewBookNotification(0, -1, req["title"].toString().toStdString());
    for (int uid : DatabaseManager::getInstance().getUserIdsByFavoriteGenre(newBook.getGenre())) {
        AppNotification copy = AppNotification::fromStorage(0, notif.getType(), notif.getMessage(), uid, false, notif.getTimestamp());
        broadcaster->sendToUser(copy); // خودش هم ذخیره در دیتابیس و هم push آنی (اگر آنلاین باشد) را انجام می‌دهد
    }

    QJsonObject resp;
    resp["bookId"] = newId;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processEditBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();

    Book existing;
    if (!DatabaseManager::getInstance().getBookById(bookId, existing)) {
        sendError(sender, cmd, "کتاب یافت نشد.");
        return;
    }
    if (existing.getPublisherId() != sender->getAssociatedUserId() && !isRequesterAdmin(sender)) {
        sendError(sender, cmd, "شما اجازه‌ی ویرایش این کتاب را ندارید.");
        return;
    }
    if (req.contains("title")) existing.setTitle(req["title"].toString().toStdString());
    if (req.contains("author")) existing.setAuthor(req["author"].toString().toStdString());
    if (req.contains("description")) existing.setDescription(req["description"].toString().toStdString());
    if (req.contains("basePrice")) existing.setBasePrice(req["basePrice"].toDouble());

    if (!DatabaseManager::getInstance().updateBook(existing)) {
        sendError(sender, cmd, "خطا در ویرایش کتاب.");
        return;
    }

    if (req.contains("isActive") && isRequesterAdmin(sender)) {
        DatabaseManager::getInstance().setBookActive(bookId, req["isActive"].toBool());
    }
    sendOk(sender, cmd);
}

void RequestProcessor::processDeleteBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();

    Book existing;
    if (!DatabaseManager::getInstance().getBookById(bookId, existing)) {
        sendError(sender, cmd, "کتاب یافت نشد.");
        return;
    }
    if (existing.getPublisherId() != sender->getAssociatedUserId() && !isRequesterAdmin(sender)) {
        sendError(sender, cmd, "شما اجازه‌ی حذف این کتاب را ندارید.");
        return;
    }
    DatabaseManager::getInstance().softDeleteBook(bookId);
    sendOk(sender, cmd);
}


// =========================================================================
// خرید
// =========================================================================
void RequestProcessor::processBuyBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();
    int buyerId = sender->getAssociatedUserId();

    Book book;
    if (!DatabaseManager::getInstance().getBookById(bookId, book) || !book.getIsActive()) {
        sendError(sender, cmd, "کتاب موجود نیست.");
        return;
    }
    if (DatabaseManager::getInstance().isBookInLibrary(buyerId, bookId)) {
        sendError(sender, cmd, "این کتاب قبلاً خریداری شده است.");
        return;
    }


    int maxPurchasesPerDay = DatabaseManager::getInstance().getIntSetting("maxPurchasesPerDay", 1000);
    if (DatabaseManager::getInstance().getPurchaseCountToday(buyerId) >= maxPurchasesPerDay) {
        sendError(sender, cmd, QString("شما به سقفِ مجازِ خرید در امروز (%1 خرید) رسیده‌اید.").arg(maxPurchasesPerDay));
        return;
    }

    UserSummary buyer;
    DatabaseManager::getInstance().findUserSummaryById(buyerId, buyer);
    double price = book.getFinalPrice(currentTimestamp());


    auto regUser = DatabaseManager::getInstance().loadRegularUser(buyerId);
    if (!regUser) {
        sendError(sender, cmd, "فقط کاربران عادی می‌توانند خرید کنند.");
        return;
    }
    if (!regUser->withdrawMoney(price)) {
        sendError(sender, cmd, "موجودی کیف پول کافی نیست.");
        return;
    }
    DatabaseManager::getInstance().updateWalletBalance(buyerId, regUser->getWalletBalance());
    DatabaseManager::getInstance().addBookToLibrary(buyerId, bookId);

    Transaction tx(0, buyerId, book.getPublisherId(), bookId, price, currentTimestamp());
    DatabaseManager::getInstance().logTransaction(tx);

    // اعلان فروش جدید برای ناشر
    AppNotification saleNotif = AppNotification::createNewSaleNotification(0, book.getPublisherId(), book.getTitle());
    broadcaster->sendToUser(saleNotif);

    QJsonObject resp;
    resp["newWalletBalance"] = regUser->getWalletBalance();
    sendOk(sender, cmd, resp);
}

// =========================================================================
// کتابخانه شخصی
// =========================================================================
void RequestProcessor::processGetLibrary(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }

    QJsonArray purchasedArr;
    for (int bookId : DatabaseManager::getInstance().getPurchasedBookIds(userId)) {
        purchasedArr.append(bookId);
    }
    QJsonArray savedArr;
    for (int bookId : DatabaseManager::getInstance().getSavedBookIds(userId)) {
        savedArr.append(bookId);
    }

    QJsonObject resp;
    resp["purchasedBookIds"] = purchasedArr;
    resp["savedBookIds"] = savedArr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processSaveBookForLater(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    DatabaseManager::getInstance().saveBookForLater(userId, req["bookId"].toInt());
    sendOk(sender, cmd);
}

void RequestProcessor::processRemoveSavedBook(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    DatabaseManager::getInstance().removeSavedBook(userId, req["bookId"].toInt());
    sendOk(sender, cmd);
}

void RequestProcessor::processCreateShelf(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    int shelfId = DatabaseManager::getInstance().createShelf(userId, req["shelfName"].toString().toStdString());
    if (shelfId < 0) { sendError(sender, cmd, "خطا در ساختِ قفسه."); return; }
    QJsonObject resp;
    resp["shelfId"] = shelfId;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processDeleteShelf(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    int shelfId = req["shelfId"].toInt();
    if (DatabaseManager::getInstance().getShelfOwnerId(shelfId) != userId) {
        sendError(sender, cmd, "شما اجازه‌ی حذفِ این قفسه را ندارید.");
        return;
    }
    DatabaseManager::getInstance().deleteShelf(shelfId);
    sendOk(sender, cmd);
}

void RequestProcessor::processAddBookToShelf(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    int shelfId = req["shelfId"].toInt();
    int bookId = req["bookId"].toInt();
    if (DatabaseManager::getInstance().getShelfOwnerId(shelfId) != userId) {
        sendError(sender, cmd, "شما اجازه‌ی افزودنِ کتاب به این قفسه را ندارید.");
        return;
    }
    if (!DatabaseManager::getInstance().isBookInLibrary(userId, bookId)) {
        sendError(sender, cmd, "فقط کتاب‌های خریداری‌شده را می‌توانید به قفسه اضافه کنید.");
        return;
    }
    DatabaseManager::getInstance().addBookToShelf(shelfId, bookId);
    sendOk(sender, cmd);
}

void RequestProcessor::processRemoveBookFromShelf(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    int shelfId = req["shelfId"].toInt();
    if (DatabaseManager::getInstance().getShelfOwnerId(shelfId) != userId) {
        sendError(sender, cmd, "شما اجازه‌ی حذفِ کتاب از این قفسه را ندارید.");
        return;
    }
    DatabaseManager::getInstance().removeBookFromShelf(shelfId, req["bookId"].toInt());
    sendOk(sender, cmd);
}

void RequestProcessor::processGetShelves(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }

    QJsonArray shelvesArr;
    for (const auto &s : DatabaseManager::getInstance().getShelvesForUser(userId)) {
        QJsonObject so;
        so["shelfId"] = s.shelfId;
        so["shelfName"] = QString::fromStdString(s.shelfName);
        QJsonArray bookIdsArr;
        for (int bid : s.bookIds) bookIdsArr.append(bid);
        so["bookIds"] = bookIdsArr;
        shelvesArr.append(so);
    }
    QJsonObject resp;
    resp["shelves"] = shelvesArr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processRenameShelf(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    int shelfId = req["shelfId"].toInt();
    if (DatabaseManager::getInstance().getShelfOwnerId(shelfId) != userId) {
        sendError(sender, cmd, "شما اجازه‌ی تغییرِ نامِ این قفسه را ندارید.");
        return;
    }
    std::string newName = req["shelfName"].toString().toStdString();
    if (newName.empty()) {
        sendError(sender, cmd, "نامِ قفسه نمی‌تواند خالی باشد.");
        return;
    }
    DatabaseManager::getInstance().renameShelf(shelfId, newName);
    sendOk(sender, cmd);
}

void RequestProcessor::processMoveBookBetweenShelves(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    int fromShelfId = req["fromShelfId"].toInt();
    int toShelfId = req["toShelfId"].toInt();
    int bookId = req["bookId"].toInt();

    auto &dbm = DatabaseManager::getInstance();
    if (dbm.getShelfOwnerId(fromShelfId) != userId || dbm.getShelfOwnerId(toShelfId) != userId) {
        sendError(sender, cmd, "شما اجازه‌ی جابجاییِ کتاب بین این قفسه‌ها را ندارید.");
        return;
    }
    if (!dbm.moveBookBetweenShelves(fromShelfId, toShelfId, bookId)) {
        sendError(sender, cmd, "خطا در جابجاییِ کتاب بین قفسه‌ها.");
        return;
    }
    sendOk(sender, cmd);
}

void RequestProcessor::processSavePageLocation(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    DatabaseManager::getInstance().savePageLocation(userId, req["bookId"].toInt(), req["pageNum"].toInt());
    sendOk(sender, cmd);
}

void RequestProcessor::processGetPageLocation(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) { sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید."); return; }
    QJsonObject req = JsonPayload::fromBytes(data);
    int page = DatabaseManager::getInstance().getPageLocation(userId, req["bookId"].toInt());
    QJsonObject resp;
    resp["pageNum"] = page;
    sendOk(sender, cmd, resp);
}

// =========================================================================
// نظر و امتیاز
// =========================================================================
void RequestProcessor::processAddComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    int userId = sender->getAssociatedUserId();
    if (userId == -1) {
        sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }

    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();
    std::string text = req["text"].toString().toStdString();

    // ۱. اعتبارسنجی ورودی
    if (text.empty()) {
        sendError(sender, cmd, "متن نظر نمی‌تواند خالی باشد.");
        return;
    }

    // ۲. بررسی سقف روزانه
    int maxCommentsPerDay = DatabaseManager::getInstance().getIntSetting("maxCommentsPerDay", 1000);
    if (DatabaseManager::getInstance().getCommentCountToday(userId) >= maxCommentsPerDay) {
        sendError(sender, cmd, QString("شما به سقفِ مجازِ ثبتِ نظر در امروز (%1 نظر) رسیده‌اید.").arg(maxCommentsPerDay));
        return;
    }

    // ۳. دریافت اطلاعات کاربر
    UserSummary user;
    if (!DatabaseManager::getInstance().findUserSummaryById(userId, user)) {
        sendError(sender, cmd, "اطلاعات کاربر یافت نشد.");
        return;
    }

    // ۴. ثبت نظر در دیتابیس
    Comment c(0, bookId, userId, user.username, text, currentTimestamp());
    int newId = DatabaseManager::getInstance().addComment(c);
    if (newId <= 0) { // تغییر به <= 0
        sendError(sender, cmd, "خطا در ثبت نظر.");
        return;
    }

    // ۵. ارسال نوتیفیکیشن به ناشر کتاب
    Book book;
    if (broadcaster && DatabaseManager::getInstance().getBookById(bookId, book)) {
        AppNotification notif = AppNotification::createNewReviewNotification(0, book.getPublisherId(), book.getTitle());
        broadcaster->sendToUser(notif);
    }

    // ۶. ارسال پاسخ موفقیت به کلاینت
    QJsonObject resp;
    resp["commentId"] = newId;
    sendOk(sender, cmd, resp);
}
void RequestProcessor::processAddRating(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();
    int score = req["score"].toInt();
    if (score < 1 || score > 5) {
        sendError(sender, cmd, "امتیاز باید بین ۱ تا ۵ باشد.");
        return;
    }
    DatabaseManager::getInstance().upsertRating(bookId, sender->getAssociatedUserId(), score);

    // ارسال نوتیفیکیشن به ناشر کتاب برای امتیاز جدید
    Book book;
    if (broadcaster && DatabaseManager::getInstance().getBookById(bookId, book)) {
        AppNotification notif = AppNotification::createNewReviewNotification(0, book.getPublisherId(), book.getTitle());
        broadcaster->sendToUser(notif);
    }

    QJsonObject resp;
    resp["newAverage"] = DatabaseManager::getInstance().getAverageRating(bookId);
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processApproveComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند نظر را تأیید کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int commentId = req["commentId"].toInt();
    bool approve = req["approve"].toBool();
    DatabaseManager::getInstance().setCommentApproved(commentId, approve);
    sendOk(sender, cmd);
}

void RequestProcessor::processGetPendingComments(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    QJsonArray arr;
    for (const auto &c : DatabaseManager::getInstance().getPendingComments()) {
        QJsonObject co;
        co["commentId"] = c.getCommentId();
        co["bookId"] = c.getBookId();
        co["username"] = QString::fromStdString(c.getSenderUsername());
        co["text"] = QString::fromStdString(c.getTextContent());
        arr.append(co);
    }
    QJsonObject resp;
    resp["comments"] = arr;
    sendOk(sender, cmd, resp);
}

// =========================================================================
// اعلان‌ها
// =========================================================================
void RequestProcessor::processGetNotifications(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonArray arr;
    for (const auto &n : DatabaseManager::getInstance().getNotificationsForUser(sender->getAssociatedUserId())) {
        QJsonObject no;
        no["id"] = n.getId();
        no["type"] = static_cast<int>(n.getType());
        no["message"] = QString::fromStdString(n.getMessage());
        no["timestamp"] = QString::fromStdString(n.getTimestamp());
        no["isRead"] = n.getIsRead();
        arr.append(no);
    }
    QJsonObject resp;
    resp["notifications"] = arr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processMarkNotificationRead(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int notifId = req["notificationId"].toInt();
    DatabaseManager::getInstance().markNotificationRead(notifId);
    sendOk(sender, cmd);
}

// =========================================================================
// ناشر
// =========================================================================
void RequestProcessor::processApplyDiscount(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();

    Book book;
    if (!DatabaseManager::getInstance().getBookById(bookId, book)) {
        sendError(sender, cmd, "کتاب یافت نشد.");
        return;
    }
    if (book.getPublisherId() != sender->getAssociatedUserId()) {
        sendError(sender, cmd, "شما اجازه‌ی اعمالِ تخفیف روی این کتاب را ندارید.");
        return;
    }

    TimedDiscount discount(
        bookId,
        static_cast<DiscountType>(req["discountType"].toInt()), // 0=Percentage, 1=Cash (سمت کلاینت هم همین ترتیب)
        req["discountValue"].toDouble(),
        req["startDateTime"].toString().toStdString(),
        req["endDateTime"].toString().toStdString()
        );
    int discountId = DatabaseManager::getInstance().addDiscount(discount);
    if (discountId < 0) {
        sendError(sender, cmd, "خطا در ثبت تخفیف.");
        return;
    }



    QJsonObject resp;
    resp["discountId"] = discountId;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processGetPublisherAnalytics(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    int publisherId = sender->getAssociatedUserId();
    if (publisherId == -1) {
        sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QVector<Book> books = DatabaseManager::getInstance().getBooksByPublisher(publisherId);

    QJsonArray arr;
    int activeCount = 0;
    double totalRevenue = 0.0;
    int totalSales = 0;
    for (const auto &b : books) {
        int salesCount = 0;
        double revenue = 0.0;
        DatabaseManager::getInstance().getBookSalesInfo(b.getId(), salesCount, revenue);
        totalRevenue += revenue;
        totalSales += salesCount;

        QJsonObject bo;
        bo["bookId"] = b.getId();
        bo["title"] = QString::fromStdString(b.getTitle());
        bo["averageRating"] = b.getAverageRating();
        bo["isActive"] = b.getIsActive();
        bo["salesCount"] = salesCount;
        bo["revenue"] = revenue;
        if (b.getIsActive()) activeCount++;
        arr.append(bo);
    }
    QJsonObject resp;
    resp["books"] = arr;
    resp["totalBooks"] = books.size();
    resp["activeBooks"] = activeCount;
    resp["totalRevenue"] = totalRevenue;
    resp["totalSales"] = totalSales;
    sendOk(sender, cmd, resp);
}

// =========================================================================
// ادمین
// =========================================================================
void RequestProcessor::processGetAllUsers(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم به لیستِ کاربران دسترسی دارد.");
        return;
    }
    QJsonArray arr;
    for (const auto &u : DatabaseManager::getInstance().getAllUsers()) {
        QJsonObject uo;
        uo["id"] = u.id;
        uo["username"] = QString::fromStdString(u.username);
        uo["role"] = QString::fromStdString(u.role);
        uo["status"] = static_cast<int>(u.status);
        uo["registrationDate"] = QString::fromStdString(u.registrationDate);
        arr.append(uo);
    }
    QJsonObject resp;
    resp["users"] = arr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processBanUser(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند کاربر را مسدود کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();

    if (!DatabaseManager::getInstance().updateUserStatus(targetUserId, AccountStatus::Blocked)) {
        sendError(sender, cmd, "خطا در مسدودسازی کاربر.");
        return;
    }
    // اگر کاربر همین الان آنلاین است، فوراً قطعش کن (نگاه کن به بحثی که قبلاً داشتیم!)
    ClientSocketWorker* targetWorker = SessionManager::getInstance().getWorkerForUser(targetUserId);
    if (targetWorker) {
        QMetaObject::invokeMethod(targetWorker, "handleDisconnect", Qt::QueuedConnection);
    }
    sendOk(sender, cmd);
}

void RequestProcessor::processUnbanUser(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند مسدودیتِ کاربر را رفع کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();
    if (!DatabaseManager::getInstance().updateUserStatus(targetUserId, AccountStatus::Active)) {
        sendError(sender, cmd, "خطا در رفع مسدودیت کاربر.");
        return;
    }
    sendOk(sender, cmd);
}

void RequestProcessor::processDisableUserTemporarily(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند حسابِ کاربری را موقتاً غیرفعال کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();
    if (!DatabaseManager::getInstance().updateUserStatus(targetUserId, AccountStatus::TemporarilyDisabled)) {
        sendError(sender, cmd, "خطا در غیرفعال‌سازی موقت.");
        return;
    }
    sendOk(sender, cmd);
}

// =========================================================================
// ادمین: تغییر نقش / حذف کاربر / جزئیات کاربر
// =========================================================================
void RequestProcessor::processChangeUserRole(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند نقشِ کاربران را تغییر دهد.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();
    QString newRole = req["newRole"].toString(); // "RegularUser" | "Publisher" | "Admin"

    if (newRole != "RegularUser" && newRole != "Publisher" && newRole != "Admin") {
        sendError(sender, cmd, "نقشِ نامعتبر.");
        return;
    }
    if (!DatabaseManager::getInstance().updateUserRole(targetUserId, newRole.toStdString())) {
        sendError(sender, cmd, "خطا در تغییرِ نقش.");
        return;
    }
    sendOk(sender, cmd);
}

void RequestProcessor::processDeleteUserAccount(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند حسابِ کاربری حذف کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();

    // اگر همین الان آنلاین است، قبل از حذف قطعش کن
    ClientSocketWorker* targetWorker = SessionManager::getInstance().getWorkerForUser(targetUserId);
    if (targetWorker) {
        QMetaObject::invokeMethod(targetWorker, "handleDisconnect", Qt::QueuedConnection);
    }
    if (!DatabaseManager::getInstance().deleteUserAccount(targetUserId)) {
        sendError(sender, cmd, "خطا در حذفِ کاربر.");
        return;
    }
    sendOk(sender, cmd);
}

void RequestProcessor::processGetUserDetails(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم به جزئیاتِ کاربران دسترسی دارد.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();

    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryById(targetUserId, summary)) {
        sendError(sender, cmd, "کاربر یافت نشد.");
        return;
    }

    QJsonObject resp;
    resp["username"] = QString::fromStdString(summary.username);
    resp["email"] = QString::fromStdString(SecurityUtils::twoWayDecrypt(summary.email));
    resp["role"] = QString::fromStdString(summary.role);
    resp["status"] = static_cast<int>(summary.status);
    resp["registrationDate"] = QString::fromStdString(summary.registrationDate);

    // سوابقِ خرید (فقط برای کاربرانِ عادی معنا دارد)
    QJsonArray purchasedArr;
    for (int bookId : DatabaseManager::getInstance().getPurchasedBookIds(targetUserId)) {
        purchasedArr.append(bookId);
    }
    resp["purchasedBookIds"] = purchasedArr;

    // فعالیت‌ها: نظراتِ ثبت‌شده توسطِ این کاربر (نظراتی که خودش نوشته، نه نظراتِ دریافتی)
    QJsonArray commentsArr;
    for (const auto &c : DatabaseManager::getInstance().getAllComments(-1, targetUserId)) {
        QJsonObject co;
        co["bookId"] = c.getBookId();
        co["text"] = QString::fromStdString(c.getTextContent());
        co["timestamp"] = QString::fromStdString(c.getTimestamp());
        commentsArr.append(co);
    }
    resp["comments"] = commentsArr;

    // تاریخچه‌ی ورود
    QJsonArray loginArr;
    for (const auto &t : DatabaseManager::getInstance().getLoginHistory(targetUserId, 20)) {
        loginArr.append(QString::fromStdString(t));
    }
    resp["loginHistory"] = loginArr;

    if (summary.role == "RegularUser") {
        auto regUser = DatabaseManager::getInstance().loadRegularUser(targetUserId);
        if (regUser) resp["walletBalance"] = regUser->getWalletBalance();
    }

    // برای ناشر، "کتاب خریداری‌شده" و "نظرِ نوشته‌شده" بی‌معنی است (همیشه صفر می‌شد)؛
    // به‌جایش آمارِ واقعیِ کتاب‌های خودِ ناشر (فروش، نظراتِ دریافتی، امتیاز) نمایش داده می‌شود
    if (summary.role == "Publisher") {
        QVector<Book> books = DatabaseManager::getInstance().getBooksByPublisher(targetUserId);
        int totalSales = 0;
        double totalRevenue = 0.0;
        int totalCommentsReceived = 0;
        double ratingSum = 0.0;
        int ratedBooksCount = 0;

        QJsonArray publisherBooksArr;
        for (const auto &b : books) {
            int salesCount = 0;
            double revenue = 0.0;
            DatabaseManager::getInstance().getBookSalesInfo(b.getId(), salesCount, revenue);
            totalSales += salesCount;
            totalRevenue += revenue;

            int commentsForThisBook = DatabaseManager::getInstance().getAllComments(b.getId(), -1).size();
            totalCommentsReceived += commentsForThisBook;

            double avgRating = b.getAverageRating();
            if (avgRating > 0.0) { ratingSum += avgRating; ratedBooksCount++; }

            QJsonObject bo;
            bo["bookId"] = b.getId();
            bo["title"] = QString::fromStdString(b.getTitle());
            bo["salesCount"] = salesCount;
            bo["commentsCount"] = commentsForThisBook;
            bo["averageRating"] = avgRating;
            publisherBooksArr.append(bo);
        }

        QJsonObject publisherStats;
        publisherStats["publishedBooksCount"] = static_cast<int>(books.size());
        publisherStats["totalSales"] = totalSales;
        publisherStats["totalRevenue"] = totalRevenue;
        publisherStats["totalCommentsReceived"] = totalCommentsReceived;
        publisherStats["averageRatingAcrossBooks"] = ratedBooksCount > 0 ? (ratingSum / ratedBooksCount) : 0.0;
        publisherStats["books"] = publisherBooksArr;
        resp["publisherStats"] = publisherStats;
    }

    sendOk(sender, cmd, resp);
}

// =========================================================================
// ادمین: نظارت بر کتاب‌ها
// =========================================================================
void RequestProcessor::processGetAllBooksAdmin(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم به این بخش دسترسی دارد.");
        return;
    }
    QJsonArray arr;
    for (const auto &b : DatabaseManager::getInstance().getAllBooksAdmin()) {
        UserSummary pub;
        DatabaseManager::getInstance().findUserSummaryById(b.getPublisherId(), pub);

        QJsonObject bo;
        bo["id"] = b.getId();
        bo["title"] = QString::fromStdString(b.getTitle());
        bo["author"] = QString::fromStdString(b.getAuthor());
        bo["genre"] = static_cast<int>(b.getGenre());
        bo["basePrice"] = b.getBasePrice();
        bo["publisherUsername"] = QString::fromStdString(pub.username);
        bo["isActive"] = b.getIsActive();
        bo["isDeleted"] = b.getIsDeleted();
        bo["averageRating"] = b.getAverageRating();
        arr.append(bo);
    }
    QJsonObject resp;
    resp["books"] = arr;
    sendOk(sender, cmd, resp);
}

// =========================================================================
// ادمین: نظارت بر نظرات (تکمیلی)
// =========================================================================
void RequestProcessor::processRejectComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند نظر را رد کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    // «رد کردن» یعنی نامعتبر ماندنِ isApproved=false (برخلافِ حذف که کاملاً از دیدِ عموم پاک می‌کند)
    DatabaseManager::getInstance().setCommentApproved(req["commentId"].toInt(), false);
    sendOk(sender, cmd);
}

void RequestProcessor::processDeleteComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند نظر را حذف کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    DatabaseManager::getInstance().deleteComment(req["commentId"].toInt());
    sendOk(sender, cmd);
}
void RequestProcessor::processEditComment(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, cmd, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int commentId = req["commentId"].toInt();
    std::string newText = req["text"].toString().toStdString();

    // فقط خودِ نویسنده می‌تواند نظرش را ویرایش کند (نه حتی ادمین -- ادمین فقط حذف/تأیید می‌کند)
    int ownerId = DatabaseManager::getInstance().getCommentOwnerId(commentId);
    if (ownerId != sender->getAssociatedUserId()) {
        sendError(sender, cmd, "شما فقط می‌توانید نظرِ خودتان را ویرایش کنید.");
        return;
    }
    if (newText.empty()) {
        sendError(sender, cmd, "متنِ نظر نمی‌تواند خالی باشد.");
        return;
    }
    if (!DatabaseManager::getInstance().editCommentText(commentId, newText, currentTimestamp())) {
        sendError(sender, cmd, "خطا در ویرایشِ نظر.");
        return;
    }
    sendOk(sender, cmd);
}
void RequestProcessor::processGetAllComments(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم به این بخش دسترسی دارد.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int filterBookId = req.contains("bookId") ? req["bookId"].toInt() : -1;
    int filterUserId = req.contains("userId") ? req["userId"].toInt() : -1;

    QJsonArray arr;
    for (const auto &c : DatabaseManager::getInstance().getAllComments(filterBookId, filterUserId)) {
        QJsonObject co;
        co["commentId"] = c.getCommentId();
        co["bookId"] = c.getBookId();
        co["userId"] = c.getUserId();
        co["username"] = QString::fromStdString(c.getSenderUsername());
        co["text"] = QString::fromStdString(c.getTextContent());
        co["timestamp"] = QString::fromStdString(c.getTimestamp());
        co["isApproved"] = c.getIsApproved();
        arr.append(co);
    }
    QJsonObject resp;
    resp["comments"] = arr;
    sendOk(sender, cmd, resp);
}

// =========================================================================
// ادمین: تأیید/رد تخفیف‌های ناشران (بخشِ ۴-۲-ج سند)
// =========================================================================
void RequestProcessor::processGetPendingDiscounts(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم به این بخش دسترسی دارد.");
        return;
    }
    QJsonArray arr;
    for (const auto &d : DatabaseManager::getInstance().getPendingDiscounts()) {
        Book book;
        DatabaseManager::getInstance().getBookById(d.getTargetBookId(), book);

        QJsonObject dobj;
        dobj["discountId"] = d.getDiscountId();
        dobj["bookId"] = d.getTargetBookId();
        dobj["bookTitle"] = QString::fromStdString(book.getTitle());
        dobj["discountType"] = static_cast<int>(d.getDiscountType());
        dobj["discountValue"] = d.getDiscountValue();
        dobj["startDateTime"] = QString::fromStdString(d.getStartDateTime());
        dobj["endDateTime"] = QString::fromStdString(d.getEndDateTime());
        arr.append(dobj);
    }
    QJsonObject resp;
    resp["discounts"] = arr;
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processApproveDiscount(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند تخفیف را تأیید کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int discountId = req["discountId"].toInt();

    TimedDiscount discount(0, DiscountType::Percentage, 0.0, "", "");
    bool found = DatabaseManager::getInstance().getDiscountById(discountId, discount);

    if (!DatabaseManager::getInstance().setDiscountApproved(discountId, true)) {
        sendError(sender, cmd, "خطا در تأییدِ تخفیف.");
        return;
    }

    // فقط از همین لحظه (بعد از تاییدِ ادمین) به کاربرانِ آنلاین اطلاع می‌دهیم؛
    // نه موقعِ ثبتِ اولیه‌اش توسطِ ناشر (نگاه کن به کامنتِ processApplyDiscount).
    if (found) {
        Book book;
        if (DatabaseManager::getInstance().getBookById(discount.getTargetBookId(), book)) {
            AppNotification notif = AppNotification::createDiscountNotification(
                0, -1, book.getTitle(), discount.getDiscountValue());
            for (int uid : SessionManager::getInstance().getOnlineUserIds()) {
                AppNotification copy = AppNotification::fromStorage(
                    0, notif.getType(), notif.getMessage(), uid, false, notif.getTimestamp());
                broadcaster->sendToUser(copy);
            }
        }
    }

    sendOk(sender, cmd);
}

void RequestProcessor::processRejectDiscount(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند تخفیف را رد کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    DatabaseManager::getInstance().setDiscountApproved(req["discountId"].toInt(), false); // false داخلش یعنی حذف کن
    sendOk(sender, cmd);
}

// =========================================================================
// ادمین: محدودیت‌های سیستمی
// =========================================================================
void RequestProcessor::processGetLimits(CommandType cmd, const QByteArray & /*data*/, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم به این بخش دسترسی دارد.");
        return;
    }
    QJsonObject resp;
    resp["maxPurchasesPerDay"] = DatabaseManager::getInstance().getIntSetting("maxPurchasesPerDay", 1000);
    resp["maxCommentsPerDay"] = DatabaseManager::getInstance().getIntSetting("maxCommentsPerDay", 1000);
    sendOk(sender, cmd, resp);
}

void RequestProcessor::processSetLimits(CommandType cmd, const QByteArray &data, ClientSocketWorker* sender) {
    if (!isRequesterAdmin(sender)) {
        sendError(sender, cmd, "فقط مدیرِ سیستم می‌تواند محدودیت تنظیم کند.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    if (req.contains("maxPurchasesPerDay")) {
        DatabaseManager::getInstance().setIntSetting("maxPurchasesPerDay", req["maxPurchasesPerDay"].toInt());
    }
    if (req.contains("maxCommentsPerDay")) {
        DatabaseManager::getInstance().setIntSetting("maxCommentsPerDay", req["maxCommentsPerDay"].toInt());
    }
    sendOk(sender, cmd);
}

