set( QT_USE_QTWEBKIT TRUE )
include( ${QT_USE_FILE} )

set( SOURCES main.cpp )

include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin/actionmanager )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtsingleapplication )

add_executable( andromeda WIN32 MACOSX_BUNDLE ${SOURCES} )
target_link_libraries( andromeda ${QT_LIBRARIES} qtsingleapplication extensionsystem )

if( MSVC )
    set_target_properties( andromeda PROPERTIES LINK_FLAGS /ENTRY:"mainCRTStartup" )
endif( MSVC )

if( APPLE )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/andromeda.icns ${LIBRARY_OUTPUT_PATH}/Andromeda.app/Contents/Resources COPYONLY)
    set_target_properties( andromeda PROPERTIES MACOSX_BUNDLE_ICON_FILE andromeda.icns
                                                MACOSX_BUNDLE_BUNDLE_NAME "Andromeda"
                                                MACOSX_BUNDLE_LONG_VERSION_STRING "0.1.0.0")
endif( APPLE )

install_bin( andromeda )
