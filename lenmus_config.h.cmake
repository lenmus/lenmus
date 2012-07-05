//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_CONFIG_H__
#define __LENMUS_CONFIG_H__

//==================================================================
// Template configuration file.
// Variables are replaced by CMake, depending on build settings
//==================================================================


//---------------------------------------------------------------------------------------
// Path for lenmus root, while developing the program.
// When properly instaled, the lenmus binary is in /bin folder and all tree
// structure can be derived from it. But during development, the lenmus executable
// could be in any other folder. Therefore, this configuration macro sets the
// lenmus root folder
//---------------------------------------------------------------------------------------
#define LENMUS_TEST_SCORES_PATH      @LENMUS_TEST_SCORES_PATH@
#define LENMUS_DBG_ROOT_PATH         @LENMUS_DBG_ROOT_PATH@


//---------------------------------------------------------------------------------------
// program name, version and related
//---------------------------------------------------------------------------------------
#define LENMUS_APP_NAME         @LENMUS_APP_NAME@
#define LENMUS_VENDOR_NAME      @LENMUS_VENDOR_NAME@
#define LENMUS_VENDOR_SITE      @LENMUS_VENDOR_SITE@

#define LENMUS_VERSION_MAJOR     @LENMUS_VERSION_MAJOR@
#define LENMUS_VERSION_MINOR     @LENMUS_VERSION_MINOR@
#define LENMUS_VERSION_STATE     @LENMUS_VERSION_STATE@
#define LENMUS_VERSION_PATCH     @LENMUS_VERSION_PATCH@


//---------------------------------------------------------------------------------------
// build type and options
//
// Variable LENMUS_DEBUG_MENU set to 1 means that the 'Debug' menu will be included.
//---------------------------------------------------------------------------------------
#define LENMUS_DEBUG_BUILD      @LENMUS_DEBUG_BUILD@
#define LENMUS_DEBUG_MENU       @LENMUS_DEBUG_MENU@


//---------------------------------------------------------------------------------------
// platform and compiler
//---------------------------------------------------------------------------------------
#define LENMUS_PLATFORM_WIN32      @LENMUS_PLATFORM_WIN32@
#define LENMUS_PLATFORM_UNIX       @LENMUS_PLATFORM_UNIX@
#define LENMUS_PLATFORM_MAC        @LENMUS_PLATFORM_MAC@
#define LENMUS_COMPILER_MSVC       @LENMUS_COMPILER_MSVC@
#define LENMUS_COMPILER_GCC        @LENMUS_COMPILER_GCC@


//---------------------------------------------------------------------------------------
// installation folders
//
// Variable LENMUS_IS_TEST_INSTALL set to 1 means it is a test installation, that is, all
// needed folders are in the source tree. Variable LENMUS_IS_TEST_INSTALL set to 0
// means a normal installation, implying that needed folders are defined by variables 
// LENMUS_INSTALL_HOME, LENMUS_CONFIG_HOME, LENMUS_DATA_HOME and LENMUS_LOGS_HOME
//---------------------------------------------------------------------------------------
#define LENMUS_IS_TEST_INSTALL    @LENMUS_IS_TEST_INSTALL@ 

#define LENMUS_INSTALL_HOME     @LENMUS_INSTALL_HOME@
#define LENMUS_CONFIG_HOME      @LENMUS_CONFIG_HOME@
#define LENMUS_DATA_HOME        @LENMUS_DATA_HOME@
#define LENMUS_LOGS_HOME        @LENMUS_LOGS_HOME@




#endif  // __LENMUS_CONFIG_H__

