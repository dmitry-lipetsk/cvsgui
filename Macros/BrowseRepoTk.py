from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.Macro import *
from cvsgui.SafeTk import *
from cvsgui.Persistent import *
import os, tempfile, shutil
import os.path

if checkTk():
  from Tkinter import *

"""
  WinCvs Macro "Browse Repository Folders"
  $Revision: 1.7 $

  written by Oliver Giesen, Aug 2002 - Sep 2003
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
   from within WinCvs! This macro has last been tested against WinCvs 2.0.3
   and Python 2.4.1 .

  ======
  Usage:

  -Run the Macro from the "Admin|Macros admin" menu.

  -Enter the CVSROOT of the repository you want to browse.

  -Hit the "Browse" button at the bottom of the window.

  ~A list of top-level modules should appear

  -If you want to browse deeper into the folder hierarchy, just doubleclick
   the respective module or alternatively select it and press "Browse" again.
   The deeper-level folders (if there are any) will be shown below the
   selected one with their fully-qualified name (i.e. the name you would
   have to use for Checkout).

  -If you want to checkout a module, select it and hit "Checkout".

  ~You will be queried for the target directory of the checkout - default is
    the current working directory.

  ~You will be queried for additional checkout arguments. Don't put spaces
   between arguments and their parameters!
   Examples:
     -rTAG     Checks out a specific tag
     -dFolder  Overrides the  module name and instead checks out to
                the specified folder name
     -l        Don't recurse

  ~The selected module will be checked out to the specified location.

  =============
  Known Issues / "Un-niceties":

  - The persistency is not quite working as I'd like to yet. Values are only
   remembered during the current WinCvs session
   (or until you invoke "ReloadMacro")

  - It'd be nice if the information could be cached somehow, so it wouldn't
   have to be refetched every time. I will probably not look into this until
   the Persistency issues are resolved.

  - It'd be nice if the CVSROOT wouldn't have to be entered manually.
   I have however not yet found a way to detect the current CVSROOT
   (as per Admin|Preferences/General) and I'm pretty sure there is no
   such interface (yet) for Python macros in WinCvs.

  - It'd be even nicer, if I was able to invoke the Checkout dialog from
   within the macro, so I could pass the selected module to it. We'll see
   if the WinCvs-Python-API will eventually get such an interface.

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

defaultRepo = ':pserver:anonymous@cvs.cvsgui.sf.net:/cvsroot/cvsgui'

def getModuleList( root, module='.', rev='HEAD', tmpdir = None):
  def fakeCvsDir( tmpdir, root, repo):
    def writeToFile( file, line):
      f_file = open( file, 'w')
      try:
        f_file.write( line)
      finally:
        f_file.close()

    cvsDir = os.path.join( tmpdir, 'CVS')
    if not os.path.exists( cvsDir):
      os.makedirs( os.path.join( cvsDir, 'CVS'))
    writeToFile( os.path.join( cvsDir, 'Root'), root)
    writeToFile( os.path.join( cvsDir, 'Repository'), repo)
    writeToFile( os.path.join( cvsDir, 'Entries'), '\n')

  result = []
  if tmpdir == None:
    tmpdir = os.path.join( tempfile.gettempdir(), 'cvsrlist')
  fakeCvsDir( tmpdir, root, module)
  cvs = Cvs( 1, 0)
  cwdbup = os.getcwd()
  try:
    os.chdir( tmpdir)
    code, out, err = cvs.Run( '-n', 'up', '-d', '-r'+rev)
    if code == 0 and err:
      rex = re.compile( "New directory \`([^\']+)\' \-\- ignored")
      for line in err.split('\n'):
        m_module = rex.search( line)
        if m_module:
          result.append( m_module.group(1))
  finally:
    os.chdir(cwdbup)
    if os.path.exists(tmpdir):
      shutil.rmtree(tmpdir)
    return result


class RList_Dlg(Frame):
  def __init__(self, master, root):
    Frame.__init__(self, master)
    self.pack(fill=BOTH, expand=YES)
    self.master.title('Browse Repository Folders')

    self.p_root = root
    self.cvsroot = StringVar()
    self.cvsroot.set(str(self.p_root))
    self.createWidgets()

    #attempt to center the window on screen:
    self.update()
    x = ( self.winfo_screenwidth() - self.winfo_width()) / 2
    y = ( self.winfo_screenheight() - self.winfo_height()) / 2
    self.master.geometry( newGeometry = '+%d+%d' % (x,y))

  def createWidgets( self):
    Label(self, text="Enter the CVSROOT and hit Browse.\nTo retrieve submodules double-click on a module.")\
         .grid(row=0, column=0, sticky=W+N+E, columnspan=3)
    Label( self, text="CVSROOT:")\
         .grid( row=1, column=0, sticky=W+N, pady=5)
    Entry( self, textvariable=self.cvsroot)\
         .grid( row=1, column=1, sticky=W+N+E, columnspan=2, pady=5)

    moduleScroll = Scrollbar( self, orient=VERTICAL)
    moduleScroll.grid( row=2, column=2, sticky=N+S+E)
    self.modulesBox = Listbox( self, yscrollcommand=moduleScroll.set, \
                               bg="SystemWindow")
    self.modulesBox.bind( '<Double-Button-1>', self.Browse)
    self.modulesBox.grid( row=2, column=0, sticky=N+W+S+E, columnspan=2)
    moduleScroll.config( command=self.modulesBox.yview)

    buttons_Frame = Frame(self)
    buttons_Frame.grid(row=3, column=0, columnspan=3, sticky=W+E)
    Button(buttons_Frame, text="     Browse    ", command=self.Browse)\
          .grid(row=0, column=0, sticky=W)

    Button(buttons_Frame, text="  Checkout...  ", command=self.Checkout)\
          .grid(row=0, column=1, sticky=W+E)

    Button(buttons_Frame, text="     Quit      ", command=self.master.destroy)\
          .grid(row=0, column=2, sticky=E)

    self.rowconfigure( 2, weight=1)
    self.columnconfigure( 1, weight=1)

  def GetSelection(self):
    sel = self.modulesBox.curselection()
    try:
      sel = map( int, sel)
    except ValueError: pass

    if len( sel) == 0:
      idx = END
      module = '.'
      prefix = ''
      self.modulesBox.delete( 0, END)
    else:
      idx = sel[0]+1
      module = self.modulesBox.get( sel[0])
      prefix = module+'/'
    return idx, module, prefix

  def Browse( self, event = None):
    idx, module, prefix = self.GetSelection()
    self['cursor'] = 'watch'
    try:
      modules = getModuleList( self.cvsroot.get(), module)
      for module in modules:
        self.modulesBox.insert( idx, prefix+module)
    finally:
      self['cursor'] = 'arrow'

    self.p_root << self.cvsroot.get()

  def Checkout(self, event = None):
    idx, module, prefix = self.GetSelection()
    cvs = Cvs( 0, 0)
    cwdbup = os.getcwd()
    try:
      dlgTitle='Checkout module ' + module
      answer, targetDir = App.CvsPrompt(cwdbup, 'Question', 'Checkout to:', alertTitle=dlgTitle)
      if answer=='IDOK':
        p_args = Persistent('PY_RListCOArgs', '', 1)
        answer, args = App.CvsPrompt(str(p_args), 'Question', 'Additional Checkout arguments:', alertTitle=dlgTitle)
        if answer=='IDOK':
          p_args << args
          args = string.split(args)
          args.append(module)
          os.chdir( targetDir)
          args = ['-q', 'checkout'] + args
          if self.cvsroot.get():
            args.insert(0, '-d'+self.cvsroot.get())
          print 'cvs %s (in %s)' % (string.join(args), os.getcwd())
          cvs.Run(*args)
    finally:
      os.chdir(cwdbup)
      self.master.destroy()


class BrowseRepoTk( Macro):
  def __init__( self):
    Macro.__init__( self, 'Browse Repository Folders...',\
                    MACRO_ADMIN, 0)
    self.p_root = Persistent( 'P_RListRoot', defaultRepo, 1)

  def OnCmdUI( self, cmdui):
    cmdui.Enable( 1)

  def Run( self):
    try:
      tk = SafeTkRoot( self)
      dlg = RList_Dlg( tk, self.p_root)
      tk.wait_window( dlg)
    except:
      if tk:
        tk.destroy()
      raise

if checkTk():
  BrowseRepoTk()
