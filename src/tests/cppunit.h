//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_CPPUNIT_H__
#define __LM_CPPUNIT_H__

#ifndef CPPUNIT_STD_NEED_ALLOCATOR
    #define CPPUNIT_STD_NEED_ALLOCATOR 0
#endif

// Set the default format for the errors, which can be used by an IDE to jump
// to the error location.
#ifndef CPPUNIT_COMPILER_LOCATION_FORMAT
    #define CPPUNIT_COMPILER_LOCATION_FORMAT "%p:%l:"
#endif

// Include all needed cppunit headers.
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

// for VC++ automatically link with the cppunit library
#ifdef __VISUALC__
  #ifdef NDEBUG
    #pragma comment(lib, "cppunit.lib")
  #else
    #pragma comment(lib, "cppunitd.lib")
  #endif
#endif

#endif      // __LM_CPPUNIT_H__

