import os
from cvsgui.Macro import *
from cvsgui.Persistent import *

"""
  CvsGui Macro Library Module "SafeTk"
  $Revision: 1.6 $
  
  written by Oliver Giesen, Aug 2002 - Jun 2005
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    icq:    18777742

  Feel free to modify or distribute in whichever way you like,
   as long as it doesn't limit my personal rights to modify and
   redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  When using Tkinter, the module FixTk is imported which creates
   an environment variable TCL_LIBRARY and points it at the TCL
   libraries within the Python installation.
   This has the unfortunate effect that TCL macros running from a
   separate installation of TCL will now try to locate packages
   within the Python directories instead of the TCL ones, which
   usually fails because the directories within the Python
   installation are named differently.
  This module employs a fix to this by removing said environment
   variable and only temporarily enabling it for the lifetime of
   a Tkinter-based Tk instance.
   
  Additionally, there's a set of base classes that could be used
   to faciliate the creation of Tkinter-based macros.
"""

try:
  from Tkinter import *
except:
  NoTk = 1
  class Frame:
    pass
else:
  NoTk = -1  # indicates uninitialized

  TCL_LIB = 'TCL_LIBRARY'

  if os.environ.has_key(TCL_LIB):
    tcllib = os.environ[TCL_LIB]
  else:
    tcllib = ''

  def prepTk():
    """ ensure that TCL_LIBRARY points to the path determined in the FixTk module
    """
    global tcllib
    if not os.environ.has_key(TCL_LIB):
      os.environ[TCL_LIB] = tcllib

  def resetTk():
    """ delete the TCL_LIBRARY environment variable, so TCL has to go searching for
      extension packages on its own instead of depending on the Python search path.
      Make a backup of the value determined in the FixTk module so we could write it
      back again later.
    """
    global tcllib
    if os.environ.has_key(TCL_LIB):
      if tcllib == '':
        tcllib = os.environ[TCL_LIB]
      del os.environ[TCL_LIB]
    #print 'reset TCL path'

  class SafeTkRoot( Tk):
    """ A replacement for the Tk class which ensures the TCL_LIBRARY variable lives
      only as long as the Tk-instance does.
    """
    def __init__( self, obj):
      prepTk()
      Tk.__init__( self, baseName=str(obj))

    def destroy( self):
      #print 'destroying SafeTkRoot instance'
      Tk.destroy( self)
      resetTk()
      
    def __del__( self):
      #print 'finalizing SafeTkRoot instance'
      resetTk()

def checkTk():
  """ Checks whether Tk could be invoked without exceptions. The check is only
    performed once. Later calls simply return the previous result.
  """
  global NoTk
  try:
    if NoTk < 0:
      try:
        testtk = SafeTkRoot( 'justChecking')
        testtk.destroy()
        del testtk
        NoTk = 0
        print 'Tk is available, Tk-macros are enabled'
      except:
        NoTk = 1
        #raise
  finally:
    return NoTk == 0
    #if NoTk: raise



_true = (1 == 1) # for Python 2.1 compatibility
_false = not (_true)

class PersistentVar(Persistent):
  def __init__(self, token, defvalue, addToSettings = 1):
    Persistent.__init__(self, token, defvalue, addToSettings)
    self.restore()

  def getTypedValue(self):
    return self.GetValue()
  
  def store(self):
    self.SetValue(str(self.get()))

  def restore(self):
    self.set(self.getTypedValue())

class PersistentStringVar(StringVar, PersistentVar):
  def __init__(self, token, defvalue, addToSettings = 1, master = None):
    StringVar.__init__(self, master)
    PersistentVar.__init__(self, token, defvalue, addToSettings)

class PersistentBoolVar(BooleanVar, PersistentVar):
  def __init__(self, token, defvalue, addToSettings = 1, master = None):
    BooleanVar.__init__(self, master)
    PersistentVar.__init__(self, token, defvalue, addToSettings)

  def getTypedValue(self):
    return self.GetValue() == str(1 == 1)

class PersistentVars:
  def storeAll(self):
    for itemname in dir(self):
      attr = getattr(self, itemname)
      try:
        if isinstance(attr, PersistentVar):
          attr.store()
      except:
        pass

  def flatten(self):
    for itemname in dir(self):
      attr = getattr(self, itemname)
      try:
        if isinstance(attr, PersistentVar):
          setattr(self, itemname[1:], attr.getTypedValue())
          delattr(self, itemname)
      except:
        pass

class TkMacroDlg(Frame):
  def __init__(self, master, caption, doItProc, options = None):
    Frame.__init__(self, master)
    self.master.title(caption)
    self.doItProc = doItProc
    self.options = options
    
    self.createWidgets(999)
    self.pack(fill=BOTH, expand=1)
    self.update()
    w = self.winfo_width()
    h = self.winfo_height()
    x = (self.winfo_screenwidth() - w) / 2
    y = (self.winfo_screenheight() - h) / 2
    self.master.geometry(newGeometry = '%dx%d+%d+%d' % (w, h, x, y))

  def createWidgets(self, buttonrow=999):
    Button(self, text='OK', command=self.doIt)\
          .grid(row=buttonrow, column=0, padx=5, pady=5, sticky=W+E+N)
    Button(self, text='Cancel', command=self.master.destroy)\
          .grid(row=buttonrow, column=1, padx=5, pady=5, sticky=W+E+N)

  def prepareOptions(self, options):
    pass
  
  def doIt(self):
    self['cursor'] = 'watch'
    try:
      self.prepareOptions(self.options)
      self.doItProc(self.options)
    finally:
      self['cursor'] = 'arrow'
      self.master.destroy()

class TkMacro(Macro):
  def __init__(self, title, group, isCheck=0, subgroup="", dlgclass=TkMacroDlg, xpmicon=""):
    Macro.__init__(self, title, group, isCheck, subgroup, xpmicon)
    self.dlgclass = dlgclass
    
  def Run(self):
    try:
      tk = SafeTkRoot(self)
      options = self.initOptions()
      dlg = self.dlgclass(tk, self.doStuff, options)
      tk.wait_window(dlg)
    except:
      if tk:
        tk.destroy()
      raise
  
