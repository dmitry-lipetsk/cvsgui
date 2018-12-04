from cvsgui.Macro import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.CvsLog import *
import cvsgui.App, os

class CopyToHeadVersion(Macro):
	def __init__(self):
		Macro.__init__(self, "Copy to HEAD version", MACRO_SELECTION, 0, "Manage Revisions")

	def _tempName(self,s):
		return ".#" + s + ".CopyToHeadVersion"

	def Run(self):
		sel = cvsgui.App.GetSelection()
		cvs = Cvs()
		list = ("update", "-A")
		for s in sel:
			if os.access(self._tempName(s.GetName()), os.F_OK):
				os.unlink(self._tempName(s.GetName()))
			os.rename(s.GetName(), self._tempName(s.GetName()))
			list = list + (s.GetName(),)
		cvs.Run(*list)
		for s in sel:
			if os.access(s.GetName(), os.F_OK):
				os.unlink(s.GetName())
			os.rename(self._tempName(s.GetName()), s.GetName())
		return 0;

CopyToHeadVersion()
