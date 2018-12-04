from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *
from cvsgui.SafeTk import *
from cvsgui.SubstModeTk import *

if checkTk():
  from Tkinter import *

"""
  CvsGui Macro "Change File Options"
  $Revision: 1.6 $

  written by Oliver Giesen, Dec 2004
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

  - Run the macro from the Macros|CVS menu

  - Enter the new k-option for the selected files. Note that with newer CVSNT
     servers you could use syntax like the following in order to modify only
     individual tokens of the existing options instead of completely
     replacing the existing ones:

       -k+c
       -k-x

  - Hit OK       

   ~ The macro will apply the new k-option(s)

  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might 
  have to user@host.org .

"""

#you could change the default commit message here:
commitMsgTemplate = 'changed k-option to %s'

class ChangeSubstModeDlg(Frame):
  def __init__(self, master, oldKopt, doItProc):
    Frame.__init__(self, master)
    self.master.title('Change File Option')
    
    self.doItProc = doItProc
    self.kmode = StringVar()
    self.kmode.set(oldKopt)

    self.createWidgets(oldKopt)
    self.pack(fill=BOTH, expand=1)
    self.update()
    w = self.winfo_width()
    h = self.winfo_height()
    x = (self.winfo_screenwidth() - w) / 2
    y = (self.winfo_screenheight() - h) / 2
    self.master.geometry(newGeometry = '%dx%d+%d+%d' % (w, h, x, y))

  def createWidgets(self, oldKopt):
    global commitMsgTemplate
    Label(self, text='ATTENTION: If your server is not CVSNT or CVSNT before\n'\
                     'version 2.0.52 then file options will not be versioned!\n'\
                     'Instead the new option will be applied to all existing\n'\
                     'revisions of the selected files as well as to future ones!', \
          relief=RIDGE, fg='red', font='arial 8 bold') \
          .grid(row=0, columnspan=2, sticky=N+W+E, padx=2, pady=2)
    Label(self, text='Please specify the new file option(s):')\
         .grid(row=1, columnspan=2, sticky=N+W)      
    CvsSubstModeUI(self, self.kmode).grid(row=2, columnspan=2, sticky=N+E+W)

    Label(self, text='Commit message (required if server is CVSNT 2.0.58 or later)\n' \
                    +'Leave blank if you don''t want to create a new revision.\n' \
                    +'("%s" will be replaced with the actual new option)') \
         .grid(row=3, column=0, columnspan=2, sticky=N+W)
    self.commitMsgTxt = Text(self, width=20, height=3)
    self.commitMsgTxt.grid(row=4, columnspan=2, sticky=N+E+W+S, padx=2)
    self.commitMsgTxt.insert(END, commitMsgTemplate)
    #self.onKoptUpdate(oldKopt)

    Button(self, text='OK', command=self.doIt)\
          .grid(row=5, column=0, padx=5, pady=5, sticky=W+E+N)
    Button(self, text='Cancel', command=self.master.destroy)\
          .grid(row=5, column=1, padx=5, pady=5, sticky=W+E+N)
    self.rowconfigure(2, weight=2)

  def onKoptUpdate(self, newKopt):
    pass
    #self.commitMsgTxt.delete(1.0, END)
    #self.commitMsgTxt.insert(END, 'changed file option to ' + newKopt)

  def doIt(self):
    self['cursor'] = 'watch'
    try:
      txt = self.commitMsgTxt.get(1.0, END).strip()
      kopt = self.kmode.get()
      if txt and (txt.find('%s') >= 0):
        txt = txt % kopt
      self.doItProc(kopt, txt)
    finally:
      self['cursor'] = 'arrow'
      self.master.destroy()

class ChangeSubstModeTk(Macro):
  def __init__(self):
    Macro.__init__(self, 'Change File Options', MACRO_SELECTION, 0, 'CVS')

  def OnCmdUI(self, cmdui):
    """ Allows only selections of cvs-controlled files """
    sel = App.GetSelection()
    enabled = len(sel) > 0
    if enabled:
      self.sel0kopt = sel[0].GetKeyword()
      for entry in sel:
        if entry.IsUnknown() or not entry.IsFile():
          enabled = 0
          break
    cmdui.Enable(enabled)

  def changeKopt(self, kopt, commitMsg):
    """ Changes the k-option on all selected files. """
    sel = App.GetSelection()
    ok = 1
    for entry in sel:
      if entry.GetKeyword() <> self.sel0kopt:
                      # should be "Warning":
        ok = CvsAlert('Question', 'Selection contains files with different options!', \
                      'Change options of all selected files anyway?', \
                      'Yes', 'No', '', '', 'Change File Options') == 'IDOK'
        break
      
    if ok:
      cwdbup = os.getcwd()
      try:
        cvs = Cvs(1,0)
        targets = []
        prepareTargets(sel, targets)

        print 'Registering option change...'
        code, out, err = cvs.Run('admin', kopt, *targets)
        if (code == 0) and (not err) or (err.find('admin -k no longer affects') < 0):
          print 'Updating sandbox...'
          code, out, err = cvs.Run('update', kopt, *targets)
          if (code == 0) and (commitMsg <> ''):
            print 'Committing option change...'
            code, out, err = cvs.Run('commit', '-m' + commitMsg, '-f', *targets)
          if code == 0:
            print 'Done.\n'
              
        if code <> 0:
          cvs_err(err)
      finally:
        os.chdir(cwdbup)
    else:
      cvs_err('Aborted by user.\n\n')

  def Run(self):
    try:
      tk = SafeTkRoot(self)
      dlg = ChangeSubstModeDlg(tk, self.sel0kopt, self.changeKopt)
      tk.wait_window(dlg)
    except:
      if tk:
        tk.destroy()
      raise

if checkTk():
  ChangeSubstModeTk()