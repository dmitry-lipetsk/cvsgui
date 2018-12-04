from cvsgui.Macro import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.CvsLog import *
import cvsgui.App, os, sys

test_xpm = \
	"16 15 5 1\n" + \
	".	c None\n" + \
	"+	c #808080\n" + \
	"@	c #FFFFFF\n" + \
	"#	c #FF0000\n" + \
	"$	c #000000\n" + \
	"..++++++++++....\n" + \
	"..+@@@@@@@@.+...\n" + \
	"..+@@@##@@@.@+..\n" + \
	"..+@@@##@@@$$$$.\n" + \
	"..+@######@@@.$.\n" + \
	"..+@######@@@.$.\n" + \
	"..+@@@##@@@@@.$.\n" + \
	"..+@@@##@@@@@.$.\n" + \
	"..+@@@@@@@@@@.$.\n" + \
	"..+@@@@@@@@@@.$.\n" + \
	"..+@@@@@@@@@@.$.\n" + \
	"..+@@@@@@@@@@.$.\n" + \
	"..+@@@@@@@@@@.$.\n" + \
	"..+...........$.\n" + \
	"..++++++++++++$."

class TestSelection(Macro):
	def __init__(self):
		Macro.__init__(self, "te&st\tCtrl+B", MACRO_SELECTION, 0, "test", test_xpm)

	def Run(self):
		sel = cvsgui.App.GetSelection()
		for s in sel:
			print s.GetName()

			if sys.platform == 'darwin':
				# cvs complains when given a full path name...
				os.chdir(s.GetPath())
				file = GetHistoryAsXML(s.GetName())
			else:
				file = GetHistoryAsXML(s.GetFullName())
			
			print file
			f = open(file, "r")
			print f.read()
			f.close()
			os.unlink(file)

class TestAdmin(Macro):
	def __init__(self):
		Macro.__init__(self, "test2", MACRO_ADMIN, 0, "test", test_xpm)

	def OnCmdUI(self, cmdui):
		cmdui.SetText("This is a new title !")

		Macro.OnCmdUI(self, cmdui)

	def Run(self):
		console = ColorConsole()
		console << kRed << "This is red" << kBlue << " And this is blue\n"
		del console

TestSelection()
TestAdmin()
