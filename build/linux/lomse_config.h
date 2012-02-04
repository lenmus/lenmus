//---------------------------------------------------------------------------------------
//  This file is part of the Lomse library.
//  Copyright (c) 2010-2012 Cecilio Salmeron
//
//  Lomse is free software; you can redistribute it and/or modify it under the
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  Lomse is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with Lomse; if not, see <http://www.gnu.org/licenses/>.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#ifndef __LOMSE_CONFIG_H__
#define __LOMSE_CONFIG_H__

//==================================================================
// Template configuration file.
// Variables are replaced by CMake settings
//==================================================================


//---------------------------------------------------------------------------------------
// paths, for test scores and for fonts
//---------------------------------------------------------------------------------------
#define LOMSE_TEST_SCORES_PATH      "../../../lomse/test-scores/"
#define LOMSE_FONTS_PATH            "../../../lomse/fonts/"


//---------------------------------------------------------------------------------------
// library version, i.e.: 2.3
//---------------------------------------------------------------------------------------
#define LOMSE_VERSION_MAJOR     0
#define LOMSE_VERSION_MINOR     0


//---------------------------------------------------------------------------------------
// platform and compiler
//---------------------------------------------------------------------------------------
#define LOMSE_PLATFORM_WIN32      0
#define LOMSE_PLATFORM_UNIX       1
#define LOMSE_COMPILER_MSVC       0
#define LOMSE_COMPILER_GCC        1


//---------------------------------------------------------------------------------------
// what are you doing?
//    - creating the library as shared library   LOMSE_CREATE_DLL == 1
//    - using the library as shared library      LOMSE_USE_DLL == 1
//    - creating the library as static library   LOMSE_CREATE_DLL == 0
//    - using the library as static library      LOMSE_USE_DLL == 0
//---------------------------------------------------------------------------------------
#define LOMSE_CREATE_DLL    0
#define LOMSE_USE_DLL       0

//---------------------------------------------------------------------------------------
// build options
//---------------------------------------------------------------------------------------

// 1 - Debug build: include debug options
#define LOMSE_DEBUG                 1

// 1 - Accept without warning/error LDP v1.5 syntax
#define LOMSE_COMPATIBILITY_1_5     1


#endif  // __LOMSE_CONFIG_H__

