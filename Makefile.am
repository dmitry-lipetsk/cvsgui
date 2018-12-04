## Process this file with automake to produce Makefile.in
## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

SUBDIRS = intl po macros am_macros rf cvstree cvsgui_protocol common . pixmaps gcvs Macros PythonLib

gcvs_src_gcvsdocdir = ${prefix}/doc/gcvs
gcvs_src_gcvsdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS\
	TODO\
	ABOUT-NLS

EXTRA_DIST = $(gcvs_src_gcvsdoc_DATA)

gnomemenudir = $(prefix)/@NO_PREFIX_PACKAGE_MENU_DIR@/Development
gnomemenu_DATA = gcvs.desktop


ACLOCAL_AMFLAGS = -I m4

gcvs_docdir = ${gcvsdocdir}

gcvs_datadir = ${prefix}/@NO_PREFIX_PACKAGE_DATA_DIR@
gcvs_data_DATA = \
	gcvs/gcvs.glade

DIST_SOURCES = \
	gcvs.spec.in \
	gcvs.desktop.in.in \
	xml-i18n-extract.in \
	xml-i18n-merge.in \
	xml-i18n-update.in \
	autogen.sh \
	GCvs.prj \
	setup-gettext \
	$(gcvs_doc_DATA) \
	$(gcvs_data_DATA)

@XML_I18N_MERGE_DESKTOP_RULE@

# XML merge is broken for .destop in RH 8.0, so temporarily just copy it
#$(gnomemenu_DATA) : $(gnomemenu_DATA).in $(top_builddir)/xml-i18n-merge $(top_srcdir)/po/*.po
#	cat $< | sed -e "s/^_//g" > $(gnomemenu_DATA)

etags:
	etags -C `find . -name \*.cpp`
	etags -C -a `find . -name \*.h`
	etags -a `find . -name \*.c`

Makefiles:
	make ./Makefile
	list='$(SUBDIRS)'; for subdir in $$list; do \
 		test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) ./Makefile.in ); \
	done

rpm: dist
	rpm -ta $(distdir).tar.gz

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

