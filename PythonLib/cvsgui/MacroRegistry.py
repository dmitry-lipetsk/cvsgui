# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

"""Macro registry class, place to register and manipulate macros
"""

import _cvsgui, MenuMgr, os, code, Macro, CmdUI, sys, getopt, compileall, traceback
from Macro import *

class MacroRegistry:
	"""Class which retains the active macros"""
	def __init__(self):
		self.m_all = []
		self.m_reloadID = MenuMgr.GetNewCmdID()
		self.m_macroFolder = ""

	def AddMacro(self, macro):
		"""Add a macro to the registry of macros"""

		if macro.m_group != MACRO_SELECTION and macro.m_group != MACRO_ADMIN:
			raise ValueError, 'wrong macro group ' + group		

		self.m_all.append(macro)

	def CreateMenusForGroup(self, top, group):
		wid = MenuMgr.GetMainMenuWidget()
		subgroups = {}

		for m in self.m_all:
			
			if m.m_group != group:
				continue

			if m.m_subgroup == "":
				menu = top
			else:
				try:
					menu = subgroups[m.m_subgroup]
				except:
					menu = top.AddPulldown(wid, m.m_subgroup)
					subgroups[m.m_subgroup] = menu

			m.m_menu = menu
			if m.m_title == '-':
				menu.AddSeparator(wid)
			else:
				m.m_cmdid = MenuMgr.GetNewCmdID()
				if m.m_isCheck:
					menu.AddToggle(wid, m.m_title, m.m_cmdid, m.m_xpmicon)
				else:
					menu.AddButton(wid, m.m_title, m.m_cmdid, m.m_xpmicon)

	def CreateMenus(self):
		"""Called by the cvsgui client to create the macros menus"""

		top = MenuMgr.GetTopMacroSelectionMenu()
		wid = MenuMgr.GetMainMenuWidget()
		top.AddButton(wid, "&ReloadMacro\tCtrl+Alt+R", self.m_reloadID)
		top.AddSeparator(wid)

		self.CreateMenusForGroup(MenuMgr.GetTopMacroSelectionMenu(), MACRO_SELECTION)
		self.CreateMenusForGroup(MenuMgr.GetTopMacroAdminMenu(), MACRO_ADMIN)
	
	def AddTclMacro(self, filename):
		try:
			f = open(filename)
			buf = f.readline()
			f.close()
			
			header = "#!CVSGUI1.0"
			if buf[0:len(header)] != header:
				return

			buf = buf[len(header)+1:]

			hasQuote = 0
			arg = ""
			args = []

			for c in buf:
				if hasQuote:
					if c == '"':
						hasQuote = 0
						args.append(arg)
						arg = ""
					else:
						arg += c

					continue
					
				if c.isspace():
					if len(arg) > 0:
						args.append(arg)
						arg = ""
					continue

				if c == '"':
					hasQuote = 1
					continue

				arg += c

			if len(arg) > 0:
				args.append(arg)
				arg = ""

			hasAdmin = 0
			hasSel = 0
			dispname = ""
			opts, args = getopt.getopt(args, "", ["selection", "admin", "name="])
			for o, v in opts:
				if o == "--selection":
					hasSel = 1
				elif o == "--admin":
					hasAdmin = 1
				elif o == "--name":
					dispname = v

			if dispname == "":
				return

			if hasSel:
				macro = TclMacro(dispname, MACRO_SELECTION, filename)
			elif hasAdmin:
				macro = TclMacro(dispname, MACRO_ADMIN, filename)

		except:
			sys.stderr.write("Could not load Tcl macro %s\n" % filename)
		

	def LoadMacrosRecurs(self, dir):
		files = os.listdir(dir)
		for file in files:
			filename = dir + os.sep + file
			if os.path.isdir(filename):
				self.LoadMacrosRecurs(filename)
			else:
				base, ext = os.path.splitext(filename)
				if ext == ".py":
					try:
						execfile(filename, self.m_globals)
					except:
						sys.stderr.write("Error loading %s:\n" % filename)
						traceback.print_exception(*sys.exc_info())
				elif ext == ".tcl":
					self.AddTclMacro(filename)
		
	def LoadMacros(self, dir):
		"""Iterate thru a folder and evaluate each python file"""

		#compileall.compile_dir(dir)
		self.m_globals = globals().copy()
		self.m_macroFolder = dir
		self.DeleteMenus()
		self.m_all = []
		MenuMgr.ResetCmdID()
		self.LoadMacrosRecurs(dir)
		self.CreateMenus()

	def DeleteMenus(self):
		"""Delete the menus used by the macros"""

		for m in self.m_all:
			if m.m_menu != None and m.m_cmdid != 0:
				m.m_menu.DeleteByCmd(m.m_cmdid)

		menu = MenuMgr.GetTopMacroSelectionMenu()
		menu.Delete(0)
		menu = MenuMgr.GetTopMacroAdminMenu()
		menu.Delete(0)

	def ReloadMacros(self):
		"""Reload the macro, re-create the menus"""

		self.LoadMacros(self.m_macroFolder)

	def DoCmdUI(self, cmdui, cmdid):
		"""Triggered by the interface when a menu entry needs to
		update its state"""

		if cmdid == self.m_reloadID:
			cmdui.Enable(1)
			return

		for m in self.m_all:
			if m.m_cmdid == cmdid:
				m.OnCmdUI(cmdui)
				break

	def DoCmd(self, cmdid):
		"""Triggered by the interface when a menu entry is selected"""

		if cmdid == self.m_reloadID:
			self.ReloadMacros()
			return

		for m in self.m_all:
			if m.m_cmdid == cmdid:
				m.Run()
				break

gMacroRegistry = MacroRegistry()
