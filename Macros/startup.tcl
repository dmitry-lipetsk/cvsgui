# put some stuff here that you want to get executed in each
# tcl interpreter.

if {$::tcl_platform(platform) == "windows"} {
    package ifneeded Tk $::tcl_version [list load [file join [info library] .. .. bin tk[string map {. {}} $::tcl_version].dll]]
}