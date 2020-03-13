# - Try to find PortMidi
# Once done, this will define
#
#  PortMidi_FOUND - system has PortMidi
#  PortMidi_INCLUDE_DIRS - the PortMidi include directories
#  PortMidi_LIBRARIES - link these to use PortMidi
#  PortMidi_VERSION - detected version of PortMidi
#  PortMidi_LIBDIR - directory for libraries
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

include(LibFindMacros)

find_path(PortMidi_INCLUDE_DIR
	NAMES
		portmidi.h
		porttime.h
    PATHS
		/usr/local/include
		/usr/include
		/sw/include
		/opt/local/include
		/usr/freeware/include
        $ENV{PortMidi_DIR}/include					#Windows
        "C:/Program Files (x86)/portmidi/include"		#Windows
        "C:/Program Files/portmidi/include"				#Windows
)

find_library(PortMidi_LIBRARY
    NAMES
		portmidi
        libportmidi
    PATHS
		/usr/lib/
		/usr/local/lib
		/sw
		/usr/freeware
        $ENV{PortMidi_DIR}/lib					#Windows
        "C:/Program Files (x86)/portmidi/lib"		#Windows
        "C:/Program Files/portmidi/lib"				#Windows
)

# On some distributions, such as Fedora, porttime is compiled into portmidi.
# On others, such as Debian, it is a separate library.
# Therefore we need to deal with this

#Determine Linux Distro
find_program(LSB_RELEASE_EXEC lsb_release)
execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
    OUTPUT_VARIABLE   DISTRIBUTOR_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(INCLUDE_PORTTIME 1)  #assume it is needed. e.g. Windows
if (UNIX)
    #determine distribution
    #TODO: Confirm Distributor ID the different distros
    # from Internet:
    #   "CentOS"
    #   "Debian"
    #   "RedHatEntrerpriseServer"
    #   "openSUSE project"
    #   "Ubuntu"
    # confirmed:
    #   "LinuxMint"
    if (("${DISTRIBUTOR_ID}" STREQUAL "Fedora") OR      #confirmed
        ("${DISTRIBUTOR_ID}" STREQUAL "Gentoo")         #not sure
       )
        set(INCLUDE_PORTTIME "0")   #porttime is included in portmidi

    #else
    #   Confirmed: Distribution_IDs that package porttime in a separate package
    #       "Fedora"
    endif()
endif(UNIX)
message(STATUS "Linux DISTRIBUTOR_ID: ${DISTRIBUTOR_ID}")


if (INCLUDE_PORTTIME)
    find_library(PortTime_LIBRARY
        NAMES
		    porttime
        PATHS
		    /usr/lib/
		    /usr/local/lib
		    /sw
		    /usr/freeware
            $ENV{PortMidi_DIR}/lib					#Windows
            "C:/Program Files (x86)/portmidi/lib"		#Windows
            "C:/Program Files/portmidi/lib"				#Windows
    )
    set( PortMidi_LIBRARIES   "${PortMidi_LIBRARY};${PortTime_LIBRARY}" )
else()
    set( PortMidi_LIBRARIES   "${PortMidi_LIBRARY}" )
endif()

# set the user variables
set( PortMidi_INCLUDE_DIRS   "${PortMidi_INCLUDE_DIR}" )
if( PortMidi_INCLUDE_DIR AND PortMidi_LIBRARY)
    set( PortMidi_FOUND   true )
else()
    set( PortMidi_FOUND   false )
endif()

# handle the QUIETLY and REQUIRED arguments and set PortMidi_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PortMidi  DEFAULT_MSG  PortMidi_LIBRARY  PortMidi_INCLUDE_DIRS)

# show the PortMidi_INCLUDE_DIRS and PortMidi_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(PortMidi_LIBRARY PortMidi_INCLUDE_DIR)

