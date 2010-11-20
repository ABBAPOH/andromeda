set( HEADERS
    utils_global.h
   )
set( SOURCES
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DUTILS_LIBRARY )

add_library( utils SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( utils ${QT_LIBRARIES} widgets )

