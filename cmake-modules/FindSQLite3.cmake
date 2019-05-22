#  - Try to find SQLite3
# Once done this will define
#
#  SQLite3_FOUND - system has SQLite3
#  SQLite3_INCLUDE_DIR - the SQLite3 include directory
#  SQLite3_LIBRARIES - Link these to use SQLite3
#  SQLite3_DEFINITIONS - Compiler switches required for using SQLite3
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

include(LibFindMacros)

if ( SQLite3_INCLUDE_DIR AND SQLite3_LIBRARIES )
   # in cache already
   SET(SQLite3_FIND_QUIETLY TRUE)
endif ( SQLite3_INCLUDE_DIR AND SQLite3_LIBRARIES )


# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
  INCLUDE(FindPkgConfig)
  pkg_check_modules(SQLite3 REQUIRED sqlite3 )
endif( NOT WIN32 )

FIND_PATH(SQLite3_INCLUDE_DIR
	NAMES
		sqlite3.h
	PATHS
		${_SQLite3IncDir}
		$ENV{SQLite3_DIR}/include				#Windows
)

FIND_LIBRARY(SQLite3_LIBRARIES
	NAMES
		sqlite3
	PATHS
		${_SQLite3LinkDir}
		$ENV{SQLite3_DIR}/lib					#Windows
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SQLite3 DEFAULT_MSG SQLite3_INCLUDE_DIR SQLite3_LIBRARIES )

# show the SQLite3_INCLUDE_DIR and SQLite3_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(SQLite3_INCLUDE_DIR SQLite3_LIBRARIES )

