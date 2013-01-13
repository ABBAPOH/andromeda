include_directories( ${PROJECT_SOURCE_DIR}/include )

add_executable( extensionsystem_test main.cpp )
target_link_libraries( extensionsystem_test ${QT_LIBRARIES} libextensionsystem )
