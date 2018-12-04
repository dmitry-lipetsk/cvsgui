#######################################################################
# 	$Id: GUSIConfig.mk,v 1.2 2002/01/10 17:35:51 aubonbeurre Exp $
#
#	Project	:	GUSI 2				-	Grand Unified Socket Interface
#	File	:	GUSIConfig.mk		-	Build configuration
#	Author	:	Matthias Neeracher		Language	:	dmake
#	
#	$Log: GUSIConfig.mk,v $
#	Revision 1.2  2002/01/10 17:35:51  aubonbeurre
#	Update to GUSI 2.1.9
#	
#	Revision 1.1  2001/01/17 07:13:45  neeri
#	Make LP tools configurable
#	

#
# The literate programming tools to use, and whether you want to tangle or
# to untangle (tangle preserves references to the literate source, untangle
# output is far more readable).
#
TANGLE	=	neitheruntangle
UNTANGLE=	neitheruntangle
