# - Try to find Lomse library
# Once done, this will define
#
#  LOMSE_FOUND - system has Lomse
#  LOMSE_INCLUDE_DIRS - the Lomse include directories
#  LOMSE_LIBRARIES - link these to use Lomse
#  LOMSE_VERSION - detected version of Lomse
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries


include(LibFindMacros)


if( WIN32 )
    set( LOMSE_LIBDIR  ${ROOT_DIR}/packages/lomse/lib )
    set( LOMSE_INCLUDE_DIR  ${ROOT_DIR}/packages/lomse/include )
    set( LOMSE_LIBRARY  "lomse.lib" )

elseif ( UNIX )
    find_path(LOMSE_INCLUDE_DIR NAMES lomse_doorway.h
        PATHS
        /usr/local/include/lomse
        /usr/include/lomse
        /sw/include/lomse
        /opt/local/include/lomse
        /usr/freeware/include/lomse
    )

    find_library(LOMSE_LIBRARY
       NAMES liblomse liblomse.a
       PATHS
       /usr/lib/
       /usr/local/lib
       /sw
       /usr/freeware
    )
endif()


# set the user variables
if( LOMSE_INCLUDE_DIR )
    set( LOMSE_INCLUDE_DIRS   "${LOMSE_INCLUDE_DIR};${LOMSE_INCLUDE_DIR}/agg" )
else()
    set( LOMSE_INCLUDE_DIRS   "${LOMSE_INCLUDE_DIR}" )
endif()
set( LOMSE_LIBRARIES   "${LOMSE_LIBRARY}" )

# handle the QUIETLY and REQUIRED arguments and set LOMSE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Lomse  DEFAULT_MSG  LOMSE_LIBRARY  LOMSE_INCLUDE_DIRS)

MARK_AS_ADVANCED(LOMSE_LIBRARY LOMSE_INCLUDE_DIR)

#MESSAGE ( "LOMSE_INCLUDE_DIRS => ${LOMSE_INCLUDE_DIRS}" )
#MESSAGE ( "LOMSE_LIBRARIES => ${LOMSE_LIBRARIES}" )

