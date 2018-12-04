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

"""Macro class, extend the capabilities of cvsgui
"""

import MacroRegistry, App, _cvsgui, MenuMgr

# to use with MacroRegistry::AddMacro
MACRO_SELECTION = "SELECTION"
MACRO_ADMIN = "ADMIN"

class Macro:
	"""This is the macro to overide to implement a macro of yours"""

	def __init__(self, title, group, isCheck=0, subgroup="", xpmicon=""):
		self.m_title = title
		self.m_group = group
		self.m_isCheck = isCheck
		self.m_subgroup = subgroup
		self.m_xpmicon = xpmicon
		self.m_cmdid = 0
		self.m_menu = None

		MacroRegistry.gMacroRegistry.AddMacro(self)

	def OnCmdUI(self, cmdui):
		"""Called from the interface for the macro to define its state.
		Overide to change the default behavior"""

		if self.m_group == MACRO_SELECTION:
			sel = App.GetSelection()
			cmdui.Enable(len(sel) > 0)
		else:
			cmdui.Enable(1)

	def Run(self):
		"""Called from the interface when the macro is selected"""

class TclMacro(Macro):
	"""This class wraps Tcl macros"""

	def __init__(self, title, group, file):
		Macro.__init__(self, title, group, 0, "Tcl")

		self.m_file = file

	def Run(self):
		"""Called from the interface when the macro is selected"""

		# default launch the Tcl Macros
		_cvsgui.LaunchTclMacro(self.m_file)

class MenuSeparator(Macro):
	"""This class is a convenient way to insert a menu separator"""

	def __init__(self, group, subgroup=""):
		Macro.__init__(self, "-", group, 0, subgroup)
