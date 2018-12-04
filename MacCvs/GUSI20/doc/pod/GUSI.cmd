#
# $Log: GUSI.cmd,v $
# Revision 1.2  2002/01/10 17:35:51  aubonbeurre
# Update to GUSI 2.1.9
#
# Revision 1.13  2000/06/01 06:39:43  neeri
# Releasing 2.1.0
#
# Revision 1.12  1999/10/28 05:48:05  neeri
# Getting ready for 2.0, removed BSD advertising clause
#
# Revision 1.11  1999/10/15 02:58:50  neeri
# Getting ready for 2.0fc2
#
# Revision 1.10  1999/09/26 03:59:25  neeri
# Releasing 2.0fc1
#
# Revision 1.9  1999/09/09 07:03:46  neeri
# Update documentation for 2.0b10
#
# Revision 1.8  1999/09/03 06:32:16  neeri
# Getting ready for 2.0b9
#
# Revision 1.7  1999/08/05 05:54:32  neeri
# Updated documentation
#
# Revision 1.6  1999/07/20 04:45:33  neeri
# Getting ready for 2.0b5
#
# Revision 1.5  1999/07/19 06:25:12  neeri
# Getting ready for 2.0b4
#
# Revision 1.4  1999/07/07 04:17:39  neeri
# Final tweaks for 2.0b3
#
# Revision 1.3  1999/03/29 09:51:25  neeri
# New configuration system with support for hardcoded configurations.
#
# Revision 1.2  1999/03/17 09:04:58  neeri
# Added GUSITimer, expanded docs
#
# Revision 1.1  1999/02/26 02:04:14  neeri
# Added Command File
#

cover true
release 'Version 2.1'
booktitle 'GUSI 2 Reference Manual'
author 'Matthias Neeracher'

output GUSI.ps

file GUSI.pod
file GUSI_Install.pod 	"Installation and Configuration"
file GUSI_Common.pod	"Common Routines"
file GUSI_Sockets.pod	"Socket Specific Routines"
file GUSI_Files.pod		"File Specific Routines"
file GUSI_Threads.pod	"Threading"
file GUSI_Misc.pod		"Miscellanea"
