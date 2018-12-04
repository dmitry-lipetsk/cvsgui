from cvsgui.Macro import *
from cvsgui.CvsEntry import *
from cvsgui.ColorConsole import *
import cvsgui.App, os.path

class FastSearh(Macro):
	def __init__(self):
		Macro.__init__(self, "Fast search modified", MACRO_SELECTION,
			0, "Search")

	def OnCmdUI(self, cmdui):
		# enable only is regular folder are selected
		sel = cvsgui.App.GetSelection()
		isOnlyCvsFolder = len(sel) > 0
		for s in sel:
			if s.IsFile() or s.IsMissing() or s.IsUnknown():
				isOnlyCvsFolder = 0
				break

		cmdui.Enable(isOnlyCvsFolder)
		if isOnlyCvsFolder and len(sel) == 1:
			cmdui.SetText("Fast search modified in '" + sel[0].GetName() + "'")
		else:
			cmdui.SetText("Fast search modified")

	def Run(self):
		print "Looking for modified files..."
		
		# at this point we are sure we have only regular folders
		sel = cvsgui.App.GetSelection()

		numFound = 0
		for s in sel:
			numFound += self.DoFolder(s.GetFullName())

		print str(numFound) + " file(s) found."

	def DoFolder(self, path):
		numFound = 0
		toRecurse = []
		list = {}
		list["added"] = []
		list["removed"] = []
		list["modified"] = []
		list["missing"] = []
		list["conflict"] = []

		# build a list of interesting files for this path
		entries = GetCvsEntriesList(path)
		for e in entries:
			if e.IsFile():
				if e.IsRemoved():
					list["removed"].append(e)
					numFound += 1
				elif e.IsAdded():
					list["added"].append(e)
					numFound += 1
				elif e.IsModified():
					if e.GetConflict() != "":
						list["conflict"].append(e)
					else:
						list["modified"].append(e)
					numFound += 1
				elif e.IsMissing():
					list["missing"].append(e)
					numFound += 1
						
			elif not e.IsMissing() and not e.IsUnknown():
				toRecurse.append(e.GetFullName())

		# now print the list
		if len(list["added"]) > 0 or len(list["removed"]) > 0 or len(list["modified"]) > 0 or len(list["missing"]) > 0 or len(list["conflict"]) > 0:

			print "In %s :" % path

			console = ColorConsole()

			if len(list["conflict"]) > 0:
				console << kBold << "\tConflict files :" << kNormal << '\n'
				for e in list["conflict"]:
					console << kRed << "\t\t" << e.GetName() << kNormal << '\n'

			if len(list["added"]) > 0:
				console  << "\tAdded files :" << '\n'
				for e in list["added"]:
					console << kMagenta << "\t\t" << e.GetName() << kNormal << '\n'

			if len(list["removed"]) > 0:
				console  << "\tRemoved files :" << '\n'
				for e in list["removed"]:
					console << kMagenta << "\t\t" << e.GetName() << kNormal << '\n'

			if len(list["modified"]) > 0:
				console  << "\tModified files :" << '\n'
				for e in list["modified"]:
					console << kMagenta << "\t\t" << e.GetName() << kNormal << '\n'

			if len(list["missing"]) > 0:
				console << kBold << "\tMissing files :" << kNormal << '\n'
				for e in list["missing"]:
					console << kRed << "\t\t" << e.GetName() << kNormal << '\n'

		# recurse
		for f in toRecurse:
			numFound += self.DoFolder(f)

		return numFound

FastSearh()
