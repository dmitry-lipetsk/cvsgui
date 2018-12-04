#!CVSGUI1.0 --selection --name "Remove from .cvsignore"

global numRemoved
set numRemoved 0

set selList [cvsbrowser get]
set selSize [llength $selList]

cvsout "Processing .cvsignore ...\n"

if { [file exists .cvsignore]== 1 } {

	set rfile [open .cvsignore r]
	set wfile [open .CVSIGNOR_ a+]
	while {[eof $rfile]==0} {
		set test ""
		gets $rfile test
		for {set i 0} {$i < $selSize} {incr i} {
			set found 0
			set filename [lindex $selList $i]
			cvsbrowser info $filename fileInfo

			if {[string equal $test $fileInfo(name)] == 1} { 
				set found 1
				cvsout "$test removed from .cvsignore\n"
				incr numRemoved
				break
			}
		}
		if { $found == 0 } {
			#cvsout "written $test\n"
			puts $wfile "$test"
		}
	}
	close $rfile
	close $wfile
}

if { $numRemoved > 0 } {
	#file delete .cvsignore
	append fileBck ".#" ".cvsignore" ".bak"
	if {[file exists $fileBck]} {
		file attributes $fileBck -readonly 0
		trash $fileBck
	}
	file rename .cvsignore $fileBck
	file rename .CVSIGNOR_ .cvsignore
} else {
	file delete .CVSIGNOR_
}

cvsout "$numRemoved pattern(s) removed from .cvsignore.\n"
