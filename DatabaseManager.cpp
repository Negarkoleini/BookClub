#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMutexLocker>
#include <QDebug>
#include <QDate>

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const QString &dbFilePath) {
    QMutexLocker locker(&dbMutex);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbFilePath);

    if (!db.open()) {
        qWarning() << "error opening Database" << db.lastError().text();
        return false;
    }

    QSqlQuery pragma(db);
    pragma.exec("PRAGMA foreign_keys = ON;");

    if (!createSchema()) {
        return false;
    }

    QSqlQuery tableInfo(db);
    tableInfo.exec("PRAGMA table_info(discounts);");
    bool hasApprovalColumn = false;
    while (tableInfo.next()) {
        if (tableInfo.value("name").toString() == "isApproved") {
            hasApprovalColumn = true;
            break;
        }
    }
    if (!hasApprovalColumn) {
        QSqlQuery alter(db);
        if (!alter.exec("ALTER TABLE discounts ADD COLUMN isApproved INTEGER DEFAULT 0;")) {
            qWarning() << "Failed to add isApproved column to discounts" << alter.lastError().text();
            return false;
        }
    }

    QSqlQuery q(db);
    if (q.exec("SELECT MAX(id) FROM books;") && q.next()) {
        int maxId = q.value(0).toInt();
        Book::seedNextId(maxId + 1);
    }

    return true;
}
bool DatabaseManager::createSchema() {
    QSqlQuery q(db);
    const char* statements[] = {
        R"(CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            passwordHash TEXT NOT NULL,
            email TEXT,
            role TEXT NOT NULL,
            status INTEGER NOT NULL DEFAULT 0,
            registrationDate TEXT,
            securityQuestion TEXT,
            securityAnswerHash TEXT,
            walletBalance REAL DEFAULT 0,
            favoriteGenres TEXT DEFAULT '',
            publisherName TEXT,
            corporateId TEXT,
            securityLevel INTEGER DEFAULT 1
        );)",
        R"(CREATE TABLE IF NOT EXISTS books (
            id INTEGER PRIMARY KEY,
            title TEXT, author TEXT, publisherId INTEGER,
            genre INTEGER, description TEXT, basePrice REAL,
            coverImagePath TEXT, pdfFileName TEXT, publishDate TEXT,
            isActive INTEGER DEFAULT 1, isDeleted INTEGER DEFAULT 0,
            averageRating REAL DEFAULT 0
        );)",
        R"(CREATE TABLE IF NOT EXISTS ratings (
            bookId INTEGER, userId INTEGER, score INTEGER,
            PRIMARY KEY (bookId, userId)
        );)",
        R"(CREATE TABLE IF NOT EXISTS comments (
            commentId INTEGER PRIMARY KEY AUTOINCREMENT,
            bookId INTEGER, userId INTEGER, senderUsername TEXT,
            textContent TEXT, timestamp TEXT,
            isApproved INTEGER DEFAULT 0, isEdited INTEGER DEFAULT 0, isDeleted INTEGER DEFAULT 0
        );)",
        R"(CREATE TABLE IF NOT EXISTS transactions (
            transactionId INTEGER PRIMARY KEY AUTOINCREMENT,
            buyerUserId INTEGER, sellerPublisherId INTEGER, purchasedBookId INTEGER,
            finalAmountPaid REAL, transactionTime TEXT
        );)",
        R"(CREATE TABLE IF NOT EXISTS discounts (
            discountId INTEGER PRIMARY KEY AUTOINCREMENT,
            targetBookId INTEGER, discountType INTEGER, discountValue REAL,
            startDateTime TEXT, endDateTime TEXT,
            isApproved INTEGER DEFAULT 0
        );)",
        R"(CREATE TABLE IF NOT EXISTS purchased_books (
            userId INTEGER, bookId INTEGER, PRIMARY KEY(userId, bookId)
        );)",
        R"(CREATE TABLE IF NOT EXISTS saved_books (
            userId INTEGER, bookId INTEGER, PRIMARY KEY(userId, bookId)
        );)",
        R"(CREATE TABLE IF NOT EXISTS shelves (
            shelfId INTEGER PRIMARY KEY AUTOINCREMENT,
            ownerUserId INTEGER, shelfName TEXT
        );)",
        R"(CREATE TABLE IF NOT EXISTS shelf_books (
            shelfId INTEGER, bookId INTEGER, PRIMARY KEY(shelfId, bookId)
        );)",
        R"(CREATE TABLE IF NOT EXISTS bookmark_pages (
            userId INTEGER, bookId INTEGER, pageNum INTEGER,
            PRIMARY KEY(userId, bookId)
        );)",
        R"(CREATE TABLE IF NOT EXISTS notifications (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type INTEGER, message TEXT, isRead INTEGER DEFAULT 0,
            timestamp TEXT, targetUserId INTEGER
        );)",
        R"(CREATE TABLE IF NOT EXISTS login_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            userId INTEGER, timestamp TEXT
        );)",
        R"(CREATE TABLE IF NOT EXISTS settings (
            key TEXT PRIMARY KEY,
            value TEXT
        );)",
    };

    for (const char* stmt : statements) {
        if (!q.exec(stmt)) {
            qWarning() << "error creat table" << q.lastError().text();
            return false;
        }
    }
    return true;
}
//کاریران
bool DatabaseManager::usernameExists(const std::string &username) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM users WHERE username = ? LIMIT 1;");
    q.addBindValue(QString::fromStdString(username));
    return q.exec() && q.next();
}
int DatabaseManager::getNextUserId() const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    if (q.exec("SELECT COALESCE(MAX(id), 0) + 1 FROM users;") && q.next()) { //coalesce نال را به صفر تبدیل میکند
        return q.value(0).toInt();
    }
    return 1;
}
bool DatabaseManager::addRegularUser(const RegularUser &user) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO users
        (id, username, passwordHash, email, role, status, registrationDate,
         securityQuestion, securityAnswerHash, walletBalance)
        VALUES (?,?,?,?,?,?,?,?,?,?);)");
    q.addBindValue(user.getId());
    q.addBindValue(QString::fromStdString(user.getUsername()));
    q.addBindValue(QString::fromStdString(user.getPasswordHash()));
    q.addBindValue(QString::fromStdString(user.getEmail()));
    q.addBindValue(QString::fromStdString(user.getRoleString()));
    q.addBindValue(static_cast<int>(user.getStatus()));
    q.addBindValue(QString::fromStdString(user.getRegistrationDate()));
    q.addBindValue(QString::fromStdString(user.getSecurityQuestion()));
    q.addBindValue(QString::fromStdString(user.getSecurityAnswerHash())); // securityAnswerHash: از طریق setSecurityQuestion جدا ست می‌شود، نه این‌جا
    q.addBindValue(user.getWalletBalance());
    if (!q.exec()) {
        qWarning() << "addRegularUser error:" << q.lastError().text();
        return false;
    }
    return true;
}
bool DatabaseManager::addPublisher(const Publisher &publisher) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO users
(id, username, passwordHash, email, role, status,
 registrationDate,
 securityQuestion,
 securityAnswerHash,
 publisherName,
 corporateId)
        VALUES (?,?,?,?,?,?,?,?,?,?,?);)");
    q.addBindValue(publisher.getId());
    q.addBindValue(QString::fromStdString(publisher.getUsername()));
    q.addBindValue(QString::fromStdString(publisher.getPasswordHash()));
    q.addBindValue(QString::fromStdString(publisher.getEmail()));
    q.addBindValue(QString::fromStdString(publisher.getRoleString()));
    q.addBindValue(static_cast<int>(publisher.getStatus()));
    q.addBindValue(QString::fromStdString(publisher.getRegistrationDate()));
    q.addBindValue(QString::fromStdString(publisher.getSecurityQuestion()));
    q.addBindValue(QString::fromStdString(publisher.getSecurityAnswerHash()));
    q.addBindValue(QString::fromStdString(publisher.getPublisherName()));
    q.addBindValue(QString::fromStdString(publisher.getCorporateId()));
    if (!q.exec()) {
        qWarning() << "addPublisher error:" << q.lastError().text();
        return false;
    }
    return true;
}
bool DatabaseManager::addAdmin(const Admin &admin) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO users
(id, username, passwordHash, email, role, status,
 registrationDate,
 securityQuestion,
 securityAnswerHash,
 securityLevel)
        VALUES (?,?,?,?,?,?,?,?,?,?);)");
    q.addBindValue(admin.getId());
    q.addBindValue(QString::fromStdString(admin.getUsername()));
    q.addBindValue(QString::fromStdString(admin.getPasswordHash()));
    q.addBindValue(QString::fromStdString(admin.getEmail()));
    q.addBindValue(QString::fromStdString(admin.getRoleString()));
    q.addBindValue(static_cast<int>(admin.getStatus()));
    q.addBindValue(QString::fromStdString(admin.getRegistrationDate()));
    q.addBindValue(QString::fromStdString(admin.getSecurityQuestion()));
    q.addBindValue(QString::fromStdString(admin.getSecurityAnswerHash()));
    q.addBindValue(admin.getSecurityLevel());
    if (!q.exec()) {
        qWarning() << "addAdmin error:" << q.lastError().text();
        return false;
    }
    return true;
}
static UserSummary rowToSummary(QSqlQuery &q) {
    UserSummary s;
    s.id = q.value("id").toInt();
    s.username = q.value("username").toString().toStdString();
    s.passwordHash = q.value("passwordHash").toString().toStdString();
    s.email = q.value("email").toString().toStdString();
    s.role = q.value("role").toString().toStdString();
    s.status = static_cast<AccountStatus>(q.value("status").toInt());
    s.registrationDate = q.value("registrationDate").toString().toStdString();
    return s;
}
bool DatabaseManager::findUserSummaryByUsername(const std::string &username, UserSummary &out) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT * FROM users WHERE username = ? LIMIT 1;");
    q.addBindValue(QString::fromStdString(username));
    if (!q.exec() || !q.next()) return false;
    out = rowToSummary(q);
    return true;
}
bool DatabaseManager::findUserSummaryById(int userId, UserSummary &out) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT * FROM users WHERE id = ? LIMIT 1;");
    q.addBindValue(userId);
    if (!q.exec() || !q.next()) return false;
    out = rowToSummary(q);
    return true;
}
bool DatabaseManager::updateUserStatus(int userId, AccountStatus status) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE users SET status = ? WHERE id = ?;");
    q.addBindValue(static_cast<int>(status));
    q.addBindValue(userId);
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::updateUserRole(int userId, const std::string &newRole) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE users SET role = ? WHERE id = ?;");
    q.addBindValue(QString::fromStdString(newRole));
    q.addBindValue(userId);
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::updatePasswordHash(int userId, const std::string &newPasswordHash) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE users SET passwordHash = ? WHERE id = ?;");
    q.addBindValue(QString::fromStdString(newPasswordHash));
    q.addBindValue(userId);
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::updateWalletBalance(int userId, double newBalance) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE users SET walletBalance = ? WHERE id = ?;");
    q.addBindValue(newBalance);
    q.addBindValue(userId);
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::updateEmail(int userId, const std::string &newEncryptedEmail) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE users SET email = ? WHERE id = ?;");
    q.addBindValue(QString::fromStdString(newEncryptedEmail));
    q.addBindValue(userId);
    return q.exec() && q.numRowsAffected() > 0;
}
std::vector<Genre> DatabaseManager::getFavoriteGenres(int userId) const {
    QMutexLocker locker(&dbMutex);
    std::vector<Genre> result;
    QSqlQuery q(db);
    q.prepare("SELECT favoriteGenres FROM users WHERE id = ?;");
    q.addBindValue(userId);
    if (q.exec() && q.next()) {
        QString raw = q.value(0).toString(); // فرمت ذخیره: "0,2,5" (اعدادِ enum با کاما جدا شده)
        for (const QString &part : raw.split(',', Qt::SkipEmptyParts)) {
            bool ok = false;
            int val = part.toInt(&ok);
            if (ok) result.push_back(static_cast<Genre>(val));
        }
    }
    return result;
}
bool DatabaseManager::updateFavoriteGenres(int userId, const std::vector<Genre> &genres) {
    QMutexLocker locker(&dbMutex);
    QStringList parts;
    for (Genre g : genres) parts << QString::number(static_cast<int>(g));
    QSqlQuery q(db);
    q.prepare("UPDATE users SET favoriteGenres = ? WHERE id = ?;");
    q.addBindValue(parts.join(','));
    q.addBindValue(userId);
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::getSecurityQuestion(const std::string &username, std::string &question, std::string &answerHashOut) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT securityQuestion, securityAnswerHash FROM users WHERE username = ?;");
    q.addBindValue(QString::fromStdString(username));
    if (!q.exec() || !q.next()) return false;
    question = q.value(0).toString().toStdString();
    answerHashOut = q.value(1).toString().toStdString();
    return true;
}
bool DatabaseManager::deleteUserAccount(int userId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("DELETE FROM users WHERE id = ?;");
    q.addBindValue(userId);
    return q.exec() && q.numRowsAffected() > 0;
}
QVector<UserSummary> DatabaseManager::getAllUsers() const {
    QMutexLocker locker(&dbMutex);
    QVector<UserSummary> result;
    QSqlQuery q(db);
    if (q.exec("SELECT * FROM users;")) {
        while (q.next()) result.push_back(rowToSummary(q));
    }
    return result;
}
std::unique_ptr<RegularUser> DatabaseManager::loadRegularUser(int userId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT * FROM users WHERE id = ? AND role = 'RegularUser';");
    q.addBindValue(userId);
    if (!q.exec() || !q.next()) return nullptr;

    auto user = std::make_unique<RegularUser>(
        q.value("id").toInt(),
        q.value("username").toString().toStdString(),
        q.value("passwordHash").toString().toStdString(),
        q.value("email").toString().toStdString(),
        q.value("registrationDate").toString().toStdString(),
        q.value("walletBalance").toDouble()
        );
    user->setStatus(static_cast<AccountStatus>(q.value("status").toInt()));

    QString raw = q.value("favoriteGenres").toString();
    std::vector<Genre> genres;
    for (const QString &part : raw.split(',', Qt::SkipEmptyParts)) {
        bool ok = false;
        int val = part.toInt(&ok);
        if (ok) genres.push_back(static_cast<Genre>(val));
    }
    user->setFavoriteGenres(genres);

    return user;
}

std::unique_ptr<Publisher> DatabaseManager::loadPublisher(int userId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT * FROM users WHERE id = ? AND role = 'Publisher';");
    q.addBindValue(userId);
    if (!q.exec() || !q.next()) return nullptr;

    auto pub = std::make_unique<Publisher>(
        q.value("id").toInt(),
        q.value("username").toString().toStdString(),
        q.value("passwordHash").toString().toStdString(),
        q.value("email").toString().toStdString(),
        q.value("registrationDate").toString().toStdString(),
        q.value("publisherName").toString().toStdString(),
        q.value("corporateId").toString().toStdString()
        );
    pub->setStatus(static_cast<AccountStatus>(q.value("status").toInt()));
    return pub;
}
//کتاب
int DatabaseManager::addBook(const Book &book) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO books
        (id, title, author, publisherId, genre, description, basePrice,
         coverImagePath, pdfFileName, publishDate, isActive, isDeleted, averageRating)
        VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?);)");
    q.addBindValue(book.getId());
    q.addBindValue(QString::fromStdString(book.getTitle()));
    q.addBindValue(QString::fromStdString(book.getAuthor()));
    q.addBindValue(book.getPublisherId());
    q.addBindValue(static_cast<int>(book.getGenre()));
    q.addBindValue(QString::fromStdString(book.getDescription()));
    q.addBindValue(book.getBasePrice());
    q.addBindValue(QString::fromStdString(book.getCoverImagePath()));
    q.addBindValue(QString::fromStdString(book.getPdfFileName()));
    q.addBindValue(QString::fromStdString(book.getPublishDate()));
    q.addBindValue(book.getIsActive() ? 1 : 0);
    q.addBindValue(book.getIsDeleted() ? 1 : 0);
    q.addBindValue(book.getAverageRating());
    if (!q.exec()) {
        qWarning() << "addBook error:" << q.lastError().text();
        return -1;
    }
    return book.getId();
}
bool DatabaseManager::updateBook(const Book &book) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(UPDATE books SET title=?, author=?, genre=?, description=?, basePrice=?,
                 coverImagePath=?, pdfFileName=? WHERE id=?;)");
    q.addBindValue(QString::fromStdString(book.getTitle()));
    q.addBindValue(QString::fromStdString(book.getAuthor()));
    q.addBindValue(static_cast<int>(book.getGenre()));
    q.addBindValue(QString::fromStdString(book.getDescription()));
    q.addBindValue(book.getBasePrice());
    q.addBindValue(QString::fromStdString(book.getCoverImagePath()));
    q.addBindValue(QString::fromStdString(book.getPdfFileName()));
    q.addBindValue(book.getId());
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::setBookActive(int bookId, bool active) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE books SET isActive = ? WHERE id = ?;");
    q.addBindValue(active ? 1 : 0);
    q.addBindValue(bookId);
    return q.exec() && q.numRowsAffected() > 0;
}

bool DatabaseManager::softDeleteBook(int bookId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE books SET isDeleted = 1, isActive = 0 WHERE id = ?;");
    q.addBindValue(bookId);
    return q.exec() && q.numRowsAffected() > 0;
}

static Book rowToBook(QSqlQuery &q) {
    Book b(
        q.value("title").toString().toStdString(),
        q.value("author").toString().toStdString(),
        q.value("publisherId").toInt(),
        static_cast<Genre>(q.value("genre").toInt()),
        q.value("description").toString().toStdString(),
        q.value("basePrice").toDouble(),
        q.value("coverImagePath").toString().toStdString(),
        q.value("pdfFileName").toString().toStdString(),
        q.value("publishDate").toString().toStdString()
        );
    b.setId(q.value("id").toInt());
    b.setIsActive(q.value("isActive").toInt() != 0);
    b.setIsDeleted(q.value("isDeleted").toInt() != 0);

    b.setAverageRating(q.value("averageRating").toDouble());

    return b;
}
QVector<Book> DatabaseManager::getAllActiveBooks() const {
    QMutexLocker locker(&dbMutex);
    QVector<Book> result;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM books WHERE isActive = 1 AND isDeleted = 0;");
    if (q.exec()) {
        while (q.next()) result.push_back(rowToBook(q));
    }
    return result;
}
bool DatabaseManager::getBookById(int bookId, Book &out) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT * FROM books WHERE id = ?;");
    q.addBindValue(bookId);
    if (!q.exec() || !q.next()) return false;
    out = rowToBook(q);
    return true;
}
QVector<Book> DatabaseManager::getBooksByPublisher(int publisherId) const {
    QMutexLocker locker(&dbMutex);
    QVector<Book> result;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM books WHERE publisherId = ?;");
    q.addBindValue(publisherId);
    if (q.exec()) {
        while (q.next()) result.push_back(rowToBook(q));
    }
    return result;
}
QVector<Book> DatabaseManager::getAllBooksAdmin() const {
    QMutexLocker locker(&dbMutex);
    QVector<Book> result;
    QSqlQuery q(db);
    if (q.exec("SELECT * FROM books;")) {
        while (q.next()) result.push_back(rowToBook(q));
    }
    return result;
}
QVector<Book> DatabaseManager::getSuggestedBooksForUser(int userId) const {
    // چون خواندنِ ژانرهای موردعلاقه خودش قفلِ دیگری می‌گیرد، این متد را قبل از
    // قفل‌کردنِ dbMutex صدا می‌زنیم (getFavoriteGenres خودش مسئولِ قفلِ خودش است)
    std::vector<Genre> favorites = getFavoriteGenres(userId);
    QVector<Book> result;
    if (favorites.empty()) return result; // کاربر هنوز ژانر انتخاب نکرده -> پیشنهادی نداریم

    QMutexLocker locker(&dbMutex);
    QStringList placeholders;
    for (size_t i = 0; i < favorites.size(); ++i) placeholders << "?";
    QSqlQuery q(db);
    q.prepare(QString("SELECT * FROM books WHERE isActive=1 AND isDeleted=0 AND genre IN (%1) "
                      "ORDER BY averageRating DESC LIMIT 30;").arg(placeholders.join(',')));
    for (Genre g : favorites) q.addBindValue(static_cast<int>(g));
    if (q.exec()) {
        while (q.next()) result.push_back(rowToBook(q));
    }
    return result;
}

QVector<Book> DatabaseManager::getPopularBooks(int limitCount) const {
    QMutexLocker locker(&dbMutex);
    QVector<Book> result;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM books WHERE isActive=1 AND isDeleted=0 "
              "ORDER BY averageRating DESC LIMIT ?;");
    q.addBindValue(limitCount);
    if (q.exec()) {
        while (q.next()) result.push_back(rowToBook(q));
    }
    return result;
}

QVector<Book> DatabaseManager::getBestsellingBooks(int limitCount) const {
    QMutexLocker locker(&dbMutex);
    QVector<Book> result;
    QSqlQuery q(db);
    // تعدادِ فروشِ هر کتاب را از جدولِ transactions می‌شماریم و کتاب‌های پرفروش‌تر را اول می‌آوریم
    q.prepare(R"(SELECT b.*, COUNT(t.transactionId) AS salesCount
                 FROM books b LEFT JOIN transactions t ON t.purchasedBookId = b.id
                 WHERE b.isActive=1 AND b.isDeleted=0
                 GROUP BY b.id
                 ORDER BY salesCount DESC
                 LIMIT ?;)");
    q.addBindValue(limitCount);
    if (q.exec()) {
        while (q.next()) result.push_back(rowToBook(q));
    }
    return result;
}

// امتیاز و نظر
bool DatabaseManager::upsertRating(int bookId, int userId, int score) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO ratings (bookId, userId, score) VALUES (?,?,?)
                 ON CONFLICT(bookId, userId) DO UPDATE SET score = excluded.score;)");
    q.addBindValue(bookId);
    q.addBindValue(userId);
    q.addBindValue(score);
    if (!q.exec()) {
        qWarning() << "upsertRating error:" << q.lastError().text();
        return false;
    }

    QSqlQuery avg(db);
    // استفاده از COALESCE برای جلوگیری از NULL شدن
    avg.prepare("SELECT COALESCE(AVG(score), 0.0) FROM ratings WHERE bookId = ?;");
    avg.addBindValue(bookId);
    if (avg.exec() && avg.next()) {
        QSqlQuery upd(db);
        upd.prepare("UPDATE books SET averageRating = ? WHERE id = ?;");
        upd.addBindValue(avg.value(0).toDouble());
        upd.addBindValue(bookId);
        upd.exec();
    }
    return true;
}
double DatabaseManager::getAverageRating(int bookId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT averageRating FROM books WHERE id = ?;");
    q.addBindValue(bookId);
    if (q.exec() && q.next()) return q.value(0).toDouble();
    return 0.0;
}
int DatabaseManager::addComment(const Comment &comment) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO comments (bookId, userId, senderUsername, textContent, timestamp, isApproved)
                 VALUES (?,?,?,?,?,0);)");
    q.addBindValue(comment.getBookId());
    q.addBindValue(comment.getUserId());
    q.addBindValue(QString::fromStdString(comment.getSenderUsername()));
    q.addBindValue(QString::fromStdString(comment.getTextContent()));
    q.addBindValue(QString::fromStdString(comment.getTimestamp()));
    if (!q.exec()) {
        qWarning() << "addComment error:" << q.lastError().text();
        return -1;
    }
    return q.lastInsertId().toInt();
}
static Comment rowToComment(QSqlQuery &q) {
    Comment c(
        q.value("commentId").toInt(),
        q.value("bookId").toInt(),
        q.value("userId").toInt(),
        q.value("senderUsername").toString().toStdString(),
        q.value("textContent").toString().toStdString(),
        q.value("timestamp").toString().toStdString()
        );
    c.setApproved(q.value("isApproved").toInt() != 0);
    return c;
}
QVector<Comment> DatabaseManager::getCommentsForBook(int bookId, int viewerUserId) const {
    QMutexLocker locker(&dbMutex);
    QVector<Comment> result;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM comments WHERE bookId = ? AND isDeleted = 0 AND (isApproved = 1 OR userId = ?);");
    q.addBindValue(bookId);
    q.addBindValue(viewerUserId);
    if (q.exec()) {
        while (q.next()) result.push_back(rowToComment(q));
    }
    return result;
}
QVector<Comment> DatabaseManager::getPendingComments() const {
    QMutexLocker locker(&dbMutex);
    QVector<Comment> result;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM comments WHERE isApproved = 0 AND isDeleted = 0;");
    if (q.exec()) {
        while (q.next()) result.push_back(rowToComment(q));
    }
    return result;
}
QVector<Comment> DatabaseManager::getAllComments(int filterBookId, int filterUserId) const {
    QMutexLocker locker(&dbMutex);
    QVector<Comment> result;
    QString sql = "SELECT * FROM comments WHERE isDeleted = 0";
    if (filterBookId >= 0) sql += " AND bookId = " + QString::number(filterBookId);
    if (filterUserId >= 0) sql += " AND userId = " + QString::number(filterUserId);
    sql += " ORDER BY commentId DESC;";
    QSqlQuery q(db);
    if (q.exec(sql)) {
        while (q.next()) result.push_back(rowToComment(q));
    }
    return result;
}
bool DatabaseManager::setCommentApproved(int commentId, bool approved) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE comments SET isApproved = ? WHERE commentId = ?;");
    q.addBindValue(approved ? 1 : 0);
    q.addBindValue(commentId);
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::deleteComment(int commentId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE comments SET isDeleted = 1 WHERE commentId = ?;");
    q.addBindValue(commentId);
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::editCommentText(int commentId, const std::string &newText, const std::string &editTimestamp) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE comments SET textContent = ?, timestamp = ?, isEdited = 1 WHERE commentId = ?;");
    q.addBindValue(QString::fromStdString(newText));
    q.addBindValue(QString::fromStdString(editTimestamp));
    q.addBindValue(commentId);
    return q.exec() && q.numRowsAffected() > 0;
}

int DatabaseManager::getCommentOwnerId(int commentId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT userId FROM comments WHERE commentId = ?;");
    q.addBindValue(commentId);
    if (!q.exec() || !q.next()) return -1;
    return q.value(0).toInt();
}
// خرید _ تراکنش _کتابخانه
bool DatabaseManager::logTransaction(const Transaction &tx) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO transactions
        (buyerUserId, sellerPublisherId, purchasedBookId, finalAmountPaid, transactionTime)
        VALUES (?,?,?,?,?);)");
    q.addBindValue(tx.getBuyerUserId());
    q.addBindValue(tx.getSellerPublisherId());
    q.addBindValue(tx.getPurchasedBookId());
    q.addBindValue(tx.getFinalAmountPaid());
    q.addBindValue(QString::fromStdString(tx.getTransactionTime()));
    if (!q.exec()) {
        qWarning() << "logTransaction error:" << q.lastError().text();
        return false;
    }
    return true;
}
bool DatabaseManager::getBookSalesInfo(int bookId, int &salesCount, double &revenue) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*), COALESCE(SUM(finalAmountPaid), 0) FROM transactions WHERE purchasedBookId = ?;");
    q.addBindValue(bookId);
    if (!q.exec() || !q.next()) {
        salesCount = 0;
        revenue = 0.0;
        return false;
    }
    salesCount = q.value(0).toInt();
    revenue = q.value(1).toDouble();
    return true;
}
bool DatabaseManager::addBookToLibrary(int userId, int bookId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("INSERT OR IGNORE INTO purchased_books (userId, bookId) VALUES (?,?);");
    q.addBindValue(userId);
    q.addBindValue(bookId);
    return q.exec();
}
bool DatabaseManager::isBookInLibrary(int userId, int bookId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM purchased_books WHERE userId=? AND bookId=?;");
    q.addBindValue(userId);
    q.addBindValue(bookId);
    return q.exec() && q.next();
}
QVector<int> DatabaseManager::getPurchasedBookIds(int userId) const {
    QMutexLocker locker(&dbMutex);
    QVector<int> ids;
    QSqlQuery q(db);
    q.prepare("SELECT bookId FROM purchased_books WHERE userId=?;");
    q.addBindValue(userId);
    if (q.exec()) {
        while (q.next()) ids.push_back(q.value(0).toInt());
    }
    return ids;
}
// کتابخانه شخصی
bool DatabaseManager::saveBookForLater(int userId, int bookId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("INSERT OR IGNORE INTO saved_books (userId, bookId) VALUES (?,?);");
    q.addBindValue(userId);
    q.addBindValue(bookId);
    return q.exec();
}
bool DatabaseManager::removeSavedBook(int userId, int bookId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("DELETE FROM saved_books WHERE userId=? AND bookId=?;");
    q.addBindValue(userId);
    q.addBindValue(bookId);
    return q.exec();
}
QVector<int> DatabaseManager::getSavedBookIds(int userId) const {
    QMutexLocker locker(&dbMutex);
    QVector<int> ids;
    QSqlQuery q(db);
    q.prepare("SELECT bookId FROM saved_books WHERE userId=?;");
    q.addBindValue(userId);
    if (q.exec()) {
        while (q.next()) ids.push_back(q.value(0).toInt());
    }
    return ids;
}
int DatabaseManager::createShelf(int userId, const std::string &shelfName) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("INSERT INTO shelves (ownerUserId, shelfName) VALUES (?,?);");
    q.addBindValue(userId);
    q.addBindValue(QString::fromStdString(shelfName));
    if (!q.exec()) return -1;
    return q.lastInsertId().toInt();
}
bool DatabaseManager::deleteShelf(int shelfId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("DELETE FROM shelves WHERE shelfId=?;");
    q.addBindValue(shelfId);
    bool ok = q.exec();
    QSqlQuery q2(db);
    q2.prepare("DELETE FROM shelf_books WHERE shelfId=?;");
    q2.addBindValue(shelfId);
    q2.exec();
    return ok;
}
bool DatabaseManager::addBookToShelf(int shelfId, int bookId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("INSERT OR IGNORE INTO shelf_books (shelfId, bookId) VALUES (?,?);");
    q.addBindValue(shelfId);
    q.addBindValue(bookId);
    return q.exec();
}
bool DatabaseManager::removeBookFromShelf(int shelfId, int bookId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("DELETE FROM shelf_books WHERE shelfId=? AND bookId=?;");
    q.addBindValue(shelfId);
    q.addBindValue(bookId);
    return q.exec();
}
QVector<ShelfInfo> DatabaseManager::getShelvesForUser(int userId) const {
    QMutexLocker locker(&dbMutex);
    QVector<ShelfInfo> result;
    QSqlQuery q(db);
    q.prepare("SELECT shelfId, shelfName FROM shelves WHERE ownerUserId = ? ORDER BY shelfId;");
    q.addBindValue(userId);
    if (!q.exec()) return result;

    while (q.next()) {
        ShelfInfo info;
        info.shelfId = q.value("shelfId").toInt();
        info.ownerUserId = userId;
        info.shelfName = q.value("shelfName").toString().toStdString();

        QSqlQuery qb(db);
        qb.prepare("SELECT bookId FROM shelf_books WHERE shelfId = ?;");
        qb.addBindValue(info.shelfId);
        if (qb.exec()) {
            while (qb.next()) info.bookIds.push_back(qb.value(0).toInt());
        }
        result.push_back(info);
    }
    return result;
}
bool DatabaseManager::renameShelf(int shelfId, const std::string &newName) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE shelves SET shelfName = ? WHERE shelfId = ?;");
    q.addBindValue(QString::fromStdString(newName));
    q.addBindValue(shelfId);
    return q.exec() && q.numRowsAffected() > 0;
}
bool DatabaseManager::moveBookBetweenShelves(int fromShelfId, int toShelfId, int bookId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery qDel(db);
    qDel.prepare("DELETE FROM shelf_books WHERE shelfId = ? AND bookId = ?;");
    qDel.addBindValue(fromShelfId);
    qDel.addBindValue(bookId);
    if (!qDel.exec()) return false;

    QSqlQuery qIns(db);
    qIns.prepare("INSERT OR IGNORE INTO shelf_books (shelfId, bookId) VALUES (?,?);");
    qIns.addBindValue(toShelfId);
    qIns.addBindValue(bookId);
    return qIns.exec();
}
int DatabaseManager::getShelfOwnerId(int shelfId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT ownerUserId FROM shelves WHERE shelfId = ?;");
    q.addBindValue(shelfId);
    if (q.exec() && q.next()) return q.value(0).toInt();
    return -1;
}
QVector<int> DatabaseManager::getUserIdsByFavoriteGenre(Genre genre) const {
    QMutexLocker locker(&dbMutex);
    QVector<int> result;
    QSqlQuery q(db);
    q.prepare("SELECT id, favoriteGenres FROM users WHERE role = 'RegularUser';");
    if (q.exec()) {
        const QString target = QString::number(static_cast<int>(genre));
        while (q.next()) {
            QString raw = q.value("favoriteGenres").toString();
            // فرمتِ ذخیره: "0,2,5" -- باید دقیقاً به عنوانِ یک توکنِ کامل چک شود، نه substring
            for (const QString &part : raw.split(',', Qt::SkipEmptyParts)) {
                if (part == target) {
                    result.push_back(q.value("id").toInt());
                    break;
                }
            }
        }
    }
    return result;
}
bool DatabaseManager::savePageLocation(int userId, int bookId, int pageNum) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO bookmark_pages (userId, bookId, pageNum) VALUES (?,?,?)
                 ON CONFLICT(userId, bookId) DO UPDATE SET pageNum = excluded.pageNum;)");
    q.addBindValue(userId);
    q.addBindValue(bookId);
    q.addBindValue(pageNum);
    return q.exec();
}
int DatabaseManager::getPageLocation(int userId, int bookId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT pageNum FROM bookmark_pages WHERE userId=? AND bookId=?;");
    q.addBindValue(userId);
    q.addBindValue(bookId);
    if (q.exec() && q.next()) return q.value(0).toInt();
    return 1;
}
//تخفیف
int DatabaseManager::addDiscount(const TimedDiscount &discount) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO discounts (targetBookId, discountType, discountValue, startDateTime, endDateTime)
                 VALUES (?,?,?,?,?);)");
    q.addBindValue(discount.getTargetBookId());
    q.addBindValue(static_cast<int>(discount.getDiscountType()));
    q.addBindValue(discount.getDiscountValue());
    q.addBindValue( QString::fromStdString(discount.getStartDateTime()));
    q.addBindValue(QString::fromStdString(discount.getEndDateTime()));
    if (!q.exec()) {
        qWarning() << "addDiscount error:" << q.lastError().text();
        return -1;
    }
    return q.lastInsertId().toInt();
}
bool DatabaseManager::removeDiscount(int discountId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("DELETE FROM discounts WHERE discountId=?;");
    q.addBindValue(discountId);
    return q.exec() && q.numRowsAffected() > 0;
}
QVector<TimedDiscount> DatabaseManager::getActiveDiscountsForBook(int bookId, const std::string &currentSystemTime) const {
    QMutexLocker locker(&dbMutex);
    QVector<TimedDiscount> result;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM discounts WHERE targetBookId=? AND isApproved = 1;");
    q.addBindValue(bookId);
    if (q.exec()) {
        while (q.next()) {
            TimedDiscount d(
                q.value("targetBookId").toInt(),
                static_cast<DiscountType>(q.value("discountType").toInt()),
                q.value("discountValue").toDouble(),
                q.value("startDateTime").toString().toStdString(),
                q.value("endDateTime").toString().toStdString()
                );
            if (d.isActiveNow(currentSystemTime)) {
                result.push_back(d);
            }
        }
    }
    return result;
}
QVector<TimedDiscount> DatabaseManager::getPendingDiscounts() const {
    QMutexLocker locker(&dbMutex);
    QVector<TimedDiscount> result;
    QSqlQuery q(db);
    if (q.exec("SELECT * FROM discounts WHERE isApproved = 0;")) {
        while (q.next()) {
            TimedDiscount d(
                q.value("targetBookId").toInt(),
                static_cast<DiscountType>(q.value("discountType").toInt()),
                q.value("discountValue").toDouble(),
                q.value("startDateTime").toString().toStdString(),
                q.value("endDateTime").toString().toStdString()
                );
            d.setDiscountId(q.value("discountId").toInt());
            result.push_back(d);
        }
    }
    return result;
}
bool DatabaseManager::getDiscountById(int discountId, TimedDiscount &out) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT * FROM discounts WHERE discountId = ?;");
    q.addBindValue(discountId);
    if (!q.exec() || !q.next()) return false;

    out = TimedDiscount(
        q.value("targetBookId").toInt(),
        static_cast<DiscountType>(q.value("discountType").toInt()),
        q.value("discountValue").toDouble(),
        q.value("startDateTime").toString().toStdString(),
        q.value("endDateTime").toString().toStdString()
        );
    out.setDiscountId(q.value("discountId").toInt());
    return true;
}

bool DatabaseManager::setDiscountApproved(int discountId, bool approved) {
    QMutexLocker locker(&dbMutex);
    if (!approved) {
        QSqlQuery q(db);
        q.prepare("DELETE FROM discounts WHERE discountId=?;");
        q.addBindValue(discountId);
        return q.exec() && q.numRowsAffected() > 0;
    }
    QSqlQuery q(db);
    q.prepare("UPDATE discounts SET isApproved = 1 WHERE discountId=?;");
    q.addBindValue(discountId);
    return q.exec() && q.numRowsAffected() > 0;
}

// محدودیت‌های سیستمی (تنظیمات ادمین)
int DatabaseManager::getIntSetting(const std::string &key, int defaultValue) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT value FROM settings WHERE key = ?;");
    q.addBindValue(QString::fromStdString(key));
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }
    return defaultValue;
}
bool DatabaseManager::setIntSetting(const std::string &key, int value) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO settings (key, value) VALUES (?, ?)
                 ON CONFLICT(key) DO UPDATE SET value = excluded.value;)");
    q.addBindValue(QString::fromStdString(key));
    q.addBindValue(QString::number(value));
    return q.exec();
}

int DatabaseManager::getPurchaseCountToday(int userId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM transactions WHERE buyerUserId = ? AND transactionTime LIKE ? || '%';");
    q.addBindValue(userId);
    q.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    if (q.exec() && q.next()) return q.value(0).toInt();
    return 0;
}

int DatabaseManager::getCommentCountToday(int userId) const {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM comments WHERE userId = ? AND timestamp LIKE ? || '%';");
    q.addBindValue(userId);
    q.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    if (q.exec() && q.next()) return q.value(0).toInt();
    return 0;
}

// اعلان
int DatabaseManager::saveNotification(const AppNotification &notif) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare(R"(INSERT INTO notifications (type, message, isRead, timestamp, targetUserId)
                 VALUES (?,?,?,?,?);)");
    q.addBindValue(static_cast<int>(notif.getType()));
    q.addBindValue(QString::fromStdString(notif.getMessage()));
    q.addBindValue(notif.getIsRead() ? 1 : 0);
    q.addBindValue(QString::fromStdString(notif.getTimestamp()));
    q.addBindValue(notif.getTargetUserId());
    if (q.exec()) {
        return q.lastInsertId().toInt();
    }
    return -1;
}
QVector<AppNotification> DatabaseManager::getNotificationsForUser(int userId) const {
    QMutexLocker locker(&dbMutex);
    QVector<AppNotification> result;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM notifications WHERE targetUserId=? ORDER BY id DESC;");// اخزین نوتیف را اول نمایش میدهد
    q.addBindValue(userId);
    if (q.exec()) {
        while (q.next()) {
            AppNotification n = AppNotification::fromStorage(
                q.value("id").toInt(),
                static_cast<NotificationType>(q.value("type").toInt()),
                q.value("message").toString().toStdString(),
                q.value("targetUserId").toInt(),
                q.value("isRead").toInt() != 0,
                q.value("timestamp").toString().toStdString()
                );
            result.push_back(n);
        }
    }
    return result;
}

bool DatabaseManager::markNotificationRead(int notificationId) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("UPDATE notifications SET isRead = 1 WHERE id = ?;");
    q.addBindValue(notificationId);
    return q.exec() && q.numRowsAffected() > 0;
}
//تاریخچه ورود
bool DatabaseManager::logLoginEvent(int userId, const std::string &timestamp) {
    QMutexLocker locker(&dbMutex);
    QSqlQuery q(db);
    q.prepare("INSERT INTO login_history (userId, timestamp) VALUES (?, ?);");
    q.addBindValue(userId);
    q.addBindValue(QString::fromStdString(timestamp));
    return q.exec();
}

QVector<std::string> DatabaseManager::getLoginHistory(int userId, int limitCount) const {
    QMutexLocker locker(&dbMutex);
    QVector<std::string> result;
    QSqlQuery q(db);
    q.prepare("SELECT timestamp FROM login_history WHERE userId = ? ORDER BY id DESC LIMIT ?;");
    q.addBindValue(userId);
    q.addBindValue(limitCount);
    if (q.exec()) {
        while (q.next()) result.push_back(q.value(0).toString().toStdString());
    }
    return result;
}
