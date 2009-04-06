# Generado con autoscan y editado por mif

AC_PREREQ(2.50)
AC_INIT(lenmus, 4.0, cecilio at sourceforge)
#AC_CONFIG_SRCDIR([packages/wxXml2/src/dtd.cpp])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile
		src/Makefile])
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# Como wxWidgets no está "instalado", he copiado a mano el fichero m4 y lo incluyo a mano
m4_include(wxwin.m4)

# Check wxWidgets
AM_OPTIONS_WXCONFIG
reqwx=2.8.0
AM_PATH_WXCONFIG($reqwx, wxWin=1)
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

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_HEADER_MAJOR
AC_CHECK_FUNCS([floor memset modf pow])

## !!!!!!!!!!! Libraries imported "by hand" ...
MIF_LIBS="/usr/lib/libportmidi.so.0 \
       /usr/lib/libporttime.so.0 \
       /usr/lib/libfreetype.so \
       /usr/local/lib/libsqlite3.so "

LIBS="$LIBS $MIF_LIBS"

## !!!!!!!!!!! ... because this doesn't work:
#AC_CHECK_LIB([portmidi], [Pm_Initialize], [PORTMIDI='yes' ], AC_MSG_ERROR([Couldn't find libportmidi]))
#AC_CHECK_LIB(portmidi, Pm_OpenInput, [], [
#        echo "Error! You need to have libpormidi around."
#        exit -1
#        ])
#AC_CHECK_LIB (library, function)
#AC_CHECK_LIB (library, function)

AC_CHECK_LIB(sqlite3, sqlite3_open, [apu_have_sqlite3=1], AC_MSG_ERROR([Couldn't find SQLite3]))

AC_OUTPUT

