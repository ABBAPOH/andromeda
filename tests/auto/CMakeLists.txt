set (QT_USE_QTTEST TRUE)
include( ${QT_USE_FILE} )

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
#set( CTEST_OUTPUT_ON_FAILURE 1)

function( qt4_wrap_single_cpp SOURCE )
    get_filename_component( SOURCENAME ${SOURCE} NAME_WE )
    qt4_generate_moc( ${SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/${SOURCENAME}.moc )
    set_source_files_properties( ${SOURCE}  OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${SOURCENAME}.moc )
endfunction( qt4_wrap_single_cpp )

add_subdirectory( registrationsystem )
