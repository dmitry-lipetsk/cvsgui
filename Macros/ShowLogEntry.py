from cvsgui.App import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
import os, os.path
import re, string

"""
  WinCvs Macro "Show log entry"
  $Revision: 1.2 $

  written by Oliver Giesen, Oct 2001-Aug 2002
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
   You will need at least WinCvs 1.3.4 to execute any Python macros
   from within WinCvs! This macro has been tested against WinCvs 1.3.8 .

  ======
  Usage:

  - Select a CVS file

  - Run the macro from the "Macros|CVS" menu or simply press Ctrl-E (might not alway work)

   ~The log entry for the currently checked out revision will be dumped to the console
  
  =============
  Known Issues / "Un-niceties":

  - The keyboard shortcut does not work sometimes.
 
  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
  
"""

icon_xpm = \
  "13 12 6 1\n" + \
  ". c None\n" + \
  "- c #eeeeee\n" + \
  "b c #000080\n" + \
  "+ c #404040\n" + \
  "* c #808080\n" + \
  "@ c #000040\n" + \
  ".............\n" + \
  ".******......\n" + \
  ".*----**.....\n" + \
  ".*-++-*-*....\n" + \
  ".*----***@@..\n" + \
  ".*-++++-@-.@.\n" + \
  ".*-----@-*..@\n" + \
  ".*-+++-@-*..@\n" + \
  ".*------@-.@.\n" + \
  ".*-+++++-@bb.\n" + \
  ".*-------*.bb\n" + \
  ".*********.bb"

class ShowLogEntry( Macro):
  def __init__(self):
    Macro.__init__(self, "Show &log entry\tCtrl+E", MACRO_SELECTION, 0, 'CVS', icon_xpm)

  def OnCmdUI(self, cmdui):
    self.sel = App.GetSelection()
    isValid = len(self.sel) == 1 and self.sel[0].IsFile() and not self.sel[0].IsUnknown()
    cmdui.Enable(isValid)

  def Run(self):
    entry = self.sel[0]
    rev = entry.GetVersion()
    cvs = Cvs(1)

    os.chdir( entry.GetPath())
    code, out, err = cvs.Run("log", "-N", "-r%s" % rev, entry.GetName())

    console = ColorConsole()

    console << kBold << entry.GetFullName() << '\n' << kNormal
    
    #parse cvs output:

    lines= string.split(out, '\n')
    startParsing = 0
    for line in lines:
      if startParsing or re.compile("^--------").match(line):
        startParsing = 1

        if re.compile("^--------").match(line):
          console << kBlue << line << "\n" << kNormal
          
        elif re.compile("^revision").match(line):
          console << kNormal << "revision : " << kRed << kBold << rev << "\n" << kNormal

        elif re.compile("^date").match(line):
          entries = string.split( line, ';')

          for item in entries:
            tokens = string.split( item, ':')
            if len( tokens) > 1:
              console << kNormal << string.rjust(tokens[0], 8) << " : "
              console << kBold << string.strip(string.join(tokens[1:], ':')) << "\n" << kNormal
            else:
              console << item << '\n'

        elif re.compile("^========").match(line):
          pass
      
        else:
          console << kBrown << line << "\n" << kNormal

ShowLogEntry()
