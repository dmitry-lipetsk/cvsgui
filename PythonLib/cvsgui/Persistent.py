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

"""Persistent class, allows to store persistent values on disk,
values you can read after the application restarts
"""

import _cvsgui

class Persistent:
	"""create/access a persistent value, the token is used to
	identify the persistent value"""

	def __init__(self, token, defvalue="", addToSettings=0):
		self.m_value = _cvsgui.CPersistent(token, defvalue, addToSettings)
		
	def GetValue(self):
		"""Return the current value"""

		return self.m_value.GetValue()

	def SetValue(self, value):
		"""Return the current value"""

		self.m_value.SetValue(value)

	def __lshift__(self, value):
		"""ex: Persistent << 'value'"""

		self.SetValue(value)

	def __str__(self):
		return self.GetValue()
