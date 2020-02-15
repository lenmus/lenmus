//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2019 LenMus project
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
// program name, version and related. To support store brands
//---------------------------------------------------------------------------------------
#define LENMUS_APP_NAME         "@LENMUS_APP_NAME@"
#define LENMUS_VENDOR_NAME      "@LENMUS_VENDOR_NAME@"
#define LENMUS_VENDOR_SITE      "@LENMUS_VENDOR_SITE@"


//--------------------------------------------------------------------------------------
//  Paths and installation folders
//  
//  LENMUS_SOURCE_ROOT must always point to source tree root. It is only used during
//  program develpment and for unit tests.
//
//  LENMUS_INSTALL_ROOT must point to the folder in which the installer places the
//	lenmus executable. I've not found a simple way of determining this path at run time,
//  so it is simpler to have this information here.
//
//--------------------------------------------------------------------------------------
#define LENMUS_TEST_SCORES_PATH     @LENMUS_TEST_SCORES_PATH@
#define LENMUS_SOURCE_ROOT          @LENMUS_SOURCE_ROOT@
#define LENMUS_INSTALL_ROOT         "@LENMUS_INSTALL_ROOT@"         //Linux only


//---------------------------------------------------------------------------------------
// Build type
//
// LENMUS_DEBUG_BUILD determines if it is a Debug or a Release build.
//
// For Debug builds, LENMUS_RELEASE_INSTALL determines if it is a build for testing or
// for installation/distribution:
//
//  a) LENMUS_RELEASE_INSTALL == 0
//      Release build but include Debug menu.
//      Uses files from source tree.
//
//  b) LENMUS_RELEASE_INSTALL == 1
//      Release build, to install or distribute. No debug menu.
//      Uses install roots.
//
//---------------------------------------------------------------------------------------
#define LENMUS_DEBUG_BUILD          @LENMUS_DEBUG_BUILD@
#define LENMUS_RELEASE_INSTALL      @LENMUS_RELEASE_INSTALL@


//---------------------------------------------------------------------------------------
// include tests in build
//
// LENMUS_ENABLE_UNIT_TESTS determines if unit test are included in the build.
//
//---------------------------------------------------------------------------------------
#define LENMUS_ENABLE_UNIT_TESTS	@LENMUS_ENABLE_UNIT_TESTS@


//---------------------------------------------------------------------------------------
// platform and compiler
//---------------------------------------------------------------------------------------
#define LENMUS_PLATFORM_WIN32      @LENMUS_PLATFORM_WIN32@
#define LENMUS_PLATFORM_UNIX       @LENMUS_PLATFORM_UNIX@
#define LENMUS_PLATFORM_MAC        @LENMUS_PLATFORM_MAC@
#define LENMUS_COMPILER_MSVC       @LENMUS_COMPILER_MSVC@
#define LENMUS_COMPILER_GCC        @LENMUS_COMPILER_GCC@


//---------------------------------------------------------------------------------------
// Parameters to configure the internal audio synthesizer (FluidSynth)
//
// LENMUS_AUDIO_DRIVER
//      The audio driver to use: alsa (Linux), dsound (Windows), coreaudio (Mac OS X)
// LENMUS_ALSA_DEVICE 
//      The ALSA audio device to use, such as: "hw:0", "plughw:1", etc.
// LENMUS_DSOUND_DEVICE 
//      The DirectSound (Windows) device to use.
// LENMUS_COREAUDIO_DEVICE 
//      The CoreAudio device to use.
//
//---------------------------------------------------------------------------------------
#define LENMUS_AUDIO_DRIVER         @LENMUS_AUDIO_DRIVER@
#define LENMUS_ALSA_DEVICE          @LENMUS_ALSA_DEVICE@
#define LENMUS_DSOUND_DEVICE        @LENMUS_DSOUND_DEVICE@
#define LENMUS_COREAUDIO_DEVICE     @LENMUS_COREAUDIO_DEVICE@


#endif  // __LENMUS_CONFIG_H__
