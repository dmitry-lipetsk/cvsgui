import sys
import os, os.path
import re, string

try:
  from cvsgui.App import *
  from cvsgui.Cvs import *
  from cvsgui.CvsEntry import *
  from cvsgui.Macro import *
  from cvsgui.MacroUtils import *
  from cvsgui.Persistent import *
  from cvsgui.SafeTk import *
  asMacro = 1
except:
  asMacro = 0

"""
  CvsGui Macro and stand-alone script "Build ChangeLog"
  $Revision: 1.23 $

  written by Oliver Giesen, Oct 2002 - Jul 2005
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
   from within WinCvs! This macro has originally been written against
   WinCvs 1.3.8 and was last tested with WinCvs 2.0.3 and Python 2.4.1 .

  ======
  Usage (as CvsGui macro "Build ChangeLog"):

  - Select one or more CVS-folders and/or files

  - Run the macro from the Macros|CVS menu

   ~the ChangeLog file should get created

  ------   
  Usage (as CvsGui macro "Build ChangeLog (advanced)..."):

  - Select one or more CVS-folders and/or files

  - Run the macro from the Macros|CVS menu

  - Enter the filename of the logfile you want to generate/update
    (will be created inside the current directory - defaults to "ChangeLog")

  - Enter optional additional arguments to be passed to the log command

   ~the log file should get created

  ------
  Usage (as CvsGui admin macro "Build ChangeLog for module..."):

  - Run the macro from the Admin|Admin macros|Build ChangeLog menu

  - Enter or confirm the CVSROOT of the repository to log
    (defaults to CVSROOT of current directory if applicable)

  - Enter or confirm the name of the module to log
    (defaults to module corresponding to current directory if aplicable)

  - Enter or confirm the file name of the log file you want to create/update
    (defaults to [module].log if applicable otherwise [cwd].log)

  - Enter optional additional arguments to be passed to the log command

   ~the log file should get created

  ------
  Usage (as CvsGui admin macro "Build ChangeLog from log dump file..."):

  - Optionally select the dump file in WinCvs' file view.
  
  - Run the macro from the Admin|Admin macros|Build ChangeLog menu

  - Enter or confirm the file name of the log dump file
    (defaults to the currently selected file)

  - Enter or confirm the file name of the log file you want to create/update

   ~the log file should get created
    
  ======
  Usage (as stand-alone script):

    cvs2cl.py [[[basedir] filename] arg1] [arg2]...

  basedir	the directory for which the ChangeLog should be generated
  			(also where the logfile will be saved to)
  			optional - defaults to the current directory

  filename	the name of the logfile to be generated/updated
  			optional - defaults to "ChangeLog"

  arg1..n	optional additional arguments to be passed to the log command

  Example:
    cvs2cl.py d:\Dev\cvsgui MyChangeLog -w
      retrieves the log of my changes to the cvsgui project
       into d:\Dev\cvsgui\MyChangeLog

  ------
  or for modules-based ChangeLog:

    cvs2cl.py -r cvsroot module filename [arg1] [arg2]...

  cvsroot	the CVSROOT of the repository you want to log

  module	the name of the module you want to log

  filename	the name of the logfile to be generated/updated

  arg1..n	optional additional arguments to be passed to the rlog command

  ------
  or for dump-file-based ChangeLog:

    cvs2cl.py -f dumpfilename filename

  dumpfilename  the fullly qualified path to the dump file which the log should be built from

  filename  the name of the logfile to be generated/updated  

  =============
  Known Issues / "Un-niceties":

  - The log-by-module currently displays the complete path of the RCS file
    which includes the path section from the CVSROOT. Truncating that part
    based on the known CVSROOT unfortunately doesn't always work, 
    e.g. Sourceforge appears to use symbolic links for publishing CVSROOTs
    while the actual path returned by cvs rlog is much longer.
    Have to think some more about this one...

  - There is no commandline equivalent of the "Build ChangeLog from Dumpfile
    macro yet.  

  Please report any problems you may encounter or suggestions you might have
  to ogware@gmx.net .
    
"""

# Change this to 0 if you want empty revision comments to be included:
skipEmptyComments = 1
defaultLogName    = 'ChangeLog'
defaultOmitAuthor = 0
defaultOmitFiles  = 0
defaultExtArgs    = ''

def runCvs(args):
  if asMacro:
    cvs = Cvs(1,0)
    return cvs.Run(*args)
  else:
    f_in, f_out, f_err = os.popen3('cvs '+string.join(args))
    out = f_out.read()
    err = f_err.read()
    f_out.close()
    f_err.close()
    code = f_in.close()
    if not code: code = 0
    return code, out, err

def wrapLine(line, wrapmargin=64):
  isBreakChar = lambda idx: not line[idx] in string.letters + string.digits + '@_.'
  result = []
  while len(line) > wrapmargin:
    for i in range(wrapmargin, 1, -1):
      if isBreakChar(i):
        result.append(line[:i+1])
        line = line[i+1:]
        break
    else:
      for i in range(wrapmargin+1, len(line)-1):
        if isBreakChar(i):
          result.append(line[:i+2])
          line = line[i+2:]
          break
      else:
        result.append(line)
        line = ''
    
  result.append(line)
  return result


class ChangeLogger:
  def __init__(self, basedir = None, targets = [], logname = defaultLogName, \
               command = 'log', cvsroot = None, clOptions = None):
    try:
      filename, oldlog, lastDate = self.prepareLogFile(basedir, logname)
      clName = filename[len(basedir)+1:]
      code, logdump = self.getLog(basedir, lastDate, targets, command, cvsroot)
      if code <> 0:
        print 'CVS returned Exit Code %d. Aborting ChangeLog creation.' % code
        print 'Original CVS Error Output follows (if any)\n'
        if(logdump):
          sys.stderr.write(logdump+'\n')
      else:
        entries = ChangeLogParser(clName, command, clOptions).parseLog(logdump)
        ChangeLogWriter().writeLog(entries, filename, oldlog, clOptions)
        print 'Done.'
    except AssertionError, detail:
      sys.stderr.write(detail+'\n')

  def prepareLogFile( self, basedir, logname):
    filename = os.path.join( basedir, logname)
    lastDate = None
    logfile = []
    print
    if os.path.exists( filename):
      print 'Analyzing existing %s...' % logname
      clFile = open( filename, 'r')
      try:
        oldlog = clFile.readlines()
        while (len(oldlog) > 0) and (oldlog[0] == '\n'):
          del oldlog[0]
        assert (len(oldlog)>0) and(len(oldlog[0])>=10), \
               'Existing %s has unknown format and could not be rewritten!' % logname
        lastDate = oldlog[0][:10]
        #print lastDate
        rx = re.compile( '^[0-9]{4}[\-\/][0-9]{2}[\-\/][0-9]{2}')
        assert rx.match(lastDate), 'Existing %s has unknown format and could not be rewritten!' % logname
        i = 0
        for line in oldlog[1:]:
          m = rx.match(line)
          if m and m.group()<>lastDate:
            break
          else:
            i+=1
        logfile += oldlog[i:]
        #print string.join( logfile[:25], '')
      finally:
        clFile.close()
    return filename, logfile, lastDate
        
  def getLog( self, basedir, lastDate, targets, command = 'log', cvsroot = None):
    print 'Downloading the log',
    if lastDate:
      print 'beginning %s ' % lastDate,
    print '...'
    cwdbup = os.getcwd()
    try:
      os.chdir(basedir)
      args = ['-Q', '-z9', command]
      if cvsroot:
        args.insert(0, '-d'+cvsroot)
      if lastDate:
        fstr = '-d%s<'
        if not asMacro:
          fstr = '"%s"'%fstr
        args += [fstr%lastDate]
      args += targets
      #print 'cvs', string.join( args), '\t(in %s)'%os.getcwd()
      code, out, err = runCvs(args)
    finally:
      os.chdir(cwdbup)
    if (code == 0) and (out):
      return code, out
    else:
      return code, err
      

class ChangeLogParser:
  def __init__(self, clName, command = 'log', options = None):
    self.clName = clName
    self.rlog = command == 'rlog'
    if options:
      self.skipEmpty = options.skipEmpty
    else:
      self.skipEmpty = skipEmptyComments

  def parseRevision( self, text, filename, branchnames, entries):
    if text == '': return
    lines = text.splitlines()
    if len(lines) <= 2: return #skip totally empty revisions
    m = re.match('^revision ([0-9\.]+)', lines[0])
    assert m, 'Malformed revision info for file "%s"!\n"%s"' % (filename, text)
    revno = m.group(1)
    m = re.match( '^date: ([^ ]+) ([^; ]+).*author: ([^;]+)', lines[1])
    assert m, 'Unknown revision detail format for file "%s", rev. $s! ("%s")' % (filename, revno, lines[1])
    date = string.replace(m.group(1), '/', '-')
    time = m.group(2)
    author = m.group(3)
    #skip "branches" line, if any:
    txtidx = 2
    if lines[txtidx][:9] == 'branches:':
      txtidx+=1
    revtext = string.join( lines[txtidx:], '\n')
    if self.skipEmpty and (revtext == 'Initial revision' or revtext == 'no message'):
      return
    
    del lines, txtidx
    if not entries.has_key(date):
      entries[date]={}
    if not entries[date].has_key(author):
      entries[date][author] = {}
    if not entries[date][author].has_key(revtext):
      entries[date][author][revtext] = []

    if len(entries[date][author][revtext]) <= 0:
      entries[date][author][revtext].append(time)
    elif entries[date][author][revtext][0] < time:
      entries[date][author][revtext][0] = time

    rootrev = re.search('(.*)\.[0-9]+$', revno).group(1)
    if branchnames.has_key(rootrev):
      branch = '[%s] '%branchnames[rootrev]
    else:
      branch = ''
    entries[date][author][revtext].append( '%s %s%s'%(filename, branch, revno))
    
  def parseFile( self, text, entries):
    #don't even scan files without selected revisions:
    m = re.search('selected revisions: ([0-9]+)', text, re.MULTILINE)
    if m and m.group(1) == '0':
      return
    #parse header:
    filename = None
    branchnames= {}
    section = 0
    idx = 0
    lines = text.splitlines()
    for line in lines:
      if section == 0:
        if not self.rlog:
          m = re.match('^Working file: ([^,]+)', line)
          if m:
            filename = m.group(1)
            #don't log changes to ourselves:
            if filename == self.clName:
              return
            section+=1
        else:
          m = re.match('^RCS file: [\/]?(([^\/,]+?\/)*([^\/,]+)),v', line)
          if m:
            #filename = m.group(m.lastindex)
            #if filename == self.clName:
            #  return
            filename = m.group(1)
            section+=1
      elif section == 1:
        if line[:15] == 'symbolic names:':
          section+=1
      elif section == 2:
        if line[0] == '\t':
          m = re.search('^\t([^:]+): ([0-9\.]+)\.0(\.[0-9]+)', line)
          if m:
            branchnames[m.group(2)+m.group(3)] = m.group(1)
        else:
          section+=1
      else:
        if line == '-'*28:
          break
      idx+=1
    else:
      return

    del section
    #iterate revisions:
    rex = re.compile('^-{28}\n', re.MULTILINE)
    revisions = rex.split(string.join(lines[idx:], '\n'))
    del lines, idx
    for revision in revisions:
      self.parseRevision(revision, filename, branchnames, entries)
    
  def parseLog(self, logdump):
    print 'Parsing log output...'
    logentries = {}
    if logdump:
      rex = re.compile('^={77}$', re.MULTILINE)
      files = rex.split(logdump)
      #print '(%d files,'%len(files),
      for file in files:
        self.parseFile(file, logentries)
      #print '%d entries)'%len(logentries)
    else:
      sys.stderr.write('Cannot parse empty log.\n')
    return logentries

class ChangeLogWriter:
  def writeLogMsg(self, msg, file):
    indent = 2
    if string.count(msg, '\n') == 0:
      wrapAt = 64
    else:
      wrapAt = 100
    for line in msg.splitlines():
      if len(line) > wrapAt:
        for wrappedline in wrapLine(line, wrapAt):
          file.write('\t'*indent + wrappedline + '\n')
      else:
        file.write('\t'*indent + line + '\n')
        
  def writeLog(self, entries, filename, oldlog = [], options = None):
    if len(entries) > 0:
      if options:
        omitAuthor = options.omitAuthor
        omitFiles = options.omitFiles
      else:
        omitAuthor = defaultOmitAuthor
        omitFiles = defaultOmitFiles
      print 'Sorting entries...'
      dates = entries.keys()
      dates.sort()
      dates.reverse()
      print 'Rewriting %s...' % os.path.basename(filename)
      clFile = open( filename, 'w')
      try:
        pos = 0
        for date in dates:
          if omitAuthor:
            clFile.write('%s\n' % date)
          authors = entries[date]
          for author, logs in authors.items():
            if not omitAuthor:
              clFile.write( '%s\t%s\n'%(date, author))
            times = []
            orderedlogs = {}
            for log, files in logs.items():
              time = str(files)
              del files[0]
              times.append(time)
              orderedlogs[time] = (log, files)
            times.sort()
            times.reverse()
            for time in times:
              log, files = orderedlogs[time]
              files.sort()
              if not omitFiles:
                for file in files:
                  clFile.write( '\t* %s:\n'%file)
              self.writeLogMsg( log, clFile)
              pos = clFile.tell()
              clFile.write('\n')
        clFile.seek(pos)
        clFile.writelines( oldlog)
      finally:
        clFile.close()
    else:
      sys.stderr.write('Nothing to do.\n');


class DumpLogger:
  def __init__(self, dumpfile, logname):
    if os.path.exists(dumpfile):
      print 'Reading ', dumpfile
      f = open(dumpfile, 'r')
      try:
        logdump = f.read()
      finally:
        f.close()
      print 'Analyzing', dumpfile, 
      if re.search('^Working file: ([^,]+)', logdump, re.MULTILINE):
        cmd = 'log'
      else:
        cmd = 'rlog'
      print '(identified as output from cvs %s)' % cmd
      entries = ChangeLogParser('', cmd).parseLog(logdump)
      ChangeLogWriter().writeLog(entries, logname)
      print 'Done.'
    else:
      sys.stderr.write('"%s" does not exist.\n')
      
if asMacro:
  class BuildChangeLogFromDump(Macro):
    def __init__(self):
      Macro.__init__(self, 'Build ChangeLog from log dump file...', MACRO_ADMIN, 0, 'ChangeLog')

    def Run(self):
      try:
        sel = App.GetSelection()
        if len(sel) > 0:
          dumpfile = sel[0].GetFullName()
        else:
          dumpfile = ''
        answer, dumpfile = App.CvsPrompt(dumpfile, 'Question', 'Enter path and name of log dump file', \
                                         alertTitle='Build ChangeLog')
        if answer == 'IDOK':
          if os.path.exists(dumpfile):
            d,n = os.path.split(dumpfile)
            n,e = os.path.splitext(n)
            answer, logname = App.CvsPrompt(os.path.join(d, '%s_ChangeLog%s' % (n, e)), \
                                            'Question', 'Enter full name of output file.', \
                                            '(will be overwritten if it exists already!)',\
                                            alertTitle='Build ChangeLog')
            if answer == 'IDOK':
              DumpLogger(dumpfile, logname)
          else:
            sys.stderr.write('"%s" does not exist.\n')
      except Exception, detail:
        sys.stderr.write(str(detail)+'\n')
          
  
  class BuildChangeLog(Macro):
    def __init__(self):
      Macro.__init__(self, 'Standard ChangeLog (default settings)', MACRO_SELECTION, 0, 'Build ChangeLog')

    def OnCmdUI(self, cmdui):
      self.sel = App.GetSelection()
      enabled = len(self.sel) > 0
      if enabled:
        for entry in self.sel:
          if entry.IsUnknown():
            enabled = 0
            break
      cmdui.Enable(enabled)
    
    def Run(self):
      targets = []
      basedir = prepareTargets(self.sel, targets, 0)
      ChangeLogger(basedir, defaultExtArgs.split() + targets)
   
  class BuildChangeLogEx(BuildChangeLog):
    def __init__(self):
      Macro.__init__(self, 'Custom ChangeLog...', MACRO_SELECTION, 0, 'Build ChangeLog')

    def Run(self):
      args = []
      basedir = prepareTargets(self.sel, args, 0)
      lognameVal = Persistent('PY_ChangelogLogName', defaultLogName, 1)
      answer, logname = App.CvsPrompt(str(lognameVal), 'Question', 'Enter Logfile name.', \
                                      '(will be created in "%s")' % basedir,\
                                      alertTitle='Build ChangeLog')
      if answer == 'IDOK':
        extargsVal = Persistent('PY_ChangelogExtArgs', defaultExtArgs, 1)
        answer, extargs = App.CvsPrompt(str(extargsVal), 'Question', 'Extended log options', \
                                        'Examples:\n'\
                                       +'-w\n\tonly lists own changes\n\n'\
                                       +'-rBRANCH,FROMTAG:TOTAG\n\tlists changes between tags on branch\n\n'\
                                       +'-d2003-01-01<2003-12-31\n\tall changes of 2003\n',\
                                        alertTitle='Build ChangeLog')
        if answer == 'IDOK':
          if len(logname) > 0:
            lognameVal << logname
          else:
            sys.stderr.write('Must specify a file name!\n')
            return
          extargsVal << extargs
          ChangeLogger(basedir, extargs.split() + args, logname)

  class BuildModuleLog(Macro):
    def __init__(self):
      Macro.__init__(self, 'Build ChangeLog for module...', MACRO_ADMIN, 0, 'ChangeLog')

    def readFile(self, file):
      if os.path.exists(file):
        f = open(file, 'r')
        try:
          line = string.strip(f.readline())
          return line
        finally:
          f.close()
      else:
        return ''
      
    def Run(self):
      dlgTitle = 'Build ChangeLog'
      basedir = os.getcwd()
      croot = self.readFile(os.path.join(basedir, 'CVS', 'Root'))
      answer, cvsroot = App.CvsPrompt(croot, 'Question', 'Enter CVSROOT.', \
                                      alertTitle=dlgTitle)
      if answer == 'IDOK':
        if croot == cvsroot:
          module = self.readFile(os.path.join(basedir, 'CVS', 'Repository'))
        else:
          module = os.path.basename(basedir)
        answer, module = App.CvsPrompt(module, 'Question', 'Enter module to log.', \
                                       alertTitle=dlgTitle)
        if answer == 'IDOK':
          answer, filename = App.CvsPrompt(os.path.basename(module)+'.log', \
                                           'Question', 'Enter logfile name.', \
                                           '(will be created in "%s")' % basedir,\
                                           alertTitle=dlgTitle)
          if answer == 'IDOK':
            extargsVal = Persistent('PY_ChangeRlogExtArgs', defaultExtArgs, 0)
            answer, extargs = App.CvsPrompt(str(extargsVal), 'Question', \
                                            'Enter extended log options (optional)', \
                                            'Examples:\n'\
                                            '-w\n\tonly lists own changes\n\n'\
                                            '-rBRANCH,FROMTAG:TOTAG\n\tlists changes between tags on branch\n\n'\
                                            '-d2004-01-01<2003-12-31\n\tall changes of 2004\n',\
                                            alertTitle=dlgTitle)
            if answer == 'IDOK':
              extargsVal << extargs
              args = [module]
              ChangeLogger(basedir, string.split(extargs+' ') + args, filename, \
                           'rlog', cvsroot)

          
  if not globals().has_key('clMacrosLoaded'):
    BuildChangeLog()
    if not checkTk():
      BuildChangeLogEx()
    BuildModuleLog()
    BuildChangeLogFromDump()
    clMacrosLoaded = 1
else:
  try:
    logname = defaultLogName
    args = string.split(defaultExtArgs)
    argc = len(sys.argv)
    basedir = os.getcwd()
    cvsroot = None
    dumpfile = None
    if argc > 1:
      if sys.argv[1] == '-r':
        basedir = os.getcwd()
        assert argc >= 5, 'Must specify CVSROOT, module and filename in remote mode!'
        cvsroot = sys.argv[2]
        logname = sys.argv[4]
        if argc > 5:
          args += sys.argv[5:]
        args += [sys.argv[3]]
      elif sys.argv[1] == '-f':
        dumpfile = sys.argv[2]
        logname = sys.argv[3]
      else:
        basedir = sys.argv[1]
        if argc > 2:
          logname = sys.argv[2]
          if argc > 3:
            args += sys.argv[3:]
  
    if cvsroot:
      ChangeLogger(basedir, args, logname, 'rlog', cvsroot)
    elif dumpfile:
      DumpLogger(dumpfile, logname)
    else:
      ChangeLogger(basedir, args, logname)
  except Exception, detail:
    sys.stderr.write(str(detail)+'\n')
        