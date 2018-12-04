bin_PROGRAMS = cvstree

cvstree_SOURCES = \
	main.cpp \
	CvsLog.cpp \
	parser.yy \
	lexer.ll

noinst_LIBRARIES = libcvstree.a

libcvstree_a_SOURCES = \
	CvsLog.cpp \
	parser.yy \
	lexer.ll
	
noinst_HEADERS = \
	CvsLog.h \
	common.h \
	parser.h

EXTRA_DIST = .cvsignore

## uncomment LFLAGS for debug output
AM_LFLAGS = -d
