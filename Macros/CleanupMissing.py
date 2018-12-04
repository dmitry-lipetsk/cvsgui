from cvsgui.App import *
from cvsgui.ColorConsole import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
import os, shutil
import re

"""
  CvsGui Macro "Cleanup Missing"
  $Revision: 1.2 $

  written by Oliver Giesen, Dec 2003
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    icq:    18777742

  Feel free to modify or distribute in whichever way you like, as long as it
   doesn't limit my personal rights to modify and redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has originally been written against
   WinCvs 1.3.14 .

  ======
  Usage:

  - Select one or more CVS-controlled files or folders

  - Run the macro from the Macros|CVS submenu

   ~All selected files and folders will now be scanned (recursively) for
     missing items and if any are found they are removed from their parent's
     Entries file
     
  =============
  Known Issues / "Un-niceties":

  - This macro has to modify the metadata files maintained by CVS! The format
   of those files is potentially subject to change so I can only say for sure
   it works for the version I tested against (CVSNT up to 2.0.13 as of this 
   writing). Furthermore, in case anything goes wrong during the 
   manipulation, your sandbox may potentially become corrupted. The macro 
   takes precautions for this by first backing up the old Entries file to 
   Entries.Backup, so you would be able to restore it.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

bup_suffix = '.Backup'
xtra_suffix = '.Extra'

class CleanupMissing(Macro):
  def __init__(self):
    Macro.__init__(self, 'Missing files and folders', MACRO_SELECTION, 0, 'Cleanup')

  def OnCmdUI(self, cmdui):
    # allow only selections of cvs-controlled entries:
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    if enabled:
      for entry in self.sel:
        if entry.IsUnknown():
          enabled = 0
          break
    cmdui.Enable(enabled)

  def readFile(self, filename):
    f = open(filename, 'r')
    try:
      s  = f.read()
    finally:
      f.close()
      return s

  def rewriteEntries(self, path, contents, extracontents):
    def writeFile(f, c):
      f = open(f, 'w')
      try:
        f.write(c)
      finally:
        f.close()
      
    bup = path + bup_suffix
    if(extracontents):
      extra = path + xtra_suffix
      extrabup = extra + bup_suffix
    mayCleanup = 1
    shutil.copyfile(path, bup)
    try:
      if(extracontents):
        shutil.copyfile(extra, extrabup)
      try:
        try:
          writeFile(path, contents);
          if(extracontents):
            writeFile(extra, extracontents);
        except:
          try:
            shutil.copyfile(bup, path)
            if(extracontents):
              shutil.copyfile(extrabup, extra)
          except:
            mayCleanup = 0
          raise
      finally:
        if(mayCleanup and extracontents):
          os.remove(extrabup)
    finally:
      if(mayCleanup):
        os.remove(bup)
  
  def cleanupMissingEntries(self, entries, console):
    for entry in entries:
      if (entry.IsMissing()):
        fn = entry.GetFullName()
        console << kNormal << '\n' << fn << '\t'
        EntriesPath = os.path.join(entry.GetPath(), 'CVS', 'Entries')
        if not os.path.exists(EntriesPath + bup_suffix):
          try:
            Entries = self.readFile(EntriesPath)
            pattern = '(?m)^D?/' + entry.GetName() + '(/[^/]*){%d}/[^\n]*\n?'
            newEntries = re.sub(pattern % 3, '', Entries)
            assert Entries <> newEntries, 'no match for %s in %s' % (entry.GetName(), EntriesPath)
            if os.path.exists(EntriesPath + xtra_suffix):
              Entries = self.readFile(EntriesPath + xtra_suffix)
              newEntriesExtra = re.sub(pattern % 2, '', Entries)
              assert Entries <> newEntriesExtra, 'no match for %s in %s%s' % (entry.GetName(), EntriesPath, xtra_suffix)
            else:
              newEntriesExtra = None
            self.rewriteEntries(EntriesPath, newEntries, newEntriesExtra)
            console << kGreen << 'successfully cleaned up' << kNormal
            self.cleanupCount += 1
          except Exception, detail:
            console << kRed << 'Error occured while cleaning up:\n\t%s' % detail
            self.errCount += 1
        else:
          console << kRed << 'Sandbox metadata in undefined state (Entries.Backup exists)' << kNormal
          self.errCount += 1
      elif (not entry.IsFile()):
        self.cleanupMissingEntries(GetCvsEntriesList(entry.GetFullName()), console)
                        
  def Run(self):
    cwdbup = os.getcwd()
    try:
      console = ColorConsole()
      try:
        self.errCount = 0
        self.cleanupCount = 0
        console << kNormal << '\nScanning selection for missing files and folders...\n'
        self.cleanupMissingEntries(self.sel, console)
        console << kBold
        if self.cleanupCount > 0:
          console << kGreen << '\n%4.d missing items removed.' % self.cleanupCount
        if self.errCount > 0:
          console << kRed << '\n%4.d errors while cleaning up.' % self.errCount
      finally:
        console << kNormal << '\nDone.\n\n'
    finally:
      os.chdir(cwdbup)

CleanupMissing()      