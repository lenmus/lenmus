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
//  They are only used in local builds for development, debug and unit tests. But must
//  be always defined, and for all other build cases its value is ignored.
//
//  If relative, they are relative to the binary.
//  If absolute, for Windows, you can use any drive letter (e.g.: "C:") as it will be
//  always replaced by that of the binary executable .
//  
//  LENMUS_SOURCE_ROOT must point to source tree root.
//
//  LENMUS_TEST_SCORES_must point to the folder containing the scores used for
//  unit tests.
//
//--------------------------------------------------------------------------------------
#define LENMUS_TEST_SCORES_PATH     @LENMUS_TEST_SCORES_PATH@   //unit tests in local builds
#define LENMUS_SOURCE_ROOT          @LENMUS_SOURCE_ROOT@        //local builds only


//---------------------------------------------------------------------------------------
// Build type
//
//    LENMUS_DEBUG_BUILD
//        Determines if the debug menu and other debug options are included in the
//        build.
//
//    LENMUS_RELEASE_INSTALL
//        Determines if it is a build for testing or for installation/distribution:
//        - LENMUS_RELEASE_INSTALL == 0
//           Build for testing. The required resources (files, icons, etc.) will
//           be taken from local source tree.
//        - LENMUS_RELEASE_INSTALL == 1
//            Build for release. The required resources (files, icons, etc.) will
//            be taken from install roots and remote servers.
//
//    | LENMUS_     | LENMUS_         |
//    | DEBUG_BUILD | RELEASE_INSTALL |
//    +-------------+-----------------+----------------------------------------------
//    |      1      |       0         | Debug build, using source tree 
//    |      1      |       1         | Debug build for distribution (?)
//    +-------------+-----------------+----------------------------------------------
//    |      0      |       0         | Release build for testing, using source tree
//    |      0      |       1         | Release build for distribution
//    +-------------+-----------------+----------------------------------------------
//
//---------------------------------------------------------------------------------------
#define LENMUS_DEBUG_BUILD          @LENMUS_DEBUG_BUILD@
#define LENMUS_RELEASE_INSTALL      @LENMUS_RELEASE_INSTALL@


//---------------------------------------------------------------------------------------
// LENMUS_ENABLE_UNIT_TESTS
//    Determines if unit test are included in the build.
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
//
// LENMUS_ALSA_DEVICE
//      The ALSA audio device to use, such as: "hw:0", "plughw:1", etc.
//
// LENMUS_DSOUND_DEVICE
//      The DirectSound (Windows) device to use.
//
// LENMUS_COREAUDIO_DEVICE
//      The CoreAudio device to use.
//
// LENMUS_SOUNDFONT_PATH must point to the folder containing the FluidR3_GM.sf2 default
//      soundfont used by the internal synthesizer or must be "" when the soundfont
//      is included in the package/installer. It is only used in Linux but the
//      macro must be allways defined, even in Windows.
//
//---------------------------------------------------------------------------------------
#define LENMUS_AUDIO_DRIVER         "@LENMUS_AUDIO_DRIVER@"
#define LENMUS_ALSA_DEVICE          "@LENMUS_ALSA_DEVICE@"
#define LENMUS_DSOUND_DEVICE        "@LENMUS_DSOUND_DEVICE@"
#define LENMUS_COREAUDIO_DEVICE     "@LENMUS_COREAUDIO_DEVICE@"
#define LENMUS_SOUNDFONT_PATH       "@LENMUS_SOUNDFONT_PATH@"       //Linux only


#endif  // __LENMUS_CONFIG_H__
