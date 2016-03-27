#-------------------------------------------------------------------------------------
# This is part of CMake configuration file for building makefiles and installfiles
# for the LenMus Phonascus program
#-------------------------------------------------------------------------------------
# This module creates the include file "lenmus_version.h"
#
#-------------------------------------------------------------------------------------

set( LENMUS_VERSION_MAJOR 5 )
set( LENMUS_VERSION_MINOR 4 )
set( LENMUS_VERSION_PATCH 1 )

# build version string for installer name
set( LENMUS_PACKAGE_VERSION "${LENMUS_VERSION_MAJOR}.${LENMUS_VERSION_MINOR}.${LENMUS_VERSION_PATCH}" )
set( LENMUS_VERSION "${LENMUS_PACKAGE_VERSION}" )
set( LENMUS_VERSION_LONG "${LENMUS_VERSION}-nogit" )

message (STATUS "  LenMus version        = '${LENMUS_VERSION}'")
message (STATUS "  LenMus version string = '${LENMUS_PACKAGE_VERSION}'")

if (EXISTS ${LENMUS_ROOT_DIR}/.git)
  find_package (Git)
  if (NOT GIT_FOUND)
    message(SEND_ERROR "Git package not found." )
  else()
    # get sha1 and dirty status directly from git
    execute_process(COMMAND "${GIT_EXECUTABLE}" log -1 --format=%h
      WORKING_DIRECTORY "${LENMUS_ROOT_DIR}"
      OUTPUT_VARIABLE LENMUS_VERSION_BUILD
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags --long --dirty
      WORKING_DIRECTORY "${LENMUS_ROOT_DIR}"
      OUTPUT_VARIABLE LENMUS_VERSION_GIT
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    message (STATUS "  version git    = '${LENMUS_VERSION_GIT}'")
    if ( ${LENMUS_VERSION_GIT} MATCHES "-dirty$" )
      set (LENMUS_VERSION_BUILD "${LENMUS_VERSION_BUILD}-dirty")
    endif()
    if (NOT ".${LENMUS_VERSION_BUILD}" STREQUAL "." )
      set(LENMUS_VERSION_LONG "${LENMUS_PACKAGE_VERSION}+${LENMUS_VERSION_BUILD}")
    else()
      set(LENMUS_VERSION_LONG "${LENMUS_PACKAGE_VERSION}")
    endif()
  endif()
endif()
message (STATUS "  LenMus version long   = '${LENMUS_VERSION_LONG}'" )

# define a header file to pass version information to source code
configure_file(
    "${LENMUS_ROOT_DIR}/lenmus_version.h.cmake"
    "${CMAKE_BINARY_DIR}/lenmus_version.h"
)
