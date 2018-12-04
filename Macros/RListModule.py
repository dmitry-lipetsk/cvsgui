from cvsgui.App import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
from cvsgui.Macro import *
from cvsgui.Persistent import *
import os, os.path
import re, string
import tempfile, shutil

"""
  WinCvs Macro "List Module Contents"
  (intended as a substitute for CVSNT's ls command when working
   against servers which do not support cvs ls.)
  $Revision: 1.4 $

  written by Oliver Giesen, Aug 2002
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

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.0.3
   and Python 2.4.1 .

  ======
  Usage:

  - Run the macro from the "Admin|Macros admin" menu or simply press
    Ctrl-Shift-M (often doesn't work)

   ~You will be queried for the CVSROOT of the repository you want to browse.

  - Enter or confirm the CVSROOT and press OK.

   ~You will be queried for the module you want to list.

  - Enter or confirm the module (enter a period if you want to list the root
    modules) and press OK.

   ~You will be queried for a revision or branch you wish to filter the output
    by. (note that directories will be listed regardless of this filter)

  - Enter or confirm the filter (enter HEAD if you do not want to apply a
    revision filter) and press OK.

   ~The contents of the given module will be dumped to the console.
    Directories will be displayed first and are put in brackets.

  - If you want to browse deeper into one of the listed directories, just invoke
    the macro again and modify the module parameter accordingly. All previous
    entries will be remembered (although only for the time of the current
    WinCvs session).
  
  =============
  Known Issues / "Un-niceties":

  - The keyboard shortcut does not work most of the time.
 
  - It'd be nice if the CVSROOT wouldn't have to be entered manually.
    I have however not yet found a way to detect the current CVSROOT
    (as per Admin|Preferences/General) and I'm pretty sure there is no
    such interface (yet) for Python macros in WinCvs.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
  
"""

def writeToFile( file, line):
  f_file = open( file, 'w')
  try:
    f_file.write( line)
  finally:
    f_file.close()
        
def fakeCvsDir( tmpdir, root, repo):
  cvsDir = os.path.join( tmpdir, 'CVS')
  if not os.path.exists( cvsDir):
    os.makedirs( os.path.join( cvsDir, 'CVS'))
  writeToFile( os.path.join( cvsDir, 'Root'), root)
  writeToFile( os.path.join( cvsDir, 'Repository'), repo)
  writeToFile( os.path.join( cvsDir, 'Entries'), '\n')
  os.chdir( tmpdir)
  
class RListModuleConfig:
  """Container class for persistent settings of the ListRemoteModule macro.
  """
  def __init__(self):
    self.root = Persistent( 'P_RListRoot', '', 1)
    self.module = Persistent( 'P_RListModule', '.', 1)
    self.rev = Persistent( 'P_RListRevision', 'HEAD', 1)

class ListRemoteModule( Macro):
  """CvsGui Macro to non-recursively list the contents of a remote module.
  """
  def __init__( self, config):
    Macro.__init__( self, 'List &Module Contents\tCtrl+Shift+M',\
                    MACRO_ADMIN, 0)
    self.config = config

  def OnCmdUI( self, cmdui):
    cmdui.Enable( 1)

  def Run( self):
    dlgTitle = 'List Module Contents'
    answer, cvsroot = App.CvsPrompt(str(self.config.root), 'Question', 'Enter CVSROOT.', \
                                    alertTitle=dlgTitle)
    if answer == 'IDOK':
      self.config.root << cvsroot
      answer, module = App.CvsPrompt(str(self.config.module), 'Question', 'Enter Module.', \
                                     '(enter a period (".") to get the top-level modules)', \
                                     alertTitle=dlgTitle)
    if answer == 'IDOK':
      self.config.module << module
      answer, rev = App.CvsPrompt(str(self.config.rev), 'Question', 'Enter Revision/Branch.', \
                                  alertTitle=dlgTitle)

    if answer == 'IDOK':
      self.config.rev << rev
      tmpdir = os.path.join( tempfile.gettempdir(), 'cvsrlist')
      cwdbup = os.getcwd()
      try:
        fakeCvsDir( tmpdir, cvsroot, module)

        cvs = Cvs( 1, 0)
        console = ColorConsole()
        code, out, err = cvs.Run( '-n', 'up', '-d', '-r'+rev)

        if code == 0:
          repo = string.join( cvsroot.split( ':')[2:], ':')
          repo = repo[repo.find('@')+1:]
          console << '\nContents of module ' << kBold << module << '\n'
          console << kNormal << '(Repository ' << kBold << repo << kNormal << '):\n'
          console << kMagenta << '='*60 << '\n' << kNormal
          rex = re.compile( "New directory \`([^\']*)\' \-\- ignored")
          if err:
            for line in err.split('\n'):
              m_module = rex.search( line)
              if m_module:
                console << kGreen << '[%s]' % m_module.group(1) << '\n' << kNormal

          if out:
            rex = re.compile( "U (.*)$")
            for line in out.split('\n'):
              m_module = rex.search( line)
              if m_module:
                console << kBlue << m_module.group(1) << '\n' << kNormal
        else:
          console << kRed << err << kNormal
      finally:
        os.chdir(cwdbup)
        if os.path.exists(tmpdir):
          shutil.rmtree(tmpdir)

config = RListModuleConfig()
ListRemoteModule(config)
