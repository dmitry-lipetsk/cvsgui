from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *
from cvsgui.SafeTk import *
from cvsgui.MacroRegistry import *

if checkTk():
  from Tkinter import *

"""
  WinCvs Macro "Build custom ChangeLog"
  $Revision: 1.2 $

  written by Oliver Giesen, Jun 2005
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    msn:    ogiesen@hotmail.com
    icq:    18777742
    y!:     ogiesen

  Feel free to modify or distribute in whichever way you like, 
   as long as it doesn't limit my personal rights to modify and
   redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATENTION:
   In its current form this macro will only run with Python 2.2.1 or later!!!
   (with earlier Python builds you would get search path related errors,
   which I have not yet been able to resolve. Currently the macro will
   not register in the WinCvs menu at all if TK is not available.)

   You will also need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.0.3 .

  ======
  Usage:

  - Select one or more CVS-controlled files and/or folders

  - Run the macro

  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might 
  have to user@host.org .

"""

_true = 1 == 1  # for Python 2.1 compatibility
_false = not _true

class ChangeLogOptions(PersistentVars):
  def __init__(self):
    self._logName = PersistentStringVar('PY_ChangelogLogName', 'ChangeLog')
    self._skipEmpty = PersistentBoolVar('PY_ChangelogSkipEmpty', str(_true))
    self._omitAuthor = PersistentBoolVar('PY_ChangelogOmitAuthor', str(_false))
    self._omitFiles = PersistentBoolVar('PY_ChangelogOmitFiles', str(_false))
    self._logArgs = PersistentStringVar('PY_ChangelogExtArgs', '')


class BuildChangeLogDlg(TkMacroDlg):
  def __init__(self, master, doItProc, options):
    TkMacroDlg.__init__(self, master, 'Build ChangeLog', doItProc, options)

  def createWidgets(self, buttonrow):
    Label(self, text='ChangeLog filename:').grid(row=0, column=0, sticky=N+W)
    Entry(self, textvariable=self.options._logName).grid(row=0, column=1, sticky=N+E+W)
    Checkbutton(self, text='skip empty comments', variable=self.options._skipEmpty).grid(row=1, column=0, sticky=N+W)
    Checkbutton(self, text='omit author', variable=self.options._omitAuthor).grid(row=2, column=0, sticky=N+W)
    Checkbutton(self, text='omit file names', variable=self.options._omitFiles).grid(row=2, column=1, sticky=N+W)
    Label(self, text='cvs log arguments:').grid(row=3, column=0, sticky=N+W)
    Entry(self, textvariable=self.options._logArgs).grid(row=3, column=1, sticky=N+E+W)

    TkMacroDlg.createWidgets(self)

class BuildChangeLogTk(TkMacro):
  def __init__(self):
    TkMacro.__init__(self, 'Custom ChangeLog...', MACRO_SELECTION, 0, 'Build ChangeLog', BuildChangeLogDlg)

  def OnCmdUI(self, cmdui):
    for entry in App.GetSelection():
      if entry.IsUnknown():
        enabled = 0
        break
    else:
      enabled = 1
    cmdui.Enable(enabled)

  def doStuff(self, options):
    options.storeAll()
    options.flatten()

    targets = []
    basedir = prepareTargets(App.GetSelection(), targets, 0)
    filename = os.path.join(gMacroRegistry.m_macroFolder, 'cvs2cl.py')
    try:
      execfile(filename)
    except:
      sys.stderr.write("Error loading %s:\n" % filename)
      raise
    ChangeLogger(basedir, options.logArgs.split() + targets, options.logName, 'log', None, options)

  def initOptions(self):
    return ChangeLogOptions()

BuildChangeLogTk()
