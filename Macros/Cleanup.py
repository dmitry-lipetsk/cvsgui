from cvsgui.App import *
from cvsgui.ColorConsole import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *

"""
  WinCvs Macro "Cleanup Ignored/Merging/Unknown Files"
  $Revision: 1.5 $

  written by Oliver Giesen, Aug 2003 - Aug 2004
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    icq:    18777742

  Feel free to modify or distribute in whichever way you like, 
   as long as it doesn't limit my personal rights to
   modify and redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has originally been written against
   WinCvs 1.3.13 and was last tested against WinCvs 1.3.20 using
   Python 2.3.4.

  ======
  Usage:

  - Select one or more folders.

  - Run the appropriate macro from the Macros|Cleanup  sub-menu

   ~watch your unwanted files disappear.
   
  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

# Change this to 0 if you want .cvsignore files to be deleted as well
# (unless they are CVS-controlled):
preserveCvsIgnoreFiles = 1

# Change this to 1 if you want unknown folders to be purged as well:
purgeUnknownDirs = 0

class BaseCleanup(Macro):
  def __init__(self, delinquent):
    Macro.__init__(self, delinquent, MACRO_SELECTION, 0, 'Cleanup')
    self.delinquent = delinquent

  def OnCmdUI(self, cmdui):
    # allow only selections of folders:
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    if enabled:
      for entry in self.sel:
        if entry.IsFile():
          enabled = 0
          break
    cmdui.Enable(enabled)

  def delEntries(self, entries, console, mayDelete = 1):
    folders = []

    #process files:
    for entry in entries:
      name = entry.GetFullName()
      if entry.IsFile():
        if mayDelete and self.mustDie(entry):
          if (not preserveCvsIgnoreFiles) or (entry.GetName() <> '.cvsignore'):
            try:
              os.remove(name)
              console << kRed << '\t%s\n' % entry.GetName() << kNormal
              self.cnt += 1
            except OSError, detail:
              console << kGreen << '\t%s\t%s\n' % (entry.GetName(), detail) << kNormal
              self.skip += 1
          else:
            console << kGreen << '\t%s\n' % entry.GetName() << kNormal
            self.skip += 1
      else:
        folders.append(entry)

    #process folders:
    for entry in folders:        
      name = entry.GetFullName()
      mayDelete = purgeUnknownDirs or ((not entry.IsUnknown()) or entry.IsIgnored())
      if mayDelete:
        console << kBrown << 'Cleaning up directory %s ...\n' % name << kNormal
      else:
        console << kBlue << 'Skipping non-CVS directory %s ...\n' % name << kNormal
        self.dirskip += 1
      self.delEntries(GetCvsEntriesList(name), console, mayDelete)

      #check if folder is empty now, if so, purge it:
      if mayDelete and entry.IsUnknown() and (len(GetCvsEntriesList(name)) <= 0):
        console << kRed << 'Deleting empty directory %s ...\n' % name << kNormal
        os.rmdir(name)
        self.dircnt += 1
    
    
  def Run(self):
    print 'Removing %s...' % self.delinquent
    self.cnt = 0
    self.skip = 0
    self.dircnt = 0
    self.dirskip = 0
    console = ColorConsole()
    try:
      console << kNormal
      self.delEntries(self.sel, console)
      if self.cnt > 0:
        console << kRed
      else:
        console << kBrown
      console << '\nFiles removed: %d' % self.cnt
      if self.skip > 0:
        console << kGreen << '\tFiles skipped: %d\n' % self.skip
      else:
        console << '\n'

      if (self.dircnt > 0) or (self.dirskip > 0):
        if self.dircnt > 0:
          console << kRed
        else:
          console << kBrown
        console << 'Directories removed: %d' % self.dircnt
        if self.dirskip > 0:
          console << kBlue << '\tDirectories skipped: %d\n' % self.dirskip
    finally:
      console << kNormal << '\n'      


class CleanupIgnored(BaseCleanup):
  def __init__(self):
    BaseCleanup.__init__(self, 'Ignored files')

  def mustDie(self, entry):
    return entry.IsIgnored()

class CleanupMergeFiles(BaseCleanup):
  def __init__(self):
    BaseCleanup.__init__(self, 'Merging files')

  def mustDie(self, entry):
    return entry.GetName()[:2] == '.#'

class CleanupUnknown(BaseCleanup):
  def __init__(self):
    BaseCleanup.__init__(self, 'All unknown files')

  def mustDie(self, entry):
    return entry.IsUnknown()

CleanupIgnored()
CleanupMergeFiles()
CleanupUnknown()