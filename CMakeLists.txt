#---------------------------------------------------------------------------------------
#  This file is part of LenMus Phonascus program
#  Copyright (c) 2002-2011 LenMus project
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
#  This is a CMake configuration file for building makefiles and installfiles for
#  the LenMus Phonascus program
#
#  To use it you need CMake which can be downloaded from http://www.cmake.org/
#
#  This file has been tested with the following generators:
#   Windows:
#       Visual Studio 7 .NET
#       Visual Studio 2008
#   Linux:
#       Unix Makefiles
#       CodeBlocks - Unix Makefiles
#
#-------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)

# project name
project(lenmus)
set( LENMUS_APP_NAME     "\"Lenmus Phonascus\"" )
set( LENMUS_VENDOR_NAME  "\"Lenmus\"" )
set( LENMUS_VENDOR_SITE  "http://www.lenmus.org" )

# main directories 
set( ROOT_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )
set( SRC_DIR  ${ROOT_DIR}/src )
set( RESOURCES_DIR  ${ROOT_DIR}/res )
set( PACKAGES_DIR ${ROOT_DIR}/packages )
set( OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/bin )
set( EXECUTABLE_OUTPUT_PATH ${OUTDIR})


# directories to search for CMake modules
set( CMAKE_MODULE_PATH    "${ROOT_DIR}/cmake-modules" )


# set up configuration variables for lenmus_config.h
#------------------------------------------------------

# version
set( LENMUS_VERSION_MAJOR "5")
set( LENMUS_VERSION_MINOR "0")
set( LENMUS_VERSION_PATCH "0")
set( LENMUS_VERSION_STATE "\"a\"")

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
    set( LENMUS_DEBUG   "1")
else ()
    set( LENMUS_DEBUG   "0")
endif ()

# path to scores for tests
set( LENMUS_TEST_SCORES_PATH     "\"${ROOT_DIR}/scores/samples/test_set/\"" )
set( LENMUS_DBG_ROOT_PATH        "\"${ROOT_DIR}/\"" )
 

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
# http:#docs.wxwidgets.org/2.8/wx_librarieslist.html
set( wxWidgets_USE_LIBS  aui base gl net xml xrc core adv html) 
find_package(wxWidgets REQUIRED)
IF(wxWidgets_FOUND)
    #include_directories( ${wxWidgets_INCLUDE_DIRS} )
    INCLUDE(${wxWidgets_USE_FILE})
    IF(UNIX)
        set(wxWidgets_CXX_FLAGS "`wx-config --cflags -D__WXGTK__  -D_UNICODE`")     
    ENDIF(UNIX)
ENDIF(wxWidgets_FOUND)



# Check for Sqlite3
find_package(SQLite3 REQUIRED)
IF(SQLite3_FOUND)
    include_directories( ${SQLite3_INCLUDE_DIRS} )
ELSE(SQLite3_FOUND)
    message(STATUS "SQLite3 not found")
ENDIF(SQLite3_FOUND)
if(0)
include_directories("packages/wxSQLite3/include")
include_directories("packages/wxSQLite3/sqlite3/include")
endif(0)



#Check for PortMidi
find_package(PortMidi REQUIRED)
IF(PortMidi_FOUND)
    include_directories( ${PortMidi_INCLUDE_DIRS} )
ELSE(PortMidi_FOUND)
    message(STATUS "PortMidi not found")
ENDIF(PortMidi_FOUND)



# Check for Lomse
find_package( Lomse )                    
if( LOMSE_FOUND )
    include_directories( ${LOMSE_INCLUDE_DIRS} )
    link_libraries( ${LOMSE_LIBRARIES} )
else()
    message(SEND_ERROR "Lomse package not found.")
endif()



# Check for Boost >= 1.42.0
set(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.43" "1.43.0")
set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED OFF)
#set(Boost_DEBUG TRUE)
find_package( Boost 1.42.0 COMPONENTS date_time thread )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries( ${Boost_LIBRARIES} )
    link_directories( ${Boost_LIBRARY_DIRS} )
    message("Boost libraries: ${Boost_LIBRARIES}" )
else()
    message(SEND_ERROR "Boost package not found. Reason: ${Boost_ERROR_REASON}" )
endif()



# Check for FreeType: header files required by some lomse headers
find_package( Freetype )                    
if( FREETYPE_FOUND )
    include_directories( ${FREETYPE_INCLUDE_DIRS} )
    link_libraries( ${FREETYPE_LIBRARIES} )
    link_directories( ${FREETYPE_LINK_DIR} )        
else()
    message(SEND_ERROR "FreeType package not found.")
endif()



message(STATUS "CMAKE_MODULE_PATH => " ${CMAKE_MODULE_PATH})
message(STATUS "wxWidgets => " ${wxWidgets_LIBRARIES})
message(STATUS "LOMSE_INCLUDE_DIRS => " ${LOMSE_INCLUDE_DIRS} )
message(STATUS "LOMSE_LIBRARIES => " ${LOMSE_LIBRARIES} )
message(STATUS "PortMidi_INCLUDE_DIRS => " ${PortMidi_INCLUDE_DIRS} )
message(STATUS "PortMidi_LIBRARIES => " ${PortMidi_LIBRARIES} )
#message(STATUS "FREETYPE => " ${FREETYPE_LIBRARIES})
message(STATUS "SQLite3_LIBRARIES => " ${SQLite3_LIBRARIES})
message(STATUS "SQLite3_INCLUDE_DIRS => " ${SQLite3_INCLUDE_DIRS})
message(STATUS "Boost_LIBRARIES => " ${Boost_LIBRARIES})
message(STATUS "Boost_LIBRARY_DIRS => " ${Boost_LIBRARY_DIRS})
message(STATUS "Boost_INCLUDE_DIRS => " ${Boost_INCLUDE_DIRS})





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
    ${SRC_DIR}/app/lenmus_document_frame.cpp
    ${SRC_DIR}/app/lenmus_events.cpp
    ${SRC_DIR}/app/lenmus_logger.cpp
    ${SRC_DIR}/app/lenmus_main_frame.cpp
    ${SRC_DIR}/app/lenmus_midi_wizard.cpp
    ${SRC_DIR}/app/lenmus_preferences.cpp
    ${SRC_DIR}/app/lenmus_score_canvas.cpp
    ${SRC_DIR}/app/lenmus_splash_frame.cpp
    ${SRC_DIR}/app/lenmus_string.cpp
    ${SRC_DIR}/app/lenmus_welcome_window.cpp
    ${SRC_DIR}/app/lenmus_zip_reader.cpp
)

set(AUXMUSIC_FILES
    ${SRC_DIR}/auxmusic/lenmus_cadence.cpp
    ${SRC_DIR}/auxmusic/lenmus_chord.cpp
    ${SRC_DIR}/auxmusic/lenmus_composer.cpp
    ${SRC_DIR}/auxmusic/lenmus_conversion.cpp
    ${SRC_DIR}/auxmusic/lenmus_harmony.cpp
    ${SRC_DIR}/auxmusic/lenmus_harmony_exercises_data.cpp
    ${SRC_DIR}/auxmusic/lenmus_interval.cpp
    ${SRC_DIR}/auxmusic/lenmus_scale.cpp
    ${SRC_DIR}/auxmusic/lenmus_scales_manager.cpp
    ${SRC_DIR}/auxmusic/lenmus_utilities.cpp
)

set(DIALOGS_FILES
    ${SRC_DIR}/dialogs/lenmus_dlg_choose_lang.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_counters.cpp
    ${SRC_DIR}/dialogs/lenmus_dlg_debug.cpp
)

set(EXERCISES_FILES
    ${SRC_DIR}/exercises/auxctrols/lenmus_counters_ctrol.cpp
    ${SRC_DIR}/exercises/auxctrols/lenmus_problem_display.cpp

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
    ${SRC_DIR}/exercises/ctrols/lenmus_score_ctrol.cpp
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

    ${SRC_DIR}/exercises/params/lenmus_exercise_params.cpp
)

set(GLOBALS_FILES
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
    ${SRC_DIR}/sound/lenmus_midi_server.cpp
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
)


set(ALL_SOURCES 
    ${APP_FILES} ${AUXMUSIC_FILES} ${DIALOGS_FILES} ${EXERCISES_FILES}
    ${GLOBALS_FILES} ${OPTIONS_FILES}
    ${SOUND_FILES} ${XML_PARSER_FILES} ${TEST_FILES} ${PACKAGES_FILES}
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


# libraries to link     ${SQLite3_LIBRARIES} wxsqlite3
target_link_libraries ( ${LENMUS} 
            ${wxWidgets_LIBRARIES} ${PortMidi_LIBRARIES} ${PortTime_LIBRARIES}
            ${LOMSE_LIBRARIES} ${UNITTEST_LIBRARIES} 
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
# Installation folders are organized into four groups
#       1. Software and essentials
#       2. Logs and temporal files
#       3. Configuration files, user dependent
#       4. User scores and samples
#
# Only files in the four group can be configured by the user
#
# For Windows all folders follow the working copy structure, but for Linux
# files go to diferent places. See following table:
#
# ------------------------------------------------------------------------------
#      Linux                             Windows
#    Default <prefix> = /usr/local
#
# 0. The lenmus program
# ------------------------------------------------------------------------------
#      <prefix>                          lenmus
#          + /bin                            + \bin
#
# 1. Software and essentials (ROOT_G1):
# ------------------------------------------------------------------------------
#      <prefix>/share/lenmus             lenmus
#          + /xrc                            + \xrc
#          + /res                            + \res
#          + /locale                         + \locale
#          + /books                          + \books
#          + /templates                      + \templates
#          + /test-scores                    + \test-scores
#
# 2. Logs and temporal files:
# ------------------------------------------------------------------------------
#                                        lenmus
# logs:    ~/.lenmus/logs                    + \logs
# temp:    /tmp/lenmus                       + \temp
#
# 3. Configuration files (user dependent):
# ------------------------------------------------------------------------------
#      ~/.lenmus                         lenmus\bin
#
# 4. User scores and samples:
# ------------------------------------------------------------------------------
#      ~/lenmus/scores                   lenmus\scores
#
#///////////////////////////////////////////////////////////////////////////////
if(0)

message("CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX} )
set(ROOT_INSTALL ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME} )

if( UNIX )
    set(ROOT_G1 ${ROOT_INSTALL}/share/lenmus )
    set(ROOT_G2 ${ROOT_INSTALL}/${CMAKE_PROJECT_NAME} )
    set(ROOT_G3 ${ROOT_INSTALL}/${CMAKE_PROJECT_NAME} )
    set(ROOT_G4 ${ROOT_INSTALL}/${CMAKE_PROJECT_NAME} )
elseif( WIN32 )
    set(ROOT_G1 ${ROOT_INSTALL} )
    set(ROOT_G2 ${ROOT_INSTALL}/${CMAKE_PROJECT_NAME} )
    set(ROOT_G3 ${ROOT_INSTALL}/${CMAKE_PROJECT_NAME} )
    set(ROOT_G4 ${ROOT_INSTALL}/${CMAKE_PROJECT_NAME} )
endif()

# files in root folder
install(FILES  AUTHORS ChangeLog COPYING INSTALL INSTALL-CMAKE LICENSE
               NEWS README THANKS 
        DESTINATION ${ROOT_INSTALL} )

# lenmus program
install( TARGETS ${LENMUS} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )

# Software and essentials
install(DIRECTORY ${INCLUDES_LOMSE}/
        DESTINATION ${ROOT_G1}
        FILES_MATCHING PATTERN "*.h"
        PATTERN ".svn" EXCLUDE
        PATTERN "platform" EXCLUDE )

# copy agg includes
install(DIRECTORY "${INCLUDES_AGG}/"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lomse/agg"
        FILES_MATCHING PATTERN "*.h"
        PATTERN ".svn" EXCLUDE )

# copy also lomse_config.h
install(FILES  ${CMAKE_CURRENT_BINARY_DIR}/lomse_config.h 
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lomse" )

endif(0)
 


if(0)
#///////////////////////////////////////////////////////////////////////////////
# CPack section: installers generation
#///////////////////////////////////////////////////////////////////////////////

set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES
    /[.]svn/ /.*[.]o$ /.*[.]o/ /autom4te[.]cache/ /[.].* /bootstrap 
    /RELEASE /styles/ /papers/ /rpm.am /msi.am )
set(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
set(CPACK_SOURCE_PACKAGE_FILE_NAME
    ${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION})
    
set(CPACK_PACKAGE_VENDOR "LenMus")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A program for learning music")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "lenmus")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README")

set(CPACK_NSIS_HELP_LINK "http://www.lenmus.org")
set(CPACK_NSIS_URL_INFO_ABOUT "http://www.lenmus.org")
set(CPACK_NSIS_CONTACT "cecilios@lists.sourceforge.net")

include(CPack)
 
endif(0)

