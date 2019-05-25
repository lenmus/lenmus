#---------------------------------------------------------------------------------------
#  This file is part of LenMus Phonascus program
#  Copyright (c) 2002-2019 LenMus project
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
#   cmake -G "CodeBlocks - Unix Makefiles" ...
#
# The default installation prefix is "/usr/local". You can change the install 
# location by specifying option CMAKE_INSTALL_PREFIX:
#   cmake -DCMAKE_INSTALL_PREFIX=/new/install/prefix ...
#
# By default, lenmus is installed in a version subfolder, such as
# /usr/local/share/lenmus/5.3/<files>, so that several lenmus versions can
# coexist in your machine. But if this is not required, you can install without 
# the version folder (i.e. to /usr/local/share/lenmus/<files>) by setting option
# LENMUS_USE_VERSION_FOLDER to OFF:
#   cmake -DLENMUS_USE_VERSION_FOLDER:BOOL=OFF ...
#
# The default folder (in Linux) for installing man pages is
#   /usr/share/man
# You can specify other location by using option MAN_INSTALL_DIR:
# choose is "Unix Makefiles". If you would like to generate
#   cmake -DMAN_INSTALL_DIR=/new/install/location/for/manpages ...
#
# By default unit tests are also build. You can reduce executable size by
# disabling this option:
#   cmake -DLENMUS_ENABLE_UNIT_TESTS:BOOL=OFF ...
#
#-------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.1)

message(STATUS "Using CMake version ${CMAKE_VERSION}")
message(STATUS "Begin CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX} )


# project name
project(lenmus)
set( LENMUS_APP_NAME     "\"Lenmus Phonascus\"" )
set( LENMUS_VENDOR_NAME  "\"Lenmus\"" )
set( LENMUS_VENDOR_SITE  "\"http://www.lenmus.org\"" )
message(STATUS "Project set CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX} )

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
set(CMAKE_CXX_STANDARD 11)				#require c+11 or greater
set(CMAKE_CXX_STANDARD_REQUIRED ON) 	#prevent fallback to any previous standard

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
OPTION(LENMUS_USE_VERSION_FOLDER
    "Install LenMus using a version subfolder (i.e. /usr/local/share/lenmus/5.3/)"
    ON)
OPTION(LENMUS_USE_LOMSE_SOURCES
    "Build LenMus using Lomse sources instead of using the library"
    ON)
OPTION(LENMUS_ENABLE_UNIT_TESTS
	"Include also unit tests in LenMus executable"
	ON)
set(MAN_INSTALL_DIR "/usr/share/man" 
    CACHE STRING "Directory for installing man pages")


# macro STRING_UNQUOTE(var str)
#   Remove quote marks or double quote marks around a string.
#   If the string is not quoted, then content of str is copied to var
#   * Parameters:
#     + var: A variable that stores the result.
#     + str: A string.
#------------------------------------------------------
MACRO(STRING_UNQUOTE var str)
	SET(_ret "${str}")
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
	SET(${var} "${_ret}")
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
elseif(UNIX)
    set( LENMUS_PLATFORM_WIN32 "0")
    set( LENMUS_PLATFORM_UNIX "1")
    set( LENMUS_PLATFORM_MAC "0")
endif()

# compiler
if(MSVC)
    set( LENMUS_COMPILER_MSVC "1")
    set( LENMUS_COMPILER_GCC "0")
elseif(CMAKE_COMPILER_IS_GNUCC)
    set( LENMUS_COMPILER_MSVC "0")
    set( LENMUS_COMPILER_GCC "1")
endif()

# build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set( LENMUS_DEBUG_BUILD   1)
    set( LENMUS_RELEASE_INSTALL 0)
else ()
    set( LENMUS_DEBUG_BUILD   0)
    set( LENMUS_RELEASE_INSTALL 1)
endif ()


message(STATUS "Build type (CMAKE_BUILD_TYPE) is ${CMAKE_BUILD_TYPE}")
message(STATUS "Build type (LENMUS_DEBUG_BUILD) set to ${LENMUS_DEBUG_BUILD}")

# path to scores for tests
set( LENMUS_TEST_SCORES_PATH     "\"${LENMUS_ROOT_DIR}/scores/samples/test_set/\"" )
set( LENMUS_SOURCE_ROOT          "\"${LENMUS_ROOT_DIR}/\"" )


# installation folders
# ------------------------------------------------------------------------------
# You can change the install location by running cmake like this:
#
#   cmake -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# Defaults:
#   Linux:      "/usr/local/lenmus"
#   Windows:    "C:/Program Files (x86)/lenmus" or "C:/Program Files/lenmus"
# ------------------------------------------------------------------------------
set(LENMUS_USE_SOURCE_TREE 0)

#set install folder 
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    #Fix CMAKE_INSTALL_PREFIX for Windows.
    #Sometimes it points to 'Program Files (x86)' for x64 builds
    if(WIN32)
        if(CMAKE_CL_64) #CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(CMAKE_INSTALL_PREFIX "C:/Program Files/${CMAKE_PROJECT_NAME}")
        else()
            set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/${CMAKE_PROJECT_NAME}")
        endif()
    endif()
endif()
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
set(ROOT_INSTALL ${CMAKE_INSTALL_PREFIX} )

if( UNIX )
    if (LENMUS_USE_VERSION_FOLDER)
        set(ROOT_INSTALL_HOME ${ROOT_INSTALL}/share/${CMAKE_PROJECT_NAME}/${LENMUS_PACKAGE_VERSION} )
    else()
        set(ROOT_INSTALL_HOME ${ROOT_INSTALL}/share/${CMAKE_PROJECT_NAME} )
    endif()
elseif( WIN32 )
    set(ROOT_INSTALL_HOME ${ROOT_INSTALL} )
endif()
set(LENMUS_INSTALL_ROOT "${ROOT_INSTALL_HOME}/" )

message(STATUS "ROOT_INSTALL_HOME = ${ROOT_INSTALL_HOME}" )
message(STATUS "LENMUS_INSTALL_ROOT = ${LENMUS_INSTALL_ROOT}" )



# set up other variables
#--------------------------------------------------------

# add headers directories from source tree
include_directories(
    ${LENMUS_ROOT_DIR}/include
    ${LENMUS_ROOT_DIR}/packages/wxMidi/src
)


# Define possible builds
set(CMAKE_CONFIGURATION_TYPES Debug Release)





##########################################################################
#
# Dependencies
# Check for needed libraries and set all related includes, flags, etc. 
#
###############################################################################



# Check for UnitTest++. Required when unit tests are enabled (default)
if (LENMUS_ENABLE_UNIT_TESTS)
    find_package(UnitTest++)
    if(0) #UNITTEST++_FOUND)
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
message("LENMUS_ENABLE_UNIT_TESTS = " ${LENMUS_ENABLE_UNIT_TESTS} )


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
    message(STATUS "SQLite3_LIBRARIES => " ${SQLite3_LIBRARIES})
    message(STATUS "SQLite3_INCLUDE_DIR => " ${SQLite3_INCLUDE_DIR})
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
    message(STATUS "PortMidi_INCLUDE_DIRS => " ${PortMidi_INCLUDE_DIRS} )
    message(STATUS "PortMidi_LIBRARIES => " ${PortMidi_LIBRARIES} )
ELSE(PortMidi_FOUND)
    message(SEND_ERROR "PortMidi not found")
    message(STATUS "PortMidi_DIR = $ENV{PortMidi_DIR}")
ENDIF(PortMidi_FOUND)

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
    message("LOMSE: Not using Lomse library, but building with its sources")
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
    message("Freetype found: libraries= ${FREETYPE_LIBRARIES}" )
    message("Freetype found: include= ${FREETYPE_INCLUDE_DIRS}" )
else()
    message(SEND_ERROR "FreeType package not found.")
    message(STATUS "FreeType_DIR = $ENV{FreeType_DIR}")
endif()

# Check for libpng: required by Lomse
find_package(PNG REQUIRED)
if( PNG_FOUND )
    include_directories( ${PNG_INCLUDE_DIRS} )
    link_libraries( ${PNG_LIBRARIES} )
    message("libpng found: libraries= ${PNG_LIBRARIES}" )
    message("libpng found: include= ${PNG_INCLUDE_DIRS}" )
else()
    message(SEND_ERROR "libpng package not found.")
endif()

# Check for zlib: required by Lomse
find_package(ZLIB REQUIRED)
if( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIR} )
    link_libraries( ${ZLIB_LIBRARY} )
    message("zlib found: libraries= ${ZLIB_LIBRARY}" )
    message("zlib found: include= ${ZLIB_INCLUDE_DIRS}" )
else()
    message(SEND_ERROR "zlib package not found.")
endif()



# "Print all warnings", macros for GCC & __UNIX__
if(UNIX)
    add_definitions( -Wall -DGCC -D__UNIX__ )
endif(UNIX)

#process template header config file to replace CMake settings
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/lenmus_config.h.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lenmus_config.h"
)

#include binary dir so that lenmus_config.h can be accesed during build
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


if (LENMUS_USE_LOMSE_SOURCES)
    #define path were fonts will be installed.
    set( FONTS_PATH   "${ROOT_INSTALL_HOME}/res/fonts" )

    # include Lomse build options and create the include file "lomse_config.h"
    include( ${LOMSE_ROOT_DIR}/build-options.cmake )
    set_target_properties(build-version PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()


# create and include the man page
if (UNIX)
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

set (LENMUS  lenmus)

# set name of lenmus executable
if( WIN32 )
    set( CMAKE_EXECUTABLE_SUFFIX ".exe" )
elseif( UNIX )
    set( CMAKE_EXECUTABLE_SUFFIX "" )
endif()



# source files to compile

set(APP_FILES
    ${SRC_DIR}/app/lenmus_about_dialog.cpp
    ${SRC_DIR}/app/lenmus_app.cpp
    ${SRC_DIR}/app/lenmus_art_provider.cpp
    ${SRC_DIR}/app/lenmus_book_reader.cpp
    ${SRC_DIR}/app/lenmus_canvas.cpp
    ${SRC_DIR}/app/lenmus_command_event_handler.cpp
    ${SRC_DIR}/app/lenmus_command_window.cpp
    ${SRC_DIR}/app/lenmus_content_box.cpp
    ${SRC_DIR}/app/lenmus_content_box_ctrol.cpp
    ${SRC_DIR}/app/lenmus_document_canvas.cpp
    ${SRC_DIR}/app/lenmus_document_frame.cpp
    ${SRC_DIR}/app/lenmus_events.cpp
    ${SRC_DIR}/app/lenmus_help_system.cpp
    ${SRC_DIR}/app/lenmus_main_frame.cpp
    ${SRC_DIR}/app/lenmus_midi_wizard.cpp
    ${SRC_DIR}/app/lenmus_preferences.cpp
    ${SRC_DIR}/app/lenmus_score_wizard.cpp
    ${SRC_DIR}/app/lenmus_splash_frame.cpp
    ${SRC_DIR}/app/lenmus_status_bar.cpp
    ${SRC_DIR}/app/lenmus_string.cpp
    ${SRC_DIR}/app/lenmus_virtual_keyboard.cpp
    ${SRC_DIR}/app/lenmus_welcome_window.cpp
    ${SRC_DIR}/app/lenmus_zip_reader.cpp
)

set(AUXMUSIC_FILES
    ${SRC_DIR}/auxmusic/lenmus_cadence.cpp
    ${SRC_DIR}/auxmusic/lenmus_chord.cpp
    ${SRC_DIR}/auxmusic/lenmus_composer.cpp
    ${SRC_DIR}/auxmusic/lenmus_interval.cpp
    ${SRC_DIR}/auxmusic/lenmus_images_creator.cpp
    ${SRC_DIR}/auxmusic/lenmus_scale.cpp
    ${SRC_DIR}/auxmusic/lenmus_score_corrector.cpp
    ${SRC_DIR}/auxmusic/lenmus_utilities.cpp
)

set(DIALOGS_FILES
    ${SRC_DIR}/dialogs/lenmus_dlg_books.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_choose_lang.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_clefs.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_debug.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_metronome.cpp
    ${SRC_DIR}/dialogs/lenmus_text_editor.cpp
)

set(EXERCISES_FILES
    ${SRC_DIR}/exercises/auxctrols/lenmus_counters_ctrol.cpp
    ${SRC_DIR}/exercises/auxctrols/lenmus_problem_displayer.cpp

    ${SRC_DIR}/exercises/constrains/lenmus_cadences_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_chords_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_dictation_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_ear_intervals_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_figured_bass_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_harmony_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_key_sign_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_music_reading_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_notes_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_scales_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_scores_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_theo_intervals_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_tonality_constrains.cpp

    ${SRC_DIR}/exercises/ctrols/lenmus_dyncontrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_dictation_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_ear_compare_intv_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_ear_intervals_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_exercise_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_figured_bass_ctrol.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_generators.cpp
    ${SRC_DIR}/exercises/ctrols/lenmus_harmony_ctrol.cpp
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
    ${SRC_DIR}/globals/lenmus_actions.cpp
    ${SRC_DIR}/globals/lenmus_colors.cpp
    ${SRC_DIR}/globals/lenmus_injectors.cpp
    ${SRC_DIR}/globals/lenmus_paths.cpp
)

set(OPTIONS_FILES
    ${SRC_DIR}/options/lenmus_colors_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_internet_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_lang_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_languages.cpp
    ${SRC_DIR}/options/lenmus_options_dlg.cpp
    ${SRC_DIR}/options/lenmus_other_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_other_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_playback_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_shortcuts_opt_panel.cpp
    ${SRC_DIR}/options/lenmus_toolbar_opt_panel.cpp
)

set(PROPERTIES_FILES
    ${SRC_DIR}/properties/lenmus_dlg_properties.cpp
    ${SRC_DIR}/properties/lenmus_properties_barline.cpp
    ${SRC_DIR}/properties/lenmus_properties_general.cpp
)

set(SOUND_FILES
    ${SRC_DIR}/sound/lenmus_metronome.cpp
    ${SRC_DIR}/sound/lenmus_midi_server.cpp
    ${SRC_DIR}/sound/lenmus_wave_player.cpp
)

set(TOOLBOX_FILES
    ${SRC_DIR}/toolbox/lenmus_time_signature_properties.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_box.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_box_events.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_box_theme.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_group.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_page.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_page_barlines.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_page_clefs.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_page_notes.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_page_rhythmic_dictation.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_page_selector.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_page_symbols.cpp
    ${SRC_DIR}/toolbox/lenmus_tool_page_top_level.cpp
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
        ${SRC_DIR}/tests/lenmus_test_score_corrector.cpp
        ${SRC_DIR}/tests/lenmus_test_tokenizer.cpp
        ${SRC_DIR}/tests/lenmus_test_runner.cpp
    )
endif()

set(LENMUS_PACKAGES_FILES
    ${PACKAGES_DIR}/wxMidi/src/wxMidi.cpp
    ${PACKAGES_DIR}/wxMidi/src/wxMidiDatabase.cpp
    ${PACKAGES_DIR}/wxSQLite3/src/wxsqlite3.cpp
)

set(UPDATER_FILES
    ${SRC_DIR}/updater/lenmus_updater.cpp
    ${SRC_DIR}/updater/lenmus_updater_dlg.cpp
)

set(WIDGETS_FILES
    ${SRC_DIR}/widgets/lenmus_msg_box.cpp
    ${SRC_DIR}/widgets/lenmus_button.cpp
    ${SRC_DIR}/widgets/lenmus_wizard.cpp
)

set(ALL_LENMUS_SOURCES 
    ${APP_FILES} ${AUXMUSIC_FILES} ${DIALOGS_FILES} ${EXERCISES_FILES}
    ${GLOBALS_FILES} ${OPTIONS_FILES} ${PROPERTIES_FILES}
    ${SOUND_FILES} ${TOOLBOX_FILES} ${XML_PARSER_FILES} ${TEST_FILES}
    ${LENMUS_PACKAGES_FILES} ${UPDATER_FILES} ${WIDGETS_FILES}
)

# Add folders for Visual Studio and other IDEs supporting it
source_group( "app"             FILES ${APP_FILES} )
source_group( "auxmusic"        FILES ${AUXMUSIC_FILES} )
source_group( "dialogs"         FILES ${DIALOGS_FILES} )
source_group( "exercises"       FILES ${EXERCISES_FILES} )
source_group( "globals"         FILES ${GLOBALS_FILES} )
source_group( "options"         FILES ${OPTIONS_FILES} )
source_group( "properties"      FILES ${PROPERTIES_FILES} )
source_group( "sound"           FILES ${SOUND_FILES} )
source_group( "xml_parser"      FILES ${XML_PARSER_FILES} )
source_group( "packages"        FILES ${LENMUS_PACKAGES_FILES} )
source_group( "toolbox"         FILES ${TOOLBOX_FILES} )
source_group( "updater"         FILES ${UPDATER_FILES} )
source_group( "widgets"         FILES ${WIDGETS_FILES} )
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
add_executable( ${LENMUS} WIN32 MACOSX_BUNDLE  ${ALL_SOURCES} )
add_dependencies(${LENMUS} lenmus-version)


# Add resources
#if(WIN32)
    # Include a RC file for windows
#    list(APPEND ALL_SOURCES ${RESOURCES_DIR}/other/lenmus.rc)
#    source_group("resources" FILES ${RESOURCES_DIR}/other/lenmus.rc )
#elseif(APPLE)
    # Add an icon for the apple .app file
    #list(APPEND ALL_SOURCES ${RESOURCES_DIR}/other/osx/carbon/wxmac.icns)
#endif()


# Compiler options
if (WIN32)
    # remove noisy warning 4996: function or variable may be unsafe (i.e. strcat)
    set_target_properties(${LENMUS} PROPERTIES COMPILE_FLAGS "/wd4996")
endif(WIN32)


# Linker options
#if(WIN32)
	#if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")
		#link_libraries(libucrtd.lib)
	#else()
		#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")
		#link_libraries(libucrt.lib)
	#endif()
#endif(WIN32)


# libraries to link
target_link_libraries ( ${LENMUS} 
            ${wxWidgets_LIBRARIES} ${PortMidi_LIBRARIES} ${PortTime_LIBRARIES}
            ${LOMSE_LIBRARIES} ${SQLite3_LIBRARIES} ${UNITTEST_LIBRARIES} 
			${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} 
			${ZLIB_LIBRARIES}
)


# Target properties
if(APPLE)
#    set_target_properties(${PROJECT_NAME} PROPERTIES
#        RESOURCE ${RESOURCES_DIR}/other/osx/carbon/wxmac.icns
#        MACOSX_BUNDLE_ICON_FILE wxmac.icns
#        MACOSX_BUNDLE_COPYRIGHT "Copyright LenMus"
#        MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxwidgets.minimal"
#        )
endif()

if (WIN32)
	# In Windows force to use static Runtime library
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
endif(WIN32)

# Display settings
message("Configuration for target ${LENMUS}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
else()
	message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
endif()
message(STATUS "Linker flags: ${CMAKE_EXE_LINKER_FLAGS}" )

foreach (prop ${CMAKE_PROPERTY_LIST})
    get_property(propval TARGET ${LENMUS} PROPERTY ${prop} SET)
    if (propval)
        get_target_property(propval ${LENMUS} ${prop})
        message (STATUS "${prop} = ${propval}")
    endif()
endforeach(prop)


#///////////////////////////////////////////////////////////////////////////////
# Program installation
# You can change the install location by running cmake like this:
#
#   cmake -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# By default, the prefix is "/usr/local"
#
# Remember:
#  LENMUS_ROOT_DIR: the root of the package source tree: i.e. $HOME/Downloads/lenmus-5.3
#  ROOT_INSTALL:  install location: i.e. /usr/local
#  ROOT_INSTALL_HOME: shared files i.e. /usr/local/share/lenmus/5.3/
#///////////////////////////////////////////////////////////////////////////////

    # lenmus program
install( TARGETS ${LENMUS} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )

    # 1. Software and essentials (ROOT_INSTALL_HOME):
    # ------------------------------------------------------------------------------

    # files in root folder
install(FILES  AUTHORS CHANGELOG.md INSTALL LICENSE
               NEWS README.md THANKS 
        DESTINATION ${ROOT_INSTALL_HOME} )

    # locale
        # ADD_LANG
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_de.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_en.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_es.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_eu.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_fr.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_gl_ES.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_it.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_nl.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_tr.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/installer/msw/locale/license_zh_CN.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
#install(FILES ${LENMUS_ROOT_DIR}/installer/msw/license_el_GR.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/docs/html/LICENSE_GNU_GPL_1.3.txt  DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${LENMUS_ROOT_DIR}/docs/html/LICENSE_GNU_FDL_1.3.txt  DESTINATION "${ROOT_INSTALL_HOME}/locale")
     
    # locale. Common folder
install(DIRECTORY ${LENMUS_ROOT_DIR}/locale/common  DESTINATION ${ROOT_INSTALL_HOME}/locale
        FILES_MATCHING PATTERN "*.*"   PATTERN ".svn" EXCLUDE )

    # locale. eBooks, help files and language catalogs for all languages
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/bitmaps   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/icons   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/sounds   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/keys   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.png" PATTERN ".svn" EXCLUDE )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/cursors   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.png" PATTERN ".svn" EXCLUDE )
install(FILES ${LENMUS_ROOT_DIR}/res/desktop/lenmus.desktop  DESTINATION "${ROOT_INSTALL_HOME}/res/desktop")

install(DIRECTORY  ${LENMUS_ROOT_DIR}/locale    DESTINATION "${ROOT_INSTALL_HOME}"
        FILES_MATCHING
        PATTERN "*.htm"
        #books 
        PATTERN "*.lmb"
        #help
        PATTERN "*.html"
        PATTERN "*.js"
        PATTERN "*.map"
        PATTERN "*.inv"
        PATTERN "*.png"
        PATTERN "*.gif"
        PATTERN "*.jpg"
        PATTERN "*.css"
        #language catalogs
        PATTERN "*.po"
        PATTERN "*.mo"
        PATTERN "x_*" EXCLUDE
        PATTERN "z_*" EXCLUDE
        PATTERN "*~" EXCLUDE
        #exclude unsupported languages
        PATTERN "kk/" EXCLUDE
        PATTERN "ru/" EXCLUDE
        PATTERN "el_GR/" EXCLUDE
        PATTERN "x_*" EXCLUDE )
    
    # resources
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/bitmaps   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN "x_*" EXCLUDE )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/cursors   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.png" PATTERN "x_*" EXCLUDE )
install(FILES ${LENMUS_ROOT_DIR}/res/desktop/lenmus.desktop  DESTINATION "${ROOT_INSTALL_HOME}/res/desktop")
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/icons   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN "x_*" EXCLUDE )
install(DIRECTORY ${LENMUS_ROOT_DIR}/res/sounds   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN "x_*" EXCLUDE )
#install(DIRECTORY ${LENMUS_ROOT_DIR}/res/keys   DESTINATION ${ROOT_INSTALL_HOME}/res
#        FILES_MATCHING PATTERN "*.png" PATTERN "x_*" EXCLUDE )

if (LENMUS_USE_LOMSE_SOURCES)
    #install Lomse fonts
    install(DIRECTORY  "${LOMSE_ROOT_DIR}/fonts/"
            DESTINATION ${ROOT_INSTALL_HOME}/res/fonts
            FILES_MATCHING PATTERN "*.ttf" 
                           PATTERN "*.ttc"
                           PATTERN "Bravura.otf" )
endif()

     # xrc
install(DIRECTORY ${LENMUS_ROOT_DIR}/xrc  DESTINATION ${ROOT_INSTALL_HOME}
        FILES_MATCHING PATTERN "*.xrc"   PATTERN "x_*" EXCLUDE)
     
     # /templates
install(DIRECTORY ${LENMUS_ROOT_DIR}/templates   DESTINATION ${ROOT_INSTALL_HOME}
        FILES_MATCHING PATTERN "*.lms"   PATTERN ".svn" EXCLUDE)

    # desktop and application icon 48x48 png
install(FILES ${LENMUS_ROOT_DIR}/res/desktop/lenmus.desktop  DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
install(FILES ${LENMUS_ROOT_DIR}/res/icons/lenmus.png  DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")


    # 4. User scores and samples 
    # ------------------------------------------------------------------------------
if( UNIX )
    set(ROOT_DATA_HOME $ENV{HOME}/${CMAKE_PROJECT_NAME} )
    set(ROOT_SAMPLES ${ROOT_INSTALL_HOME} )
elseif( WIN32 )
    set(ROOT_DATA_HOME ${ROOT_INSTALL} )
    set(ROOT_SAMPLES ${ROOT_DATA_HOME}/${LENMUS_PACKAGE_VERSION} )
endif()
set(ROOT_SCORES ${ROOT_DATA_HOME}/scores )

message("ROOT_SCORES = " ${ROOT_SCORES} )
message("ROOT_SAMPLES = " ${ROOT_SAMPLES} )

install(DIRECTORY ${LENMUS_ROOT_DIR}/scores/samples  DESTINATION "${ROOT_SAMPLES}"
	    FILES_MATCHING PATTERN "*.lms"
	    PATTERN ".svn" EXCLUDE
	    PATTERN "test_set" EXCLUDE
)





#///////////////////////////////////////////////////////////////////////////////
# CPack section: installers generation
#///////////////////////////////////////////////////////////////////////////////

set(CPACK_PACKAGE_VENDOR "LenMus")
set(CPACK_PACKAGE_DESCRIPTION "A program for aural training and learning music theory")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lenmus Phonascus is a totally free program for studying music theory that allows you to focus on specific skills and exercises, on both theory and aural training. The different activities can be customized to meet your needs. Phonascus allows you to work at your own pace, providing interactive feedback until mastery of each concept is achieved. ")
set(CPACK_PACKAGE_CONTACT "s.cecilio@gmail.com")

if(UNIX)
	#variables for Debian package
	SET(CPACK_GENERATOR "DEB")
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cecilio Salmeron")
	set(CPACK_DEBIAN_PACKAGE_NAME "lenmus_${LENMUS_PACKAGE_VERSION}")
	set(CPACK_DEBIAN_PACKAGE_VERSION ${LENMUS_PACKAGE_VERSION} )
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "www.lenmus.org")
	set(CPACK_DEBIAN_PACKAGE_SECTION "education")

	IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^x86_64$|^amd64$")  
	  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
	ELSE()
	  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
	ENDIF() 
	 
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
	if (LENMUS_USE_LOMSE_SOURCES)
		set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsqlite3-0, libportmidi0,
		            libfreetype6, libpng12-0 (>=1.2.42), zlib1g (>= 1:1.2.1) ")
	else()
		set(CPACK_DEBIAN_PACKAGE_DEPENDS "liblomse (>= 0.25), libsqlite3-0, libportmidi0,
		        libfreetype6, libpng12-0 (>=1.2.42), zlib1g (>= 1:1.2.1) ")
	endif()
	message("Package name: ${CPACK_PACKAGE_FILE_NAME}" )
	message("Package dependencies: ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )

	include(CPack)
endif(UNIX)

