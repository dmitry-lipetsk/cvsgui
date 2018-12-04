from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.SafeTk import *
from cvsgui.SubstModeTk import *
import string

if checkTk():
  from Tkinter import *

"""
  CvsGui Macro "Extended Add Selection"
  $Revision: 1.5 $

  written by Oliver Giesen, Oct 2004 - Jul 2005
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
   The macro will not be able to show its full potential unless you run it
   with Python 2.2.1 or later!!! (with earlier Python builds you would get
   search path related errors, which I have not yet been able to resolve)
   If Tk is not available, the macro will use a simplified UI instead.

   You will also need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.0.3 .

  ======
  Usage:

  - Select one or more non-CVS-controlled files or folders.

  - Run the macro from the Macros|Add menu.

   ~Depending on whether Tk is available in your setup or not you will be
    presented with a more or less elaborate dialog which will let you specify
    the "k-flags" that should be applied to the selected items.

  - Specify the relevant k-flags or leave the entry empty to let the server
    handle this according to its cvswrappers file.

  - Hit "OK".

   ~the selected items will be added.
    
  =============
  Known Issues / "Un-niceties":

  - The Tk-based advanced "k-flags" UI used in this macro still has some quirks
    of its own. See comments inside the cvsgui.CvsSubstModeTk module for details.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""


class ExtendedAddSelDlg(Frame):
  def __init__(self, master, doItProc, getHelpProc):
    Frame.__init__(self, master)
    self.master.title('Extended Add Selection')

    self.doItProc = doItProc
    self.kmode = StringVar()

    self.createWidgets(getHelpProc)
    self.pack(fill=BOTH, expand=1)
    self.update()
    w = self.winfo_width()
    h = self.winfo_height()
    x = (self.winfo_screenwidth() - w) / 2
    y = (self.winfo_screenheight() - h) / 2
    self.master.geometry(newGeometry = '%dx%d+%d+%d' % (w, h, x, y))

  def createWidgets(self, getHelpProc):
    Label(self, text='Please specify file flags or leave empty for server defaults')\
          .grid(row=0, columnspan=2, sticky=N+W+E)      
    CvsSubstModeUI(self, self.kmode).grid(row=1, columnspan=2, sticky=N+E+W)
#    Label(self, text=getHelpProc(), justify=LEFT)\
#         .grid(row=2, columnspan=2, sticky=N+E+W+S)
    Button(self, text='OK', command=self.doIt)\
          .grid(row=3, column=0, padx=5, pady=5, sticky=W+E+N)
    Button(self, text='Cancel', command=self.master.destroy)\
          .grid(row=3, column=1, padx=5, pady=5, sticky=W+E+N)
    self.rowconfigure(1, weight=2)

  def doIt(self):
    self['cursor'] = 'watch'
    try:
      self.doItProc(self.kmode.get())
    finally:
      self['cursor'] = 'arrow'
      self.master.destroy()

class CvsExtendedAddSelTk(Macro):
  def __init__(self):
    Macro.__init__(self, 'Extended Add Selection...', MACRO_SELECTION, 0, 'Add')

  def OnCmdUI(self, cmdui):
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    if enabled:
      for entry in self.sel:
        if not entry.IsUnknown():
          enabled = 0
          break
    cmdui.Enable(enabled)

  def addSel(self, kmode):
    cwdbup = os.getcwd()
    try:
      cvs = Cvs(1, 0)
      targets = []
      basedir = prepareTargets(self.sel, targets, 0)
      os.chdir(basedir)
      code, out, err = cvs.Run('add', kmode, *targets)
      if code == 0:
        print 'Done'
      else:
        print err
      
    finally:
      os.chdir(cwdbup)

  def getHelp(self):
    res = ''
    code, out, err = Cvs(1, 0).Run('up', '-k?')
    if code == 1:
      lines = err.splitlines()
      for line in lines[1:]:
        #print '"%s" "%s"' % (line[1], line[1:3])
        if (line[:3] <> 'cvs') and (line[:8] <> '(Specify'):
          res += line + '\n'
    return res      
      
  def Run(self):
    if checkTk():
      try:
        tk = SafeTkRoot(self)
        dlg = ExtendedAddSelDlg(tk, self.addSel, self.getHelp)
        tk.wait_window(dlg)
      except:
        if tk:
          tk.destroy()
        raise
    else:
      #display available options:
      print '\n', self.getHelp()
      answer, kmode = CvsPrompt('', 'Question', 'Please specify file flags or leave empty for server defaults', \
                                alertTitle='Extended Add Selection')
      if answer == 'IDOK':
        self.addSel(kmode)

if checkTk():
  CvsExtendedAddSelTk()