#!CVSGUI1.0 --selection --name "Change CVSRoot (GUI)"

# Author: Rafael Sanz <rafael_sanz@ya.com>
# RCS: @(#) $Id: ChangeRootTK.tcl,v 1.1 2001/04/03 00:23:48 davygrvy Exp $

# This macro is versioned from ChangeRoot.tcl
# and added a GUI for interactive use.
# Is required that TK dll is installed in the same
# directory where the tcl library is.  Normally, this is true.

#Load the TK library
package require Tk

#Global flag
set end 0
set numChanged 0
set changeTo ""


#Open the current CVS/Root 
proc getCVSRoot {dirName} {

	set fName [file join $dirName CVS Root]
	if {[file exists $fName]} {
		set fileid [open $fName r]
		set cvr [gets $fileid]
		close $fileid
		return "$cvr"
	}
	
	return ""
}

#Retrieve the content of .cvspass file
proc getloggedRoot {{fPass ""}} {
	
	#Predefined path .../WinCVS/Settings/.cvspass
	if { [string length $fPass]<=0} {
		set fPass [file join [file dirname [info nameofexecutable]] Settings .cvspass]
		}
	
	set loggedRoot {}
	
	if {[file exists $fPass]} {
		set fileid [open $fPass r]
		while {	[string length [set cvr [gets $fileid]]] } {
			regexp "(.*?) (.*)" $cvr match cvsroot pass]!=0
			lappend loggedRoot $cvsroot
			}
		close $fileid
	} else {
	set loggedRoot [list {} {}]
	}
	
	return $loggedRoot
}


#Dialog to select a new CVSRoot
proc ChoiceRoot {initialRoot} {
	
	global end
	
	#Create the window and center it.
	wm title . "Change cvsroot"
	set x [expr ([winfo rootx .]+[winfo screenwidth .]-150)/2]
	set y [expr ([winfo rooty .]+[winfo screenheight .]-50)/2]
	wm geometry . "+$x+$y"
	wm resizable . 0 0
	#Create widget and Bind procedures
	wm protocol . WM_DELETE_WINDOW {set end no}
	button .ok -text Apply -command {set end yes}
	button .cancel -text Cancel -command {set end no}
	label .label1 -text "Select new cvsroot for [pwd]:"
	label .label2 -text "Logged cvsroot:"
	entry  .data -width 60
	listbox .list -height 0 -width 60 -selectmode browse
		
	#Pack components
	pack  .label1 .data .label2 .list -side top -anchor w -padx 15 -pady 5
	pack  .cancel .ok -side right -expand 1 -pady 15 -ipadx 15
	
	#Set initial values
	.data insert 0 $initialRoot
	foreach {lin} [getloggedRoot] {	.list insert end $lin }
	bind .list <<ListboxSelect>> { .data delete 0 end 
	.data insert 0 [.list get [.list curselection]]}
	
	# block into the event loop waiting for the variable to be changed.
	update
	vwait end

	#Released the window, then return
	switch $end {
		yes {return [.data get]}
		no {return ""}
	}
	
}



# SELECT THE CVSROOT
set changeTo [ChoiceRoot [getCVSRoot .]]
if {[string length $changeTo]==0} {
	cvsout "Canceled by user!\n"
	return
}




proc changeRoot {dirName} {
	set oldDir [pwd]
	cd $dirName

	set fileid [open Root w]
	global changeTo
	puts -nonewline $fileid $changeTo
	close $fileid
	
	global numChanged
	incr numChanged
	
	cd $oldDir
}

proc iterate {dirName} {
	set oldDir [pwd]
	cd $dirName
	cvsout "Entering $dirName\n"

	set dirList [glob -nocomplain *]
	set dirSize [llength $dirList]
	for {set j 0} {$j < $dirSize} {incr j} {
		set fileName [lindex $dirList $j]
		if {[file isdirectory $fileName]} {
			if {[string compare cvs [string tolower $fileName]] == 0} {
				changeRoot $fileName
			} else {
				iterate $fileName
			}
		}
	}
	cd $oldDir
}

set selList [cvsbrowser get]
set selSize [llength $selList]

for {set i 0} {$i < $selSize} {incr i} {
	set filename [lindex $selList $i]
	cvsbrowser info $filename fileInfo

	# check it is a folder
	if {[string compare $fileInfo(kind) "folder"] == 0} {
		iterate $filename
	}
}
cvsout "Done !\n"
cvsout "$numChanged file(s) changed !\n"
