from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *
from cvsgui.SafeTk import *
if checkTk():
  from Tkinter import *

"""
  WinCvs Macro "MyMacro"
  $Revision: 1.2 $

  written by Someone, Sometime
  contact:
    email:  user@host.org

  Insert legal disclaimer here.

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.x.x .

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

class MyMacroDlg(Frame):
  def __init__(self, master, doItProc):
    Frame.__init__(self, master)
    self.master.title('My Tk Macro')
    self.doItProc = doItProc

    self.createWidgets()
    self.pack(fill=BOTH, expand=1)
    self.update()
    w = self.winfo_width()
    h = self.winfo_height()
    x = (self.winfo_screenwidth() - w) / 2
    y = (self.winfo_screenheight() - h) / 2
    self.master.geometry(newGeometry = '%dx%d+%d+%d' % (w, h, x, y))

  def createWidgets(self):
    #Put your UI here.

    Button(self, text='OK', command=self.doIt)\
          .grid(row=3, column=0, padx=5, pady=5, sticky=W+E+N)
    Button(self, text='Cancel', command=self.master.destroy)\
          .grid(row=3, column=1, padx=5, pady=5, sticky=W+E+N)
    self.rowconfigure(1, weight=2)

  def doIt(self):
    self['cursor'] = 'watch'
    try:
      self.doItProc()
    finally:
      self['cursor'] = 'arrow'
      self.master.destroy()

class MyTkMacro(Macro):
  def __init__(self):
    Macro.__init__(self, 'My Tk Macro', MACRO_SELECTION, 0, 'My Macros')

  def OnCmdUI(self, cmdui):
    """ Allows only selections of cvs-controlled entries """
    for entry in App.GetSelection():
      if entry.IsUnknown():
        enabled = 0
        break
    else:
      enabled = 1
    cmdui.Enable(enabled)

  def doStuff(self):
    """ Runs a CVS command on all selected items """
    cwdbup = os.getcwd()
    try:
      cvs = Cvs(1,0)
      targets = []
      prepareTargets(App.GetSelection(), targets)

      #substitute 'command' and 'myargs' with your actual cvs command
      # and arguments, e.g. 
      #   code, out, err = cvs.Run('update', '-d yesterday', '-C', *targets)
      
      code, out, err = cvs.Run('command', 'myargs', *targets)
      if code == 0:
        print 'Done.'
      else:
        cvs_err(err)
    finally:
      os.chdir(cwdbup)

  def Run(self):
    try:
      tk = SafeTkRoot(self)
      dlg = MyMacroDlg(tk, self.doStuff)
      tk.wait_window(dlg)
    except:
      if tk:
        tk.destroy()
      raise

# uncomment the following line to make the macro register on the menu:
#MyTkMacro()