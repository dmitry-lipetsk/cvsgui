#!CVSGUI1.0 --selection --name "Build ChangeLog"

# A WinCVS macro for generating a GNU style ChangeLog file.
#
# Please send diffs to David Gravereaux <davygrvy@pobox.com>
# if you can improve this :)
#
# 2002.03.01 -- Denis Ballant added revision numbers and branch names
#		near each filename.  <Denis.Ballant@cediti.be>
#
# 2000.11.07 -- Stephane Matamontero <dev1.gemodek@t-online.de> added 
#               better wordwrapping code in [Cvs2CL::RewriteIt].
#
# RCS: @(#) $Id: cvs2cl.tcl,v 1.10 2002/03/19 13:02:05 jerzyk Exp $


namespace eval ::Cvs2CL {
    variable db                      ;# our database array
    array set db [list]
    variable usermap                 ;# user translations from $CVSROOT/CVSROOT/users
    array set usermap [list]
    variable outputList [list]       ;# the "structure" we write output from
    variable sorter [list]           ;# the pre-sort helper list
    variable CLog_fname "ChangeLog"  ;# the output filename we want to use
    variable rawLog                  ;# All the log in one string before parsing
    variable major [lindex [split $::tcl_version .] 0]
    variable minor [lindex [split $::tcl_version .] 1]
}


proc ::Cvs2CL::Init {} {
    variable CLog_fname
    variable rawLog
    variable usermap

    switch -regexp [string tolower [info nameofexecutable]] {
        ^(.*)wincvs.exe$ -
        ^(.*)maccvs$ -
        ^(.*)gcvs$ {
            set cvsCmd [list cvs -Q log]

            # uncomment this stuff when the replace works in [Cvs2CL::ReWriteIt]
            #
#            if {![catch {set fOldLog [open $CLog_fname r]}]} {
#                cvsout "Checking date range of current $CLog_fname...\n"
#                # the first "word" of the first line is a date string.
#                # we ask cvs for logs greater than this old date
#                lappend cvsCmd -d ">[join [split [lindex [split [gets $fOldLog] { }] 0] /] -]"
#                close $fOldLog
#            }

            cvsout "Downloading the log...\n"
            set rawLog [eval $cvsCmd]

            # TODO: fill the usermap array here
        } 
        ^(.*)tclsh -
        ^(.*)wish {
            # debugging under a tcl shell

            set cvsCmd [list cvs -Q log]
#            puts "Checking date range..."
#            if {![catch {set fOldLog [open $CLog_fname r]}]} {
#                # the first "word" of the first line is a date string.
#                lappend cvsCmd -d "[lindex [split [gets $fOldLog] { }] 0]>"
#                close $fOldLog
#            }
#            puts $cvsCmd

            if {$argc != 2} { puts stderr "gimme a file to open!" ; exit }
            set flog [open [lindex $argv 1] r]
            fconfigure $flog -encoding iso8859-1 -translation auto

            # bring the file into Tcl in one big SLAM!
            # this of-course assumes one character==one byte
            seek $flog 0 end
            set theEnd [tell $flog]
            seek $flog 0 start
            fconfigure $flog -buffersize $theEnd
            puts "Downloading the log..."
            set rawLog [read $flog $theEnd]
            close $flog

            proc ::cvsout {text} {puts -nonewline $text}
        }
    }
}


proc ::Cvs2CL::cmpDateScan {a b} {
    variable major
    variable minor

    if {$major < 8 || ($major == 8 && $minor < 3)} {
        # ISO8601 date formats can't be scanned by tcl8.2, bummer...
        # works in 8.3, though...
        #
        # clock scan wants mm/dd/yyyy, so convert it from yyyy-mm-dd.
        #
        set al [split $a -]
        set a [lreplace $a 0 0 [list "[lindex $al 1]/[lindex $al 2]/[lindex $al 0]"]]
        set bl [split $b -]
        set b [lreplace $b 0 0 [list "[lindex $bl 1]/[lindex $bl 2]/[lindex $bl 0]"]]
    }
    return [expr {[clock scan $a -gmt 1] - [clock scan $b -gmt 1]}]
}


proc ::Cvs2CL::cmpTime {a b} {
    if {[set sort [cmpDateScan [lindex $a 0] [lindex $b 0]]] == 0} {
        # dates are identical so sort using the id
        return [expr {[lindex $a 1] - [lindex $b 1]}]
    }
    return $sort
}


proc ::Cvs2CL::cmpDate {a b} {
    set dateA [lindex [split $a ,] 0]
    set dateB [lindex [split $b ,] 0]
    return [cmpDateScan $dateA $dateB]
}


proc ::Cvs2CL::cmpInteger {a b} {
    return [expr {$a - $b}]
}


proc ::Cvs2CL::ParseIt {} {
    variable db
    variable sorter
    variable rawLog
    variable CLog_fname

    array set db {}
    set sorter {}
    set id 0

    #  make each line a list element
    if {[regexp -nocase {^(.*)maccvs$} [info nameofexecutable]]} {
        #### BUG: MacCvs's cvs Tcl command returns lines with a \r <eol>.
        #### BUG: This is a big bug and needs to be repaired.
        set rawLogList [split $rawLog \r]
    } else {
        set rawLogList [split $rawLog \n]
    }

    # count the number of lines
    set lines [llength $rawLogList]
    for {set a 0} {$a < $lines} {incr a} {
        set line [lindex $rawLogList $a]

        # The log for a certain file and version starts with
        #    Working file: <filename>
        # The is our tag for a new entry into our database. First we just
        # store <filename> in fname
        if {[regexp {^Working file: ([^,]+)} $line null fname]} {

            # we don't care about ourselves
            if {![string compare $CLog_fname $fname]} {
                # skip all the way forward to the next
                while {$a < $lines} {
                    if {[regexp {^======} [lindex $rawLogList $a]]} {break} {incr a}
                }
            }
	    set branchnames(x) x
	    unset branchnames
            continue
        }

	if {[regexp {^revision } $line]} {
	    set rev [lindex $line 1]

	    # If part of a branch, have the branch name appear along with the revision number
	    regexp {(.*)\.[0-9]+$} $rev --> rootrev
	    if [info exists branchnames($rootrev)] {
		set rev "\[$branchnames($rootrev)\] $rev"
	    }
	}

	if {[regexp {^symbolic names:} $line]} {
	    # interpret the list of symbolic names
	    # to get the branches names
            while {[incr a] < $lines} {
                set line [lindex $rawLogList $a]
                if {![regexp {^\t([^:]+): ([0-9\.]+)} $line --> branchname version]} break
		if [regexp {(.*)\.0\.([0-9]+)$} $version --> branchhead branchid] {
#		    cvsout "$branchname - $branchhead.$branchid\n"
		    set branchnames($branchhead.$branchid) $branchname
		}
            }
	}

        # A line like
        #   date: <date> <time> ...
        # follows soon. Everthing after that up to a line with all `='s in
        # it is stored.
        if {[regexp {^date:} $line]} {
            set date [join [split [lindex $line 1] /] -]
            set author [string trimright [lindex $line 4] {;}]
            set text {}

            # skip over branches line if any
            if {[regexp {^branches:} [lindex $rawLogList [expr {$a+1}]]]} {
                incr a
            }

            # extract the message
            while {[incr a] < $lines} {
                set line [lindex $rawLogList $a]
                if {[regexp {^------} $line] || [regexp {^======} $line]} break
                append text "$line\n"
            }
            set text [string trimright $text \n]

            if {![string compare $text "Initial revision"]} {
                # we don't log the initial import branch announcement
                continue
            }

            if {![string compare $text "no message"]} {
                # no message means exactly that.  So ignore it.
                continue
            }

            set db($id,fname) $fname
            set db($id,date) $date
            set db($id,revision) $rev
            set db($id,text) $text
            set db($id,author) $author
            lappend sorter [list $date $id]
            incr id
        }
    }
}


proc ::Cvs2CL::SortIt {} {
    variable outputList
    variable sorter
    variable db

    array set dates [list]
    array set datesAuthors [list]

    # first, do a rough sort along date
    set preSort [lsort -command cmpTime -decreasing $sorter]

    # next, we seperate it by date.
    foreach element $preSort {
        lappend dates([lindex $element 0]) [lindex $element 1]
    }

    # next, sub-divide each date by the commit author
    foreach date [array names dates] {

        # sort the id's in decending numerical order
        set dates($date) [lsort -command cmpInteger -decreasing $dates($date)]

        foreach id $dates($date) {
            lappend datesAuthors($date,$db($id,author)) $id
        }
    }

    # foreach author on a specific date, organize by log message and build the
    # applies-to list then store it, repeat 'till done.
    foreach DApair [lsort -command cmpDate -decreasing [array names datesAuthors]] {
        if {[array exist commitArray]} {unset commitArray}
        set entry [list]
        lappend entry $db([lindex $datesAuthors($DApair) 0],date)
        lappend entry $db([lindex $datesAuthors($DApair) 0],author)
        foreach id $datesAuthors($DApair) {
            lappend commitArray($db($id,text)) "$db($id,fname) $db($id,revision)"
        }

        # alphabetize 'em
        foreach commitMsg [array names commitArray] {
            set commitArray($commitMsg) [lsort $commitArray($commitMsg)]
        }

        lappend entry [array get commitArray]
        lappend outputList $entry
    }
}


proc ::Cvs2CL::RewriteIt {} {
    variable usermap
    variable outputList
    variable CLog_fname

#    if {[file exist $CLog_fname]} {
#        file rename $CLog_fname $CLog_fname.old
#    }

    set fCLog [open $CLog_fname w]
    fconfigure $fCLog -encoding iso8859-1

    foreach outputElement $outputList {
        # write the date
        puts -nonewline $fCLog "[lindex $outputElement 0]  "

        # write the author of this commit.  translate to an email, if listed
        # in the usermap array we got from CVSROOT/users
        if {[info exist usermap([lindex $outputElement 1])]} {
            puts $fCLog $usermap([lindex $outputElement 1])
        } else {
            puts $fCLog [lindex $outputElement 1]
        }

        if {[array exist commits]} {unset commits}
        array set commits [lindex $outputElement 2]

        foreach msg [array names commits] {
            # write the applies-to files
            foreach applyto $commits($msg) {
                puts $fCLog "\t* $applyto:"
            }
            # check if the message has line seperators
            if {[llength [set msg_split [split $msg "\n"]]] > 1} {
                # must already be formatted..  just append some tabs to each line
                foreach i $msg_split {puts $fCLog "\t\t$i"}
                puts $fCLog ""
            } else {
                # check to make sure this single line entry isn't too long
                if {[string length $msg] > 64} {
                    # wordwrap it to look nicer than one big long line
                    set start 0
                    set next 0
                    set previous 0
                    set not_done true
                    while {$not_done} {
                        set start [string wordstart $msg [expr {$previous + 1}]]
                        while {($next - $start) < 64 && $not_done} {
                            set previous $next
                            set next [string wordend $msg [expr {$previous + 1}]]
                            if {$next == [string length $msg]} {
                                set previous end
                                set not_done false
                            }
                        }
                        puts $fCLog "\t\t[string range $msg $start [expr {$previous == "end" ? "end" : $previous - 1}]]"
                    }
                    puts $fCLog ""
                } else {
                    puts $fCLog "\t\t$msg\n"
                }
            }
        }
    }

    close $fCLog

#    if {[file exist $CLog_fname.old]} {
#        file delete -force $CLog_fname.old
#    }
}

Cvs2CL::Init

cvsout "Parsing output...\n"
Cvs2CL::ParseIt

cvsout "Sorting entries...\n"
Cvs2CL::SortIt

cvsout "Rewriting ChangeLog...\n"
Cvs2CL::RewriteIt

cvsout "Done!\n"
