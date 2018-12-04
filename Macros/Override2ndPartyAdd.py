from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *

"""
  WinCvs Macro "Override Second-party Add"
  $Revision: 1.2 $

  written by Oliver Giesen, April 2004
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

  - Select one or more unknown files that you know already exist in the repository

  - Run the macro via Macos|Problem Resolution|Override second-party Add

   ~The selected files should now be brought into a committable state, allowing
     you to diff them against the repository files.
     
  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

class Override2ndPartyAdd(Macro):
  def __init__(self):
    Macro.__init__(self, 'Override Second-party Add', MACRO_SELECTION, 0, 'Problem Resolution')

  def OnCmdUI(self, cmdui):
    # allow only selections of unknown entries:
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    if enabled:
      for entry in self.sel:
        if not entry.IsUnknown():
          enabled = 0
          break
    cmdui.Enable(enabled)

  def backupFiles(self, paths):
    success = 1
    print 'Backing up files...'
    for path in paths:
      try:
        os.rename(path, path + '.bup2')
      except Error, detail:
        print '\tFailed to back up "%s"\t%s' % (path, detail)
        success = 0
        break
    return success

  def removeFiles(self, paths):
    success = 1
    print 'Invalidating repository files...'
    for path in paths:
      try:
        os.chmod(path, 664)
        os.remove(path)
      except Exception, detail:
        print '\tFailed to remove "%s"\t%s' % (path, detail)
        success = 0
        break
    return success
        
  def restoreFiles(self, paths):
    success = 1
    print 'Restoring original files...'
    for path in paths:
      try:
        os.rename(path + '.bup2', path)
      except Exception, detail:
        print '\tFailed to restore "%s"\t%s' % (path, detail)
        success = 0
        break
    return success
                  
  def Run(self):
    cwdbup = os.getcwd()
    try:
      cvs = Cvs(1,0)
      args = []
      code = 1
      err = 'Failed.'
      prepareTargets(self.sel, args)

      try:
        if self.backupFiles(args):
          print 'Retrieving up-to-date files from repository...'
          code, out, err = cvs.Run('update', *args)
          if code == 0:
            self.removeFiles(args)
      finally:
        self.restoreFiles(args)

      if code == 0:
        print 'Done.'
      else:
        print err
    finally:
      os.chdir(cwdbup)

Override2ndPartyAdd()
