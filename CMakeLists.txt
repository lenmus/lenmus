#---------------------------------------------------------------------------------------
#  This file is part of LenMus Phonascus program
#  Copyright (c) 2002-2012 LenMus project
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
#-------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)

# project name
project(lenmus)
set( LENMUS_APP_NAME     "\"Lenmus Phonascus\"" )
set( LENMUS_VENDOR_NAME  "\"Lenmus\"" )
set( LENMUS_VENDOR_SITE  "\"http://www.lenmus.org\"" )

# main directories 
set( ROOT_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )
set( SRC_DIR  ${ROOT_DIR}/src )
set( RESOURCES_DIR  ${ROOT_DIR}/res )
set( PACKAGES_DIR ${ROOT_DIR}/packages )
set( OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/bin )
set( EXECUTABLE_OUTPUT_PATH ${OUTDIR})


# directories to search for CMake modules
set( CMAKE_MODULE_PATH    "${ROOT_DIR}/cmake-modules" )

# some options for building
OPTION(LENMUS_USE_VERSION_FOLDER
  "Install LenMus using a version subfolder (i.e. /usr/local/share/lenmus/5.3/)"
  ON)


# set up configuration variables for lenmus_config.h
#------------------------------------------------------

# version. Extract values from lenmus_version.h header file
file(STRINGS ${ROOT_DIR}/include/lenmus_version.h LENMUS_VERSION_LIST)
list (GET LENMUS_VERSION_LIST 5 MAJOR_LINE)
list (GET LENMUS_VERSION_LIST 6 MINOR_LINE)
list (GET LENMUS_VERSION_LIST 7 TYPE_LINE)
list (GET LENMUS_VERSION_LIST 8 PATCH_LINE)
string(REGEX REPLACE "\#define LENMUS_VERSION_MAJOR   " "" LENMUS_VERSION_MAJOR "${MAJOR_LINE}")
string(REGEX REPLACE "\#define LENMUS_VERSION_MINOR   " "" LENMUS_VERSION_MINOR "${MINOR_LINE}")
string(REGEX REPLACE "\#define LENMUS_VERSION_TYPE    " "" LENMUS_VERSION_TYPE "${TYPE_LINE}")
string(REGEX REPLACE "\#define LENMUS_VERSION_PATCH   " "" LENMUS_VERSION_PATCH "${PATCH_LINE}")

message ("major = '${LENMUS_VERSION_MAJOR}'") 
message ("minor = '${LENMUS_VERSION_MINOR}'") 
message ("type = '${LENMUS_VERSION_TYPE}'") 
message ("patch = '${LENMUS_VERSION_PATCH}'") 


#build version string for installer name
set(LENMUS_VERSION_STRING "${LENMUS_VERSION_MAJOR}.${LENMUS_VERSION_MINOR}" )
if (NOT("${LENMUS_VERSION_TYPE}" STREQUAL "' '"))
    if ("${LENMUS_VERSION_TYPE}" STREQUAL "'a'")
        set(LTYPE "a")
    else()
        set(LTYPE "b")
    endif()
    set(LENMUS_VERSION_STRING "${LENMUS_VERSION_STRING}.${LTYPE}${LENMUS_VERSION_PATCH}" )
else()
    if (NOT("${LENMUS_VERSION_PATCH}" STREQUAL "0"))
        set(LENMUS_VERSION_STRING "${LENMUS_VERSION_STRING}.${LENMUS_VERSION_PATCH}" )
    endif()
endif()
message ("version string = '${LENMUS_VERSION_STRING}'") 


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


message(STATUS "Build type (LENMUS_DEBUG_BUILD) set to ${LENMUS_DEBUG_BUILD}")

# path to scores for tests
set( LENMUS_TEST_SCORES_PATH     "\"${ROOT_DIR}/scores/samples/test_set/\"" )
set( LENMUS_SOURCE_ROOT          "\"${ROOT_DIR}/\"" )


# installation folders
# ------------------------------------------------------------------------------
# You can change the install location by running cmake like this:
#
#   cmake -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# By default, the prefix is "/usr/local"
# ------------------------------------------------------------------------------
set(LENMUS_USE_SOURCE_TREE 0)

message("CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX} )
set(ROOT_INSTALL ${CMAKE_INSTALL_PREFIX} )

if( UNIX )
    if (LENMUS_USE_VERSION_FOLDER)
        set(ROOT_INSTALL_HOME ${ROOT_INSTALL}/share/${CMAKE_PROJECT_NAME}/${LENMUS_VERSION_STRING} )
    else()
        set(ROOT_INSTALL_HOME ${ROOT_INSTALL}/share/${CMAKE_PROJECT_NAME} )
    endif()
elseif( WIN32 )
    set(ROOT_INSTALL_HOME ${ROOT_INSTALL} )
endif()
set(LENMUS_INSTALL_ROOT "\"${ROOT_INSTALL_HOME}/\"" )

message("ROOT_INSTALL_HOME = " ${ROOT_INSTALL_HOME} )
message("LENMUS_INSTALL_ROOT = " ${LENMUS_INSTALL_ROOT} )



# set up other variables
#--------------------------------------------------------


# check that compiler supports namespace sdt
include(TestForSTDNamespace)
if(CMAKE_NO_STD_NAMESPACE)
  message(FATAL_ERROR "The compiler doesn't support namespace std.")
endif()

 
# add headers directories from source tree
include_directories(
    ${ROOT_DIR}/include
    ${ROOT_DIR}/packages/wxMidi/include
    ${UNITTEST_INCLUDE_DIRS}
)


# Define possible builds
set(CMAKE_CONFIGURATION_TYPES Debug Release)



# Check for needed libraries and set all related includes, flags, etc. 
#-------------------------------------------------------------------------


# Check for UnitTest++. Required for Debug version
if(0)
if(WIN32)
    set( UNITTEST_LIBDIR  ${ROOT_DIR}/packages/UnitTest++/lib )
    set( UNITTEST_INCLUDE_DIRS  ${ROOT_DIR}/packages/UnitTest++/src )
    set( UNITTEST_LIBRARIES  "UnitTest++.vsnet2003.lib" )
elseif(UNIX)
    include(FindPkgConfig)
    pkg_check_modules(UNITTEST REQUIRED unittest++)
    set( UNITTEST_LIBRARIES  "libunittest++.a" )
endif()
link_directories( ${UNITTEST_LIBDIR}  ${LIBRARY_OUTPUT_PATH} )
endif(0)

find_package(UnitTest REQUIRED)
if(UNITTEST_FOUND)
    include_directories( ${UNITTEST_INCLUDE_DIRS} )
    link_libraries( ${UNITTEST_LIBRARIES} )
    if( WIN32 )
        link_directories( ${ROOT_DIR}/packages/UnitTest++/lib )
    endif(WIN32)
else()
    message(STATUS "UnitTest++ not found")
endif()



# Check for wxWidgets
# http://docs.wxwidgets.org/2.8/wx_librarieslist.html
find_package(wxWidgets REQUIRED) 
IF(wxWidgets_FOUND)
    #include_directories( ${wxWidgets_INCLUDE_DIRS} )
    INCLUDE(${wxWidgets_USE_FILE})
    IF(UNIX)
        set(wxWidgets_CXX_FLAGS "`wx-config --cflags -D__WXGTK__  -D_UNICODE`")     
    ENDIF(UNIX)
ENDIF(wxWidgets_FOUND)
message(STATUS "wxWidgets => " ${wxWidgets_LIBRARIES})

# Check for Sqlite3
find_package(SQLite3 REQUIRED)
IF(SQLite3_FOUND)
    include_directories( ${SQLite3_INCLUDE_DIRS} )
    link_libraries( ${SQLite3_LIBRARIES} )
    message("SQLite3_LIBRARIES => " ${SQLite3_LIBRARIES})
    message("SQLite3_INCLUDE_DIRS => " ${SQLite3_INCLUDE_DIRS})
ELSE(SQLite3_FOUND)
    message(STATUS "SQLite3 not found")
ENDIF(SQLite3_FOUND)
#if(0)
include_directories("packages/wxSQLite3/include")
#include_directories("packages/wxSQLite3/sqlite3/include")
#endif(0)

#Check for PortMidi
find_package(PortMidi REQUIRED)
IF(PortMidi_FOUND)
    include_directories( ${PortMidi_INCLUDE_DIRS} )
    link_libraries( ${PortMidi_LIBRARIES} )
    message("PortMidi_INCLUDE_DIRS => " ${PortMidi_INCLUDE_DIRS} )
    message("PortMidi_LIBRARIES => " ${PortMidi_LIBRARIES} )
ELSE(PortMidi_FOUND)
    message(STATUS "PortMidi not found")
ENDIF(PortMidi_FOUND)

# Check for Lomse
find_package( Lomse )                    
if( LOMSE_FOUND )
    include_directories( ${LOMSE_INCLUDE_DIRS} )
    link_libraries( ${LOMSE_LIBRARIES} )
    message("LOMSE_INCLUDE_DIRS => " ${LOMSE_INCLUDE_DIRS} )
    message("LOMSE_LIBRARIES => " ${LOMSE_LIBRARIES} )
else()
    message(SEND_ERROR "Lomse package not found.")
endif()

# Check for Boost >= 1.42.0
set(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0"
    "1.45" "1.45.0" "1.46" "1.46.0" "1.47" "1.47.0" "1.48" "1.48.0""1.49" "1.49.0"
    "1.50" "1.50.0" "1.51" "1.51.0" "1.52" "1.52.0" )
set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED OFF)
#set(Boost_DEBUG TRUE)

find_package(Boost 1.42.0 COMPONENTS date_time thread REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries( ${Boost_LIBRARIES} )
    link_directories( ${Boost_LIBRARY_DIRS} )
    message("Boost found: libraries: ${Boost_LIBRARIES}" )
    message("Boost found: include= ${Boost_INCLUDE_DIRS}" )
    message("Boost found: libdir= ${Boost_LIBRARY_DIRS}" )
    message("Boost found: version= ${Boost_LIB_VERSION}" )
else()
    message(SEND_ERROR "Boost package not found. Reason: ${Boost_ERROR_REASON}" )
endif()

# Check for FreeType: header files required by some lomse headers
find_package(Freetype REQUIRED)              
if( FREETYPE_FOUND )
    include_directories( ${FREETYPE_INCLUDE_DIRS} )
    link_libraries( ${FREETYPE_LIBRARIES} )
    link_directories( ${FREETYPE_LINK_DIR} )        
    message("Freetype found: libraries= ${FREETYPE_LIBRARIES}" )
    message("Freetype found: include= ${FREETYPE_INCLUDE_DIRS}" )
    message("Freetype found: libdir= ${FREETYPE_LINK_DIR}" )
else()
    message(SEND_ERROR "FreeType package not found.")
endif()

# Check for libpng: required by Lomse
find_package(PNG REQUIRED)
if( PNG_FOUND )
    include_directories( ${PNG_INCLUDE_DIRS} )
    link_libraries( ${PNG_LIBRARIES} )
    link_directories( ${PNG_LINK_DIR} )
    message("libpng found: libraries= ${PNG_LIBRARIES}" )
    message("libpng found: include= ${PNG_INCLUDE_DIRS}" )
    message("libpng found: libdir= ${PNG_LINK_DIR}" )
else()
    message(SEND_ERROR "libpng package not found.")
endif()

# Check for zlib: required by Lomse
find_package(ZLIB REQUIRED)
if( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIR} )
    link_libraries( ${ZLIB_LIBRARIES} )
    link_directories( ${ZLIB_LINK_DIR} )        
    message("zlib found: libraries= ${ZLIB_LIBRARIES}" )
    message("zlib found: include= ${ZLIB_INCLUDE_DIRS}" )
    message("zlib found: libdir= ${ZLIB_LINK_DIR}" )
else()
    message(SEND_ERROR "zlib package not found.")
endif()

message(STATUS "CMAKE_MODULE_PATH => " ${CMAKE_MODULE_PATH})
    




# "Print all warnings", macros for GCC & __UNIX__
if(UNIX)
    add_definitions( -Wall -DGCC -D__UNIX__ )
endif(UNIX)

#define a header file to pass CMake settings to source code
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/lenmus_config.h.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lenmus_config.h"
)

#include binary dir so that lenmus_config.h can be accesed during build
include_directories("${CMAKE_CURRENT_BINARY_DIR}")




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
    ${SRC_DIR}/app/lenmus_content_box.cpp
    ${SRC_DIR}/app/lenmus_content_box_ctrol.cpp
    ${SRC_DIR}/app/lenmus_document_canvas.cpp
    ${SRC_DIR}/app/lenmus_document_frame.cpp
    ${SRC_DIR}/app/lenmus_events.cpp
    ${SRC_DIR}/app/lenmus_logger.cpp
    ${SRC_DIR}/app/lenmus_main_frame.cpp
    ${SRC_DIR}/app/lenmus_midi_wizard.cpp
    ${SRC_DIR}/app/lenmus_preferences.cpp
    ${SRC_DIR}/app/lenmus_splash_frame.cpp
    ${SRC_DIR}/app/lenmus_status_bar.cpp
    ${SRC_DIR}/app/lenmus_string.cpp
    ${SRC_DIR}/app/lenmus_welcome_window.cpp
    ${SRC_DIR}/app/lenmus_zip_reader.cpp
)

set(AUXMUSIC_FILES
    ${SRC_DIR}/auxmusic/lenmus_cadence.cpp
    ${SRC_DIR}/auxmusic/lenmus_chord.cpp
    ${SRC_DIR}/auxmusic/lenmus_composer.cpp
    ${SRC_DIR}/auxmusic/lenmus_conversion.cpp
    ${SRC_DIR}/auxmusic/lenmus_interval.cpp
    ${SRC_DIR}/auxmusic/lenmus_scale.cpp
    ${SRC_DIR}/auxmusic/lenmus_utilities.cpp
)

set(DIALOGS_FILES
    ${SRC_DIR}/dialogs/lenmus_dlg_books.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_choose_lang.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_debug.cpp
)

set(EXERCISES_FILES
    ${SRC_DIR}/exercises/auxctrols/lenmus_counters_ctrol.cpp
    ${SRC_DIR}/exercises/auxctrols/lenmus_problem_displayer.cpp

    ${SRC_DIR}/exercises/constrains/lenmus_cadences_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_chords_constrains.cpp
    ${SRC_DIR}/exercises/constrains/lenmus_constrains.cpp
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
    ${SRC_DIR}/options/lenmus_toolbar_opt_panel.cpp
)

set(SOUND_FILES
    ${SRC_DIR}/sound/lenmus_metronome.cpp
    ${SRC_DIR}/sound/lenmus_midi_server.cpp
    ${SRC_DIR}/sound/lenmus_wave_player.cpp
)

set(XML_PARSER_FILES
    ${SRC_DIR}/xml_parser/lenmus_xml_parser.cpp
)

set(TEST_FILES
    ${SRC_DIR}/tests/lenmus_test_app.cpp
    ${SRC_DIR}/tests/lenmus_test_mainframe.cpp
    ${SRC_DIR}/tests/lenmus_test_runner.cpp
)

set(PACKAGES_FILES
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
)

set(ALL_SOURCES 
    ${APP_FILES} ${AUXMUSIC_FILES} ${DIALOGS_FILES} ${EXERCISES_FILES}
    ${GLOBALS_FILES} ${OPTIONS_FILES}
    ${SOUND_FILES} ${XML_PARSER_FILES} ${TEST_FILES} ${PACKAGES_FILES}
    ${UPDATER_FILES} ${WIDGETS_FILES}
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
source_group( "tests"           FILES ${TEST_FILES} )
source_group( "packages"        FILES ${PACKAGES_FILES} )
source_group( "updater"         FILES ${UPDATER_FILES} )
source_group( "widgets"         FILES ${WIDGETS_FILES} )


# add resource (RC) file for Windows
if ( MSVC )
    list( APPEND ALL_SOURCES    ${RESOURCES_DIR}/other/lenmus.rc )
    source_group( "resources"   FILES ${RESOURCES_DIR}/other/lenmus.rc )
endif()



if(0)
# Dirty hack: I don't know why it conflicts with LIBCD. But for
# now I need to continue. See:
# http://mygoldenchariot.blogspot.com/2006/07/link-warning-lnk4098-defaultlib-libcd.html
if(MSVC)
    set(CMAKE_LINKER_FLAGS_DEBUG
         ${CMAKE_LINKER_FLAGS_DEBUG} "/NODEFAULTLIB:LIBCD"
    )
endif()
endif(0)

#Executable
add_executable( ${LENMUS} ${ALL_SOURCES} )


# libraries to link
target_link_libraries ( ${LENMUS} 
            ${wxWidgets_LIBRARIES} ${PortMidi_LIBRARIES} ${PortTime_LIBRARIES}
            ${LOMSE_LIBRARIES} ${SQLite3_LIBRARIES} ${UNITTEST_LIBRARIES} 
)

if(0)
# properties
if(MSVC)
    set_target_properties(${TESTLIB} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCD" )
    set_target_properties(${TESTLIB} PROPERTIES LINK_FLAGS "/MACHINE:X86" )
endif()

# dependencies
add_dependencies(lenmus lomse)
endif(0)



#///////////////////////////////////////////////////////////////////////////////
# Program installation
# You can change the install location by running cmake like this:
#
#   cmake -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# By default, the prefix is "/usr/local"
#
# Remember:
#  ROOT_DIR: the root of the package source tree: i.e. $HOME/Downloads/lenmus-5.3
#  ROOT_INSTALL:  install location: i.e. /usr/local
#  ROOT_INSTALL_HOME: shared files i.e. /usr/local/share/lenmus/5.3/
#///////////////////////////////////////////////////////////////////////////////

    # lenmus program
install( TARGETS ${LENMUS} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )

    # 1. Software and essentials (ROOT_INSTALL_HOME):
    # ------------------------------------------------------------------------------

    # files in root folder
install(FILES  AUTHORS CHANGELOG COPYING INSTALL LICENSE
               NEWS README THANKS 
        DESTINATION ${ROOT_INSTALL_HOME} )

    # locale
        # ADD_LANG
install(FILES ${ROOT_DIR}/installer/msw/locale/license_en.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/installer/msw/locale/license_es.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/installer/msw/locale/license_fr.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/installer/msw/locale/license_tr.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/installer/msw/locale/license_nl.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/installer/msw/locale/license_eu.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/installer/msw/locale/license_it.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/installer/msw/locale/license_gl_ES.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
#install(FILES ${ROOT_DIR}/installer/msw/license_el_GR.txt   DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/docs/html/LICENSE_GNU_GPL_1.3.txt  DESTINATION "${ROOT_INSTALL_HOME}/locale")
install(FILES ${ROOT_DIR}/docs/html/LICENSE_GNU_FDL_1.3.txt  DESTINATION "${ROOT_INSTALL_HOME}/locale")
     
    # locale. Common folder
install(DIRECTORY ${ROOT_DIR}/locale/common  DESTINATION ${ROOT_INSTALL_HOME}/locale
        FILES_MATCHING PATTERN "*.*"   PATTERN ".svn" EXCLUDE )

    # locale. eBooks, help files and language catalogs for all languages
install(DIRECTORY  ${ROOT_DIR}/locale    DESTINATION "${ROOT_INSTALL_HOME}"
        FILES_MATCHING 
        PATTERN "*.hcc"
        PATTERN "*.hpp"
        PATTERN "*.htm"
        PATTERN "*.htb"
        PATTERN "*.lmb"
        PATTERN "*.po"
        PATTERN "*.mo"
        PATTERN "*.png"
        PATTERN "*.gif"
        PATTERN "*.jpg"
        PATTERN ".svn" EXCLUDE )
    
    # resources
install(DIRECTORY ${ROOT_DIR}/res/bitmaps   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE )
install(DIRECTORY ${ROOT_DIR}/res/icons   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE )
install(DIRECTORY ${ROOT_DIR}/res/sounds   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE )
install(DIRECTORY ${ROOT_DIR}/res/keys   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.png" PATTERN ".svn" EXCLUDE )
install(DIRECTORY ${ROOT_DIR}/res/cursors   DESTINATION ${ROOT_INSTALL_HOME}/res
        FILES_MATCHING PATTERN "*.png" PATTERN ".svn" EXCLUDE )
install(FILES ${ROOT_DIR}/res/fonts/lmbasic2.ttf  DESTINATION "${ROOT_INSTALL_HOME}/res/fonts")
install(FILES ${ROOT_DIR}/res/desktop/lenmus.desktop  DESTINATION "${ROOT_INSTALL_HOME}/res/desktop")

     # xrc
install(DIRECTORY ${ROOT_DIR}/xrc  DESTINATION ${ROOT_INSTALL_HOME}
        FILES_MATCHING PATTERN "*.xrc"   PATTERN ".svn" EXCLUDE)
     
     # /templates
install(DIRECTORY ${ROOT_DIR}/templates   DESTINATION ${ROOT_INSTALL_HOME}
        FILES_MATCHING PATTERN "*.lms"   PATTERN ".svn" EXCLUDE)

    # desktop and application icon 48x48 png
install(FILES ${ROOT_DIR}/res/desktop/lenmus.desktop  DESTINATION "/usr/share/applications")
install(FILES ${ROOT_DIR}/res/icons/lenmus.png  DESTINATION "/usr/share/pixmaps")


    # 4. User scores and samples 
    # ------------------------------------------------------------------------------
if( UNIX )
    set(ROOT_DATA_HOME $ENV{HOME}/${CMAKE_PROJECT_NAME} )
    set(ROOT_SAMPLES ${ROOT_INSTALL_HOME} )
elseif( WIN32 )
    set(ROOT_DATA_HOME ${ROOT_INSTALL} )
    set(ROOT_SAMPLES ${ROOT_DATA_HOME}/${LENMUS_VERSION_STRING} )
endif()
set(ROOT_SCORES ${ROOT_DATA_HOME}/scores )

message("ROOT_SCORES = " ${ROOT_SCORES} )
message("ROOT_SAMPLES = " ${ROOT_SAMPLES} )

install(DIRECTORY ${ROOT_DIR}/scores/samples  DESTINATION "${ROOT_SAMPLES}"
	    FILES_MATCHING PATTERN "*.lms"
	    PATTERN ".svn" EXCLUDE
	    PATTERN "test_set" EXCLUDE
)





#///////////////////////////////////////////////////////////////////////////////
# CPack section: installers generation
#///////////////////////////////////////////////////////////////////////////////
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cecilio Salmeron") #required
set(CPACK_DEBIAN_PACKAGE_NAME "lenmus")
set(CPACK_DEBIAN_PACKAGE_VERSION ${LENMUS_VERSION_STRING} )
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

set(CPACK_PACKAGE_VENDOR "LenMus")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A program for learning music theory")

set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${LENMUS_VERSION_STRING}-linux-amd64")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12)")


include(CPack)

