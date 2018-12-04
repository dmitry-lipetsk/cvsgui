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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

/*
 * 
 */

#include "stdafx.h"

#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "UCvsGraph.h"
#include "UCvsFiles.h"
#include "CvsEntries.h"
#include "AppConsole.h"
#include "LogParse.h"

UIMPLEMENT_DYNAMIC(UCvsGraph, UWidget)

UBEGIN_MESSAGE_MAP(UCvsGraph, UWidget)
	ON_UDESTROY(UCvsGraph)
	ON_UCREATE(UCvsGraph)
	ON_CUSTOM_DRAW(UCvsGraph::kDrawing, UCvsGraph::OnDrawing)
	ON_CUSTOM_CLICK(UCvsGraph::kDrawing, UCvsGraph::OnBtnClick)
	ON_RESIZE(UCvsGraph::kDrawing, UCvsGraph::OnDrawing)
UEND_MESSAGE_MAP()

UCvsGraph::UCvsGraph(CLogNode *node, const char *dir) :
	UWidget(::UEventGetWidID()), m_node(node), m_dir(dir), m_data(0L)
{
	// get the CVS/Entries infos for this file
	CSortList<ENTNODE> entries(200, ENTNODE::Compare);
	if(!Entries_Open (entries, dir))
	{
		cvs_err("Warning : Error while accessing the CVS folder in '%s'\n", dir);
	}
	else
	{
		if(node->GetType() != kNodeHeader)
		{
			cvs_err("Warning : Top node is not a RCS file description !\n");
		}
		else
		{
			const char *name = (**(CLogNodeHeader *)node).WorkingFile().c_str();
			struct stat sb;
			if (stat(name, &sb) == -1)
			{
				cvs_err("Warning : Unknown file '%s'\n", name);
			}
			else
			{
				m_data = Entries_SetVisited(dir, entries, name, sb, false);
				m_data->Ref();
				m_name = name;
			}
		}
	}
}

UCvsGraph::~UCvsGraph()
{
	if(m_node != 0L)
		delete m_node;
	if(m_data != 0L)
		m_data->UnRef();
}

void UCvsGraph::OnDestroy(void)
{
	delete this;
}

void UCvsGraph::OnCreate(void)
{
	std::string title("Graph log : ");
	title += m_name;
	UEventSendMessage(GetWidID(), EV_SETTEXT, kUMainWidget, (void *)(const char *)title.c_str());
}

USize CWinLogHeaderData::GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo)
{
	CLogNodeHeader & header = *(CLogNodeHeader *)m_node;
	return hdc.GetTextExtent((*header).WorkingFile().c_str(),
		(*header).WorkingFile().length());
}

static void DrawNode(UDC & hdc, const URect & selfB,
					 UCOLORREF contour, UCOLORREF shadowc,
					 bool isRounded, bool issel)
{
	UPen *pOldPen = 0L;
	UBrush *pOldBrush = 0L;
	UPen pen, pen2;
	UBrush brush;
	UCOLORREF selcolor = URGB(160, 160, 160);
	UCOLORREF wcolor = URGB(255, 255, 255);

	try
	{
		if(!isRounded)
		{
			if(!issel)
			{
				brush.CreateSolidBrush(wcolor);
				pOldBrush = hdc.SelectObject(&brush);
				pen.CreatePen(UPS_SOLID, 1, contour);
				pOldPen = hdc.SelectObject(&pen);
				hdc.Rectangle(selfB);
			}
			else
			{
				brush.CreateSolidBrush(selcolor);
				pOldBrush = hdc.SelectObject(&brush);
				pen.CreatePen(UPS_SOLID, 1, contour);
				pOldPen = hdc.SelectObject(&pen);
				hdc.Rectangle(selfB);
			}
		}
		else
		{
			URect shadow;
			if(!issel)
			{
				brush.CreateSolidBrush(shadowc);
				pOldBrush = hdc.SelectObject(&brush);
				pen.CreatePen(UPS_SOLID, 1, shadowc);
				pOldPen = hdc.SelectObject(&pen);
				shadow = selfB;
				shadow.OffsetRect(UPoint(2, 2));
				hdc.RoundRect(&shadow, UPoint(6, 6));

				brush.DeleteObject();
				brush.CreateSolidBrush(wcolor);
				hdc.SelectObject(&brush);
				pen2.CreatePen(UPS_SOLID, 1, contour);
				hdc.SelectObject(&pen2);
				hdc.RoundRect(selfB, UPoint(6, 6));
			}
			else
			{
				brush.CreateSolidBrush(shadowc);
				pOldBrush = hdc.SelectObject(&brush);
				pen.CreatePen(UPS_SOLID, 1, shadowc);
				pOldPen = hdc.SelectObject(&pen);
				shadow = selfB;
				shadow.OffsetRect(UPoint(2, 2));
				hdc.RoundRect(&shadow, UPoint(6, 6));

				brush.DeleteObject();
				brush.CreateSolidBrush(selcolor);
				hdc.SelectObject(&brush);
				pen2.CreatePen(UPS_SOLID, 1, contour);
				hdc.SelectObject(&pen2);
				hdc.RoundRect(selfB, UPoint(6, 6));
			}
		}

		if (pOldPen != 0L)
		{
			hdc.SelectObject(pOldPen);
			pOldPen = 0L;
		}
		if (pOldBrush != 0L)
		{
			hdc.SelectObject(pOldBrush);
			pOldBrush = 0L;
		}

		pen.DeleteObject();
		pen2.DeleteObject();
		brush.DeleteObject();
	}
	catch(...)
	{
		if (pOldPen != 0L)
			hdc.SelectObject(pOldPen);
		if (pOldBrush != 0L)
			hdc.SelectObject(pOldBrush);
	}
}

void CWinLogHeaderData::UpdateSelf(UDC & hdc, EntnodeData *entryInfo)
{
	// draw the node
	::DrawNode(hdc, selfB, URGB(255, 0, 0), URGB(120, 0, 0), true, sel);

	// draw the content of the node
	UPoint center = selfB.CenterPoint();
	int nCenterX = center.x;
	int nCenterY = center.y - hdc.GetTextHeight()/2;
	CLogNodeHeader & header = *(CLogNodeHeader *)m_node;
	hdc.TextOutClipped(nCenterX, nCenterY, &selfB,
		(*header).WorkingFile().c_str(), (*header).WorkingFile().length());
}

USize CWinLogRevData::GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo)
{
	CLogNodeRev & rev = *(CLogNodeRev *)m_node;
	USize size = hdc.GetTextExtent((*rev).RevNum().c_str(),
		strlen((*rev).RevNum().c_str()));
	if(IsDiskNode(entryInfo))
	{
		size.cx += 2 + 16 + 2;
		size.cy = std::max(16, size.cy);
	}
	return size;
}

void CWinLogRevData::UpdateSelf(UDC & hdc, EntnodeData *entryInfo)
{
	URect txtRect = selfB;

	if(IsDiskNode(entryInfo))
	{
		txtRect.left += 2 + 16 + 2;
	}

	// draw the node
	::DrawNode(hdc, selfB, URGB(0, 0, 255), URGB(0, 0, 0), false, sel);

	// draw the state icon if it is the rev. of the disk
	if(IsDiskNode(entryInfo))
	{
		UPoint w(selfB.TopLeft());
		w.x += 2;
		w.y += (selfB.Height() - 16) / 2;
		hdc.DrawBitmap(UCvsFiles::GetImageForEntry(entryInfo), w);
	}

	// draw the content of the node
	UPoint center = txtRect.CenterPoint();
	int nCenterX = center.x;
	int nCenterY = center.y - hdc.GetTextHeight()/2;
	CLogNodeRev & rev = *(CLogNodeRev *)m_node;
	hdc.TextOutClipped(nCenterX, nCenterY, &txtRect,
			(*rev).RevNum().c_str(), strlen((*rev).RevNum().c_str()));
}

USize CWinLogTagData::GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo)
{
	CLogNodeTag & tag = *(CLogNodeTag *)m_node;
	return hdc.GetTextExtent((*tag).c_str(), (*tag).length());
}

void CWinLogTagData::UpdateSelf(UDC & hdc, EntnodeData *entryInfo)
{
	// draw the node
	::DrawNode(hdc, selfB, URGB(0, 0, 0), URGB(120, 120, 120), true, sel);

	// draw the content of the node
	UPoint center = selfB.CenterPoint();
	int nCenterX = center.x;
	int nCenterY = center.y - hdc.GetTextHeight()/2;
	CLogNodeTag & tag = *(CLogNodeTag *)m_node;
	hdc.TextOutClipped(nCenterX, nCenterY, &selfB,
		(*tag).c_str(), (*tag).length());
}

USize CWinLogBranchData::GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo)
{
	CLogNodeBranch & branch = *(CLogNodeBranch *)m_node;
	return hdc.GetTextExtent((*branch).c_str(), (*branch).length());
}

void CWinLogBranchData::UpdateSelf(UDC & hdc, EntnodeData *entryInfo)
{
	// draw the node
	::DrawNode(hdc, selfB, URGB(0, 0, 255), URGB(0, 0, 120), true, sel);

	// draw the content of the node
	UPoint center = selfB.CenterPoint();
	int nCenterX = center.x;
	int nCenterY = center.y - hdc.GetTextHeight()/2;
	CLogNodeBranch & branch = *(CLogNodeBranch *)m_node;
	hdc.TextOutClipped(nCenterX, nCenterY, &selfB,
		(*branch).c_str(), (*branch).length());
}

CWinLogData::CWinLogData(CLogNode *node) : CLogNodeData(node)
{
	selfB.SetRectEmpty();
	totB.SetRectEmpty();
	sel = false;
}

CWinLogData::~CWinLogData()
{
}

const char *CWinLogData::GetStr(void)
{
	switch(GetType())
	{
		case kNodeHeader :
		{
			CLogNodeHeader & header = *(CLogNodeHeader *)m_node;
			return (*header).WorkingFile().c_str();
			break;
		}
		case kNodeBranch :
		{
			CLogNodeBranch & branch = *(CLogNodeBranch *)m_node;
			return (*branch).c_str();
			break;
		}
		case kNodeRev :
		{
			CLogNodeRev & rev = *(CLogNodeRev *)m_node;
			return (*rev).RevNum().c_str();
			break;
		}
		case kNodeTag :
		{
			CLogNodeTag & tag = *(CLogNodeTag *)m_node;
			return (*tag).c_str();
			break;
		}
	}
	return 0L;
}

CWinLogData * CWinLogData::CreateNewData(CLogNode *node)
{
	if(node->GetUserData() != 0L)
		return (CWinLogData *)node->GetUserData();

	CWinLogData *res = 0L;
	switch(node->GetType())
	{
	case kNodeHeader :
		res = new CWinLogHeaderData(node);
		break;
	case kNodeBranch :
		res = new CWinLogBranchData(node);
		break;
	case kNodeRev :
		res = new CWinLogRevData(node);
		break;
	case kNodeTag :
		res = new CWinLogTagData(node);
		break;
	}
	node->SetUserData(res);
	return res;
}

bool CWinLogData::IsDiskNode(EntnodeData *entryInfo)
{
	if(entryInfo == 0L || GetType() != kNodeRev)
		return false;
	const char *vn = ((*entryInfo)[EntnodeFile::kVN]).c_str();
	if(vn == 0L)
		return false;

	return strcmp(vn, (**(CLogNodeRev *)m_node).RevNum().c_str()) == 0;
}

const int kVChildSpace = 8;
const int kHChildSpace = 40;
const int kInflateNodeSpace = 8;

void CWinLogData::Offset(UPoint o)
{
	selfB.OffsetRect(o);
	totB.OffsetRect(o);
	std::vector<CLogNode *>::iterator i;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		CLogNode *subNode = *i;
		GetData(subNode)->Offset(o);
	}
	if(m_node->Next() != 0L)
		GetData(m_node->Next())->Offset(o);
}


void CWinLogData::SetSelected(CScrollView *view, bool state)
{
	sel = state;

	URect invalR = selfB;
	invalR.OffsetRect(-view->GetDeviceScrollPosition());
	view->InvalidateRect(&invalR);
}

void CWinLogData::UnselectAll(CScrollView *view)
{
	if(sel)
	{
		if(view != NULL)
			SetSelected(view, false);
		else
			SetSelected(false);
	}

	std::vector<CLogNode *>::iterator i;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		CLogNode *subNode = *i;
		GetData(subNode)->UnselectAll(view);
	}
	if(m_node->Next() != 0L)
		GetData(m_node->Next())->UnselectAll(view);
}

void CWinLogData::UnselectAll(void)
{
	CWinLogData::UnselectAll(NULL);
}

CWinLogData *CWinLogData::HitTest(UPoint point)
{
	if(!totB.PtInRect(point))
		return 0L;
	if(selfB.PtInRect(point))
		return this;

	CWinLogData *result;
	std::vector<CLogNode *>::iterator i;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		CLogNode *subNode = *i;
		result = GetData(subNode)->HitTest(point);
		if(result != 0L)
			return result;
	}
	if(m_node->Next() != 0L)
	{
		result = GetData(m_node->Next())->HitTest(point);
		if(result != 0L)
			return result;
	}
	return 0L;
}

void CWinLogData::Update(UDC & hdc, EntnodeData *entryInfo)
{
	CLogNode *subNode;
	CLogNode *nextNode = m_node->Next();
	UPen *pOldPen = 0L;
	UPen pen, pen2;

	UpdateSelf(hdc, entryInfo);

	try
	{
		pen.CreatePen(UPS_SOLID, 2, URGB(0, 0, 255));
		pen2.CreatePen(UPS_SOLID, 1, URGB(0, 0, 0));
		pOldPen = hdc.SelectObject(&pen);

		int topY = selfB.TopLeft().y + 2;
		int botY = selfB.BottomRight().y - 2;
		int lefX = selfB.BottomRight().x + 4;
		int count = 1;
		int tot = m_node->Childs().size() + 1;

		// draw the children
		std::vector<CLogNode *>::iterator i;
		for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i, ++count)
		{
			subNode = *i;
			
			// draw the link
			URect & cb = GetData(subNode)->SelfBounds();
			if(subNode->GetType() == kNodeRev || subNode->GetType() == kNodeBranch)
				hdc.SelectObject(&pen);
			else
				hdc.SelectObject(&pen2);
			float y = (float)topY + (float)(botY - topY) *
				(float)count / (float)tot;
			int rigX = cb.TopLeft().x - 4;
			float x = (float)rigX - (float)(rigX - lefX) *
				(float)count / (float)tot;
			//int halfX = (cb.TopLeft().x - selfB.BottomRight().x) / 2;
			hdc.MoveTo(selfB.BottomRight().x, (int)y);
			hdc.LineTo((int)x, (int)y);
			hdc.LineTo((int)x, cb.CenterPoint().y);
			hdc.LineTo(cb.TopLeft().x, cb.CenterPoint().y);
			
			// draw the sub-node
			GetData(subNode)->Update(hdc, entryInfo);
		}

		// draw the next node
		if(nextNode != 0L)
		{
			// draw the link
			URect & nb = GetData(nextNode)->SelfBounds();
			if(nextNode->GetType() == kNodeRev || nextNode->GetType() == kNodeBranch)
				hdc.SelectObject(&pen);
			else
				hdc.SelectObject(&pen2);
			int centerX = selfB.Width() < nb.Width() ?
				selfB.CenterPoint().x : nb.CenterPoint().x;
			hdc.MoveTo(centerX, selfB.BottomRight().y);
			hdc.LineTo(centerX, nb.TopLeft().y);
			
			// draw the next node
			GetData(nextNode)->Update(hdc, entryInfo);
		}

		if (pOldPen != 0L)
		{
			hdc.SelectObject(pOldPen);
			pOldPen = 0L;
		}

		pen.DeleteObject();
		pen2.DeleteObject();
	}
	catch(...)
	{
		if (pOldPen != 0L)
			hdc.SelectObject(pOldPen);
	}
}

void CWinLogData::ComputeBounds(const UPoint & topLeft, UDC & hdc,
								EntnodeData *entryInfo)
{
	CLogNode *subNode;
	CLogNode *nextNode = m_node->Next();

	// first compute childrens bounds
	std::vector<CLogNode *>::iterator i;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		subNode = *i;
		CWinLogData::CreateNewData(subNode);
		GetData(subNode)->ComputeBounds(topLeft, hdc, entryInfo);
	}
	if(nextNode != 0L)
		CWinLogData::CreateNewData(nextNode);

	// compute self place
	selfB.SetRectEmpty();
	USize size = GetBoundsSelf(hdc, entryInfo);
	size.cx += kInflateNodeSpace;
	size.cy += kInflateNodeSpace;
	selfB.SetRect(0, 0, size.cx, size.cy);

	// offset to the place assigned to this node
	selfB.OffsetRect(topLeft - selfB.TopLeft());

	// calculate the total height of the childrens
	int vSize = 0;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		subNode = *i;
		URect & b = GetData(subNode)->Bounds();
		vSize += b.Height();
	}
	if(!m_node->Childs().empty())
		vSize += (m_node->Childs().size() - 1) * kVChildSpace;

	// offset the children relative to self
	UPoint startChilds(topLeft.x + selfB.Width() + kHChildSpace,
		/*selfB.CenterPoint().y - vSize / 2*/topLeft.y);
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		subNode = *i;
		URect curPos = GetData(subNode)->Bounds();
		GetData(subNode)->Offset(startChilds - curPos.TopLeft());
		startChilds.y += curPos.Height() + kVChildSpace;
	}

	// calculate the total bounds of the childrens
	URect bc;
	bc.SetRectEmpty();
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		subNode = *i;
		URect & b = GetData(subNode)->Bounds();
		bc.UnionRect(&bc, &b);
	}

	// now we got the complete size
	totB.UnionRect(&selfB, &bc);

	if(nextNode != 0L)
	{
		UPoint nextTopLeft;
		nextTopLeft.x = totB.TopLeft().x;
		nextTopLeft.y = totB.BottomRight().y + kVChildSpace;
		GetData(nextNode)->ComputeBounds(nextTopLeft, hdc, entryInfo);

		totB.UnionRect(&totB, &GetData(nextNode)->Bounds());
	}
}

void UCvsGraph::CalcImageSize(void)
{
	if(m_node == 0L)
		return;

	// set all the bounds locations
	UDC dc(GetWidID(), kDrawing);
	CWinLogData *data = CWinLogData::CreateNewData(m_node);
	m_node->SetUserData(data);
	UPoint start(5, 5);
	data->ComputeBounds(start, dc, m_data);

	// reoffset from (0, 0)
	URect bounds = data->Bounds();
	data->Offset(start - bounds.TopLeft());
	bounds = data->Bounds();

#if qGTK
	GtkWidget *layout = (GtkWidget *)GetWidget(kDrawing);
	gtk_drawing_area_size(GTK_DRAWING_AREA(layout), bounds.Width()+5, bounds.Height()+5);
#endif
#if 0
	// set the scroll size
	SetScrollSizes(MM_TEXT, bounds.Size() + USize(5, 5));
#endif
}

void UCvsGraph::OnDrawing(void *info)
{
#if qGTK
	GdkEventExpose *event = (GdkEventExpose *)info;
	GtkDrawingArea *drawing = GTK_DRAWING_AREA(GetWidget(kDrawing));
	gdk_window_clear_area (drawing->widget.window,
						   event->area.x, event->area.y,
						   event->area.width, event->area.height);
	//fprintf(stderr,"Area: x: %d, y:%d, width: %d, height:%d\n", 
	//		event->area.x, event->area.y, event->area.width, event->area.height);
#endif
	CalcImageSize();

	if(m_node == 0L || m_node->GetUserData() == 0L) 
		return;
		
	OnBlockHandler(drawing, kDrawing, GDK_EXPOSE);
		
	UDC dc(GetWidID(), kDrawing);
	CWinLogData *data = (CWinLogData *)m_node->GetUserData();
	data->Update(dc, m_data);

	while (gtk_events_pending())
          gtk_main_iteration();
	
	OnUnblockHandler(drawing, kDrawing, GDK_EXPOSE);
		
	/*	GtkWidget *widget = (GtkWidget *)GetWidget(kDrawing);
	if(widget == 0L)
		return;

	GtkDrawingArea *darea;
	GdkDrawable *drawable;
	GdkGC *black_gc;
	GdkGC *gray_gc;
	GdkGC *white_gc;
	guint max_width;
	guint max_height;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (GTK_IS_DRAWING_AREA (widget));
	
	darea = GTK_DRAWING_AREA (widget);
	drawable = widget->window;
	white_gc = widget->style->white_gc;
	gray_gc = widget->style->bg_gc[GTK_STATE_NORMAL];
	black_gc = widget->style->black_gc;
	max_width = widget->allocation.width;
	max_height = widget->allocation.height;
	gdk_draw_rectangle (drawable, black_gc,
						TRUE,
						0,
						max_height / 2,
						max_width,
						max_height / 2);*/
}

void UCvsGraph::OnBtnClick(void *info)
{
	GdkEventButton *event = (GdkEventButton *)info;
	UPoint point((int)event->x, (int)event->y);
	int nFlags = event->state;    
	CWinLogData *data = (CWinLogData *)m_node->GetUserData();
	UDC dc(GetWidID(), kDrawing);
		
	//fprintf(stderr,"x: %f, y:%f, x_root: %f, y_root:%f, state:%d, button:%d\n", 
	//				event->x, event->y, event->x_root, event->y_root, event->state, event->button);
	
	if(event->button & UK_LBUTTON ) {
		if((nFlags & GDK_SHIFT_MASK) == 0 && (nFlags & GDK_CONTROL_MASK) == 0)
			data->UnselectAll();
		
		CWinLogData *hit = data->HitTest(point);

	 	if(hit != 0L) {
			if((nFlags & GDK_SHIFT_MASK) == 0 && (nFlags & GDK_CONTROL_MASK) == 0) {
				if(!hit->Selected()) {
					CColorConsole out;
				
					hit->SetSelected(true);
									
					CvsLogOutput(out, hit->Node());					  
				}
			} else {
				hit->SetSelected(!hit->Selected());
			}
			
			data->Update(dc, m_data);
		}
	}
}
