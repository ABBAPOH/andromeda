
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtfilecopier )

add_executable( copydialog_test main.cpp )
target_link_libraries( copydialog_test ${QT_LIBRARIES} io )
