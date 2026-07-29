cmake_minimum_required(VERSION 3.19)
project(BookClub LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    Network
    LinguistTools
    PdfWidgets
    Pdf
    Charts
)
qt_standard_project_setup()

# ---------------------------------------------------------------------------
# فایل‌های مشترک (مدل‌های داده) که هم در سرور و هم در کلاینت استفاده می‌شوند
# ---------------------------------------------------------------------------
set(COMMON_SOURCES
    AppTheme.h
    Book.cpp Book.h
    Genre.cpp Genre.h
    CartItem.cpp CartItem.h
    Comment.cpp Comment.h
    Rating.cpp Rating.h
    ShoppingCart.cpp ShoppingCart.h
    Bookshelf.cpp Bookshelf.h
    AppNotification.cpp AppNotification.h
    NetworkMessage.cpp NetworkMessage.h
    CommandType.h
    User.cpp User.h
    Admin.cpp Admin.h
    RegularUser.cpp RegularUser.h
    Publisher.cpp Publisher.h
    TimedDiscount.cpp TimedDiscount.h
    AnalyticsData.cpp AnalyticsData.h
    Transaction.cpp Transaction.h
    ReadingSession.cpp ReadingSession.h
    JsonPayLoad.h
)

# ---------------------------------------------------------------------------
# فایل‌های اختصاصی سرور (Server.exe)
# ---------------------------------------------------------------------------
set(SERVER_SOURCES
    MainServer.cpp
    DatabaseManager.h DatabaseManager.cpp
    ServerCore.h ServerCore.cpp
    ClientHandlerThread.h ClientHandlerThread.cpp
    ClientSocketWorker.h ClientSocketWorker.cpp
    RequestProcessor.h RequestProcessor.cpp
    ServerLogManager.h ServerLogManager.cpp
    ServerDashboardWindow.h ServerDashboardWindow.cpp
    SessionManager.h SessionManager.cpp
    NotificationBroadcaster.h NotificationBroadcaster.cpp
    SecurityUtils.h SecurityUtils.cpp
    SystemResourceMonitor.h SystemResourceMonitor.cpp
)

# ---------------------------------------------------------------------------
# فایل‌های اختصاصی کلاینت (Client.exe)
# ---------------------------------------------------------------------------
set(CLIENT_SOURCES
    MainClient.cpp
    ClientNetworkManager.h ClientNetworkManager.cpp
    LoginWindow.h LoginWindow.cpp
    DashboardController.h DashboardController.cpp
    UserPanelWindow.h UserPanelWindow.cpp
    PublisherPanelWindow.h PublisherPanelWindow.cpp
    AdminPanelWindow.h AdminPanelWindow.cpp
    AnalyticsChartWidget.h AnalyticsChartWidget.cpp
    DiscountManagerWidget.h DiscountManagerWidget.cpp
    NotificationCenterWidget.h NotificationCenterWidget.cpp
    InAppNotificationWidget.h InAppNotificationWidget.cpp
    PdfReaderWidget.h PdfReaderWidget.cpp
    BookSearchEngine.h BookSearchEngine.cpp
    ClientUtils.h
    loginpic.qrc
)

# ---------------------------------------------------------------------------
# اجرایی سرور: هسته اصلی سامانه که همیشه در حال اجراست و روی یک پورت Listen
# می‌کند. تمام اطلاعات و درخواست‌ها را مدیریت می‌کند.
# ---------------------------------------------------------------------------
qt_add_executable(BookClubServer
    WIN32 MACOSX_BUNDLE
    ${COMMON_SOURCES}
    ${SERVER_SOURCES}
)

target_link_libraries(BookClubServer
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Sql
        Qt::Network
)

# ---------------------------------------------------------------------------
# اجرایی کلاینت: برنامه‌ای که کاربران (کاربر عادی، ناشر و مدیر سیستم) اجرا
# می‌کنند و از طریق سوکت با سرور در ارتباط هستند.
# ---------------------------------------------------------------------------
qt_add_executable(BookClubClient
    WIN32 MACOSX_BUNDLE
    ${COMMON_SOURCES}
    ${CLIENT_SOURCES}
)

target_link_libraries(BookClubClient
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Sql
        Qt::Network
        Qt::Pdf
        Qt::PdfWidgets
        Qt::Charts
)

# ---------------------------------------------------------------------------
# نصب و بسته‌بندی هر دو اجرایی (برای اینستالر نهایی)
# ---------------------------------------------------------------------------
install(TARGETS BookClubServer BookClubClient
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET BookClubServer
    OUTPUT_SCRIPT deploy_script_server
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script_server})

qt_generate_deploy_app_script(
    TARGET BookClubClient
    OUTPUT_SCRIPT deploy_script_client
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script_client})
