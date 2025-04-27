#---------------------------------------------------------------------------------------
#  This file is part of LenMus Phonascus program
#  Copyright (c) 2002-2021 LenMus project
#
#  This program is free software; you can redistribute it and/or modify it under the
#  terms of the GNU General Public License as published by the Free Software Foundation,
#  either version 3 of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
#  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along with this
#  program. If not, see <http:#www.gnu.org/licenses/>.
#
#  For any comment, suggestion or feature request, please contact the manager of
#  the project at cecilios@users.sourceforge.net
#
#---------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------
# This is a CMake configuration file for building makefiles and installfiles for
# the LenMus Phonascus program
#
# To use it you need CMake which can be downloaded from http://www.cmake.org/
#
# Usage
#   cmake [<options>] <source-tree>
#
# example:
#   cmake ../lenmus
#
# The default target (in Linux) is "Unix Makefiles". If you would like to generate
# a diferent makefile use option -G<desired-target>. For instance:
#   cmake -G "Ninja" ...
#
#
# Default configuration
# ~~~~~~~~~~~~~~~~~~~~~~~
# - Build type is 'Release'
#
# - Unix: only main package containing the binaries will be generated.
#   Windows: a boundle installer containing all will be generated.
#   See "Packages to build (Only for Unix)" section.
#
# - Unix: The FluidR3_GM soundfont *IS NOT* included in the package.
#   Windows: It *IS INCLUDED*.
#   See "SoundFont for the internal synthesizer" section.
#
# - The Barvura.otf music font *IS INCLUDED* in the package.
#   See "Bravura music font required to render scores" section.
#
#
# Installation folders
# ~~~~~~~~~~~~~~~~~~~~~
# The default installation prefix is "/usr/local" for Linux and
# "C:/Program Files (x86)" or "C:/Program Files" for Windows. 
# You can change the install location by running cmake like this:
#   cmake -DCMAKE_INSTALL_PREFIX=/new/install/prefix ...
#
# The default folder (in Linux) for installing man pages is
#   /usr/share/man
# You can specify other location by using option MAN_INSTALL_DIR:
#   cmake -DMAN_INSTALL_DIR=/new/install/location/for/manpages ...
#
#
# Packages to build (Only for Unix)
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# In Linux, the installation is divided into several packages:
# - lenmus: (main package) binaries and related files
# - lenmus-common: common support files not bound to any particular
#       release, architecture or language, such as icons, samples and templates.
# - lenmus-i18n: the message catalogues for lenmus and related translated files
# - lenmus-ebooks: the eBooks (all translations).
# In addition, a boundle package containing all can be generated.
#
# If nothing specified in build options, only the main package (the package
# containing the binaries) is built. To build any other packages you must 
# enable the option for the desired package:
#   cmake -DBUILD_PKG_<pkg>:BOOL=ON ...
# Replace <pkg> by the desired package name:
#   pkg = { MAIN | COMMON | I18N | EBOOKS | BOUNDLE }
# For instance, to build lenmus-i18n and lenmus-ebooks packages:
#   cmake -DBUILD_PKG_I18N:BOOL=ON -DBUILD_PKG_EBOOKS:BOOL=ON ...
# Including the option to build the boundle package will force to ignore
# any other options to build other packages.
#
#
# Unit Tests
# ~~~~~~~~~~~~
# By default unit tests are also build. You can reduce executable size by
# disabling this option:
#   cmake -DLENMUS_ENABLE_UNIT_TESTS:BOOL=OFF ...
#
#
# SoundFont for the internal synthesizer
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# The internal synthesizer needs the FluidR3_GM.sf2 soundfont. Therefore, it is
# a dependency to have it installed in the system. In Linux this
# is provided for specific packages such as the "fluid-soundfont-gm" package in
# Debian and derivatives.
#
# If you would like that this cmake script will download the FluidR3_GM.sf2
# sound font in case it is not found in the system, you can use the option:
#   cmake -DLENMUS_DOWNLOAD_SOUNDFONT:BOOL=ON ...
#
# By default, the FluidR3 soundfont must not be included in Linux packages and
# the required soundfont package must be set as an install dependency. But it is
# necessary to install it in Windows. Therefore, the default behaviour is
# to include it in Windows installer but not in Linux packages.
# You can control this behaviour with the option:
#   cmake -DLENMUS_INSTALL_SOUNDFONT:BOOL={OFF | ON} ...
#
#
# Bravura music font required to render scores
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# By default, this font must not be included in Linux packages and the required
# font package must be set as an install dependency.
# As it is necessary to install it in Windows, and as in some Linux distros
# there is no package to install this font, this cmake scripts includes this
# font in the installer. To prevent this, you can use the option:
#   cmake -DLENMUS_INSTALL_BRAVURA_FONT:BOOL=OFF ...
#
# When it is requested that Bravura font be included in the package, this
# cmake script will download the Bravura.otf font in case it is not found in
# the system. You can use the option:
#   cmake -DLENMUS_DOWNLOAD_BRAVURA_FONT:BOOL=OFF ...
# to force not to download it if not found in the system. But if the font is
# not found, it will not be installed in any case.
#
#
#
#
#-------------------------------------------------------------------------------------
# Modifying this script
# ==========================
#
# How to add a new language
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~
# To add a new translation:
#   Search for ADD_LANG and modify CMakeList.txt in these points
#   Check that the added language is supported by NSIS in NSIS/Contrib/Language files/
#
#-------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
message(STATUS "Using cmake version ${CMAKE_VERSION}")

# project name
project(lenmus)
set( LENMUS_APP_NAME     "Lenmus Phonascus" )
set( LENMUS_VENDOR_NAME  "LenMus" )
set( LENMUS_VENDOR_SITE  "http://www.lenmus.org" )

# Define possible builds
set(CMAKE_CONFIGURATION_TYPES Debug Release)


# display settings
message("Initial settings:")
message("     Using CMake version ${CMAKE_VERSION}")
message("     CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message("     CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message("     CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

# main directories 
set( LENMUS_ROOT_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )
set( SRC_DIR  ${LENMUS_ROOT_DIR}/src )
set( RESOURCES_DIR  ${LENMUS_ROOT_DIR}/res )
set( PACKAGES_DIR ${LENMUS_ROOT_DIR}/packages )
set( OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/bin )
set( EXECUTABLE_OUTPUT_PATH ${OUTDIR})
set( LOMSE_ROOT_DIR ${LENMUS_ROOT_DIR}/lomse/trunk )
set( LOMSE_SRC_DIR ${LOMSE_ROOT_DIR}/src )
set( LOMSE_PKG_DIR ${LOMSE_ROOT_DIR}/packages )

# check that compiler supports namespace sdt
include(TestForSTDNamespace)
if(CMAKE_NO_STD_NAMESPACE)
  message(FATAL_ERROR "The compiler doesn't support namespace std.")
endif()

# force to use c++11
set(CMAKE_CXX_STANDARD 11)                #require c+11 or greater
set(CMAKE_CXX_STANDARD_REQUIRED ON)     #prevent fallback to any previous standard

#check that the compiler supports c++11 and std::regex
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.9
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        message(FATAL_ERROR "GCC version must be at least 4.9!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # require at least clang 3.4
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4)
        message(FATAL_ERROR "Clang version must be at least 3.4!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # require at least clang 3.4
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15)
        message(FATAL_ERROR "MSVC version must be at least 15!")
    endif()
else()
    message(WARNING "** Warning **: You are using an untested compiler! (${CMAKE_CXX_COMPILER_ID}) LenMus has only been tested with GCC, Clang and MSVC")
endif()


# directories to search for CMake modules
set( CMAKE_MODULE_PATH    "${LENMUS_ROOT_DIR}/cmake-modules" )

# some options for building
OPTION(LENMUS_USE_LOMSE_SOURCES
    "Build LenMus using Lomse sources instead of using the library"
    ON)
OPTION(LENMUS_ENABLE_UNIT_TESTS
    "Include also unit tests in LenMus executable"
    ON)
OPTION(LENMUS_DOWNLOAD_SOUNDFONT
    "Download FluidR3 soundfont if not present in source tree"
    OFF)
OPTION(LENMUS_DOWNLOAD_BRAVURA_FONT
    "Download Bravura.otf font if not present in source tree"
    ON)
OPTION(LENMUS_INSTALL_BRAVURA_FONT
    "Include Bravura music font in the package in the main and boundle packages"
    ON)
if(WIN32)
    OPTION(LENMUS_INSTALL_SOUNDFONT
        "Include FluidR3 soundfont in the package in the main and boundle packages"
        ON)
else()
    OPTION(LENMUS_INSTALL_SOUNDFONT
        "Include FluidR3 soundfont in the package in the main and boundle packages"
        OFF)
endif()
OPTION(BUILD_PKG_MAIN
    "Build package 'lenmus-main': Binaries and related files"
    "OFF")
OPTION(BUILD_PKG_COMMON
    "Build package 'lenmus-common': Common support files, such as icons"
    "OFF")
OPTION(BUILD_PKG_I18N
    "Build package 'lenmus-i18n': Message catalogues and translated files (but not eBooks)"
    "OFF")
OPTION(BUILD_PKG_EBOOKS
    "Build package 'lenmus-ebooks': eBooks (all translations)"
    "OFF")
OPTION(BUILD_PKG_BOUNDLE
    "Build package 'lenmus-ebooks': eBooks (all translations)"
    "OFF")

if(NOT APPLE)
set(MAN_INSTALL_DIR "/usr/share/man" 
    CACHE STRING "Directory for installing man pages")
else()
set(MAN_INSTALL_DIR "/usr/local/man"
    CACHE STRING "Directory for installing man pages")
endif()

#Set a default build type if none was specified
# https://stackoverflow.com/questions/24460486/cmake-build-type-is-not-being-used-in-cmakelists-txt
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

#disable pragma warning when compiling with GCC
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
endif()

#------------------------------------------------------
# macro STRING_UNQUOTE(var str)
#   Remove quote marks or double quote marks around a string.
#   If the string is not quoted, then content of str is copied to var
#   * Parameters:
#     + var: A variable that stores the result.
#     + str: A string.
MACRO(STRING_UNQUOTE var str)
    set(_ret "${str}")
    STRING(LENGTH "${str}" _strLen)

    # if _strLen > 1
    #   if lCh and rCh are both "'"
    #      Remove _lCh and _rCh
    #   elseif lCh and rCh are both "\""
    #      Remove _lCh and _rCh
    # Otherwise don't touch
    IF(_strLen GREATER 1)
        STRING(SUBSTRING "${str}" 0 1 _lCh)
        MATH(EXPR _strLen_1 ${_strLen}-1)
        MATH(EXPR _strLen_2 ${_strLen_1}-1)
        STRING(SUBSTRING "${str}" ${_strLen_1} 1 _rCh)
        #MESSAGE("_lCh=${_lCh} _rCh=${_rCh} _ret=|${_ret}|")
        IF("${_lCh}" STREQUAL "'" AND "${_rCh}" STREQUAL "'")
            STRING(SUBSTRING "${_ret}" 1 ${_strLen_2} _ret)
        ELSEIF ("${_lCh}" STREQUAL "\"" AND "${_rCh}" STREQUAL "\"")
            STRING(SUBSTRING "${_ret}" 1 ${_strLen_2} _ret)
        ENDIF()
    ENDIF()
    set(${var} "${_ret}")
ENDMACRO()


# create lenmus_version.h with version information from Git
include( ${LENMUS_ROOT_DIR}/version-info.cmake )

add_custom_target (lenmus-version ALL
  COMMAND ${CMAKE_COMMAND} -D LENMUS_ROOT_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/version-info.cmake
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Setting LenMus version information ...")


# set up configuration variables for lenmus_config.h
#------------------------------------------------------


# platform
if(WIN32)
    set( LENMUS_PLATFORM_WIN32 "1")
    set( LENMUS_PLATFORM_UNIX "0")
    set( LENMUS_PLATFORM_MAC "0")
elseif(UNIX AND NOT APPLE)
    set( LENMUS_PLATFORM_WIN32 "0")
    set( LENMUS_PLATFORM_UNIX "1")
    set( LENMUS_PLATFORM_MAC "0")
elseif(APPLE)
    set( LENMUS_PLATFORM_WIN32 "0")
    set( LENMUS_PLATFORM_UNIX "0")
    set( LENMUS_PLATFORM_MAC "1") #Mac config differs from unix config only in a few places
endif()

# compiler
if(MSVC)
    set( LENMUS_COMPILER_MSVC "1")
    set( LENMUS_COMPILER_GCC "0")
elseif(CMAKE_COMPILER_IS_GNUCC)
    set( LENMUS_COMPILER_MSVC "0")
    set( LENMUS_COMPILER_GCC "1")
elseif(APPLE)    #???  set( LENMUS_COMPILER_MSVC "0") seems to be necessary.  Does  LENMUS_COMPILER_GCC  need to be defined?
     set( LENMUS_COMPILER_MSVC "0")
     set( LENMUS_COMPILER_GCC "0")
endif()

# build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set( LENMUS_DEBUG_BUILD   1)
    set( LENMUS_RELEASE_INSTALL 0)
else ()
    set( LENMUS_DEBUG_BUILD   0)
    set( LENMUS_RELEASE_INSTALL 1)
endif ()

message("LenMus configuration for building:")
message(STATUS "LENMUS_DEBUG_BUILD set to ${LENMUS_DEBUG_BUILD}")

# path to scores for tests
set( LENMUS_TEST_SCORES_PATH     "\"${LENMUS_ROOT_DIR}/scores/samples/test_set/\"" )
set( LENMUS_SOURCE_ROOT          "\"${LENMUS_ROOT_DIR}/\"" )

# Audio driver and device to use
if(UNIX AND NOT APPLE)
    set(LENMUS_AUDIO_DRIVER "alsa")
    set(LENMUS_ALSA_DEVICE "default")
elseif(WIN32)
    set(LENMUS_AUDIO_DRIVER "dsound")
    set(LENMUS_DSOUND_DEVICE "default")
elseif(APPLE)
    set(LENMUS_AUDIO_DRIVER "coreaudio")
    set(LENMUS_COREAUDIO_DEVICE "default")
endif()
message("LenMus audio driver: ${LENMUS_AUDIO_DRIVER}")

# determine system arquitecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SYSTEM_ARCHITECTURE "x64")
else()
    set(SYSTEM_ARCHITECTURE "x86")
endif()
message(STATUS "SYSTEM_ARCHITECTURE = ${SYSTEM_ARCHITECTURE}" )


# installation folders
# ------------------------------------------------------------------------------
set(LENMUS_USE_SOURCE_TREE 0)

#set install folder 
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT
    OR (${CMAKE_INSTALL_PREFIX} STREQUAL "C:/Program Files/${CMAKE_PROJECT_NAME}")
    OR (${CMAKE_INSTALL_PREFIX} STREQUAL "C:/Program Files (x86)/${CMAKE_PROJECT_NAME}")
  )
    message(STATUS "    CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT = ${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}")
    #Fix CMAKE_INSTALL_PREFIX for Windows.
    #Sometimes it points to 'Program Files (x86)' for x64 builds
    if(WIN32)
        message(STATUS "    Win32. SYSTEM_ARCHITECTURE = ${SYSTEM_ARCHITECTURE}")
        if(${SYSTEM_ARCHITECTURE} STREQUAL "x64")
            set(CMAKE_INSTALL_PREFIX "C:/Program Files/${CMAKE_PROJECT_NAME}")
        else()
            set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/${CMAKE_PROJECT_NAME}")
        endif()
    endif()
endif()
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
set(ROOT_INSTALL ${CMAKE_INSTALL_PREFIX} )
set(LENMUS_INSTALL_ROOT "${ROOT_INSTALL}/" )    #LENMUS_INSTALL_ROOT is needed for lenmus_config.h


#set shared folder
if(UNIX AND NOT APPLE)
    set(SHARED_DIR ${ROOT_INSTALL}/share/${CMAKE_PROJECT_NAME}/${LENMUS_PACKAGE_VERSION} )
elseif(APPLE)
    set(SHARED_DIR ${ROOT_INSTALL}/Resources )
elseif(WIN32)
    set(SHARED_DIR ${ROOT_INSTALL} )
endif()

message(STATUS "SHARED_DIR = ${SHARED_DIR}" )



# set up other variables
#--------------------------------------------------------

# add headers directories from source tree
include_directories(
    ${LENMUS_ROOT_DIR}/include
    ${LENMUS_ROOT_DIR}/packages/wxMidi/src
)


#determine packages to build
#------------------------------------------------------------------------
if( WIN32 )
    #only boundle package is possible
    set(BUILD_PKG_BOUNDLE 1)
elseif( APPLE )
    #force to build the boundle package
    set(BUILD_PKG_BOUNDLE 1)
elseif( UNIX )
    if((NOT BUILD_PKG_BOUNDLE)
        AND (NOT BUILD_PKG_MAIN)
        AND (NOT BUILD_PKG_COMMON)
        AND (NOT BUILD_PKG_I18N)
        AND (NOT BUILD_PKG_EBOOKS)
      )
        #nothing requested. Build main package
        set(BUILD_PKG_MAIN 1)
    endif()
endif()

message("Packages to build:")
message(STATUS "BUILD_PKG_MAIN = ${BUILD_PKG_MAIN}")
message(STATUS "BUILD_PKG_COMMON = ${BUILD_PKG_COMMON}")
message(STATUS "BUILD_PKG_I18N = ${BUILD_PKG_I18N}")
message(STATUS "BUILD_PKG_EBOOKS = ${BUILD_PKG_EBOOKS}")
message(STATUS "BUILD_PKG_BOUNDLE = ${BUILD_PKG_BOUNDLE}")

if(BUILD_PKG_BOUNDLE OR BUILD_PKG_MAIN)
    set(BUILD_PROGRAM 1)
endif()



if (BUILD_PROGRAM)

##########################################################################
#
# Dependencies
# Check for needed libraries and set all related includes, flags, etc. 
#
###############################################################################

message("Checking for LenMus dependencies:")

# Check for FluidR3_GM.sf2 sound font required by internal synthesizer
# package 'fluid-soundfont-gm'
set (INSTALL_SOUNDFONT ${LENMUS_INSTALL_SOUNDFONT})
find_path(FLUIDR3_PATH
          NAMES FluidR3_GM.sf2
          PATHS
              /usr/share/sounds/sf2     # Ubuntu and possibly all Debian derivatives
              /usr/share/soundfonts     # Red Hat
              ${LENMUS_ROOT_DIR}/res/sounds     #local builds
         )

if("${FLUIDR3_PATH}" STREQUAL "FLUIDR3_PATH-NOTFOUND")
    if (LENMUS_DOWNLOAD_SOUNDFONT)
        message(STATUS "FluidR3_GM.sf2 soundfont not found. Downloading it ...")
        file(DOWNLOAD 
            "https://github.com/lenmus/lenmus/releases/download/Release_5.5.0/FluidR3_GM.sf2"
            "${LENMUS_ROOT_DIR}/res/sounds/FluidR3_GM.sf2"
            SHOW_PROGRESS
            EXPECTED_HASH MD5=af289497caf8c76d97fdc67ec8409f05
            STATUS status
            LOG log
        )

        list(GET status 0 status_code)
        list(GET status 1 status_string)
        
        if(NOT status_code EQUAL 0)
            message(FATAL_ERROR "Error downloading FluidR3%20GM.sf2. Status_code: ${status_code}
                status_string: ${status_string}
                log: ${log}
                ")
        endif()
        message(STATUS "FluidR3_GM.sf2 soundfont downloaded.")
        set(LENMUS_SOUNDFONT_PATH "${LENMUS_ROOT_DIR}/res/sounds/")
        set (INSTALL_SOUNDFONT ON)

    else(LENMUS_DOWNLOAD_SOUNDFONT)
        message(FATAL_ERROR "FluidR3_GM.sf2 soundfont not found. Aborting...")
    endif(LENMUS_DOWNLOAD_SOUNDFONT)

else()
    set(LENMUS_SOUNDFONT_PATH ${FLUIDR3_PATH})
    message(STATUS "FluidR3_GM.sf2 found in ${FLUIDR3_PATH}")
endif()


# Check for Bravura music font required to render scores
set (INSTALL_BRAVURA_FONT ${LENMUS_INSTALL_BRAVURA_FONT})
find_path(BRAVURA_FONT_PATH
          NAMES Bravura.otf
          PATHS
              /usr/share/fonts/truetype/               # Red Hat & fonts-lenmus-bravura.deb
              ${LENMUS_ROOT_DIR}/res/fonts             # local builds
              ${LENMUS_ROOT_DIR}/res/bravura/src       # builds for PPA at Launchpad.net
         )
if("${BRAVURA_FONT_PATH}" STREQUAL "BRAVURA_FONT_PATH-NOTFOUND")
    if (LENMUS_DOWNLOAD_BRAVURA_FONT)
        message(STATUS "Bravura font not found. Downloading it ...")
        file(DOWNLOAD 
            "https://github.com/steinbergmedia/bravura/raw/master/redist/otf/Bravura.otf"
            "${LENMUS_ROOT_DIR}/res/fonts/Bravura.otf"
            SHOW_PROGRESS
#            EXPECTED_HASH MD5=863c90e6f99c264a1747856130ebdc3c
            STATUS status
            LOG log
        )

        list(GET status 0 status_code)
        list(GET status 1 status_string)
        
        if(NOT status_code EQUAL 0)
            message(FATAL_ERROR "Error downloading Bravura font. Status_code: ${status_code}
                status_string: ${status_string}
                log: ${log}
                ")
        endif()
        message(STATUS "Bravura font downloaded.")
        set(BRAVURA_FONT_PATH "${LENMUS_ROOT_DIR}/res/fonts/")
        set(INSTALL_BRAVURA_FONT ON)

    else(LENMUS_DOWNLOAD_BRAVURA_FONT)
        if (WIN32)
            message(FATAL_ERROR "Bravura font not found. Aborting...")
        else()
            message(STATUS "Bravura font not found. It will not be installed.")
        endif()
        set(INSTALL_BRAVURA_FONT OFF)
    endif(LENMUS_DOWNLOAD_BRAVURA_FONT)

else()
    message(STATUS "Bravura font found in ${BRAVURA_FONT_PATH}")
endif()

# Check for UnitTest++. Required when unit tests are enabled (default)
if (LENMUS_ENABLE_UNIT_TESTS)
    find_package(UnitTest++)
    if(UNITTEST++_FOUND)
        include_directories(${UNITTEST++_INCLUDE_DIR})
        link_libraries( ${UNITTEST++_LIBRARY} )
        message(STATUS "UnitTest++ found: libraries= ${UNITTEST++_LIBRARY}" )
        message(STATUS "UnitTest++ found: include= ${UNITTEST++_INCLUDE_DIR}" )
    else(UNITTEST++_FOUND)
        message(STATUS "UnitTest++ not found. Unit tests disabled.")
        message(STATUS "UnitTest++_DIR = $ENV{UnitTest++_DIR}")
        set(LENMUS_ENABLE_UNIT_TESTS OFF)
    endif()
endif(LENMUS_ENABLE_UNIT_TESTS)


#change value to "1" or "0", as required in lenmus_config.h
if (LENMUS_ENABLE_UNIT_TESTS)
    set(LENMUS_ENABLE_UNIT_TESTS "1")
else()
    set(LENMUS_ENABLE_UNIT_TESTS "0")
endif()    
if(WIN32)
    #TODO: I have problems to link with UnitTest++, so disable it for now
    set(LENMUS_ENABLE_UNIT_TESTS "0")
endif()
message(STATUS "LENMUS_ENABLE_UNIT_TESTS = " ${LENMUS_ENABLE_UNIT_TESTS} )


# Check for wxWidgets
if(WIN32)
    #See: https://stackoverflow.com/questions/41574177/cmake-findwxwidgets-fails-only-on-first-invocation-of-configure-command
    set(WX_ROOT_DIR "$ENV{wxWidgets_DIR}")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(wxWidgets_CONFIGURATION mswud)
    else()
        set(wxWidgets_CONFIGURATION mswu)
    endif()
    set(wxWidgets_ROOT_DIR "$ENV{wxWidgets_DIR}")
    set(wxWidgets_LIB_DIR "$ENV{wxWidgets_DIR}/lib/vc_x64_lib")
endif(WIN32)
find_package(wxWidgets 3.0 COMPONENTS base core adv aui xml xrc net qa html propgrid stc REQUIRED )
#find_package(wxWidgets 3.0 HINTS "$ENV{wxWidgets_DIR}" COMPONENTS base core adv aui xml xrc net qa html propgrid stc REQUIRED )
IF(wxWidgets_FOUND)
    INCLUDE(${wxWidgets_USE_FILE})
    include_directories( ${wxWidgets_INCLUDE_DIRS} )
    link_libraries( ${wxWidgets_LIBRARIES} )
    message(STATUS "wxWidgets include dirs: " ${wxWidgets_INCLUDE_DIRS})
    message(STATUS "wxWidgets lib dir: " ${wxWidgets_LIB_DIR})
    message(STATUS "wxWidgets libraries: " ${wxWidgets_LIBRARIES})
ELSE(wxWidgets_FOUND)
    message(SEND_ERROR "wxWidgets not found")
    message(STATUS "wxWidgets_DIR = $ENV{wxWidgets_DIR}")
ENDIF(wxWidgets_FOUND)

# Check for Sqlite3
find_package(SQLite3 REQUIRED)
IF(SQLite3_FOUND)
    include_directories( ${SQLite3_INCLUDE_DIR} )
    link_libraries( ${SQLite3_LIBRARIES} )
    message(STATUS "SQLite3 found: libraries= ${SQLite3_LIBRARIES}")
    message(STATUS "SQLite3 found: include= ${SQLite3_INCLUDE_DIR}")
ELSE(SQLite3_FOUND)
    message(SEND_ERROR "SQLite3 not found")
    message(STATUS "SQLite3_DIR = $ENV{SQLite3_DIR}")
ENDIF(SQLite3_FOUND)
include_directories("packages/wxSQLite3/include")

#Check for PortMidi
find_package(PortMidi REQUIRED)
IF(PortMidi_FOUND)
    include_directories( ${PortMidi_INCLUDE_DIRS} )
    link_libraries( ${PortMidi_LIBRARIES} )
    message(STATUS "PortMidi found: include= ${PortMidi_INCLUDE_DIRS}" )
    message(STATUS "PortMidi found: libraries= ${PortMidi_LIBRARIES}" )
ELSE(PortMidi_FOUND)
    message(SEND_ERROR "PortMidi not found")
    message(STATUS "PortMidi_DIR = $ENV{PortMidi_DIR}")
ENDIF(PortMidi_FOUND)

#Check for FluidSynth
#For me, in Windows, even using vcpkg, it's always been a nightmare to build. Bypass after trying for hours ... :-(
if(WIN32)
    set(FLUIDSYNTH_INCLUDE_DIR "$ENV{FluidSynth_DIR}/include")
    set(FLUIDSYNTH_LIBRARIES "$ENV{FluidSynth_DIR}/lib/fluidsynth.lib")
endif(WIN32)
find_package(FluidSynth REQUIRED)
IF(FLUIDSYNTH_FOUND)
    include_directories( ${FLUIDSYNTH_INCLUDE_DIR} )
    link_libraries( ${FLUIDSYNTH_LIBRARIES} )
    message(STATUS "FluidSynth found: include= ${FLUIDSYNTH_INCLUDE_DIR}" )
    message(STATUS "FluidSynth found: libraries= ${FLUIDSYNTH_LIBRARIES}" )
ELSE(FLUIDSYNTH_FOUND)
    message(SEND_ERROR "FluidSynth not found")
    message(STATUS "FLUIDSYNTH_DIR = $ENV{FLUIDSYNTH_DIR}")
ENDIF(FLUIDSYNTH_FOUND)

# Check for Lomse (only if not building from Lomse sources)
if (LENMUS_USE_LOMSE_SOURCES)
    include_directories(
        ${LOMSE_ROOT_DIR}/include
        ${LOMSE_ROOT_DIR}/src/agg/include
        ${LOMSE_ROOT_DIR}/src/agg/font_freetype
        ${LOMSE_ROOT_DIR}/packages/utfcpp
        ${LOMSE_ROOT_DIR}/packages/minizip
        ${LOMSE_ROOT_DIR}/packages
    )
    message(STATUS "LOMSE: Not using Lomse library, but building with its sources")
else()
    find_package(Lomse 0.25)                    
    if( LOMSE_FOUND )
        include_directories( ${LOMSE_INCLUDE_DIRS} )
        link_libraries( ${LOMSE_LIBRARIES} )
        message(STATUS "Lomse found: libraries= ${LOMSE_LIBRARIES}" )
        message(STATUS "Lomse found: include= ${LOMSE_INCLUDE_DIRS}" )
        message(STATUS "Lomse found: version= ${LOMSE_LIB_VERSION}" )
    else()
        message(SEND_ERROR "Lomse package not found.")
    endif()
endif()

# Check for FreeType: header files required by some lomse headers
find_package(Freetype REQUIRED)              
if( FREETYPE_FOUND )
    include_directories( ${FREETYPE_INCLUDE_DIRS} )
    message(STATUS "Freetype found: libraries= ${FREETYPE_LIBRARIES}" )
    message(STATUS "Freetype found: include= ${FREETYPE_INCLUDE_DIRS}" )
else()
    message(SEND_ERROR "FreeType package not found.")
    message(STATUS "FreeType_DIR = $ENV{FreeType_DIR}")
endif()

# Check for zlib: required by Lomse
find_package(ZLIB REQUIRED)
if( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIR} )
    link_libraries( ${ZLIB_LIBRARY} )
    message(STATUS "zlib found: libraries= ${ZLIB_LIBRARY}" )
    message(STATUS "zlib found: include= ${ZLIB_INCLUDE_DIRS}" )
else()
    message(SEND_ERROR "zlib package not found.")
endif()

# Check for libpng: required by Lomse. As png requires zlib, png should be looked for after zlib
if(WIN32)
    #Default CMake module does not work for me. So for now use this bad hack.
    include(FindPNG_WIN)
else()
    find_package(PNG REQUIRED)
endif(WIN32)
if( PNG_FOUND )
    include_directories( ${PNG_INCLUDE_DIRS} )
    link_libraries( ${PNG_LIBRARIES} )
    message(STATUS "libpng found: libraries= ${PNG_LIBRARIES}" )
    message(STATUS "libpng found: include= ${PNG_INCLUDE_DIRS}" )
    message(STATUS "libpng found: definitions= ${PNG_DEFINITIONS}" )
    message(STATUS "libpng found: version string= ${PNG_VERSION_STRING}" )
    string(REPLACE "." ";" PNG_VERSION_LIST ${PNG_VERSION_STRING})
    list(GET PNG_VERSION_LIST 0 PNG_VERSION_MAJOR)
    list(GET PNG_VERSION_LIST 1 PNG_VERSION_MINOR)
    if(${PNG_VERSION_MINOR} STREQUAL "6")
        set(PNG_DEPENDENCY "libpng16-16")
    elseif(${PNG_VERSION_MINOR} STREQUAL "2")
        set(PNG_DEPENDENCY "libpng12-0")
    else()
        set(PNG_DEPENDENCY "libpng")
    endif()
    message(STATUS "libpng found: major=${PNG_VERSION_MAJOR}, minor=${PNG_VERSION_MINOR}" )
    message(STATUS "libpng found: dependency to add=${PNG_DEPENDENCY}" )
else()
    message(SEND_ERROR "libpng package not found.")
endif(PNG_FOUND)


# Check for fontconfig: required by Lomse
if (UNIX)   #includes APPLE
    find_path(FONTCONFIG_INCLUDE_DIR fontconfig/fontconfig.h)
    find_library(FONTCONFIG_LIBRARIES NAMES fontconfig libfontconfig)
    if (("${FONTCONFIG_INCLUDE_DIR}" STREQUAL "FONTCONFIG_INCLUDE_DIR-NOTFOUND")
        OR ("${FONTCONFIG_LIBRARIES}" STREQUAL "FONTCONFIG_LIBRARIES-NOTFOUND")
       )
        set(FONTCONFIG_FOUND FALSE)
    else()
        set(FONTCONFIG_FOUND TRUE)
    endif()

    if(FONTCONFIG_FOUND )
        include_directories( ${FONTCONFIG_INCLUDE_DIR}/fontconfig )
        link_libraries( ${FONTCONFIG_LIBRARIES} )
        message(STATUS "fontconfig found: libraries= ${FONTCONFIG_LIBRARIES}" )
        message(STATUS "fontconfig found: include= ${FONTCONFIG_INCLUDE_DIR}/fontconfig" )
    else()
        message(STATUS "fontconfig libraries= ${FONTCONFIG_LIBRARIES}" )
        message(STATUS "fontconfig include= ${FONTCONFIG_INCLUDE_DIR}" )
        message(FATAL_ERROR "fontconfig package not found.")
    endif()
endif()


#======= End of dependencies checking ========================================


#-----------------------------------------------------------------------------
# Decide installation folders, as some values need to be defined for configuring
# file lenmus_config.h before building.
#
# By default, the installation prefix is "/usr/local"
# You can change the install location by running cmake like this:
#   cmake -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# Remember:
#  LENMUS_ROOT_DIR:
#       path to source tree, to copy files from there: i.e. $HOME/projects/lenmus
#  ROOT_INSTALL:
#       path to install binaries: i.e.:
#       '/usr/local'
#       'C:\Program Files\lenmus'
#  SHARED_DIR:
#       path to install shared files i.e.:
#           '/usr/local/share/lenmus/5.3/'
#           'C:\Program Files\lenmus'
#-----------------------------------------------------------------------------

# set installation paths
if(WIN32)
    #For CPack using NSIS, the DESTINATION path must be relative because installation path
    #is always specified at the moment when user executes the installer. Nevertheless
    #current NSIS installer does not allow the user to select instalation folder and
    #it is always installed in "C:/Program Files/lenmus"
    set(BIN_DIR bin)
    set(SAMPLES_DIR .)
    set(LOCALE_DIR .)
    set(LOCALE_LICENSES_DIR locale)
    set(RES_DIR res)
    set(TEMPLATES_DIR .)
    set(XRC_DIR .)
    set(DOCS_DIR .)
elseif(APPLE)
    set(BIN_DIR "${ROOT_INSTALL}/Contents/MacOS")
    set(SAMPLES_DIR $ENV{HOME}/${CMAKE_PROJECT_NAME} )
    set(LOCALE_DIR ${SHARED_DIR})
    set(LOCALE_LICENSES_DIR ${SHARED_DIR})
    set(RES_DIR "${SHARED_DIR}/res")
    set(TEMPLATES_DIR ${SHARED_DIR})
    set(XRC_DIR ${SHARED_DIR})
    set(DOCS_DIR ${SHARED_DIR})
else()
    set(BIN_DIR "${ROOT_INSTALL}/bin")
    set(SAMPLES_DIR $ENV{HOME}/${CMAKE_PROJECT_NAME} )
    set(LOCALE_DIR ${SHARED_DIR})
    set(LOCALE_LICENSES_DIR ${SHARED_DIR})
    set(RES_DIR "${SHARED_DIR}/res")
    set(TEMPLATES_DIR ${SHARED_DIR})
    set(XRC_DIR ${SHARED_DIR})
    set(DOCS_DIR ${SHARED_DIR})
endif()

message("Installation paths:")
message(STATUS "ROOT_INSTALL = ${ROOT_INSTALL}")
message(STATUS "BIN_DIR = ${BIN_DIR}")
message(STATUS "SHARED_DIR = ${SHARED_DIR}")
message(STATUS "SAMPLES_DIR = ${SAMPLES_DIR}" )
message(STATUS "LOCALE_DIR = ${LOCALE_DIR}")
message(STATUS "LOCALE_LICENSES_DIR = ${LOCALE_LICENSES_DIR}")
message(STATUS "RES_DIR = ${RES_DIR}")
message(STATUS "TEMPLATES_DIR = ${TEMPLATES_DIR}")
message(STATUS "XRC_DIR = ${XRC_DIR}")
message(STATUS "DOCS_DIR = ${DOCS_DIR}")


# Variable LENMUS_SOUNDFONT_PATH is needed for lenmus_config.h file and is
# used at run time. Thus for building packages for distribution, this
# variable must point to a valid path in the target distro, that is to the 
# paths in which the distro installs FluidR3_GM.sf2 file. But if
# the package is going to include this file then the variable 
# LENMUS_SOUNDFONT_PATH must point to the place at which the soundfont
# will be installed.

#To build Windows installers force to install Bravura and Soundfont
if (WIN32)
    set(INSTALL_SOUNDFONT 1)
    set(INSTALL_BRAVURA_FONT 1)
endif()

if(INSTALL_SOUNDFONT)
    set(LENMUS_SOUNDFONT_PATH "")   #to force to make it relative to <SHARED_DIR>
    if (NOT ("${FLUIDR3_PATH}" STREQUAL "${LENMUS_ROOT_DIR}/res/sounds"))
        install(FILES ${FLUIDR3_PATH}/FluidR3_GM.sf2
                DESTINATION ${SHARED_DIR}/res/sounds )
#       file(COPY ${FLUIDR3_PATH}/FluidR3_GM.sf2
#             DESTINATION ${SHARED_DIR}/res/sounds)
    endif()
endif()

#This is probably not the best way to handle this for Mac, but... :
if(APPLE)
    set(LENMUS_SOUNDFONT_PATH "")   #to force to make it relative to <SHARED_DIR>
endif()


message(STATUS "LENMUS_SOUNDFONT_PATH=${LENMUS_SOUNDFONT_PATH}")

if(INSTALL_BRAVURA_FONT AND
   NOT ("${BRAVURA_FONT_PATH}" STREQUAL "${LENMUS_ROOT_DIR}/res/fonts"))
        message(STATUS "BRAVURA_FONT_PATH=${BRAVURA_FONT_PATH}")
        #file(COPY ${BRAVURA_FONT_PATH}/Bravura.otf
        #     DESTINATION ${SHARED_DIR}/res/fonts)
        install(FILES ${BRAVURA_FONT_PATH}/Bravura.otf
                DESTINATION ${SHARED_DIR}/res/fonts )
endif()



#process template header lenmus_config.h file to replace CMake settings
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/lenmus_config.h.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lenmus_config.h"
)
message("lenmus_config.h configuration:" )
message(STATUS "LENMUS_INSTALL_ROOT=${LENMUS_INSTALL_ROOT}")
message(STATUS "LENMUS_TEST_SCORES_PATH=${LENMUS_TEST_SCORES_PATH}")
message(STATUS "LENMUS_SOUNDFONT_PATH=${LENMUS_SOUNDFONT_PATH}")


#include binary dir so that lenmus_config.h can be accesed during build
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


if (LENMUS_USE_LOMSE_SOURCES)
    #define path were fonts will be found by lomse when running. The 
    #value defined here *is not used* because LenMus program will force
    #the lomse library to use lenmus fonts path, but I have to put something!
    set( FONTS_PATH   "${SHARED_DIR}/res/fonts" )

    # include Lomse build options and create the include file "lomse_config.h"
    message("Lomse configuration")
    include( ${CMAKE_CURRENT_SOURCE_DIR}/lomse-build-options.cmake )
    set_target_properties(build-version PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()


# create and include the man page
if (UNIX)   #includes APPLE
    include( ${LENMUS_ROOT_DIR}/manpage.cmake )
endif()


#define a header file to pass CMake settings to source code
configure_file(
    "${LENMUS_ROOT_DIR}/lenmus_config.h.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lenmus_config.h"
)


#////////////////////////////////////////////////////////////////////////
# Target: LenMus program
#////////////////////////////////////////////////////////////////////////

set (LENMUS_APP  lenmus)

# set name of lenmus executable
if( WIN32 )
    set( CMAKE_EXECUTABLE_SUFFIX ".exe" )
elseif( UNIX )   #includes APPLE
    set( CMAKE_EXECUTABLE_SUFFIX "" )
endif()



# source files to compile

set(APP_FILES
    ${SRC_DIR}/app/lenmus_about_dialog.cpp
    ${SRC_DIR}/app/lenmus_app.cpp
    ${SRC_DIR}/app/lenmus_art_provider.cpp
    ${SRC_DIR}/app/lenmus_book_reader.cpp
    ${SRC_DIR}/app/lenmus_canvas.cpp
    ${SRC_DIR}/app/lenmus_content_box.cpp
    ${SRC_DIR}/app/lenmus_content_box_ctrol.cpp
    ${SRC_DIR}/app/lenmus_document_canvas.cpp
    ${SRC_DIR}/app/lenmus_document_frame.cpp
    ${SRC_DIR}/app/lenmus_events.cpp
    ${SRC_DIR}/app/lenmus_main_frame.cpp
    ${SRC_DIR}/app/lenmus_preferences.cpp
    ${SRC_DIR}/app/lenmus_splash_frame.cpp
    ${SRC_DIR}/app/lenmus_string.cpp
    ${SRC_DIR}/app/lenmus_welcome_window.cpp
    ${SRC_DIR}/app/lenmus_zip_reader.cpp
)

set(AUXMUSIC_FILES
    ${SRC_DIR}/auxmusic/lenmus_cadence.cpp
    ${SRC_DIR}/auxmusic/lenmus_chord.cpp
    ${SRC_DIR}/auxmusic/lenmus_composer.cpp
    ${SRC_DIR}/auxmusic/lenmus_interval.cpp
    ${SRC_DIR}/auxmusic/lenmus_scale.cpp
    ${SRC_DIR}/auxmusic/lenmus_utilities.cpp
)

set(DIALOGS_FILES
    ${SRC_DIR}/dialogs/lenmus_dlg_choose_lang.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_debug.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_levels.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_metronome.cpp
)

set(EXERCISES_FILES
    ${SRC_DIR}/exercises/auxctrols/lenmus_counters_ctrol.cpp
    ${SRC_DIR}/exercises/auxctrols/lenmus_problem_displayer.cpp

    ${SRC_DIR}/exercises/constrains/lenmus_cadences_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_chords_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_ear_intervals_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_key_sign_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_music_reading_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_notes_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_scales_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_scores_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_theo_intervals_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_tonality_constrains.cpp

    ${SRC_DIR}/exercises/ctrols/lenmus_dyncontrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_ear_compare_intv_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_ear_intervals_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_exercise_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_generators.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_idfy_cadences_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_idfy_chords_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_idfy_notes_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_idfy_scales_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_idfy_tonality_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_theo_intervals_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_theo_key_sign_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_theo_music_reading_ctrol.cpp

    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_cfg_cadences.cpp
    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_cfg_ear_intervals.cpp
    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_cfg_idfy_chords.cpp
    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_cfg_idfy_notes.cpp
    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_cfg_idfy_scales.cpp
    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_cfg_idfy_tonality.cpp
    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_cfg_score_reading.cpp
    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_cfg_theo_intervals.cpp
    ${SRC_DIR}/exercises/dialogs/lenmus_dlg_exercise_mode.cpp

    ${SRC_DIR}/exercises/params/lenmus_exercise_params.cpp
)

set(GLOBALS_FILES
    ${SRC_DIR}/globals/lenmus_colors.cpp
    ${SRC_DIR}/globals/lenmus_injectors.cpp
    ${SRC_DIR}/globals/lenmus_paths.cpp
)

set(OPTIONS_FILES
    ${SRC_DIR}/options/lenmus_lang_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_languages.cpp
    ${SRC_DIR}/options/lenmus_options_dlg.cpp
    ${SRC_DIR}/options/lenmus_other_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_playback_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_sound_opt_panel.cpp
)

set(SOUND_FILES
    ${SRC_DIR}/sound/lenmus_metronome.cpp
    ${SRC_DIR}/sound/lenmus_midi_server.cpp
    ${SRC_DIR}/sound/lenmus_wave_player.cpp
)

set(XML_PARSER_FILES
    ${SRC_DIR}/xml_parser/lenmus_xml_parser.cpp
)

if (${LENMUS_ENABLE_UNIT_TESTS})
    set(TEST_FILES
        ${SRC_DIR}/tests/lenmus_test_app.cpp
        ${SRC_DIR}/tests/lenmus_test_cadence.cpp
        ${SRC_DIR}/tests/lenmus_test_composer.cpp
        ${SRC_DIR}/tests/lenmus_test_mainframe.cpp
        ${SRC_DIR}/tests/lenmus_test_tokenizer.cpp
        ${SRC_DIR}/tests/lenmus_test_runner.cpp
    )
endif()

set(LENMUS_PACKAGESS_FILES
    ${PACKAGES_DIR}/wxMidi/src/wxMidi.cpp
    ${PACKAGES_DIR}/wxMidi/src/wxMidiDatabase.cpp
    ${PACKAGES_DIR}/wxSQLite3/src/wxsqlite3.cpp
)

set(ALL_LENMUS_SOURCES 
    ${APP_FILES} ${AUXMUSIC_FILES} ${DIALOGS_FILES} ${EXERCISES_FILES}
    ${GLOBALS_FILES} ${OPTIONS_FILES} ${SOUND_FILES} ${XML_PARSER_FILES}
    ${TEST_FILES} ${LENMUS_PACKAGESS_FILES}
)

# Add folders for Visual Studio and other IDEs supporting it
source_group( "app"             FILES ${APP_FILES} )
source_group( "auxmusic"        FILES ${AUXMUSIC_FILES} )
source_group( "dialogs"         FILES ${DIALOGS_FILES} )
source_group( "exercises"       FILES ${EXERCISES_FILES} )
source_group( "globals"         FILES ${GLOBALS_FILES} )
source_group( "options"         FILES ${OPTIONS_FILES} )
source_group( "sound"           FILES ${SOUND_FILES} )
source_group( "xml_parser"      FILES ${XML_PARSER_FILES} )
source_group( "packages"        FILES ${LENMUS_PACKAGESS_FILES} )
if (${LENMUS_ENABLE_UNIT_TESTS})
    source_group( "tests"           FILES ${TEST_FILES} )
endif()



#Add Lomse sources
if (LENMUS_USE_LOMSE_SOURCES)
    include( ${LOMSE_ROOT_DIR}/add-sources.cmake )
    set (LOMSE_GROUP_FOLDER "lomse/" )
    include( ${LOMSE_ROOT_DIR}/add-group-folders.cmake )
    set(ALL_SOURCES ${ALL_LENMUS_SOURCES} ${ALL_LOMSE_SOURCES} )
else()
    set(ALL_SOURCES ${ALL_LENMUS_SOURCES} )
endif()



# Define target
if (UNIX AND NOT APPLE)
    add_executable( ${LENMUS_APP} ${ALL_SOURCES} )
elseif (APPLE)
    add_executable( ${LENMUS_APP} MACOSX_BUNDLE ${ALL_SOURCES} )
else()
    add_executable( ${LENMUS_APP} WIN32 ${ALL_SOURCES} )
endif()
add_dependencies(${LENMUS_APP} lenmus-version)


# Add resources
#if(WIN32)
    # Windows users expect to see an icon associated with your program.
    # So, include a RC file for windows with the icon
#    list(APPEND ALL_SOURCES ${RESOURCES_DIR}/other/lenmus.rc)
#    source_group("resources" FILES ${RESOURCES_DIR}/other/lenmus.rc )
#elseif(APPLE)
    # Add an icon for the apple .app file
    #list(APPEND ALL_SOURCES ${RESOURCES_DIR}/other/osx/carbon/wxmac.icns)
#endif()


# Compiler options and definitions
if (WIN32)
    # remove noisy warning 4996: function or variable may be unsafe (i.e. strcat)
    set_target_properties(${LENMUS_APP} PROPERTIES COMPILE_FLAGS "/wd4996")

    # force to use static Runtime library
    set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()

elseif(UNIX AND NOT APPLE)
    # "Print all warnings", macros for GCC & __UNIX__
    add_definitions( -Wall -DGCC -D__UNIX__ )

elseif(APPLE)
    # "Print all warnings", macros for __UNIX__
    add_definitions( -Wall -D__UNIX__ )

endif()


# libraries to link
target_link_libraries ( ${LENMUS_APP} 
            ${wxWidgets_LIBRARIES} ${PortMidi_LIBRARIES} ${PortTime_LIBRARIES}
            ${LOMSE_LIBRARIES} ${SQLite3_LIBRARIES} ${UNITTEST_LIBRARIES} 
            ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} 
            ${ZLIB_LIBRARIES} ${FONTCONFIG_LIBRARY}
)


# Target properties for macOS bundle
if(APPLE)
    set_target_properties(${LENMUS_APP} PROPERTIES
        MACOSX_BUNDLE_EXECUTABLE_NAME ${LENMUS_APP}
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.lenmus.lenmus"
        MACOSX_BUNDLE_BUNDLE_NAME "LenMus Phonascus"
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${LENMUS_VERSION}
        MACOSX_BUNDLE_LONG_VERSION_STRING ${LENMUS_VERSION_LONG}
        MACOSX_BUNDLE_COPYRIGHT "© 2002-2020 LenMus project. License GNU GPL v.3 or greater"
        MACOSX_BUNDLE_INFO_PLIST "${LENMUS_ROOT_DIR}/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_VERSION 1
    )

        # ADD_LANG
    set(BUNDLE_LOCALIZATIONS
"       <string>de</string>
        <string>el</string>
        <string>en</string>
        <string>es</string>
        <string>eu</string>
        <string>fr</string>
        <string>gl_ES</string>
        <string>it</string>
        <string>nl</string>
        <string>tr</string>
        <string>zh_CN</string>
")
endif()


# Display compiler and linker settings
message("Configuration for target ${LENMUS_APP}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
else()
    message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
endif()
message(STATUS "Linker flags: ${CMAKE_EXE_LINKER_FLAGS}" )

endif(BUILD_PROGRAM)


#///////////////////////////////////////////////////////////////////////////////
# Program installation
#///////////////////////////////////////////////////////////////////////////////


# Package: lenmus (component: main)
# ------------------------------------------------------------------------------

# bin. LenMus program and other binary and related files (BIN_DIR)
install(TARGETS ${LENMUS_APP}
    RUNTIME DESTINATION ${BIN_DIR} COMPONENT main
    BUNDLE DESTINATION . COMPONENT main
)

if(WIN32)
    # customization XML file for tile icons and icons
    install(FILES ${LENMUS_ROOT_DIR}/installer/msw/lenmus.visualelementsmanifest.xml DESTINATION ${BIN_DIR} )
    install(FILES ${LENMUS_ROOT_DIR}/res/icons/tile-150x150.png DESTINATION ${BIN_DIR}/icons )
    install(FILES ${LENMUS_ROOT_DIR}/res/icons/tile-70x70.png DESTINATION ${BIN_DIR}/icons )
endif(WIN32)

# xrc resources
install(DIRECTORY ${LENMUS_ROOT_DIR}/xrc  DESTINATION ${XRC_DIR}
        COMPONENT main
        FILES_MATCHING
            PATTERN "*.xrc"
            PATTERN "x_*" EXCLUDE
            PATTERN "z_*" EXCLUDE
)

# templates
#install(DIRECTORY ${LENMUS_ROOT_DIR}/templates
#        DESTINATION ${TEMPLATES_DIR}
#        COMPONENT main
#        FILES_MATCHING PATTERN "*.lms"
#)

# text files in root folder (license, etc.) shared among all users (SHARED_DIR)
install(FILES
            AUTHORS
            CHANGELOG.md
            INSTALL
            LICENSE
            NEWS
            README.md
            THANKS 
        DESTINATION ${DOCS_DIR}
        COMPONENT main
)
install(FILES ${LENMUS_ROOT_DIR}/docs/html/LICENSE_GNU_GPL_1.3.txt
        DESTINATION ${DOCS_DIR} COMPONENT main )
install(FILES ${LENMUS_ROOT_DIR}/docs/html/LICENSE_GNU_FDL_1.3.txt
        DESTINATION ${DOCS_DIR} COMPONENT main )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_en.txt
        DESTINATION ${DOCS_DIR} COMPONENT main )

if (UNIX AND NOT APPLE)
    # Install desktop entry
    install(FILES ${LENMUS_ROOT_DIR}/res/desktop/org.lenmus.lenmus.desktop
            DESTINATION "/usr/share/applications"
            COMPONENT main )
    # Install icon
    install(FILES ${LENMUS_ROOT_DIR}/res/icons/lenmus.png
            DESTINATION "/usr/share/pixmaps"
            COMPONENT main )

    # Install AppStream upstream metadata
    install(FILES ${LENMUS_ROOT_DIR}/res/other/org.lenmus.lenmus.appdata.xml
            DESTINATION "/usr/share/metainfo"
            COMPONENT main )
endif(UNIX AND NOT APPLE)

if(INSTALL_SOUNDFONT)
    install(FILES ${LENMUS_ROOT_DIR}/res/sounds/FluidR3_GM.sf2
            DESTINATION ${RES_DIR}/sounds
            COMPONENT main )
endif()

if(INSTALL_BRAVURA_FONT)
    install(FILES ${BRAVURA_FONT_PATH}/Bravura.otf
            DESTINATION ${RES_DIR}/fonts
            COMPONENT main )
endif()



# Package: lenmus-i18n
# ------------------------------------------------------------------------------
# locale folder. License translations
    # ADD_LANG
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_de.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_el.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_es.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_eu.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_fr.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_gl_ES.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_it.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_nl.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_tr.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_zh_CN.txt
        DESTINATION ${LOCALE_LICENSES_DIR} COMPONENT i18n )

# locale folder. help files and language catalogs for all languages
install(DIRECTORY  ${LENMUS_ROOT_DIR}/locale
        DESTINATION ${LOCALE_DIR}
        COMPONENT i18n 
        FILES_MATCHING
            PATTERN "common/*.*"
            PATTERN "*.htm"
            #help
            PATTERN "help*" EXCLUDE
            PATTERN "images*" EXCLUDE
            #PATTERN "*.html"
            #PATTERN "*.js"
            #PATTERN "*.map"
            #PATTERN "*.inv"
            #PATTERN "*.png"
            #PATTERN "*.gif"
            #PATTERN "*.jpg"
            #PATTERN "*.css"
            #language catalogs
            PATTERN "*.mo"
            PATTERN "x_*" EXCLUDE
            PATTERN "z_*" EXCLUDE
            PATTERN "*~" EXCLUDE
            #exclude unsupported languages
                # ADD_LANG
            PATTERN "kk*" EXCLUDE
            PATTERN "ru*" EXCLUDE
)

#generate include files with translations for NSIS installer 
if(WIN32)
    #GUI translations
    set(NSIS_TRANSLATIONS ${CMAKE_BINARY_DIR}/mui_locale.nsh)
    file(REMOVE ${NSIS_TRANSLATIONS})
    file(APPEND ${NSIS_TRANSLATIONS} ";translations for installer GUI\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !addincludedir \"$")
    file(APPEND ${NSIS_TRANSLATIONS} "{LENMUS_NSIS_LOCALE}\"\n")
        # ADD_LANG
    file(APPEND ${NSIS_TRANSLATIONS} "  !include de.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include el.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include en.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include es.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include eu.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include fr.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include gl_ES.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include it.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include nl.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include tr.nsh\n")
    file(APPEND ${NSIS_TRANSLATIONS} "  !include zh_CN.nsh\n")
    
    message("translations for NSIS installer created: ${NSIS_TRANSLATIONS}")

    #Available languages
    set(NSIS_LANGUAGES ${CMAKE_BINARY_DIR}/mui_languages.nsh)
    file(REMOVE ${NSIS_LANGUAGES})
    file(APPEND ${NSIS_LANGUAGES} ";available languages for the installer\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE English    ;first language is the default language\n")
        # ADD_LANG
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE Basque\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE SimpChinese\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE Dutch\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE French\n")        
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE Galician\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE German\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE Greek\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE Italian\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE Spanish\n")
    file(APPEND ${NSIS_LANGUAGES} "  !insertmacro MUI_LANGUAGE Turkish\n")

    message("languages for NSIS installer created: ${NSIS_LANGUAGES}")
endif()


# Package: lenmus-ebooks
# ------------------------------------------------------------------------------

# locale folder. eBooks for all languages (English included)
install(DIRECTORY  ${LENMUS_ROOT_DIR}/locale/
        DESTINATION ${LOCALE_DIR}/locale
        COMPONENT ebooks
        FILES_MATCHING
            #books 
            PATTERN "*.lmb"
            #exclude unsupported languages
                # ADD_LANG
            PATTERN "x_*" EXCLUDE
            PATTERN "z_*" EXCLUDE
            PATTERN "kk*" EXCLUDE
            PATTERN "ru*" EXCLUDE
)


# Package: lenmus-common
# ------------------------------------------------------------------------------

# res folder. bitmaps, cursors, desktop, icons, sounds
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/bitmaps   DESTINATION ${RES_DIR}
        COMPONENT common
        FILES_MATCHING PATTERN "*.*" PATTERN "x_*" EXCLUDE )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/cursors   DESTINATION ${RES_DIR}
        COMPONENT common
        FILES_MATCHING PATTERN "*.png" PATTERN "x_*" EXCLUDE )
install(FILES ${LENMUS_ROOT_DIR}/res/desktop/org.lenmus.lenmus.desktop
        DESTINATION "${RES_DIR}/desktop"
        COMPONENT common )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/icons   DESTINATION ${RES_DIR}
        COMPONENT common
        FILES_MATCHING PATTERN "*.*" PATTERN "x_*" EXCLUDE )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/sounds   DESTINATION ${RES_DIR}
        COMPONENT common
        FILES_MATCHING PATTERN "*.*" 
                       PATTERN "*.sf2" EXCLUDE
                       PATTERN "x_*" EXCLUDE
                       PATTERN "z_*" EXCLUDE
        )

# samples 
#install(DIRECTORY ${LENMUS_ROOT_DIR}/scores/samples  DESTINATION "${SAMPLES_DIR}"
#        FILES_MATCHING PATTERN "*.lms"
#        PATTERN "test_set" EXCLUDE
#       COMPONENT common )
#)


# fix up the macOS bundle in-place and make it standalone
if (APPLE)
    # add Mac specific extension .app
    set(APPS "\${CMAKE_INSTALL_PREFIX}/${LENMUS_APP}.app")

    # Directories to look for dependencies
    set(DIRS ${CMAKE_BINARY_DIR})

    install(CODE "include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")")
endif()




#///////////////////////////////////////////////////////////////////////////////
# CPack section: installers generation
#///////////////////////////////////////////////////////////////////////////////

# define the components to package
if (BUILD_PKG_BOUNDLE)
    set(CPACK_MONOLITHIC_INSTALL 1)
    set(CPACK_DEB_COMPONENT_INSTALL 0)
else()
    set(CPACK_MONOLITHIC_INSTALL 0)
    set(CPACK_DEB_COMPONENT_INSTALL 1)
    if(BUILD_PKG_MAIN)
        set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} "main")
    endif()
    if(BUILD_PKG_COMMON)
        set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} "common")
    endif()
    if(BUILD_PKG_I18N)
        set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} "i18n")
    endif()
    if(BUILD_PKG_EBOOKS)
        set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} "ebooks")
    endif()
endif()
message("CPACK_COMPONENTS_ALL=${CPACK_COMPONENTS_ALL}")



# Common settings for all installers
set(CPACK_PACKAGE_VENDOR ${LENMUS_VENDOR_NAME})
set(CPACK_PACKAGE_CONTACT "s.cecilio@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION "A program for aural training and to learn music theory")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
"Lenmus Phonascus is a totally free program for studying music theory that
 allows you to focus on specific skills and exercises, on both theory and aural
 training.
 .
 The different activities can be customized to meet your needs. Phonascus
 allows you to work at your own pace, providing interactive feedback until
 mastery of each concept is achieved.
 .
 Phonascus aims to create, as a community, the best free program for 
 music students, to help them in their studies. Phonascus has no 
 commercial purposes.
")

if (APPLE)
    # For Apple generate a drang & drop package
    set(CPACK_GENERATOR "DragNDrop")

elseif(UNIX AND NOT APPLE)
    # For Linux generate one or more Debian packages
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cecilio Salmeron <s.cecilio@gmail.com>")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${LENMUS_VENDOR_SITE}")
    set(CPACK_STRIP_FILES ON)        #remove debug information, if any, from binaries

    #auxiliary variable describing current available translations
        # ADD_LANG
    set(LENMUS_AVAILABLE_LOCALE
" Currently available locales are:
    de (German)                   gl_ES(Galician)
    el (Greek)                    it (Italian)
    en (English)                  nl (Dutch)
    es (Spanish)                  tr (Turkish)
    eu (Basque)                   zh_CN (Simplified Chinese)
    fr (French)
")

    #boundle package
    set(CPACK_DEBIAN_PACKAGE_NAME "lenmus-all")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${LENMUS_PACKAGE_VERSION})
    set(CPACK_DEBIAN_PACKAGE_SECTION "education") 
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^x86_64$|^amd64$")  
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    else()
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
    endif() 
    string(CONCAT CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}"
            "_${LENMUS_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    if (LENMUS_USE_LOMSE_SOURCES)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
    else()
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "liblomse (>= 0.25), ")
    endif()
    string(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS
        "libsqlite3-0, libportmidi0, fluid-soundfont-gm, "
        "libfreetype6, ${PNG_DEPENDENCY}, libfluidsynth-dev, zlib1g (>= 1:1.2.1)")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
"A program for aural training and to learn music theory
 This package provides the program and all lenmus packages required to run
 the LenMus Phonascus program.
 .
 Lenmus Phonascus is a totally free program for studying music theory that
 allows you to focus on specific skills and exercises, on both theory and aural
 training.
 .
 The different activities can be customized to meet your needs. Phonascus
 allows you to work at your own pace, providing interactive feedback until
 mastery of each concept is achieved.
 .
 Phonascus aims to create, as a community, the best free program for 
 music students, to help them in their studies. Phonascus has no 
 commercial purposes.
")

    #main package
    set(CPACK_DEBIAN_MAIN_PACKAGE_SECTION "education")
    set(CPACK_DEBIAN_MAIN_PACKAGE_NAME "lenmus")
    set(CPACK_DEBIAN_MAIN_PACKAGE_ARCHITECTURE ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
    set(CPACK_DEBIAN_MAIN_PACKAGE_DEPENDS ${CPACK_DEBIAN_PACKAGE_DEPENDS})
    string(APPEND CPACK_DEBIAN_MAIN_PACKAGE_DEPENDS
        ", lenmus-common, lenmus-i18n, lenmus-ebooks")
    string(CONCAT CPACK_DEBIAN_MAIN_FILE_NAME
            "${CPACK_DEBIAN_MAIN_PACKAGE_NAME}"
            "_${LENMUS_PACKAGE_VERSION}"
            "_${CPACK_DEBIAN_MAIN_PACKAGE_ARCHITECTURE}.deb"
          )
    set(CPACK_COMPONENT_MAIN_DESCRIPTION
"LenMus Phonascus (aural training and music theory) binaries and related files
 This package provides the program and related files for the LenMus Phonascus
 program, a program for aural training and to learn music theory.
 .
 Lenmus Phonascus is a totally free program for studying music theory that
 allows you to focus on specific skills and exercises, on both theory and aural
 training. The  different activities can be customized to meet your needs. 
 Phonascus allows you to work at your own pace, providing interactive feedback
 until mastery of each concept is achieved.
")

    #ebooks package
    set(CPACK_DEBIAN_EBOOKS_PACKAGE_SECTION "education")
    set(CPACK_DEBIAN_EBOOKS_PACKAGE_NAME "lenmus-ebooks")
    set(CPACK_DEBIAN_EBOOKS_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_EBOOKS_PACKAGE_DEPENDS "")
    string(CONCAT CPACK_DEBIAN_EBOOKS_FILE_NAME
            "${CPACK_DEBIAN_EBOOKS_PACKAGE_NAME}"
            "_${LENMUS_PACKAGE_VERSION}"
            "_${CPACK_DEBIAN_EBOOKS_PACKAGE_ARCHITECTURE}.deb"
          )
    set(CPACK_COMPONENT_EBOOKS_DESCRIPTION
"LenMus Phonascus eBooks (all current translations)
 This package provides the eBooks for the LenMus Phonascus program for aural
 training and to learn music theory.
 .
${LENMUS_AVAILABLE_LOCALE}"
)

    #common package
    set(CPACK_DEBIAN_COMMON_PACKAGE_SECTION "education")
    set(CPACK_DEBIAN_COMMON_PACKAGE_NAME "lenmus-common")
    set(CPACK_DEBIAN_COMMON_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_COMMON_PACKAGE_DEPENDS "")
    string(CONCAT CPACK_DEBIAN_COMMON_FILE_NAME
            "${CPACK_DEBIAN_COMMON_PACKAGE_NAME}"
            "_${LENMUS_PACKAGE_VERSION}"
            "_${CPACK_DEBIAN_COMMON_PACKAGE_ARCHITECTURE}.deb"
          )
    set(CPACK_COMPONENT_COMMON_DESCRIPTION
"LenMus Phonascus (aural training and music theory) shared files
 This package provides the support files needed by the LenMus Phonascus program.
 It includes bitmaps, cursors, desktop, icons and sounds, shared by all users.
")

    #i18n package
    set(CPACK_DEBIAN_I18N_PACKAGE_SECTION "education")
    set(CPACK_DEBIAN_I18N_PACKAGE_NAME "lenmus-i18n")
    set(CPACK_DEBIAN_I18N_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_I18N_PACKAGE_DEPENDS "")
    string(CONCAT CPACK_DEBIAN_I18N_FILE_NAME
            "${CPACK_DEBIAN_I18N_PACKAGE_NAME}"
            "_${LENMUS_PACKAGE_VERSION}"
            "_${CPACK_DEBIAN_I18N_PACKAGE_ARCHITECTURE}.deb"
          )
    set(CPACK_COMPONENT_I18N_DESCRIPTION
"Message catalogues and translated files (but not eBooks) for all translations
${LENMUS_AVAILABLE_LOCALE}"
)

elseif(WIN32)
    # For Windows generate a NSIS installer
    # AWARE: Only variables starting with "CPACK_" can be used to customize the installer.
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_STRIP_FILES ON)        #remove debug information, if any, from binaries
    set(CPACK_PACKAGE_NAME "lenmus")
    set(CPACK_PACKAGE_VERSION ${LENMUS_PACKAGE_VERSION})
    
    #name of the package file to generate, not including the extension
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${SYSTEM_ARCHITECTURE}-setup")
    
    #some paths used while building the installer
    set(CPACK_LENMUS_ROOT "${LENMUS_ROOT_DIR}")
    set(CPACK_LENMUS_BUILD_INSTALLER_PATH "${CMAKE_BINARY_DIR}")
    set(CPACK_LENMUS_INSTALL_PATH "${SHARED_DIR}")

    message("Package name: ${CPACK_PACKAGE_FILE_NAME}" )
    message("Installation default path: ${CPACK_LENMUS_INSTALL_PATH}" )

endif()

# Must be after the last CPACK macro
include(CPack)

message(" ")
message("End of CMakeLists.txt ==================================================")
message(" ")
message("Generation starts:")
