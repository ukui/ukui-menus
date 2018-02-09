directorydir = $(datadir)/ukui/desktop-directories

# Should be moved to ukui-control-center:
directory_in_controlcenterfiles = \
	ukui-hardware.directory.in \
	ukui-internet-and-network.directory.in \
	ukui-look-and-feel.directory.in \
	ukui-personal.directory.in \
	ukui-system.directory.in

directory_in_files = \
	ukui-audio-video.directory.in \
	ukui-development.directory.in \
	ukui-education.directory.in \
	ukui-game.directory.in \
	ukui-graphics.directory.in \
	ukui-network.directory.in \
	ukui-office.directory.in \
	ukui-settings.directory.in \
	ukui-settings-system.directory.in \
	ukui-system-tools.directory.in \
	ukui-utility.directory.in \
	ukui-utility-accessibility.directory.in \
	ukui-other.directory.in \
	ukui-android.directory.in \
	ukui-menu-applications.directory.in \
	ukui-menu-system.directory.in \
	$(directory_in_controlcenterfiles)

directory_DATA = $(directory_in_files:.directory.in=.directory)

@INTLTOOL_DIRECTORY_RULE@

EXTRA_DIST= $(directory_in_files)

DISTCLEANFILES = $(directory_DATA)

-include $(top_srcdir)/git.mk
