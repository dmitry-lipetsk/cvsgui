from cvsgui.App import *
from cvsgui.Cvs import *
from cvsgui.CvsEntry import *
from cvsgui.Macro import *
from cvsgui.MacroUtils import *
from cvsgui.Persistent import *
from cvsgui.ColorConsole import *

"""
  WinCvs Macro "Locate in Log comments"
  $Revision: 1.3 $

  written by Oliver Giesen, May 2005 - Jul 2005
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
   from within WinCvs! This macro has last been tested against WinCvs 2.0.3 .

  ======
  Usage:

  - Select one or more CVS-controlled files and/or folders

  - Run the macro via Macros|Search|Locate in log comments...

  =============
  Known Issues / "Un-niceties":

  - None so far.

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .

"""


# Number of characters to display before and after match:
matchPadding = 120

class SearchLogComments(Macro):
  def __init__(self):
    Macro.__init__(self, 'Locate in Log comments...', MACRO_SELECTION, 0, 'Search')

  def OnCmdUI(self, cmdui):
    """ Allows only selections of cvs-controlled entries """
    sel = App.GetSelection()
    enabled = len(sel) > 0
    if enabled:
      for entry in sel:
        if entry.IsUnknown():
          enabled = 0
          break
    cmdui.Enable(enabled)

  def processMatches(self, m, revtext, cconsole):
    global matchPadding
    cconsole << kItalic
    pos = 0
    while m:
      # print trailing padding of previous match:
      if pos > 0 and m.start() - pos > matchPadding:
        cconsole << revtext[max(0,pos-1):pos+matchPadding-1] << '...\n'

      # print leading padding:
      pos2 = max(pos, m.start() - matchPadding)
      if pos2 > pos:
        cconsole << '\n...'
      if m.start() > pos2:
        cconsole << revtext[max(0,pos2-1):m.start()]

      # print actual match:          :
      cconsole << kBold << kGreen << revtext[m.start():m.end()] << kNormal << kItalic
      
      pos = m.end() + 1
      m = self.searchRX.search(revtext, pos)
      if not m:
        # no more matches, print last trailing padding
        pos2 = min(len(revtext), pos + matchPadding - 1)
        cconsole << revtext[pos-1:pos2]
        if pos2 < len(revtext):
          cconsole << '...\n'
          

  def searchRev(self, revdump, filename, search, cconsole):
    # process revision header:
    if revdump == '': return
    lines = revdump.splitlines()
    if len(lines) <= 2: return
    m = self.revnoRX.match(lines[0])
    revno = m.group(1)
    #print '\t%s' % revno
    m = self.revdetailRX.match(lines[1])
    date = string.replace(m.group(1), '/', '-')
    author = m.group(2)
    txtidx = 2
    if lines[txtidx][:9] == 'branches:':
      txtidx += 1
    revtext = string.join(lines[txtidx:], '\n')
    del lines, txtidx

    # start the actual search:    
    m = self.searchRX.search(revtext)
    if m:
      # output file match header if this is the first match for this file:
      if filename <> self.lastfile:
        cconsole << kBold << '%s ' % filename << kNormal
        cconsole << '='*(76-len(filename)) << '\n'
        self.lastfile = filename

      # output revision match header:
      cconsole << kBold << kRed << '\n%s' % revno << kNormal
      cconsole << kBlue << '\t%s\t(by %s)\n' % (date, author) << kNormal

      self.processMatches(m, revtext, cconsole)      
      cconsole << '\n\n' << kNormal                    
      

  def searchFile(self, filedump, search, cconsole):
    if len(filedump) < 28: return
    #print 'searching',
    filename = None
    idx = 0
    lines = filedump.splitlines()
    for line in lines:
      if not filename:
        m = self.filenameRX.match(line)
        if m:
          filename = m.group(1)
          #print '%s' % filename,
      elif line == '-' * 28:
        break
      idx += 1
    else:
     # print '???'
      return

    revs = string.split(string.join(lines[idx:], '\n'), '-'*28+'\n')
    del lines, idx
    #print '(%d revs)...' % (len(revs) - 1)
    for rev in revs:
      self.searchRev(rev, filename, search, cconsole)

  def searchLog(self, logdump, search, cconsole):
    self.lastfile = ''
    self.filenameRX = re.compile('^Working file: ([^,]+)')
    self.revnoRX = re.compile('^revision ([0-9\.]+)')
    self.revdetailRX = re.compile('^date: ([^ ]+).*author: ([^;]+)')
    self.searchRX = re.compile(search, re.MULTILINE + re.IGNORECASE)

    files = string.split(logdump, '='*77)
    print 'Searching %d files...' % (len(files) - 1)
    for file in files:
      self.searchFile(file, search, cconsole)

  def Run(self):
    """ Runs a CVS command on all selected items """
    sel = App.GetSelection()
    searchVal = Persistent('PY_LogCommentSearchExpr', '', 1)
    answer, search = App.CvsPrompt(str(searchVal), 'Question', 'Enter search term.', \
                                   '(multi-line, case-insensitive regular expression)', \
                                   alertTitle='Locate in Log comments')
    if answer != 'IDOK': return
    searchVal << search    

    cwdbup = os.getcwd()
    try:
      cvs = Cvs(1,0)
      targets = []
      prepareTargets(sel, targets)
      print 'Fetching log...'
      code, out, err = cvs.Run('log', *targets)
      if code == 0:
        self.searchLog(out, search, ColorConsole())
        print '\nDone.'
      else:
        cvs_err(err)
    finally:
      os.chdir(cwdbup)

SearchLogComments()