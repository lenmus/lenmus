//--------------------------------------------------------------------------------------
//  This file is part of LenMus project.
//  Copyright (c) 2002-2011 Cecilio Salmeron
//
//  LenMus is free software; you can redistribute it and/or modify it under the
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  langtool is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with langtool; if not, see <http://www.gnu.org/licenses/>.
//  
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LANGTOOL_CONFIG_H__
#define __LANGTOOL_CONFIG_H__

//==================================================================
// Template configuration file.
// Variables are replaced by CMake settings
//==================================================================


//---------------------------------------------------------------------------------------
// platform and compiler
//---------------------------------------------------------------------------------------
#define LANGTOOL_PLATFORM_WIN32      @LANGTOOL_PLATFORM_WIN32@
#define LANGTOOL_PLATFORM_UNIX       @LANGTOOL_PLATFORM_UNIX@
#define LANGTOOL_COMPILER_MSVC       @LANGTOOL_COMPILER_MSVC@
#define LANGTOOL_COMPILER_GCC        @LANGTOOL_COMPILER_GCC@
       

#endif  // __LANGTOOL_CONFIG_H__

