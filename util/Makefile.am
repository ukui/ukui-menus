noinst_PROGRAMS = ukui-menu-spec-test

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(srcdir)/../libmenu \
	-DUKUIMENU_I_KNOW_THIS_IS_UNSTABLE \
	-DUKUILOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(DISABLE_DEPRECATED_CFLAGS) \
	$(DEBUG_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

ukui_menu_spec_test_SOURCES = \
	test-menu-spec.c

ukui_menu_spec_test_LDADD = \
	$(GLIB_LIBS) \
	../libmenu/libukui-menu.la

if HAVE_PYTHON
pyexampledir = $(pkgdatadir)/examples
pyexample_DATA = ukui-menus-ls.py
else
pyexampledir =
pyexample_DATA =
endif

EXTRA_DIST = $(pyexample_DATA)

-include $(top_srcdir)/git.mk

