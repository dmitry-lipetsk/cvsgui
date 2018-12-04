from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *
import os, os.path, shutil
import string, time

"""
  WinCvs Macro "Copy To Branch..."
  $Revision: 1.4 $

  written by Oliver Giesen, Aug 2003 - Mar 2005
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    msn:    ogiesen@hotmail.com
    icq:    18777742
    y!:     ogiesen

  Feel free to modify or distribute in whichever way you like, 
   as long as it doesn't limit my personal rights to modify and
   redistribute this code.
   Apart from that the code is supplied "as-is", i.e. without warranty of any
   kind, either expressed or implied, regarding its quality or security.
   Have fun!

  ATTENTION:
   You will need at least WinCvs 1.3.5 to execute any Python macros
   from within WinCvs! This macro has last been tested against WinCvs 2.0.3
   and Python 2.4.1 .

  ======
  Usage:

  - Select one or more CVS-controlled files or folders.

  - Run the macro from the Macros|CVS menu.

  - Enter the name of the branch you want to copy the selection to or just
     hit OK  if you want to copy to HEAD.
     
  =============
  Known Issues / "Un-niceties":

  - When copying individual files that were added on a branch the automatic
     adding will not work as CVS will assume the tag of the directory which
     would be incorrect (this results in a "foo.txt added independently by a
     second party" error message).
     It's usually better to always use the macro on directory selections.

  - An update -P in a .cvsrc file could potentially mess up recreation of
     folders added on a branch, so I explicitly disable use of .cvsrc and set
     compression fixedly to -z7 (reasonable IMO). All other .cvsrc options
     will be lost.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

icon_xpm = \
  "13 12 5 1\n" + \
  ". c None\n" + \
  "r c #ff0000\n" + \
  "g c #00aa00\n" + \
  "b c #0000ff\n" + \
  "* c #808080\n" + \
  "bbbbb........\n" + \
  "b...brrrrrr..\n" + \
  "b...b.....r..\n" + \
  "bbbbggg...r..\n" + \
  "..b.gg....r..\n" + \
  "....g.g...r..\n" + \
  "..b...g.g.r..\n" + \
  ".......gg.r..\n" + \
  "..b...gggrrrr\n" + \
  "........r...r\n" + \
  "..b.....r...r\n" + \
  "........rrrrr"

class CopyToBranch(Macro):
  """
    CvsGui macro to copy any selection of sandbox files and folders to a
    specified branch or the trunk "as-is", i.e. without merging.
  """
  def __init__(self):
    Macro.__init__(self, 'Copy to branch/trunk...', MACRO_SELECTION, 0, 'CVS', icon_xpm)

  def OnCmdUI(self, cmdui):
    # allow only selections of cvs-controlled entries:
    self.sel = App.GetSelection()
    enabled = len(self.sel) > 0
    if enabled:
      for entry in self.sel:
        if entry.IsUnknown():
          enabled = 0
          break
    cmdui.Enable(enabled)

    
  def backupEntries(self, basedir, entries, tmpdir, kmodes):
    """
      Create a temporary copy of all CVS-controlled files in the selection.
      While doing so, record the keyword substitution modes in case we need
      to cvs add them later on (because they were initially added on a branch).
    """
    for entry in entries:
      if not entry.IsUnknown():
        if entry.IsFile():
          kmodes[entry.GetFullName()] = entry.GetKeyword()
          srcname = entry.GetFullName()
          relname = entry.GetPath()[len(basedir):]
          if (len(relname) > 0) and (relname[0] in '\/'):
            relname = relname[1:]
          targetname = os.path.join(tmpdir, relname)
          if not os.path.exists(targetname):
            os.makedirs(targetname)
          shutil.copy2(srcname, targetname)
        else:
          self.backupEntries(basedir, GetCvsEntriesList(entry.GetFullName()), tmpdir, kmodes)

  def cleanEntries(self, entries):
    """
      Recursively delete all selected files and folders.
    """
    for entry in entries:
      if not entry.IsUnknown():
        if entry.IsFile() and os.path.exists(entry.GetFullName()):
          os.chmod(entry.GetFullName(), 644)
          os.remove(entry.GetFullName())
        else:
          self.cleanEntries(GetCvsEntriesList(entry.GetFullName()))

  def restoreEntries(self, targetdir, srcdir):
    """
      Restore the temporary copy into the sandbox.
    """
    names = os.listdir(srcdir)
    if not os.path.exists(targetdir):
      os.makedirs(targetdir)
    for name in names:
      src = os.path.join(srcdir, name)
      dst = os.path.join(targetdir, name)
      if os.path.isdir(src):
        self.restoreEntries(dst, src)
      else:
        shutil.copy2(src, dst)

  def removeMissing(self, cvs, entries):
    """
      cvs remove all missing files within the selection as Missing means they
      were not present in the source sandbox.
    """
    for entry in entries:
      if not entry.IsUnknown():
        if entry.IsFile():
          if entry.IsMissing():
            os.chdir(entry.GetPath())
            code, out, err = cvs.Run('remove', entry.GetName())
        else:
          self.removeMissing(cvs, GetCvsEntriesList(entry.GetFullName()))

  def addUnknown(self, cvs, entries, kmodes, refreshEntries = 0):
    """
      Locate unknown files that used to be cvs-controlled in the source
      sandbox and cvs add them with the correct keyword substitution mode.
    """
    if refreshEntries: # update the Unknown states of the selection
      newentries = []
      for entry in entries:
        tmpEntries = GetCvsEntriesDict(entry.GetPath())
        assert tmpEntries.has_key(entry.GetName()), \
               '\tentry %s has not been restored!'%entry.GetFullName()
        newentries.append(tmpEntries[entry.GetName()])
      entries = newentries
      del newentries
        
    for entry in entries:
      fn = entry.GetFullName()
      if entry.IsFile():
        if entry.IsUnknown() and (fn in kmodes.keys()):
          os.chdir(entry.GetPath())
          args = []
          if kmodes[fn] <> '':
            args += [kmodes[fn]]
          args += [entry.GetName()]
          code, out, err = cvs.Run('add', *args)
          if code <> 0:
            print '\tCouldn''t add file %s . CVS Error: "%s"'  % (fn, err)
            print '\tNote: This macro cannot copy file selections that were\n'\
                 +'\tadded on a branch. Try running the macro again on the\n'\
                 +'\twhole directory.'
      else:
        if entry.IsUnknown():
          # todo: determine if added on branch and if so add directory
          print '\tunhandled unknown directory'
        else:
          self.addUnknown(cvs, GetCvsEntriesList(fn), kmodes)
    
  def detectModifieds(self, entries):
    """
      Recurse through the selection and determine whether there are any
      modified files in there. If there are we may not purge the backup 
      copy in case of a CVS error.
    """
    res = 0
    for entry in entries:
      if entry.IsFile():
        if entry.IsModified():
          res = 1
          break
      else:
        res = self.detectModifieds(GetCvsEntriesList(entry.GetFullName()))
        if res == 1:
          break
    return res
    
  def Run(self):
    code = -1
    answer, branch = App.CvsPrompt('', 'Question',
                                   'Enter the name of the branch to copy the ' \
                                  +'current selection to.', '(Leave blank to ' \
                                  +'copy to the trunk, aka HEAD)',
                                   alertTitle='Copy to branch/trunk')
    if answer=='IDOK':
      cwdbup = os.getcwd()
      try:
        cvs = Cvs(1,0)
        targets = []
        basedir = prepareTargets(self.sel, targets, 0)
        tmpdir = os.path.join(tempfile.gettempdir(), 'cvscopy'+str(time.time()))

        print 'Analyzing selection...'
        mayPurgeBup = not self.detectModifieds(self.sel)
        if not mayPurgeBup:
          print '\t(found modified files in selection)'
        
        print 'Backing up selection...'
        kmodes = {}
        self.backupEntries(basedir, self.sel, tmpdir, kmodes)
        
        if branch == '':
          branch = 'HEAD'
          arg = '-A'
        else:
          arg = '-r'+branch
          
        print 'Updating selection to %s...' % branch
        os.chdir(basedir)
        try:
          code, out, err = cvs.Run('-f', '-z7', 'up', arg, *targets)      
          if code == 0:
            print 'Preparing restore...'
            self.cleanEntries(self.sel)

            print 'Restoring selection...'
            self.restoreEntries(basedir, tmpdir)

            print 'Verifying removed files...'
            self.removeMissing(cvs, self.sel)

            print 'Verifying new files and folders...'
            self.addUnknown(cvs, self.sel, kmodes, 1)
          else:
            print 'CVS exited with code %d (error output follows)' % code
            cvs_err(err+'\n')
        except:
          code = 1
          raise
      finally:
        os.chdir(cwdbup)
        if tmpdir and os.path.exists(tmpdir):
          if code == 0 or mayPurgeBup:
            print 'Removing temp dir...'
            try:
              shutil.rmtree(tmpdir)
            except:
              purgeTree(tmpdir)
          else:
            cvs_err('\nAn error occurred (see above for messages if any).\n')
            cvs_err('As your selection contained modified files, the backup '\
                    'copy has not been removed. You can access it at the '\
                    'following location:\n\n\t%s\n\n' % tmpdir)
        if code == 0:
          print 'Done.'
            
CopyToBranch()      