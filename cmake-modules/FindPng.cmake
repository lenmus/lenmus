# - Try to find libpng
# Once done, this will define
#
#  PNG_FOUND - system has libpng
#  PNG_INCLUDE_DIRS - the libpng include directories
#  PNG_LIBRARIES - link these to use libpng
#  PNG_VERSION - detected version of libpng
#  PNG_LIBDIR - directory for libraries
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

#include(LibFindMacros)

find_path(PNG_INCLUDE_DIR
    NAMES
        png.h
    PATHS
        /usr/local/include
        /usr/include
        /sw/include
        /opt/local/include
        /usr/libpng/include
        $ENV{libpng_DIR}/include					#Windows
        "C:/Program Files (x86)/libpng/include"		#Windows
        "C:/Program Files/libpng/include"				#Windows
)

find_library(PNG_LIBRARIES
    NAMES
        libpng16
    PATHS
        /usr/lib/
        /usr/local/lib
        /sw
        /usr/libpng
        $ENV{libpng_DIR}/include			    #Windows
        "C:/Program Files (x86)/libpng/bin"		#Windows
        "C:/Program Files (x86)/libpng/lib"		#Windows
        "C:/Program Files/libpng/bin"		        #Windows
        "C:/Program Files/libpng/lib"		        #Windows
)

# set the user variables
set( PNG_INCLUDE_DIRS   "${PNG_INCLUDE_DIR}" )
set( PNG_LIBRARIES   "${PNG_LIBRARIES}" )
if( PNG_INCLUDE_DIR AND PNG_LIBRARIES)
    set( PNG_FOUND   true )
else()
    set( PNG_FOUND   false )
endif()

# handle the QUIETLY and REQUIRED arguments and set PNG_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libpng  DEFAULT_MSG  PNG_LIBRARIES  PNG_INCLUDE_DIRS)

# show the PNG_INCLUDE_DIRS and PNG_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(PNG_LIBRARIES PNG_INCLUDE_DIR)
