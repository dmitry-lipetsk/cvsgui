from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *
import os, os.path
import string

"""
  WinCvs Macro "Tag/Branch and Update"
  $Revision: 1.3 $

  written by Oliver Giesen, May 2003 - Jul 2005
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    icq:    18777742

  Feel free to modify or distribute in whichever way you like, 
   as long as it doesn't limit my personal rights to modify and
   redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.0.3 .

  ======
  Usage:

  - Select one or more CVS-controlled files or folders.

  - Run the macro from the Macros|CVS submenu.

   ~ You will be asked to enter the name of the new tag/branch you wish to create.

  - Enter the name of the tag/branch and confirm with OK.

   ~ The tag/branch will be created and if this is successful, the selection will be
     updated to that branch.
     
  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

branch_icon_xpm = \
  "13 12 5 1\n" + \
  ". c None\n" + \
  "r c #ff0000\n" + \
  "g c #00aa00\n" + \
  "b c #0000ff\n" + \
  "* c None\n" + \
  ".ggggggggg...\n" + \
  ".ggggggggg...\n" + \
  ".........g...\n" + \
  ".......ggggg.\n" + \
  "..b.....ggg..\n" + \
  "bbbbb....g...\n" + \
  "b...brrrrrr..\n" + \
  "b...br....r..\n" + \
  "bbbbb...rrrrr\n" + \
  "..b.....r...r\n" + \
  "........r...r\n" + \
  "..b.....rrrrr"

tag_icon_xpm = \
  "13 12 5 1\n" + \
  ". c None\n" + \
  "- c #eeeeee\n" + \
  "g c #00aa00\n" + \
  "~ c #dfdfdf\n" + \
  "* c #808080\n" + \
  ".............\n" + \
  ".*********...\n" + \
  ".*-~~~~~~**..\n" + \
  ".*~gggggg*-*.\n" + \
  ".*~gggggg****\n" + \
  ".*~g~*gg*~g~*\n" + \
  ".*-~-~gg~-~-*\n" + \
  ".*---~gg~---*\n" + \
  ".*---*gg*---*\n" + \
  ".*--~gggg~--*\n" + \
  ".*---~~~~---*\n" + \
  ".************"

class CustomTagAndUpdate(Macro):
  def __init__(self, cmdname, cmdoptions, icon):
    self.cmdname = cmdname
    self.cmdoptions = cmdoptions
    Macro.__init__(self, self.cmdname+' and update...', MACRO_SELECTION, 0, 'CVS', icon)

  def OnCmdUI(self, cmdui):
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    if enabled:
      for entry in self.sel:
        if entry.IsUnknown():
          enabled = 0
          break
    cmdui.Enable(enabled)

  def Run(self):
    cwdbup = os.getcwd()
    try:
      cvs = Cvs(1,0)
      msg = 'Enter the name of the %s you wish to create:'%self.cmdname
      title = self.cmdname + ' and Update'
      answer, branch = App.CvsPrompt('', 'Question', msg, alertTitle=title)
      if (answer == 'IDOK') and len(branch) > 0:
        args = []
        prepareTargets(self.sel, args)
        print 'Creating %s "%s" on selection...'%(self.cmdname, branch)
        cmdargs = ['-q', 'tag'] + self.cmdoptions + [branch] + args  #'-c', 
        #print '\tcvs %s (in %s)'%(string.join(cmdargs), os.getcwd())
        code, out, err = cvs.Run(*cmdargs)
        if code == 0:
          print 'Updating selection to %s "%s"...'%(self.cmdname, branch)
          #print '\tcvs -q update -r%s %s (in %s)'%(branch, string.join(args), os.getcwd())
          code, out, err = cvs.Run('-q', 'up', '-r'+branch, *args)
          if code == 0:
            print 'Done.'
          else:
            print 'Update failed!'
            print err
        else:
          print self.cmdname+ ' creation failed!'
          print err
    finally:
      os.chdir(cwdbup)

class BranchAndUpdate(CustomTagAndUpdate):
  def __init__(self):
    CustomTagAndUpdate.__init__(self, 'Branch', ['-b'], branch_icon_xpm)

class TagAndUpdate(CustomTagAndUpdate):
  def __init__(self):
    CustomTagAndUpdate.__init__(self, 'Tag', [], tag_icon_xpm)


BranchAndUpdate()
TagAndUpdate()
