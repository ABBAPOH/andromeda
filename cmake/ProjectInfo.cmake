find_package( Git )

macro( get_git_revision VAR)
    if( GIT_EXECUTABLE )
        exec_program(
            ${GIT_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}
            ARGS "rev-parse -q HEAD"
            OUTPUT_VARIABLE ${VAR}
            RETURN_VALUE RET )
        if( "${RET}" GREATER 0 )
            set( ${VAR} "unknown" )
        endif()
    else()
        set( ${VAR} "unknown" )
    endif()
endmacro( get_git_revision )

get_git_revision(GIT_REVISION)
set( PROJECT_VERSION "0.3.0.0" )

if(MSVC)
    add_definitions( -DGIT_REVISION="\""${GIT_REVISION}"\"" )
    add_definitions( -DPROJECT_VERSION=" \""${PROJECT_VERSION}"\" " )
else(MSVC)
    add_definitions( -DGIT_REVISION=\"${GIT_REVISION}\" )
    add_definitions( -DPROJECT_VERSION=\"${PROJECT_VERSION}\" )
endif(MSVC)
