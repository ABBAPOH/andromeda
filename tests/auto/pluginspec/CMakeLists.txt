SET( SOURCE tst_pluginspec.cpp )
#qt4_automoc( ${SOURCES} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )

qt4_wrap_single_cpp( ${SOURCE} )
add_executable( tst_pluginspec ${SOURCE} )
target_link_libraries( tst_pluginspec ${QT_LIBRARIES} libextensionsystem )
