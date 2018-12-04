noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = \
	AboutDlg.cpp \
	AnnotateDlg.cpp \
	AppConsole.cpp \
	AppGlue.cpp \
	AskYesNo.cpp \
	Authen.cpp \
	BrowseViewHandlers.cpp \
	BrowseViewColumn.cpp \
	BrowseViewModel.cpp \
	CommitDlg.cpp \
	CheckoutDlg.cpp \
	CvsAlert.cpp \
	CvsArgs.cpp \
	CvsCommands.cpp \
	CvsEntries.cpp \
	CvsIgnore.cpp \
	CvsPrefs.cpp \
	CvsRootDlg.cpp \
	DiffDlg.cpp \
	DiffParse.cpp \
	FileActionDlg.cpp \
	FileTraversal.cpp \
	GetPassword.cpp \
	GetPrefs.cpp \
	ImportDlg.cpp \
	ImportFilter.cpp \
	ImportFilterDlg.cpp \
	InitDlg.cpp \
	ItemListDlg.cpp \
	LaunchHandlers.cpp \
	LineCmd.cpp \
	LogDlg.cpp \
	LoginDlg.cpp \
	LogParse.cpp \
	MacrosSetup.cpp \
	MoveToTrash.cpp \
	MultiFiles.cpp \
	MultiString.cpp \
	Persistent.cpp \
	PromptFiles.cpp \
	ProgressDlg.cpp \
	PythonContainer.cpp \
	PythonGlue.cpp \
	PythonImpl.cpp \
	ReleaseDlg.cpp \
	RtagDlg.cpp \
	SelectionDetailsDlg.cpp \
	StatusDlg.cpp \
	TagDlg.cpp \
	TextBinary.cpp \
	TclGlue.cpp \
	UpdateDlg.cpp \
	UCvsApp.cpp \
	UCvsConsole.cpp \
	UCvsFolders.cpp \
	UCvsFiles.cpp \
	UCvsFrame.cpp \
	UCvsGraph.cpp \
	UCvsMenubar.cpp \
	UCvsStatusbar.cpp \
	UCvsToolbar.cpp \
	UpdateToTempParse.cpp \
	getline.c \
	getopt.c \
	getopt1.c

noinst_HEADERS = \
	AboutDlg.h \
	AnnotateDlg.h \
	AppConsole.h \
	AppGlue.h \
	AskYesNo.h \
	Authen.h \
	BrowseViewHandlers.h \
	BrowseViewColumn.h \
	BrowseViewModel.h \
	CheckoutDlg.h \
	CommitDlg.h \
	CvsAlert.h \
	CvsArgs.h \
	CvsCommands.h \
	CvsEntries.h \
	CvsIgnore.h \
	CvsPrefs.h \
	CvsRootDlg.h \
	DiffDlg.h \
	DiffParse.h \
	EditorMonDlg.h \
	FileActionDlg.h \
	FileTraversal.h \
	GetPassword.h \
	GetPrefs.h \
	ImportDlg.h \
	ImportFilter.h \
	ImportFilterDlg.h \
	InitDlg.h \
	ItemListDlg.h \
	LaunchHandlers.h \
	LineCmd.h \
	LogDlg.h \
	LoginDlg.h \
	LogParse.h \
	MacrosSetup.h \
	MoveToTrash.h \
	MakeSurePathExists.h \
	MultiFiles.h \
	MultiString.h \
	Persistent.h \
	PromptFiles.h \
	ProgressDlg.h \
	PythonContainer.h  \
	PythonGlue.h  \
	PythonGlue.impl.h  \
	PythonImpl.h  \
	PythonMacros.h \
	ReleaseDlg.h \
	RtagDlg.h \
	SelectionDetailsDlg.h \
	SmartComboBox.h \
	SortList.h \
	StatusDlg.h \
	TagDlg.h \
	TclGlue.h \
	TextBinary.h \
	UpdateDlg.h \
	UCvsApp.h \
	UCvsCommands.h \
	UCvsConsole.h \
	UCvsDialogs.h \
	UCvsFolders.h \
	UCvsFiles.h \
	UCvsFrame.h \
	UCvsGraph.h \
	UCvsMenubar.h \
	UCvsStatusbar.h \
	UCvsToolbar.h \
	VolsPaths.h \
	dll_garbage.h \
	dll_glue.h \
	dll_loader.h \
	dll_process.h \
	fnmatch.h \
	getline.h \
	getopt.h

INCLUDES = -I$(top_srcdir)/gcvs -I$(top_srcdir)/gcvs/src -I/. -I$(top_srcdir)/cvstree -I$(top_srcdir)/rf -I$(top_srcdir)/cvsgui_protocol

EXTRA_DIST = .cvsignore

UCvsApp.o: UCvsApp.cpp $(top_srcdir)/GCVS-VERSION
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) -DPACKAGE="\"$(PACKAGE)\"" -DVERSION="\"$(VERSION)\"" -DPKGDATA="\"$(pkgdatadir)\"" -DEXECLIB="\"$(bindir)\"" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<

TclGlue.o: TclGlue.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) @TCL_CFLAGS@ -DUSE_TCL="$(HAVE_TCL)" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<

CvsPrefs.o: CvsPrefs.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) -DEXECLIB="\"$(bindir)\"" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<

PythonContainer.o: PythonContainer.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) @PYTHON_CFLAGS@ -DUSE_PYTHON="$(HAVE_PYTHON)" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<
	
PythonGlue.o: PythonGlue.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) @PYTHON_CFLAGS@ -DUSE_PYTHON="$(HAVE_PYTHON)" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<
	
PythonImpl.o: PythonImpl.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) @PYTHON_CFLAGS@ -DUSE_PYTHON="$(HAVE_PYTHON)" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<

UCvsFrame.o: UCvsFrame.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) @PYTHON_CFLAGS@ -DUSE_PYTHON="$(HAVE_PYTHON)" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<

UCvsFolders.o: UCvsFolders.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) @PYTHON_CFLAGS@ -DUSE_PYTHON="$(HAVE_PYTHON)" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<

UCvsFiles.o: UCvsFiles.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) @PYTHON_CFLAGS@ -DUSE_PYTHON="$(HAVE_PYTHON)" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<

UCvsMenubar.o: UCvsMenubar.cpp
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) @PYTHON_CFLAGS@ -DUSE_PYTHON="$(HAVE_PYTHON)" -c -o $@ `test -f '$<' || echo '$(srcdir)/'` $<
