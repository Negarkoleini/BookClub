#include "RequestProcessor.h"
#include "DatabaseManager.h"
#include "SessionManager.h"
#include "JsonPayload.h"

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
#include <QJsonDocument>

RequestProcessor::RequestProcessor(NotificationBroadcaster* broadcaster, QObject *parent)
    : QObject(parent), broadcaster(broadcaster) {
}

std::string RequestProcessor::currentTimestamp() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString();
}
void RequestProcessor::sendError(ClientSocketWorker* sender, const QString &message) {
    QJsonObject resp;
    resp["ok"] = false;
    resp["error"] = message;

    QMetaObject::invokeMethod(sender, "sendResponse", Qt::QueuedConnection,
                              Q_ARG(CommandType, CommandType::ResponseError),
                              Q_ARG(QByteArray, JsonPayload::toQByteArray(resp)));
}
void RequestProcessor::sendOk(ClientSocketWorker* sender, const QJsonObject &extra) {
    QJsonObject resp = extra;
    resp["ok"] = true;
    QMetaObject::invokeMethod(sender, "sendResponse", Qt::QueuedConnection,
                              Q_ARG(CommandType, CommandType::ResponseOk),
                              Q_ARG(QByteArray, JsonPayload::toQByteArray(resp)));
}
void RequestProcessor::handleRequest(CommandType commandType, const QByteArray &payload, ClientSocketWorker* sender) {
    switch (commandType) {
    case CommandType::Login:                    processLogin(payload, sender); break;
    case CommandType::Register:                 processRegister(payload, sender); break;
    case CommandType::ForgotPasswordRequest:     processForgotPasswordRequest(payload, sender); break;
    case CommandType::ResetPassword:             processResetPassword(payload, sender); break;
    case CommandType::ChangePassword:            processChangePassword(payload, sender); break;

    case CommandType::GetBooks:                  processGetBooks(payload, sender); break;
    case CommandType::GetBookDetails:            processGetBookDetails(payload, sender); break;
    case CommandType::AddBook:                   processAddBook(payload, sender); break;
    case CommandType::EditBook:                  processEditBook(payload, sender); break;
    case CommandType::DeleteBook:                processDeleteBook(payload, sender); break;

    case CommandType::BuyBook:                   processBuyBook(payload, sender); break;

    case CommandType::AddComment:                processAddComment(payload, sender); break;
    case CommandType::AddRating:                 processAddRating(payload, sender); break;
    case CommandType::ApproveComment:            processApproveComment(payload, sender); break;
    case CommandType::GetPendingComments:        processGetPendingComments(payload, sender); break;

    case CommandType::ApplyDiscount:             processApplyDiscount(payload, sender); break;
    case CommandType::GetPublisherAnalytics:     processGetPublisherAnalytics(payload, sender); break;

    case CommandType::GetAllUsers:               processGetAllUsers(payload, sender); break;
    case CommandType::BanUser:                   processBanUser(payload, sender); break;
    case CommandType::UnbanUser:                 processUnbanUser(payload, sender); break;
    case CommandType::DisableUserTemporarily:    processDisableUserTemporarily(payload, sender); break;

    default:
        sendError(sender, "این دستور هنوز در سرور پیاده سازی نشده");
        break;
    }
}
//احراز هویت
void RequestProcessor::processLogin(const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string username = req["username"].toString().toStdString();
    std::string password = req["password"].toString().toStdString();

    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryByUsername(username, summary)) {
        sendError(sender, "نام کاربری یا رمز عبور اشتباه است.");
        return;
    }
    if (!SecurityUtils::verifyoneWayHash(password, summary.passwordHash)) {
        sendError(sender, "نام کاربری یا رمز عبور اشتباه است.");
        return;
    }
    if (summary.status == AccountStatus::Blocked) {
        sendError(sender, "این حساب کاربری مسدود شده است.");
        return;
    }
    if (summary.status == AccountStatus::TemporarilyDisabled) {
        sendError(sender, "این حساب کاربری موقتاً غیرفعال شده است.");
        return;
    }

    sender->setAssociatedUserId(summary.id);
    SessionManager::getInstance().registerSession(summary.id, sender);

    QJsonObject resp;
    resp["userId"] = summary.id;
    resp["role"] = QString::fromStdString(summary.role);
    resp["email"] = QString::fromStdString(SecurityUtils::twoWayDecrypt(summary.email));
    sendOk(sender, resp);
}
void RequestProcessor::processRegister(const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string username = req["username"].toString().toStdString();
    std::string password = req["password"].toString().toStdString();
    std::string email = req["email"].toString().toStdString();
    std::string role = req["role"].toString().toStdString();
    std::string securityQuestion = req["securityQuestion"].toString().toStdString();
    std::string securityAnswer = req["securityAnswer"].toString().toStdString();

    if (username.empty() || password.empty()) {
        sendError(sender, "نام کاربری و رمز عبور نمی‌توانند خالی باشند.");
        return;
    }
    if (DatabaseManager::getInstance().usernameExists(username)) {
        sendError(sender, "این نام کاربری قبلاً ثبت شده است.");
        return;
    }
    std::string passwordHash = SecurityUtils::oneWayHash(password);
    std::string encryptedEmail = SecurityUtils::twoWayEncrypt(email);
    std::string answerHash = SecurityUtils::oneWayHash(securityAnswer);

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
        sendError(sender, "خطا در ثبت‌نام؛ لطفاً دوباره تلاش کنید.");
        return;
    }

    QJsonObject resp;
    resp["userId"] = newId;
    sendOk(sender, resp);
}
void RequestProcessor::processForgotPasswordRequest(const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string username = req["username"].toString().toStdString();

    std::string question, answerHash;
    if (!DatabaseManager::getInstance().getSecurityQuestion(username, question, answerHash) || question.empty()) {
        sendError(sender, "کاربری با این نام یافت نشد یا سوال امنیتی ثبت نکرده است.");
        return;
    }
    QJsonObject resp;
    resp["securityQuestion"] = QString::fromStdString(question);
    sendOk(sender, resp);
}
void RequestProcessor::processResetPassword(const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string username = req["username"].toString().toStdString();
    std::string answer = req["securityAnswer"].toString().toStdString();
    std::string newPassword = req["newPassword"].toString().toStdString();

    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryByUsername(username, summary)) {
        sendError(sender, "کاربر یافت نشد.");
        return;
    }
    std::string question, storedAnswerHash;
    DatabaseManager::getInstance().getSecurityQuestion(username, question, storedAnswerHash);

    if (!SecurityUtils::verifyoneWayHash(answer, storedAnswerHash)) {
        sendError(sender, "پاسخ امنیتی صحیح نیست.");
        return;
    }
    std::string newHash = SecurityUtils::oneWayHash(newPassword);
    if (!DatabaseManager::getInstance().updatePasswordHash(summary.id, newHash)) {
        sendError(sender, "خطا در تغییر رمز عبور.");
        return;
    }
    sendOk(sender);
}
void RequestProcessor::processChangePassword(const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    std::string oldPassword = req["oldPassword"].toString().toStdString();
    std::string newPassword = req["newPassword"].toString().toStdString();

    UserSummary summary;
    DatabaseManager::getInstance().findUserSummaryById(sender->getAssociatedUserId(), summary);
    if (!SecurityUtils::verifyoneWayHash(oldPassword, summary.passwordHash)) {
        sendError(sender, "رمز عبور فعلی اشتباه است.");
        return;
    }
    DatabaseManager::getInstance().updatePasswordHash(summary.id, SecurityUtils::oneWayHash(newPassword));
    sendOk(sender);
}
//کتاب
void RequestProcessor::processGetBooks(const QByteArray & data, ClientSocketWorker* sender) {
    QVector<Book> books = DatabaseManager::getInstance().getAllActiveBooks();
    QJsonArray arr;
    for (const auto &b : books) {
        QJsonObject bo;
        bo["id"] = b.getId();
        bo["title"] = QString::fromStdString(b.getTitle());
        bo["author"] = QString::fromStdString(b.getAuthor());
        bo["genre"] = static_cast<int>(b.getGenre());
        bo["basePrice"] = b.getBasePrice();
        bo["finalPrice"] = b.getFinalPrice(currentTimestamp());
        bo["coverImagePath"] = QString::fromStdString(b.getCoverImagePath());
        bo["averageRating"] = b.getAverageRating();
        bo["isFree"] = b.isFree();
        arr.append(bo);
    }
    QJsonObject resp;
    resp["books"] = arr;
    sendOk(sender, resp);
}
void RequestProcessor::processGetBookDetails(const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();

    Book b;
    if (!DatabaseManager::getInstance().getBookById(bookId, b)) {
        sendError(sender, "کتاب یافت نشد.");
        return;
    }
    QJsonArray commentsArr;
    for (const auto &c : DatabaseManager::getInstance().getCommentsForBook(bookId)) {
        QJsonObject co;
        co["commentId"] = c.getCommentId();
        co["username"] = QString::fromStdString(c.getSenderUsername());
        co["text"] = QString::fromStdString(c.getTextContent());
        co["timestamp"] = QString::fromStdString(c.getTimestamp());
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
    sendOk(sender, resp);
}

void RequestProcessor::processAddBook(const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
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
        sendError(sender, "خطا در ثبت کتاب.");
        return;
    }
    AppNotification notif = AppNotification::createNewBookNotification(0, -1, req["title"].toString().toStdString());
    for (int uid : SessionManager::getInstance().getOnlineUserIds()) {
        AppNotification copy = AppNotification::fromStorage(0, notif.getType(), notif.getMessage(), uid, false, notif.getTimestamp());
        broadcaster->sendToUser(copy);
    }

    QJsonObject resp;
    resp["bookId"] = newId;
    sendOk(sender, resp);
}
void RequestProcessor::processEditBook(const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();

    Book existing;
    if (!DatabaseManager::getInstance().getBookById(bookId, existing)) {
        sendError(sender, "کتاب یافت نشد.");
        return;
    }
    if (existing.getPublisherId() != sender->getAssociatedUserId()) {
        sendError(sender, "شما اجازه‌ی ویرایش این کتاب را ندارید.");
        return;
    }
    if (req.contains("title")) existing.setTitle(req["title"].toString().toStdString());
    if (req.contains("author")) existing.setAuthor(req["author"].toString().toStdString());
    if (req.contains("description")) existing.setDescription(req["description"].toString().toStdString());
    if (req.contains("basePrice")) existing.setBasePrice(req["basePrice"].toDouble());

    if (!DatabaseManager::getInstance().updateBook(existing)) {
        sendError(sender, "خطا در ویرایش کتاب.");
        return;
    }
    sendOk(sender);
}
void RequestProcessor::processDeleteBook(const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();

    Book existing;
    if (!DatabaseManager::getInstance().getBookById(bookId, existing)) {
        sendError(sender, "کتاب یافت نشد.");
        return;
    }
    if (existing.getPublisherId() != sender->getAssociatedUserId()) {
        sendError(sender, "شما اجازه‌ی حذف این کتاب را ندارید.");
        return;
    }
    DatabaseManager::getInstance().softDeleteBook(bookId);
    sendOk(sender);
}
//خرید
void RequestProcessor::processBuyBook(const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();
    int buyerId = sender->getAssociatedUserId();

    Book book;
    if (!DatabaseManager::getInstance().getBookById(bookId, book) || !book.getIsActive()) {
        sendError(sender, "کتاب موجود نیست.");
        return;
    }
    if (DatabaseManager::getInstance().isBookInLibrary(buyerId, bookId)) {
        sendError(sender, "این کتاب قبلاً خریداری شده است.");
        return;
    }

    UserSummary buyer;
    DatabaseManager::getInstance().findUserSummaryById(buyerId, buyer);
    double price = book.getFinalPrice(currentTimestamp());


    auto regUser = DatabaseManager::getInstance().loadRegularUser(buyerId);
    if (!regUser) {
        sendError(sender, "فقط کاربران عادی می‌توانند خرید کنند.");
        return;
    }
    if (!regUser->withdrawMoney(price)) {
        sendError(sender, "موجودی کیف پول کافی نیست.");
        return;
    }
    DatabaseManager::getInstance().updateWalletBalance(buyerId, regUser->getWalletBalance());
    DatabaseManager::getInstance().addBookToLibrary(buyerId, bookId);

    Transaction tx(0, buyerId, book.getPublisherId(), bookId, price, currentTimestamp());
    DatabaseManager::getInstance().logTransaction(tx);
        AppNotification saleNotif = AppNotification::createNewSaleNotification(0, book.getPublisherId(), book.getTitle());
    broadcaster->sendToUser(saleNotif);

    QJsonObject resp;
    resp["newWalletBalance"] = regUser->getWalletBalance();
    sendOk(sender, resp);
}
//نظر و امتیاز
void RequestProcessor::processAddComment(const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();
    std::string text = req["text"].toString().toStdString();
    int userId = sender->getAssociatedUserId();

    UserSummary user;
    DatabaseManager::getInstance().findUserSummaryById(userId, user);

    Comment c(0, bookId, userId, user.username, text, currentTimestamp());
    int newId = DatabaseManager::getInstance().addComment(c);
    if (newId < 0) {
        sendError(sender, "خطا در ثبت نظر.");
        return;
    }

    Book book;
    if (DatabaseManager::getInstance().getBookById(bookId, book)) {
        AppNotification notif = AppNotification::createNewReviewNotification(0, book.getPublisherId(), book.getTitle());
        broadcaster->sendToUser(notif);
    }

    QJsonObject resp;
    resp["commentId"] = newId;
    sendOk(sender, resp);
}
void RequestProcessor::processAddRating(const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();
    int score = req["score"].toInt();
    if (score < 1 || score > 5) {
        sendError(sender, "امتیاز باید بین ۱ تا ۵ باشد.");
        return;
    }
    DatabaseManager::getInstance().upsertRating(bookId, sender->getAssociatedUserId(), score);

    QJsonObject resp;
    resp["newAverage"] = DatabaseManager::getInstance().getAverageRating(bookId);
    sendOk(sender, resp);
}
void RequestProcessor::processApproveComment(const QByteArray &data,ClientSocketWorker* sender)
{
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }

    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryById(
            sender->getAssociatedUserId(), summary))
    {
        sendError(sender, "کاربر یافت نشد.");
        return;
    }

    if (summary.role != "Admin") {
        sendError(sender, "فقط مدیر سیستم اجازه انجام این عملیات را دارد.");
        return;
    }

    QJsonObject req = JsonPayload::fromBytes(data);

    int commentId = req["commentId"].toInt();
    bool approve = req["approve"].toBool();

    DatabaseManager::getInstance().setCommentApproved(commentId, approve);

    sendOk(sender);
}
void RequestProcessor::processGetPendingComments(const QByteArray & data, ClientSocketWorker* sender) {
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
    sendOk(sender, resp);
}
//ناشر
void RequestProcessor::processApplyDiscount(const QByteArray &data, ClientSocketWorker* sender) {
    QJsonObject req = JsonPayload::fromBytes(data);
    int bookId = req["bookId"].toInt();

    Book book;
    if (!DatabaseManager::getInstance().getBookById(bookId, book)) {
        sendError(sender, "کتاب یافت نشد.");
        return;
    }
    if (book.getPublisherId() != sender->getAssociatedUserId()) {
        sendError(sender, "شما اجازه‌ی اعمالِ تخفیف روی این کتاب را ندارید.");
        return;
    }

    TimedDiscount discount(
        bookId,
        req["discountValue"].toDouble(),
        req["startDateTime"].toString().toStdString(),
        req["endDateTime"].toString().toStdString()
        );
    int discountId = DatabaseManager::getInstance().addDiscount(discount);
    if (discountId < 0) {
        sendError(sender, "خطا در ثبت تخفیف.");
        return;
    }

    AppNotification notif = AppNotification::createDiscountNotification(0, -1, book.getTitle(), req["discountValue"].toDouble());
    for (int uid : SessionManager::getInstance().getOnlineUserIds()) {
        AppNotification copy = AppNotification::fromStorage(0, notif.getType(), notif.getMessage(), uid, false, notif.getTimestamp());
        broadcaster->sendToUser(copy);
    }

    QJsonObject resp;
    resp["discountId"] = discountId;
    sendOk(sender, resp);
}
void RequestProcessor::processGetPublisherAnalytics(const QByteArray & data, ClientSocketWorker* sender) {
    int publisherId = sender->getAssociatedUserId();
    if (publisherId == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    QVector<Book> books = DatabaseManager::getInstance().getBooksByPublisher(publisherId);

    QJsonArray arr;
    int activeCount = 0;
    for (const auto &b : books) {
        QJsonObject bo;
        bo["bookId"] = b.getId();
        bo["title"] = QString::fromStdString(b.getTitle());
        bo["averageRating"] = b.getAverageRating();
        bo["isActive"] = b.getIsActive();
        if (b.getIsActive()) activeCount++;
        arr.append(bo);
    }
    QJsonObject resp;
    resp["books"] = arr;
    resp["totalBooks"] = books.size();
    resp["activeBooks"] = activeCount;
    sendOk(sender, resp);
}
//ادمین
void RequestProcessor::processGetAllUsers(const QByteArray & /*data*/, ClientSocketWorker* sender) {
        if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
        UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryById(
            sender->getAssociatedUserId(), summary)) {
        sendError(sender, "کاربر یافت نشد.");
        return;
    }
        if (summary.role !="Admin") {
        sendError(sender, "فقط مدیر سیستم اجازه انجام این عملیات را دارد.");
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
    sendOk(sender, resp);
}
void RequestProcessor::processBanUser(const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryById(
            sender->getAssociatedUserId(), summary)) {
        sendError(sender, "کاربر یافت نشد.");
        return;
    }
    if (summary.role !="Admin") {
        sendError(sender, "فقط مدیر سیستم اجازه انجام این عملیات را دارد.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();

    if (!DatabaseManager::getInstance().updateUserStatus(targetUserId, AccountStatus::Blocked)) {
        sendError(sender, "خطا در مسدودسازی کاربر.");
        return;
    }
    ClientSocketWorker* targetWorker = SessionManager::getInstance().getWorkerForUser(targetUserId);
    if (targetWorker) {
        QMetaObject::invokeMethod(targetWorker, "handleDisconnect", Qt::QueuedConnection);
    }
    sendOk(sender);
}
void RequestProcessor::processUnbanUser(const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryById(
            sender->getAssociatedUserId(), summary)) {
        sendError(sender, "کاربر یافت نشد.");
        return;
    }
    if (summary.role !="Admin") {
        sendError(sender, "فقط مدیر سیستم اجازه انجام این عملیات را دارد.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();
    if (!DatabaseManager::getInstance().updateUserStatus(targetUserId, AccountStatus::Active)) {
        sendError(sender, "خطا در رفع مسدودیت کاربر.");
        return;
    }
    sendOk(sender);
}
void RequestProcessor::processDisableUserTemporarily(const QByteArray &data, ClientSocketWorker* sender) {
    if (sender->getAssociatedUserId() == -1) {
        sendError(sender, "ابتدا وارد حساب کاربری خود شوید.");
        return;
    }
    UserSummary summary;
    if (!DatabaseManager::getInstance().findUserSummaryById(
            sender->getAssociatedUserId(), summary)) {
        sendError(sender, "کاربر یافت نشد.");
        return;
    }
    if (summary.role !="Admin") {
        sendError(sender, "فقط مدیر سیستم اجازه انجام این عملیات را دارد.");
        return;
    }
    QJsonObject req = JsonPayload::fromBytes(data);
    int targetUserId = req["userId"].toInt();
    if (!DatabaseManager::getInstance().updateUserStatus(targetUserId, AccountStatus::TemporarilyDisabled)) {
        sendError(sender, "خطا در غیرفعال‌سازی موقت.");
        return;
    }
    sendOk(sender);
}
