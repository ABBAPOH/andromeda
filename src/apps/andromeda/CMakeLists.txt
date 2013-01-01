if( ${QT_QTWEBKIT_FOUND} )
    set( QT_USE_QTWEBKIT TRUE )
    include( ${QT_USE_FILE} )
else()
    add_definitions( -DQT_WEBKIT_NOT_FOUND )
endif()

set( HEADERS
    application.h
    browserwindow.h
    browserwindow_p.h
    commandssettingspage.h
    myaddressbar.h
    settingsmodel.h
    settingsmodel_p.h
    settingswidget.h
    tabbar.h
    tabbarbutton.h
    tabcontainer.h
    tabwidget.h
)

set( SOURCES
    application.cpp
    browserwindow.cpp
    commandssettingspage.cpp
    main.cpp
    myaddressbar.cpp
    settingsmodel.cpp
    settingswidget.cpp
    tabbar.cpp
    tabbarbutton.cpp
    tabcontainer.cpp
    tabwidget.cpp
)

set( FORMS
    settingswidget.ui
   )

set( RESOURCES resources.qrc )

set( TS_FILES translations/coreplugin_ru.ts translations/coreplugin_pt.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtsingleapplication )

# on macosx the icons has to be part of sources. Bummer.
#if( APPLE )
#    set(GUI_RESOURCES andromeda.icns)
#endif()

if( WIN32 )
   if( ${CMAKE_SYSTEM_VERSION} LESS "6.0" )
        set(GUI_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/andromeda.rc)
    else()
        message("Resources are disabled for windows 7")
   endif()
endif()

if( APPLE )
    set( EXECUTABLE_OUTPUT_PATH ${APP_APPLICATION_PATH} )
else()
    set( EXECUTABLE_OUTPUT_PATH ${APP_BINARY_PATH} )
endif()


add_executable( andromeda MACOSX_BUNDLE WIN32 ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} ${GUI_RESOURCES} )
target_link_libraries( andromeda ${QT_LIBRARIES} qtsingleapplication extensionsystem guisystem widgets )

if( APPLE )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/andromeda.icns ${APP_DATA_PATH}/andromeda.icns COPYONLY)
    set_target_properties( andromeda PROPERTIES MACOSX_BUNDLE_ICON_FILE andromeda.icns
                                                MACOSX_BUNDLE_BUNDLE_NAME "Andromeda"
                                                MACOSX_BUNDLE_LONG_VERSION_STRING "0.2.1.0"
                                                MACOSX_BUNDLE_INFO_STRING "Andromeda 0.1.0"
                                                MACOSX_BUNDLE_INFO_STRING "Andromeda 0.1.0"
                                                MACOSX_BUNDLE_GUI_IDENTIFIER "org.andromeda"
                                                MACOSX_BUNDLE_COPYRIGHT "ABBAPOH"
                         )
elseif( MSVC )
    set_target_properties( andromeda PROPERTIES LINK_FLAGS /ENTRY:"mainCRTStartup" )
elseif( UNIX )
    add_definitions( -DLIB_SUFFIX=\"${LIB_SUFFIX}\" )

    set(ICON_LOCATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")
    add_definitions( -DICON_LOCATION=\"${ICON_LOCATION}\" )

    install( FILES andromeda.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications" )
    install( FILES andromeda.png DESTINATION "${ICON_LOCATION}" )
endif()

install_bin( andromeda )
