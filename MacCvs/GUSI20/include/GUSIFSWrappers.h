// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
// % Project	:	GUSI				-	Grand Unified Socket Interface                    
// % File		:	GUSIFSWrappers.nw	-	Pseudo-synchronous file calls             
// % Author	:	Matthias Neeracher                                           
// % Language	:	C++                                                        
// %                                                                       
// % $Log: GUSIFSWrappers.h,v $
// % Revision 1.7  2002/01/10 17:35:51  aubonbeurre
// % Update to GUSI 2.1.9
// %                                           
// % Revision 1.8.2.1  2001/11/07 06:28:50  aubonbeurre                    
// % Port to Carbon and CW7. Mostly the work done for MacPython and MacCvs.
// %                                                                       
// % Revision 1.8  2001/07/23 06:31:37  neeri                              
// % Use PBXGetVolInfoSync to get correct block size for stat() (MacPerl Bug #424874)
// %                                                                       
// % Revision 1.7  2001/01/17 08:45:49  neeri                              
// % Make open calls synchronous                                           
// %                                                                       
// % Revision 1.6  2000/05/23 06:58:04  neeri                              
// % Improve formatting                                                    
// %                                                                       
// % Revision 1.5  2000/01/17 01:41:52  neeri                              
// % Handle special cases in FSMoveRename                                  
// %                                                                       
// % Revision 1.4  1999/08/26 05:45:01  neeri                              
// % Fixes for literate edition of source code                             
// %                                                                       
// % Revision 1.3  1999/07/19 06:21:02  neeri                              
// % Add mkdir/rmdir, fix various file manager related bugs                
// %                                                                       
// % Revision 1.2  1999/05/30 02:16:54  neeri                              
// % Cleaner definition of GUSICatInfo                                     
// %                                                                       
// % Revision 1.1  1998/10/11 16:45:14  neeri                              
// % Ready to release 2.0a2                                                
// %                                                                       
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
//                                                                         
// \chapter{Pseudo-synchronous File System Calls}                          
//                                                                         
// MacOS offers a wide variety of file system APIs, but the most convenient
// of them--the [[FSpXXX]] calls only works synchronously. The routines defined
// here offer a version of these calls, executed asynchronously and embedded
// into the [[GUSIContext]] switching model.                               
//                                                                         
// <GUSIFSWrappers.h>=                                                     
#ifndef _GUSIFSWrappers_
#define _GUSIFSWrappers_

#ifdef GUSI_SOURCE

#include <Files.h>

#include <sys/cdefs.h>

#include "GUSIFileSpec.h"

__BEGIN_DECLS
// <Declarations of C [[GUSIFSWrappers]]>=                                 
#if !TARGET_API_MAC_CARBON
OSErr GUSIFSOpenDriver(StringPtr name, short * refNum);
#endif
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSGetFInfo(const FSSpec * spec, FInfo * info);
OSErr GUSIFSSetFInfo(const FSSpec * spec, const FInfo * info);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSOpenDF(const FSSpec * spec, char permission, short * refNum);
OSErr GUSIFSOpenRF(const FSSpec * spec, char permission, short * refNum);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSGetVolParms(short vRefNum, GetVolParmsInfoBuffer * volParms);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSCreate(const FSSpec * spec, OSType creator, OSType type, ScriptCode script);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSDelete(const FSSpec * spec);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSDirCreate(const FSSpec * spec);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSSetFLock(const FSSpec * spec);
OSErr GUSIFSRstFLock(const FSSpec * spec);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSRename(const FSSpec * spec, ConstStr255Param newname);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSCatMove(const FSSpec * spec, const FSSpec * dest);
// <Declarations of C [[GUSIFSWrappers]]>=                                 
OSErr GUSIFSMoveRename(const FSSpec * spec, const FSSpec * dest);
__END_DECLS
#ifdef __cplusplus
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
OSErr GUSIFSGetCatInfo(GUSIIOPBWrapper<GUSICatInfo> * info);
OSErr GUSIFSSetCatInfo(GUSIIOPBWrapper<GUSICatInfo> * info);
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
OSErr GUSIFSGetFCBInfo(GUSIIOPBWrapper<FCBPBRec> * fcb);
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
#if !TARGET_API_MAC_CARBON
OSErr GUSIFSGetVInfo(GUSIIOPBWrapper<ParamBlockRec> * pb);
#endif
OSErr GUSIFSHGetVInfo(GUSIIOPBWrapper<HParamBlockRec> * pb);
// According to Andreas Grosam, [[PBOpenAsync]] may cause the file not to be closed properly when the
// process quits, so we call that call synchronously.                      
//                                                                         
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
#if !TARGET_API_MAC_CARBON
OSErr GUSIFSOpen(GUSIIOPBWrapper<ParamBlockRec> * pb);
#endif
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
OSErr GUSIFSHGetFInfo(GUSIIOPBWrapper<HParamBlockRec> * pb);
OSErr GUSIFSHSetFInfo(GUSIIOPBWrapper<HParamBlockRec> * pb);
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
OSErr GUSIFSHGetVolParms(GUSIIOPBWrapper<HParamBlockRec> * pb);
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
OSErr GUSIFSCreate(const FSSpec * spec);
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
OSErr GUSIFSCatMove(const FSSpec * spec, long dest);
// Getting the correct allocation block size can be sort of tricky. [[PBHGetVInfoAsync]] returns
// a value tweaked to allow free space calculations on large volumes, so we have to walk the
// fcb queue to get a legitimate value.                                    
//                                                                         
// <Declarations of C++ [[GUSIFSWrappers]]>=                               
OSErr GUSIFSXGetVolInfo(GUSIIOPBWrapper<XVolumeParam> * pb);
#endif

#endif /* GUSI_SOURCE */

#endif /* GUSIFSWrappers */
