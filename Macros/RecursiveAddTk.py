from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.Persistent import *
from cvsgui.SafeTk import *
from cvsgui.SubstModeTk import *
import os, os.path
import string

if checkTk():
  from Tkinter import *

"""
  CvsGui Macro "Recursive Add with auto-commit option"
  $Revision: 1.12 $

  written by Oliver Giesen, Aug 2002 - Oct 2004
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

  -Select one or more non-CVS folders

  -Run the Macro from the "Macros|Add" menu.

  ~A question dialog will pop up asking whether you want to want ignored
   files to be included in the selection list.

  -Answer Yes or No accordingly

  ~You will be presented with a dialog in which you could specify which
   keyword substitution mode to use for which file extension (the defaults could
   be configured via the Recursive Add Options dialog).

  -The actual UI for specifying file modes depends on the setting of the
   "Advanced File Flags UI" option:
   - In "Advanced" mode (the default) you will be able to enter the "k-mode"
     for each file type either manually or by selecting the relevant items on
     the popup-menu which is available via the ">" button next to each entry.
     Alternatively you could choose to ignore a file type altogether by ticking
     the corresponding checkbox.
   - In "Simple" or "Classic" mode you could only choose between "Ignore",
     "Binary", "Unicode", "Server Default" and "No Kw" using radio buttons.

  ~The right hand pane of the window allows you to set the options for
   "auto-commit", i.e. to automatically commit the newly added files.

   -If you want to automatically commit the added files, tick the
    "Auto-commit" checkbox.

   -Enter the commit comment for the new files.

   -If you want to specify additional commit options (e.g. -r to force
    a revision number) you could do so in
    the entry box at the bottom of the dialog. just type them as you
    would on the commandline (on invokation they will be used between
    "cvs commit" and the file names).

  -Hit the "Add!" button if you want the files and folders to be added or
   "Cancel" if you want to abort.

  =============
  Known Issues / "Un-niceties":

  - There is no detection of text/binary being done. The initial classification
   is solely based on the default lists that could be specified via the
   Recursive Add Options dialog.
   I haven't yet found a suitable algorithm to implement this but might well do
   so in the future.
   
  - The classifications done by the user are currently not remembered. This is
   quite high on my ToDo-list.

  - I have not yet found a way to make the window scrollable, hence when
   there are a lot of file types the dialog might get very big to the extent
   that it might not even fit on the screen anymore.

  - I have not yet found a way to sort the list of file extensions. They are
   currently displayed in the order they were found. Will work on this.

  - The auto-commit settings, i.e. the log comment and additional options
   are currently not remembered. This as well is at the top of the To Do list.

  - The advanced "k-flags" UI used by this macro still has some quirks of its 
   own, see comments inside the cvsgui.CvsSubstModeTk module for details.

  - The dialog is not truly modal until you run the underlying CVS command.
   It is sometimes possible to work in WinCvs without closing the dialog.
   Although I haven't experienced any serious problems with this, you should
   definitely be careful about this.

  - If for some reason the macro crashes and is restarted via ReloadMacros,
   additional windows might pop up. I haven't found a way to prevent this yet.
   AFAICT they don't do any harm either, though.

  Please report any problems you encounter or suggestions you
    might have to ogware@gmx.net .
  
"""

DefExtensions = {}

# Defaut Binary extensions:
DefExtensions['b'] = ['.doc', '.exe', '.dll', '.res', '.dcr', '.obj', \
                      '.bpl', '.ocx', '.zip', '.rar', '.gz', '.tar', \
                      '.xls', '.lzh', '.ico', '.bmp', '.jpg', '.gif', \
                      '.jpeg', '.com', '.pyc', '.tlb', '.hlp', '.pdf', \
                      '.cab', '.msi', '.mst', '.png', '.dcp', '.class', \
                      '.tif', '.tiff', '.mdb', '.mde']

# Default Unicode extensions
DefExtensions['u'] = [] #don't know any unicode file format

# Extensions w/o keyword expansion:
DefExtensions['o'] = ['.dfm', '.cfg', '.dof', '.dti']

# Ignored extensions:
DefExtensions['i'] = ['.bup', '.bak', '.~pas', '.~dpr', '.~dfm', '.~dpk']

ExtensionTypes = DefExtensions.keys()

KModes = ['i', '-kb', '-ku', ' ', '-ko']

KMODE = 0
FILES = 1

icon_xpm = \
  "13 12 5 1\n" + \
  ". c None\n" + \
  "# c #808080\n" + \
  "@ c #000000\n" + \
  "+ c #ff0000\n" + \
  "* c #ffee80\n" + \
  "...#####.....\n" + \
  "..#*****#....\n" + \
  ".#####**####.\n" + \
  "#*****#*****@\n" + \
  "#*****####**@\n" + \
  "#***++****@*@\n" + \
  "#***++****@*@\n" + \
  "#*++++++**@*@\n" + \
  "#*++++++**@@@\n" + \
  "#***++****@..\n" + \
  "#***++****@..\n" + \
  "@@@####@@@@.."
  #0123456789012345

class RecurAddOptions:
  def __init__(self):
    global DefExtensions
    self._advUI = Persistent('P_RecurAdd_AdvUI', '1', 0)
    self._ExtVals = {}
    for type, valname in [('b', 'P_RecurAdd_BExts'), \
                          ('u', 'P_RecurAdd_UExts'), \
                          ('o', 'P_RecurAdd_OExts'), \
                          ('i', 'P_RecurAdd_IExts')]:
      self._ExtVals[type] = Persistent(valname, string.join(DefExtensions[type]), 0)
      DefExtensions[type] = self._ExtVals[type].GetValue().split()

  def get_advUI(self):
    return self._advUI.GetValue() == '1'

  def set_advUI(self, value):
    if value:
      value = '1'
    else:
      value = '0'
    self._advUI.SetValue(value)

  def get_DefExts(self, type):
    return string.split(self._ExtVals[type].GetValue())

  def set_DefExts(self, type, value):
    global DefExtensions
    self._ExtVals[type].SetValue(string.join(value))
    DefExtensions[type] = value  
    
class RecursiveAddOptionsDlg(Frame):
  def __init__(self, master):
    Frame.__init__(self, master)
    self.master.title('Recursive Add Options')
    
    self.options = RecurAddOptions()
    self.advUI = BooleanVar()
    self.advUI.set(self.options.get_advUI())

    self.createWidgets()
    self.pack(fill=BOTH, expand=1)

    #attempt to center the window on screen:
    self.update()
    w = self.winfo_width()
    h = self.winfo_height()
    x = (self.winfo_screenwidth() - w) / 2
    y = (self.winfo_screenheight() - h) / 2
    self.master.geometry(newGeometry = '%dx%d+%d+%d' % (w,h,x,y)) # '+%d+%d' % (x,y))

  def createWidgets(self):
    Checkbutton(self, text='Use Advanced File Flags UI', variable=self.advUI)\
               .grid(row=0, column=0, columnspan=2, sticky=N)

    self.ExtEntries = {}
    rid = 1
    cid = 0
    for type, text in [('b', 'Default Binary Extensions'), \
                       ('u', 'Default Unicode Extensions'), \
                       ('o', 'Default Extensions w/o kw expansion'), \
                       ('i', 'Ignored Extensions')]:
      Label(self, text=text).grid(column=cid, row=rid*2-1, sticky=N+W)
      self.ExtEntries[type] = Text(self, width=30, height=5, wrap=WORD)
      self.ExtEntries[type].grid(row=rid*2, column=cid, sticky=N+W, padx=5)
      self.ExtEntries[type].insert(END, string.join(self.options.get_DefExts(type)))
      rid += 1
      if rid > 2:
        rid = 1
        cid += 1

    Button(self, text='OK', command=self.save)\
          .grid(column=0, row=5, padx=5, pady=5, sticky=W+E+N)
    Button(self, text='Cancel', command=self.master.destroy)\
          .grid(row=5, column=1, padx=5, pady=5, sticky=W+E+N)

  def save(self):
    self.options.set_advUI(self.advUI.get())
    for type in ['b', 'u', 'o', 'i']:
      self.options.set_DefExts(type, self.ExtEntries[type].get(1.0, END).strip().split())
                    
    del self.options
    self.master.destroy()

class RecursiveAddOptionsTk(Macro):
  def __init__(self):
    Macro.__init__(self, 'Recursive Add Options...', MACRO_SELECTION, 0, 'Add')

  def OnCmdUI(self, cmdui):
    cmdui.Enable(1)

  def Run(self):
    try:
      tk = SafeTkRoot(self)
      dlg = RecursiveAddOptionsDlg(tk)
      tk.wait_window(dlg)
    except:
      if tk:
        tk.destroy()
      raise
    
    
class AutoCommitOptions:
  pass

class AddSubstModeUI(CvsSubstModeUI):
  def __init__(self, master, var):
    self.Ignore = BooleanVar()
    self.Ignore.set(var.get() == 'i')
    CvsSubstModeUI.__init__(self, master, var)
    self.doIgnore()

  def createWidgets(self):
    Checkbutton(self, text='Ignore', variable=self.Ignore, command=self.doIgnore)\
               .grid(row=0, column=0, sticky=W)
    CvsSubstModeUI.createWidgets(self)

  def setFlags(self):
    if self.Ignore.get():
      self.var.set('i')
    else:
      CvsSubstModeUI.setFlags(self)

  def doIgnore(self):
    if self.Ignore.get():
      state = DISABLED
    else:
      state = NORMAL
    self.btn.configure(state=state)
    self.entry.configure(state=state)
    self.label.configure(state=state)
    self.setFlags()

    
class RecursiveAddDlg(Frame):
  def __init__(self, master, items, doItProc):
    Frame.__init__(self, master)
    self.master.title('Recursive Add')
    
    self.autoCommit = BooleanVar()
    self.commitOpts = StringVar()

    self.doItProc = doItProc
    
    self.createWidgets(items)
    self.pack(fill=BOTH, expand=1)

    #attempt to center the window on screen:
    self.update()
    w = self.winfo_width()
    h = min( self.winfo_height(), (max( 4, len( items)) * 30 + 40))
    x = (self.winfo_screenwidth() - w) / 2
    y = (self.winfo_screenheight() - h) / 2
    self.master.geometry(newGeometry = '%dx%d+%d+%d' % (w,h,x,y)) # '+%d+%d' % (x,y))


  def createWidgets(self, items):
    filesFrm = Frame(self, relief=GROOVE, bd=2)
    filesFrm.grid(row=0, column=0, rowspan=2, sticky=N+E+S, padx=2, ipadx=2, pady=2)

    advUI = RecurAddOptions().get_advUI() #Persistent('P_RecurAdd_AdvUI', '0', 1).GetValue() == '1'
    
    cidx = 0
    labels = ['Extension']
    if advUI:
      labels += ['File flags']
    else:
      labels += ['ignore', 'binary', 'unicode', 'server default', 'no kw']
      
    for label in labels:
      Label(filesFrm, text=label).grid(row=0, column=cidx)
      cidx+=1
    
    ridx=1
    for ext, itm in items.items():
      Label(filesFrm, text='%s [%d files]' % (ext,len(itm[FILES])), height=1)\
           .grid(row=ridx, column=0, sticky=W+N)

      if advUI:
        AddSubstModeUI(filesFrm, itm[KMODE]).grid(row=ridx, column=1, sticky=W+N)
      else:
        cidx = 1
        for kmode in KModes:
          Radiobutton(filesFrm, height=0, \
                      variable=itm[KMODE], value=kmode)\
                     .grid( row=ridx, column=cidx, sticky=N)
          cidx+=1
      ridx+=1

    commitFrm = Frame(self, relief=GROOVE, bd=2)
    commitFrm.grid(row=0, column=1, columnspan=2, sticky= N+E+W+S, padx=2, ipadx=2, pady=2)

    Checkbutton(commitFrm, text='Auto-commit', variable=self.autoCommit)\
    .grid(row=0, columnspan=2, sticky=W)
    Label(commitFrm, text='commit message').grid(row=1, column=0, columnspan=2, sticky=W)
    self.commitMsgTxt = Text(commitFrm, width=20, height=10)
    self.commitMsgTxt.grid(row=2, columnspan=2, sticky=N+E+W+S, padx=2)
    Label(commitFrm, text='additional commit options').grid(row=3, column=0, sticky=W)
    Entry(commitFrm, textvariable=self.commitOpts).grid(row=3, column=1, padx=2, sticky=W+E)
    commitFrm.rowconfigure(2, weight=2)
    commitFrm.columnconfigure(0, weight=3)
    commitFrm.columnconfigure(1, weight=2)

    Button(self, text='Add!', command=self.DoIt)\
          .grid(column=1, row=1, padx=5, pady=5, sticky=W+E+N)
    Button(self, text='Cancel', command=self.master.destroy)\
          .grid(row=1, column=2, padx=5, pady=5, sticky=W+E+N)

    self.columnconfigure(1, weight=2)
    self.rowconfigure(0, weight=1)

  def DoIt(self):
    self['cursor']='watch'
    try:
      autoCommitOptions = AutoCommitOptions()
      autoCommitOptions.autoCommit = self.autoCommit.get()
      if autoCommitOptions.autoCommit:
       autoCommitOptions.commitOptions = self.commitOpts.get()
       autoCommitOptions.commitMessage = self.commitMsgTxt.get( 1.0, END).strip()
      self.doItProc(autoCommitOptions)
    finally:
      self['cursor']='arrow'
      self.master.destroy()


class CvsRecursiveAddTk(Macro):
  def __init__(self):
    Macro.__init__(self, 'Recursive Add (auto-commit)...', MACRO_SELECTION, 0, 'Add', icon_xpm)

  def OnCmdUI(self, cmdui):
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    cmdui.Enable(enabled)

  def collectItems(self, entries, inclIgnored, tkMaster):
    """ prepare lists of files and folders to be added

      will recursively scan the given entries list for unknown files and folders
      and associate each distinct extension with a kmode and a list of files in
      the self.items dictionary like this:
      
      { ext1:{ KMODE:kmode1,
               FILES:[file1,file2,...]},
        ext2:{ KMODE:kmode2,
               FILES:[file3,file4,...]},
        ...}

      If inclIgnored is false, no keys will be created for ignored files.

      The kmodes are stored in StringVar instances so they could easily be
      reused by the Tk GUI. For this reason the Tk master has to be given.
    """
    for entry in entries:
      if entry.IsUnknown():
        if entry.IsFile():
          #extract extension:
          root, ext = os.path.splitext(entry.GetName())
          ext = string.lower(ext)

          #determine default keyword mode:
          if entry.IsIgnored():
            if inclIgnored:
              kmode = KModes[0]
            else: #skip ignored files
              continue
          elif ext in DefExtensions['b']:
          #elif ext in string.split(self.P_BinExtensions.GetValue()):
            kmode = KModes[1]
          elif ext in DefExtensions['u']:
          #elif ext in string.split(self.P_UniExtensions.GetValue()):
            kmode = KModes[2]
          elif ext in DefExtensions['o']:
          #elif ext in string.split(self.P_NoKwExtensions.GetValue()):
            kmode = KModes[4]
          else:
            kmode = KModes[3]

          #create dictionary entry for extension if necessary:
          if ext not in self.items:
            self.items[ext] = {}

          #apply keyword mode
          if KMODE not in self.items[ext]:
            self.items[ext][KMODE] = StringVar(tkMaster)
          self.items[ext][KMODE].set(kmode)

          #initialize file list if necessary:          
          if FILES not in self.items[ext]:
            self.items[ext][FILES] = []

          #append file list:
          self.items[ext][FILES].append(entry.GetFullName())

      #recurse into folder:
      if not entry.IsFile():
        print '\t'+entry.GetFullName()
        self.collectItems(GetCvsEntriesList(entry.GetFullName()), inclIgnored, tkMaster)
    
  def addFolder(self, folder, cvs):
    folderCount = 0
    if os.path.exists( folder):
      parent, dir = os.path.split( folder)
      if not os.path.exists( os.path.join( parent, 'CVS')):
        folderCount += self.addFolder( parent, cvs)
      os.chdir( parent)
      #print 'cvs add %s (in %s)\n' % (dir, os.getcwd())
      code, out, err = cvs.Run( 'add', dir)
      if code == 0:
        print '\t'+folder
        folderCount+=1
      else:
        cvs_err(err + '\n\n')
        cvs_err('Commandline was:\ncvs add %s (in %s)\n\n' % (string.join(cvsArgs), os.getcwd()))
      return folderCount
    else:
      raise Exception, 'Invalid sandbox!'


  def commitFiles( self, files, autoCommitOptions, cvs):
    cvsArgs = []
    if autoCommitOptions.commitOptions != '':
      cvsArgs += autoCommitOptions.commitOptions.split()
    if autoCommitOptions.commitMessage != '':
      cvsArgs.append( '-m'+autoCommitOptions.commitMessage)
    cvsArgs += files
    #print 'cvs commit %s (in %s)\n\n' % (string.join(cvsArgs), os.getcwd())
    code, out, err = cvs.Run( 'commit', *cvsArgs)
    if code == 0:
      return len( files)
    else:
      cvs_err(err + '\n\n')
      cvs_err('Commandline was:\ncvs commit %s (in %s)\n\n' % (string.join(cvsArgs), os.getcwd()))
      return 0

  def DoIt(self, autoCommitOptions):
    folderCount = 0
    fileCount = 0
    commitCount = 0
    try:
      cvs = Cvs( 1, 0)
      #create batches grouped by parent folder and kmode
      batches = {}
      print 'reorganizing file list...'
      for ext, item in self.items.items():
        kmode = item[KMODE].get()
        if not kmode == 'i':
          for file in item[FILES]:
            parent, filename = os.path.split( file)
            if parent not in batches:
              batches[parent] = {}
            if kmode not in batches[parent]:
              batches[parent][kmode] = []
            batches[parent][kmode].append( filename)

      #run batches:
      print 'adding files and folders...'
      for dir, batch in batches.items():
        #ensure folder is added already:
        if not os.path.exists(os.path.join( dir, 'CVS')):
          folderCount += self.addFolder(dir, cvs)

        os.chdir(dir)
        
        for kmode, files in batch.items():
          cvsArgs = files
          if kmode.strip() <> '': # != KModes[3]:
            cvsArgs = [kmode] + cvsArgs
          #print 'cvs add %s (in %s)\n\n' % (string.join( cvsArgs), os.getcwd())
          code, out, err = cvs.Run('add', *cvsArgs)
          if code == 0:
            fileCount+=len(files)
            if autoCommitOptions.autoCommit:
              commitCount += self.commitFiles(files, autoCommitOptions, cvs)
          else:
            cvs_err(err + '\n\n')
            cvs_err('Commandline was:\ncvs add %s (in %s)\n\n' % (string.join(cvsArgs), os.getcwd()))

    finally:
      print 'Added %d folders and %d files' % (folderCount, fileCount)
      if autoCommitOptions.autoCommit:
        print 'Committed %d files' % commitCount

  def Run(self):
    try:
      incIgn = App.CvsAlert('Question', 'Offer ignored files for inclusion?', \
                            'Do you intend to add files that would otherwise be ignored by CVS?', \
                            'Yes', 'No', '', '', 'Recursive Add') == 'IDOK'
      self.items = {}
      self.folders = []
      try:
        #initialize extension lists from persistent settings:
        RecurAddOptions()
        tk = SafeTkRoot(self)
        print 'analyzing selection...'
        self.collectItems(self.sel, incIgn, tk)
        dlg = RecursiveAddDlg(tk, self.items, self.DoIt)
        tk.wait_window(dlg)
      finally:
        del self.items
        del self.folders
    except:
      if tk:
        tk.destroy()
      raise

if checkTk():
  CvsRecursiveAddTk()
  MenuSeparator(MACRO_SELECTION, 'Add')
  RecursiveAddOptionsTk()