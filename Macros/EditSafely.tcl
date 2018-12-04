#!CVSGUI1.0 --selection --name "Edit Safely"

#
# 5.6.2001 Andre Kostur (andre@incognito.com), Incognito Software Inc.
#
# 14.1.2003 Thomas Günther (thomas.guenther@intesio.de), Alexander Schön (alexander.schoen@intesio.de), Intesio GmbH
# 
# EditSafely.tcl - Ensure that the editors list is empty before
#                  allowing a file to be edited.
#
# Perform a "cvs editors" on a file and check to see if anybody
# else is already editing the file.  If there are no other
# editors, then do a "cvs edit" on the file.  If there are
# multiple files selected, do this sequence on all of the selected
# files.  If any file in the selection has other editors, then
# do a "cvs unedit" on any file that was previously "edit"ed.
# Basically it makes this an all-or-nothing script.  Either all
# files that you have selected are now being edited by you,
# or none of the files are being edited by you.
#
# Version 1.0 - Basic functionality.  Edits a file if it can,
#               does nothing if it can't.
# Version 2.0 - Reworked the display to be easier to read.
#               If the script can't obtain an edit on all of
#               the selected files, release the edits that it
#               did get.
# Version 2.1 - Changed the error output to use cvserr instead
#               of cvsout.
# Version 2.2 - Changed 'release' to 'undone'
# Version 2.3 - If any previous file has been denied, don't bother
#               to mark any subsequent files as "edit", I'll just
#               undo it later.
# Version 2.4 - Update before edit to sync with the repo, getting the writable copy using -w global option
# Version 2.5 - Handling of filenames and paths including spaces is now provided

set selList [cvsbrowser get]
set selSize [llength $selList]

set editsAllowed 0
set editsDenied 0

cvsout "\n"

for {set i 0} {$i < $selSize} {incr i} {
	set file [lindex $selList $i]
	cvsbrowser info $file fileInfo

	set editorsList [eval cvs editors \"$fileInfo(name)\"]
	
	set nameParts [llength $fileInfo(name)]
	set pathParts [llength $fileInfo(path)]
	
	set numEditors [llength $editorsList]

	if {$numEditors > 0} {
		cvserr "EDIT DENIED  : " $fileInfo(name) " is being edited by "
		
		for {set j $nameParts} {$j < $numEditors} {set j [expr $j + 8 + $pathParts]} {
			if {$j > 8} {
				cvserr ", "
			}

			cvserr [lindex $editorsList $j];
		}
		
		cvserr "\n"

		incr editsDenied
	}

	if {$numEditors == 0} {
		if {$editsDenied == 0} {
			cvsout "Get writable and edit: " $fileInfo(name) 

			cvsout [eval cvs -w update \"$fileInfo(name)\"]
			cvsout [eval cvs edit \"$fileInfo(name)\"]
			incr editsAllowed
			lappend succeededEdits $fileInfo(name)
		} else {
			cvsout "Edit possible: "  $fileInfo(name) 
			cvsout " but there is already a failed edit"
		}

		cvsout "\n"
	}
}

if {$editsDenied > 0} {
	if {[info exists succeededEdits]} {
		for {set i 0} {$i < [llength $succeededEdits]} {incr i} {
			eval cvs unedit \"[lindex $succeededEdits $i]\"
			cvsout "Edit undone  : " [lindex $succeededEdits $i] "\n"
		}
	}

	cvserr "\n"
	cvserr "*** EditSafely FAILED. " $editsDenied " FILE";
	
	if {$editsDenied > 1} {
		cvserr "S ARE"
	} else {
		cvserr " IS"
	}

	cvserr " ALREADY BEING EDITED ***\n"
} else {
	cvsout "\n"
	cvsout "*** EditSafely completed successfully. " $editsAllowed " file"
	
	if {$editsAllowed > 1} {
		cvsout "s"
	}
	
	cvsout " being edited ***\n"
}
