find_package( git )

macro( get_git_revision VAR)
if( GIT_EXECUTABLE )
exec_program(
    ${GIT_EXECUTABLE}
    ${PROJECT_SOURCE_DIR}
    ARGS "rev-parse -q HEAD"
    OUTPUT_VARIABLE ${VAR} )
else()
    set( ${VAR} "unknown" )
endif()
endmacro( get_git_revision )

get_git_revision(GIT_REVISION)
set( PROJECT_VERSION "0.0.0.1" )
