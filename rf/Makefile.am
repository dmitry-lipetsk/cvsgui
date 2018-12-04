SUBDIRS = support

EXTRA_DIST = .cvsignore

noinst_LIBRARIES = librf.a

librf_a_SOURCES = \
	uconsole.cpp \
	udraw.cpp \
	uevent.cpp \
	umenu.cpp \
	uobject.cpp \
	utoolbar.cpp \
	ustr.cpp \
	uwidget.cpp

noinst_HEADERS = \
	uconfig.h \
	uconsole.h \
	udraw.h \
	umain.h \
	umenu.h \
	uobject.h \
	ustr.h \
	utoolbar.h \
	uwidget.h

INCLUDES = -I$(srcdir)/support/gtk -I$(srcdir)/../common

