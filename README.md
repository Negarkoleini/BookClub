# 📚 BookClub Client-Server System (Advanced Programming Final Project)

A robust, enterprise-grade, multi-threaded **Client-Server Book Club Management System** built with **C++** and **Qt 6**. This application features a modular architecture separating core server management from client graphical interfaces, supporting multiple user roles, secure TCP/IP network communication, real-time broadcasting, built-in PDF reading, interactive charts, and advanced database operations.

---

## 🌟 Key Features

### 🖥️ Server Side (`BookClubServer`)
* **Multi-Threaded Architecture:** Utilizes dedicated worker threads (`ClientHandlerThread`, `ClientSocketWorker`) to manage concurrent client connections safely and efficiently.
* **Database Management (`DatabaseManager`):** Persistent storage using SQLite/Qt SQL for users, books, transactions, comments, and ratings.
* **Security & Monitoring:** Built-in cryptographic security utilities (`SecurityUtils`), live system resource monitoring (`SystemResourceMonitor`), and comprehensive server logging (`ServerLogManager`).
* **Notification Broadcaster:** Real-time event broadcasting and session management (`SessionManager`).
* **Server Dashboard:** Live GUI dashboard (`ServerDashboardWindow`) monitoring active connections, server logs, and system health.

### 💻 Client Side (`BookClubClient`)
* **Role-Based Access Control (RBAC):** Distinct panels and permissions for:
  * 👤 **Regular Users:** Browse books, read summaries, manage shopping carts, write reviews, and track reading sessions.
  * 📖 **Publishers:** Add/manage books, set timed discounts, and view sales performance.
  * 👑 **Administrators:** Full system oversight, user management, and analytics monitoring.
* **Interactive GUI Components:**
  * **Advanced Book Search Engine (`BookSearchEngine`):** Fast querying and filtering by genre, author, and title.
  * **Built-in PDF Reader (`PdfReaderWidget`):** Direct in-app reading experience for digital books.
  * **Analytics & Charts (`AnalyticsChartWidget`):** Visual representation of sales, reading trends, and user statistics using `QtCharts`.
  * **In-App Notifications (`NotificationCenterWidget`, `InAppNotificationWidget`):** Real-time alerts and announcements.
* **Network & Localization:** Asynchronous TCP communication (`ClientNetworkManager`) with multilingual support (`.ts` translation files).

---

## 🛠️ Tech Stack & Libraries

* **Language:** C++ (Modern C++17/20 standards)
* **Framework:** Qt 6 (Core, Gui, Widgets, Sql, Network, Pdf, PdfWidgets, Charts, LinguistTools)
* **Build System:** CMake (>= 3.19)
* **Database:** SQLite

---

https://iutbox.iut.ac.ir/index.php/s/t64M6ZrF7LKSxYM

## 📁 Project Structure

```text
BookClub/
├── COMMON_SOURCES (Shared Models & Entities)
│   ├── Book.cpp / .h
│   ├── Genre.cpp / .h
│   ├── User.cpp / .h (Admin, RegularUser, Publisher)
│   ├── ShoppingCart.cpp / .h
│   ├── Transaction.cpp / .h
│   ├── Comment.cpp / .h & Rating.cpp / .h
│   └── NetworkMessage.cpp / .h
├── SERVER_SOURCES (Backend)
│   ├── MainServer.cpp & ServerCore.cpp / .h
│   ├── DatabaseManager.cpp / .h
│   ├── ClientHandlerThread.cpp / .h
│   └── ServerDashboardWindow.cpp / .h
└── CLIENT_SOURCES (Frontend)
│   ├── MainClient.cpp & LoginWindow.cpp / .h
│   ├── UserPanelWindow.cpp / .h
│   ├── PublisherPanelWindow.cpp / .h
│   ├── AdminPanelWindow.cpp / .h
│   └── PdfReaderWidget.cpp / .hed only into their respective target.



