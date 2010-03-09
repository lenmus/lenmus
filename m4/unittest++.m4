# Configure paths for libunittest++
# Antonio Nicolás 2010-03-05, based on libunittest++.m4 by Kirill Smelkov

# AC_CHECK_LIBUNITTESTPP([MINIMUM-VERSION [, ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]])
# Test for libunittest++, and define LIBUNITTESTPP_CFLAGS and LIBUNITTESTPP_LIBS
#
AC_DEFUN([AC_CHECK_LIBUNITTESTPP],
[
  # Get the cflags and libraries from pkg-config libunittest++ ...
  AC_ARG_WITH([libunittestpp],
    AS_HELP_STRING([--with-libunittestpp=PREFIX],
                   [Prefix where libunittest++ is installed (optional)]),
    [libunittestpp_prefix="$withval"],
    [libunittestpp_prefix=""])

  libunittestpp_name="unittest++"
  if test "x$libunittestpp_prefix" != "x"; then
    libunittestpp_name="$libunittestpp_prefix/lib/pkgconfig/unittest++.pc"
  fi

  PKG_CHECK_MODULES([UNITTESTPP], "$libunittestpp_name", success=yes, success=no)

  if test "x$success" = xyes; then
    ifelse([$2], , :, [$2])

    AC_SUBST([UNITTESTPP_CFLAGS])
    AC_SUBST([UNITTESTPP_LIBS])
  else
    ifelse([$3], , :, [$3])
  fi
])

# end of libunittest++.m4
