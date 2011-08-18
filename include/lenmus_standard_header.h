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

#ifndef __LENMUS_DEFINES_H__
#define __LENMUS_DEFINES_H__

#include "lenmus_config.h"


//---------------------------------------------------------------------------------------
// for detecting and isolating memory leaks with Visual C++ in Debug builds

#if ( LENMUS_COMPILER_MSVC == 1 ) && ( LENMUS_DEBUG == 1 )
    #ifndef _DEBUG
        #define _DEBUG
    #endif
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
    #include <wx/memory.h>
    #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define new DEBUG_NEW

#else
    #define DEBUG_NEW new
#endif


//    // Now we'll include any OS headers we need.
//// At this point we are outside the LenMus namespace.
//
//#if LENMUS_MSVC
//  #if LENMUS_VC6
//    #pragma warning (disable: 4284 4786)  // (spurious VC6 warnings)
//
//    namespace std   // VC6 doesn't have sqrt/sin/cos/tan/abs in std, so declare them here:
//    {
//        template <typename Type> Type abs (Type a)              { if (a < 0) return -a; return a; }
//        template <typename Type> Type tan (Type a)              { return static_cast<Type> (::tan (static_cast<double> (a))); }
//        template <typename Type> Type sin (Type a)              { return static_cast<Type> (::sin (static_cast<double> (a))); }
//        template <typename Type> Type cos (Type a)              { return static_cast<Type> (::cos (static_cast<double> (a))); }
//        template <typename Type> Type sqrt (Type a)             { return static_cast<Type> (::sqrt (static_cast<double> (a))); }
//        template <typename Type> Type floor (Type a)            { return static_cast<Type> (::floor (static_cast<double> (a))); }
//        template <typename Type> Type ceil (Type a)             { return static_cast<Type> (::ceil (static_cast<double> (a))); }
//        template <typename Type> Type atan2 (Type a, Type b)    { return static_cast<Type> (::atan2 (static_cast<double> (a), static_cast<double> (b))); }
//    }
//  #endif
//
//  #pragma warning (push)
//  #pragma warning (disable: 4514 4245 4100)
//#endif
//
//#include <cstdlib>
//#include <cstdarg>
//#include <climits>
//#include <limits>
//#include <cmath>
//#include <cwchar>
//#include <stdexcept>
//#include <typeinfo>
//#include <cstring>
//#include <cstdio>
//#include <iostream>
//#include <vector>
//
//#if LENMUS_USE_INTRINSICS
//  #include <intrin.h>
//#endif
//
//#if LENMUS_MAC || LENMUS_IOS
//  #include <libkern/OSAtomic.h>
//#endif
//
//#if LENMUS_LINUX
//  #include <signal.h>
//
//  #if __INTEL_COMPILER
//    #if __ia64__
//      #include <ia64intrin.h>
//    #else
//      #include <ia32intrin.h>
//    #endif
//  #endif
//#endif
//
//#if LENMUS_MSVC && LENMUS_DEBUG
//  #include <crtdbg.h>
//#endif
//
//#if LENMUS_MSVC
//  #include <malloc.h>
//  #pragma warning (pop)
//
//  #if ! LENMUS_PUBLIC_INCLUDES
//    #pragma warning (4: 4511 4512 4100)  // (enable some warnings that are turned off in VC8)
//  #endif
//#endif
//
//
////==============================================================================
//// DLL building settings on Win32
//#if LENMUS_MSVC
//  #ifdef LENMUS_DLL_BUILD
//    #define LENMUS_API __declspec (dllexport)
//    #pragma warning (disable: 4251)
//  #elif defined (LENMUS_DLL)
//    #define LENMUS_API __declspec (dllimport)
//    #pragma warning (disable: 4251)
//  #endif
//  #ifdef __INTEL_COMPILER
//   #pragma warning (disable: 1125) // (virtual override warning)
//  #endif
//#elif defined (__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
//  #ifdef LENMUS_DLL_BUILD
//    #define LENMUS_API __attribute__ ((visibility("default")))
//  #endif
//#endif
//
//
////==============================================================================
//// Now include some basics that are needed by most of the LenMus classes...
//BEGIN_LENMUS_NAMESPACE
//
//extern LENMUS_API bool LENMUS_CALLTYPE juce_isRunningUnderDebugger();
//
//#if LENMUS_LOG_ASSERTIONS
//  extern LENMUS_API void juce_LogAssertion (const char* filename, int lineNum) noexcept;
//#endif
//
//#include "../memory/juce_Memory.h"
//#include "../maths/juce_MathsFunctions.h"
//#include "../memory/juce_ByteOrder.h"
//#include "juce_Logger.h"
//#include "../memory/juce_LeakedObjectDetector.h"
//
//#undef TYPE_BOOL  // (stupidly-named CoreServices definition which interferes with other libraries).
//
//END_LENMUS_NAMESPACE


#endif   // __LENMUS_DEFINES_H__
