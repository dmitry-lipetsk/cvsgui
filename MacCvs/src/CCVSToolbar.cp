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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- October 2002
 */

/*
 * CCVSToolbar.cp --- toolbar for MacCvs
 */

#include "CCVSToolbar.h"

TCVSToolbar::TCVSToolbar(LStream* inStream) : LWindow(inStream), fWindowMovie(0L),
	fOldPort(0L), fOldGD(0L), fPlay(0L), fStart(false), fCanClose(true)
{
}

TCVSToolbar::~TCVSToolbar()
{
}

void TCVSToolbar::AttemptClose()
{
	if ( fCanClose )
	{
  	if(fPlay != 0L)
  	{
  		::DisposeMovieController(fPlay);
  		fPlay = 0L;
  	}

  	if(fWindowMovie != 0L)
  	{
  		::SetMovieActive(fWindowMovie, false);
  		::SetMovieGWorld(fWindowMovie, fOldPort, fOldGD);
  		::DisposeMovie(fWindowMovie);
  		fWindowMovie = 0L;
  	}
  	
  	LWindow::AttemptClose();
  }
  else this->Hide();
}

void TCVSToolbar::DrawSelf()
{
	LWindow::DrawSelf();

	if(fPlay == 0L || !fStart)
		return;

	::MCDraw(fPlay, GetMacWindow());
	::UpdateMovie(fWindowMovie);
}

void TCVSToolbar::Start(void)
{
	StartIdling(40 * kEventDurationMillisecond);

	fStart = false;
	
	if(fPlay == 0L || !FocusDraw())
		return;

	fStart = true;

	::GoToBeginningOfMovie(fWindowMovie);

	ComponentResult res = MCDoAction(fPlay, mcActionPlay, (Ptr)(1<<0));
	ThrowIfOSStatus_(res);
}

void TCVSToolbar::Stop(void)
{
	StopIdling();

	fStart = false;

	if(fPlay == 0L || !FocusDraw())
		return;

	LView *movieView = dynamic_cast<LView *>(FindPaneByID(kMovieItem));
	ThrowIfNil_(movieView);

	ComponentResult res = MCDoAction(fPlay, mcActionPlay, (Ptr)0);
	ThrowIfOSStatus_(res);

	Refresh();
}

void TCVSToolbar::SpendTime(
								const EventRecord&	inMacEvent)
{
	if(fPlay == 0L || !fStart || !FocusDraw())
		return;

	::MoviesTask(fWindowMovie, 0);

	ComponentResult res = ::MCIdle(fPlay);
	ThrowIfOSStatus_(res);
}

void TCVSToolbar::HaveMovie(Movie moov)
{
	if(!FocusDraw())
		return;
	
	// save the movie
	fWindowMovie = moov;
	::GetMovieGWorld(fWindowMovie, &fOldPort, &fOldGD);

	// adjust the size of the window to match the movie
	LView *movieView = dynamic_cast<LView *>(FindPaneByID(kMovieItem));
	ThrowIfNil_(movieView);

	SPoint32 outLocation;
	SDimension16 outSize;

 	movieView->GetFrameLocation(outLocation);
	movieView->GetFrameSize(outSize);
	Rect movieItemRect;
	::SetRect(&movieItemRect, 0, 0, outSize.width, outSize.height);
	::OffsetRect(&movieItemRect, outLocation.h, outLocation.v);
	
	Rect movieBounds;
	::GetMovieBox(fWindowMovie, &movieBounds);
	
	// now set the movie rect to match the item rect
	::StopMovie(fWindowMovie);
	::GoToBeginningOfMovie(fWindowMovie);
	::PrerollMovie(fWindowMovie, 0, (Fixed)(1 << 24));
	::OffsetRect(&movieBounds,
		-movieBounds.left + movieItemRect.left + (movieItemRect.right - movieItemRect.left - movieBounds.right + movieBounds.left) / 2,
		-movieBounds.top + movieItemRect.top + (movieItemRect.bottom - movieItemRect.top - movieBounds.bottom + movieBounds.top) / 2);
	::SetMovieBox(fWindowMovie, &movieBounds);

	// create the controller
	fPlay = ::NewMovieController(fWindowMovie, &movieItemRect, mcTopLeftMovie + mcNotVisible);
	if(fPlay == 0L)
		return;

	ComponentResult res;
	res = ::MCSetControllerPort(fPlay, (CGrafPtr)GetMacPort());
	ThrowIfOSStatus_(res);
	res = ::MCSetControllerAttached(fPlay, false);
	ThrowIfOSStatus_(res);
	res = ::MCSetMovie(fPlay, fWindowMovie, GetMacWindow(), topLeft(movieBounds));
	ThrowIfOSStatus_(res);
	res = MCDoAction(fPlay, mcActionSetLooping, (Ptr)true);
	ThrowIfOSStatus_(res);
}

