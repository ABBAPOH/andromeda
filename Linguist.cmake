# create function to wrap qt *.ts files
# qt4_wrap_ts(qmfiles tsfile1 tsfile2 ... [SOURCES cppfile1 cppfile2 dir ...] [OPTIONS arguments...])
# outfiles receives .qm generated files from
# .ts files in arguments
# files_or_dirs - directory or path to source files
# arguments - additional arguments for lupdate
# a target lupdate is created for you
# update/generate your translations files
# example: QT4_WRAP_TS(foo_QM ${foo_TS} SOURCES . OPTIONS -recursive -extensions js,qs)
macro( qt4_wrap_ts outfiles )

if( QT_LUPDATE_EXECUTABLE )
    set( CURRENT_VAR TS_FILES )
    set( TS_FILES )
    set( TS_SOURCES )
    set( LUPDATE_ARGS )
    foreach( ARGUMENT ${ARGN} )
        if( ARGUMENT STREQUAL "SOURCES" )
            set( CURRENT_VAR SOURCES )
        elseif( ARGUMENT STREQUAL "OPTIONS" )
            set( CURRENT_VAR LUPDATE_ARGS )
        else( ARGUMENT )
            list( APPEND ${CURRENT_VAR} ${ARGUMENT} )
        endif( ARGUMENT STREQUAL "SOURCES" )
    endforeach( ARGUMENT )

    if( NOT TS_SOURCES )

        list( APPEND TS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR} )
        list( APPEND LUPDATE_ARGS -recursive )
        list( REMOVE_DUPLICATES LUPDATE_ARGS )

    endif( NOT TS_SOURCES )

message("bla" ${TS_SOURCES})

    foreach( it ${TS_FILES} )
        set( tmp "" )

        get_filename_component( _outname ${it} NAME )
        get_filename_component( _outpath ${it} PATH )
        if( NOT _outpath OR NOT IS_ABSOLUTE _outpath )
            set( _outpath ${CMAKE_CURRENT_SOURCE_DIR}/${_outpath} )
        endif()
        set( tmp ${${tmp}} ${_outpath}/${_outname} )
    endforeach( it )
    set( TS_FILES ${tmp} )

#    add_custom_command( OUTPUT ${TS_FILES}
#        COMMAND ${QT_LUPDATE_EXECUTABLE}
#        ${TS_SOURCES}
#        ${LUPDATE_ARGS}
#        -ts ${TS_FILES}
#    )

    add_custom_command( OUTPUT ${TS_FILES} DEPENDS ${TS_SOURCES}
        COMMAND ${QT_LUPDATE_EXECUTABLE}
        ${TS_SOURCES}
        ${LUPDATE_ARGS}
        -ts ${TS_FILES}
        COMMENT "Creating qt translations files"
    )

    if( QT_LRELEASE_EXECUTABLE )

        foreach( it ${TS_FILES} )
            get_filename_component( outfile ${it} NAME_WE)
            get_filename_component( outpath ${it} PATH)

            file( MAKE_DIRECTORY ${QM_OUTPUT_PATH} )
            set( outpath ${QM_OUTPUT_PATH} )
            set( outfile ${outpath}/${outfile}.qm)

            add_custom_command( OUTPUT ${outfile}
                COMMAND ${QT_LRELEASE_EXECUTABLE}
                ARGS ${it} -qm ${outfile}
                DEPENDS ${it}
                COMMENT "Compilation qt translations files"
            )

            set( ${outfiles} ${${outfiles}} ${outfile} )
        endforeach( it )

    else( QT_LRELEASE_EXECUTABLE )
        message(WARNING "lrelease not found. Command for compilation of translations files not created.")
    endif( QT_LRELEASE_EXECUTABLE )

else( QT_LUPDATE_EXECUTABLE )
    message( WARNING "lupdate not found. Translations files not created." )
endif( QT_LUPDATE_EXECUTABLE )

endmacro( qt4_wrap_ts )

macro(copy_qt_qm)

    set( QT_QM_FILES qt_ru.qm qt_cs.qm)
    foreach( QM_FILE ${QT_QM_FILES} )
        configure_file(  ${QT_TRANSLATIONS_DIR}/${QM_FILE} ${QM_OUTPUT_PATH} COPYONLY)
    endforeach( QM_FILE )

endmacro(copy_qt_qm)

copy_qt_qm()
