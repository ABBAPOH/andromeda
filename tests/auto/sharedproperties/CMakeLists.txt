SET( SOURCE tst_sharedproperties.cpp )
#qt4_automoc( ${SOURCES} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )

qt4_wrap_single_cpp( ${SOURCE} )
add_executable( tst_sharedproperties ${SOURCE} )
target_link_libraries( tst_sharedproperties ${QT_LIBRARIES} guisystem )
