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
    find_path(LOMSE_INCLUDE_DIR NAMES lomse_version.h
        PATHS
        /usr/include/lomse
        /usr/local/include/lomse
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
set( LOMSE_INCLUDE_DIRS   "${LOMSE_INCLUDE_DIR}" )
set( LOMSE_LIBRARIES   "${LOMSE_LIBRARY}" )


# ------------------------------------------------------------------------
# Extract LOMSE_VERSION from lomse_version.h
# ------------------------------------------------------------------------

file(STRINGS ${LOMSE_INCLUDE_DIRS}/lomse_version.h LOMSE_VERSION_LIST)
list (GET LOMSE_VERSION_LIST 5 MAJOR_LINE)
list (GET LOMSE_VERSION_LIST 6 MINOR_LINE)
list (GET LOMSE_VERSION_LIST 7 TYPE_LINE)
list (GET LOMSE_VERSION_LIST 8 PATCH_LINE)
string(REGEX REPLACE "\#define LOMSE_VERSION_MAJOR    " "" LOMSE_VERSION_MAJOR "${MAJOR_LINE}")
string(REGEX REPLACE "\#define LOMSE_VERSION_MINOR    " "" LOMSE_VERSION_MINOR "${MINOR_LINE}")
string(REGEX REPLACE "\#define LOMSE_VERSION_TYPE     " "" LOMSE_VERSION_TYPE "${TYPE_LINE}")
string(REGEX REPLACE "\#define LOMSE_VERSION_PATCH    " "" LOMSE_VERSION_PATCH "${PATCH_LINE}")

set( LOMSE_REVISION "0")

#message ("major = '${LOMSE_VERSION_MAJOR}'") 
#message ("minor = '${LOMSE_VERSION_MINOR}'") 
#message ("type = '${LOMSE_VERSION_TYPE}'") 
#message ("patch = '${LOMSE_VERSION_PATCH}'") 
#message ("revision = '${LOMSE_REVISION}'") 

set(LOMSE_LIB_VERSION "${LOMSE_VERSION_MAJOR}.${LOMSE_VERSION_MINOR}.${LOMSE_VERSION_PATCH}" )



# handle the QUIETLY and REQUIRED arguments and set LOMSE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Lomse  DEFAULT_MSG  LOMSE_LIBRARY  LOMSE_INCLUDE_DIRS)

MARK_AS_ADVANCED(LOMSE_LIBRARY LOMSE_INCLUDE_DIR)

#MESSAGE ( "LOMSE_INCLUDE_DIRS => ${LOMSE_INCLUDE_DIRS}" )
#MESSAGE ( "LOMSE_LIBRARIES => ${LOMSE_LIBRARIES}" )

