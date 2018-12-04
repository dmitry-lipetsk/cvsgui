/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

/*
 * MacBinEncoding.cpp --- class to store the mac binary encoding mappings
 */

/* here should be a define, to recorgnize if this file is needed in dev-tools */
#ifdef TARGET_OS_MAC

#include <stdlib.h>

#include <UModalDialogs.h>
#include <LTableMultiGeometry.h>
#include <LOutlineKeySelector.h>
#include <LTableSingleSelector.h>
#include <LOutlineRowSelector.h>
#include <UAttachments.h>
#include <UGAColorRamp.h>
#include <LEditText.h>

#include "MacCvsConstant.h"
#include "MacBinEncoding.h"

MacBinMaps *CMacBinTable::gCurMacBinMap = 0L;

CMacBinTable::CMacBinTable(LStream *inStream) : LOutlineTable(inStream)
{
	// set the table geometry
	LTableMultiGeometry *geom = NEW LTableMultiGeometry(this, 50, 20);
	ThrowIfNil_(geom);
	SetTableGeometry(geom);
	
	// set the table selector
	SetTableSelector(NEW LTableSingleSelector( this )/*LOutlineRowSelector*/ );
	
	// and note that we don't set the table storage....
	
	// most of the table classes not only maintain the graphical
	// representation of your data but also the data itself. But
	// LOutlineTable doesn't really do this...it mostly handles
	// graphical representation... you need to handle your data
	// maintenance elsewhere by yourself.
	
	// insert a couple columns (name and size)
	InsertCols( 4, 0, nil, nil, false );
	geom->SetColWidth(60, 1, 1);
	geom->SetColWidth(100, 2, 2);
	geom->SetColWidth(207, 3, 3);
	geom->SetColWidth(100, 4, 4);

	// Set up keyboard selection and scrolling.
	AddAttachment(NEW LOutlineKeySelector(this, msg_AnyMessage));
	AddAttachment(NEW LKeyScrollAttachment(this));

	// Try to become default commander in the window.
	if (mSuperCommander != nil)
		mSuperCommander->SetLatentSub(this);
}

CMacBinTable::~CMacBinTable()
{
}

void CMacBinTable::FinishCreateSelf()
{
	// insert the root level of the disk into the table
	CMacBinItem *theItem = nil;
	CMacBinItem *lastItem = nil;

	const MACBINMAPENTRIES * entries = gCurMacBinMap->Entries();

	if(entries != 0L) for(int i = 0; i < entries->numMaps; i++)
	{
		// Store this information in the list view
		theItem = NEW CMacBinItem(entries->maps[i]);
		ThrowIfNil_(theItem);
		
		// and insert it at the end of the table
		InsertItem( theItem, nil, lastItem );
		lastItem = theItem;
	}
}

CMacBinItem::CMacBinItem(const MACBINMAP & map) :fMap(map)
{
}

CMacBinItem::~CMacBinItem()
{
}

// this is the routine called to know what to draw within the
// table cell. See the comments in LOutlineItem.cp for more info.
void
CMacBinItem::GetDrawContentsSelf(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
	CPStr desc;

	switch (inCell.col)
	{
	case 1:
	{	ioDrawContents.outShowSelection = true;
		ioDrawContents.outHasIcon = false;
		//ioDrawContents.outIconSuite = ;
		ioDrawContents.outTextTraits.style = 0;
		
		unsigned char *ptr = ioDrawContents.outTextString;
		if ( fMap.sig != 0  &&  fMap.sig != '\?\?\?\?' )
		{
  		*ptr++ = 4;
  		*ptr++ = ((unsigned char *)&fMap.sig)[0];
  		*ptr++ = ((unsigned char *)&fMap.sig)[1];
  		*ptr++ = ((unsigned char *)&fMap.sig)[2];
  		*ptr++ = ((unsigned char *)&fMap.sig)[3];
    }
    else 
    {
			//ioDrawContents.outTextTraits.style = italic;
			//LString::CopyPStr("\pany", ioDrawContents.outTextString);
			ioDrawContents.outTextString[0] = 0;
    }
    }
		break;
  case 2:
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outHasIcon = false;
		ioDrawContents.outTextTraits.style = 0;
		ioDrawContents.outTextString[0] = strlen(fMap.ext);
		if ( ioDrawContents.outTextString[0] )
		{
		  ++ioDrawContents.outTextString[0];
		  ioDrawContents.outTextString[1] = '.';
		  strncpy((char*)&ioDrawContents.outTextString[2], fMap.ext, sizeof(ioDrawContents.outTextString)-2);
    }
    else 
    {
			//ioDrawContents.outTextTraits.style = italic;
			//LString::CopyPStr("\pany", ioDrawContents.outTextString);
			ioDrawContents.outTextString[0] = 0;
    }
    break;
	case 3:
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outTextTraits.style = 0;
		c2pstrcpy(ioDrawContents.outTextString, fMap.desc);
		break;
	case 4:
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outTextTraits.style = 0;
		if(fMap.encod == MAC_PLAIN_BINARY)
		{
			ioDrawContents.outTextTraits.style = bold;
			LString::CopyPStr("\pPlain binary", ioDrawContents.outTextString);
		}
		else if(fMap.useDefault)
		{
			LString::CopyPStr("\pDefault encoding", ioDrawContents.outTextString);
		}
		else if(fMap.encod == MAC_HQX)
		{
			ioDrawContents.outTextTraits.style = bold;
			LString::CopyPStr("\pHQX encoding", ioDrawContents.outTextString);
		}
		else
		{
			ioDrawContents.outTextTraits.style = bold;
			LString::CopyPStr("\pAppleSingle encoding", ioDrawContents.outTextString);
		}
		break;
	}
}

// just to be cute, we'll draw an adornment (again, see the LOutlineItem.cp
// comments for more information). We'll draw a groovy gray background
void CMacBinItem::DrawRowAdornments(const Rect & inLocalRowRect)
{
	ShadeRow(UGAColorRamp::GetColor(0), inLocalRowRect);
}

bool CMacBinItem::EditCell(void)
{
	StDialogHandler	theHandler(dlg_MacBinEntry, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	
	LEditText *typ = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_MacBinType));
	LEditText *ext = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_MacBinExtension));
	LEditText *desc = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_MacBinDesc));

	if ( fMap.sig != 0  &&  fMap.sig != '\?\?\?\?' )
	  typ->SetText((Ptr)&fMap.sig, 4);
	else
	  typ->SetText((Ptr)"", 0);
	LStr255   extension(fMap.ext);
	if ( extension.Length() > 0 ) extension.Insert('.', 0);
	ext->SetText(extension);
	desc->SetText((Ptr)fMap.desc, strlen(fMap.desc));

	OSType  oldsig = fMap.sig;
	char    oldext[MAX_MACBIN_EXTENSION+1];
	strncpy(oldext, fMap.ext, MAX_MACBIN_EXTENSION);
	
	if(fMap.encod == MAC_PLAIN_BINARY)
		theDialog->SetValueForPaneID (item_MacBinPlain, Button_On);
	else if(fMap.useDefault)
		theDialog->SetValueForPaneID (item_MacBinDefault, Button_On);
	else if(fMap.encod == MAC_HQX)
		theDialog->SetValueForPaneID (item_MacBinHQX, Button_On);
	else
		theDialog->SetValueForPaneID (item_MacBinAS, Button_On);
	
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK)
		{
		  bool    dataOK(true);
			char    type[255], extension[255];
			Size    typeLen, extLen;
			
			typ->GetText(type, 254, &typeLen);
			ext->GetText(extension, 254, &extLen);
			
			if( typeLen != 4  &&  typeLen != 0 ) 
			  dataOK = false;			
			else if ( typeLen == 4 )
			{
			  if ( type[0] == ';' || type[1] == ';' || type[2] == ';' || type[3] == ';' )
			    dataOK = false;
			  else if ( ((OSType *)type)[0] == '\?\?\?\?' && extLen == 0 )
			    dataOK = false;
      }
      else if ( typeLen == 0  &&  extLen == 0 )
        dataOK = false;
      else if ( extLen == 1  &&  extension[0] == '.' )
        dataOK = false;
        
      if ( !dataOK )
			{
				SysBeep(10);
				continue;
			}
				
		}
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
	}
	theDialog->Hide();
	
	if(hitMessage == msg_OK)
	{
		char buf[255];
		Size len;
		
		typ->GetText(buf, 254, &len);
		if(len == 4)
		{
			((char *)&fMap.sig)[0] = buf[0];
			((char *)&fMap.sig)[1] = buf[1];
			((char *)&fMap.sig)[2] = buf[2];
			((char *)&fMap.sig)[3] = buf[3];
		}
		desc->GetText(buf, 254, &len);
		buf[len] = '\0';
		strncpy(fMap.desc, buf, MAX_MACBIN_DESC);
    ext->GetText(buf, 254, &len);
		buf[len] = '\0';
    if ( buf[0] == '.' )
		  strncpy(fMap.ext, buf+1, MAX_MACBIN_EXTENSION);
    else
		  strncpy(fMap.ext, buf, MAX_MACBIN_EXTENSION);
		fMap.useDefault = true;

		if(theDialog->GetValueForPaneID (item_MacBinPlain) == Button_On)
		{
			fMap.encod = MAC_PLAIN_BINARY;
			fMap.useDefault = false;
		}
		else if(theDialog->GetValueForPaneID (item_MacBinDefault) == Button_On)
		{
		}
		else if(theDialog->GetValueForPaneID (item_MacBinHQX) == Button_On)
		{
			fMap.useDefault = false;
			fMap.encod = MAC_HQX;
		}
		else
		{
			fMap.useDefault = false;
			fMap.encod = MAC_APPSINGLE;
		}
		
		CMacBinTable *list = dynamic_cast<CMacBinTable*>(mOutlineTable);
		if(list != 0L)
		{
			list->GetCurrentMap()->RemoveEntry(oldsig, oldext);
			list->GetCurrentMap()->AddEntry(fMap.sig, fMap.ext, fMap.desc, fMap.encod, fMap.useDefault);
		}
		return true;
	}
	return false;
}

void CMacBinItem::RemoveCell(void)
{
	CMacBinTable *list = dynamic_cast<CMacBinTable*>(mOutlineTable);
	if(list != 0L)
	{
		list->GetCurrentMap()->RemoveEntry(fMap.sig, NULL);
	}
}

void CMacBinItem::DoubleClick(
	const STableCell&			/* inCell */,
	const SMouseDownEvent&		/* inMouseDown */,
	const SOutlineDrawContents&	/* inDrawContents */,
	Boolean						/* inHitText */)
{
	EditCell();
}

#endif /* TARGET_OS_MAC */
