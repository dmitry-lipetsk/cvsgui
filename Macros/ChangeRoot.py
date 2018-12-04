from cvsgui.App import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
import os, os.path
import string

"""
  WinCvs Macro "Change CVSROOT"
  $Revision: 1.5 $

  written by Oliver Giesen, Aug 2002 - Jul 2005
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
   WinCvs 1.3.8 and was last tested against WinCvs 2.0.3 .

  NOTE:
   In contrast to the previously existing TCL macros ChangeRoot.tcl
   and ChangeRootTk.tcl, this macro will actually look at the contents
   of each Root file and will only replace it if it matches a given
   value. This way you could run it on sandboxes that are composed from
   different repositories.
   
  ======
  Usage:

  - Select one or more folders containing valid sandboxes

  - Run the macro from the Macros|CVS menu

   ~A dialog will pop up asking you to confirm or enter the CVSROOT to be changed

  - Press "OK" when done

   ~Another dialog will pop up asking you for the new CVSROOT string.

  - Press "OK" when done

   ~The macro will dump a log of its operation to the console,
    reporting how many Root entries were changed

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

class ChangeRoot( Macro):
  def __init__( self):
    Macro.__init__( self, 'Change CVSROOT', MACRO_SELECTION, 0, 'CVS', icon_xpm)

  def OnCmdUI( self, cmdui):
    self.sel = App.GetSelection()
    enabled = len( self.sel) > 0
    if enabled:
      for entry in self.sel:
        if entry.IsFile():
          enabled=0
          break
    cmdui.Enable( enabled)

  def getRoot( self, entry):
    if entry.IsUnknown():
      return ''
    else:
      root = os.path.join( entry.GetFullName(), 'CVS', 'Root')
      if( os.path.exists( root)):
        f_root = open( root, 'r')
        try:
          cvsroot = string.strip( f_root.readline())
          return cvsroot
        finally:
          f_root.close()
      else:
        return ''

  def changeRoot( self, entry, console):
    console << kNormal << 'checking %s ...\t' % entry.GetFullName(),
    root = os.path.join( entry.GetFullName(), 'CVS', 'Root')
    if( os.path.exists( root)):
      f_root = open( root, 'r+')
      try:
        try:
          if( self.oldroot == ''): # a blank oldroot forces replace without checking the previous value
            cvsroot = self.oldroot
          else:
            cvsroot = string.strip( f_root.readline())
            
          if( cvsroot == self.oldroot):
            f_root.seek( 0)
            f_root.write( self.newroot + '\n' )
            f_root.truncate()
            self.count += 1
            console << kGreen << 'changed\n' << kNormal
          else:
            console << kRed << 'CVSROOT mismatch! (%s)\n' % cvsroot << kNormal
            if not self.mismatches.has_key(cvsroot):
              self.mismatches[cvsroot] = 0
            self.mismatches[cvsroot] += 1
        except:
          console << kRed << kBold << 'file read/write error!\n' << kNormal
          self.errCnt += self.errCnt
      finally:
        f_root.close()
    else:
      console << kRed << kBold <<  'no Root file!\n' << kNormal
      self.errCnt += self.errCnt

  def changeRoots( self, dir, console):
    if not( dir.IsUnknown()):
      self.changeRoot( dir, console)
    entries = GetCvsEntriesList( dir.GetFullName())
    for entry in entries:
      if not( entry.IsFile()):
        self.changeRoots( entry, console)
  
  def Run( self):
    dlgTitle='Change CVSROOT'
    self.count = 0
    self.errCnt = 0
    self.mismatches = {}
    oldroot = self.getRoot( self.sel[0])
    answer, self.oldroot = App.CvsPrompt(oldroot, 'Question', \
                                         'Enter the old CVSROOT that is to be changed.', \
                                         '(this is not the new CVSROOT!).\n\n' \
                                        +'Clear to change all existing CVSROOTs.\n\n' \
                                        +'Just press OK to accept current.',
                                         alertTitle=dlgTitle)
    if answer == 'IDOK':
      if self.oldroot != '':
        oldroot = self.oldroot
      answer, self.newroot = App.CvsPrompt(oldroot, 'Question', \
                                           'Enter the new CVSROOT.',
                                           'Change old CVSROOT\n\t%s\n\nto new:' % self.oldroot,
                                           alertTitle=dlgTitle)
      if answer == 'IDOK':
        console = ColorConsole()
        console << kNormal << '\nchanging CVSROOT %s to %s ...\n' % ( self.oldroot, self.newroot)
        for entry in self.sel:
          self.changeRoots( entry, console)
        console << kBlue << 'Changed %d Root entries.\n' % self.count
        if self.errCnt > 0:
          console << kRed <<  'Encountered %d errors.\n' % self.errCnt << kNormal
        if len(self.mismatches) > 0:
          console << kBrown << 'Skipped %d mismatching CVSROOTs:\n' % len(self.mismatches)
          for root, cnt in self.mismatches.items():
            console << kBrown << '\t%s\t(%d directories)\n' % (root, cnt)
        console << kNormal << '\n'

ChangeRoot()
