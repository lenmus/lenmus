# - Try to find PortMidi
# Once done, this will define
#
#  PortMidi_FOUND - system has PortMidi
#  PortMidi_INCLUDE_DIRS - the PortMidi include directories
#  PortMidi_LIBRARIES - link these to use PortMidi
#  PortMidi_VERSION - detected version of PortMidi
#  PortMidi_LIBDIR - directory for libraries
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

include(LibFindMacros)

if( WIN32 )
    set( PortMidi_LIBDIR  ${ROOT_DIR}/packages/wxMidi/lib/pm )
    set( PortMidi_INCLUDE_DIR  ${ROOT_DIR}/wxMidi/include )
    set( PortMidi_LIBRARY  "portmidi.lib" )
    set( PortTime_LIBRARY  "porttime.lib" )

elseif ( UNIX )
    find_path(PortMidi_INCLUDE_DIR NAMES portmidi.h porttime.h
        PATHS
        /usr/local/include
        /usr/include
        /sw/include
        /opt/local/include
        /usr/freeware/include
    )

    find_library(PortMidi_LIBRARY
       NAMES libportmidi.so libportmidi.a
       PATHS
       /usr/lib/
       /usr/local/lib
       /sw
       /usr/freeware
    )

    find_library(PortTime_LIBRARY
       NAMES libporttime.so libporttime.a
       PATHS
       /usr/lib/
       /usr/local/lib
       /sw
       /usr/freeware
    )
endif()

# set the user variables
set( PortMidi_INCLUDE_DIRS   "${PortMidi_INCLUDE_DIR}" )
set( PortMidi_LIBRARIES   "${PortMidi_LIBRARY};${PortTime_LIBRARY}" )
if( PortMidi_INCLUDE_DIR AND PortMidi_LIBRARY)
    set( PortMidi_FOUND   true )
else()
    set( PortMidi_FOUND   false )
endif()

# handle the QUIETLY and REQUIRED arguments and set PortMidi_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PortMidi  DEFAULT_MSG  PortMidi_LIBRARY  PortMidi_INCLUDE_DIRS)

# show the PortMidi_INCLUDE_DIRS and PortMidi_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(PortMidi_LIBRARY PortMidi_INCLUDE_DIR)

MESSAGE ( "PortMidi_INCLUDE_DIRS => ${PortMidi_INCLUDE_DIRS}" )
MESSAGE ( "PortMidi_LIBRARIES => ${PortMidi_LIBRARIES}" )

