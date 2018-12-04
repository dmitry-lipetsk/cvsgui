from cvsgui.Macro import *
from cvsgui.ColorConsole import *
from cvsgui.Cvs import *
import cvsgui.App
import StringIO

class ListTags(Macro):
	def __init__(self):
		Macro.__init__(self, "List module tags", MACRO_SELECTION, 0, "Search")
		self.m_lastModule = ""
	def OnCmdUI(self, cmdui):
		cmdui.Enable(1)
	def Run(self):
		cvs = Cvs(1, 1)
		console = ColorConsole ()
		
		answer, module = App.CvsPrompt(self.m_lastModule, "Question", \
						   "Enter the module name:", \
						   alertTitle="Query existing tags")

		if answer!='IDOK':
			return
		
		self.m_lastModule = module
		console << kBlue <<"Querying tags ...\n"
		try:
			code, out, err = cvs.Run("rlog", module)
		except:
			console << kRed << "cvs error, bad module name maybe ?\n"
			console << kNormal
			return
		output = StringIO.StringIO(out)
		line = output.readline()
		inTag = 0
		tags = []

		console << kBlue <<"Processing tags : "
		
		while not line == "":
			if inTag and string.find(line, "keyword") == -1:
				try:
					if tags.index(line[1:string.find(line, ":")]) >= 0:
						pass
				except ValueError:
					console << ". "
					tags.append(line[1:string.find(line, ":")])
			if not string.find(line, "symbolic") == -1:
				inTag = 1
			elif not string.find(line, "keyword") == -1:
				inTag = 0
			line = output.readline()
		console << kNormal << kBold << kUnderline << "\nTags for module " + module + ":\n"
		console << kNormal
		for tag in tags:
			console << kRed<< "\t" + tag + "\n"
		console << kNormal
		del console

ListTags()

