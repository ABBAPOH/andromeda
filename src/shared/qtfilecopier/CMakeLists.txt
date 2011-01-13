include_directories(${CMAKE_CURRENT_BINARY_DIR})

function( qt4_wrap_single_cpp SOURCE TARGET )
    get_filename_component( SOURCENAME ${SOURCE} NAME_WE )
    qt4_generate_moc( ${SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} )
    set_source_files_properties( ${SOURCE}  OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} )
endfunction( qt4_wrap_single_cpp )

set( HEADERS
    qtfilecopier.h
    QtFileCopier
)
set( SOURCES qtfilecopier.cpp )

qt4_wrap_single_cpp( qtfilecopier.cpp moc_qtfilecopier.cpp )
qt4_automoc( qtfilecopier.cpp )

#add_definitions( -DQT_QTCOPYDIALOG_EXPORT )
add_definitions( -fPIC )

add_library( qtfilecopier STATIC ${HEADERS} ${SOURCES}  )
target_link_libraries( qtfilecopier ${QT_LIBRARIES} )
