set( SOURCES main.cpp )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )

add_executable( Andromeda MACOSX_BUNDLE WIN32 ${SOURCES} )
target_link_libraries( Andromeda ${QT_LIBRARIES} extensionsystem )
