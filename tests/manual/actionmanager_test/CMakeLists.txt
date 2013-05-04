include_directories( ${PROJECT_SOURCE_DIR}/include )

set( HEADERS
    mainwindow.h
   )
set( SOURCES
    main.cpp
    mainwindow.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

add_executable( actionmanager_test ${HEADERS} ${UI_HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( actionmanager_test ${QT_LIBRARIES} libguisystem )
