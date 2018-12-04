#!CVSGUI1.0 --selection  --name "Prepare patch from [pwd]"
# --------------------------------------------------------------------
#  PrepPatch.tcl  --
#
#  A macro for WinCVS to generate error-free(tm) diffs all set to
#  email the maintainer of the module.
#
#  New files are diff'ed against a nul target to make a pure additive
#  difference with an external diff program.  Modified repository
#  files are diff'd with CVS.  Paths are modified in the patchfile
#  to ensure patch.exe understands what goes where without error.
#
#  The directory patch.txt is written to coresponds (exactly) to the
#  directory patch.exe should be run from by the maintainer and will
#  only require a minimum of switches:
#
#     C:\Some\Path\To\A\CVSWorkingCopy\> patch.exe -c -i patch.txt
#  or                  "                 patch.exe -c < patch.txt
#
#  you may even drop -c if you want.
#
#  UNIX users applying patch.txt may need to correct for <EOL> or add
#  --ignore-whitespace to the patch commandline.
#
#  author: David Gravereaux <davygrvy@pobox.com>  3:36 PM 4/6/2000
#
#  RCS: @(#) $Id: PrepPatch.tcl,v 1.1 2001/06/24 07:33:59 davygrvy Exp $
# --------------------------------------------------------------------

set newFiles [list]
set cvsFiles [list]
set root [pwd]
set outFile [file join $root patch.txt]

# diff.exe included with WinCVS 1.1b12 is broken.
# It crashes with the -N switch and nul as target1.
# ex.  c:\> diff -c -N nul somefile.c > patchfile
#
# WinCVS's patch.exe is even more screwed, too.
# but it might have been fixed in a later release.
#
set ourDiffApp "c:/cygwin/bin/diff.exe"


proc iterate {dirName relativeDir {ignoreFile ""}} {
  global newFiles cvsFiles
  set toRecurse [list]

  cvsentries $dirName browsit

  foreach file [browsit get] {
    browsit info $file fileInfo

    # an odd form of mental logic tells me that if a .cvsignore
    # file exists and is not under version control, we would not
    # want to diff it with this script.
    #
    if {![string compare $fileInfo(kind) "file"] &&
        !$fileInfo(ignored)  &&
        [string compare $ignoreFile $fileInfo(name)] != 0} {

      if {$fileInfo(modified)} {
        lappend cvsFiles [file join $relativeDir $fileInfo(name)]
      } elseif {$fileInfo(unknown) && [string compare $fileInfo(name) ".cvsignore"] != 0} {
        lappend newFiles [file join $relativeDir $fileInfo(name)]
      }
    }

    if {![string compare $fileInfo(kind) "folder"] &&
        !$fileInfo(missing) &&
        !$fileInfo(ignored)} {
      lappend toRecurse [list $file [file join $relativeDir $fileInfo(name)]]
    }
  }

  foreach dir $toRecurse {
    set cmdline "iterate $dir"
    eval $cmdline
  }
}


proc diffNEW {fileName} {
  global ourDiffApp out
  set lines [list]
  set diffPipe [open "|$ourDiffApp -c -N nul $fileName" r]
  fconfigure $diffPipe -buffering line
  while {![eof $diffPipe]} {
    lappend lines [gets $diffPipe]
  }

  # diff.exe returns an errorcode.  Ignore it with catch.
  catch {close $diffPipe}

  if {[llength $lines] == 2} {
    # must have been a binary file by accident...
    return
  }

  cvsout "  Added new: $fileName\n"
  set lines [lreplace $lines 0 0 "*** [lindex [lindex $lines 1] 1]\tWed Dec 31 14:00:00 1969"]

  foreach line $lines {
    puts $out $line
  }
  flush $out
}


proc diffCVS {fileName} {
  global out

  catch {cvs diff -c $fileName} lines

  cvsout "  Added mod: $fileName\n"

  # strip most of the header 'cause it's junk.
  set lines [lreplace [split $lines \n] 0 4]

  # replace the filenames with the relative path one.
  # the form is: "*** <filename>\t<timestamp>[\t<revision>]
  #
  set f_t_rList1 [split [lindex $lines 0] \t]
  set firstline  "*** [join [lreplace $f_t_rList1 0 0 $fileName] \t]"

  set f_t_rList2 [split [lindex $lines 1] \t]
  set secondline "--- [join [lreplace $f_t_rList2 0 0 $fileName] \t]"

  set lines [lreplace $lines 0 1 $firstline $secondline]

  # write it out.
  foreach line $lines {
    puts $out $line
  }
  flush $out
}

cvsout "Iterating directories...\n"
iterate $root "" [file tail $outFile]

cd $root
set out [open $outFile w]
fconfigure $out -translation crlf

cvsout "Diff'ing new files locally...\n"
foreach newFile $newFiles {
  diffNEW $newFile
}

cvsout "Diff'ing modified files against repository...\n"
foreach cvsFile $cvsFiles {
  diffCVS $cvsFile
}
close $out

cvsout "$outFile created. Done!\n"