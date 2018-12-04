from cvsgui.Macro import *
import os, os.path

"""
  WinCvs Macro "Command Prompt here"
  $Revision: 1.3 $

  written by Oliver Giesen, January - May 2005
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    msn:    ogiesen@hotmail.com
    icq:    18777742
    y!:     ogiesen

  Feel free to modify or distribute in whichever way you like, 
   as long as it doesn't limit my personal rights to modify and
   redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.0.1 .

  ======
  Usage:

  - Just run the macro via Macros|Command Prompt here or press Ctrl-K

  =============
  Known Issues / "Un-niceties":

  - None. This macro is now replaced with a native WinCvs menu thus a shortcut is 
  removed to avoid conflict.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

class CmdHere(Macro):
  def __init__(self):
    Macro.__init__(self, 'Command Prompt here', MACRO_SELECTION)

  def OnCmdUI(self, cmdui):
    # always enabled:
    cmdui.Enable(1 == 1)

  def Run(self):
    cmd = os.getenv('comspec');
    os.spawnl(os.P_NOWAIT, cmd, \
              '/k %s && cd %s' % os.path.splitdrive(os.getcwd()));

CmdHere()