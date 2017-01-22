AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(PYTHON_CFLAGS) \
	-I$(srcdir)/../libmenu \
	-DUKUIMENU_I_KNOW_THIS_IS_UNSTABLE \
	$(DISABLE_DEPRECATED_CFLAGS) \
	$(DEBUG_CFLAGS)

AM_CFLAGS = \
	$(WARN_CFLAGS) \
	$(NO_STRICT_ALIASING_CFLAGS)

ukuimenudir = $(pyexecdir)
ukuimenu_LTLIBRARIES = ukuimenu.la

ukuimenu_la_SOURCES = ukuimenu.c
ukuimenu_la_LDFLAGS = -module -avoid-version -fPIC -export-symbols-regex initukuimenu
ukuimenu_la_LIBADD = $(GLIB_LIBS) $(PYTHON_LIBS) $(top_builddir)/libmenu/libukui-menu.la

-include $(top_srcdir)/git.mk
