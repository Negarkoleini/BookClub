include(D:/ME/project/BookClub/build/Desktop_Qt_6_5_3_MSVC2019_64bit_Debug/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/BookClub-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_ALL_MODULES_FOUND_VIA_FIND_PACKAGE "ZlibPrivate;EntryPointPrivate;Core;Gui;Widgets")

qt6_deploy_runtime_dependencies(
    EXECUTABLE D:/ME/project/BookClub/build/Desktop_Qt_6_5_3_MSVC2019_64bit_Debug/BookClub.exe
    GENERATE_QT_CONF
)
