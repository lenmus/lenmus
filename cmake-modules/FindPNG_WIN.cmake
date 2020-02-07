# - Try to find libpng in Windows. Default CMake module does not work for me.
# Once done, this will define
#
#  PNG_FOUND - system has libpng
#  PNG_INCLUDE_DIRS - the libpng include directories
#  PNG_LIBRARIES - link these to use libpng
#  PNG_VERSION_STRING - detected version of libpng
#  PNG_DEFINITIONS - You should add_definitions(${PNG_DEFINITIONS}) before
#                    compiling code that includes png library files.
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

find_path(PNG_INCLUDE_DIR
    NAMES
        png.h
    PATHS
        $ENV{PNG_DIR}/include
        "C:/Program Files/libpng/include"
        "C:/Program Files (x86)/libpng/include"
)

set (PNG_LIB_NAME libpng16_static)

find_library(PNG_LIBRARY
    NAMES
        ${PNG_LIB_NAME}
    PATHS
        $ENV{PNG_DIR}/include
        "C:/Program Files/libpng/bin"
        "C:/Program Files/libpng/lib"
        "C:/Program Files (x86)/libpng/bin"
        "C:/Program Files (x86)/libpng/lib"
)

# set the user variables
set( PNG_INCLUDE_DIRS   "${PNG_INCLUDE_DIR}" )
set( PNG_LIBRARIES   "${PNG_LIBRARY}" )
if( PNG_INCLUDE_DIR AND PNG_LIBRARY)
    set( PNG_FOUND   true )
else()
    set( PNG_FOUND   false )
endif()

if (PNG_INCLUDE_DIR AND EXISTS "${PNG_INCLUDE_DIR}/png.h")
  file(STRINGS "${PNG_INCLUDE_DIR}/png.h" png_version_str REGEX "^#define[ \t]+PNG_LIBPNG_VER_STRING[ \t]+\".+\"")

  string(REGEX REPLACE "^#define[ \t]+PNG_LIBPNG_VER_STRING[ \t]+\"([^\"]+)\".*" "\\1" PNG_VERSION_STRING "${png_version_str}")
  unset(png_version_str)
endif ()

# handle the QUIETLY and REQUIRED arguments and set PNG_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libpng  DEFAULT_MSG  PNG_LIBRARY  PNG_INCLUDE_DIRS)

# show the PNG_INCLUDE_DIRS and PNG_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(PNG_LIBRARY PNG_INCLUDE_DIR)
