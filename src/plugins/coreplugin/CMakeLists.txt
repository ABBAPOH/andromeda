set( HEADERS
    coreplugin_global.h
    icore.h
    core.h
    core_p.h
    coreplugin.h
    ifile.h
   )
set( SOURCES
    icore.cpp
    core.cpp
    coreplugin.cpp
    ifile.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/registrationsystem )
include_directories( editormanager )

add_definitions( -DCOREPLUGIN_LIBRARY )

add_library( coreplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( coreplugin ${QT_LIBRARIES} extensionsystem registrationsystem editormanager )

install_spec( coreplugin.spec )
