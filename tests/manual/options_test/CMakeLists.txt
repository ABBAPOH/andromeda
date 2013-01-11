include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )

add_executable( options_test main.cpp )
target_link_libraries( options_test ${QT_LIBRARIES} libextensionsystem )
