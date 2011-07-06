set( QT_USE_QTWEBKIT TRUE )
include( ${QT_USE_FILE} )

set( SOURCES main.cpp )

include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin/actionmanager )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtsingleapplication )

add_executable( Andromeda  MACOSX_BUNDLE ${SOURCES} )
target_link_libraries( Andromeda ${QT_LIBRARIES} qtsingleapplication extensionsystem guisystem )

if( MSVC )
    set_target_properties( Andromeda PROPERTIES LINK_FLAGS /ENTRY:"mainCRTStartup" )
endif( MSVC )
