from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *

"""
  WinCvs Macro "Move Branchpoint to HEAD"
  $Revision: 1.2 $

  written by Oliver Giesen, Jun 2004
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    msn:    ogiesen@hotmail.com
    icq:    18777742

  Feel free to modify or distribute in whichever way you like, 
   as long as it doesn't limit my personal rights to modify and
   redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has been tested against WinCvs 1.3.18 .

  ======
  Usage:

  - Select one or more CVS-controlled files that fulfill the
     following requirements:
     *   located in a folder that is checked out on a branch
      or checked out individually on a branch while the containing
       folder is checked out on the trunk
     * does not have any revisions on abovementioned branch

  - Run the macro from the Macros|CVS menu

   ~ The macro will attempt to move the determined branch tag on the selected
     files to the files' HEAD revisions and reports on progress, success or
     failure along the way.
     
  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

class MoveBranchPointToHead(Macro):
  def __init__(self):
    Macro.__init__(self, 'Move Branchpoint to HEAD', MACRO_SELECTION, 0, 'CVS')

  def OnCmdUI(self, cmdui):
    # allow only selections of cvs-controlled files:
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    if enabled:
      for entry in self.sel:
        if entry.IsUnknown() or not entry.IsFile():
          enabled = 0
          break
    cmdui.Enable(enabled)

  def getDirTag(self, dir):
    print 'determining branch... ',
    tagfn = os.path.join( dir, 'CVS', 'Tag')
    if os.path.exists(tagfn):
      f_tag = open(tagfn, 'r')
      try:
        tag = f_tag.readline().strip()
      finally:
        f_tag.close()
        
      if tag[0] == 'T':
        tag = tag[1:]
        print '\t'+tag
      else:
        print '\nDirectory is on a sticky non-branch tag!'
        tag = None
    else:
      print '\ton trunk'
      tag = 'HEAD'
    return tag
    
  def Run(self):
    cwdbup = os.getcwd()
    try:
      cvs = Cvs(1,0)
      args = []
      prepareTargets(self.sel, args)
      path = self.sel[0].GetPath()
      branch = self.getDirTag(path)
      
      # validate selection:
      if not branch:
        cvs_err('Not on a branch.\n')
        return
      
      for entry in self.sel:
        if entry.GetVersion().count('.') > 1:
          cvs_err('%s is already branched!\n' % entry.GetFullName())
          return
        if entry.GetPath() <> path:
          path = entry.GetPath()
          if self.getDirTag(path) <> branch:
            cvs_err('All files have to be on the same branch.\n')
            return
      
      print 'Updating to HEAD'
      code, out, err = cvs.Run('up', '-A', *args)
      if code == 0:
        if out: print out
        print 'Moving branchpoint'
        code, out, err = cvs.Run('tag', '-bBF', branch, *args)
      if code == 0:
        if out: print out
        print 'Updating back to branch'
        code, out, err = cvs.Run('up', '-r'+branch, *args)      
      if code == 0:
        if out: print out
        print 'Done.\n'
      else:
        cvs_err(err)
    finally:
      os.chdir(cwdbup)

MoveBranchPointToHead()