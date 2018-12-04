from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *

"""
  WinCvs Macro "CvsPromptTest"
  $Revision: 1.1 $

  written by Jerzy Kaczorowski, July 2005
  contact:
    email:  kaczoroj@hotmail.com

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.x.x .

  ======
  Usage:

  - Run the macro

  =============
  Known Issues / "Un-niceties":

  - None so far.

"""

class CvsPromptTest(Macro):
  def __init__(self):
    Macro.__init__(self, 'CvsPromptTest', MACRO_SELECTION, 0, 'test')

  def OnCmdUI(self, cmdui):
    """ Always enabled """
    enabled = 1
    cmdui.Enable(enabled)

  def Run(self):
    """ Runs a CVS command on all selected items """
    cwdbup = os.getcwd()
    try:
      # Enter value
      value = "Input data"
      print 'Prompt for Value(' + value + ')\n'
      answer, value = cvsgui.App.CvsPrompt(value, "Question", "Enter a value.", "Details about the value")
      if answer == 'IDOK':
        print 'OK\n'
        print 'Returned Value(' + value + ')\n'
      else:
        cvs_err('Cancelled\n')

    finally:
      os.chdir(cwdbup)

# uncomment the following line to make the macro register on the menu:
CvsPromptTest()