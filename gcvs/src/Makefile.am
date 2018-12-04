bin_PROGRAMS = gcvs

gcvs_SOURCES = \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	main.cpp 


gcvs_LDFLAGS = 
gcvs_LDADD = \
	$(top_builddir)/common/libcommon.a \
	$(top_builddir)/cvstree/libcvstree.a \
	$(top_builddir)/rf/librf.a \
	$(top_builddir)/cvsgui_protocol/libcvsgui.a \
	@TCL_LIB_SPEC@ \
	@PYTHON_LIB_SPEC@


INCLUDES = \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/rf \
	-I$(top_srcdir)/cvsgui_protocol


EXTRA_DIST = .cvsignore

../common/libcommon.a:

../cvstree/libcvstree.a:

../rf/librf.a::

../cvsgui_protocol/libcvsgui.a:
