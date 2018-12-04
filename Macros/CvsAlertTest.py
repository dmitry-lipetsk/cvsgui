from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *

"""
  WinCvs Macro "CvsAlertTest"
  $Revision: 1.2 $

  written by Jerzy Kaczorowski, June 2005
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

class CvsAlertTest(Macro):
  def __init__(self):
    Macro.__init__(self, 'CvsAlertTest', MACRO_SELECTION, 0, 'test')

  def OnCmdUI(self, cmdui):
    """ Always enabled """
    enabled = 1
    cmdui.Enable(enabled)

  def Run(self):
    """ Runs a CVS command on all selected items """
    cwdbup = os.getcwd()
    try:
      # The simplest information box
      cvsgui.App.CvsAlert("Note", "Information only.")

      # More complex warning with detailed multi-line description
      if cvsgui.App.CvsAlert("Warning", "This is a warning.", "More details about a warning.\nSecond line of details.") == 'IDOK':
        print 'OK\n'
      else:
        cvs_err('Cancelled\n')

      # Error message with selection to continue or stop, "Cancel" preselected as default
      if cvsgui.App.CvsAlert("Stop", "Some problem.", "", "OK", "Cancel", "", "IDCANCEL") == 'IDOK':
        print 'OK\n'
      else:
        cvs_err('Cancelled\n')

      # Question with selection and three buttons, "No" preselected as default and title specified
      answer = cvsgui.App.CvsAlert("Question", "Some question.", "Explanation of the question.", "Yes", "Cancel", "No", "IDOTHER", "Message Title")
      if answer == 'IDOK':
        print 'Yes\n'
      elif answer == 'IDOTHER':
        cvs_err('No\n')
      elif answer == 'IDCANCEL':
        cvs_err('Cancelled\n')
      else:
        cvs_err(answer + '\n')

      # Question with selection and three buttons, "Yes" preselected as default and title specified
      answer = cvsgui.App.CvsAlert("Question", "Some question.", "Explanation of the question.", "Yes", "No", "Yes to All", "", "Message Title")
      if answer == 'IDOK':
        print 'Yes\n'
      elif answer == 'IDOTHER':
        print 'Yes to All\n'
      elif answer == 'IDCANCEL':
        cvs_err('No\n')
      else:
        cvs_err(answer + '\n')
    finally:
      os.chdir(cwdbup)

# uncomment the following line to make the macro register on the menu:
CvsAlertTest()