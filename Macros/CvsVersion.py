from cvsgui.Macro import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
import re, string

class CvsVersion(Macro):
	def __init__(self):
		Macro.__init__(self, "Get the cvs version", MACRO_ADMIN)

	def Run(self):
		cvs = Cvs(1)
		code, out, err = cvs.Run("--version")

		console = ColorConsole()
		lines = string.split(out, '\n')
		hasVersion = 0
		for l in lines:
			if re.compile("^Concurrent Versions System").match(l):
				console << kRed << l << "\n" << kNormal
				hasVersion = 1

			if re.compile(".*WinCVS.*").match(l):
				console << kBlue << l << "\n" << kNormal
				
		del console

		if not hasVersion:
			print out

CvsVersion()
