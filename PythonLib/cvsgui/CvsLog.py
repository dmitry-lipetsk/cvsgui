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

"""CvsLog class, return the history of the file
"""

import _cvsgui, cvsgui.Cvs, tempfile, os

def GetHistoryAsXML(filename):
	"""Return the cvs history of the file in an xml format,
	it returns the result in a temporay file you can erase or
	rename afterwhile"""

	cvs = cvsgui.Cvs.Cvs(1)
	code, out, err = cvs.Run("log", filename)

	textfile = tempfile.mktemp()
	xmlfile = tempfile.mktemp() + ".xml"
	f = open(textfile, 'w')
	f.write(out)
	f.close()

	f = open(textfile, 'r')
	fd = open(xmlfile, 'w')
	res = _cvsgui.GetHistoryAsXML(f, fd)
	f.close()
	fd.close()

	os.unlink(textfile)

	return xmlfile
