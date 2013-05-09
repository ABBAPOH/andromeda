SET( SOURCE tst_filemanagerwidget.cpp )
#qt4_automoc( ${SOURCES} )

include_directories( ${PROJECT_SOURCE_DIR}/include )

qt4_wrap_single_cpp( ${SOURCE} )
add_executable( tst_filemanagerwidget ${SOURCE} )
target_link_libraries( tst_filemanagerwidget ${QT_LIBRARIES} libfilemanager )
