# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
# % $Id: Makefile.mk,v 1.1.1.4 2001/04/28 16:10:41 aubonbeurre Exp $               
# %                                                                       
# % Project	:	GUSI 2				-	Grand Unified Socket Interface                  
# % File		:	Makefile			-	Build examples                                   
# % Author	:	Matthias Neeracher		Language	:	dmake                         
# %	                                                                      
# % $Log: Makefile.mk,v $
# % Revision 1.1.1.4  2001/04/28 16:10:41  aubonbeurre
# % Update
# %                                                 
# % Revision 1.24  2001/01/22 04:31:11  neeri                             
# % Last minute changes for 2.1.5                                         
# %                                                                       
# % Revision 1.23  2001/01/17 08:55:37  neeri                             
# % Make LP tools configurable                                            
# %                                                                       
# % Revision 1.22  2000/03/15 07:08:58  neeri                             
# % New yield model                                                       
# %                                                                       
# % Revision 1.21  2000/01/17 01:42:55  neeri                             
# % Update to CW 5.3 command line tools                                   
# %                                                                       
# % Revision 1.20  1999/09/26 03:59:27  neeri                             
# % Releasing 2.0fc1                                                      
# %                                                                       
# % Revision 1.19  1999/08/05 05:57:05  neeri                             
# % Getting ready for 2.0b7                                               
# %                                                                       
# % Revision 1.18  1999/08/02 07:03:54  neeri                             
# % Getting ready for 2.0b6                                               
# %                                                                       
# % Revision 1.17  1999/07/19 06:25:14  neeri                             
# % Getting ready for 2.0b4                                               
# %                                                                       
# % Revision 1.16  1999/06/30 07:42:09  neeri                             
# % Getting ready to release 2.0b3                                        
# %                                                                       
# % Revision 1.15  1999/06/08 04:31:31  neeri                             
# % Getting ready for 2.0b2                                               
# %                                                                       
# % Revision 1.14  1999/05/30 03:09:34  neeri                             
# % Added support for MPW compilers                                       
# %                                                                       
# % Revision 1.13  1999/03/17 09:05:15  neeri                             
# % Added GUSITimer, expanded docs                                        
# %                                                                       
# % Revision 1.12  1998/11/22 23:07:03  neeri                             
# % Releasing 2.0a4 in a hurry                                            
# %                                                                       
# % Revision 1.11  1998/10/25 11:57:40  neeri                             
# % Ready to release 2.0a3                                                
# %                                                                       
# % Revision 1.10  1998/10/11 16:45:26  neeri                             
# % Ready to release 2.0a2                                                
# %                                                                       
# % Revision 1.9  1998/08/01 21:32:13  neeri                              
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
# \documentstyle[noweb]{article}                                          
# \begin{document}                                                        
# \section{Build rules for GUSI Examples}                                 
#                                                                         
#                                                                         
# <Makefile.mk>=                                                          
.INCLUDE : "::GUSIConfig.mk"

# \subsection{General build structure}                                    
#                                                                         
# For some parts of GUSI, you may want to use STLport and/or SFIO. If so, specify their paths here.
#                                                                         
# <Paths to be configured>=                                               
STLportInc	=	-i "{{STLport}}"stl:
SFIO		=  "{{SFIO}}"
# Our build targets are the Metrowerks C/C++ compiler and the standard MPW compilers SCpp and MrC.
#                                                                         
# <Compiler options for GUSI>=                                            
SFIOInc		=	-i $(SFIO)include:
MWCWarn		= 	-w noimplicitconv,nostructclass,nopossible
MWCInc		= 	-convertpaths -nodefaults -i- -i ::include -i "{{CWANSIIncludes}}" -i "{{CIncludes}}" -i ::DCon:Headers: ${SFIOInc} -i ::
MPWInc		= 	-i ::include -i "{{CIncludes}}" -i ::DCon:Headers:  -i :: ${STLportInc}
COpt		=	-d SystemSevenOrLater -d OLDROUTINENAMES=0 -bool on -sym on 
Opt68K		=	-model far -mc68020 -mbg on
OptPPC		=	-tb on
MWCOpt		=	${COpt} ${MWCInc} ${MWCWarn}
MPWOpt		=	${COpt} ${MPWInc} -ER -includes unix -w 2 
C68K		=	MWC68K ${MWCOpt} ${Opt68K}
CPPC		=	MWCPPC ${MWCOpt} ${OptPPC}
CSC			=	SCpp ${MPWOpt} ${Opt68K}
CMRC		=	MrCpp ${SFIOInc} ${MPWOpt} ${OptPPC}
ROptions 	= 	-i :
LOpt		= 	-sym on 
MWLOpt		= 	${LOpt} -xm mpwtool -d
Link68K		=	MWLink68K ${MWLOpt} -model far
LinkPPC		=	MWLinkPPC ${MWLOpt} 
MPWLOpt		=	${LOpt} -c 'MPS ' -t MPST -w
LinkSC		=	Link ${MPWLOpt}  -model far -srtsg all -br 020
LinkMrC		= 	PPCLink ${MPWLOpt}
# \subsection{Libraries used for examples}                                
#                                                                         
#                                                                         
# <Libraries for GUSI tests>=                                             
GUSICore68K	= ::Lib:GUSI_Core.68K.Lib
GUSICorePPC	= ::Lib:GUSI_Core.PPC.Lib
GUSICoreSC	= ::Lib:GUSI_Core.SC.Lib
GUSICoreMrC	= ::Lib:GUSI_Core.MrC.Lib
GUSIMPW68K	= ::Lib:GUSI_MPW.68K.Lib
GUSIMPWPPC	= ::Lib:GUSI_MPW.PPC.Lib
GUSIMPWSC	= ::Lib:GUSI_MPW.SC.Lib
GUSIMPWMrC	= ::Lib:GUSI_MPW.MrC.Lib
GUSIMSL68K	= ::Lib:GUSI_MSL.68K.Lib
GUSIMSLPPC	= ::Lib:GUSI_MSL.PPC.Lib
GUSIStdioSC = ::Lib:GUSI_Stdio.SC.Lib
GUSISfioMrC	= ::Lib:GUSI_Sfio.MrC.Lib
GUSIForward	= ::Lib:GUSI_Forward.68K.Lib
GUSI68K		= GUSIConfig_MTINET.cp.o.68K $(GUSICore68K) $(GUSIMPW68K) $(GUSIForward)
GUSIPPC		= GUSIConfig_MTINET.cp.o.PPC $(GUSICorePPC) $(GUSIMPWPPC)
GUSI68KOT	= GUSIConfig_OTINET.cp.o.68K $(GUSICore68K) $(GUSIMPW68K) $(GUSIForward)
GUSIPPCOT	= GUSIConfig_OTINET.cp.o.PPC $(GUSICorePPC) $(GUSIMPWPPC)
GUSISC		= GUSIConfig_MTINET.cp.o.SC $(GUSICore68K) $(GUSIMPW68K) $(GUSIForward)
GUSIMrC		= GUSIConfig_MTINET.cp.o.MrC $(GUSIMPWMrC) $(GUSIStdioMrC) $(GUSICoreMrC)
GUSISCOT	= GUSIConfig_OTINET.cp.o.SC $(GUSICoreSC) $(GUSIMPWSC) 
GUSIMrCOT	= GUSIConfig_OTINET.cp.o.MrC $(GUSIMPWMrC) $(GUSIStdioMrC) $(GUSICoreMrC)
LibFiles68K	=	\
	"{{MW68KLibraries}}MSL MPWRuntime.68K.Lib"		\
	"{{MW68KLibraries}}MSL Runtime68K.Lib"			\
	"{{MW68KLibraries}}MacOS.Lib"					\
	"{{MW68KLibraries}}MSL C.68K MPW(NL_4i_8d).Lib"	\
	"{{MW68KLibraries}}MSL C++.68K (4i_8d).Lib"		\
	"{{MW68KLibraries}}MathLib68K (4i_8d).Lib"		\
	"{{Libraries}}ToolLibs.far.o"					\
	"{{Libraries}}IntEnv.o"							\
	"{{Libraries}}OpenTransportApp.o"				\
	"{{Libraries}}OpenTransport.o"					\
	"{{Libraries}}OpenTptInet.o"
DCon68K = "::DCon:Libraries:DConCW68K (4i_8d).lib"
			
LibFilesPPC	=	\
	"{{MWPPCLibraries}}MSL MPWCRuntime.Lib"			\
	"{{SharedLibraries}}InterfaceLib"				\
	"{{MWPPCLibraries}}MSL C.PPC MPW(NL).Lib"		\
	"{{MWPPCLibraries}}MSL C++.PPC (NL).Lib"		\
	"{{SharedLibraries}}MathLib"					\
	"{{SharedLibraries}}ThreadsLib"					\
	"{{PPCLibraries}}PPCToolLibs.o"					\
	"{{MWPPCLibraries}}PLStringFuncsPPC.lib"		\
	"{{SharedLibraries}}OpenTransportLib"			\
	"{{SharedLibraries}}OpenTptInternetLib"			\
	"{{PPCLibraries}}OpenTransportAppPPC.o"			\
	"{{PPCLibraries}}OpenTptInetPPC.o"
DConPPC = "::DCon:Libraries:DConCWPPC.lib"

LibFilesSC	=	\
	"{{CLibraries}}CPlusLib.o"						\
	"{{CLibraries}}StdCLib.o"						\
	"{{Libraries}}MacRuntime.o"						\
	"{{Libraries}}Interface.o"						\
	"{{Libraries}}IntEnv.o"							\
	"{{Libraries}}MathLib.o"						\
	"{{Libraries}}ToolLibs.o"						\
	"{{CLibraries}}IOStreams.far.o"					\
	"{{Libraries}}OpenTransport.o"					\
	"{{Libraries}}OpenTransportApp.o"				\
	"{{Libraries}}OpenTptInet.o"
DConSC = "::DCon:Libraries:DCon68K.o"

LibFilesMrC	= 	\
	"$(SFIO)lib:Sfio.MrC.Lib"						\
	"{{PPCLibraries}}MrCPlusLib.o"					\
	"{{PPCLibraries}}PPCStdCLib.o"					\
	"{{PPCLibraries}}StdCRuntime.o"					\
	"{{PPCLibraries}}PPCCRuntime.o"					\
	"{{SharedLibraries}}MathLib"					\
	"{{PPCLibraries}}PPCToolLibs.o"					\
	"{{SharedLibraries}}InterfaceLib"				\
	"{{SharedLibraries}}ThreadsLib"					\
	"{{PPCLibraries}}MrCIOStreams.o"				\
	"{{SharedLibraries}}StdCLib"					\
	"{{SharedLibraries}}OpenTransportLib"			\
	"{{SharedLibraries}}OpenTptInternetLib"			\
	"{{PPCLibraries}}OpenTransportAppPPC.o"			\
	"{{PPCLibraries}}OpenTptInetPPC.o"
DConMrC = "::DCon:Libraries:DConPPC.xcoff"
# Almost all GUSI source files are built from nowebs.                     
#                                                                         
# <Source files for GUSI>=                                                
TESTWEBS	=	\
	TestTimers.nw		TestPipes.nw		TestMTTcp.nw		TestFileSpecs.nw	\
	TestHTGet.nw		TestDuplicate.nw	TestMutex.nw		TestMPWFD.nw		\
	TestMSLTee.nw		TestHttpD.nw		TestDaytime.nw
WEBS		=	$(TESTWEBS)
SOURCES		= 	$(TESTWEBS:s/.nw/.nws/)
# All files in the {\tt web} subdirectory are original. All files in {\tt src} and
# {\tt obj} are derived from nowebs. {\tt include} contains both GUSI headers, which
# are built from nowebs, and BSD 4.4 headers which are included as is.    
#                                                                         
# <Directory locations for GUSI>=                                         
.SOURCE.nw	:	":"
.SOURCE.h	:	"::include"
.SOURCE.c	:	":tangled"
.SOURCE.cp	:	":tangled"
.SOURCE.nws	:	":tangled"
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
	$(TANGLE) '-c++' -L -t -R$*.cp $< > tmp; ::scripts:update-source tmp $(@:s/nws/cp/)
	Set EzDepend 1
	echo > $@
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
%.o.68K		:	%
	$(C68K) -t -ext .o.68K $< -o :Obj:
%.o.PPC		:	%
	$(CPPC) -t -ext .o.PPC $< -o :Obj:
%.o.SC		:	%
	$(CSC)  $< -o $@
%.o.MrC		:	%
	$(CMRC)  $< -o $@
# <Top level rules for GUSI>=                                             
all		:	tests 
.PHONY 	:	source tests
source 	:	tangled $(SOURCES)
	If "{{EzDepend}}" != "" 
		::scripts:EzDepend -f Makefile.mk :tangled:�.cp -i ::include
	End
	Echo > source
tests 	::	source Obj
# We do tests on various levels of abstraction.                           
#                                                                         
#                                                                         
# <Test build rules for GUSI>=                                            
# \subsection{High level (stdio) examples}                                
#                                                                         
# Finally, we test the level of the various stdio libraries.              
#                                                                         
# <Build rules for stdio tests>=                                          
# [[TestHttpD]] implements a toy HTTP daemon using MacTCP.                
#                                                                         
# <Build rules for [[TestHttpD]]>=                                        
tests	::	 TestHttpD.PPC TestHttpD.68K TestHttpD.SC TestHttpD.MrC

TestHttpD.68K : TestHttpD.cp.o.68K $(GUSI68K)
	${Link68K} -o TestHttpD.68K $(DCon68K) $(GUSIMPW68K) $(GUSIForward)  $(GUSIMSL68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestHttpD.cp GUSIConfig_MTINET.cp}.o.68K
TestHttpD.PPC : TestHttpD.cp.o.PPC $(GUSIPPC) 
	${LinkPPC} -o TestHttpD.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSIMSLPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestHttpD.cp GUSIConfig_MTINET.cp}.o.PPC
TestHttpD.SC : TestHttpD.cp.o.SC $(GUSISC) $(GUSIStdioSC)
	${LinkSC} -o TestHttpD.SC $(DConSC) $(GUSIMPWSC) $(GUSIStdioSC) $(GUSICoreSC) $(LibFilesSC) :Obj:{TestHttpD.cp GUSIConfig_MTINET.cp}.o.SC
TestHttpD.MrC : TestHttpD.cp.o.MrC $(GUSIMrC) $(GUSIStdioMrC)
	${LinkMrC} -o TestHttpD.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSISfioMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestHttpD.cp GUSIConfig_MTINET.cp}.o.MrC
# [[TestHttpD_OT]] implements a toy HTTP daemon using Open Transport.     
#                                                                         
# <Build rules for [[TestHttpD_OT]]>=                                     
tests	::	 TestHttpD_OT.PPC TestHttpD_OT.68K TestHttpD_OT.SC TestHttpD_OT.MrC    

TestHttpD_OT.68K : TestHttpD.cp.o.68K $(GUSI68KOT)
	${Link68K} -o TestHttpD_OT.68K $(DCon68K) $(GUSIMPW68K) $(GUSIForward)  $(GUSIMSL68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestHttpD.cp GUSIConfig_OTINET.cp}.o.68K
TestHttpD_OT.PPC : TestHttpD.cp.o.PPC $(GUSIPPCOT) 
	${LinkPPC} -o TestHttpD_OT.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSIMSLPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestHttpD.cp GUSIConfig_OTINET.cp}.o.PPC
TestHttpD_OT.SC : TestHttpD.cp.o.SC $(GUSISCOT) $(GUSIStdioSC)
	${LinkSC} -o TestHttpD_OT.SC $(DConSC) $(GUSIMPWSC) $(GUSIStdioSC) $(GUSICoreSC) $(LibFilesSC) :Obj:{TestHttpD.cp GUSIConfig_OTINET.cp}.o.SC
TestHttpD_OT.MrC : TestHttpD.cp.o.MrC $(GUSIMrCOT) $(GUSIStdioMrC)
	${LinkMrC} -o TestHttpD_OT.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSISfioMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestHttpD.cp GUSIConfig_OTINET.cp}.o.MrC
# [[TestMSLTee]] copies the input data stream to [[stdout]] and a number of other open data streams.
#                                                                         
# <Build rules for [[TestMSLTee]]>=                                       
tests	::	TestMSLTee.PPC TestMSLTee.68K TestMSLTee.SC TestMSLTee.MrC

TestMSLTee.68K : TestMSLTee.cp.o.68K $(GUSI68K) $(GUSIMSL68K) 
	${Link68K} -o TestMSLTee.68K  $(DCon68K) $(GUSIForward) $(GUSIMSL68K) $(LibFiles68K) $(GUSIMPW68K) $(GUSICore68K) :Obj:{TestMSLTee.cp GUSIConfig_MTINET.cp}.o.68K
TestMSLTee.PPC : TestMSLTee.cp.o.PPC $(GUSIPPC) $(GUSIMSLPPC)
	${LinkPPC} -o TestMSLTee.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSIMSLPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestMSLTee.cp GUSIConfig_MTINET.cp}.o.PPC
TestMSLTee.SC : TestMSLTee.cp.o.SC $(GUSISC) $(GUSIStdioSC)
	${LinkSC} -o TestMSLTee.SC $(DConSC) $(GUSIMPWSC) $(GUSIStdioSC) $(GUSICoreSC) $(LibFilesSC) :Obj:{TestMSLTee.cp GUSIConfig_MTINET.cp}.o.SC
TestMSLTee.MrC : TestMSLTee.cp.o.MrC $(GUSIMrC) $(GUSIStdioMrC)
	${LinkMrC} -o TestMSLTee.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSISfioMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestMSLTee.cp GUSIConfig_MTINET.cp}.o.MrC
# \subsection{Low level (file descriptor) examples}                       
#                                                                         
# The next group works on the level of POSIX I/O.                         
#                                                                         
# <Build rules for file descriptor tests>=                                
# [[TestMPWFD]] writes text to the file descriptors 1 and 2.              
#                                                                         
# <Build rules for [[TestMPWFD]]>=                                        
tests	::	TestMPWFD.68K TestMPWFD.PPC TestMPWFD.SC TestMPWFD.MrC

TestMPWFD.68K : TestMPWFD.cp.o.68K $(GUSI68K)
	${Link68K} -o TestMPWFD.68K $(DCon68K) $(GUSIMPW68K) $(GUSIForward) $(LibFiles68K) $(GUSICore68K) :Obj:{TestMPWFD.cp GUSIConfig_MTINET.cp}.o.68K
TestMPWFD.PPC : TestMPWFD.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestMPWFD.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestMPWFD.cp GUSIConfig_MTINET.cp}.o.PPC
TestMPWFD.SC : TestMPWFD.cp.o.SC $(GUSISC)
	${LinkSC} -o TestMPWFD.SC $(DConSC) $(GUSIMPWSC) $(GUSICoreSC) $(LibFilesSC) :Obj:{TestMPWFD.cp GUSIConfig_MTINET.cp}.o.SC
TestMPWFD.MrC : TestMPWFD.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestMPWFD.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestMPWFD.cp GUSIConfig_MTINET.cp}.o.MrC
# [[TestDaytime]] performs an UDP connection to a daytime port and prints the result.
#                                                                         
# <Build rules for [[TestDaytime]]>=                                      
tests	::	TestDaytime.68K TestDaytime.PPC TestDaytime.SC TestDaytime.MrC

TestDaytime.68K : TestDaytime.cp.o.68K $(GUSI68K)
	${Link68K} -o TestDaytime.68K $(DCon68K) $(GUSIMPW68K) $(GUSIForward) $(LibFiles68K) $(GUSICore68K) :Obj:{TestDaytime.cp GUSIConfig_MTINET.cp}.o.68K
TestDaytime.PPC : TestDaytime.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestDaytime.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestDaytime.cp GUSIConfig_MTINET.cp}.o.PPC
TestDaytime.SC : TestDaytime.cp.o.SC $(GUSISC)
	${LinkSC} -o TestDaytime.SC $(DConSC) $(GUSIMPWSC) $(GUSICoreSC) $(LibFilesSC) :Obj:{TestDaytime.cp GUSIConfig_MTINET.cp}.o.SC
TestDaytime.MrC : TestDaytime.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestDaytime.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestDaytime.cp GUSIConfig_MTINET.cp}.o.MrC
# \subsection{C++ API examples}                                           
#                                                                         
# The first group of tests accesses GUSI through the C++ API, leaving the standard
# C/C++ library untouched.                                                
#                                                                         
#                                                                         
# <Build rules for C++ API tests>=                                        
# [[TestMutex]] demonstrates the pthread mutual exclusion mechanisms.     
#                                                                         
# <Build rules for [[TestMutex]]>=                                        
tests	::	TestMutex.68K TestMutex.PPC TestMutex.SC TestMutex.MrC

TestMutex.68K : TestMutex.cp.o.68K $(GUSICore68K)
	${Link68K} -o TestMutex.68K $(DCon68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestMutex.cp.o.68K}
TestMutex.PPC: TestMutex.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestMutex.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestMutex.cp GUSIConfig_MTINET.cp}.o.PPC
TestMutex.SC : TestMutex.cp.o.SC $(GUSISC)
	${LinkSC} -o TestMutex.SC $(DConSC) $(LibFilesSC) $(GUSICoreSC) :Obj:{TestMutex.cp GUSIConfig_MTINET.cp}.o.SC
TestMutex.MrC : TestMutex.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestMutex.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestMutex.cp GUSIConfig_MTINET.cp}.o.MrC
# [[TestTimers]] uses three threads sleeping on drift free timers to print elements in the Hamming sequence.
#                                                                         
# <Build rules for [[TestTimers]]>=                                       
tests	::	TestTimers.68K TestTimers.PPC TestTimers.SC TestTimers.MrC

TestTimers.68K : TestTimers.cp.o.68K $(GUSI68K)
	${Link68K} -o TestTimers.68K $(DCon68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestTimers.cp.o.68K}
TestTimers.PPC : TestTimers.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestTimers.PPC $(DConPPC) $(LibFilesPPC) $(GUSICorePPC) :Obj:{TestTimers.cp.o.PPC}
TestTimers.SC : TestTimers.cp.o.SC $(GUSISC)
	${LinkSC} -o TestTimers.SC $(DConSC) $(LibFilesSC) $(GUSICoreSC) :Obj:{TestTimers.cp GUSIConfig_MTINET.cp}.o.SC
TestTimers.MrC : TestTimers.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestTimers.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestTimers.cp GUSIConfig_MTINET.cp}.o.MrC
# [[TestPipes]] sends a data stream through a pipe.                       
#                                                                         
# <Build rules for [[TestPipes]]>=                                        
tests	::	TestPipes.68K TestPipes.PPC TestPipes.SC TestPipes.MrC

TestPipes.68K : TestPipes.cp.o.68K $(GUSI68K)
	${Link68K} -o TestPipes.68K $(DCon68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestPipes.cp.o.68K}
TestPipes.PPC : TestPipes.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestPipes.PPC $(DConPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestPipes.cp GUSIConfig_MTINET.cp}.o.PPC
TestPipes.SC : TestPipes.cp.o.SC $(GUSISC)
	${LinkSC} -o TestPipes.SC $(DConSC) $(LibFilesSC) $(GUSICoreSC) :Obj:{TestPipes.cp GUSIConfig_MTINET.cp}.o.SC
TestPipes.MrC : TestPipes.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestPipes.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestPipes.cp GUSIConfig_MTINET.cp}.o.MrC
# [[TestMTTcp]] tests TCP sockets by sending a data stream to an echo port and by waiting for a connection
# on port 7000.                                                           
#                                                                         
# <Build rules for [[TestMTTcp]]>=                                        
tests	::	TestMTTcp.68K TestMTTcp.PPC TestMTTcp.SC TestMTTcp.MrC
 
TestMTTcp.68K : TestMTTcp.cp.o.68K $(GUSICore68K)
	${Link68K} -o TestMTTcp.68K $(DCon68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestMTTcp.cp.o.68K}
TestMTTcp.PPC : TestMTTcp.cp.o.PPC $(GUSICorePPC)
	${LinkPPC} -o TestMTTcp.PPC $(DConPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestMTTcp.cp GUSIConfig_MTINET.cp}.o.PPC
TestMTTcp.SC : TestMTTcp.cp.o.SC $(GUSISC)
	${LinkSC} -o TestMTTcp.SC $(DConSC) $(LibFilesSC) $(GUSICoreSC) :Obj:{TestMTTcp.cp GUSIConfig_MTINET.cp}.o.SC
TestMTTcp.MrC : TestMTTcp.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestMTTcp.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestMTTcp.cp GUSIConfig_MTINET.cp}.o.MrC
# [[TestFileSpecs]] lists the contents of the current directory.          
#                                                                         
# <Build rules for [[TestFileSpecs]]>=                                    
tests	::	TestFileSpecs.68K TestFileSpecs.PPC TestFileSpecs.SC TestFileSpecs.MrC

TestFileSpecs.68K : TestFileSpecs.cp.o.68K $(GUSI68K)
	${Link68K} -o TestFileSpecs.68K $(DCon68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestFileSpecs.cp.o.68K}
TestFileSpecs.PPC : TestFileSpecs.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestFileSpecs.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestFileSpecs.cp GUSIConfig_MTINET.cp}.o.PPC
TestFileSpecs.SC : TestFileSpecs.cp.o.SC $(GUSISC)
	${LinkSC} -o TestFileSpecs.SC $(DConSC) $(LibFilesSC) $(GUSICoreSC) :Obj:{TestFileSpecs.cp GUSIConfig_MTINET.cp}.o.SC
TestFileSpecs.MrC : TestFileSpecs.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestFileSpecs.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestFileSpecs.cp GUSIConfig_MTINET.cp}.o.MrC
# [[TestHTGet]] executes a HTTP GET request.                              
#                                                                         
# <Build rules for [[TestHTGet]]>=                                        
tests	::	TestHTGet.68K TestHTGet.PPC TestHTGet.SC TestHTGet.MrC

TestHTGet.68K : TestHTGet.cp.o.68K $(GUSI68K)
	${Link68K} -o TestHTGet.68K $(DCon68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestHTGet.cp GUSIConfig_MTINET.cp}.o.68K
TestHTGet.PPC : TestHTGet.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestHTGet.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestHTGet.cp GUSIConfig_MTINET.cp}.o.PPC
TestHTGet.SC : TestHTGet.cp.o.SC $(GUSISC)
	${LinkSC} -o TestHTGet.SC $(DConSC) $(LibFilesSC) $(GUSICoreSC) :Obj:{TestHTGet.cp GUSIConfig_MTINET.cp}.o.SC
TestHTGet.MrC : TestHTGet.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestHTGet.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestHTGet.cp GUSIConfig_MTINET.cp}.o.MrC
# [[TestHTGet_OT]] executes a HTTP GET request using Open Transport.      
#                                                                         
# <Build rules for [[TestHTGet_OT]]>=                                     
tests	::	TestHTGet_OT.68K TestHTGet_OT.PPC TestHTGet_OT.SC TestHTGet_OT.MrC

TestHTGet_OT.68K : TestHTGet.cp.o.68K $(GUSI68K)
	${Link68K} -o TestHTGet_OT.68K $(DCon68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestHTGet.cp GUSIConfig_OTINET.cp}.o.68K
TestHTGet_OT.PPC : TestHTGet.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestHTGet_OT.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestHTGet.cp GUSIConfig_OTINET.cp}.o.PPC
TestHTGet_OT.SC : TestHTGet.cp.o.SC $(GUSISC)
	${LinkSC} -o TestHTGet.SC $(DConSC) $(LibFilesSC) $(GUSICoreSC) :Obj:{TestHTGet.cp GUSIConfig_OTINET.cp}.o.SC
TestHTGet_OT.MrC : TestHTGet.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestHTGet_OT.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestHTGet.cp GUSIConfig_OTINET.cp}.o.MrC
# [[TestDuplicate]] duplicates a file.                                    
#                                                                         
# <Build rules for [[TestDuplicate]]>=                                    
tests	::	TestDuplicate.68K TestDuplicate.PPC TestDuplicate.SC TestDuplicate.MrC

TestDuplicate.68K : TestDuplicate.cp.o.68K $(GUSI68K)
	${Link68K} -o TestDuplicate.68K $(DCon68K) $(LibFiles68K) $(GUSICore68K) :Obj:{TestDuplicate.cp GUSIConfig_MTINET.cp}.o.68K
TestDuplicate.PPC : TestDuplicate.cp.o.PPC $(GUSIPPC)
	${LinkPPC} -o TestDuplicate.PPC $(DConPPC) $(GUSIMPWPPC) $(GUSICorePPC) $(LibFilesPPC) :Obj:{TestDuplicate.cp GUSIConfig_MTINET.cp}.o.PPC
TestDuplicate.SC : TestDuplicate.cp.o.SC $(GUSISC)
	${LinkSC} -o TestDuplicate.SC $(DConSC) $(LibFilesSC) $(GUSICoreSC) :Obj:{TestDuplicate.cp GUSIConfig_MTINET.cp}.o.SC
TestDuplicate.MrC : TestDuplicate.cp.o.MrC $(GUSIMrC)
	${LinkMrC} -o TestDuplicate.MrC $(DConMrC) $(GUSIMPWMrC) $(GUSICoreMrC) $(LibFilesMrC) :Obj:{TestDuplicate.cp GUSIConfig_MTINET.cp}.o.MrC
# <Object build rules for GUSI>=                                          
Obj :
	NewFolder	Obj

tangled :
	NewFolder	tangled

### Dependences start here. Do not change anything below this line
GUSIBuffer.h         : GUSIBasics.h GUSIDiag.h	; setfile -m . $@
GUSIContext.h        : GUSIBasics.h GUSIContextQueue.h GUSISpecific.h	; setfile -m . $@
GUSIDevice.h         : GUSIFileSpec.h GUSISocket.h	; setfile -m . $@
GUSIFactory.h        : GUSISocket.h	; setfile -m . $@
GUSIFileSpec.h       : GUSIBasics.h GUSIContext.h	; setfile -m . $@
GUSIMTNetDB.h        : GUSINetDB.h	; setfile -m . $@
GUSIMTTcp.h          : GUSIFactory.h	; setfile -m . $@
GUSIMacFile.h        : GUSIDevice.h	; setfile -m . $@
GUSINetDB.h          : GUSISpecific.h	; setfile -m . $@
GUSIPipe.h           : GUSIFactory.h GUSISocket.h	; setfile -m . $@
GUSISocket.h         : GUSIBasics.h GUSIBuffer.h GUSIContext.h GUSIContextQueue.h	; setfile -m . $@
GUSITimer.h          : GUSISpecific.h	; setfile -m . $@
TestDuplicate.cp     : GUSIDevice.h GUSIDiag.h GUSISocket.h	; setfile -m . $@
TestFileSpecs.cp     : GUSIFileSpec.h	; setfile -m . $@
TestHTGet.cp         : GUSIDiag.h GUSIFactory.h GUSIMTNetDB.h GUSIMacFile.h GUSINetDB.h	; setfile -m . $@
TestMTTcp.cp         : GUSIDiag.h GUSIMTTcp.h	; setfile -m . $@
TestPipes.cp         : GUSIPipe.h GUSISocket.h	; setfile -m . $@
TestTimers.cp        : GUSIBasics.h GUSIContext.h GUSITimer.h	; setfile -m . $@
