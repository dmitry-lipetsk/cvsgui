from cvsgui.App import *
from cvsgui.ColorConsole import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
import os, shutil
import re

"""
  CvsGui Macros "Mark Resolved" and "Locate conflicts"
  $Revision: 1.1 $

  written by Oliver Giesen, Oct 2003 - Dec 2003
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
   WinCvs 1.3.13.1 and was last tested against WinCvs 1.3.14 as of this
   writing.

  ======
  Usage (Mark Resolved):

  - Select one or more CVS-controlled files or folders

  - Run the macro from the Macros|CVS submenu

   ~All selected files and folders will now be scanned (recursively) for
     conflict files and if found they are scanned for remaining conflict
     markers. If none are found they are marked as Resolved. Each scanned
     file will be reported to the console with the results of the scan.
     Finally a summary of marked, skipped and erroneous files is presented.

  ------
  Usage (Locate conflicts):     

  - Select one or more CVS-controlled files or folders

  - Run the macro from the Macros|Search submenu

   ~All selected files and folders will now be scanned (recursively) for
     potential conflict files and if found they are scanned for conflict
     markers. Each scanned file that was either marked as Conflict before or
     that turned out to contain conflict markers will be reported to the
     console with the results of the scan. Finally a summary of possibly 
     resolved, unresolved and erroneous files is presented.
     
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

class ConflictScanner(Macro):
  def __init__(self, title, group, icon = ''):
    Macro.__init__(self, title, MACRO_SELECTION, 0, group, icon)
    
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

  def checkPrecondition(self, entry, console, data):
    return 1

  def dumpEntryName(self, entry, console):
    console << kNormal << '\n' << entry.GetFullName() << '\t'
    
  def prepareData(self, entry):
    return None

  def handleError(self, entry, console, text):  
    self.dumpEntryName(entry, console)
    console << kRed << '%s\n\t%s' % (self.s_error, detail)
    self.errCount += 1
  
  def scanEntries(self, entries, console):
    for entry in entries:
      if not (entry.IsUnknown() or entry.IsMissing()):
        if entry.IsFile():
          data = self.prepareData(entry)
          if self.checkPrecondition(entry, console, data) and (entry.GetKeyword() <> '-kb'):
            fn = entry.GetFullName()
            if entry.GetConflict() == '' or (entry.GetConflict() <> string.strip(GetFileModTime(fn))):
              m = self.reConflicts.search(self.readFile(fn))
              if m == None:
                try:
                  if self.handleResolving(entry, console, data):
                    self.resolvingCount += 1
                except Exception, detail:
                  self.handleError(entry, console, detail)
              else:
                if self.handleUnresolved(entry, console, data, 0):
                  self.unresolvedCount += 1
            else:
              if self.handleUnresolved(entry, console, data, 1):
                self.unresolvedCount += 1
        else:
          self.scanEntries(GetCvsEntriesList(entry.GetFullName()), console)

  def Run(self):
    cwdbup = os.getcwd()
    try:
      self.reConflicts = re.compile('^<<<<<<< [^\n]*\n.*=======\n.*>>>>>>> [\d\.]*$', re.MULTILINE|re.DOTALL)
      console = ColorConsole()
      try:
        self.resolvingCount = 0
        self.unresolvedCount = 0
        self.errCount = 0
        console << kNormal << '\nScanning for %s files...\n' % self.s_scantype
        self.scanEntries(self.sel, console)
        console << kBold
        if self.resolvingCount > 0:
          console << kGreen << '\n%4.d %s.' % (self.resolvingCount, self.s_resolvingreport)
        if self.unresolvedCount > 0:
          console << kMagenta << '\n%4.d %s.' % (self.unresolvedCount, self.s_unresolvedreport)
        if self.errCount > 0:
          console << kRed << '\n%4.d errors while scanning files.' % self.errCount
        if self.resolvingCount + self.unresolvedCount + self.errCount <= 0:
          console << 'No relevant files found.'
      finally:
        console << kNormal << '\nDone.\n\n'
    finally:
      os.chdir(cwdbup)


class SearchConflicts(ConflictScanner):
  def __init__(self):
    ConflictScanner.__init__(self, 'Locate conflicts', 'Search')
    self.s_scantype = 'conflict'
    self.s_resolvingreport = 'conflict file(s) without detectable conflicts (possibly resolved)'
    self.s_unresolvedreport = 'apparently unresolved conflict file(s)'
    self.s_error = 'Error scanning file'

  def handleResolving(self, entry, console, data):
    if entry.GetConflict() <> '':
      self.dumpEntryName(entry, console)
      console << kGreen << 'no conflicts detected' << kNormal
      return 1
    else:
      return 0

  def handleUnresolved(self, entry, console, data, reason):
    self.dumpEntryName(entry, console)
    console << kMagenta
    if reason == 0:
      console << 'file appears to contain conflict markers!'
    else:
      console << 'file is unmodified since conflict!'
    console << kNormal
    return 1
    
class MarkResolved(ConflictScanner):
  def __init__(self):
    ConflictScanner.__init__(self, 'Mark resolved', 'CVS')
    self.s_scantype = 'resolved'
    self.s_resolvingreport = 'file(s) marked as Resolved'
    self.s_unresolvedreport = 'apparently unresolved file(s) skipped'
    self.s_error = 'Error marking as resolved'

  def prepareData(self, entry):
      return os.path.join(entry.GetPath(), 'CVS', 'Entries')

  def checkPrecondition(self, entry, console, data):
    res = 0
    if entry.GetConflict() <> '':
      if not os.path.exists(data + '.Backup'):
        res = 1
      else:
        self.handleError(entry, console, '\nSandbox metadata for %s in undefined state (Entries.Backup exists)' % entry.GetPath())
    return res
    
  def rewriteEntries(self, path, contents):
    bup = path + '.Backup'
    shutil.copyfile(path, bup)
    try:
      try:
        f = open(path, 'w')
        try:
          f.write(contents)
        finally:
          f.close()
      except:
        shutil.copyfile(bup, path)
        raise
    finally:
      os.remove(bup)
  
  def handleResolving(self, entry, console, data):
    self.dumpEntryName(entry, console)
    assert data, 'data parameter does not contain Entries path!'
    Entries = self.readFile(data)
    pattern = '(?m)^(/%s/[^/]+/)[^\+/]+\+[^/]+(/[^/]*/[^\n]*)$' % entry.GetName()
    newEntries = re.sub(pattern, '\g<1>Resolved\g<2>', Entries)
    assert Entries <> newEntries, 'no match for %s in %s' % (entry.GetName(), data)
    self.rewriteEntries(data, newEntries)
    console << kGreen << 'successfully marked as Resolved' << kNormal
    return 1

  def handleUnresolved(self, entry, console, data, reason):
    self.dumpEntryName(entry, console)
    console << kMagenta
    if reason == 0:
      console << 'skipped because of remaining conflict marker(s)'
    else:
      console << 'skipped because still unmodified'
    console << kNormal
    return 1

SearchConflicts()
MarkResolved()
