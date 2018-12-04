from cvsgui.App import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
import os, os.path

"""
  WinCvs Macro "Hide/Unhide admin files"
  $Revision: 1.2 $

  written by Oliver Giesen, Sep 2002
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    icq:    18777742

  Feel free to modify or distribute in whichever way you like,
   as long as it doesn't limit my personal rights to
   modify and redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has been tested against WinCvs 1.3.8 .

  NOTE:
   This macro works best with either the ActivePython distribution
   or if you have installed the Win32 Python extensions
   (available from http://starship.python.net/crew/mhammond/win32).
   The macro has not been heavily tested without these extensions
   although it should work without them as well (you will see command
   windows pop up for every attrib operation).

  ======
  Usage:

  - Select one or more folders

  - Choose either Hide or Unhide from the Macros|Admin files menu

   ~watch the admin areas (dis)appear... ;)

  =============
  Known Issues / "Un-niceties":

  - If you haven't got the Win32 extensions installed the attrib operations
    have to be carried out by opening a command prompt and calling the attrib
    command. This is ugly but it works. There is unfortunately no generic
    wrapper for controlling file attributes in the core library as they are a
    Windows-only feature.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

try:
  import win32file
  hidden = win32file.FILE_ATTRIBUTE_HIDDEN
  useWin32 = 1
except:
  useWin32 = 0

def isHidden( fd):
  global hidden
  if useWin32:
    return win32file.GetFileAttributes( fd) & hidden <> 0
  else:
    return 0

def setHidden( fd, hide):
  global hidden
  print fd,
  if useWin32:
    fa = win32file.GetFileAttributes( fd)
    print fa,
    if hide:
      win32file.SetFileAttributes( fd, fa | hidden)
    else:
      if fa & hidden:
        win32file.SetFileAttributes( fd, fa ^ hidden)
    print win32file.GetFileAttributes( fd)
  else:
    if hide:
      setter = '+h'
    else:
      setter = '-h'
#    code = os.spawnlp( os.P_WAIT, 'attrib', setter, fd, '/s', '/d')
    code = os.system( 'attrib %s %s /s /d' % ( setter, fd))
    print code

class AdminFilesVisibility( Macro):
  def __init__( self, caption, hide):
    Macro.__init__( self, caption, MACRO_SELECTION, 0, 'Admin files')
    self.hide = hide
    
  def OnCmdUI( self, cmdui):
    self.sel = App.GetSelection()
    enabled = len( self.sel) > 0
    if enabled:
      for entry in self.sel:
        if entry.IsFile():
          enabled=0
          break
      #enable/disable based on existing hidden flag
#      if useWin32:
#        fd = os.path.join( self.sel[0].GetFullName(), 'CVS')
#        enabled = self.hide ^ isHidden( fd)
    cmdui.Enable( enabled)
  
  def SetEntriesHidden( self, entries, isAdmin = 0):
    for entry in entries:
      fn = entry.GetFullName()
      if not entry.IsFile():
        if isAdmin:
          #hide folders within the admin area:
          setHidden( fn, self.hide)

        if not entry.IsUnknown():
          cvsDir = os.path.join( fn, 'CVS')
          #hide admin area:
          setHidden( cvsDir, self.hide)

          if useWin32:
            #SetFileAttributes does not recurse, so we have to do it manually:
            self.SetEntriesHidden( GetCvsEntriesList( cvsDir), 1)
            
        #recurse:
        self.SetEntriesHidden( GetCvsEntriesList( fn), isAdmin)

      elif isAdmin and entry.IsUnknown():
        #hide files in the admin area:
        setHidden( fn, self.hide)

  def Run( self):
    if self.hide:
      prefix = 'H'
    else:
      prefix = 'Unh'
    print prefix+'iding admin files...'
    self.SetEntriesHidden( self.sel)
    print 'Done.'

class HideAdminFiles( AdminFilesVisibility):
  def __init__( self):
    AdminFilesVisibility.__init__( self, 'Hide', 1)

class UnhideAdminFiles( AdminFilesVisibility):
  def __init__( self):
    AdminFilesVisibility.__init__( self, 'Unhide', 0)

if os.name == "nt":
  HideAdminFiles()
  UnhideAdminFiles()
