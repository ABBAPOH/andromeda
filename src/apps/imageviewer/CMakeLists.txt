set( HEADERS
    application.h
    mainwindow.h
    preferenceswindow.h
)
set( SOURCES
    application.cpp
    main.cpp
    mainwindow.cpp
    preferenceswindow.cpp
)
set( RESOURCES resources.qrc )

#set( TS_FILES translations/imageviewer_ru.ts )

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtsingleapplication )

# on macosx the icons has to be part of sources. Bummer.
#if( APPLE )
#    set(GUI_RESOURCES imageviewer.icns)
#endif()

if( WIN32 )
   if( ${CMAKE_SYSTEM_VERSION} LESS "6.0" )
        if( NOT MINGW )
            set(GUI_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/imageviewer.rc)
        endif()
    else()
        message("Resources are disabled for windows 7")
   endif()
endif()

if( APPLE )
    set( EXECUTABLE_OUTPUT_PATH ${APP_APPLICATION_PATH} )
else()
    set( EXECUTABLE_OUTPUT_PATH ${APP_BINARY_PATH} )
endif()

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

add_executable( imageviewer MACOSX_BUNDLE WIN32 ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${QM_FILES} ${GUI_RESOURCES} )
target_link_libraries( imageviewer ${QT_LIBRARIES} qtsingleapplication libimageviewer libwidgets )

if( APPLE )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Icons/imageviewer.icns ${APP_DATA_PATH}/imageviewer.icns COPYONLY)
    set_target_properties( imageviewer PROPERTIES MACOSX_BUNDLE_ICON_FILE imageviewer.icns
                                                MACOSX_BUNDLE_BUNDLE_NAME "imageviewerer"
                                                MACOSX_BUNDLE_LONG_VERSION_STRING "0.3.0.0"
                                                MACOSX_BUNDLE_INFO_STRING "imageviewerer 0.3.0"
                                                MACOSX_BUNDLE_INFO_STRING "imageviewerer 0.3.0"
                                                MACOSX_BUNDLE_GUI_IDENTIFIER "org.imageviewer"
                                                MACOSX_BUNDLE_COPYRIGHT "ABBAPOH"
                         )
elseif( MSVC )
    set_target_properties( imageviewer PROPERTIES LINK_FLAGS /ENTRY:"mainCRTStartup" )
elseif( UNIX )
    add_definitions( -DLIB_SUFFIX=\"${LIB_SUFFIX}\" )

    set(ICON_LOCATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")
    add_definitions( -DICON_LOCATION=\"${ICON_LOCATION}\" )

    install( FILES imageviewer.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications" )
    install( FILES icons/imageviewer.png DESTINATION "${ICON_LOCATION}" )
endif()

install_bin( imageviewer )
