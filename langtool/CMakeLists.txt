#-------------------------------------------------------------------------------------
#  This is a CMake configuration file for building the langtool program
#
#  To use it you need CMake which can be downloaded from http://www.cmake.org/
#-------------------------------------------------------------------------------------

#--------------------------------------------------------------------------------------
#  This file is part of LenMus project.
#  Copyright (c) 2002-2011 Cecilio Salmeron
#
#  LenMus is free software; you can redistribute it and/or modify it under the
#  terms of the GNU General Public License as published by the Free Software Foundation,
#  either version 3 of the License, or (at your option) any later version.
#
#  Lomse is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
#  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with Lomse; if not, see <http://www.gnu.org/licenses/>.
#  
#  For any comment, suggestion or feature request, please contact the manager of
#  the project at cecilios@users.sourceforge.net
#
#-------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)

# Project name. 
project(langtool)

# set directories 
set( LANGTOOL_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )
set( LENMUS_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/.. ) 
set( OUTDIR ${LANGTOOL_DIR}/bin )
set( EXECUTABLE_OUTPUT_PATH ${OUTDIR} )
set( CMAKE_MODULE_PATH "/usr/share/cmake-2.8/Modules" )
set( CMAKE_LIBRARY_PATH "/usr/lib" )
set( CMAKE_INCLUDE_PATH "/usr/include" )
#set( CMAKE_MODULE_PATH "${LENMUS_DIR}/cmake-modules" )
set( PACKAGES_DIR  ${LENMUS_DIR}/packages )

message(STATUS "CMAKE_MODULE_PATH => " ${CMAKE_MODULE_PATH})
message(STATUS "CMAKE_LIBRARY_PATH => " ${CMAKE_LIBRARY_PATH})
message(STATUS "CMAKE_INCLUDE_PATH => " ${CMAKE_INCLUDE_PATH})

include_directories(
    ${LANGTOOL_DIR}/src
    ${PACKAGES_DIR}/wxXml2/include
)

#set platform
if(WIN32)
    set( LANGTOOL_PLATFORM_WIN32 "1")
    set( LANGTOOL_PLATFORM_UNIX "0")
    
elseif(UNIX)
    set( LANGTOOL_PLATFORM_WIN32 "0")
    set( LANGTOOL_PLATFORM_UNIX "1")
endif()

#set compiler
if(MSVC)
    set( LANGTOOL_COMPILER_MSVC "1")
    set( LANGTOOL_COMPILER_GCC "0")
    
elseif(CMAKE_COMPILER_IS_GNUCC)
    set( LANGTOOL_COMPILER_MSVC "0")
    set( LANGTOOL_COMPILER_GCC "1")
endif()



#Check wxWidgets
#http://docs.wxwidgets.org/2.8/wx_librarieslist.html
SET( wxWidgets_USE_LIBS  aui base gl net richtext xml xrc core adv html) 
FIND_PACKAGE(wxWidgets REQUIRED)

IF(wxWidgets_FOUND)
    #include_directories( ${wxWidgets_INCLUDE_DIRS} )
    INCLUDE(${wxWidgets_USE_FILE})
    IF(UNIX)
        set(wxWidgets_CXX_FLAGS "`wx-config --cflags -D__WXGTK__  -D_UNICODE`")     
    ENDIF(UNIX)
ENDIF(wxWidgets_FOUND)


#Check for libxml2 and libiconv
#FIND_PACKAGE(LibXml2)
set( LIBXML2_LIBRARIES "/usr/lib/libxml2.so.2" ) 
message(STATUS "LIBXML2_LIBRARIES => " ${LIBXML2_LIBRARIES})

# "Print all warnings", macros for GCC & __UNIX__
if(UNIX)
    add_definitions( -Wall -DGCC -D__UNIX__ )
endif(UNIX)


message(STATUS "wxWidgets => " ${wxWidgets_LIBRARIES} )


#define a header file to pass CMake settings to source code
configure_file(
    "${LANGTOOL_DIR}/langtool_config.h.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/langtool_config.h"
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

link_directories(
    ${LIBRARY_OUTPUT_PATH} 
)
        

# source files to compile
#--------------------------------------------------------------

set( SRC_DIR     ${LANGTOOL_DIR}/src )
set( WXXML2_DIR  ${PACKAGES_DIR}/wxXml2 )
set( AGDOC_DIR   ${LANGTOOL_DIR}/src/agdoc )

message(STATUS "SRC_DIR => " ${SRC_DIR})
message(STATUS "WXXML2_DIR => " ${WXXML2_DIR})
message(STATUS "AGDOC_DIR => " ${AGDOC_DIR})


set(LANGTOOL_FILES
    ${SRC_DIR}/command.cpp
    ${SRC_DIR}/command.h
    ${SRC_DIR}/DlgCompileBook.cpp
    ${SRC_DIR}/DlgCompileBook.h
    ${SRC_DIR}/DlgRunAgdoc.cpp
    ${SRC_DIR}/DlgRunAgdoc.h
    ${SRC_DIR}/ebook_processor.cpp
    ${SRC_DIR}/ebook_processor.h
    ${SRC_DIR}/help_processor.cpp
    ${SRC_DIR}/help_processor.h
    ${SRC_DIR}/installer.cpp
    ${SRC_DIR}/installer.h
    ${SRC_DIR}/langtool.cpp
    ${SRC_DIR}/langtool.rc
    ${SRC_DIR}/MainFrame.cpp
    ${SRC_DIR}/MainFrame.h
    ${SRC_DIR}/Paths.cpp
    ${SRC_DIR}/Paths.h
)

set(WXXML2_FILES
    ${WXXML2_DIR}/src/dtd.cpp
    ${WXXML2_DIR}/include/wx/dtd.h
    ${WXXML2_DIR}/src/xml2.cpp
    ${WXXML2_DIR}/include/wx/xml2.h
)

set(AGDOC_FILES
    ${AGDOC_DIR}/agdoc_basics.cpp
    ${AGDOC_DIR}/agdoc_basics.h
    ${AGDOC_DIR}/agdoc_code_colorer.cpp
    ${AGDOC_DIR}/agdoc_code_colorer.h
    ${AGDOC_DIR}/agdoc_content_processor.cpp
    ${AGDOC_DIR}/agdoc_content_processor.h
    ${AGDOC_DIR}/agdoc_cross_reference_processor.cpp
    ${AGDOC_DIR}/agdoc_cross_reference_processor.h
    ${AGDOC_DIR}/agdoc_index_storage.cpp
    ${AGDOC_DIR}/agdoc_index_storage.h
    ${AGDOC_DIR}/agdoc_label_collector.cpp
    ${AGDOC_DIR}/agdoc_label_collector.h
    ${AGDOC_DIR}/agdoc_link_structurizer.cpp
    ${AGDOC_DIR}/agdoc_link_structurizer.h
    ${AGDOC_DIR}/agdoc_list_structurizer.cpp
    ${AGDOC_DIR}/agdoc_list_structurizer.h
    ${AGDOC_DIR}/agdoc_pair_quote_structurizer.cpp
    ${AGDOC_DIR}/agdoc_pair_quote_structurizer.h
    ${AGDOC_DIR}/agdoc_paragraph_structurizer.cpp
    ${AGDOC_DIR}/agdoc_paragraph_structurizer.h
    ${AGDOC_DIR}/agdoc_project_processor.cpp
    ${AGDOC_DIR}/agdoc_project_processor.h
    ${AGDOC_DIR}/agdoc_replacement_processor.cpp
    ${AGDOC_DIR}/agdoc_replacement_processor.h
    ${AGDOC_DIR}/agdoc_singleton_structurizer.cpp
    ${AGDOC_DIR}/agdoc_singleton_structurizer.h
    ${AGDOC_DIR}/agdoc_structure_processor.cpp
    ${AGDOC_DIR}/agdoc_structure_processor.h
    ${AGDOC_DIR}/agdoc_table_structurizer.cpp
    ${AGDOC_DIR}/agdoc_table_structurizer.h
    ${AGDOC_DIR}/agdoc_toc_structurizer.cpp
    ${AGDOC_DIR}/agdoc_toc_structurizer.h
)


set(ALL_SOURCES 
    ${LANGTOOL_FILES} ${WXXML2_FILES} ${AGDOC_FILES}
)


# Adds folders for Visual Studio and other IDEs
source_group( "agdoc"           FILES ${AGDOC_FILES} )
source_group( "langtool"        FILES ${LANGTOOL_FILES} )
source_group( "wxXml2"          FILES ${WXXML2_FILES} )



# langtool program generation
#-------------------------------------------------------------

set(LANGTOOL  langtool)

add_executable( ${LANGTOOL} ${ALL_SOURCES} )

# link to the necessary libraries
target_link_libraries ( ${LANGTOOL}
    ${wxWidgets_LIBRARIES}
    ${LIBXML2_LIBRARIES}
 )

message(STATUS "wxWidgets libraries => " ${wxWidgets_LIBRARIES})
message(STATUS "LibXml2 libraries => " ${LIBXML2_LIBRARIES})


# once built, place executable at langtool/bin
set_target_properties(${LANGTOOL}  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  ${EXECUTABLE_OUTPUT_PATH} )

