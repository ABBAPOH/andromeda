set( HEADERS
    registrationsystem_global.h
    registrationmanager.h
   )
set( SOURCES
    registrationmanager.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DREGISTRATIONSYSTEM_LIBRARY )
add_definitions( -fPIC )

add_library( registrationsystem STATIC ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( registrationsystem ${QT_LIBRARIES} )

