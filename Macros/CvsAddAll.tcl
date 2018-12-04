#!CVSGUI1.0 --selection --name "Add All to CVS"

# cvsaddall.tcl
# This macro adds all selected files and folders to the cvs repository
# along with all the subfolders and files 

# Author: Gopalakrishnan.P (gopal@workspot.net)
# Version: 1.0

#----------------------------------------------------------------------------
# All procedures for cvsaddall.tcl
#----------------------------------------------------------------------------


set fcount 0;   # global count 

# This procedure addes the folder/file argument to cvs using cvs add 
# In case it fails we return a 1 and break from the add loop.
# We print the cvs message irrespective of the result
proc cvsadd { fname } {
 set out [catch {cvs add $fname} cval]; 
 cvsout "$cval\n"; 
 global fcount;
  if { $out != 0 } {
    return 1; 
  } else {
    incr fcount;
    return 0;
  }
}


# This procedure takes a list and for each element calls addcvs procedure.
# If any of them fails it breaks out from the loop. If any file gets added
# the count value is returned.
proc checkadd { addlist } { 
set cnt 0;
 foreach item $addlist {
   if { [cvsadd $item] } {
     break;
   } else {
     incr cnt;
   }
  }
 return $cnt;
}

# This procedure recurses the folder list and retuns a list of folders
# and files contained therin.
proc recursefolder {flist} {
set filelist {};
set foldlist {};
  if { [llength $flist] > 0 } {
    foreach item $flist {
      cvsentries $item entrydet;
        set files [ entrydet get ];
	  foreach i $files {
	    entrydet info $i farr;
              if { [string equal -nocase $farr(kind) "folder"] && \
	           $farr(missing) == 0 && $farr(unknown) == 1 } {
	        lappend foldlist $i;
              } elseif { [string equal -nocase $farr(kind) "file"] && \
	               $farr(missing) == 0 && $farr(unknown) == 1 } {
	        lappend filelist $i;
	      }
	  }
    }
  }
  return [list $foldlist $filelist]
}

# This procedure concats two lists -- a folderlist and a filelist -- and 
# invokes checkadd procedure. Based on the return value it invokes 
# recursefolder procedure recursively.
proc addfiles {list1 list2} {
  set ret [checkadd [concat $list1 $list2]]
  if { $ret > 0 } {
    set listret [recursefolder $list1]
    if { [llength [lindex $listret 0]] > 0 || [llength [lindex $listret 1]] \
                                                                   >  0 } {
      addfiles [lindex $listret 0] [lindex $listret 1];
    } else {
      return
    }
  }
}

#----------------------------------------------------------------------------
# main body of the program
#----------------------------------------------------------------------------

set dirlist {};
set filelist {};

set select [ cvsbrowser get ]; # get the selection
foreach i $select {
  cvsbrowser info $i aname;
 if { [string equal -nocase $aname(kind) "folder"] && $aname(missing) \
                                     == 0 && $aname(unknown) == 1 } {
   lappend dirlist $i;
 } elseif { [string equal -nocase $aname(kind) "file"] && $aname(missing) \
                                         == 0 && $aname(unknown) == 1 } {
   lappend filelist $i;
 } 
}

# All unknown files and folders are available in the filelist and dirlist
# add them to cvs using addfiles procedure.

addfiles $dirlist $filelist;

# Print out the total count of folders/files added.
cvsout "\n($fcount) folders/files added to cvs repository\n";

#----------------------------------------------------------------------------
