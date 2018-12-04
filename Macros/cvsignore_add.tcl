#!CVSGUI1.0 --selection --name "Add to .cvsignore"

global numAdded
set numAdded 0

set selList [cvsbrowser get]
set selSize [llength $selList]

cvsout "Processing .cvsignore ...\n"

for {set i 0} {$i < $selSize} {incr i} {

	set found 0
	set filename [lindex $selList $i]

	cvsbrowser info $filename fileInfo
	
	if { [file exists .cvsignore]== 1 } {
	
		set wfile [open .cvsignore r]
		while {[eof $wfile]==0} {
			set test ""
			gets $wfile test
			if {[string equal $test $fileInfo(name)] == 1} { 
				set found 1
				cvsout "$test already in .cvsignore\n"
				break
			}
		}
		close $wfile
	}
	set wfile [open .cvsignore a+]
	if { $found == 0 } {
		puts $wfile "$fileInfo(name)"
		incr numAdded
		cvsout "$fileInfo(name) added to .cvsignore\n"
	}
	close $wfile
}

#cvsout "Done!\n"
cvsout "$numAdded pattern(s) added to .cvsignore.\n"
