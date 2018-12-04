#!CVSGUI1.0 --selection  --name "Force Update"
# Originally by: Scott MacHaffie <scottm@saltire.com>
# Modified by: Jerzy Kaczorowski <kaczoroj@hotmail.com>
#
# Forces update - remove selected non-modified files and call cvs up -C on all selected
# Only works on files (by design, too dangerous for directories IMHO) 

set selList [cvsbrowser get]
set selSize [llength $selList]

cvsout "Force Update...\n"
for {set i 0} {$i < $selSize} {incr i} {
	set file [lindex $selList $i]
	cvsbrowser info $file fileInfo
	if {[string compare $fileInfo(kind) "file"] == 0 && $fileInfo(unknown) == 0} {
		if {$fileInfo(missing) == 0 && $fileInfo(modified) == 0} {
			trash $file
			cvserr "D " $fileInfo(name) "\n"
		}

		cvsout "U " $fileInfo(name) "\n"
		cvs update -C $fileInfo(name)
	}
}

