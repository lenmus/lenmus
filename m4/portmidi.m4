AC_DEFUN([AX_PATH_PORTMIDI],
[
	AC_ARG_WITH(portmidi-prefix,
		AC_HELP_STRING([--with-portmidi-prefix=PREFIX],[Prefix where PortMidi was installed.]),
		[_portmidi_prefix="$withval"],[_portmidi_prefix="/usr"])

	AC_MSG_CHECKING(for PortMidi compatible library)

	if test -e "${_portmidi_prefix}/lib/libportmidi.so" -a -e "${_portmidi_prefix}/lib/libporttime.so"
	then
		AC_MSG_RESULT([yes])
#		AC_CHECK_LIB(porttime, main)
#		AC_CHECK_LIB(portmidi, main)
	else
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([cannot find PortMidi])
	fi

	PORTMIDI_CFLAGS=""
	PORTMIDI_LDFLAGS="-L${_portmidi_prefix}/lib -lportmidi -lporttime"
	AC_SUBST(PORTMIDI_CFLAGS)
	AC_SUBST(PORTMIDI_LDFLAGS)

	unset _portmidi_prefix
])dnl
