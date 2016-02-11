#-------------------------------------------------------------------------------------
# This is part of CMake configuration file for building makefiles and installfiles
# for the LenMus Phonascus program
#-------------------------------------------------------------------------------------
# This module is for getting information from Git repository and composing
# LenMus version string
#
# The following variables are defined (with example of value):
#   LENMUS_PACKAGE_VERSION   0.17.36
#   LENMUS_VERSION_MAJOR     0
#   LENMUS_VERSION_MINOR     17
#   LENMUS_VERSION_PATCH     36
#   LENMUS_VERSION_SHA1      a1b2c3f
#
#-------------------------------------------------------------------------------------

find_package (Git)
if (NOT GIT_FOUND)
    message(SEND_ERROR "Git package not found." )
endif()

# get description
execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags
	            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	            RESULT_VARIABLE res
	            OUTPUT_VARIABLE out
	            OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT res EQUAL 0)
    message("Error ${res} in 'git describe' command. Out: ${out}")
	set(DESCRIPTION "Release_0.0-0-gERROR")
else()
    set(DESCRIPTION "${out}")
endif()
#set (DESCRIPTION "Release_5.4.2-7-gabcdef2")
#set (DESCRIPTION "Release_0.0-0-gERROR")
#set (DESCRIPTION "Release_5.5-7-gabcdef2")
message(STATUS "git description is: ${DESCRIPTION}")


# extract components from description string
string(REGEX REPLACE "^Release_([0-9]+)\\..*" "\\1" LENMUS_VERSION_MAJOR "${DESCRIPTION}")
string(REGEX REPLACE "^Release_[0-9]+\\.([0-9]+).*" "\\1" LENMUS_VERSION_MINOR "${DESCRIPTION}")
string(REGEX REPLACE "^Release_[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" LENMUS_VERSION_PATCH "${DESCRIPTION}")
if ("${LENMUS_VERSION_PATCH}" STREQUAL "${DESCRIPTION}")
    # ok. New tag format X.Y
    string(REGEX REPLACE "^Release_[0-9]+\\.[0-9]+\\-([0-9]+).*" "\\1" LENMUS_VERSION_PATCH "${DESCRIPTION}")
    string(REGEX REPLACE "^Release_[0-9]+\\.[0-9]+\\-[0-9]+\\-g(.*)" "\\1" LENMUS_VERSION_SHA1 "${DESCRIPTION}")
else()
    # Old tag format X.Y.Z. Ignore Z
    string(REGEX REPLACE "^Release_[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+).*" "\\1" LENMUS_VERSION_PATCH "${DESCRIPTION}")
    string(REGEX REPLACE "^Release_[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-g(.*)" "\\1" LENMUS_VERSION_SHA1 "${DESCRIPTION}")
endif()
message(STATUS "LenMus version. Major:${LENMUS_VERSION_MAJOR}, Minor:${LENMUS_VERSION_MINOR}, Patch:${LENMUS_VERSION_PATCH}, SHA1:${LENMUS_VERSION_SHA1}")

#build version string for package name
set(LENMUS_PACKAGE_VERSION "${LENMUS_VERSION_MAJOR}.${LENMUS_VERSION_MINOR}.${LENMUS_VERSION_PATCH}" )
message (STATUS "LenMus package version = '${LENMUS_PACKAGE_VERSION}'") 


