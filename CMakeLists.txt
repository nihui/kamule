project(kamule)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

include_directories(${KDE4_INCLUDES} ${QT_INCLUDES})

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

kde4_add_ui_files(kamule_SRCS
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

kde4_add_kcfg_files(kamule_SRCS kamulesettings.kcfgc)

kde4_add_executable(kamule ${kamule_SRCS})

target_link_libraries(kamule
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KPTY_LIBS}
)

install(TARGETS kamule ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kamuleui.rc DESTINATION ${DATA_INSTALL_DIR}/kamule)
install(PROGRAMS kamule.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

add_subdirectory(icons)
add_subdirectory(po)
