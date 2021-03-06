AC_PREREQ(2.62)

AC_INIT([ukui-menus], [1.1.2], [http://www.ukui.org])
AC_CONFIG_SRCDIR(libmenu/ukuimenu-tree.h)

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-xz check-news])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS(config.h)

AM_MAINTAINER_MODE
MATE_MAINTAINER_MODE_DEFINES

# Before making a release, the LT_VERSION string should be modified.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A

LIB_MENU_LT_VERSION=6:9:4
AC_SUBST(LIB_MENU_LT_VERSION)

GETTEXT_PACKAGE=ukui-menus
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of default gettext domain])
AC_SUBST(GETTEXT_PACKAGE)

AM_GLIB_GNU_GETTEXT

IT_PROG_INTLTOOL([0.40.0])

AC_PROG_CC
AC_STDC_HEADERS
AC_ARG_PROGRAM
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.36.0 gio-2.0 >= 2.15.2)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

MATE_COMPILE_WARNINGS(yes)
AC_ARG_ENABLE(deprecation_flags,
              [AC_HELP_STRING([--enable-deprecation-flags],
                              [use *_DISABLE_DEPRECATED flags @<:@default=no@:>@])],,
              [enable_deprecation_flags=no])

if test "x$enable_deprecation_flags" = "xyes"; then
   DISABLE_DEPRECATED_CFLAGS=$DISABLE_DEPRECATED
   AC_SUBST(DISABLE_DEPRECATED_CFLAGS)
fi

dnl --enable-debug=(yes|minimum|no)
AC_ARG_ENABLE(debug,
              [AC_HELP_STRING([--enable-debug],
                              [turn on debugging @<:@default=minimum@:>@])],,
              [enable_debug=minimum])
if test "$enable_debug" = "yes"; then
  DEBUG_CFLAGS="-DG_ENABLE_DEBUG"
else
  if test "x$enable_debug" = "xno"; then
    DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS"
  else
    DEBUG_CFLAGS=""
    # make sure we have the right string for the configure summary
    enable_debug="minimum"
  fi
fi
AC_SUBST(DEBUG_CFLAGS)

AC_ARG_ENABLE(python,
              [AC_HELP_STRING([--enable-python],
                              [build python bindings])],
              [enable_python=$enableval],
              [enable_python=yes])

# Detect if we can build Python bindings (need python and python headers)
if test "x$enable_python" = "xyes" ; then
	AM_PATH_PYTHON(3.6)
	PKG_CHECK_MODULES([PYTHON], [python-${PYTHON_VERSION}], [have_python=yes], [have_python=no])
else
	have_python=no
fi
AC_SUBST(PYTHON_LIBS)
AC_SUBST(PYTHON_CFLAGS)

AM_CONDITIONAL(HAVE_PYTHON, test x$have_python = xyes)


GOBJECT_INTROSPECTION_CHECK([0.6.7])

AC_OUTPUT([
Makefile
libmenu/Makefile
libmenu/libukui-menu.pc
libmenu/libukui-menu-uninstalled.pc
layout/Makefile
desktop-directories/Makefile
util/Makefile
python/Makefile
po/Makefile.in
])

dnl ---------------------------------------------------------------------------
dnl - Show summary
dnl ---------------------------------------------------------------------------

echo "
              ukui-menus $VERSION
              `echo ukui-menus $VERSION | sed "s/./=/g"`

        prefix:                       ${prefix}
        exec_prefix:                  ${exec_prefix}
        libdir:                       ${libdir}
        bindir:                       ${bindir}
        sbindir:                      ${sbindir}
        sysconfdir:                   ${sysconfdir}
        localstatedir:                ${localstatedir}
        datadir:                      ${datadir}
        source code location:         ${srcdir}
        compiler:                     ${CC}
        cflags:                       ${CFLAGS}
        Maintainer mode:              ${USE_MAINTAINER_MODE}
        Use *_DISABLE_DEPRECATED:     ${enable_deprecation_flags}

        Turn on debugging:            ${enable_debug}
        Build python bindings:        ${have_python}
        Build introspection support:  ${found_introspection}

"
