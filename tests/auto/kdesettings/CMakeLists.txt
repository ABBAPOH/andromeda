SET( SOURCE tst_kdesettings.cpp )
qt4_automoc( ${SOURCES} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )

qt4_wrap_single_cpp( ${SOURCE} )
add_executable( tst_kdesettings ${SOURCE} )
target_link_libraries( tst_kdesettings ${QT_LIBRARIES} io )
