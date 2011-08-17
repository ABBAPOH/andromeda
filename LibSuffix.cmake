# some system (rpm builds) setup LIB_SUFFIX for cmake. If there is no set, try to get it from system
IF (NOT LIB_SUFFIX) 
        IF (NOT APPLE) 
                # check 64 bit 
                IF (CMAKE_SIZEOF_VOID_P EQUAL 4) 
                        SET (LIB_SUFFIX "") 
                ELSE (CMAKE_SIZEOF_VOID_P EQUAL 4) 
                        SET (LIB_SUFFIX "64") 
                        MESSAGE(STATUS " LIB_SUFFIX is set to '${LIB_SUFFIX}'") 
                ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 4) 
        ELSE (NOT APPLE) 
                SET (LIB_SUFFIX "") 
        ENDIF (NOT APPLE)
        # BSD does not use lib64 for 64bit libs
        IF (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
                message(STATUS "LIB_SUFFIX is disabled for *BSD: ${CMAKE_SYSTEM_NAME}")
                SET (LIB_SUFFIX "")
        ENDIF()
        #
ENDIF (NOT LIB_SUFFIX)
