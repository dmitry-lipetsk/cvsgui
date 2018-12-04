from cvsgui.Macro import *
from cvsgui.CvsEntry import *
from cvsgui.Persistent import *
from cvsgui.MenuMgr import *
import cvsgui.App, os.path, string

class CvsViewConfig(Macro):
	def __init__(self):
		Macro.__init__(self, "Config ViewCVS", MACRO_SELECTION,
			0, "ViewCVS")

		self.m_cvsView = Persistent("PY_CVSVIEW", "http://cvs.sourceforge.net/viewcvs.py/cvsgui/", 1)

	def OnCmdUI(self, cmdui):
		cmdui.Enable(1)

	def Run(self):
		msg = "Please enter the base URL for ViewCVS :"
		title = "ViewCVS settings"
		value = str(self.m_cvsView)
		answer, value = cvsgui.App.CvsPrompt(value, 'Question', msg, alertTitle=title)
		if (answer=='IDOK') and len(value) > 0:
			self.m_cvsView << value

class CvsViewBrowse(Macro):
	def __init__(self, cvsView):
		Macro.__init__(self, "ViewCVS Browser", MACRO_SELECTION,
			0, "ViewCVS")

		self.m_cvsView = cvsView

	def OnCmdUI(self, cmdui):
		# enable only if one cvs file or folder is selected
		sel = cvsgui.App.GetSelection()
		isGood = len(sel) == 1 and not sel[0].IsUnknown() and not sel[0].IsMissing()

		cmdui.Enable(isGood)
		if isGood:
			cmdui.SetText("ViewCVS browse '%s'" % sel[0].GetName())
		else:
			cmdui.SetText("ViewCVS browse")

	def myconcat(self, url, file):
		if url[-1] != '/':
			url += '/'
		if file[0] == '/':
			file = file[1:]
		return url + file

	def Run(self):
		sel = cvsgui.App.GetSelection()

		entry = sel[0]

		if entry.IsFile():
			rep = os.path.join(os.path.join(entry.GetPath(), "CVS"), "Repository")
		else:
			rep = os.path.join(os.path.join(entry.GetFullName(), "CVS"), "Repository")

		fd = open(rep, "r")
		rep = fd.readline()[:-1]
		fd.close()

		url = str(self.m_cvsView)
		url = self.myconcat(url, rep)
		if entry.IsFile():
			url = self.myconcat(url, entry.GetName())

		cvsgui.App.LaunchBrowser(url)

config = CvsViewConfig()
MenuSeparator(MACRO_SELECTION, "ViewCVS")
CvsViewBrowse(config.m_cvsView)
