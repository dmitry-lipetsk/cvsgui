from cvsgui.App import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
from cvsgui.Macro import *
from cvsgui.Persistent import *
import os, os.path
import shutil

"""
  WinCvs Macro "Add Root Module"
  $Revision: 1.5 $

  written by Oliver Giesen, Dec 2002
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

  - Select a non-CVS folder from the tree-/file-view
  
  - Run the macro from the Macros|CVS menu

   ~You will be asked for the CVSROOT of the repository to add the module to

  - Enter or accept the CVSROOT and press OK

   ~The directory will be added as a new root-level module to the
    specified repository

  =============

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
  
"""

icon_xpm = \
  "13 12 5 1\n" + \
  ". c None\n" + \
  "# c #808080\n" + \
  "@ c #000000\n" + \
  "+ c #ff0000\n" + \
  "* c #ffee80\n" + \
  ".............\n" + \
  "..#####......\n" + \
  ".#*****#.....\n" + \
  ".#*****####..\n" + \
  ".#***++****@.\n" + \
  ".#***++****@.\n" + \
  ".#*++++++**@.\n" + \
  ".#*++++++**@.\n" + \
  ".#***++****@..\n" + \
  ".#***++****@..\n" + \
  ".@@@####@@@@..\n" + \
  ".............."

  
class AddRootModule(Macro):
  def __init__(self):
    Macro.__init__(self, 'Add Root Module...', MACRO_SELECTION, 0, 'CVS', icon_xpm)

  def OnCmdUI(self, cmdui):
    sel = App.GetSelection()
    enabled =    (len(sel) == 1) \
              and(not sel[0].IsFile()) \
              and(sel[0].IsUnknown())
    if enabled:
      self.dir = sel[0].GetFullName()
    cmdui.Enable(enabled)

  def runCvs(self, cvs, console, *args):
    code, err, out = cvs.Run(*args)
    if code <> 0:
      console << kRed
      if err:
        console << err
      else:
        console << out
      console << '\n' << kNormal
    return code == 0
      
  def Run(self):
    basedir, module = os.path.split(self.dir)
    console = ColorConsole()
    if os.path.exists(os.path.join(basedir, 'CVS')):
      console << kRed << 'Parent directory may not be CVS-controlled!\n' << kNormal
      return
    msg = 'Enter the CVSROOT of the repository you wish to add the new root module "%s" to!' % module
    title = 'Add Root Module'
    answer, root = App.CvsPrompt(str(Persistent('P_CVSROOT', '', 0)), 'Question', msg, alertTitle=title)
    if not (answer=='IDOK' and len(root)>0):
      print 'Operation cancelled by user.'
      return
    
    os.chdir(basedir)
    cvs = Cvs(1,0)
    print 'Preparing sandbox...'
    if not self.runCvs(cvs, console, '-d'+root, 'co', '-l', '.'):
      return

    print 'Creating new module %s/%s...' % (root, module)
    if not self.runCvs(cvs, console, 'add', module):
      return

    print 'Cleaning up...'
    shutil.rmtree(os.path.join(basedir, 'CVS'))

    print 'Done.'

AddRootModule()
