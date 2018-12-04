from cvsgui.Macro import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
import re, string

class ListModules(Macro):
	def __init__(self):
		Macro.__init__(self, "Get the cvs modules", MACRO_ADMIN)

	def Run(self):
		cvs = Cvs(1)
		code, out, err = cvs.Run("co", "-c")

		console = ColorConsole()
		if out:
			lines = string.split(out, '\n')
			test1 = re.compile('^([\w-]+)(\s*)(.*)')
			test2 = re.compile('^(\s*)(.*)')
			for l in lines:
				m = test1.match(l)
				if m != None:
					console << kRed << m.group(1) << kNormal << '\n'
					console << kBlue << '\t' << m.group(3) << kNormal << '\n'
				else:
					m = test2.match(l)
					if m != None:
						console << kBlue << '\t' << m.group(2) << kNormal << '\n'
					else:
						console << l << '\n'
		else:
			console << kRed << "No module entries found! Consult your repository's admin.\n" << kNormal
					
		del console

ListModules()
