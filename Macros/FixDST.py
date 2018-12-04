from cvsgui.Macro import *
from cvsgui.CvsEntry import *
from cvsgui.ColorConsole import *
import cvsgui.App, os.path
import re, time, tempfile

# debugging = 1 for useful traces
# debugging = 2 to disable modifying CVS/Entries.
_debugging = 1

class FixDST(Macro):
    __doc__ = """Go through selection and look for modified files that may be due to
the infamous DST (Daylight Savings Time) bug. If we find them, fix them.

The criterion for candidacy is that there is almost exactly an hour between the
modification time listed in CVS/Entries and that provided by the file system.
There may be a few seconds' slop due to the different way NTFS and FAT deal with
file modification times, so we use a little slop in our calculations.

If candidate files are found, we edit the CVS/Entries file to set the modification
time in CVS/Entries to the modification time of the actual file in the sandbox."""
    def __init__(self):
        Macro.__init__(self, "Fix red files", MACRO_SELECTION,
            0, "Search")
        self._slop = 2.   # slop in seconds for comparing modification times

    def _strptime(self, str):
        __doc__ = """Not really like the UNIX strptime. This one parses only the date strings
used in CVS/Entries files.

Parses the date/time string, returns time_t (time in seconds since beginning of epoch).
Returns -1 for error conditions."""
        # print '_strptime("', str, '"'        
        Months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
        Days = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
        try:
            ls = str.split()
            lt = ls[3].split(':')
            month = Months.index(ls[1])
            day = Days.index(ls[0])
            # print '\t', ls
            # print '\t', lt
            timetuple = (int(ls[-1]), month + 1, int(ls[2]), int(lt[0]), int(lt[1]), int(lt[2]), day, 0, 0)
            t = time.mktime(timetuple) - time.timezone
        except:
            t = -1.0    # Error conditions...
        return t

    def FixEntries(self, path, list):
        __doc__ = """Fix CVS/Entries file for parent directory path. For each entry in list
(an entry is a two-tuple (cvs-entry, mod-time)), find the matching entry in path/CVS/Entries
and edit the modification time to be mod_time."""
        d = {}
        console = ColorConsole()
        for ent in list:
            d[ent[0].GetName()] = ent
        efname = path + '/CVS/Entries'
        push_tempdir = tempfile.gettempdir()
        tempfile.tempdir = path + '/CVS'
        outfname = tempfile.mktemp('.tmp')
        tempfile.tempdir = push_tempdir
        f_Entries = open(efname, 'r')
        f_out = open(outfname, 'w')
        r = re.compile('^/(?P<fname>[^/]*)/(?P<rev>[^/]*)/(?P<date>[^/]*)/(?P<rest>.*)$')
        for line in f_Entries.xreadlines():
            m = r.match(line)
            if m is not None:
                md = m.groupdict()
                if d.has_key(md['fname']):
                    ent, modtime = d[md['fname']]
                    ename = ent.GetName()
                    mname = md['fname']
                    edate = ent.GetUTC()
                    mdate = md['date']
                    if ename != mname:
                        console << kRed << ('Internal error: "%s" does not match "%s".' % (ename, mname)) << kNormal << '\n'
                    elif edate != mdate:
                        console << kRed << ('Internal error: "%s" does not match "%s".' % (edate, mdate)) << kNormal << '\n'
                    else:
                        if _debugging:
                            oldline = line
                        line = '/' + md['fname'] + '/' + md['rev'] + '/' + modtime + '/' + md['rest'] + '\n'
                        if _debugging:
                            console << kMagenta << 'Changing' << kRed << '\n\t' << oldline \
                                    << kMagenta << 'to' << kBlue << '\n\t' << line
                        
            f_out.write(line)
        f_Entries.close()
        f_out.close()
        if _debugging >= 2:
            os.rename(outfname, efname + '.new')
        else:
            # up to 20 backup files...
            # circular pool. If there are 20 files
            # of form Entries.000, .001, etc.
            # delete the oldest one and use that
            # for the current backup.
            xfbase = os.path.splitext(efname)[0]
            token = None
            for i in range(20):
                xfname = xfbase + '.%03d' % i
                # does the candidate name exist?
                if os.access(xfname, os.F_OK):
                    mtime = os.stat(xfname)[8]
                    if token is None or mtime < token[1]:
                        token = (xfname, mtime)
                else:
                    token = (xfname, 0)
                    break
            if os.access(token[0], os.F_OK):
                os.unlink(token[0])
            if _debugging:
                print 'Renaming "%s" --> "%s".' % (efname, token[0])
            os.rename(efname, token[0])
            if _debugging:
                print 'Renaming "%s" --> "%s".' % (outfname, efname)
            os.rename(outfname, efname) 

    def OnCmdUI(self, cmdui):
        # enable only is regular folder are selected
        sel = cvsgui.App.GetSelection()
        isCvsFileOrFolder = len(sel) > 0
        for s in sel:
            if s.IsMissing() or s.IsUnknown():
                isCvsFileOrFolder = 0
                break

        cmdui.Enable(isCvsFileOrFolder and len(sel) > 0)
        if len(sel) == 1:
            if (sel[0].IsFile()):
                cmdui.SetText("Fix red icon for '" + sel[0].GetName() + "'")
            else:
                cmdui.SetText("Fix red files in '" + sel[0].GetName() + "'")
        else:
            cmdui.SetText("Fix red files ")

    def Run(self):
        print "Looking for erroneous modified files..."
        
        # at this point we are sure we have only regular folders
        sel = cvsgui.App.GetSelection()

        numFixed = 0
        for s in sel:
            if s.IsFile():
                numFixed += self.DoFile(s.GetFullName())
            elif not (s.IsMissing() or s.IsUnknown()):
                numFixed += self.DoFolder(s.GetFullName())

        print numFixed, " file(s) fixed."

    def IsBadDSTentry(self, ent):
        __doc__ = """Is the file with CVS Entry ent a bad DST file?
Answer the question by comparing the mod time in CVS/Entries to the
mod time reported by the file system. If the two differ by an hour
(with a few seconds slop, as dictated by self._slop), mark it as a
candidate DST-problem."""
        if ent.IsFile():
            if ent.IsModified():
                if ent.GetConflict() == '':
                    ent_time = ent.GetUTC()
                    mod_time = GetFileModTime(ent.GetFullName()).strip()
                    # Fix day with 0 (Bug in Visual C++ version of asctime)
                    if mod_time[7:9] == " 0":
                        mod_time = mod_time[:8] + " " + mod_time[9:]
                    try:
                        te = self._strptime(ent_time)
                        tm = self._strptime(mod_time)
                        if te > 0 and tm > 0 and \
                           (abs(abs(te - tm)) < self._slop or
                            abs(abs(te - tm) - 3600.) < self._slop):
                            return (1, mod_time)
                            if _debugging:
                                print "Found '", ent.GetFullName(), "': Entry ", ent, ", file mod time ", mod_time
                    except ValueError, e:
                        pass
        return (0,None)

    def DoFile(self, path):
        __doc__ = """Check a single file and fix its CVS/Entries entry
if it is a DST candidate"""
        folder, fname = os.path.split(path)
        dictEntries = GetCvsEntriesDict(folder)
        entry = dictEntries[fname]
        bFixIt, mod_time = self.IsBadDSTentry(entry)
        if bFixIt:
            self.FixEntries(folder, ((entry, mod_time),))
            print "Fixed DST error on timestamp for '", path, "'"
        return bFixIt

    def DoFolder(self, path):
        __doc__ = """Check all files in a folder and subfolders for DST
problems. Simmilar to DoFile(), but with looping and recursion."""
        numFixed = 0
        toRecurse = []
        list = {}
        list["modified"] = []
        list["tofix"] = []

        # build a list of interesting files for this path
        entries = GetCvsEntriesList(path)
        for e in entries:
            if e.IsFile():
                tofix, mod_time = self.IsBadDSTentry(e)
                if tofix:
                    list["tofix"].append((e,mod_time))
                    if _debugging:
                        print "Appending '", e.GetFullName(), "' to 'tofix' list: Entry: ", e, ", mod time: ", mod_time
                    numFixed += 1
                elif e.IsModified() and e.GetConflict() == '':
                    list["modified"].append(e)
            elif not e.IsMissing() and not e.IsUnknown():
                toRecurse.append(e.GetFullName())

        # now print the list
        if len(list["modified"]) > 0 or len(list["tofix"]) > 0:
            print "In %s :" % path
            console = ColorConsole()
            console  << ("\t%d Modified files :" % len(list["modified"])) << '\n'
            for e in list["modified"]:
                console << kMagenta << "\t\t" << e.GetName() << kNormal << '\n'
            console  << ("\t%d Files to fix for DST:" % len(list["tofix"])) << '\n'
            for (ent, modtime) in list["tofix"]:
                console << kMagenta << "\t\t" << ent.GetName() << kNormal << '\n'
                #': ' << kRed << ent.GetUTC() << kMagenta \
                #       << '-->' << kBlue << modtime << kNormal << '\n'
            if (len(list["tofix"]) > 0):
                self.FixEntries(path, list["tofix"])

        # recurse
        for f in toRecurse:
            numFixed += self.DoFolder(f)

        return numFixed

if os.name == "nt":
        FixDST()
