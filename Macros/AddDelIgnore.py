from cvsgui.Macro import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
import cvsgui.App, os.path, string, sys

class AddIgnore(Macro):
	def __init__(self):
		Macro.__init__(self, "Add to .cvsignore", MACRO_SELECTION,
			0, "Ignore")

	def OnCmdUI(self, cmdui):
		# enable only if one unknown file is selected
		sel = cvsgui.App.GetSelection()
		isOnlyUnknown = len(sel) == 1 and sel[0].IsUnknown()

		cmdui.Enable(isOnlyUnknown)
		if isOnlyUnknown:
			cmdui.SetText("Add '%s' to .cvsignore" % sel[0].GetName())
		else:
			cmdui.SetText("Add to .cvsignore")

	def Run(self):
		sel = cvsgui.App.GetSelection()

		entry = sel[0]

		root, ext = os.path.splitext(entry.GetName())
		if ext != "":
			ign = "*" + ext;
		else:
			ign = root

		msg = "Add '%s' to .cvsignore ?" % ign
		title = "Add to .cvsignore"
		answer, ign = cvsgui.App.CvsPrompt(ign, 'Question', msg, alertTitle=title)
		if (answer!='IDOK') or len(ign) == 0:
			return

		cvsignore = os.path.join(entry.GetPath(), ".cvsignore")
		createIgnore = 0
		if not os.path.isfile(cvsignore):
			createIgnore = 1

		if createIgnore:
			msg = "Create and add '%s' to the sandbox ?" % cvsignore
			title = "Create .cvsignore"
			if cvsgui.App.CvsAlert('Question', msg, 'Yes', 'No', '', '', title) != 'IDOK':
				return

		fd = open(cvsignore, "a+")
		entries = fd.readlines()
		entries.append(ign + "\n")
		fd.truncate(0)
		fd.writelines(entries)
		fd.close()

		if createIgnore:
			cvs = Cvs()
			dirname, filename = os.path.split(cvsignore) 
			os.chdir(dirname) 
			code, out, err = cvs.Run("add", filename) 

AddIgnore()
