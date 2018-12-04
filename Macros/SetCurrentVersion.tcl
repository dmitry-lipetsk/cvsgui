#!CVSGUI1.0 --selection --name "Copy to HEAD version"

# Author: Rafael Sanz <rafael_sanz@ya.com>

# This macro move your selected version to head revision
# For that:
# rename the file to temporal name
# update the head version
# set to edit mode
# delete the head version
# and rename the temporal file


set selList [cvsbrowser get]
set selSize [llength $selList]

cvsout "\n--- Files selected:  "
cvsout [clock format [clock seconds] -format "%a %m/%d/%y %H:%M:%S"] " ---\n"

for {set i 0} {$i < $selSize} {incr i} {
   set file [lindex $selList $i]

   cvsbrowser info $file fileInfo

   if { $fileInfo(missing) == 0 && $fileInfo(unknown) == 0 && $fileInfo(kind) == "file"} {

      cd $fileInfo(path)
      set tmpName tmp_$fileInfo(name)
      file copy -force  $fileInfo(name) $tmpName
      cvsout "Bringing file " $fileInfo(name) " based on revision " $fileInfo(revision) " to HEAD\n"
      cvs update -A $fileInfo(name)
      file copy -force  $tmpName $fileInfo(name)
      file delete $tmpName
      cvs edit $fileInfo(name)
   }
}

cvsout "----------------------------------------\n"
cvsout "(done)\n"
