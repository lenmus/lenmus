#-------------------------------------------------------------------------------------
# This is part of CMake configuration file for building makefiles and installfiles
# for the LenMus Phonascus program
#-------------------------------------------------------------------------------------
# This module creates the manpage for LenMus, for Linux and BSD based systems
#
#-------------------------------------------------------------------------------------

set (MAN_NAME lenmus.1)
set (MAN_SOURCE ${LENMUS_ROOT_DIR}/docs/${MAN_NAME}.txt )
set (MAN_UNCOMPRESSED ${CMAKE_CURRENT_BINARY_DIR}/${MAN_NAME} )

message("MAN_INSTALL_DIR = " ${MAN_INSTALL_DIR} )

# replace variables within the man page and put the result into cmake binary dir
configure_file( ${MAN_SOURCE} ${MAN_UNCOMPRESSED} )

# compress the man page if gzip is installed (except for OpenBSD)
find_program( GZIP_EXECUTABLE gzip )
if (GZIP_EXECUTABLE AND NOT CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
    message(STATUS "  gzip found. Man page will be compressed.")
    set (MAN_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/${MAN_NAME}.gz )
    execute_process(COMMAND ${GZIP_EXECUTABLE} ${MAN_UNCOMPRESSED})
else() # system is OpenBSD or gzip not found
    message(STATUS "  System is OpenBSD or gzip not found: Man page will not be compressed.")
    set (MAN_INSTALL ${MAN_UNCOMPRESSED} )
endif()


# install the man page
install( FILES ${MAN_INSTALL} 
         DESTINATION ${MAN_INSTALL_DIR}/man1
         COMPONENT doc
       )


