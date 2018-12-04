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

"""CmdUI class, set the state of an object in the interface
"""

import _cvsgui

TRUE = 1
FALSE = 0

class CmdUI:
	"""This is the same as the CmdUI on Windows which let
	you define a state for an object"""

	def __init__(self, cmdui):
		self.m_cmdui = _cvsgui.CCmdUI(cmdui)

	def Enable(self, state):
		"""Enable/Disable the object (i.e. menu entry, icon...)"""

		self.m_cmdui.Enable(state)

	def SetCheck(self, state):
		"""Check/uncheck the object (i.e. menu entry, icon...)"""

		self.m_cmdui.SetCheck(state)

	def SetText(self, text):
		"""Rename the object (i.e. menu entry, icon...)"""

		self.m_cmdui.SetText(text)
