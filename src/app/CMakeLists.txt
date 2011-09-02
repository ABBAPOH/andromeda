set( QT_USE_QTWEBKIT TRUE )
include( ${QT_USE_FILE} )

set( SOURCES main.cpp )

include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin/actionmanager )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtsingleapplication )

# on macosx the icons has to be part of sources. Bummer.
#if( APPLE )
#    set(GUI_RESOURCES andromeda.icns)
#endif()

if( WIN32 )
    set(GUI_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/andromeda.rc)
endif()

add_executable( andromeda MACOSX_BUNDLE WIN32 ${SOURCES} ${GUI_RESOURCES} )
target_link_libraries( andromeda ${QT_LIBRARIES} qtsingleapplication extensionsystem )

if( APPLE )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/andromeda.icns ${LIBRARY_OUTPUT_PATH}/Andromeda.app/Contents/Resources/andromeda.icns COPYONLY)
    set_target_properties( andromeda PROPERTIES MACOSX_BUNDLE_ICON_FILE andromeda.icns
                                                MACOSX_BUNDLE_BUNDLE_NAME "Andromeda"
                                                MACOSX_BUNDLE_LONG_VERSION_STRING "0.1.0.0"
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
