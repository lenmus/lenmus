# Init
AC_PREREQ(2.50)
AC_INIT(lenmus, 4.0, cecilios at sourceforge)

# Set paths
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src)
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

# Check for programs
AC_PROG_CC
AC_PROG_CXX

# Check for functions
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([strstr])
AC_CHECK_HEADERS([sys/time.h])
AC_C_RESTRICT
AC_CHECK_LIB([m],floor)
AC_CHECK_FUNCS([floor], [], [AC_MSG_ERROR(afloor not found.)])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([modf])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_C_INLINE
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_HEADER_MAJOR
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Search for required libraries
AX_PATH_PORTMIDI
AX_PATH_PORTAUDIO
AC_CHECK_LIBUNITTESTPP(1.3, [], [AC_MSG_ERROR(libunittest++ not found.)])
AC_CHECK_LIBAGG(2.4, [], [AC_MSG_ERROR(libagg not found.)])
AX_LIB_SQLITE3(3.6.10, [], [AC_MSG_ERROR(freetype2 not found.)])
AC_CHECK_FT2(, [], [AC_MSG_ERROR(freetype2 not found.)])

# Check wxWidgets
WX_CONFIG_OPTIONS
reqwx=2.8.0
WX_CONFIG_CHECK($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.
 
		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi

# Generate output
AC_PROG_INSTALL
AC_OUTPUT(Makefile src/Makefile)
