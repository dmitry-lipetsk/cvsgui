from cvsgui.Macro import *
from cvsgui.CvsEntry import *
from cvsgui.Persistent import *
from cvsgui.MenuMgr import *
import cvsgui.App, os.path, string

class BonsaiConfig(Macro):
	def __init__(self):
		Macro.__init__(self, "Config Bonsai", MACRO_SELECTION,
			0, "Bonsai")

		self.m_Bonsai = Persistent("PY_BONSAI", "http://www.wincvs.org/bonsai", 1)

	def OnCmdUI(self, cmdui):
		cmdui.Enable(1)

	def Run(self):
		msg = "Please enter the base URL for Bonsai :"
		title = "Bonsai settings"
		value = str(self.m_Bonsai)
		answer, value = cvsgui.App.CvsPrompt(value, 'Question', msg, alertTitle=title)
		if (answer=='IDOK') and len(value) > 0:
			self.m_Bonsai << value

class BonsaiBrowse(Macro):
	def __init__(self, Bonsai):
		Macro.__init__(self, "Bonsai Browser", MACRO_SELECTION,
			0, "Bonsai")

		self.m_Bonsai = Bonsai

	def OnCmdUI(self, cmdui):
		# enable only if one cvs file or folder is selected
		sel = cvsgui.App.GetSelection()
		isGood = len(sel) == 1 and not sel[0].IsUnknown() and not sel[0].IsMissing()

		cmdui.Enable(isGood)
		if isGood:
			cmdui.SetText("Bonsai browse '%s'" % sel[0].GetName())
		else:
			cmdui.SetText("Bonsai Browse")

	def myconcat(self, url, file):
		if url[-1] != '/':
			url += '/'
		if file[0] == '/':
			file = file[1:]
		return url + file

	def Run(self):
		sel = cvsgui.App.GetSelection()

		entry = sel[0]

		rep = os.path.join(os.path.join(entry.GetPath(), "CVS"), "Repository")
		fd = open(rep, "r")
		rep = fd.readline()[:-1]
		fd.close()

		cvsroot = os.path.join(os.path.join(entry.GetPath(), "CVS"), "Root")
		fd = open(cvsroot, "r")
		cvsroot = fd.readline()[:-1]
		ind = string.rfind(cvsroot, ':')
		cvsroot = cvsroot[ind+1:]
		fd.close()

		tag = os.path.join(os.path.join(entry.GetPath(), "CVS"), "Tag")
		try:
			fd = open(tag, "r")
			tag = fd.readline()[1:-1]
			fd.close()
		except:
			tag = "HEAD"

		url = str(self.m_Bonsai)
		url = self.myconcat(url, "cvsview2.cgi")
		
		arg = "subdir=" + rep
		arg += "&files=" + entry.GetName()
		arg += "&branch=" + tag
		arg += "&root=" + cvsroot
		arg += "&command=DIRECTORY"

		cvsgui.App.LaunchBrowser(url + '?' + arg)

config = BonsaiConfig()
MenuSeparator(MACRO_SELECTION, "Bonsai")
BonsaiBrowse(config.m_Bonsai)
