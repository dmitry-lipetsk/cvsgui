from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
import os, os.path
import re, string

"""
  WinCvs Macro "Resurrect file"
  $Revision: 1.3 $

  written by Oliver Giesen, Sep 2002
  contact:
    email:  ogware@gmx.net
    jabber: ogiesen@jabber.org
    msn:    ogiesen@hotmail.com
    icq:    18777742

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

  - Select one CVS folders

  - Run the macro from the Macros|CVS menu

   ~a dialog will pop up asking you for the name of the file to be resurrected.

  - enter the file name and hit OK

   ~watch your file arise from the ashes... ;)

  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

class ResurrectFile( Macro):
  def __init__( self):
    Macro.__init__( self, 'Resurrect file', MACRO_SELECTION, 0, 'CVS')

  def OnCmdUI( self, cmdui):
    self.sel = App.GetSelection()
    enabled = len( self.sel) == 1 and not(  self.sel[0].IsFile() or self.sel[0].IsUnknown())
    cmdui.Enable( enabled)

  def determineBranch( self, dir):
    print 'determining branch... ',
    tagfn = os.path.join( dir, 'CVS', 'Tag')
    if os.path.exists( tagfn):
      f_tag = open( tagfn, 'r')
      try:
        tag = f_tag.readline().strip()
      finally:
        f_tag.close()
        
      if tag[0] == 'T':
        tag = tag[1:]
        print '\t'+tag
      else:
        print '\nDirectory is on a sticky non-branch tag!'
        tag = None
    else:
      print '\ton trunk'
      tag = 'HEAD'
    return tag

  def determineRevisions( self, filename, tag):
    print 'determining revisions...'
    ok = 0
    aliverev=''
    deadrev=''
    try:
      args = ['-N']
      if tag == 'HEAD':
        args += ['-b']
      else:
        args += ['-r'+tag]
      args += [filename]
      print 'cvs log '+ string.join( args, ' ')
      code, out, err = self.cvs.Run( 'log', *args)
      if code == 0:
        rex = re.compile( '^revision ([0-9\.]*)\n[^\n]*state: ([^;]*);', re.M)
        for revstate in rex.findall( out):
          if revstate[1] == 'dead':
            deadrev = revstate[0]
            print '\tdead: '+deadrev,
          elif aliverev == '' and deadrev != '':
            aliverev = revstate[0]
            print '\t%s: %s' % ( revstate[1], revstate[0])
            ok = 1
            break
        else:
          print 'No applicable revisions found!'
      else:
        print err
    finally:
      return ok, deadrev, aliverev

  def resurrectFile( self, filename, deadrev, aliverev):
    args = [ '-j'+deadrev, '-j'+aliverev, filename]
    print 'resurrecting revision %s of file "%s"' % ( aliverev, filename)
    print 'cvs up '+ string.join( args, ' ')
    code, out, err = self.cvs.Run( 'up', *args)
    if code == 0:
      print 'Done.'
    else:
      print err
    
  def Run( self):
    dir = self.sel[0].GetFullName()
    answer, filename = App.CvsPrompt('', 'Question', 'Enter the name of the file to be resurrected.', \
                                     alertTitle='Resurrect File')
    if answer=='IDOK':
      print 'Resurrection of ' + os.path.join( dir, filename)
      if not os.path.exists( os.path.join( dir, filename)):
        tag = self.determineBranch( dir)
        if tag:
          os.chdir( dir)
          self.cvs = Cvs( 1, 0)
          try:
            ok, deadrev, aliverev = self.determineRevisions( filename, tag)
            if ok:
              self.resurrectFile( filename, deadrev, aliverev)
          finally:
            del self.cvs
      else:
        print 'File already exists. Nothing to do.'

ResurrectFile()
