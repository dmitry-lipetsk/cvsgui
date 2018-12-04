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

"""Cvs class, let you launch cvs
"""

import _cvsgui, sys

class Cvs:
	def __init__(self, quiet = 0, exitOnError = 1):
		"""- Initialize and optionally set the exitOnError flag : if true,
		cvs will throw an exception on an error (default is true).
		- The quiet flag tells if stderr and stdout of the cvs process are
		reported into the client console (default is false)"""

		self.m_exitOnError = exitOnError
		self.m_code = 0
		self.m_out = ""
		self.m_err = ""
		self.m_quiet = quiet

	def Run(self, *args):
		"""Launch a cvs process with some arguments, return the error code, stdout and
		stderr of the process. Throws if an error occur if exitOnError is true"""

		self.m_code, self.m_out, self.m_err = _cvsgui.RunCvs(args, self.m_exitOnError)

		if not self.m_quiet:
			if self.m_out != None:
				sys.stdout.write(self.m_out)
			if self.m_err != None:
				sys.stderr.write(self.m_err)

		return (self.m_code, self.m_out, self.m_err)
