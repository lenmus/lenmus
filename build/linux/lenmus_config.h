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
// Variables are replaced by CMake settings
//==================================================================


//---------------------------------------------------------------------------------------
// Path for lenmus root, while developing the program.
// When properly instaled, the lenmus binary is in /bin folder and all tree
// structure can be derived from it. But during development, the lenmus executable
// could be in any other folder. Therefore, this configuration macro sets the
// lenmus root folder
//---------------------------------------------------------------------------------------
#define LENMUS_TEST_SCORES_PATH      "../../../lenmus/scores/samples/test_set"
#define LENMUS_DBG_ROOT_PATH         "../../../lenmus"


//---------------------------------------------------------------------------------------
// program name, version and related
//---------------------------------------------------------------------------------------
#define LENMUS_APP_NAME      	"Lenmus Phonascus"
#define LENMUS_VENDOR_NAME		"Lenmus"
#define LENMUS_VENDOR_SITE		"http://www.lenmus.org"

#define LENMUS_VERSION_MAJOR     5
#define LENMUS_VERSION_MINOR     0
#define LENMUS_VERSION_STATE     "a"
#define LENMUS_VERSION_PATCH     0

#define LENMUS_DEBUG 	1


//---------------------------------------------------------------------------------------
// platform and compiler
//---------------------------------------------------------------------------------------
#define LENMUS_PLATFORM_WIN32      0
#define LENMUS_PLATFORM_UNIX       1
#define LENMUS_PLATFORM_MAC        0
#define LENMUS_COMPILER_MSVC       0
#define LENMUS_COMPILER_GCC        1




#endif  // __LENMUS_CONFIG_H__

