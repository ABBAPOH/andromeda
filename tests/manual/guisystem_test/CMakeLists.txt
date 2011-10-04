set( QT_USE_QTWEBKIT TRUE )
include( ${QT_USE_FILE} )

set( HEADERS
    testview.h
    testview2.h
    test.h
   )
set( SOURCES
    main.cpp
    testview.cpp
    testview2.cpp
    test.cpp
   )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

add_executable( guisystem_test ${HEADERS} ${UI_HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( guisystem_test ${QT_LIBRARIES} guisystem )
