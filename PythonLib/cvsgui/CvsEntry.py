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

"""CvsEntry class, information for an entry of CVS/Entries
"""

import _cvsgui, os.path

class CvsEntry:
	def __init__(self, entry):
		self.m_entry = entry

	def GetName(self):
		"""Return the name of the file/folder"""

		return self.m_entry.name

	def GetFullName(self):
		"""Return the full qualified name of the file/folder"""

		return self.m_entry.fullname

	def GetPath(self):
		"""Return the full qualified name of the parent folder"""

		return os.path.dirname(self.m_entry.fullname)

	def GetVersion(self):
		"""Return the revision number of the file"""

		return self.m_entry.version

	def GetUTC(self):
		"""Return the modification time (in UTC) of the repository file which was used
		to update the file for the last time"""

		return self.m_entry.timeutc

	def GetKeyword(self):
		"""Return the substitution keyword for the file"""

		return self.m_entry.keyword

	def GetTag(self):
		"""Return the name of the tag (or the branch) where the file is
		checked out"""

		return self.m_entry.tag

	def IsFile(self):
		"""Return true for a file, false for a folder"""

		return self.m_entry.isfile

	def IsModified(self):
		"""Return true if the file is modified locally"""

		return self.m_entry.modified

	def IsMissing(self):
		"""Return true if the file/folder is missing locally"""

		return self.m_entry.missing

	def IsUnknown(self):
		"""Return true if the file/folder is not part of cvs (i.e. needs
		to be added)"""

		return self.m_entry.unknown

	def IsIgnored(self):
		"""Return true if the file/folder is ignored because of the presence of a
		.cvsignore file (either in the same folder or in the home folder)"""

		return self.m_entry.ignored

	def IsReadOnly(self):
		"""Return true if the file is read only on the file system"""

		return self.m_entry.readonly

	def IsRemoved(self):
		"""Return true if the file is scheduled for removing"""

		return self.m_entry.removed

	def IsAdded(self):
		"""Return true if the file is scheduled for adding"""

		return self.m_entry.added

	def GetDesc(self):
		"""Return a literal description of the state of the file"""

		return self.m_entry.desc

	def GetConflict(self):
		"""Return the conflict information for the file (if any)
		or an empty string"""

		return self.m_entry.conflict

def GetCvsEntriesList(path):
	"""Return a list of CvsEntry for the directory"""

	intern = _cvsgui.GetCvsEntries(path)
	res = []
	for i in intern:
		res.append(CvsEntry(i))
	return res;

def GetCvsEntriesDict(path):
	"""Return a dictionary of CvsEntry (filename -> CvsEntry)"""

	intern = _cvsgui.GetCvsEntries(path)
	res = {}
	for i in intern:
		res[i.name] = CvsEntry(i)
	return res;

def GetFileModTime(path):
	"""Return the UTC modification time for a file. Corrects for DST bug."""

	intern = _cvsgui.GetFileModTime(path)
	return intern