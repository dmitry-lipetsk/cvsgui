bin_PROGRAMS = qcvs

PACKAGE=@PACKAGE@
VERSION=@VERSION@

BUILT_SOURCES = \
	QCheckout_STICKY.moc \
	QCheckout_STICKYData.moc \
	QCheckoutTab.moc \
	QCheckoutTabData.moc \
	QCheckout_MAIN.moc \
	QCheckout_MAINData.moc \
	QImportTab.moc \
	QImportTabData.moc \
	QImport_MAIN.moc \
	QImport_MAINData.moc \
	QImportWarning.moc \
	QImportWarningData.moc \
	QImportReport.moc \
	QImportReportData.moc \
	QImportFilter.moc \
	QImportFilterData.moc \
	QCommitTab.moc \
	QCommitTabData.moc \
	QCommit_MAIN.moc \
	QCommit_MAINData.moc \
	QLineCmdTab.moc \
	QLineCmdTabData.moc \
	QLineCmd.moc \
	QLineCmdData.moc \
	QCvsAlert.moc \
	QCvsAlertData.moc \
	QGetPassword.moc \
	QGetPasswordData.moc \
	QGetPrefs_CVSROOT.moc \
	QGetPrefs_CVSROOTData.moc \
	QGetPrefs_GLOBALS.moc \
	QGetPrefs_GLOBALSData.moc \
	QGetPrefs_PORTS.moc \
	QGetPrefs_PORTSData.moc \
	QGetPrefsTab.moc \
	QGetPrefsTabData.moc \
	QUpdate_MAIN.moc \
	QUpdate_MAINData.moc \
	QUpdate_MERGE.moc \
	QUpdate_MERGEData.moc \
	QUpdate_STICKY.moc \
	QUpdate_STICKYData.moc \
	QUpdateTab.moc \
	QUpdateTabData.moc \
	menumgr.moc \
	qcvsapp.moc \
	qcvsconsole.moc \
	qcvsmainw.moc \
	qdiritem.moc \
	qdirview.moc \
	qfileitem.moc \
	qfileview.moc

qcvs_SOURCES = \
	QCheckout_STICKY.cpp \
	QCheckout_STICKYData.cpp \
	QCheckoutTab.cpp \
	QCheckoutTabData.cpp \
	QCheckout_MAIN.cpp \
	QCheckout_MAINData.cpp \
	QImportTab.cpp \
	QImportTabData.cpp \
	QImport_MAIN.cpp \
	QImport_MAINData.cpp \
	QImportWarning.cpp \
	QImportWarningData.cpp \
	QImportReport.cpp \
	QImportReportData.cpp \
	QImportFilter.cpp \
	QImportFilterData.cpp \
	QCommitTab.cpp \
	QCommitTabData.cpp \
	QCommit_MAIN.cpp \
	QCommit_MAINData.cpp \
	QLineCmdTab.cpp \
	QLineCmdTabData.cpp \
	QLineCmd.cpp \
	QLineCmdData.cpp \
	QCvsAlert.cpp \
	QCvsAlertData.cpp \
	QGetPassword.cpp \
	QGetPasswordData.cpp \
	QGetPrefs_CVSROOT.cpp \
	QGetPrefs_CVSROOTData.cpp \
	QGetPrefs_GLOBALS.cpp \
	QGetPrefs_GLOBALSData.cpp \
	QGetPrefs_PORTS.cpp \
	QGetPrefs_PORTSData.cpp \
	QGetPrefsTab.cpp \
	QGetPrefsTabData.cpp \
	QUpdate_MAIN.cpp \
	QUpdate_MAINData.cpp \
	QUpdate_MERGE.cpp \
	QUpdate_MERGEData.cpp \
	QUpdate_STICKY.cpp \
	QUpdate_STICKYData.cpp \
	QUpdateTab.cpp \
	QUpdateTabData.cpp \
	menumgr.cpp \
	qcvs.cpp \
	qcvsapp.cpp \
	qcvsconsole.cpp \
	qcvsmainw.cpp \
	qdiritem.cpp \
	qdirview.cpp \
	qfileitem.cpp \
	qfileview.cpp

noinst_HEADERS = \
	QCheckout_STICKY.h \
	QCheckout_STICKYData.h \
	QCheckoutTab.h \
	QCheckoutTabData.h \
	QCheckout_MAIN.h \
	QCheckout_MAINData.h \
	QImportTab.h \
	QImportTabData.h \
	QImport_MAIN.h \
	QImport_MAINData.h \
	QImportWarning.h \
	QImportWarningData.h \
	QImportReport.h \
	QImportReportData.h \
	QImportFilter.h \
	QImportFilterData.h \
	QCommitTab.h \
	QCommitTabData.h \
	QCommit_MAIN.h \
	QCommit_MAINData.h \
	QLineCmdTab.h \
	QLineCmdTabData.h \
	QLineCmd.h \
	QLineCmdData.h \
	QCvsAlert.h \
	QCvsAlertData.h \
	QGetPassword.h \
	QGetPasswordData.h \
	QGetPrefs_CVSROOT.h \
	QGetPrefs_CVSROOTData.h \
	QGetPrefs_GLOBALS.h \
	QGetPrefs_GLOBALSData.h \
	QGetPrefs_PORTS.h \
	QGetPrefs_PORTSData.h \
	QGetPrefsTab.h \
	QGetPrefsTabData.h \
	QUpdate_MAIN.h \
	QUpdate_MAINData.h \
	QUpdate_MERGE.h \
	QUpdate_MERGEData.h \
	QUpdate_STICKY.h \
	QUpdate_STICKYData.h \
	QUpdateTab.h \
	QUpdateTabData.h \
	menumgr.h \
	qcvsapp.h \
	qcvsconsole.h \
	qcvsmainw.h \
	qdiritem.h \
	qdirview.h \
	qfileitem.h \
	qfileview.h \
	stdafx.h

EXTRA_DIST = mocit tabit \
	*.dlg \
	*.xpm


INCLUDES = -I../common -I/.
LDADD = ../common/libcommon.a ../cvstree/libcvstree.a
DISTCLEANFILES = $(BUILT_SOURCES)

qcvsapp.o: qcvsapp.cpp
	$(CXXCOMPILE) -DPACKAGE="\"$(PACKAGE)\"" -DVERSION="\"$(VERSION)\"" -DPKGDATA="\"$(pkgdatadir)\"" -c $<

qcvsmainw.cpp:qcvsmainw.moc
qcvsmainw.moc: qcvsmainw.h
	@MOC@ $< -o $@

qcvsapp.cpp:qcvsapp.moc
qcvsapp.moc: qcvsapp.h
	@MOC@ $< -o $@

qcvsconsole.cpp:qcvsconsole.moc
qcvsconsole.moc: qcvsconsole.h
	@MOC@ $< -o $@

menumgr.cpp:menumgr.moc
menumgr.moc: menumgr.h
	@MOC@ $< -o $@

qdiritem.cpp:qdiritem.moc
qdiritem.moc : qdiritem.h
	@MOC@ $< -o $@

qdirview.cpp:qdirview.moc
qdirview.moc : qdirview.h
	@MOC@ $< -o $@

qfileitem.cpp:qfileitem.moc
qfileitem.moc : qfileitem.h
	@MOC@ $< -o $@

qfileview.cpp:qfileview.moc
qfileview.moc : qfileview.h
	@MOC@ $< -o $@

QCvsAlertData.cpp:QCvsAlertData.moc
QCvsAlertData.moc : QCvsAlertData.h
	@MOC@ $< -o $@

QCvsAlert.cpp:QCvsAlert.moc
QCvsAlert.moc : QCvsAlert.h
	@MOC@ $< -o $@

QGetPasswordData.cpp:QGetPasswordData.moc
QGetPasswordData.moc : QGetPasswordData.h
	@MOC@ $< -o $@

QGetPassword.cpp:QGetPassword.moc
QGetPassword.moc : QGetPassword.h
	@MOC@ $< -o $@

QGetPrefs_CVSROOTData.cpp:QGetPrefs_CVSROOTData.moc
QGetPrefs_CVSROOTData.moc : QGetPrefs_CVSROOTData.h
	@MOC@ $< -o $@

QGetPrefs_CVSROOT.cpp:QGetPrefs_CVSROOT.moc
QGetPrefs_CVSROOT.moc : QGetPrefs_CVSROOT.h
	@MOC@ $< -o $@

QGetPrefs_GLOBALSData.cpp:QGetPrefs_GLOBALSData.moc
QGetPrefs_GLOBALSData.moc : QGetPrefs_GLOBALSData.h
	@MOC@ $< -o $@

QGetPrefs_GLOBALS.cpp:QGetPrefs_GLOBALS.moc
QGetPrefs_GLOBALS.moc : QGetPrefs_GLOBALS.h
	@MOC@ $< -o $@

QGetPrefs_PORTSData.cpp:QGetPrefs_PORTSData.moc
QGetPrefs_PORTSData.moc : QGetPrefs_PORTSData.h
	@MOC@ $< -o $@

QGetPrefs_PORTS.cpp:QGetPrefs_PORTS.moc
QGetPrefs_PORTS.moc : QGetPrefs_PORTS.h
	@MOC@ $< -o $@

QGetPrefsTabData.cpp:QGetPrefsTabData.moc
QGetPrefsTabData.moc : QGetPrefsTabData.h
	@MOC@ $< -o $@

QGetPrefsTab.cpp:QGetPrefsTab.moc
QGetPrefsTab.moc : QGetPrefsTab.h
	@MOC@ $< -o $@

QUpdate_MAINData.cpp:QUpdate_MAINData.moc
QUpdate_MAINData.moc : QUpdate_MAINData.h
	@MOC@ $< -o $@

QUpdate_MAIN.cpp:QUpdate_MAIN.moc
QUpdate_MAIN.moc : QUpdate_MAIN.h
	@MOC@ $< -o $@

QUpdate_MERGEData.cpp:QUpdate_MERGEData.moc
QUpdate_MERGEData.moc : QUpdate_MERGEData.h
	@MOC@ $< -o $@

QUpdate_MERGE.cpp:QUpdate_MERGE.moc
QUpdate_MERGE.moc : QUpdate_MERGE.h
	@MOC@ $< -o $@

QUpdate_STICKYData.cpp:QUpdate_STICKYData.moc
QUpdate_STICKYData.moc : QUpdate_STICKYData.h
	@MOC@ $< -o $@

QUpdate_STICKY.cpp:QUpdate_STICKY.moc
QUpdate_STICKY.moc : QUpdate_STICKY.h
	@MOC@ $< -o $@

QUpdateTabData.cpp:QUpdateTabData.moc
QUpdateTabData.moc : QUpdateTabData.h
	@MOC@ $< -o $@

QUpdateTab.cpp:QUpdateTab.moc
QUpdateTab.moc : QUpdateTab.h
	@MOC@ $< -o $@

QLineCmdData.cpp:QLineCmdData.moc
QLineCmdData.moc : QLineCmdData.h
	@MOC@ $< -o $@

QLineCmd.cpp:QLineCmd.moc
QLineCmd.moc : QLineCmd.h
	@MOC@ $< -o $@

QLineCmdTabData.cpp:QLineCmdTabData.moc
QLineCmdTabData.moc : QLineCmdTabData.h
	@MOC@ $< -o $@

QLineCmdTab.cpp:QLineCmdTab.moc
QLineCmdTab.moc : QLineCmdTab.h
	@MOC@ $< -o $@

QCommit_MAINData.cpp:QCommit_MAINData.moc
QCommit_MAINData.moc : QCommit_MAINData.h
	@MOC@ $< -o $@

QCommit_MAIN.cpp:QCommit_MAIN.moc
QCommit_MAIN.moc : QCommit_MAIN.h
	@MOC@ $< -o $@

QCommitTabData.cpp:QCommitTabData.moc
QCommitTabData.moc : QCommitTabData.h
	@MOC@ $< -o $@

QCommitTab.cpp:QCommitTab.moc
QCommitTab.moc : QCommitTab.h
	@MOC@ $< -o $@

QImportFilterData.cpp:QImportFilterData.moc
QImportFilterData.moc : QImportFilterData.h
	@MOC@ $< -o $@

QImportFilter.cpp:QImportFilter.moc
QImportFilter.moc : QImportFilter.h
	@MOC@ $< -o $@

QImportReportData.cpp:QImportReportData.moc
QImportReportData.moc : QImportReportData.h
	@MOC@ $< -o $@

QImportReport.cpp:QImportReport.moc
QImportReport.moc : QImportReport.h
	@MOC@ $< -o $@

QImportWarningData.cpp:QImportWarningData.moc
QImportWarningData.moc : QImportWarningData.h
	@MOC@ $< -o $@

QImportWarning.cpp:QImportWarning.moc
QImportWarning.moc : QImportWarning.h
	@MOC@ $< -o $@

QImport_MAINData.cpp:QImport_MAINData.moc
QImport_MAINData.moc : QImport_MAINData.h
	@MOC@ $< -o $@

QImport_MAIN.cpp:QImport_MAIN.moc
QImport_MAIN.moc : QImport_MAIN.h
	@MOC@ $< -o $@

QImportTabData.cpp:QImportTabData.moc
QImportTabData.moc : QImportTabData.h
	@MOC@ $< -o $@

QImportTab.cpp:QImportTab.moc
QImportTab.moc : QImportTab.h
	@MOC@ $< -o $@

QCheckout_MAINData.cpp:QCheckout_MAINData.moc
QCheckout_MAINData.moc : QCheckout_MAINData.h
	@MOC@ $< -o $@

QCheckout_MAIN.cpp:QCheckout_MAIN.moc
QCheckout_MAIN.moc : QCheckout_MAIN.h
	@MOC@ $< -o $@

QCheckoutTabData.cpp:QCheckoutTabData.moc
QCheckoutTabData.moc : QCheckoutTabData.h
	@MOC@ $< -o $@

QCheckoutTab.cpp:QCheckoutTab.moc
QCheckoutTab.moc : QCheckoutTab.h
	@MOC@ $< -o $@

QCheckout_STICKYData.cpp:QCheckout_STICKYData.moc
QCheckout_STICKYData.moc : QCheckout_STICKYData.h
	@MOC@ $< -o $@

QCheckout_STICKY.cpp:QCheckout_STICKY.moc
QCheckout_STICKY.moc : QCheckout_STICKY.h
	@MOC@ $< -o $@
