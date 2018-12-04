from cvsgui.App import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
import os, os.path
import string

"""
  WinCvs Macro "Change Module Path"
  $Revision: 1.3 $

  written by Oliver Giesen, Nov 2004 - Jul 2005
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
   from within WinCvs! This macro has originally been written against
   WinCvs 1.3.22 and was last tested against WinCvs 2.0.3 .

  ======
  Usage:

  - Select one or more folders containing valid sandboxes

  - Run the macro from the Macros|CVS menu

   ~A dialog will pop up asking you to confirm or enter the module path to be changed

  - Press "OK" when done

   ~Another dialog will pop up asking you for the new module path.

  - Press "OK" when done

   ~The macro will dump a log of its operation to the console,
    reporting how many Repository entries were changed

  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""
icon_xpm = \
  "13 12 5 1\n" + \
  ". c None\n" + \
  "# c #0000ff\n" + \
  "@ c #ff0000\n" + \
  "+ c #00aa00\n" + \
  "* c #aaaaaa\n" + \
  ".....##....++\n" + \
  ".....##....++\n" + \
  "....#*....++.\n" + \
  "....#*@...++.\n" + \
  "...**.@@.**..\n" + \
  "..@@@@@@@@*..\n" + \
  "..@@@@@@@@...\n" + \
  "..**..@@**...\n" + \
  ".##...@**....\n" + \
  ".##....*+....\n" + \
  "##....++.....\n" + \
  "##....++....."
#  "13 12 5 2\n" + \
#  ". c None\n" + \
#  "# c #0000ff\n" + \
#  "@ c #ff0000\n" + \
#  "+ c #00ff00\n" + \
#  "* c #aaaaaa\n" + \
#  ".....##....++.....**....**\n" + \
#  ".....##....++.....**....**\n" + \
#  "....#*....++.....*.....**.\n" + \
#  "....#*@...++.....*.*...**.\n" + \
#  "...**.@@.**........**.....\n" + \
#  "..@@@@@@@@*....********...\n" + \
#  "..@@@@@@@@.....********...\n" + \
#  "..**..@@**.........**.....\n" + \
#  ".##...@**.....**...*......\n" + \
#  ".##....*+.....**.....*....\n" + \
#  "##....++.....**....**.....\n" + \
#  "##....++.....**....**....."

class ChangeRepo(Macro):
  def __init__(self):
    Macro.__init__(self, 'Change module path', MACRO_SELECTION, 0, 'CVS', icon_xpm)

  def OnCmdUI(self, cmdui):
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    if enabled:
      for entry in self.sel:
        if entry.IsFile():
          enabled=0
          break
    cmdui.Enable( enabled)

  def getRepo(self, entry):
    if entry.IsUnknown():
      return ''
    else:
      repo = os.path.join(entry.GetFullName(), 'CVS', 'Repository')
      if os.path.exists(repo):
        f_repo = open(repo, 'r')
        try:
          oldrepo = string.strip(f_repo.readline())
          return oldrepo
        finally:
          f_repo.close()
      else:
        return ''

  def changeRepo(self, entry, console):
    console << kNormal << 'checking %s ...\t' % entry.GetFullName(),
    repo = os.path.join(entry.GetFullName(), 'CVS', 'Repository')
    if os.path.exists(repo):
      f_repo = open( repo, 'r+')
      try:
        try:
          currentRepo = string.strip(f_repo.readline())
            
          if (currentRepo.find(self.oldrepo) == 0):
            #print 'changed to', self.newrepo + cvsroot[len(self.oldrepo):] + '\n'
            f_repo.seek(0)
            f_repo.write(self.newrepo + currentRepo[len(self.oldrepo):] + '\n')
            f_repo.truncate()
            self.count += 1
            console << kGreen << 'changed\n' << kNormal
          else:
            console << kRed << 'path mismatch! (%s)\n' % currentRepo << kNormal
            if not self.mismatches.has_key(currentRepo):
              self.mismatches[currentRepo] = 0
            self.mismatches[currentRepo] += 1
        except:
          console << kRed << kBold << 'file read/write error!\n' << kNormal
          self.errCnt += self.errCnt
      finally:
        f_repo.close()
    else:
      console << kRed << kBold <<  'no Repository file!\n' << kNormal
      self.errCnt += self.errCnt

  def changeRepos(self, dir, console):
    if not dir.IsUnknown():
      self.changeRepo(dir, console)
    entries = GetCvsEntriesList(dir.GetFullName())
    for entry in entries:
      if not entry.IsFile():
        self.changeRepos(entry, console)
  
  def Run(self):
    dlgTitle='Change module path'
    self.count = 0
    self.errCnt = 0
    self.mismatches = {}
    oldrepo = self.getRepo( self.sel[0])
    answer, self.oldrepo = App.CvsPrompt(oldrepo, 'Question', \
                                         'Enter the old module path that is to be changed', \
                                         '(this is not the new module path!).\n\n' \
                                        +'Just press OK to accept current.',
                                         alertTitle=dlgTitle)
    if answer == 'IDOK':
      if self.oldrepo != '':
        oldrepo = self.oldrepo
      answer, self.newrepo = App.CvsPrompt(oldrepo, 'Question', 'Enter new module path.', \
                                           'Change old module path\n\n\t%s\n\nto new:' % self.oldrepo,
                                           alertTitle=dlgTitle)
      if answer == 'IDOK':
        console = ColorConsole()
        console << kNormal << '\nchanging module path %s to %s ...\n' % (self.oldrepo, self.newrepo)
        for entry in self.sel:
          self.changeRepos( entry, console)
        console << kBlue << 'Changed %d Repository entries.\n' % self.count
        if self.errCnt > 0:
          console << kRed <<  'Encountered %d errors.\n' % self.errCnt << kNormal
        if len(self.mismatches) > 0:
          console << kBrown << 'Skipped %d mismatching module paths:\n' % len(self.mismatches)
          for repo, cnt in self.mismatches.items():
            console << kBrown << '\t%s\t(%d directories)\n' % (repo, cnt)
        console << kNormal << '\n'

ChangeRepo()
