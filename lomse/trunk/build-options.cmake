#-------------------------------------------------------------------------------------
# This is part of CMake configuration file for building makefiles and installfiles
# for the Lomse library
#-------------------------------------------------------------------------------------
# This module is for defining build options and creating the
# include file "lomse_config.h"
#
# Various options that the user can select (var, msg, default value).
# Values can be changed directly in the CMake GUI or through
# the command line by prefixing a variable's name with '-D':
# i.e.:    cmake -DLOMSE_DEBUG=ON
#
#-------------------------------------------------------------------------------------

# build options
if(WIN32)
	option(LOMSE_BUILD_STATIC_LIB "Build the static library" ON)
	option(LOMSE_BUILD_SHARED_LIB "Build the shared library" OFF)
else()
	option(LOMSE_BUILD_STATIC_LIB "Build static library" OFF)
	option(LOMSE_BUILD_SHARED_LIB "Build shared library" ON)
endif()

option(LOMSE_BUILD_MONOLITHIC
    "Build a monolithic library with no dependencies" 
    OFF)
option(LOMSE_BUILD_TESTS
    "Build the unit tests runner program 'testlib'"
    ON)
option(LOMSE_BUILD_EXAMPLE
    "Build the tutorial_1 program"
    OFF)

# Debug options (ON / OFF values):
option(LOMSE_DEBUG
    "Debug build, with debug symbols"
    OFF)
option(LOMSE_ENABLE_DEBUG_LOGS
    "Enable debug logs. Doesn't require debug build"
    OFF)

# Bravura music font required to render scores
option(LOMSE_DOWNLOAD_BRAVURA_FONT
    "Download Bravura.otf font if not present in source tree"
	ON)
option(LOMSE_INSTALL_BRAVURA_FONT
    "Include Bravura music font in the package"
	ON)

# Options to reduce dependencies from other libraries
option(LOMSE_ENABLE_PNG
    "Enable png format (requires pnglib and zlib)"
    ON)
option(LOMSE_ENABLE_COMPRESSION
    "Enable compressed formats (requires zlib)"
    ON)

# Other options
option(LOMSE_COMPATIBILITY_LDP_1_5
    "Enable compatibility for LDP v1.5"
    ON)

#----- end of options definition -----


#santity checks
if (LOMSE_ENABLE_PNG)
	if (NOT LOMSE_ENABLE_COMPRESSION)
        message(STATUS "**WARNING**: Enabling PNG requires enabling compression. LOMSE_ENABLE_COMPRESSION set to ON" )
    	set(LOMSE_ENABLE_COMPRESSION ON)
	endif()
endif()      

#libraries to build
#if (WIN32)
#    set(LOMSE_BUILD_STATIC_LIB ON)
#    set(LOMSE_BUILD_SHARED_LIB OFF)
#    message(STATUS "In Windows force to build the static library")
#else()
#    set(LOMSE_BUILD_STATIC_LIB OFF)
#    set(LOMSE_BUILD_SHARED_LIB ON)
#    message(STATUS "In Unix force to build the dynamic library")
#endif()

#if (WIN32)
#    if (LOMSE_BUILD_SHARED_LIB AND (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
#        message(FATAL_ERROR "Shared C++ libraries (C++ DLL) are not supported by MSVC")
#    endif()
#endif()

# display options values for building
message(STATUS "Build monolithic library = ${LOMSE_BUILD_MONOLITHIC}")
message(STATUS "Build the static library = ${LOMSE_BUILD_STATIC_LIB}")
message(STATUS "Build the shared library = ${LOMSE_BUILD_SHARED_LIB}")
message(STATUS "Build testlib program = ${LOMSE_BUILD_TESTS}")
message(STATUS "Build tutorial_1 program = ${LOMSE_BUILD_EXAMPLE}")
message(STATUS "Create Debug build = ${LOMSE_DEBUG}")
message(STATUS "Enable debug logs = ${LOMSE_ENABLE_DEBUG_LOGS}")
message(STATUS "Download Bravura font = ${LOMSE_DOWNLOAD_BRAVURA_FONT}")
message(STATUS "Install Bravura font = ${LOMSE_INSTALL_BRAVURA_FONT}")
message(STATUS "Enable png format = ${LOMSE_ENABLE_PNG}")
message(STATUS "Enable compressed formats = ${LOMSE_ENABLE_COMPRESSION}")
message(STATUS "Compatibility for LDP v1.5 = ${LOMSE_COMPATIBILITY_LDP_1_5}")



# set up configuration variables for lomse_config.h
#------------------------------------------------------

# build type (this variables affects lomse_config.h and are used
# in lomse_build_options.h. But there are two problems:
# 1. Both builds (static and shared) can be built in the same cmake command.
#    And in this case there is (currently) a single lomse_config.h file
#    common to both builds.
# 2. When defining LOMSE_USE_DLL=1 there are compilation errors in Windows.
#    So, until further investigation, I have commented out this.
# Another issue to consider is that using makes it impossible to build both
# library versions in the same build.
#if (LOMSE_BUILD_STATIC_LIB)
    set( LOMSE_USE_DLL "0")
    set( LOMSE_CREATE_DLL "0")
#else()
#    set( LOMSE_USE_DLL "1")
#    set( LOMSE_CREATE_DLL "1")
#endif()


# set up lomse library version information
include( ${LOMSE_ROOT_DIR}/build-version.cmake )

add_custom_target (build-version ALL
  COMMAND ${CMAKE_COMMAND} -D LOMSE_ROOT_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/build-version.cmake
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "setting Lomse version information ...")

# identify platform and compiler
if(WIN32 OR CYGWIN)
    # for Windows operating system or Windows when using the CygWin version of cmake
    set( LOMSE_PLATFORM_WIN32  "1")
    set( LOMSE_PLATFORM_UNIX   "0")
    set( LOMSE_PLATFORM_APPLE  "0")
elseif(APPLE)
    # for MacOS X or iOS, watchOS, tvOS (since 3.10.3)
    set( LOMSE_PLATFORM_WIN32  "0")
    set( LOMSE_PLATFORM_UNIX   "0")
    set( LOMSE_PLATFORM_APPLE  "1")
elseif(UNIX AND NOT APPLE AND NOT CYGWIN)
    # for Linux, BSD, Solaris, Minix
    set( LOMSE_PLATFORM_WIN32  "0")
    set( LOMSE_PLATFORM_UNIX   "1")
    set( LOMSE_PLATFORM_APPLE  "0")
endif()

# compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set( LOMSE_COMPILER_MSVC  "1")
else()
    set( LOMSE_COMPILER_MSVC  "0")
endif()


# other variables needed by lomse_config.h

# paths for tests
set( TESTLIB_SCORES_PATH     "\"${LOMSE_ROOT_DIR}/test-scores/\"" )
set( TESTLIB_FONTS_PATH      "\"${LOMSE_ROOT_DIR}/fonts/\"" )

# path to fonts (will be hardcoded in lomse library, so *MUST* be the
# path in which Lomse standard fonts will be installed)
set( LOMSE_FONTS_PATH   "\"${FONTS_PATH}/\"" )



# Build environtment checks and settings
#------------------------------------------------------------------------------

# check that compiler supports namespace sdt
include(TestForSTDNamespace)
if(CMAKE_NO_STD_NAMESPACE)
  message(FATAL_ERROR "The compiler doesn't support namespace std.")
endif()

# determine system arquitecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SYSTEM_ARCHITECTURE "x64")
else()
    set(SYSTEM_ARCHITECTURE "x86")
endif()
message(STATUS "SYSTEM_ARCHITECTURE = ${SYSTEM_ARCHITECTURE}" )


# force to use c++11
if (CMAKE_VERSION VERSION_LESS "3.1")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else ()
    set(CMAKE_CXX_STANDARD 11)				#require c+11 or greater
    set(CMAKE_CXX_STANDARD_REQUIRED ON) 	#prevent fallback to any previous standard
endif ()


#check that the compiler supports c++11 and std::regex
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # require at least gcc 4.9
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        message(FATAL_ERROR "GCC version must be at least 4.9!")
    endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    # require at least clang 3.4
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4)
        message(FATAL_ERROR "Clang version must be at least 3.4!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # require at least msvc 15 (Visual Studio 2008 version 9.0)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15)
        message(FATAL_ERROR "MSVC version must be at least 15!")
    endif()
else()
    message(WARNING "** Warning **: You are using an untested compiler! Lomse has not been tested with ${CMAKE_CXX_COMPILER_ID}.")
endif()
message(STATUS "** Compiler to use: ${CMAKE_CXX_COMPILER_ID} v.${CMAKE_CXX_COMPILER_VERSION}")



# names for libraries and execs.
#-------------------------------------------------------------------------------------
if( WIN32 )
    set( CMAKE_STATIC_LIBRARY_PREFIX "" )
    set( CMAKE_STATIC_LIBRARY_SUFFIX ".lib" )
    set( CMAKE_SHARED_LIBRARY_PREFIX "" )
    set( CMAKE_SHARED_LIBRARY_SUFFIX ".dll" )
    set( CMAKE_EXECUTABLE_SUFFIX ".exe" )
elseif( UNIX )
    set( CMAKE_STATIC_LIBRARY_PREFIX "lib" )
    set( CMAKE_STATIC_LIBRARY_SUFFIX ".a" )
    set( CMAKE_SHARED_LIBRARY_PREFIX "lib" )
    set( CMAKE_SHARED_LIBRARY_SUFFIX ".so" )
    set( CMAKE_EXECUTABLE_SUFFIX "" )
endif()


#----------------------------------------------------------------------------
# Determine installation folders and create the include file "lomse_config.h"
#
# values for configuration macros in "lomse_config.h" could depend on 
# installation folders. Therefore, before configuring it is necessary to set
# installation folders. By default, CMake uses the following prefix:
#   - Linux: "/usr/local"
#   - Windows: "C:\Program Files\lomse\"
#
# You can change the install location by running cmake like this:
#   cmake -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#----------------------------------------------------------------------------

message(STATUS "CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX} )

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
set(LOMSE_LIBDIR        ${CMAKE_INSTALL_PREFIX}/lib)
set(LOMSE_INCLUDEDIR    ${CMAKE_INSTALL_PREFIX}/include/lomse )



#----------------------------------------------------------------------------
# Dependencies
# Check for needed libraries and set all related includes, flags, etc. 
#----------------------------------------------------------------------------

#dependencies for building and for pkg-config file (intitially empty)
set(LOMSE_REQUIRES "")
set(LOMSE_DEBIAN_DEPS "")   #Build deps: Debian "Build-Depends:" string
set(LOMSE_BUILD_DEPS "")    #Build deps: Libraries to link

# include directories to be installed
set( INCLUDES_LOMSE  ${LOMSE_ROOT_DIR}/include )
set( INCLUDES_AGG  ${LOMSE_ROOT_DIR}/src/agg/include )
set( INCLUDES_UTFCPP  ${LOMSE_ROOT_DIR}/packages/utfcpp )
set( INCLUDES_MINIZIP  ${LOMSE_ROOT_DIR}/packages/minizip )
set( INCLUDES_PUGIXML  ${LOMSE_ROOT_DIR}/packages )
include_directories(
    ${INCLUDES_LOMSE}
    ${INCLUDES_AGG}
    ${LOMSE_ROOT_DIR}/src/agg/font_freetype
    ${INCLUDES_UTFCPP}
    ${INCLUDES_PUGIXML}
    ${INCLUDES_MINIZIP}
)


# Check for Bravura music font required to render scores
set (INSTALL_BRAVURA_FONT ${LOMSE_INSTALL_BRAVURA_FONT})
find_path(BRAVURA_FONT_PATH
          NAMES Bravura.otf
          PATHS
              /usr/share/fonts/truetype/          # Red Hat & fonts-lenmus-bravura.deb
              ${LOMSE_ROOT_DIR}/z_fonts           # local builds
              ${LOMSE_ROOT_DIR}/fonts             # local builds
              ${LOMSE_ROOT_DIR}/bravura/src       # builds for PPA at Launchpad.net
         )
if("${BRAVURA_FONT_PATH}" STREQUAL "BRAVURA_FONT_PATH-NOTFOUND")
    if (LOMSE_DOWNLOAD_BRAVURA_FONT)
        message(STATUS "Bravura font not found. Downloading it ...")
        file(DOWNLOAD 
            "https://github.com/steinbergmedia/bravura/raw/master/redist/otf/Bravura.otf"
            "${LOMSE_ROOT_DIR}/fonts/Bravura.otf"
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
        set(BRAVURA_FONT_PATH "${LOMSE_ROOT_DIR}/fonts/")
        set(INSTALL_BRAVURA_FONT ON)

    else(LOMSE_DOWNLOAD_BRAVURA_FONT)
        if (WIN32)
            message(FATAL_ERROR "Bravura font not found. Aborting...")
        else()
            message(STATUS "Bravura font not found. It will not be installed.")
        endif()
        set(INSTALL_BRAVURA_FONT OFF)
    endif(LOMSE_DOWNLOAD_BRAVURA_FONT)

else()
    message(STATUS "Bravura font found in ${BRAVURA_FONT_PATH}")
endif()

# Check for UnitTest++. Required for unit test
if (LOMSE_BUILD_TESTS)
	find_package(UnitTest++)
	if(UNITTEST++_FOUND)
		get_filename_component(UNITTEST++_LINK_DIR ${UNITTEST++_LIBRARY} DIRECTORY)
		include_directories(${UNITTEST++_INCLUDE_DIR})
		message("      include= ${UNITTEST++_INCLUDE_DIR}" )
	else()
		message(STATUS "Not found UnitTest++. Test program will not be built" )
		set (LOMSE_BUILD_TESTS OFF)
	endif()
endif(LOMSE_BUILD_TESTS)

if( LOMSE_ENABLE_COMPRESSION )
    # Check for zlib
    # libpng and freetype require zlib. So deal with zlib first.
    find_package(ZLIB REQUIRED)
    include_directories( ${ZLIB_INCLUDE_DIR} )
    message(STATUS "Found ZLib: ${ZLIB_LIBRARIES}" )
    message("      include= ${ZLIB_INCLUDE_DIR}" )
    set(LOMSE_REQUIRES "${LOMSE_REQUIRES}, zlib")
    set(LOMSE_BUILD_DEPS ${LOMSE_BUILD_DEPS} ${ZLIB_LIBRARIES})
    set(LOMSE_DEBIAN_DEPS "${LOMSE_DEBIAN_DEPS}, zlib1g (>= ${ZLIB_VERSION_STRING})")
endif(LOMSE_ENABLE_COMPRESSION)

# Check for FreeType
find_package(Freetype REQUIRED)                 
include_directories( ${FREETYPE_INCLUDE_DIRS} )
message(STATUS "Found Freetype: ${FREETYPE_LIBRARY}" )
message("      include= ${FREETYPE_INCLUDE_DIRS}" )
set(LOMSE_REQUIRES "${LOMSE_REQUIRES}, freetype2")
set(LOMSE_DEBIAN_DEPS "${LOMSE_DEBIAN_DEPS}, libfreetype6")
set(LOMSE_BUILD_DEPS ${LOMSE_BUILD_DEPS} ${FREETYPE_LIBRARY})

# Check for libpng
if( LOMSE_ENABLE_PNG )
    find_package(PNG REQUIRED)  
    include_directories( ${PNG_INCLUDE_DIRS} )
    message(STATUS "Found PNG: ${PNG_LIBRARIES}" )
    message("     include= ${PNG_INCLUDE_DIRS}" )
    set(LOMSE_REQUIRES "${LOMSE_REQUIRES}, libpng")
    set(LOMSE_BUILD_DEPS ${LOMSE_BUILD_DEPS} ${PNG_LIBRARIES})
    set(LOMSE_DEBIAN_DEPS "${LOMSE_DEBIAN_DEPS}, 
		libpng12-0 (>=1.2.42) | libpng16-16" )
endif(LOMSE_ENABLE_PNG)

# Check for fontconfig
if (UNIX)  #Linux, macOS & Android
    find_path(FONTCONFIG_INCLUDE_DIR fontconfig/fontconfig.h)
    find_library(FONTCONFIG_LIBRARIES NAMES fontconfig)
    if (("${FONTCONFIG_INCLUDE_DIR}" STREQUAL "FONTCONFIG_INCLUDE_DIR-NOTFOUND")
        OR ("${FONTCONFIG_LIBRARIES}" STREQUAL "FONTCONFIG_LIBRARIES-NOTFOUND")
       )
        set(FONTCONFIG_FOUND FALSE)
    else()
        set(FONTCONFIG_FOUND TRUE)
    endif()

    if(FONTCONFIG_FOUND )
        include_directories( ${FONTCONFIG_INCLUDE_DIR}/fontconfig )
        set(LOMSE_REQUIRES "${LOMSE_REQUIRES}, fontconfig")
        set(LOMSE_BUILD_DEPS ${LOMSE_BUILD_DEPS} ${FONTCONFIG_LIBRARIES})
        set(LOMSE_DEBIAN_DEPS "${LOMSE_DEBIAN_DEPS}, fontconfig")
        message(STATUS "Found FontConfig: libraries= ${FONTCONFIG_LIBRARIES}" )
        message("        include= ${FONTCONFIG_INCLUDE_DIR}/fontconfig" )
    else()
        message(FATAL_ERROR "fontconfig package not found.")
    endif()
endif()

# strip leading commas from LOMSE_REQUIRES and LOMSE_DEBIAN_DEPS
string( REGEX REPLACE "^, " "" LOMSE_REQUIRES ${LOMSE_REQUIRES})
string( REGEX REPLACE "^, " "" LOMSE_DEBIAN_DEPS ${LOMSE_DEBIAN_DEPS})

if(UNIX)
    # macros for "Print all warnings", GCC & __UNIX__
    add_definitions( -Wall -DGCC -D__UNIX__ )
endif(UNIX)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# source files to compile
include( ${LOMSE_ROOT_DIR}/add-sources.cmake )


# Adds folders for Visual Studio and other IDEs
set (LOMSE_GROUP_FOLDER "lomse/" )
include( ${LOMSE_ROOT_DIR}/add-group-folders.cmake )

message(STATUS "LOMSE_BUILD_DEPS= ${LOMSE_BUILD_DEPS}")

#remove dependencies for monolithic builds
if (LOMSE_BUILD_MONOLITHIC)
    set(LOMSE_DEBIAN_DEPS "")
endif()



#define a header file to pass CMake settings to source code
configure_file(
    "${LOMSE_ROOT_DIR}/lomse_config.h.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lomse_config.h"
)



#======= End of dependencies checking ========================================

