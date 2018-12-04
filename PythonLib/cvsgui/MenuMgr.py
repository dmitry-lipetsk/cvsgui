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

"""Menu manager class, manipulates the menu bar of the cvsgui client
"""

import _cvsgui, sys

class Menu:
	"""Menu class, let you extend the menu entries"""

	def __init__(self, menu):
		m_menu = menu

	def AddPulldown(self, widid, name):
		"""Add a pulldown to the top menu. If top is NULL,
		then add it to the main menu bar"""

		return self.m_menu.AddPulldown(widid, name)

	def AddToggle(self, widid, name, cmd, pixmap = ""):
		"""Add a check box to the top menu."""
			
		self.m_menu.AddToggle(widid, name, cmd, pixmap)

	def AddButton(self, widid, name, cmd, pixmap = ""):
		"""Add a regular text entry to the top menu."""
		
		self.m_menu.AddButton(widid, name, cmd, pixmap)

	def AddSeparator(self, widid):
		"""Add a separator to the top menu."""
			
		self.m_menu.AddSeparator(widid)

	def Delete(self, afterIndex):
		"""Remove a menu entry after an index (all of them)."""
			
		self.m_menu.Delete(afterIndex)

	def DeleteByCmd(self, cmd):
		"""Remove a menu entry by command id."""
			
		self.m_menu.DeleteByCmd(cmd)

	def Count(self):
		"""Return the number of menu entries in the top menu."""
			
		return self.m_menu.Count()

gCmdIDStart = 0

def ResetCmdID():
	"""Reset command ID to use for a menu entry"""
	global gCmdIDStart
	if sys.platform == "win32":
		gCmdIDStart = 34000
	elif sys.platform == "mac" or sys.platform == "darwin":
		# cmd_FirstMacro in MacCvsConstant.h
		gCmdIDStart = 3200
	else:
		gCmdIDStart = 10000

def GetNewCmdID():
	"""Return a new command ID to use for a menu entry"""

	global gCmdIDStart
	if gCmdIDStart == 0:
		ResetCmdID()

	gCmdIDStart += 1
	return gCmdIDStart

def GetTopMacroSelectionMenu():
	"""Return the default pulldown for the macro selection menu"""

	return _cvsgui.GetTopMacroSelectionMenu()

def GetTopMacroAdminMenu():
	"""Return the default pulldown for the macro admin menu"""

	return _cvsgui.GetTopMacroAdminMenu()

def GetMainMenuWidget():
	"""Return the id of the default menu controller (pseudo widget)"""

	return _cvsgui.GetMainMenuWidget()

