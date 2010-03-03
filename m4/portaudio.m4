AC_DEFUN([AX_PATH_PORTAUDIO],
[
	AC_ARG_WITH(portaudio-prefix,
		AC_HELP_STRING([--with-portaudio-prefix=PREFIX],[Prefix where PortAudio was installed.]),
		[_portaudio_prefix="$withval"],[_portaudio_prefix="/usr"])

	AC_MSG_CHECKING(for PortAudio compatible library)

	if test -e "${_portaudio_prefix}/lib/libportaudio.so"
	then
		AC_MSG_RESULT([yes])
		AC_CHECK_LIB(portaudio, Pa_Initialize)
	else
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([cannot find PortAudio])
	fi

	PORTAUDIO_CFLAGS=""
	PORTAUDIO_LDFLAGS="-L${_portaudio_prefix}/lib -lportaudio"
	AC_SUBST(PORTAUDIO_CFLAGS)
	AC_SUBST(PORTAUDIO_LDFLAGS)

	unset _portaudio_prefix
])dnl
