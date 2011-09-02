# some system (rpm builds) setup LIB_SUFFIX for cmake. If there is no set, try to get it from system
if( NOT LIB_SUFFIX )

    if( UNIX AND NOT APPLE)
        # BSD does not use lib64 for 64bit libs
        if( ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" )
#            message(STATUS "LIB_SUFFIX is disabled for *BSD: ${CMAKE_SYSTEM_NAME}")
            set( LIB_SUFFIX "" )
        else()
            # check 64 bit
            if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
                set( LIB_SUFFIX "" )
            else()
                set( LIB_SUFFIX "64" )
            endif()
        endif()
    else()
        set( LIB_SUFFIX "" )
    endif()

endif()
