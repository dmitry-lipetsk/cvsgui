//################################################################################
//
//  Copyright (C) 1997-1998 Netscape Communications Corporation.
// 
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//  02111-1307, USA.
//
//
//		File: 		UProcessUtilities.cp
//
//	  Authors: 	Simon Fraser
//
//################################################################################

#include <LString.h>
#include "UProcessUtilities.h"

//-----------------------LocateApplicationBySig--------------------------

Boolean UProcessUtils::LocateApplicationBySig(OSType inAppCreator, FSSpec &outSpec)
	{
		FSSpec 							newestAppSpec;
		UInt32 							newestAppDate = 0;
		TString<Str63> 			theAppName;
		SInt16 							theIndex = 0;
		Boolean 						foundApp = false;
		
		HParamBlockRec 			hpb;

		do {
			DTPBRec			dtParams = {0};		//C magic; zeros out the whole thing

			hpb.volumeParam.ioVolIndex = ++theIndex;
			if (PBHGetVInfoSync(&hpb) != noErr)
				break;

			dtParams.ioVRefNum = hpb.volumeParam.ioVRefNum;
			if (PBDTGetPath(&dtParams) == noErr)
				{
					dtParams.ioFileCreator = inAppCreator;
					dtParams.ioNamePtr = theAppName;

					if (PBDTGetAPPLSync(&dtParams) == noErr)
						{
							CInfoPBRec 	catInfoParams = {0};
							
							catInfoParams.hFileInfo.ioNamePtr = theAppName;
							catInfoParams.hFileInfo.ioVRefNum = dtParams.ioVRefNum;
							catInfoParams.hFileInfo.ioDirID = dtParams.ioAPPLParID;

							if ((PBGetCatInfoSync(&catInfoParams) == noErr) && (catInfoParams.hFileInfo.ioFlMdDat > newestAppDate))
								{
									newestAppSpec.vRefNum = dtParams.ioVRefNum;
									newestAppSpec.parID = dtParams.ioAPPLParID;
									::BlockMoveData(&theAppName[0], newestAppSpec.name, theAppName.Length() + 1);
									newestAppDate = catInfoParams.hFileInfo.ioFlMdDat;
								}
								
							foundApp = true;
						}
				}
			}
		while (!hpb.volumeParam.ioResult);

		if (foundApp)
			outSpec = newestAppSpec;

		return foundApp;
	}

//-----------------------IsApplicationRunning--------------------------

Boolean UProcessUtils::IsApplicationRunning( OSType inAppCreator, ProcessSerialNumber &outPSN)
	{
		Boolean isRunning = false;
		//	spin through the process list to determine whether there's a
		//	process with our desired signature.
		outPSN.highLongOfPSN = 0;
		outPSN.lowLongOfPSN = 0;

		ProcessInfoRec info = {0};	//	information from the process list.
		info.processInfoLength = sizeof(ProcessInfoRec);

		while ((GetNextProcess(&outPSN) == noErr) && (GetProcessInformation(&outPSN, &info) == noErr))
			{
				if (info.processSignature == inAppCreator)
					{
						isRunning = true;
						break;
					}
			}

		return isRunning;
	}

//-----------------------LaunchApplication--------------------------

OSErr UProcessUtils::LaunchApplication( FSSpec &inAppSpec, UInt16 inLaunchFlags, ProcessSerialNumber &outPSN)
	{
		LaunchParamBlockRec launchParams;

		launchParams.launchBlockID = extendedBlock;
		launchParams.launchEPBLength = extendedBlockLen;
		launchParams.launchFileFlags = 0;
		launchParams.launchControlFlags = inLaunchFlags;
		launchParams.launchAppSpec = &inAppSpec;
		launchParams.launchAppParameters = 0L;

		OSErr err = ::LaunchApplication(&launchParams);
		outPSN = launchParams.launchProcessSN;
		return err;
	}


//-----------------------SendAppToFront--------------------------

void UProcessUtils::SendAppToFront(const ProcessSerialNumber &inWhichProcess)
	{
		EventRecord 	dummyEvent;

		::SetFrontProcess(&inWhichProcess);
		::EventAvail(nullEvent, &dummyEvent);		//make it happen
	}


//-----------------------IsFrontProcess--------------------------

Boolean UProcessUtils::IsFrontProcess(const ProcessSerialNumber& inWhichProcess)
	{
		ProcessSerialNumber frontProcess;
		::GetFrontProcess(&frontProcess);

		Boolean inFront;
		::SameProcess(&frontProcess, &inWhichProcess, &inFront);

		return inFront;
	}

