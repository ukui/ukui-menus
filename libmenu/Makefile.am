lib_LTLIBRARIES = libukui-menu.la

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	-DUKUIMENU_I_KNOW_THIS_IS_UNSTABLE	\
	$(DISABLE_DEPRECATED_CFLAGS) \
	$(DEBUG_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

libukui_menu_includedir = $(includedir)/ukui-menus
libukui_menu_include_HEADERS = \
	ukuimenu-tree.h

libukui_menu_sources = \
	canonicalize.c \
	desktop-entries.c \
	entry-directories.c \
	ukuimenu-tree.c \
	menu-layout.c \
	menu-monitor.c \
	menu-util.c

libukui_menu_la_SOURCES = \
	$(libukui_menu_sources) \
	canonicalize.h \
	desktop-entries.h \
	entry-directories.h \
	ukuimenu-tree.h \
	menu-layout.h \
	menu-monitor.h \
	menu-util.h

libukui_menu_la_LIBADD = \
	$(GLIB_LIBS)

libukui_menu_la_LDFLAGS = \
	-version-info $(LIB_MENU_LT_VERSION) \
	-no-undefined \
	-export-symbols-regex ukuimenu_tree

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libukui-menu.pc

EXTRA_DIST = \
	libukui-menu.pc.in \
	libukui-menu-uninstalled.pc.in

CLEANFILES =

# Introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
# Note: we only include the headers here so far because there's no gtk-doc at all anyway
introspection_sources = $(libukui_menu_include_HEADERS)

UkuiMenu-2.0.gir: libukui-menu.la
UkuiMenu_2_0_gir_INCLUDES = GObject-2.0
UkuiMenu_2_0_gir_CFLAGS = $(AM_CPPFLAGS)
UkuiMenu_2_0_gir_LIBS = libukui-menu.la
UkuiMenu_2_0_gir_SCANNERFLAGS = --pkg-export=libukui-menu
UkuiMenu_2_0_gir_FILES = $(addprefix $(srcdir)/,$(introspection_sources))
INTROSPECTION_GIRS += UkuiMenu-2.0.gir

girdir = $(INTROSPECTION_GIRDIR)
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(INTROSPECTION_TYPELIBDIR)
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

-include $(top_srcdir)/git.mk
