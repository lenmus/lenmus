AC_COPYRIGHT([Copyright 2003-2010 Cecilio Salmerón])

# Init
AC_PREREQ(2.50)
AC_INIT(lenmus, 4.2, cecilios at sourceforge)

# Set paths and files
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src)
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT(/usr/local)
if test "x$prefix" = "xNONE"; then
	prefix=$ac_default_prefix
	ac_configure_args="$ac_configure_args --prefix $prefix"
fi
AC_DEFINE_UNQUOTED([PACKAGE_PREFIX], ["${prefix}/"], [Prefix for the package (/usr/local/)])

# Check for programs
AC_PROG_CC
AC_PROG_CXX

# Check for functions
AC_CHECK_FUNCS([gettimeofday], [], [AC_MSG_ERROR(gettimeofday not found.)])
AC_CHECK_FUNCS([mkdir], [], [AC_MSG_ERROR(mkdir not found.)])
AC_CHECK_FUNCS([strstr], [], [AC_MSG_ERROR(strstr not found.)])
AC_CHECK_HEADERS([sys/time.h], [], [AC_MSG_ERROR(sys/time.h not found.)])
AC_C_RESTRICT
AC_CHECK_LIB([m],floor, [], [AC_MSG_ERROR(function floor in math lib not found.)])
AC_CHECK_FUNCS([floor], [], [AC_MSG_ERROR(floor not found.)])
AC_CHECK_FUNCS([memset], [], [AC_MSG_ERROR(memset not found.)])
AC_CHECK_FUNCS([modf], [], [AC_MSG_ERROR(modf not found.)])
AC_CHECK_FUNCS([pow], [], [AC_MSG_ERROR(pow not found.)])
AC_CHECK_FUNCS([sqrt], [], [AC_MSG_ERROR(sqrt not found.)])
AC_CHECK_HEADERS([limits.h], [], [AC_MSG_ERROR(limits.h not found.)])
AC_CHECK_HEADERS([stddef.h], [], [AC_MSG_ERROR(stddef.h not found.)])
AC_CHECK_HEADERS([stdlib.h], [], [AC_MSG_ERROR(stdlib.h not found.)])
AC_CHECK_HEADERS([string.h], [], [AC_MSG_ERROR(string.h not found.)])
AC_C_INLINE
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_HEADER_MAJOR
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Check the operating system
case "${host_os}" in
	"")
		AC_MSG_ERROR(unknown operating system.)
		;;
	linux*)
		SYS=linux
		;;
	darwin*)
		SYS=darwin
		;;
	*mingw32* | *cygwin*)
		case "${host_os}" in
			*mingw32*)
				SYS=win32
			;;
			*cygwin*)
				# Check if we are using the mno-cygwin mode in which case we are
				# actually dealing with a mingw32 compiler.
				AC_EGREP_CPP([yes],
				[#ifdef WIN32
				yes
				#endif],
				SYS=win32)
			;;
		esac
		;;
	*)
		AC_MSG_ERROR(operating system ${host_os} not supported.)
		;;
esac
AM_CONDITIONAL([LINUX], test "x${SYS}" = xlinux)
AM_CONDITIONAL([MINGW32], test "x${SYS}" = xwin32)
AM_CONDITIONAL([DARWIN], test "x${SYS}" = xdarwin)

# Search for required libraries
AX_PATH_PORTMIDI
AX_LIB_SQLITE3(3.6.10, [], [AC_MSG_ERROR(SQLite3 not found.)])
AC_CHECK_FT2(2.0, [], [AC_MSG_ERROR(freetype2 not found.)])

# Check whether to do a debug build.
AC_MSG_CHECKING([whether to do a debug build])
AC_ARG_ENABLE([debug],
     [  --enable-debug    Turn on debugging],
     [case "${enableval}" in
       yes) debug=true ;;
       no)  debug=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
     esac],[debug=false])
     AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

if test x$debug = xtrue; then
	AC_MSG_RESULT(yes)
	AC_CHECK_LIBUNITTESTPP(1.3, [], [AC_MSG_ERROR(libunittest++ not found.)])
else
	AC_MSG_RESULT(no)
fi

# Check wxWidgets
AM_OPTIONS_WXCONFIG
AM_PATH_WXCONFIG(2.8.0, [wxWin=1])

AC_CONFIG_FILES(Makefile src/Makefile)
reqwx=2.8.0
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
AC_OUTPUT
