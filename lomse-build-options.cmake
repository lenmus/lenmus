# determine lomse version
# ---------------------------------

include( ${LOMSE_ROOT_DIR}/build-version.cmake )

add_custom_target (build-version ALL
  COMMAND ${CMAKE_COMMAND} -D LOMSE_ROOT_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/build-version.cmake
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "setting Lomse version information ...")


# set lomse build options and configure file lomse_config.h
#------------------------------------------------------------

# identify platform
if(WIN32 OR CYGWIN)
    # for Windows operating system or Windows when using the CygWin version of cmake
    set(LOMSE_PLATFORM_WIN32  "1")
    set(LOMSE_PLATFORM_UNIX   "0")
    set(LOMSE_PLATFORM_APPLE  "0")
elseif(APPLE)
    # for MacOS X or iOS, watchOS, tvOS (since 3.10.3)
    set(LOMSE_PLATFORM_WIN32  "0")
    set(LOMSE_PLATFORM_UNIX   "0")
    set(LOMSE_PLATFORM_APPLE  "1")
elseif(UNIX AND NOT APPLE AND NOT CYGWIN)
    # for Linux, BSD, Solaris, Minix
    set(LOMSE_PLATFORM_WIN32  "0")
    set(LOMSE_PLATFORM_UNIX   "1")
    set(LOMSE_PLATFORM_APPLE  "0")
endif()


# identify compiler
if(MSVC)
    set(LOMSE_COMPILER_MSVC  "1")
else()
    set(LOMSE_COMPILER_MSVC  "0")
endif()


# paths for tests
set(TESTLIB_SCORES_PATH     "\"${LOMSE_ROOT_DIR}/test-scores/\"" )
set(TESTLIB_FONTS_PATH      "\"${LOMSE_ROOT_DIR}/fonts/\"" )


# path to fonts (will be hardcoded in lomse library, so *MUST* be the
# path in which Lomse standard fonts will be installed)
set(LOMSE_FONTS_PATH   "\"${FONTS_PATH}/\"" )


# always create static library
set(LOMSE_USE_DLL "0")
set(LOMSE_CREATE_DLL "0")

# other options
set(LOMSE_DEBUG  "0")
set(LOMSE_COMPATIBILITY_LDP_1_5  "1")
set(LOMSE_ENABLE_DEBUG_LOGS  "0")
set(LOMSE_ENABLE_COMPRESSION  "1")
set(LOMSE_ENABLE_PNG  "1")


# instantiate the template to create file lomse_config.h with lomse configuration
configure_file(
    "${LOMSE_ROOT_DIR}/lomse_config.h.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lomse_config.h"
)


