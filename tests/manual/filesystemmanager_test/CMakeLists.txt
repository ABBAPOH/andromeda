set( LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins )

set( HEADERS
    mainwindow.h
   )
set( SOURCES
    main.cpp
    mainwindow.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins )

add_executable( filesystemmanager_test ${HEADERS} ${UI_HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( filesystemmanager_test ${QT_LIBRARIES} extensionsystem filemanagerplugin )
