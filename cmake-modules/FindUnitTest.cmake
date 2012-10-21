# - Try to find unittest++ library
# Once done, this will define
#
#  UNITTEST_FOUND - system has unittest++
#  UNITTEST_INCLUDE_DIRS - the unittest++ include directories
#  UNITTEST_LIBRARIES - link these to use unittest++
#  UNITTEST_VERSION - detected version of unittest++
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries


include(LibFindMacros)

if( WIN32 )
    set( UNITTEST_LIBDIR  ${ROOT_DIR}/packages/UnitTest++/lib )
    set( UNITTEST_INCLUDE_DIR  ${ROOT_DIR}/packages/UnitTest++/src )
    set( UNITTEST_LIBRARY  "UnitTest++.vsnet2003.lib" )

elseif ( UNIX )
    find_path(UNITTEST_INCLUDE_DIR NAMES UnitTest++.h
        PATHS
        /usr/local/include/unittest++
        /usr/include/unittest++
        /sw/include/unittest++
        /opt/local/include/unittest++
        /usr/freeware/include/unittest++
    )

    find_library(UNITTEST_LIBRARY
       NAMES libUnitTest++.a
       PATHS
       /usr/lib/
       /usr/local/lib
       /sw
       /usr/freeware
    )
endif()


# set the user variables
set( UNITTEST_INCLUDE_DIRS   "${UNITTEST_INCLUDE_DIR}" )
set( UNITTEST_LIBRARIES   "${UNITTEST_LIBRARY}" )
if( UNITTEST_INCLUDE_DIR AND UNITTEST_LIBRARY)
    set( UNITTEST_FOUND   true )
else()
    set( UNITTEST_FOUND   false )
endif()

# handle the QUIETLY and REQUIRED arguments and set UNITTEST_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(unittest++  DEFAULT_MSG  UNITTEST_LIBRARY  UNITTEST_INCLUDE_DIRS)

# show the UNITTEST_INCLUDE_DIRS and UNITTEST_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(UNITTEST_LIBRARY UNITTEST_INCLUDE_DIR)

#MESSAGE ( "UNITTEST_INCLUDE_DIRS => ${UNITTEST_INCLUDE_DIRS}" )
#MESSAGE ( "UNITTEST_LIBRARIES => ${UNITTEST_LIBRARIES}" )

