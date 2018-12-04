# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
# % $Id: Makefile.mk,v 1.2 2002/01/10 17:35:54 aubonbeurre Exp $               
# %                                                                       
# % Project	:	GUSI 2				-	Grand Unified Socket Interface                  
# % File		:	Makefile			-	Build library                                    
# % Author	:	Matthias Neeracher		Language	:	dmake                         
# %	                                                                      
# % $Log: Makefile.mk,v $
# % Revision 1.2  2002/01/10 17:35:54  aubonbeurre
# % Update to GUSI 2.1.9
# %                                                 
# % Revision 1.26  2001/01/17 08:48:21  neeri                             
# % Make LP tools configurable                                            
# %                                                                       
# % Revision 1.25  2000/05/23 07:24:58  neeri                             
# % Improve formatting                                                    
# %                                                                       
# % Revision 1.24  2000/01/17 01:42:39  neeri                             
# % Update to CW 5.3 command line tools                                   
# %                                                                       
# % Revision 1.23  1999/09/09 07:18:06  neeri                             
# % Added support for foreign threads                                     
# %                                                                       
# % Revision 1.22  1999/09/03 06:31:37  neeri                             
# % Needed more mopups                                                    
# %                                                                       
# % Revision 1.21  1999/08/26 05:45:10  neeri                             
# % Fixes for literate edition of source code                             
# %                                                                       
# % Revision 1.20  1999/08/02 07:02:46  neeri                             
# % Support for asynchronous errors and other socket options              
# %                                                                       
# % Revision 1.19  1999/07/19 06:18:51  neeri                             
# % Add SIOW support                                                      
# %                                                                       
# % Revision 1.18  1999/06/30 07:42:08  neeri                             
# % Getting ready to release 2.0b3                                        
# %                                                                       
# % Revision 1.17  1999/06/08 06:09:20  neeri                             
# % Create include/mpw                                                    
# %                                                                       
# % Revision 1.16  1999/06/08 04:31:31  neeri                             
# % Getting ready for 2.0b2                                               
# %                                                                       
# % Revision 1.15  1999/05/30 03:09:33  neeri                             
# % Added support for MPW compilers                                       
# %                                                                       
# % Revision 1.14  1999/04/29 04:57:03  neeri                             
# % Added Sfio Support                                                    
# %                                                                       
# % Revision 1.13  1999/03/17 09:05:14  neeri                             
# % Added GUSITimer, expanded docs                                        
# %                                                                       
# % Revision 1.12  1998/11/22 23:07:02  neeri                             
# % Releasing 2.0a4 in a hurry                                            
# %                                                                       
# % Revision 1.11  1998/10/25 11:57:40  neeri                             
# % Ready to release 2.0a3                                                
# %                                                                       
# % Revision 1.10  1998/10/11 16:45:25  neeri                             
# % Ready to release 2.0a2                                                
# %                                                                       
# % Revision 1.9  1998/08/01 21:32:12  neeri                              
# % About ready for 2.0a1                                                 
# %                                                                       
# % Revision 1.8  1998/02/11 12:57:15  neeri                              
# % PowerPC Build                                                         
# %                                                                       
# % Revision 1.7  1998/01/25 20:54:00  neeri                              
# % Engine implemented, except for signals & scheduling                   
# %                                                                       
# % Revision 1.6  1997/11/13 21:12:13  neeri                              
# % Fall 1997                                                             
# %                                                                       
# % Revision 1.5  1996/12/22 19:57:58  neeri                              
# % TCP streams work                                                      
# %                                                                       
# % Revision 1.4  1996/12/16 02:18:47  neeri                              
# % Add MacTCP/TCP                                                        
# %                                                                       
# % Revision 1.3  1996/11/24  12:52:10  neeri                             
# % Added GUSIPipeSockets                                                 
# %                                                                       
# % Revision 1.2  1996/11/18  00:53:47  neeri                             
# % TestTimers (basic threading/timer test) works                         
# %                                                                       
# % Revision 1.1.1.1  1996/11/03  02:43:33  neeri                         
# % Imported into CVS                                                     
# %                                                                       
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
#                                                                         
# \chapter{Build rules for GUSI}                                          
#                                                                         
#                                                                         
# <Makefile.mk>=                                                          
.INCLUDE : "::GUSIConfig.mk"

# For some parts of GUSI, you may want to use STLport and/or SFIO. If so, specify their paths here.
#                                                                         
# <Paths to be configured>=                                               
SFIOInc		=	-i ::::sfio98:src:lib:sfio:
STLportInc	=	-i ::::STLport-3.12.3:stl:
# Our build targets are the Metrowerks C/C++ compiler and the standard MPW compilers SCpp and MrC.
#                                                                         
# <Compiler options for GUSI>=                                            
MWCWarn		= 	-w noimplicitconv,nostructclass,nopossible
MWCSysInc	= 	-i "{{CWANSIIncludes}}" -i "{{CIncludes}}" -i ::DCon:Headers: ${SFIOInc}
MWCInc		= 	-convertpaths -nodefaults -i- -i ::include ${MWCSysInc}
MPWInc		= 	-i ::include -i "{{CIncludes}}" -i ::DCon:Headers:  ${SFIOInc} ${STLportInc} 
COpt		=	-d SystemSevenOrLater -d OLDROUTINENAMES=0 -bool on -sym on 
Opt68K		=	-model far -mc68020 -mbg on
OptPPC		=	-tb on
MWCOpt		=	${COpt} ${MWCInc} ${MWCWarn} -opt full
MPWOpt		=	${COpt} ${MPWInc} -ER -includes unix -w 2,6,35
C68K		=	MWC68K ${MWCOpt} ${Opt68K}
CPPC		=	MWCPPC ${MWCOpt} ${OptPPC}
CSC			=	SCpp ${MPWOpt} ${Opt68K}
CMRC		=	MrCpp ${MPWOpt} ${OptPPC} -ansifor
ROptions 	= 	-i :
Lib68K		=	MWLink68K -xm library -sym on
LibPPC		=	MWLinkPPC -xm library -sym on
LibSC		=	Lib -sym on -d
LibMrC		=	PPCLink -xm l -sym on -d
# The GUSI distribution consists of a variety of file types.              
#                                                                         
# <Files for GUSI>=                                                       
# The original source code resides in noweb [[.nw]] files.                
#                                                                         
# <Noweb files for GUSI>=                                                 
COREWEBS		=	\
	GUSIDiag.nw			GUSIBasics.nw		GUSIContext.nw		GUSISpecific.nw			\
	GUSISocket.nw		GUSIBuffer.nw		GUSIPipe.nw			GUSIFactory.nw			\
	GUSIMTInet.nw		GUSISocketMixins.nw	GUSIMTTcp.nw		GUSIFileSpec.nw			\
	GUSIDevice.nw		GUSIMacFile.nw		GUSIConfig.nw		GUSIDescriptor.nw		\
	GUSIPOSIX.nw		GUSIPThread.nw		GUSIContextQueue.nw	GUSINull.nw				\
	GUSIInet.nw			GUSINetDB.nw		GUSIMTNetDB.nw		GUSIFSWrappers.nw		\
	GUSIMTUdp.nw		GUSIOTInet.nw		GUSIOTNetDB.nw		GUSIOpenTransport.nw	\
	GUSIDCon.nw			GUSIPPC.nw			GUSITimer.nw		GUSISignal.nw
MSLWEBS			= 	\
	GUSIMSL.nw
SFIOWEBS		= 	\
	GUSISfio.nw
STDIOWEBS		= 	\
	GUSIMPWStdio.nw
MPWWEBS			= 	\
	GUSIMPW.nw
SIOUXWEBS		=	\
	GUSISIOUX.nw
SIOWWEBS		=	\
	GUSISIOW.nw
FTWEBS		=	\
	GUSIForeignThreads.nw
SRCWEBS		=	$(COREWEBS) $(MSLWEBS) $(SFIOWEBS) $(STDIOWEBS) $(MPWWEBS) \
	$(SIOUXWEBS) $(SIOWWEBS) $(FTWEBS)
BUILDWEBS	=	Makefile.nw
WEBS		=	$(SRCWEBS) $(BUILDWEBS) 
# These are tangled (or untangled) into C++ and DMake files.              
#                                                                         
# <Tangled files for GUSI>=                                               
TANGLED		=	$(SRCWEBS:s/.nw/.nws/) pthread.h sched.h GUSIInternal.h ::include:sys:ppc.h
BUILDS		=	$(BUILDWEBS:s/.nw/.mk/)
CORESRC		=	$(COREWEBS:s/.nw/.cp/)
MSLSRC		=	$(MSLWEBS:s/.nw/.cp/)
SFIOSRC		=	$(SFIOWEBS:s/.nw/.cp/)
STDIOSRC	=	$(STDIOWEBS:s/.nw/.cp/)
MPWSRC		=	$(MPWWEBS:s/.nw/.cp/)
SIOUXSRC	=	$(SIOUXWEBS:s/.nw/.cp/)
SIOWSRC		=	$(SIOWWEBS:s/.nw/.cp/)
FTSRC		=	$(FTWEBS:s/.nw/.cp/)
FORWARDSRC	= 	GUSIPOSIX.cp GUSISignal.cp
SOURCES		= 	$(CORESRC) $(MSLSRC) $(SFIOSRC) $(MPWSRC) $(SIOUXSRC) $(FTSRC)
MPWSOURCES	= 	$(CORESRC) $(SFIOSRC) $(MPWSRC) $(STDIOSRC) $(SIOWSRC) $(FTSRC)
# Alternatively, the noweb sources are woven into \TeX sources.           
#                                                                         
# <Woven files for GUSI>=                                                 
WOVEN		= 	$(WEBS:s/.nw/.nww/)
# The C++ files get compiled into object files.                           
#                                                                         
# <Object files for GUSI>=                                                
OBJ68K		=	{$(SOURCES)}.68K.o
OBJPPC		=	{$(SOURCES)}.PPC.o
OBJSC		=	{$(MPWSOURCES)}.SC.o
OBJMRC		=	{$(MPWSOURCES)}.MrC.o
# And finally, the object files get linked into libraries.                
#                                                                         
# <Library files for GUSI>=                                               
GUSIL		=	Core MSL Sfio MPW SIOUX ForeignThreads
GUSILMPW	=	Core Sfio MPW SIOW ForeignThreads
GUSILIBS68K	=	GUSI_{$(GUSIL)}.68K.Lib GUSI_Forward.68K.Lib
GUSILIBSPPC	=	GUSI_{$(GUSIL)}.PPC.Lib
GUSILIBSSC	=	GUSI_{$(GUSILMPW) Stdio}.SC.Lib
GUSILIBSMRC	=	GUSI_{$(GUSILMPW)}.MrC.Lib
# All files in the {\tt web} subdirectory are original. All files in {\tt tangled} and
# {\tt obj} are derived from nowebs. {\tt include} contains both GUSI headers, which
# are built from nowebs, and BSD 4.4 headers which are included as is.    
#                                                                         
# <Directory locations for GUSI>=                                         
.SOURCE.nw	:	":"
.SOURCE.h	:	"::include"
.SOURCE.c	:	":tangled"
.SOURCE.cp	:	":tangled"
.SOURCE.nws	:	":tangled"
.SOURCE.nww	:	":woven"
.SOURCE.o	:	":obj"
.SOURCE.68K	:	":obj"
.SOURCE.PPC	:	":obj"
.SOURCE.SC	:	":obj"
.SOURCE.MrC	:	":obj"
.SOURCE.Lib	:	"::lib"
# Most targets can be defined by a pattern rules, combined with a directory rule.
#                                                                         
# <Pattern rules for GUSI>=                                               
%.nws 	: %.nw
	$(TANGLE) '-c++' -L -t -R$*.h  $< > tmp
	::scripts:update-source tmp $(@:s/nws/h/:s/:tangled/::include/)
	$(TANGLE) '-c++' -L -t -R$*.cp $< > tmp
	::scripts:update-source tmp $(@:s/nws/cp/)
	Set EzDepend 1
	echo > $@
%.nww	: %.nw
	pnoweave -t4 -x -n -db GUSI.db $< > $@
%.68K.o		:	%
	Set Echo 0
	Set Src68K "{{Src68K}} $<"
%.PPC.o		:	%
	Set Echo 0
	Set SrcPPC "{{SrcPPC}} $<"
%.SC.o		:	%
	$(CSC)  $< -o $@
%.MrC.o		:	%
	$(CMRC)  $< -o $@
# <Top level rules for GUSI>=                                             
all		:	lib
.PHONY 	:	source lib
source 	:	tangled $(TANGLED)
	If "{{EzDepend}}" != "" 
		::scripts:EzDepend -f Makefile.mk :tangled:�.cp -i ::include
	End
	Echo > source
lib		:	lib-ppc lib-68k lib-sc lib-mrc
lib-68k	:	source $(GUSILIBS68K)
lib-ppc	:	source $(GUSILIBSPPC)
lib-sc	:	source $(GUSILIBSSC)
lib-mrc	:	source $(GUSILIBSMRC)
liber	:	woven "Liber GUSI.ps"
# <Library build rules for GUSI>=                                         
GUSI_Core.68K.Lib	:	Objects68K
	$(Lib68K) -o $@ :obj:{$(CORESRC)}.68K.o
GUSI_Core.PPC.Lib	:	ObjectsPPC
	$(LibPPC) -o $@ :obj:{$(CORESRC)}.PPC.o
GUSI_Core.SC.Lib	:	ObjectsSC
	$(LibSC) -o $@ :obj:{$(CORESRC)}.SC.o
GUSI_Core.MrC.Lib	:	ObjectsMrC
	$(LibMrC) -o $@ :obj:{$(CORESRC)}.MrC.o
GUSI_MSL.68K.Lib	:	Objects68K
	$(Lib68K) -o $@ :obj:{$(MSLSRC)}.68K.o
GUSI_MSL.PPC.Lib	:	ObjectsPPC
	$(LibPPC) -o $@ :obj:{$(MSLSRC)}.PPC.o
GUSI_Sfio.68K.Lib	:	Objects68K
	$(Lib68K) -o $@ :obj:{$(SFIOSRC)}.68K.o
GUSI_Sfio.PPC.Lib	:	ObjectsPPC
	$(LibPPC) -o $@ :obj:{$(SFIOSRC)}.PPC.o
GUSI_Sfio.SC.Lib	:	ObjectsSC
	$(LibSC) -o $@ :obj:{$(SFIOSRC)}.SC.o
GUSI_Sfio.MrC.Lib	:	ObjectsMrC
	$(LibMrC) -o $@ :obj:{$(SFIOSRC)}.MrC.o
GUSI_Stdio.SC.Lib	:	ObjectsSC
	$(LibSC) -o $@ :obj:{$(STDIOSRC)}.SC.o
GUSI_MPW.68K.Lib	:	Objects68K
	$(Lib68K) -o $@ :obj:{$(MPWSRC)}.68K.o GUSIMPWGlue.68K.Lib
GUSI_MPW.PPC.Lib	:	ObjectsPPC
	$(LibPPC) -o $@ :obj:{$(MPWSRC)}.PPC.o
GUSI_MPW.SC.Lib	:	ObjectsSC
	$(LibSC) -o $@ :obj:{$(MPWSRC)}.SC.o GUSIMPWGlue.SC.Lib
GUSI_MPW.MrC.Lib	:	ObjectsMrC
	$(LibMrC) -o $@ :obj:{$(MPWSRC)}.MrC.o 
GUSI_SIOUX.68K.Lib	:	Objects68K
	$(Lib68K) -o $@ :obj:{$(SIOUXSRC)}.68K.o
GUSI_SIOUX.PPC.Lib	:	ObjectsPPC
	$(LibPPC) -o $@ :obj:{$(SIOUXSRC)}.PPC.o
GUSI_SIOW.SC.Lib	:	ObjectsSC
	$(LibSC) -o $@ :obj:{$(SIOWSRC) $(MPWSRC)}.SC.o GUSIMPWGlue.SC.Lib
GUSI_SIOW.MrC.Lib	:	ObjectsMrC
	$(LibMrC) -o $@ :obj:{$(SIOWSRC) $(MPWSRC)}.MrC.o
GUSI_ForeignThreads.68K.Lib	:	Objects68K
	$(Lib68K) -o $@ :obj:{$(FTSRC)}.68K.o
GUSI_ForeignThreads.PPC.Lib	:	ObjectsPPC
	$(LibPPC) -o $@ :obj:{$(FTSRC)}.PPC.o
GUSI_ForeignThreads.SC.Lib	:	ObjectsSC
	$(LibSC) -o $@ :obj:{$(FTSRC)}.SC.o
GUSI_ForeignThreads.MrC.Lib	:	ObjectsMrC
	$(LibMrC) -o $@ :obj:{$(FTSRC)}.MrC.o
GUSI_Forward.68K.Lib	:	Objects68K
	$(Lib68K) -o $@ :obj:{$(FORWARDSRC)}.68K.o
# The [[pthread.h]] header file is a standard C library header, but generated from
# a noweb file. [[GUSIInternal.h]] is too small to merit its own noweb file.
#                                                                         
# <Additional dependences for GUSI>=                                      
pthread.h		:	GUSIPThread.nw
	$(UNTANGLE) '-c++' -t -Rpthread.h 		$<	> $@
sched.h		:	GUSIPThread.nw
	$(UNTANGLE) '-c++' -t -Rsched.h 		$<	> $@
GUSIPThread.cp	:	GUSIContext.h
GUSIInternal.h	:	GUSIBasics.nw
	$(TANGLE) '-c++' -L -t -RGUSIInternal.h 	$<	> $@
"::include:sys:ppc.h"	:	GUSIPPC.nw
	$(UNTANGLE) '-c++' -t '-Rsys/ppc.h' 		$<	> ::include:sys:ppc.h
# <Documentation build rules for GUSI>=                                   
"Liber GUSI.ps"  : GUSI.dvi
	dvips -o "Liber GUSI.ps" GUSI.dvi

GUSI.dvi : GUSI.tex
	biglatex GUSI
	pnoindex GUSI

GUSI.tex : $(WOVEN)
	setfile -m . $@
# <Object build rules for GUSI>=                                          
obj :
	NewFolder	obj

"::lib" :
	NewFolder	::lib

tangled :
	NewFolder	tangled
woven :
	NewFolder	woven

"::include:mpw:" : 
	NewFolder "::include:mpw:"
	Duplicate -y "{{CIncludes}}"errno.h ::include:mpw:errno.h
# <Bulk build rules for GUSI>=                                            
Objects68K	:	obj "::lib" $(OBJ68K) 
	Set Echo 1
	If "{{Src68K}}" != "" 
		${C68K} -t -fatext {{Src68K}} -o :Obj:
	End
	echo > Objects68K
	
ObjectsPPC	:	obj "::lib" $(OBJPPC) 	
	Set Echo 1
	If "{{SrcPPC}}" != "" 
		${CPPC} -t -fatext {{SrcPPC}} -o :Obj:
	End
	echo > ObjectsPPC
	
ObjectsSC	:	obj "::include:mpw:" "::lib" $(OBJSC) 	
ObjectsMrC	:	obj "::lib" $(OBJMRC) 	

### Dependences start here. Do not change anything below this line
GUSIBasics.cp        : GUSIBasics.h GUSIDiag.h GUSIInternal.h	; setfile -m . $@
GUSIBuffer.cp        : GUSIBuffer.h GUSIInternal.h	; setfile -m . $@
GUSIBuffer.h         : GUSIBasics.h GUSIDiag.h	; setfile -m . $@
GUSIConfig.cp        : GUSIBasics.h GUSIConfig.h GUSIContext.h GUSIDiag.h GUSIFSWrappers.h GUSIInternal.h	; setfile -m . $@
GUSIConfig.h         : GUSIFileSpec.h	; setfile -m . $@
GUSIContext.cp       : GUSIConfig.h GUSIContext.h GUSIDescriptor.h GUSIDiag.h GUSIInternal.h GUSISignal.h GUSITimer.h	; setfile -m . $@
GUSIContext.h        : GUSIBasics.h GUSIContextQueue.h GUSISpecific.h	; setfile -m . $@
GUSIContextQueue.cp  : GUSIContext.h GUSIContextQueue.h GUSIDiag.h GUSIInternal.h	; setfile -m . $@
GUSIDCon.cp          : GUSIBasics.h GUSIDCon.h GUSIDiag.h GUSIInternal.h	; setfile -m . $@
GUSIDCon.h           : GUSIDevice.h	; setfile -m . $@
GUSIDescriptor.cp    : GUSIBasics.h GUSIDescriptor.h GUSIDiag.h GUSIInternal.h GUSINull.h	; setfile -m . $@
GUSIDescriptor.h     : GUSISocket.h	; setfile -m . $@
GUSIDevice.cp        : GUSIDevice.h GUSIDiag.h GUSIInternal.h GUSIMacFile.h GUSINull.h	; setfile -m . $@
GUSIDevice.h         : GUSIFileSpec.h GUSISocket.h	; setfile -m . $@
GUSIDiag.cp          : GUSIBasics.h GUSIDiag.h GUSIInternal.h	; setfile -m . $@
GUSIFSWrappers.cp    : GUSIContext.h GUSIFSWrappers.h GUSIInternal.h	; setfile -m . $@
GUSIFSWrappers.h     : GUSIFileSpec.h	; setfile -m . $@
GUSIFactory.cp       : GUSIDiag.h GUSIFactory.h GUSIInet.h GUSIInternal.h	; setfile -m . $@
GUSIFactory.h        : GUSISocket.h	; setfile -m . $@
GUSIFileSpec.cp      : GUSIFSWrappers.h GUSIFileSpec.h GUSIInternal.h	; setfile -m . $@
GUSIFileSpec.h       : GUSIBasics.h GUSIContext.h	; setfile -m . $@
GUSIForeignThreads.cp : GUSIContext.h GUSIInternal.h	; setfile -m . $@
GUSIInet.cp          : GUSIInet.h GUSIInternal.h GUSIMTInet.h GUSIOTInet.h GUSIOpenTransport.h	; setfile -m . $@
GUSIInet.h           : GUSIFactory.h	; setfile -m . $@
GUSIMPW.cp           : GUSIBasics.h GUSIConfig.h GUSIDescriptor.h GUSIDevice.h GUSIDiag.h GUSIInternal.h GUSIMPW.h GUSIMacFile.h GUSITimer.h	; setfile -m . $@
GUSIMPWStdio.cp      : GUSIDescriptor.h GUSIInternal.h	; setfile -m . $@
GUSIMSL.cp           : GUSIDescriptor.h GUSIInternal.h GUSIMSL.h	; setfile -m . $@
GUSIMTInet.cp        : GUSIDiag.h GUSIFSWrappers.h GUSIInternal.h GUSIMTInet.h GUSIMTTcp.h GUSIMTUdp.h	; setfile -m . $@
GUSIMTInet.h         : GUSISocket.h GUSISocketMixins.h	; setfile -m . $@
GUSIMTNetDB.cp       : GUSIContext.h GUSIInternal.h GUSIMTInet.h GUSIMTNetDB.h	; setfile -m . $@
GUSIMTNetDB.h        : GUSINetDB.h	; setfile -m . $@
GUSIMTTcp.cp         : GUSIDiag.h GUSIInet.h GUSIInternal.h GUSIMTInet.h GUSIMTNetDB.h GUSIMTTcp.h GUSISocketMixins.h	; setfile -m . $@
GUSIMTTcp.h          : GUSIFactory.h	; setfile -m . $@
GUSIMTUdp.cp         : GUSIDiag.h GUSIInet.h GUSIInternal.h GUSIMTInet.h GUSIMTNetDB.h GUSIMTUdp.h GUSISocketMixins.h	; setfile -m . $@
GUSIMTUdp.h          : GUSIFactory.h	; setfile -m . $@
GUSIMacFile.cp       : GUSIBasics.h GUSIConfig.h GUSIDiag.h GUSIFSWrappers.h GUSIInternal.h GUSIMacFile.h GUSISocketMixins.h	; setfile -m . $@
GUSIMacFile.h        : GUSIDevice.h	; setfile -m . $@
GUSINetDB.cp         : GUSIFactory.h GUSIFileSpec.h GUSIInternal.h GUSINetDB.h	; setfile -m . $@
GUSINetDB.h          : GUSISpecific.h	; setfile -m . $@
GUSINull.cp          : GUSIBasics.h GUSIDiag.h GUSIInternal.h GUSINull.h	; setfile -m . $@
GUSINull.h           : GUSIDevice.h	; setfile -m . $@
GUSIOTInet.cp        : GUSIDiag.h GUSIInet.h GUSIInternal.h GUSIOTInet.h GUSIOTNetDB.h GUSIOpenTransport.h	; setfile -m . $@
GUSIOTNetDB.cp       : GUSIInternal.h GUSIOTInet.h GUSIOTNetDB.h	; setfile -m . $@
GUSIOTNetDB.h        : GUSIContext.h GUSINetDB.h GUSIOpenTransport.h	; setfile -m . $@
GUSIOpenTransport.cp : GUSIDiag.h GUSIInternal.h GUSIOpenTransport.h GUSITimer.h	; setfile -m . $@
GUSIOpenTransport.h  : GUSIFactory.h GUSISocket.h GUSISocketMixins.h	; setfile -m . $@
GUSIPOSIX.cp         : GUSIBasics.h GUSIConfig.h GUSIDescriptor.h GUSIDevice.h GUSIDiag.h GUSIFactory.h GUSIInternal.h GUSINetDB.h GUSIPOSIX.h GUSIPipe.h GUSISocket.h GUSITimer.h	; setfile -m . $@
GUSIPPC.cp           : GUSIBasics.h GUSIBuffer.h GUSIInternal.h GUSIPPC.h GUSISocketMixins.h	; setfile -m . $@
GUSIPPC.h            : GUSIFactory.h GUSISocket.h	; setfile -m . $@
GUSIPThread.cp       : GUSIInternal.h GUSIPThread.h GUSITimer.h	; setfile -m . $@
GUSIPThread.h        : GUSIContext.h GUSIContextQueue.h GUSISpecific.h	; setfile -m . $@
GUSIPipe.cp          : GUSIBasics.h GUSIBuffer.h GUSIInternal.h GUSIPipe.h	; setfile -m . $@
GUSIPipe.h           : GUSIFactory.h GUSISocket.h	; setfile -m . $@
GUSISIOUX.cp         : GUSIBasics.h GUSIDescriptor.h GUSIDevice.h GUSIDiag.h GUSIInternal.h GUSISIOUX.h	; setfile -m . $@
GUSISIOW.cp          : GUSIBasics.h GUSIDescriptor.h GUSIInternal.h GUSISIOW.h	; setfile -m . $@
GUSISfio.cp          : GUSIBasics.h GUSIDescriptor.h GUSIInternal.h	; setfile -m . $@
GUSISignal.cp        : GUSIDiag.h GUSIInternal.h GUSIPThread.h GUSISignal.h GUSITimer.h	; setfile -m . $@
GUSISocket.cp        : GUSIBuffer.h GUSIDiag.h GUSIInternal.h GUSISocket.h	; setfile -m . $@
GUSISocket.h         : GUSIBasics.h GUSIBuffer.h GUSIContext.h GUSIContextQueue.h	; setfile -m . $@
GUSISocketMixins.cp  : GUSIInternal.h GUSISocketMixins.h	; setfile -m . $@
GUSISocketMixins.h   : GUSIBuffer.h GUSISocket.h	; setfile -m . $@
GUSISpecific.cp      : GUSIContext.h GUSIDiag.h GUSIInternal.h GUSISpecific.h	; setfile -m . $@
GUSITimer.cp         : GUSIContext.h GUSIDiag.h GUSIInternal.h GUSITimer.h	; setfile -m . $@
GUSITimer.h          : GUSISpecific.h	; setfile -m . $@
