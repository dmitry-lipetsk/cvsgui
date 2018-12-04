from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *

"""
  WinCvs Macro "MyMacro"
  $Revision: 1.2 $

  written by Someone, Sometime
  contact:
    email:  user@host.org

  Insert legal disclaimer here.

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.x.x .

  ======
  Usage:

  - Select one or more CVS-controlled files and/or folders

  - Run the macro

  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might
   have to user@host.org .
    
"""

class MyMacro(Macro):
  def __init__(self):
    Macro.__init__(self, 'My Macro', MACRO_SELECTION, 0, 'My Macros')

  def OnCmdUI(self, cmdui):
    """ Allows only selections of cvs-controlled entries """
    for entry in App.GetSelection():
      if entry.IsUnknown():
        enabled = 0
        break
    else:
      enabled = 1
    cmdui.Enable(enabled)

  def Run(self):
    """ Runs a CVS command on all selected items """
    cwdbup = os.getcwd()
    try:
      cvs = Cvs(1,0)
      targets = []
      prepareTargets(App.GetSelection(), targets)

      #substitute 'command' and 'myargs' with your actual cvs command
      # and arguments, e.g. 
      #   code, out, err = cvs.Run('update', '-d yesterday', '-C', *targets)
      
      code, out, err = cvs.Run('command', 'myargs', *targets)
      if code == 0:
        print 'Done.'
      else:
        cvs_err(err)
    finally:
      os.chdir(cwdbup)

# uncomment the following line to make the macro register on the menu:
#MyMacro()