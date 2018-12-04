"""
  WinCvs Macro "Edit association"
  $Revision: 1.5 $

  written by Oliver Giesen, Jan 2004 - Jul 2005
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    icq:    18777742
    y!:     ogiesen

  Feel free to modify or distribute in whichever way you like, 
   as long as it doesn't limit my personal rights to
   modify and redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has been written against WinCvs 1.3.14
   and was last tested against WinCvs 2.0.3 .
   You will also need either Mark Hammond's Win32 extensions for Python
   (from http://starship.python.net/crew/skippy/win32/)
   or simply use ActivePython (which already contains Mark's extensions
   by default). Without this the macro will not be registered on the menu.

  ======
  Usage:

  - Select a file.

  - Run the macro from the Macros menu

   ~A dialog box will pop up, presenting you the old "edit" association for
    the selected file type, or, if there isn't one already, it will propose
    either the current "open" association or the default association (if one
    exists).

  - Enter or confirm the new "edit" association for the selected file type

   ~The relevant information will be written to the registry and you should now
    be able to open files of the selected type in their specified editor
    application simply by double-clicking them in WinCvs.
   
  =============
  Known Issues / "Un-niceties":

  - It'd be nice if one could browse for a handler application instead of having
   to enter it manually.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

try:
  from win32api import *
  import win32con
  py4win32 = 1
except:
  py4win32 = None
  
from cvsgui.App import *
from cvsgui.Macro import *
import string, os, os.path

class EditFileTypeAssoc(Macro):
  def __init__(self):
    self.caption = 'Edit association for "%s"...'
    Macro.__init__(self, self.caption % 'selection', MACRO_SELECTION, 0, )

  def OnCmdUI(self, cmdui):
    sel = App.GetSelection()
    enabled =    (len(sel) == 1) \
              and(sel[0].IsFile())
    if enabled:
      fn, self.ext = os.path.splitext(sel[0].GetName())
      self.ext = string.lower(self.ext)
      cmdui.SetText(self.caption % self.ext)
      enabled = self.ext != ''
    cmdui.Enable(enabled)

  def QueryShAssoc(self, progId, verb):
    cmd = ''
    typ = 0
    try:
      try:
        k = RegOpenKeyEx(win32con.HKEY_CLASSES_ROOT, '%s\\shell\\%s\\command' % (progId, verb))
        cmd, typ = RegQueryValueEx(k, '')
        print 'Found "%s" handler for ProgID "%s", command = "%s"' % (verb, progId, str(cmd))
      finally:
        RegCloseKey(k)
    except:
      #print 'Could not determine "%s" handler for ProgID "%s"' % (verb, progId)
      cmd = None
    return cmd

  def QueryEditAssoc(self):
    originOfValue = ''
    print 'Analyzing existing shell associations for %s-files ...' % self.ext
    try:
      progId = RegQueryValue(win32con.HKEY_CLASSES_ROOT, '%s' % self.ext)
      if progId <> '' and progId <> None:
        print '\tEdit...'
        editCmd = self.QueryShAssoc(progId, 'edit')
        if editCmd == None:
          originMsg = '\nNote: The value shown below corresponds to the current "%s" ' \
                    + 'handler as there is no "edit" handler defined yet'
          print '\tOpen...'
          editCmd = self.QueryShAssoc(progId, 'open')
          if editCmd == None:
            print '\tDefault...',
            try:
              defaultVerb = RegQueryValue(win32con.HKEY_CLASSES_ROOT, '%s\\shell' % progId)
              if defaultVerb != '':
                print '("%s")...' % defaultVerb,
                editCmd = self.QueryShAssoc(progId, defaultVerb)
                if editCmd != '':
                  originOfValue = originMsg % defaultVerb
            except:
              editCmd = ''
            print ''
          else:
            originOfValue = originMsg % 'open'
      else:
        Abort
    except:
      cvs_err('Could not determine any existing shell associations for %s-files!\n' % self.ext)
      progId = '%sfile' % self.ext[1:]
      editCmd = ''
          
    if editCmd == None:
      editCmd = ''

    return editCmd, progId, originOfValue

  def Run(self):
    editCmd, progId, defOrigin = self.QueryEditAssoc()
    
    ok = 0
    newCmd = ''
    while (not ok) and (string.find(newCmd, '%1') < 0):
      answer, newCmd = App.CvsPrompt(editCmd, 'Question', \
                                     'Enter default "edit" command line.',
                                     'Will apply to all "%s" files (%s)\n%s' % (self.ext, progId, defOrigin), \
                                     alertTitle='Edit association')
      ok = (answer == 'IDOK')
      if ok:
        if string.find(newCmd, '%1') < 0:
          ok = App.CvsAlert('Question', 'The command line must contain the "%1" placeholder.', \
                            'Append it now?\n\n(i.e. %s "%%1")' % newCmd,
                            'Yes', 'No', '', '', 'Missing placeholder') == 'IDOK'
          if ok:
            newCmd = '%s "%%1"' % newCmd
      else:
        break
    
    if ok and (newCmd != editCmd or defOrigin != ''):
      if editCmd == '': # there was no existing file handler, create a ProgID
        print 'Setting HKEY_CLASSES_ROOT\\%s\\@ = %s' % (self.ext, progId)
        RegSetValue(win32con.HKEY_CLASSES_ROOT, self.ext, win32con.REG_SZ, progId)
      k = RegCreateKey(win32con.HKEY_CLASSES_ROOT, '%s\\shell\\Edit\\command' % progId)
      try:
        print 'Setting HKEY_CLASSES_ROOT\\%s\\shell\\edit\\command\\@ = %s' % (progId, newCmd)
        RegSetValueEx(k, '', 0, win32con.REG_EXPAND_SZ, newCmd)
      finally:
        RegCloseKey(k)

    if ok:
      print 'Done.\n'
    else:
      cvs_err('Cancelled.\n\n')

if py4win32:
  EditFileTypeAssoc()
