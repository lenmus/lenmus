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
            #Linux
		/usr/local/include
		/usr/include
		/sw/include
		/opt/local/include
		/usr/freeware/include
            #Windows
        $ENV{PortMidi_DIR}/include
        "C:/Program Files (x86)/portmidi/include"
        "C:/Program Files/portmidi/include"
)

find_library(PortMidi_LIBRARY
    NAMES
		portmidi
        libportmidi
    PATHS
            #Linux
		/usr/lib/
		/usr/local/lib
		/sw
		/usr/freeware
            #Windows
        $ENV{PortMidi_DIR}/lib					    
        "C:/Program Files (x86)/portmidi/lib"
        "C:/Program Files/portmidi/lib"
)

# On some distributions, such as Fedora, porttime is compiled into portmidi.
# On others, such as Debian, it is a separate library.
# Therefore we need to deal with this

set(INCLUDE_PORTTIME 1)  #assume porttime lib is needed. e.g. Windows
cmake_policy(SET CMP0057 NEW)   #do not ignore the IN_LIST operator
if (UNIX)

    #Determine Linux Distro
    find_program(LSB_RELEASE_EXEC lsb_release)
    if(NOT "${LSB_RELEASE_EXEC}" STREQUAL "LSB_RELEASE_EXEC-NOTFOUND")
        execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
            OUTPUT_VARIABLE   DISTRIBUTOR_ID
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()

    #TODO: Confirm Distributor ID for the different distros
    # confirmed:
    #   "CentOS", "Debian", "Fedora", "LinuxMint", "Ubuntu"
    # Unconfirmed values found googling on Internet:
    #   "archlinux", "Mageia", "openSUSE project",
    #   "RedHatEntrerpriseServer", "SUSE"
    message(STATUS "Linux Distribution ID =${DISTRIBUTOR_ID}")

    #porttime is included in portmidi package
    set(DISTROS_NOT_USING_PORTTIME
        Fedora
    )
    #porttime is an independent package
    set(DISTROS_USING_PORTTIME
        Debian
        LinuxMint
        Ubuntu
    )

    if("${DISTRIBUTOR_ID}" IN_LIST   DISTROS_NOT_USING_PORTTIME)
        set(INCLUDE_PORTTIME 0)   
    elseif("${DISTRIBUTOR_ID}" IN_LIST   DISTROS_USING_PORTTIME)
        set(INCLUDE_PORTTIME 1)   
    else()
        set(INCLUDE_PORTTIME 0)
        message(WARNING
            "Failed to identify your Linux distro: either lsb_release is missing " 
            "or the Distributor ID '${DISTRIBUTOR_ID}' is not included in this "
            "cmake script.\n"
            "It will be assumed that porttime library is included in portmidi "
            "package. The linkage step might fail, but if it does not fail, you "
            "will confirm that, in your distro, libporttime is not nedeed.\n"
            "Please send a PR with your findings for improving this cmake script. "
            "Thank you!"
        )
    endif()
endif(UNIX)


if (INCLUDE_PORTTIME)
    find_library(PortTime_LIBRARY
        NAMES
		    porttime
            libporttime
        PATHS
                #Linux
		    /usr/lib/
		    /usr/local/lib
		    /sw
		    /usr/freeware
                #Windows
            $ENV{PortMidi_DIR}/lib
            "C:/Program Files (x86)/portmidi/lib"
            "C:/Program Files/portmidi/lib"
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

