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

"""App class, provides interaction with the cvsgui application
"""

import sys, _cvsgui, cvsgui.CvsEntry

def cvs_out(s):
	"""Print text in the application console

	This is for backward compatibility only,
	you can use sys.stdout directly
	"""
	sys.stdout.write(s)

def cvs_err(s):
	"""Print text in the application console

	This is for backward compatibility only,
	you can use sys.stderr directly
	"""
	sys.stderr.write(s)

def GetSelection():
	"""Return a list of the currently selected CvsEntry in the
	file or folder browser"""

	sel = _cvsgui.GetSelection()
	res = []
	for i in sel:
		res.append(cvsgui.CvsEntry.CvsEntry(i))
	return res

def PromptMessage(msg, title="Message", default="OK", cncl="Cancel"):
	"""Ask to prompt a message and return 1 (default) or 0 (cancel)"""

	return _cvsgui.PromptMessage(msg, title, default, cncl)

def PromptEditMessage(msg, value, title="Message", default="OK", cncl="Cancel"):
	"""Ask to prompt a message and edit a value and return a list with
	1 (default) or 0 (cancel) + the value"""

	return _cvsgui.PromptEditMessage(msg, value, title, default, cncl)

def LaunchBrowser(url):
	"""Launch a web browser with the URL"""

	_cvsgui.LaunchBrowser(url)

def CvsAlert(icon, message, messageDetails="", okButtonTitle="", cancelButtonTitle="", otherButtonTitle="", defaultButton="", alertTitle=""):
	""" Display Alert message, values:
	icon:				[NoIcon | Stop | Note | Warning | Question]
	message:			Main alert message
	messageDetails:		Details of the alert message
	okButtonTitle:		OK button title
	cancelButtonTitle:	Cancel button title
	otherButtonTitle:	Third button title
	defaultButton:		[IDOK | IDCANCEL | IDOTHER]
	alertTitle:			Alert title
	
	return:				[IDOK | IDCANCEL | IDOTHER]"""

	return _cvsgui.CvsAlert(icon, message, messageDetails, okButtonTitle, cancelButtonTitle, otherButtonTitle, defaultButton, alertTitle)

def CvsPrompt(value, icon, message, messageDetails="", okButtonTitle="", cancelButtonTitle="", otherButtonTitle="", defaultButton="", alertTitle=""):
	""" Display Prompt message, values same as CvsAlert plus:
	value:				Value
	
	return:				([IDOK | IDCANCEL | IDOTHER], value)"""

	return _cvsgui.CvsPrompt(value, icon, message, messageDetails, okButtonTitle, cancelButtonTitle, otherButtonTitle, defaultButton, alertTitle)

def Exit(exitCode):
	"""Exit application"""

	_cvsgui.Exit(exitCode)