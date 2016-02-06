project(kamule)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.2.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(CheckIncludeFile)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)

find_package(Qt5 5.3.0 CONFIG REQUIRED
    Core
    Network
    Widgets
)

find_package(KF5 5.2.0 REQUIRED
    Config
    ConfigWidgets
    CoreAddons
    I18n
    IconThemes
    KIO
    Notifications
    Pty
)

add_definitions(-Wall -Wextra)

set(libqec_SRCS
    qecpacket.cpp
    qecpacketsubscriber.cpp
    qecsocket.cpp
)

set(kamule_SRCS
    main.cpp
    kamule.cpp

    qecamule.cpp
    drawprogress.cpp

    amuledpage.cpp

    serverlistmodel.cpp
    networkpage.cpp

    searchlistmodel.cpp
    searchpage.cpp

    downloadlistmodel.cpp
    downloadpage.cpp

    uploadlistmodel.cpp
    uploadpage.cpp

    sharedfilelistmodel.cpp
    sharedfilepage.cpp

    preferencemodel.cpp
    preferencepage.cpp

    mystatusbar.cpp

    preference/generalpage.cpp
    preference/connectionpage.cpp
    preference/serverpage.cpp
    preference/filepage.cpp
    preference/directorypage.cpp
    preference/coretweakpage.cpp
    ${libqec_SRCS}
)

ki18n_wrap_ui(kamule_SRCS
    amuledpage.ui
    networkpage.ui
    searchpage.ui
    downloadpage.ui
    uploadpage.ui
    sharedfilepage.ui
    preferencepage.ui

    downloadinfodialog.ui
    uploadinfodialog.ui

    preference/generalpage.ui
    preference/connectionpage.ui
    preference/serverpage.ui
    preference/filepage.ui
    preference/directorypage.ui
    preference/coretweakpage.ui
    preference/kadpage.ui
    preference/securitypage.ui
)

kconfig_add_kcfg_files(kamule_SRCS kamulesettings.kcfgc)

add_executable(kamule ${kamule_SRCS})

target_link_libraries(kamule
    Qt5::Widgets
    Qt5::Network
    KF5::ConfigWidgets
    KF5::CoreAddons
    KF5::I18n
    KF5::IconThemes
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIOWidgets
    KF5::Notifications
    KF5::Pty
)

install(TARGETS kamule ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.kamule.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

add_subdirectory(icons)
add_subdirectory(po)
