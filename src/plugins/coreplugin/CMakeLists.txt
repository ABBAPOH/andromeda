set( HEADERS
    coreplugin_global.h
    icore.h
    core.h
   )
set( SOURCES
    icore.cpp
    core.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/registrationsystem )

add_definitions( -DCOREPLUGIN_LIBRARY )

add_library( coreplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( coreplugin ${QT_LIBRARIES} registrationsystem )

