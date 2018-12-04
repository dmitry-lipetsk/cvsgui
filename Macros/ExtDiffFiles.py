from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.Persistent import *
from cvsgui.MacroUtils import *
import os, os.path

"""
  WinCvs Macro "Externally Diff selected Files"
  prepared WinCvs Macro "Externally Diff revisions of selected Files"
  $Revision: 1.2 $

  written by Oliver Giesen, Feb 2004
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
   from within WinCvs! This macro has been tested against WinCvs 1.3.13 .

  ======
  Usage:

  - Select two files _or_ two folders
  
  - Run the macro via Macros|Diff|"<File1>" and "<File2"
  
   ~The External Diff application should now be launched with the two selected
     files or folders
     
  =============
  Known Issues / "Un-niceties":

  - Diff Revisions not ready yet. Less useful anyway.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

class CustomExtDiff(Macro):
  def __init__(self, title, extdiff):
    Macro.__init__(self, title % ('', ''), MACRO_SELECTION, 0, 'Diff')
    self.extdiff = extdiff
    self.title = title

  def OnCmdUI(self, cmdui):
    self.sel = App.GetSelection()
    #print len(self.sel)
    enabled = (len(self.sel) == 2) 
    if enabled:
      enabled = (     self.sel[0].IsFile()  and      self.sel[1].IsFile()) \
              or((not self.sel[0].IsFile()) and (not self.sel[1].IsFile()))
      enabled = enabled and self.checkSel()
      cmdui.SetText(self.title % (self.sel[0].GetName(), self.sel[1].GetName()))
    cmdui.Enable(enabled)

  def Run(self):
    cwdbup = os.getcwd()
    try:
      args = self.getArgs()
      print '"%s" %s' % (self.extdiff, string.join(args))
      args.insert(0, '"%s"' % self.extdiff)
      code = os.spawnl(os.P_NOWAIT, self.extdiff, *args)
    finally:
      os.chdir(cwdbup)

class ExtDiffFiles(CustomExtDiff):
  def __init__(self, extdiff):
    CustomExtDiff.__init__(self, '"%s" and "%s"', extdiff)

  def checkSel(self):
    return 1

  def getArgs(self):
    return ['"%s"' % self.sel[0].GetFullName(), '"%s"' % self.sel[1].GetFullName()]
    

class ExtDiffRevs(CustomExtDiff):
  def __init__(self, extdiff):
    CustomExtDiff.__init__(self, 'Revisions of "%s" and "%s"', extdiff)

  def checkSel(self):
    result = self.sel[0].IsFile()
    if result:
      for entry in self.sel:
        result = (not entry.IsUnknown())
        if result:
          break
    return result

  def getArgs(self):
    #TODO: - query for revisions,
    #      - use update -p to save revisions to temp files
    #      - return paths of above temp files
    pass 


try:
  try:
    p_extdiff = Persistent('P_Extdiff', '', 0)
    extdiff = str(p_extdiff)
    if extdiff <> '' and os.path.exists(extdiff):
      ExtDiffFiles(extdiff)
      #ExtDiffRevs(extdiff)
      #print 'External Diff:', extdiff
    #else:
      #print 'Could not determine valid external diff!'
  finally:
    del p_extdiff
    del extdiff
except:
  pass
  #raise
