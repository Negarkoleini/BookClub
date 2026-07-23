cmake_minimum_required(VERSION 3.19)
project(BookClub LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Widgets
    Sql
    Network
    LinguistTools
    PdfWidgets
    Pdf
)
qt_standard_project_setup()

qt_add_executable(BookClub
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    Book.cpp
    Book.h
    Genre.h
    CartItem.cpp
    CartItem.h
    Comment.h
    Rating.h
    Rating.cpp
    ShoppingCart.cpp
    ShoppingCart.h
    Genre.cpp
    Bookshelf.h
    Bookshelf.cpp
    AppNotification.h AppNotification.cpp
    NetworkMessage.h NetworkMessage.cpp
    CommandType.h
    User.h
    User.cpp
    Admin.h
    Admin.cpp
    RegularUser.h
    RegularUser.cpp
    Publisher.h
    Publisher.cpp
    TimedDiscount.cpp
    AnalyticsData.h
    AnalyticsData.cpp
    SecurityUtils.h
    SecurityUtils.cpp
    Transaction.h
    Transaction.cpp
    ReadingSession.h
    ReadingSession.cpp
    Comment.cpp
    ServerCore.h ServerCore.cpp
    DatabaseManager.h
    DatabaseManager.cpp
    TimedDiscount.h
    ClientHandlerThread.h ClientHandlerThread.cpp
    ServerCore.h
    ServerCore.cpp
    DatabaseManager.h
    DatabaseManager.cpp
    TimedDiscount.h
    ClientHandlerThread.h
    ClientHandlerThread.cpp
    RequestProcessor.h
    RequestProcessor.cpp
    JsonPayLoad.h
    ServerLogManager.h
    ServerLogManager.cpp
    servercore.h
    ClientSocketWorker.h
    ClientSocketWorker.cpp
    ServerDashboardWindow.h
    ServerDashboardWindow.cpp
    SessionManager.h SessionManager.cpp
    NotificationBroadcaster.h NotificationBroadcaster.cpp
    ClientNetworkManager.h ClientNetworkManager.cpp
    UserPanelWindow.h
    UserPanelWindow.cpp
    DashboardController.h
    DashboardController.cpp
    LoginWindow.h LoginWindow.cpp
    PdfReaderWidget.h
    PdfReaderWidget.cpp
    PublisherPanelWindow.h PublisherPanelWindow.cpp
    BookSearchEngine.h
    BookSearchEngine.cpp
    DiscountManagerWidget.h DiscountManagerWidget.cpp
    AdminPanelWindow.h
    AdminPanelWindow.cpp
    NotificationCenterWidget.h
    NotificationCenterWidget.cpp
)


target_link_libraries(BookClub
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
        Qt::Network
        Qt::Pdf
        Qt::PdfWidgets
)

install(TARGETS BookClub
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET BookClub
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
