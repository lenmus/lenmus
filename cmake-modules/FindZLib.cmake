# - Try to find ZLib
# Once done, this will define
#
#  ZLIB_FOUND - system has ZLib
#  ZLIB_INCLUDE_DIRS - the ZLib include directories
#  ZLIB_LIBRARY - link these to use ZLib
#  ZLIB_VERSION - detected version of ZLib
#  ZLIB_LIBDIR - directory for libraries
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

find_path(ZLIB_INCLUDE_DIR
    NAMES
        zlib.h
    PATHS
        /usr/local/include
        /usr/include
        /sw/include
        /opt/local/include
        /usr/freeware/include
        $ENV{ZLib_DIR}/include					    #Windows
        "C:/Program Files (x86)/zlib/include"		#Windows
        "C:/Program Files/zlib/include"				#Windows
)

find_library(ZLIB_LIBRARY
    NAMES
        zlib
	z
    PATHS
        /usr/lib/
        /usr/local/lib
        /sw
        /usr/freeware
        $ENV{ZLib_DIR}/include			        #Windows
        "C:/Program Files (x86)/zlib/bin"		#Windows
        "C:/Program Files (x86)/zlib/lib"		#Windows
        "C:/Program Files/zlib/bin"		        #Windows
        "C:/Program Files/zlib/lib"		        #Windows
)

# set the user variables
set( ZLIB_INCLUDE_DIRS   "${ZLIB_INCLUDE_DIR}" )
set( ZLIB_LIBRARIES   "${ZLIB_LIBRARY}" )
if( ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
    set( ZLIB_FOUND   true )
else()
    set( ZLIB_FOUND   false )
endif()

# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZLib  DEFAULT_MSG  ZLIB_LIBRARY  ZLIB_INCLUDE_DIRS)

# show the ZLIB_INCLUDE_DIRS and ZLIB_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
