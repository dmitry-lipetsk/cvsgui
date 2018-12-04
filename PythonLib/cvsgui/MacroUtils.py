from cvsgui.CvsEntry import *
import os, os.path
import string

"""
  CvsGui Macro Library Module "MacroUtils"
  $Revision: 1.5 $

  written by Oliver Giesen, May 2003 - Apr 2004
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

  ======

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

def prepareTargets(entries, args, doChDir = 1):
  """ Given a CvsEntries list, calculates a common basedir
    and a corresponding list of targets. This is necessary
    in order to ensure correct behaviour when working on
    flat view selections.
    See macros cvs2cl.py or TagAndUpdate.py for examples of usage.
  """
  if len(entries) < 1: return
  if len(entries) > 1 or entries[0].IsFile():
    targets = []
    for entry in entries:
      targets.append(entry.GetFullName())

    basedir = os.path.dirname(os.path.commonprefix(targets))

    #re-attach the path-separator that os.path.dirname has stripped:
    if basedir[-1] != os.sep:
      basedir += os.sep;
    
    if not os.path.exists(basedir):
      raise Exception, 'Error determining common base dir for selected entries!\n[%s]'%basedir
    #chop basedir part of all targets:
    targets = map(lambda x: x[len(basedir):], targets)

    args += targets
  elif len(entries) == 1: #selection contains a single directory:
    basedir = entries[0].GetFullName()
  else: #empty selection:
    basedir = os.getcwd()
  
  if doChDir:
    os.chdir(basedir)
  else:
    return basedir

def purgeTree(path):
  """
    shutil.rmtree() cannot delete read-only files, so we need to do it
    manually by changing the attributes on each file to r/w before deleting.
  """
  try:
    names = os.listdir(path)
    for name in names:
      name = os.path.join(path, name)
      if os.path.isdir(name):
        purgeTree(name)
      else:
        os.chmod(name, 664)
        os.remove(name)
    os.rmdir(path)
  except:
    print 'Could not remove dir:', path
  return not os.path.exists(path)
