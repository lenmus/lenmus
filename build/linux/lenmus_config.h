//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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
// program name, version and related. To support store brands
//---------------------------------------------------------------------------------------
#define LENMUS_APP_NAME         "Lenmus Phonascus"
#define LENMUS_VENDOR_NAME      "Lenmus"
#define LENMUS_VENDOR_SITE      "http://www.lenmus.org"


//--------------------------------------------------------------------------------------
//  Paths and installation folders
//
//	LENMUS_SOURCE_ROOT	must always point to source tree root
//  LENMUS_INSTALL_ROOT root to install shared non-modificable files (Linux only).
//
//  In Debug build always use source tree. Binaries, config and logs in z_bin
//  In Release mode, it depends on LENMUS_RELEASE_INSTALL:
//
//  a) LENMUS_RELEASE_INSTALL == 0
//		Release build but include Debug menu and use source tree.
//		Binaries, config and logs in z_bin.
//
//	b) LENMUS_RELEASE_INSTALL == 1
//		Release build, to install or distribute.
//		Use install root. No debug menu.
//
//--------------------------------------------------------------------------------------
// current working directory is   /xxxx/build/linux
#define LENMUS_TEST_SCORES_PATH     "../scores/samples/test_set"
#define LENMUS_SOURCE_ROOT          "../../../trunk"
#define LENMUS_INSTALL_ROOT         "/usr/local/share/lenmus/5.2"   //Linux only


//---------------------------------------------------------------------------------------
// build type
//
// LENMUS_DEBUG_BUILD determines if it is a Debug or a Release build.
//
// For Debug buils, LENMUS_RELEASE_INSTALL determines if it is a build for testing or
// for installation/distribution:
//
// a) LENMUS_RELEASE_INSTALL == 0 means it is a Release build for testing.
//    The LENMUS_SOURCE_ROOT tree folders are used and debug menu is included.
//
// b) LENMUS_RELEASE_INSTALL == 1 means it is a Release build for
//    installation/distribution. No debug menu is included. In Linux, the
//    LENMUS_INSTALL_ROOT is used for shared non-modificable files and all others
//    go to /home folder. In Windows, the installation root is used.
//---------------------------------------------------------------------------------------
#define LENMUS_DEBUG_BUILD  1
#define LENMUS_RELEASE_INSTALL  0


//---------------------------------------------------------------------------------------
// platform and compiler
//---------------------------------------------------------------------------------------
#define LENMUS_PLATFORM_WIN32      0
#define LENMUS_PLATFORM_UNIX       1
#define LENMUS_PLATFORM_MAC        0
#define LENMUS_COMPILER_MSVC       0
#define LENMUS_COMPILER_GCC        1


#endif  // __LENMUS_CONFIG_H__
