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

"""ColorConsole class, provides *some* coloring in the console window
"""

import _cvsgui, sys, operator

kNormal = _cvsgui.kNormal
kBrown = _cvsgui.kBrown
kGreen = _cvsgui.kGreen
kMagenta = _cvsgui.kMagenta
kRed = _cvsgui.kRed
kBlue = _cvsgui.kBlue
kBold = _cvsgui.kBold
kItalic = _cvsgui.kItalic
kUnderline = _cvsgui.kUnderline
kNL  = _cvsgui.kNL 

class ColorConsole:
	def __init__(self):
		self.m_console = _cvsgui.CColorConsole()
		self << kNormal

	def __del__(self):
		self << kNormal

	def out(self, stringOrStyle):
		"""Print text in the application console
		"""

		self.m_console.Print(stringOrStyle)

	def __lshift__(self, stringOrStyle):
		"""ex: ColorConsole() << kRed << "This is red"
		"""

		self.m_console.Print(stringOrStyle)		
		return self
