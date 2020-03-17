#===============================================================================
# Try to determine Linux flavour and right CPACK Generator
#===============================================================================
#
# The following variables will be set:
#
#   PACKAGE_TYPE:
#   - For Windows systems will contain NSIS
#   - For Unix systems will contain DEB, RPM or TGZ
#       TODO: other CPACK generators: apk, txz, xz, zst
#
#   SYSTEM_TYPE:
#   - For Windows systems will contain "Windows"
#   - For Linux systems will contain the distro type:
#       CentOS, Debian, Fedora, LinuxMint, Mageia, openSUSE, Scientific, SuSE,
#       Ubuntu
#        TODO: Set/confirm values for other distros
#   - For Unix systems other than Linux will contain the value of cmake 
#     variable CMAKE_SYSTEM_NAME. It is the result of command `uname -s`:
#		AIX, BSD/OS, CYGWIN_NT-5.1, Darwin, FreeBSD, GNU/kFreeBSD, HP-UX,
#		NetBSD, OpenBSD, OSF1, SCO_SV, UnixWare, UNIX_SV, Xenix, SunOS, Tru64,
#       ULTRIX
#
#
#  Copyright (c) 2020 Cecilio Salmeron
#  Licensed under MIT license.
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0057 NEW)   #do not ignore the IN_LIST operator

message(STATUS "Determining Linux distro script:")

if(WIN32)
    set(SYSTEM_TYPE "")
    set(PACKAGE_TYPE "NSIS")

elseif(UNIX)

    set(SYSTEMS_USING_RPM
        CentOS
        Fedora
        Mageia
        openSUSE
        Scientific
        SuSE
    )
    set(SYSTEMS_USING_DEB
        Debian
        LinuxMint
        Ubuntu
    )
	
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

        # Try lsb_release to determine Linux distro
        find_program(LSB_RELEASE_EXEC lsb_release)
        if(NOT "${LSB_RELEASE_EXEC}" STREQUAL "LSB_RELEASE_EXEC-NOTFOUND")
            execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
                OUTPUT_VARIABLE  SYSTEM_TYPE
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            #string(TOLOWER "${SYSTEM_TYPE}" SYSTEM_TYPE)

            #TODO: Confirm Distributor ID for the different distros
            # confirmed values:
            #   "CentOS", "Debian", "Fedora", "LinuxMint", "Ubuntu"
            # unconfirmed values found by googling:
            #   "archlinux", "Mageia", "openSUSE project",
            #   "RedHatEntrerpriseServer", "SUSE"
            message(STATUS "    Linux Distribution ID =${SYSTEM_TYPE}")

            if (SYSTEM_TYPE MATCHES "opensuse.*" OR SYSTEM_TYPE MATCHES "suse.*" OR SYSTEM_TYPE MATCHES "sles.*")
                set(SYSTEM_TYPE "SuSE")
            endif()
        endif()

        # If lsb_release not found or does not give expected results try
        # to use /etc/*-release files to determine Linux distro
        if ("xz${SYSTEM_TYPE}zx" STREQUAL "xzzx")
            if (EXISTS "/etc/debian_version")
               set(SYSTEM_TYPE "Debian based")
            elseif (EXISTS "/etc/fedora-release")
               set(SYSTEM_TYPE "Fedora")
            elseif (EXISTS "/etc/gentoo-release")
               set(SYSTEM_TYPE "Gentoo")
            elseif (EXISTS "/etc/mandrake-release")
               set(SYSTEM_TYPE "Mandrake")
            elseif (EXISTS "/etc/SuSE-release")
               set(SYSTEM_TYPE "SuSE based")
            elsif (EXISTS "/etc/redhat-release")
               set(SYSTEM_TYPE "Redhat based")
            elseif (EXISTS "/etc/slackware-release")
               set(SYSTEM_TYPE "Slackware")
            elseif (EXISTS "/etc/slamd64-version")
               set(SYSTEM_TYPE "Slamd64")
            elseif (EXISTS "/etc/release")
               set(SYSTEM_TYPE "Solaris/Sparc")
            elseif (EXISTS "/etc/sun-release")
               set(SYSTEM_TYPE "Sun JDS")
            else()
                message(WARNING
                    "Failed to identify your Linux distro. Either:\n"
                    "a) lsb_release is missing or the Distributor ID value '${SYSTEM_TYPE}' "
                    "provided by lsb_release is not included in this cmake script; or\n"
                    "b) no suitable file /etc/*-release has been found or it is not "
                    "included in this cmake script.\n"
                    "Please open an issue with your findings for improving this cmake "
                    "script at https://github.com/lenmus/lenmus/issues. "
                    "Thank you!")
                set(SYSTEM_TYPE "unknown")
            endif()
        endif()

    else()  # unix but not linux
        set(SYSTEM_TYPE ${CMAKE_SYSTEM_NAME})
    endif()


    # determine package generator
    # ----------------------------
    if("${SYSTEM_TYPE}" IN_LIST   SYSTEMS_USING_RPM)
        set(PACKAGE_TYPE "RPM")   
    elseif("${SYSTEM_TYPE}" IN_LIST   SYSTEMS_USING_DEB)
        set(PACKAGE_TYPE "DEB")   
    else()
        set(PACKAGE_TYPE "TGZ")
    endif()


endif()	

message(STATUS "    SYSTEM_TYPE=${SYSTEM_TYPE}")
message(STATUS "    PACKAGE_TYPE=${PACKAGE_TYPE}")

