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

// GraphView.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "GraphView.h"
#include "GraphDoc.h"
#include "AppConsole.h"
#include "LogParse.h"
#include "CvsEntries.h"
#include "BrowseFileView.h"
#include "WinCvsBrowser.h"
#include "CvsArgs.h"
#include "WincvsView.h"
#include "CvsPrefs.h"
#include "PromptFiles.h"
#include "FileTraversal.h"
#include "ProgressDlg.h"
#include "GraphOptDlg.h"
#include "AdminOptionDlg.h"
#include "CvsAlert.h"
#include "CvsCommands.h"
#include "MultiFiles.h"
#include "BrowseViewHandlers.h"
#include "LaunchHandlers.h"
#include <CLogNode_Walker_Std.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Empty font
static LOGFONT emptyLogFont = { 0 };

// Persistent settings
CPersistentInt gGraphSelColor("P_GraphSelColor", RGB_DEF_SEL);
CPersistentInt gGraphShadowColor("P_GraphShadowColor", RGB_DEF_SHADOW);
CPersistentInt gGraphHeaderColor("P_GraphHeaderColor", RGB_DEF_HEADER);
CPersistentInt gGraphTagColor("P_GraphTagColor", RGB_DEF_TAG);
CPersistentInt gGraphBranchColor("P_GraphBranchColor", RGB_DEF_BRANCH);
CPersistentInt gGraphNodeColor("P_GraphNodeColor", RGB_DEF_NODE);
CPersistentInt gGraphDeadNodeColor("P_GraphDeadNodeColor", RGB_DEF_DEADNODE);
CPersistentInt gGraphUserStateNodeColor("P_GraphUserStateNodeColor", RGB_DEF_USERSTATENODE);
CPersistentInt gGraphMergeColor("P_GraphMergeColor", RGB_DEF_MERGE);
CPersistentT<LOGFONT> gGraphFont("P_GraphFont", emptyLogFont, kNoClass);

const int kVChildSpace = 8;		/*!< Vertical child spacing */
const int kHChildSpace = 40;	/*!< Horizontal child spacing */
const int kInflateNodeSpace = 8;/*!< Inflate node spacing */

/*!
	Helper function to draw the figure used for branch tag representation
	\param dc Device context
	\param selfB Bounds
*/
static void DrawOctangle(CDC& dc, const CRect& selfB)
{
	int cutLen = selfB.Height() / 4;
	CPoint point1(selfB.left, selfB.top + cutLen);
	CPoint point2(selfB.left + cutLen, selfB.top);
	CPoint point3(selfB.right - cutLen, selfB.top);
	CPoint point4(selfB.right, selfB.top + cutLen);
	CPoint point5(selfB.right, selfB.bottom - cutLen);
	CPoint point6(selfB.right - cutLen, selfB.bottom);
	CPoint point7(selfB.left + cutLen, selfB.bottom);
	CPoint point8(selfB.left, selfB.bottom - cutLen);
	CPoint arrPoints[OCTANGLE_COUNT] = {
		point1,
		point2,
		point3,
		point4,
		point5,
		point6,
		point7,
		point8};

	dc.Polygon(arrPoints, OCTANGLE_COUNT);
}

/*!
	Helper function used to draw the node
	\param dc Device context
	\param selfB Bounds
	\param contour Contour color
	\param shape Shape
	\param isSel true if it's selected, false if not selected
	\param penStyle Pen style
*/
static void DrawNode(CDC& dc, const CRect& selfB,
					 COLORREF contour, 
					 kNodeShape shape, bool isSel, int penStyle = PS_SOLID)
{
	CPen* pOldPen = 0L;
	CBrush* pOldBrush = 0L;
	CPen pen, pen2;
	CBrush brush;
	COLORREF selcolor = gGraphSelColor;
	COLORREF shadowc = gGraphShadowColor;
	
	TRY
	{
		// Prepare the shadow
		CRect shadow = selfB;
		shadow.OffsetRect(SHADOW_OFFSET_PT);
		
		brush.CreateSolidBrush(shadowc);
		pOldBrush = dc.SelectObject(&brush);
		pen.CreatePen(penStyle, 1, shadowc);
		pOldPen = dc.SelectObject(&pen);

		// Draw the shadow
		switch( shape )
		{
		case kRectangle:
			dc.Rectangle(shadow);
			break;
		case kRoundRect:
			dc.RoundRect(shadow, ROUND_RECT_PT);
			break;
		case kOctangle:
			DrawOctangle(dc, shadow);
			break;
		default:
			ASSERT(FALSE);	//unknown type
			return;
		}

		// Prepare selection
		if( isSel )
		{
			brush.DeleteObject();
			brush.CreateSolidBrush(selcolor);
			dc.SelectObject(&brush);
		}
		else
		{
			dc.SelectObject(pOldBrush);
			pOldBrush = 0L;
		}

		pen2.CreatePen(penStyle, 1, contour);
		dc.SelectObject(&pen2);
		
		// Draw the main shape
		switch( shape )
		{
		case kRectangle:
			dc.Rectangle(selfB);
			break;
		case kRoundRect:
			dc.RoundRect(selfB, ROUND_RECT_PT);
			break;
		case kOctangle:
			DrawOctangle(dc, selfB);
			break;
		default:
			ASSERT(FALSE);	//unknown type
			return;
		}

		// Cleanup
		if( pOldPen != 0L )
		{
			dc.SelectObject(pOldPen);
			pOldPen = 0L;
		}
		
		if( pOldBrush != 0L )
		{
			dc.SelectObject(pOldBrush);
			pOldBrush = 0L;
		}

		pen.DeleteObject();
		pen2.DeleteObject();
		brush.DeleteObject();
	}
	CATCH_ALL(e)
	{
		if( pOldPen != 0L )
			dc.SelectObject(pOldPen);

		if( pOldBrush != 0L )
			dc.SelectObject(pOldBrush);
	}
	END_CATCH_ALL
}

//////////////////////////////////////////////////////////////////////////
// CWinLogData

CWinLogData::CWinLogData(CLogNode* node) : CLogNodeData(node)
{
	m_selfB.SetRectEmpty();
	m_totB.SetRectEmpty();
	m_sel = false;
}

CWinLogData::~CWinLogData()
{
}

/*!
	Get log node data
	\param node Log node to get the date from
	\return The log node data
*/
CWinLogData* CWinLogData::GetData(CLogNode* const node)
{
	_ASSERT(node!=NULL);

	return static_cast<self_type*>(node->GetUserData());
}//GetData

/*!
	Get log node data
	\param node Log node to get the date from
	\return The log node data
*/
const CWinLogData* CWinLogData::GetData(const CLogNode* const node)
{
	_ASSERT(node!=NULL);

	return static_cast<const self_type*>(node->GetUserData());
}//GetData - const

/*!
	Get log node type
	\return The log node type
*/
kLogNode CWinLogData::GetType(void) const
{ 
	return m_node->GetType(); 
}

/*!
	Get the node string
	\return The node string
*/
const char* CWinLogData::GetStr(void)
{
	switch( GetType() )
	{
	case kNodeHeader:
		{
			CLogNodeHeader& header = *(CLogNodeHeader*)m_node;

			return (*header).WorkingFile().c_str();
		}
	case kNodeBranch:
		{
			CLogNodeBranch& branch = *(CLogNodeBranch*)m_node;

			return (*branch).c_str();
		}
	case kNodeRev:
		{
			CLogNodeRev& rev = *(CLogNodeRev*)m_node;

			return (*rev).RevNum().c_str();
		}
	case kNodeTag:
		{
			CLogNodeTag& tag = *(CLogNodeTag*)m_node;

			return (*tag).c_str();
		}
	}

	return NULL;
}

/*!
	Get the bounds rectangle
	\return The bounds rectangle
*/
CRect& CWinLogData::Bounds(void)
{
	return m_totB;
}

/*!
	Get the bounds rectangle
	\return The bounds rectangle
*/
const CRect& CWinLogData::Bounds(void)const
{
	return m_totB;
}

/*!
	Get the self bounds
	\return The self bounds
*/
CRect& CWinLogData::SelfBounds(void)
{
	return m_selfB;
}

/*!
	Get the self bounds
	\return The self bounds
*/
const CRect& CWinLogData::SelfBounds(void)const
{
	return m_selfB;
}

/*!
	Create new log data
	\param node Log node
	\return New log data on success, NULL otherwise
*/
CWinLogData* CWinLogData::CreateNewData(CLogNode* const node)
{
	_ASSERT(node != NULL);

	if( self_type* const pData=self_type::GetData(node) )
	{
		_ASSERT(pData != NULL);
		return pData;
	}//if

	self_type* res = 0L;
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

	default:
		_ASSERT(false);
		break;
	}//switch

	node->SetUserData(res);
	return res;
}//CreateNewData

/*!
	Check whether the selected node is the same as sandbox revision
	\return true if the selected node is a revision and the revision is the same as sandbox revision, false otherwise
*/
bool CWinLogData::IsDiskNode(const EntnodeData* const entryInfo)const
{
	if( entryInfo == 0L || GetType() != kNodeRev )
		return false;

	const string& vn = (*entryInfo)[EntnodeFile::kVN];
	if( vn.empty() )
		return false;

	return strcmp(vn.c_str(), (**(CLogNodeRev*)m_node).RevNum().c_str()) == 0;
}

/*!
	Check whether the selected node has the <b>dead</b> state
	\return true if the selected node is a revision and the state is <b>dead</b>, false otherwise
*/
bool CWinLogData::IsDeadNode()const
{
	if( GetType() != kNodeRev )
		return false;
	
	return strcmp("dead", (**(CLogNodeRev*)m_node).State().c_str()) == 0;
}

/*!
	Check whether the selected state is a user-defined state
	\return true if the selected node is a revision and the state is user-defined, false otherwise
*/
bool CWinLogData::IsUserState()const
{
	if( GetType() != kNodeRev )
		return false;
	
	return !(strcmp("dead", (**(CLogNodeRev*)m_node).State().c_str()) == 0 || 
		strcmp("Exp", (**(CLogNodeRev*)m_node).State().c_str()) == 0);
}

/*!
	Offset the log node
	\param offset Offset
*/
void CWinLogData::Offset(CPoint offset)
{
#if 1
	CLogNode_Walker_Std<CLogNode> walker(m_node);

	for(;;)
	{
		CWinLogData* const pData=self_type::GetData(walker.CurNode());

		_ASSERT(pData!=NULL);

		pData->m_selfB.OffsetRect(offset);
		pData->m_totB.OffsetRect(offset);

		if(!walker.Next())
		{
			break;
		}
	}//for[ever]
#else /*[2012-11-07] sf_animal: #OLD_CODE*/
	m_selfB.OffsetRect(offset);
	m_totB.OffsetRect(offset);
	
	vector<CLogNode*>::iterator i;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		CLogNode* subNode = *i;
		GetData(subNode)->Offset(offset);
	}

	if( m_node->Next() != 0L )
	{
		GetData(m_node->Next())->Offset(offset);
	}
#endif
}//Offset

/*!
	Get the selection state
	\return true if selected, false otherwise
*/
bool CWinLogData::Selected(void) const
{
	return m_sel;
}

/*!
	Set the selection state
	\param view View to set the selection state
	\param state New selection state
*/
void CWinLogData::SetSelected(CScrollView* view, bool state)
{
	m_sel = state;

	CRect invalR = m_selfB;
	invalR.OffsetRect(-view->GetDeviceScrollPosition());
	view->InvalidateRect(invalR);
}

/*!
	Remove selection from all nodes
	\param view View
*/
void CWinLogData::UnselectAll(CScrollView* view)
{
#if 1
	typedef CLogNode_Walker_Std<CLogNode> walker_type;

	walker_type walker(m_node);

	for(;;)
	{
		CWinLogData* const pData=GetData(walker.CurNode());

		_ASSERT(pData);

		if(pData->m_sel)
		{
			pData->SetSelected(view,false);
		}

		if(!walker.Next())
		{
			break;
		}//if
	}//for[ever]
#else /*[2012-11-07] sf_animal: #OLD_CODE*/
	if( m_sel )
	{
		SetSelected(view, false);
	}

	vector<CLogNode*>::iterator i;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		CLogNode* subNode = *i;
		GetData(subNode)->UnselectAll(view);
	}

	if( m_node->Next() != 0L )
		GetData(m_node->Next())->UnselectAll(view);
#endif
}//UnselectAll

/*!
	Scroll to node
	\param view View to scroll
*/
void CWinLogData::ScrollTo(CScrollView* view)
{
	if( view->GetStyle() & (WS_HSCROLL | WS_VSCROLL) )
	{
		CPoint pt = view->GetScrollPosition();
		
		CRect rect;
		view->GetClientRect(rect);
		
		if( view->GetStyle() & WS_HSCROLL )
		{
			const LONG tmp=SelfBounds().left + SelfBounds().Width() + 10 - rect.Width();

			pt.x = ((tmp<0)?0:tmp);
		}//if
		
		if( view->GetStyle() & WS_VSCROLL )
		{
			const LONG tmp=SelfBounds().top - SelfBounds().Height();

			pt.y = ((tmp<0)?0:tmp);
		}

		view->ScrollToPosition(pt);
	}
}

//------------------------------------------------------------------------
//class CWinLogData::tag_traits_for_ht

class CWinLogData::tag_traits_for_ht
{
public:
	enum tag_state_id
	{
		state_id__process_this   =0,
		state_id__process_childs =1,
		state_id__process_next   =2,
		state_id__stop           =3,
	};//enum tag_state_id

public:
	class tag_stack_item
	{
	public:
		tag_state_id m_state_id;
		CLogNode*    m_pNode;
		size_t       m_iChild;

	public:
		tag_stack_item(CLogNode* const pNode)
			:m_state_id(state_id__process_this),
			 m_pNode   (pNode),
			 m_iChild  (static_cast<size_t>(-1))
		{
			_ASSERT(m_pNode!=NULL);
		}
	};//class tag_state_item
};//class CWinLogData::tag_traits_for_ht

//------------------------------------------------------------------------
/*!
	Text whether the point is within the node and return the found node
	\param point Point to test
	\return Log data if node found, NULL otherwise
*/
CWinLogData* CWinLogData::HitTest(CPoint point)
{
#if 1
	typedef tag_traits_for_ht				traits_type;
	typedef traits_type::tag_stack_item     stack_item_type;
	typedef std::list<stack_item_type>      stack_type;

	stack_type Stack;

	Stack.push_back(m_node);

	while(!Stack.empty())
	{
		stack_item_type& CurItem=Stack.back();

		CLogNode* const CurItem_pNode=CurItem.m_pNode;

		_ASSERT(CurItem_pNode!=NULL);

		CWinLogData* const CurItem_pData=self_type::GetData(CurItem_pNode);

		_ASSERT(CurItem_pData!=NULL);

		switch(CurItem.m_state_id)
		{
			case traits_type::state_id__process_this:
			{
				if( !CurItem_pData->m_totB.PtInRect(point) )
				{
					CurItem.m_state_id=traits_type::state_id__stop;
					continue;
				}

				if( CurItem_pData->m_selfB.PtInRect(point) )
				{
					return CurItem_pData;
				}

				CurItem.m_state_id=traits_type::state_id__process_childs;

				CurItem.m_iChild=static_cast<size_t>(-1);

				continue;
			}//case state_id__process_this

			case traits_type::state_id__process_childs:
			{
				if(CurItem.m_iChild==static_cast<size_t>(-1))
				{
					CurItem.m_iChild=0;
				}
				else
				{
					_ASSERT(CurItem.m_iChild<CurItem_pNode->Childs().size());

					++CurItem.m_iChild;
				}

				if(CurItem.m_iChild<CurItem_pNode->Childs().size())
				{
					_ASSERT(CurItem_pNode->Childs()[CurItem.m_iChild]!=NULL);
					_ASSERT(CurItem_pNode->Childs()[CurItem.m_iChild]->Root()==CurItem_pNode);

					Stack.push_back(CurItem_pNode->Childs()[CurItem.m_iChild]);
					continue;
				}//if

				CurItem.m_state_id=traits_type::state_id__process_next;
				continue;
			}//case	state_id__process_childs

			case traits_type::state_id__process_next:
			{
				CurItem.m_state_id=traits_type::state_id__stop;

				if(CurItem_pNode->Next())
				{
					_ASSERT(CurItem_pNode->Next()->Root()==CurItem_pNode);

					Stack.push_back(CurItem_pNode->Next());
				}

				continue;
			}//case state_id__process_next

			case traits_type::state_id__stop:
			{
				Stack.pop_back();
				continue;
			}

			default:
			{
				_ASSERT(false);
			}
		}//switch
	}//while

	return NULL;
#else /*[2012-11-07] sf_animal: #OLD_CODE*/
	if( !m_totB.PtInRect(point) )
		return NULL;

	if( m_selfB.PtInRect(point) )
		return this;

	CWinLogData* result;
	vector<CLogNode*>::iterator i;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		result = GetData(*i)->HitTest(point);
		if( result != NULL )
			return result;
	}

	if( m_node->Next() != NULL )
	{
		result = GetData(m_node->Next())->HitTest(point);
		if( result != NULL )
			return result;
	}

	return NULL;
#endif
}//HitTest

//------------------------------------------------------------------------
class CWinLogData::tag_traits_for_update
{
public:
	enum tag_state_id
	{
		state_id__draw_this			=0,
		state_id__process_childs	=1,
		state_id__draw_next			=2,
		state_id__process_next		=3,
	};

	//--------------------------------------
	class tag_stack_item
	{
	public:
		tag_state_id		m_state_id;
		const CWinLogData*	m_pData;
		size_t				m_iChild;

	public:
		tag_stack_item(const CWinLogData* const pData)
			:m_state_id (state_id__draw_this),
			 m_pData    (pData),
			 m_iChild   (static_cast<size_t>(-1))
		{
			_ASSERT(m_pData!=NULL);
		}
	};//class tag_stack_item

	//--------------------------------------
	class tag_gdi_resources
	{
	public:
		CDC&	m_dc;

		CPen	m_pen;
		CPen	m_pen2;

		CPen*	m_pOldPen;

	public:
		tag_gdi_resources(CDC& dc)
			:m_dc(dc),
			 m_pOldPen(NULL)
		{;}

		~tag_gdi_resources()
		{
			if(m_pOldPen!=NULL)
				m_dc.SelectObject(m_pOldPen);

			m_pen.DeleteObject();
			m_pen2.DeleteObject();
		}//~tag_gdi_resources
	};//class tag_gdi_resources
};//class CWinLogData::tag_traits_for_update

//------------------------------------------------------------------------
/*!
	Draw the node
	\param dc Device context
	\param entryInfo Entry info
*/
void CWinLogData::Update(CDC& dc, const EntnodeData* const entryInfo)const
{
#if 1
	typedef tag_traits_for_update			traits_type;
	typedef traits_type::tag_gdi_resources  gdi_resources_type;
	typedef traits_type::tag_stack_item		stack_item_type;
	typedef std::list<stack_item_type>		stack_type;

	stack_type Stack;

	Stack.push_back(this);

	//--------------------------------------
	gdi_resources_type gdi(dc);

	//--------------------------------------

	TRY
	{
		gdi.m_pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255)); //throw
		gdi.m_pen2.CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); //throw

		gdi.m_pOldPen = gdi.m_dc.SelectObject(&gdi.m_pen); //throw?

		while(!Stack.empty())
		{
			stack_item_type& CurItem=Stack.back();

			const CWinLogData* const CurItem_pData=CurItem.m_pData;

			_ASSERT(CurItem_pData!=NULL);

			const CLogNode* const CurItem_pNode=CurItem_pData->m_node;

			_ASSERT(CurItem_pNode!=NULL);

			//-------
			const int topY = CurItem_pData->m_selfB.TopLeft().y + 2;
			const int botY = CurItem_pData->m_selfB.BottomRight().y - 2;
			const int lefX = CurItem_pData->m_selfB.BottomRight().x + 4;

			//------
			switch(CurItem.m_state_id)
			{
				case traits_type::state_id__draw_this:
				{
					CurItem_pData->UpdateSelf(gdi.m_dc,entryInfo);

					CurItem.m_state_id=traits_type::state_id__process_childs;

					_ASSERT(CurItem.m_iChild==static_cast<size_t>(-1));

					continue;
				}//case state_id__draw_this

				case traits_type::state_id__process_childs:
				{
					if(CurItem.m_iChild==static_cast<size_t>(-1))
					{
						CurItem.m_iChild=0;
					}
					else
					{
						_ASSERT(CurItem.m_iChild<CurItem_pNode->Childs().size());

						++CurItem.m_iChild;
					}//else

					if(CurItem.m_iChild<CurItem_pNode->Childs().size())
					{
						const size_t count	= CurItem.m_iChild+1;
						const size_t tot	= CurItem_pNode->Childs().size() + 1;

						//----
						const CLogNode* const subNode = CurItem_pNode->Childs()[CurItem.m_iChild];

						_ASSERT(subNode!=NULL);

						// Draw the link
						const CRect& cb = self_type::GetData(subNode)->SelfBounds();

						if( subNode->GetType() == kNodeRev || subNode->GetType() == kNodeBranch )
							gdi.m_dc.SelectObject(&gdi.m_pen);
						else
							gdi.m_dc.SelectObject(&gdi.m_pen2);

						float y = (float)topY + (float)(botY - topY) * (float)count / (float)tot;

						int rigX = cb.TopLeft().x - 4;

						float x = (float)rigX - (float)(rigX - lefX) * (float)count / (float)tot;

						// Special drawing for the header node
						if( CurItem_pNode->GetType() == kNodeHeader )
						{
							gdi.m_dc.MoveTo(CurItem_pData->m_selfB.TopLeft().x + kHChildSpace/2,
											CurItem_pData->m_selfB.BottomRight().y);

							gdi.m_dc.LineTo(CurItem_pData->m_selfB.TopLeft().x + kHChildSpace/2,
											cb.CenterPoint().y);

							gdi.m_dc.LineTo(cb.TopLeft().x,
											cb.CenterPoint().y);
						}
						else
						{
							//int halfX = (cb.TopLeft().x - m_selfB.BottomRight().x) / 2;
							const int ix = (int)x;
							int iy = (int)y;

							if( CurItem_pNode->Childs().size() == 1 )
							{
								if( iy > cb.CenterPoint().y )
									iy = cb.CenterPoint().y;

								gdi.m_dc.MoveTo(CurItem_pData->m_selfB.BottomRight().x, iy);
								gdi.m_dc.LineTo(cb.TopLeft().x, iy);
							}
							else
							{
								LONG const cb_CenterPoint_y=cb.CenterPoint().y;

								gdi.m_dc.MoveTo(CurItem_pData->m_selfB.BottomRight().x, iy);
								gdi.m_dc.LineTo(ix, iy);
								gdi.m_dc.LineTo(ix, cb_CenterPoint_y);
								gdi.m_dc.LineTo(cb.TopLeft().x, cb_CenterPoint_y);
							}
						}//else

						// Draw the sub-node
						Stack.push_back(stack_item_type(self_type::GetData(subNode)));

						continue;
					}//if CurItem.m_iChild<CurItem_pNode->Childs().size()

					CurItem.m_state_id=traits_type::state_id__draw_next;

					continue;
				}//case state_id__process_childs

				case traits_type::state_id__draw_next:
				{
					const CLogNode* const nextNode=CurItem_pNode->Next();

					CurItem.m_state_id=traits_type::state_id__process_next;

					if(nextNode!=NULL)
					{
						// Draw the link
						const CRect& nb = self_type::GetData(nextNode)->SelfBounds();

						if( nextNode->GetType() == kNodeRev || nextNode->GetType() == kNodeBranch )
							gdi.m_dc.SelectObject(&gdi.m_pen);
						else
							gdi.m_dc.SelectObject(&gdi.m_pen2);
			
						const int centerX
						 = (CurItem_pData->m_selfB.Width() < nb.Width())
							 ? CurItem_pData->m_selfB.CenterPoint().x
							 : nb.CenterPoint().x;

						gdi.m_dc.MoveTo(centerX,
										CurItem_pData->m_selfB.BottomRight().y);

						gdi.m_dc.LineTo(centerX,
										nb.TopLeft().y);

						// Draw the next node
						Stack.push_back(self_type::GetData(nextNode));
					}//if nextNode!=NULL

					continue;
				}//case state_id__draw_next

				case traits_type::state_id__process_next:
				{
					Stack.pop_back();
					continue;
				}

				default:
				{
					_ASSERT(false);
				}
			}//switch
		}//while
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL

#else /*[2012-11-07] sf_animal: #OLD_CODE*/
	const CLogNode* subNode;
	const CLogNode* const nextNode = m_node->Next();
	CPen* pOldPen = 0L;
	CPen pen, pen2;

	UpdateSelf(dc, entryInfo);

	TRY
	{
		pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		pen2.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		pOldPen = dc.SelectObject(&pen);

		{
		 const int topY = m_selfB.TopLeft().y + 2;
		 const int botY = m_selfB.BottomRight().y - 2;
		 const int lefX = m_selfB.BottomRight().x + 4;

		 size_t count = 1;
		 const size_t tot = m_node->Childs().size() + 1;

		 // Draw the children
		 vector<CLogNode*>::const_iterator        i(m_node->Childs().begin());
		 vector<CLogNode*>::const_iterator const _e(m_node->Childs().end());

		 for(; i != _e; ++i, ++count)
		 {
			subNode = *i;
			
			// Draw the link
			const CRect& cb = GetData(subNode)->SelfBounds();
			
			if( subNode->GetType() == kNodeRev || subNode->GetType() == kNodeBranch )
				dc.SelectObject(&pen);
			else
				dc.SelectObject(&pen2);

			const float y = (float)topY + (float)(botY - topY) * (float)count / (float)tot;
			const int rigX = cb.TopLeft().x - 4;
			const float x = (float)rigX - (float)(rigX - lefX) * (float)count / (float)tot;

			// Special drawing for the header node
			if( m_node->GetType() == kNodeHeader )
			{
				dc.MoveTo(m_selfB.TopLeft().x + kHChildSpace/2, m_selfB.BottomRight().y);
				dc.LineTo(m_selfB.TopLeft().x + kHChildSpace/2, cb.CenterPoint().y);
				dc.LineTo(cb.TopLeft().x, cb.CenterPoint().y);
			}
			else
			{
				//int halfX = (cb.TopLeft().x - m_selfB.BottomRight().x) / 2;
				const int ix = (int)x;
				int iy = (int)y;

				if( m_node->Childs().size() == 1 )
				{
					if( iy > cb.CenterPoint().y )
						iy = cb.CenterPoint().y;

					dc.MoveTo(m_selfB.BottomRight().x, iy);
					dc.LineTo(cb.TopLeft().x, iy);
				}
				else
				{
					dc.MoveTo(m_selfB.BottomRight().x, iy);
					dc.LineTo(ix, iy);
					dc.LineTo(ix, cb.CenterPoint().y);
					dc.LineTo(cb.TopLeft().x, cb.CenterPoint().y);
				}
			}
			
			// Draw the sub-node
			GetData(subNode)->Update(dc, entryInfo);
		 }//for
		}//local

		// Draw the next node
		if( nextNode != 0L )
		{
			// Draw the link
			const CRect& nb = GetData(nextNode)->SelfBounds();
			if( nextNode->GetType() == kNodeRev || nextNode->GetType() == kNodeBranch )
				dc.SelectObject(&pen);
			else
				dc.SelectObject(&pen2);
			int centerX = m_selfB.Width() < nb.Width() ?
				m_selfB.CenterPoint().x : nb.CenterPoint().x;

			dc.MoveTo(centerX, m_selfB.BottomRight().y);
			dc.LineTo(centerX, nb.TopLeft().y);
			
			// Draw the next node
			GetData(nextNode)->Update(dc, entryInfo);
		}

		if( pOldPen != 0L )
		{
			dc.SelectObject(pOldPen);
			pOldPen = 0L;
		}

		pen.DeleteObject();
		pen2.DeleteObject();
	}
	CATCH_ALL(e)
	{
		if( pOldPen != 0L )
			dc.SelectObject(pOldPen);
	}
	END_CATCH_ALL
#endif
}//Update

//------------------------------------------------------------------------
class CWinLogData::tag_traits_for_cb
{
public:
	enum tag_state_id
	{
		state_id__start         =0,
		state_id__process_child =1,
		state_id__process_this  =2,
		state_id__process_next  =3,
	};

	class tag_stack_item
	{
	public:
		tag_state_id  m_state_id;
		CPoint        m_topLeft;
		CWinLogData*  m_pData;
		size_t        m_iChild;

	public:
		tag_stack_item(const CPoint&       topLeft,
					   CWinLogData*  const pData)
		:m_state_id (state_id__start),
		 m_topLeft  (topLeft),
		 m_pData    (pData),
		 m_iChild   (0)
		{
			_ASSERT(m_pData!=NULL);
		}
	};//class tag_stack_item
};//class tag_traits_for_cb

//------------------------------------------------------------------------
/*!
	Calculate the bounds
	\param topLeft Top-left point for the bounds
	\param dc Device context
	\param entryInfo Entry info
*/
void CWinLogData::ComputeBounds(const CPoint&            topLeft,
								CDC&                     dc,
								const EntnodeData* const entryInfo)
{
#if 1
	typedef vector<CLogNode*>::iterator     node_iterator;

	typedef tag_traits_for_cb               traits_type;
	typedef traits_type::tag_stack_item     stack_item_type;
	typedef std::list<stack_item_type>		stack_type;

	stack_type Stack;

	Stack.push_back(stack_item_type(topLeft,this));

	while(!Stack.empty())
	{
		stack_item_type& CurItem=Stack.back();

		CWinLogData* const CurItem_pData=CurItem.m_pData;

		_ASSERT(CurItem_pData!=NULL);

		CLogNode* const CurItem_pNode=CurItem_pData->m_node;

		_ASSERT(CurItem_pNode!=NULL);
		_ASSERT(CurItem.m_iChild<=CurItem_pNode->Childs().size());

		switch(CurItem.m_state_id)
		{
			case traits_type::state_id__start:
			{
				CurItem.m_state_id=traits_type::state_id__process_child;
				CurItem.m_iChild=0;

				if(CurItem.m_iChild<CurItem_pNode->Childs().size())
				{
					_ASSERT(CurItem_pNode->Childs()[0]!=NULL);
					_ASSERT(CurItem_pNode->Childs()[0]->Root()==CurItem_pNode);

					CWinLogData* const Child_pData=self_type::CreateNewData(CurItem_pNode->Childs()[0]);

					_ASSERT(Child_pData!=NULL);

					Stack.push_back(stack_item_type(CurItem.m_topLeft,
													Child_pData));
					continue;
				}//if

				CurItem.m_state_id=traits_type::state_id__process_this;
				continue;
			}//case state_id__start

			case traits_type::state_id__process_child:
			{
				_ASSERT(CurItem.m_iChild<CurItem_pNode->Childs().size());
				
				++CurItem.m_iChild;

				if(CurItem.m_iChild<CurItem_pNode->Childs().size())
				{
					_ASSERT(CurItem_pNode->Childs()[CurItem.m_iChild]!=NULL);
					_ASSERT(CurItem_pNode->Childs()[CurItem.m_iChild]->Root()==CurItem_pNode);

					CWinLogData* const Child_pData=self_type::CreateNewData(CurItem_pNode->Childs()[CurItem.m_iChild]);

					_ASSERT(Child_pData!=NULL);

					Stack.push_back(stack_item_type(CurItem.m_topLeft,
													Child_pData));
					continue;
				}//if

				CurItem.m_state_id=traits_type::state_id__process_this;
				continue;
			}//case state_id__process_child

			case traits_type::state_id__process_this:
			{
				_ASSERT(CurItem.m_iChild==CurItem_pNode->Childs().size());

				// Compute self place
				CurItem_pData->m_selfB.SetRectEmpty();
				CSize size = CurItem_pData->GetBoundsSelf(dc, entryInfo);
				size.cx += kInflateNodeSpace;
				size.cy += kInflateNodeSpace;
				CurItem_pData->m_selfB.SetRect(0, 0, size.cx, size.cy);

				// Offset to the place assigned to this node
				CurItem_pData->m_selfB.OffsetRect(CurItem.m_topLeft - CurItem_pData->m_selfB.TopLeft());

				// Calculate the total height of the childrens
				int vSize = 0;

				{
					for(node_iterator i = CurItem_pNode->Childs().begin(),
								 _e = CurItem_pNode->Childs().end();
						i != CurItem_pNode->Childs().end();
						++i)
					{
						const CRect& b = self_type::GetData(*i)->Bounds();
						vSize += b.Height();
					}
				}

				if( !CurItem_pNode->Childs().empty() )
					vSize += (CurItem_pNode->Childs().size() - 1) * kVChildSpace;

				// Offset the children relative to self
				CPoint startChilds(CurItem.m_topLeft.x + CurItem_pData->m_selfB.Width() + kHChildSpace,
								   /*selfB.CenterPoint().y - vSize / 2*/CurItem.m_topLeft.y);

				// Place the first child at the bottom of the header node so it won't take so much space for long filenames
				if( CurItem_pNode->GetType() == kNodeHeader )
				{
					startChilds = CPoint(CurItem.m_topLeft.x + kHChildSpace,
										 CurItem.m_topLeft.y + CurItem_pData->m_selfB.Height() + kVChildSpace);
				}

				for(node_iterator i = CurItem_pNode->Childs().begin(),
								 _e = CurItem_pNode->Childs().end();
					i != _e;
					++i)
				{
					//attention: this is really COPY of child bounds!
					const CRect curPos = self_type::GetData(*i)->Bounds();

					self_type::GetData(*i)->Offset(startChilds - curPos.TopLeft());

					startChilds.y += curPos.Height() + kVChildSpace;
				}

				// Calculate the total bounds of the childrens
				{
					CRect bc(0,0,0,0);

					for(node_iterator i = CurItem_pNode->Childs().begin(),
								 _e = CurItem_pNode->Childs().end();
						i != _e;
						++i)
					{
						const CRect& b = self_type::GetData(*i)->Bounds();
						bc.UnionRect(bc, b);
					}

					// Now we got the complete size
					CurItem_pData->m_totB.UnionRect(CurItem_pData->m_selfB, bc);
				}//local

				if( CurItem_pNode->Next() != NULL )
				{
					_ASSERT(CurItem_pNode->Next()->Root()==CurItem_pNode);

					CPoint nextTopLeft;
					nextTopLeft.x = CurItem_pData->m_totB.TopLeft().x;
					nextTopLeft.y = CurItem_pData->m_totB.BottomRight().y + kVChildSpace;
		
					Stack.push_back(stack_item_type(nextTopLeft,
													self_type::CreateNewData(CurItem_pNode->Next())));

					CurItem.m_state_id=traits_type::state_id__process_next;
					continue;
				}//if

				_ASSERT(CurItem_pNode->Next()==NULL);

				Stack.pop_back();
				continue;
			}//case state_id__process_this

			case traits_type::state_id__process_next:
			{
				_ASSERT(CurItem.m_iChild==CurItem_pNode->Childs().size());
				_ASSERT(CurItem_pNode->Next()!=NULL);

				CurItem_pData->m_totB.UnionRect(CurItem_pData->m_totB,
				                                self_type::GetData(CurItem_pNode->Next())->Bounds());

				Stack.pop_back();
				continue;
			}//case state_id__process_this

			default:
			{
				_ASSERT(false);
			}
		}//switch
	}//while
#else /*[2012-11-08] sf_animal: #OLD_CODE*/
	// First compute childrens bounds
	vector<CLogNode*>::iterator i;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		self_type::CreateNewData(*i)->ComputeBounds(topLeft, dc, entryInfo);
	}

	// Compute self place
	m_selfB.SetRectEmpty();
	CSize size = GetBoundsSelf(dc, entryInfo);
	size.cx += kInflateNodeSpace;
	size.cy += kInflateNodeSpace;
	m_selfB.SetRect(0, 0, size.cx, size.cy);

	// Offset to the place assigned to this node
	m_selfB.OffsetRect(topLeft - m_selfB.TopLeft());

	// Calculate the total height of the childrens
	int vSize = 0;
	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		vSize +=self_type::GetData(*i)->Bounds().Height();
	}

	if( !m_node->Childs().empty() )
		vSize += (m_node->Childs().size() - 1) * kVChildSpace;

	// Offset the children relative to self
	CPoint startChilds(topLeft.x + m_selfB.Width() + kHChildSpace,
		/*m_selfB.CenterPoint().y - vSize / 2*/topLeft.y);

	// Place the first child at the bottom of the header node so it won't take so much space for long filenames
	if( m_node->GetType() == kNodeHeader )
	{
		startChilds = CPoint(topLeft.x + kHChildSpace, topLeft.y + m_selfB.Height() + kVChildSpace);
	}

	for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
	{
		//attention: this is really COPY of child bounds!
		const CRect curPos = GetData(*i)->Bounds();

		self_type::GetData(*i)->Offset(startChilds - curPos.TopLeft());

		startChilds.y += curPos.Height() + kVChildSpace;
	}//for

	// Calculate the total bounds of the childrens
	{
		CRect bc(0,0,0,0);
		for(i = m_node->Childs().begin(); i != m_node->Childs().end(); ++i)
		{
			const CRect& b = self_type::GetData(*i)->Bounds();
			bc.UnionRect(bc, b);
		}

		// Now we got the complete size
		m_totB.UnionRect(m_selfB, bc);
	}//local

	if(CLogNode* const pNextNode=m_node->Next())
	{
		_ASSERT(pNextNode!=NULL);

		self_type::CreateNewData(pNextNode);

		CPoint nextTopLeft;
		nextTopLeft.x = m_totB.TopLeft().x;
		nextTopLeft.y = m_totB.BottomRight().y + kVChildSpace;

		self_type::GetData(pNextNode)->ComputeBounds(nextTopLeft, dc, entryInfo);

		m_totB.UnionRect(m_totB, self_type::GetData(pNextNode)->Bounds());
	}//if has NextNode
#endif
}//ComputeBounds

/*!
	Draw the merges relationship
	\param dc Device context
*/
void CWinLogData::UpdateMergePoints(CDC& dc)
{
	data_pair_vec_type mergePair;
	
	CollectMergePointPairPass1(mergePair);
	CollectMergePointPairPass2(mergePair);

	CPen pen;

	pen.CreatePen(PS_SOLID, 0, gGraphMergeColor);

	CPen* oldPen = dc.SelectObject(&pen);

	TRY
	{
		for(int i = 0; i < (int)mergePair.size(); i++)
		{
			if( mergePair[i].first != 0L && mergePair[i].second != 0L )
			{
				CRect rect_source = mergePair[i].first->SelfBounds();
				CRect rect_target = mergePair[i].second->SelfBounds();
			
				if( rect_source.left > rect_target.right )
				{
					CPoint pts[7];
					
					pts[0] = CPoint(rect_source.left, rect_source.top + rect_source.Height()/2);
					pts[1] = CPoint(pts[0].x - 8, pts[0].y);
					pts[2] = CPoint(rect_target.right + 8, rect_target.top + rect_target.Height()/2);
					pts[3] = CPoint(pts[2].x - 8, pts[2].y);
					pts[4] = CPoint(pts[3].x + 4, pts[3].y-2);
					pts[5] = CPoint(pts[4].x, pts[4].y+4);
					pts[6] = pts[3];
					
					dc.Polyline(pts, 7);
				}
				else if( rect_source.left == rect_target.left )
				{
					CPoint pts[7];
					
					pts[0] = CPoint(rect_source.right, rect_source.top + rect_source.Height()/2);
					pts[1] = CPoint(pts[0].x + 8, pts[0].y);
					pts[2] = CPoint(rect_target.right + 8, rect_target.top + rect_target.Height()/2);
					pts[3] = CPoint(pts[2].x - 8, pts[2].y);
					pts[4] = CPoint(pts[3].x + 4, pts[3].y-2);
					pts[5] = CPoint(pts[4].x, pts[4].y+4);
					pts[6] = pts[3];
					
					dc.Polyline(pts, 7);
				}
				else
				{
					CPoint pts[7];
					
					pts[0] = CPoint(rect_source.right, rect_source.top + rect_source.Height()/2);
					pts[1] = CPoint(pts[0].x + 8, pts[0].y);
					pts[2] = CPoint(rect_target.left - 8, rect_target.top + rect_target.Height()/2);
					pts[3] = CPoint(pts[2].x + 8, pts[2].y);
					pts[4] = CPoint(pts[3].x - 4, pts[3].y-2);
					pts[5] = CPoint(pts[4].x, pts[4].y+4);
					pts[6] = pts[3];
					
					dc.Polyline(pts, 7);
				}
			}
		}
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	
	if( oldPen != 0L )
		dc.SelectObject(&oldPen);

	pen.DeleteObject();
}

void CWinLogData::CollectMergePointPairPass1(data_pair_vec_type& mergePair)
{
#if 1
	typedef CLogNode_Walker_Std<CLogNode> walker_type;

	walker_type walker(m_node);

	for(;;)
	{
		self_type::GetData(walker.CurNode())->CollectMergePointPairPass1Self(mergePair);

		if(!walker.Next())
		{
			break;
		}
	}//for[ever]
#else /*[2012-11-07] sf_animal: #OLD_CODE*/
	this->CollectMergePointPairPass1Self(mergePair);
	
	vector<CLogNode*>::const_iterator it = m_node->Childs().begin();
	
	while( it != m_node->Childs().end() )
	{
		self_type::GetData(*it)->CollectMergePointPairPass1(mergePair);

		++it;
	}
	
	if( CLogNode* const pNextNode=m_node->Next() )
	{
		_ASSERT(pNextNode!=NULL);

		self_type::GetData(pNextNode)->CollectMergePointPairPass1(mergePair);
	}
#endif
}//CollectMergePointPairPass1

void CWinLogData::CollectMergePointPairPass2(data_pair_vec_type& mergePair)
{
#if 1
	typedef CLogNode_Walker_Std<CLogNode> walker_type;

	walker_type walker(m_node);

	for(;;)
	{
		self_type::GetData(walker.CurNode())->CollectMergePointPairPass2Self(mergePair);

		if(!walker.Next())
		{
			break;
		}
	}//for[ever]
#else /*[2012-11-07] sf_animal: #OLD_CODE*/
	this->CollectMergePointPairPass2Self(mergePair);
	
	vector<CLogNode*>::const_iterator it = m_node->Childs().begin();
	
	while( it != m_node->Childs().end() )
	{
		self_type::GetData(*it)->CollectMergePointPairPass2(mergePair);
		
		++it;
	}
	
	if( CLogNode* const pNextNode=m_node->Next() )
	{
		_ASSERT(pNextNode!=NULL);

		self_type::GetData(pNextNode)->CollectMergePointPairPass2(mergePair);
	}
#endif
}//CollectMergePointPairPass2

/*!
	Draw the node
	\param dc Device context
	\param entryInfo Entry info
*/
void CWinLogData::UpdateSelf(CDC& dc, const EntnodeData* const /*entryInfo*/)const
{
}

/*!
	Get the bounds size
	\param dc Device context
	\param entryInfo Entry info
	\return Bounds size as CSize
*/
CSize CWinLogData::GetBoundsSelf(CDC& dc, const EntnodeData* const /*entryInfo*/)const
{
	return CSize(0, 0);
}


/*!
	Get a vector of merge points pair (source-->Target).
	This is the first pass, which collects only the target
	\param mergePair Vector of merge pair
*/
void CWinLogData::CollectMergePointPairPass1Self(data_pair_vec_type& mergePair)
{
}

/*!
	Get a vector of merge points pair (source-->Target).
	This is the second pass, which makes the link between source and target
	\param mergePair Vector of merge pair
*/
void CWinLogData::CollectMergePointPairPass2Self(data_pair_vec_type& mergePair)
{
}
	

//////////////////////////////////////////////////////////////////////////
// CWinLogHeaderData

CSize CWinLogHeaderData::GetBoundsSelf(CDC& dc, const EntnodeData* const /*entryInfo*/)const
{
	CLogNodeHeader& header = *(CLogNodeHeader*)m_node;
	
	return dc.GetTextExtent((*header).WorkingFile().c_str(),
		(*header).WorkingFile().length());
}

void CWinLogHeaderData::UpdateSelf(CDC& dc, const EntnodeData* const /*entryInfo*/)const
{
	// Draw the node
	::DrawNode(dc, SelfBounds(), gGraphHeaderColor, kRoundRect, Selected());

	// Draw the content of the node
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	
	CPoint center = SelfBounds().CenterPoint();
	int nCenterX = center.x;
	int nCenterY = center.y - tm.tmHeight/2;
	
	CLogNodeHeader& header = *(CLogNodeHeader*)m_node;
	
	dc.ExtTextOut(nCenterX, nCenterY, ETO_CLIPPED, SelfBounds(),
		(*header).WorkingFile().c_str(), (*header).WorkingFile().length(), NULL);
}

//////////////////////////////////////////////////////////////////////////
// CWinLogRevData

CSize CWinLogRevData::GetBoundsSelf(CDC& dc, const EntnodeData* const entryInfo)const
{
	CLogNodeRev& rev = *(CLogNodeRev*)m_node;
	CSize size = dc.GetTextExtent((*rev).RevNum().c_str(), strlen((*rev).RevNum().c_str()));
	
	if( IsDiskNode(entryInfo) )
	{
		size.cx += 2 + 16 + 2;
		size.cy = max(16, size.cy);
	}

	return size;
}

void CWinLogRevData::UpdateSelf(CDC& dc, const EntnodeData* const entryInfo)const
{
	CRect txtRect = SelfBounds();

	if( IsDiskNode(entryInfo) )
	{
		txtRect.left += 2 + 16 + 2;
	}

	// Draw the node
	COLORREF contour = gGraphNodeColor;
	int penStyle = PS_SOLID;

	if( IsDeadNode() )
	{
		contour = (COLORREF)gGraphDeadNodeColor;
		penStyle = STYLE_DEADNODE;
	}
	else if( IsUserState() )
	{
		contour = (COLORREF)gGraphUserStateNodeColor;
		penStyle = STYLE_USERSTATENODE;
	}

	::DrawNode(dc, SelfBounds(), contour, kRectangle, Selected(), penStyle);

	// Draw the state icon if it is the rev. of the disk
	if( IsDiskNode(entryInfo) )
	{
		CImageList& list = CGraphView::GetImgList();
		CPoint w(SelfBounds().TopLeft());
		w.x += 2;
		w.y += (SelfBounds().Height() - 16) / 2;

		list.Draw(&dc, CBrowseFileView::GetImageForEntry(entryInfo), w, ILD_TRANSPARENT);
	}

	// Draw the content of the node
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	
	CPoint center = txtRect.CenterPoint();
	int nCenterX = center.x;
	int nCenterY = center.y - tm.tmHeight/2;
	
	CLogNodeRev& rev = *(CLogNodeRev*)m_node;
	dc.ExtTextOut(nCenterX, nCenterY, ETO_CLIPPED, txtRect,
		(*rev).RevNum().c_str(), strlen((*rev).RevNum().c_str()), NULL);
}


void CWinLogRevData::CollectMergePointPairPass1Self(data_pair_vec_type& mergePair)
{
	const CRevFile& rev = *(*((const CLogNodeRev*)Node()));
	
	if( !rev.MergePoint().empty() )
	{
		mergePair.push_back(data_pair_type((self_type*)NULL, this));
	}
}

void CWinLogRevData::CollectMergePointPairPass2Self(data_pair_vec_type& mergePair)
{
	const CRevFile& rev = *(*((const CLogNodeRev*)m_node));
	
	typedef data_pair_vec_type::size_type size_type;

	for(size_type i = 0; i < mergePair.size(); i++)
	{
		if( mergePair[i].first == 0L &&  (*(*((const CLogNodeRev*)(mergePair[i].second->Node())))).MergePoint() == rev.RevNum() )
		{
			mergePair[i].first = this;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CWinLogTagData

CSize CWinLogTagData::GetBoundsSelf(CDC& dc, const EntnodeData* const /*entryInfo*/)const
{
	CLogNodeTag& tag = *(CLogNodeTag*)m_node;
	
	return dc.GetTextExtent((*tag).c_str(), (*tag).length());
}

void CWinLogTagData::UpdateSelf(CDC& dc, const EntnodeData* const /*entryInfo*/)const
{
	// Draw the node
	::DrawNode(dc, SelfBounds(), gGraphTagColor, kRoundRect, Selected());

	// Draw the content of the node
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	CPoint center = SelfBounds().CenterPoint();
	int nCenterX = center.x;
	int nCenterY = center.y - tm.tmHeight/2;
	
	CLogNodeTag& tag = *(CLogNodeTag*)m_node;
	dc.ExtTextOut(nCenterX, nCenterY, ETO_CLIPPED, SelfBounds(), (*tag).c_str(), (*tag).length(), NULL);
}

//////////////////////////////////////////////////////////////////////////
// CWinLogBranchData

CSize CWinLogBranchData::GetBoundsSelf(CDC& dc, const EntnodeData* const /*entryInfo*/)const
{
	CLogNodeBranch& branch = *(CLogNodeBranch*)m_node;

	return dc.GetTextExtent((*branch).c_str(), (*branch).length());
}

void CWinLogBranchData::UpdateSelf(CDC& dc, const EntnodeData* const /*entryInfo*/)const
{
	// Draw the node
	::DrawNode(dc, SelfBounds(), gGraphBranchColor, kOctangle, Selected());

	// Draw the content of the node
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	CPoint center = SelfBounds().CenterPoint();
	int nCenterX = center.x;
	int nCenterY = center.y - tm.tmHeight/2;
	CLogNodeBranch& branch = *(CLogNodeBranch*)m_node;
	dc.ExtTextOut(nCenterX, nCenterY, ETO_CLIPPED, SelfBounds(), (*branch).c_str(), (*branch).length(), NULL);
}

//////////////////////////////////////////////////////////////////////////
/// CGraphRectTracker

/// OnChangedRect virtual override
void CGraphRectTracker::OnChangedRect(const CRect& rectOld)
{
#if 0 // :-< doesn't work...
	CRect r;
	m_view->GetClientRect(&r);
	CPoint rb = m_rect.BottomRight();

	if( !r.PtInRect(rb) )
	{
		CPoint orig = m_view->GetDeviceScrollPosition();
		CPoint offset;
		if( rb.x >= r.right )
			offset.x += 10;

		if( rb.y >= r.bottom )
			offset.y += 10;

		orig += offset;
		m_rect -= offset;
		m_view->ScrollToPosition(orig);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
//class TGraph::tag_traits_for_tg

class TGraph::tag_traits_for_tg
{
public:
	enum tag_state_id
	{
		state_id__process_this	=0,
		state_id__process_childs=1,
		state_id__process_next  =2,
		state_id__stop          =3,
	};//enum

	class tag_stack_item
	{
	public:
		tag_state_id	m_state_id;
		CLogNode*		m_pNode;
		size_t			m_iChild;

	public:
		tag_stack_item(CLogNode* const pNode)
			:m_state_id(state_id__process_this),
			 m_pNode	(pNode),
			 m_iChild	(static_cast<size_t>(-1))
		{
			_ASSERT(m_pNode!=NULL);
		}
	};//class tag_stack_item
};//class TGraph::tag_traits_for_tg

//////////////////////////////////////////////////////////////////////////
//class TGraph

/*!
	Traverse the graph
	\param node Node
	\return kTGraph
*/
kTGraph TGraph::TraverseGraph(CLogNode* const node)
{
	_ASSERT(node!=NULL);

#if 1
	typedef tag_traits_for_tg				traits_type;
	typedef traits_type::tag_stack_item		stack_item_type;
	typedef std::list<stack_item_type>      stack_type;

	stack_type Stack;

	Stack.push_back(node);

	while(!Stack.empty())
	{
		stack_item_type& CurItem=Stack.back();

		CLogNode* const CurItem_pNode=CurItem.m_pNode;

		_ASSERT(CurItem_pNode!=NULL);

		switch(CurItem.m_state_id)
		{
			case traits_type::state_id__process_this:
			{
				kTGraph const res = TraverseNode(CurItem_pNode);

				if( res == kTStop )
					return kTStop;

				if( res == kTGetDown)
				{
					CurItem.m_state_id=traits_type::state_id__process_childs;

					CurItem.m_iChild=static_cast<size_t>(-1);

					continue;
				}//if

				CurItem.m_state_id=traits_type::state_id__process_next;
				continue;
			}//case state_id__process_this

			case traits_type::state_id__process_childs:
			{
				if(CurItem.m_iChild==static_cast<size_t>(-1))
				{
					CurItem.m_iChild=0;
				}
				else
				{
					_ASSERT(CurItem.m_iChild<=CurItem_pNode->Childs().size());

					++CurItem.m_iChild;
				}//else

				if(CurItem.m_iChild<CurItem_pNode->Childs().size())
				{
					Stack.push_back(CurItem_pNode->Childs()[CurItem.m_iChild]);

					continue;
				}

				CurItem.m_state_id=traits_type::state_id__process_next;
				continue;
			}//case state_id__process_childs

			case traits_type::state_id__process_next:
			{
				CurItem.m_state_id=traits_type::state_id__stop;

				if(CurItem_pNode->Next()!=NULL)
				{
					Stack.push_back(CurItem_pNode->Next());
				}

				continue;
			}//case state_id__process_next

			case traits_type::state_id__stop:
			{
				Stack.pop_back();
				continue;
			}

			default:
			{
				_ASSERT(false);
			}
		}//switch
	}//while

	return kTContinue;	
#else /*[2012-11-07] sf_animal: #OLD_CODE*/
	kTGraph res = TraverseNode(node);
	if( res == kTStop )
		return kTStop;

	if( res == kTGetDown )
	{
		vector<CLogNode*>::iterator i;
		for(i = node->Childs().begin(); i != node->Childs().end(); ++i)
		{
			res = TraverseGraph(*i);
			if( res == kTStop )
				return kTStop;
		}
	}

	if( node->Next() != 0L )
	{
		res = TraverseGraph(node->Next());
		if( res == kTStop )
			return kTStop;
	}

	return kTContinue;
#endif
}//TraverseGraph

//////////////////////////////////////////////////////////////////////////
// TGraphRectSelect

TGraphRectSelect::TGraphRectSelect(bool shiftOn, CGraphView* view, CRect& rect) 
	: m_shiftOn(shiftOn), m_view(view), m_rect(rect)
{
}

kTGraph TGraphRectSelect::TraverseNode(CLogNode* const node)
{
	CWinLogData* const data = (CWinLogData*)node->GetUserData();
	
	// No need to go further
	CRect inter;
	if( !inter.IntersectRect(m_rect, data->Bounds()) )
	{
		return kTContinue;
	}
	
	// Self enclosed in the tracker ?
	const CRect& boundsRect = data->SelfBounds();
	if( m_rect.PtInRect(boundsRect.TopLeft()) && m_rect.PtInRect(boundsRect.BottomRight()) )
	{
		if( m_shiftOn )
		{
			data->SetSelected(m_view, !data->Selected());
		}
		else
		if( !data->Selected() )
		{
			data->SetSelected(m_view, true);
		}
	}//if
	
	// Traverse childrens
	return kTGetDown;
}//TraverseNode

//////////////////////////////////////////////////////////////////////////
// TGraphCmdStatus

TGraphCmdStatus::TGraphCmdStatus() 
	: m_sel1(0L), m_sel2(0L), m_selCount(0)
{
}

kTGraph TGraphCmdStatus::TraverseNode(CLogNode* node)
{
	CWinLogData* data = (CWinLogData*)node->GetUserData();
	
	if( data->Selected() )
	{
		if( m_sel1 == 0L )
			m_sel1 = data;
		else if( m_sel2 == 0L )
			m_sel2 = data;
		
		m_selCount++;
	}
	
	return kTGetDown;
}

/*!
	Get the selection count
	\return The selection count
*/
int TGraphCmdStatus::GetSelCount() const
{
	return m_selCount;
}

/*!
	Get the first selection
	\return The first selection
*/
CWinLogData* TGraphCmdStatus::GetSel1() const
{
	return m_sel1;
}

/*!
	Get the second selection
	\return The second selection
*/
CWinLogData* TGraphCmdStatus::GetSel2() const
{
	return m_sel2;
}

//////////////////////////////////////////////////////////////////////////
// CGraphConsole

CGraphConsole::CGraphConsole()
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	m_view = app->GetConsoleView();
}

long CGraphConsole::cvs_out(const char* txt, long len)
{
	if( m_view != 0L )
		m_view->OutConsole(txt, len);
	
	return len;
}

long CGraphConsole::cvs_err(const char* txt, long len)
{
	if( m_view != 0L )
		m_view->OutConsole(txt, len, true);
	
	return len;
}

//////////////////////////////////////////////////////////////////////////
// TGraphDelCmd

TGraphDelCmd::TGraphDelCmd() 
	: m_countSel(0), m_onlyRev(true)
{
}

kTGraph TGraphDelCmd::TraverseNode(CLogNode* node)
{
	CWinLogData* data = (CWinLogData*)node->GetUserData();

	if( data->Selected() )
	{
		if( data->GetType() != kNodeRev )
			m_onlyRev = false;
		else
			m_allRevs.push_back(string(data->GetStr()));
	
		m_countSel++;
	}

	return kTGetDown;
}

/*!
	Get the selection count
	\return The selection count
*/
int TGraphDelCmd::GetCountSel() const
{
	return m_countSel;
}

/*!
	Get only revisions flag
	\return The only revisions flag
*/
bool TGraphDelCmd::GetOnlyRev() const
{
	return m_onlyRev;
}

/*!
	Get all revisions
	\return All revisions
*/
std::vector<std::string>& TGraphDelCmd::GetAllRevs()
{
	return m_allRevs;
}

//////////////////////////////////////////////////////////////////////////
// TGraphSelNonSig

TGraphSelNonSig::TGraphSelNonSig(CGraphView* view) 
	: m_view(view)
{
}

kTGraph TGraphSelNonSig::TraverseNode(CLogNode* node)
{
	CWinLogData* data = (CWinLogData*)node->GetUserData();
	
	if( data->GetType() == kNodeRev )
	{
		if( node->Next() != 0L && node->Childs().size() == 0 )
			data->SetSelected(m_view, true);
	}
	
	return kTGetDown;
}

//////////////////////////////////////////////////////////////////////////
// CRevisionConsole

CRevisionConsole::CRevisionConsole() 
{
}

/*!
	Initialize console
	\param path File path
	\param fname File name
	\return true on success, false otherwise
*/
bool CRevisionConsole::Init(LPCTSTR path, LPCTSTR fname)
{
	TCHAR buf[_MAX_PATH] = "";

	_tmakepath(buf, NULL, path, fname, NULL);

	return CCvsStreamConsole::Open(buf);
}

//////////////////////////////////////////////////////////////////////////
// TGraphCurSelector

class TGraphCurSelector : public TGraph
{
	enum { CACHE_INTERVAL = 10 };
	
	void construct_()
	{
		CGraphDoc* pDoc = m_view ? (CGraphDoc*)m_view->GetDocument() : 0L;
		m_disknodeEntryInfo = pDoc ? pDoc->GetEntryInfo() : 0L;
	}

	TGraphCurSelector(int offset_from_sel, CGraphView* view, CLogNode* sel1, CLogNode* sel2)
		: m_offset(offset_from_sel), m_view(view),
		m_selId(-1), m_bRestart(true), m_sel1(sel1), m_sel2(sel2), 
		m_newSel(0L), m_firstSel(0L), m_lastSel(0L), m_diskNode(0L)
	{
		construct_();
	}

public:
	TGraphCurSelector(int offset_from_sel, CGraphView* view)
		: m_offset(offset_from_sel), m_view(view),
		m_selId(-1), m_bRestart(false), m_sel1(0L), m_sel2(0L), 
		m_newSel(0L), m_firstSel(0L), m_lastSel(0L), m_diskNode(0L)
	{
		construct_();
	}

private:
	int m_offset;

	CGraphView* m_view;
	
	int m_selId;
	bool m_bRestart; // for cache

	map<int, CLogNode*> m_restartPointerCache;
	
	CLogNode* m_sel1;
	CLogNode* m_sel2;

	CLogNode* m_newSel;
	
	CLogNode* m_firstSel; // might be root
	CLogNode* m_lastSel;
	
	CLogNode* m_diskNode;
	
	EntnodeData* m_disknodeEntryInfo;
	
	CWinLogData* getWinLogData_(CLogNode* node) const
	{
		return node ? (CWinLogData*)node->GetUserData() : 0L;
	}
public:
	CWinLogData* GetSel1() const
	{
		return getWinLogData_(m_sel1);
	}

	CWinLogData* GetSel2() const
	{
		return getWinLogData_(m_sel2);
	}

	CWinLogData* GetNewSel() const
	{
		return getWinLogData_(m_newSel ? m_newSel : m_firstSel);
	}

	CWinLogData* GetFirstSel() const
	{
		return getWinLogData_(m_firstSel);
	}

	CWinLogData* GetLastSel() const
	{
		return getWinLogData_(m_lastSel);
	}

	CWinLogData* GetDiskNodeSel() const
	{
		return getWinLogData_(m_diskNode);
	}

	virtual kTGraph TraverseNode(CLogNode* node)
	{
		++m_selId;

		if( !m_firstSel )
		{
			m_lastSel = m_firstSel = node;
		}

		if( !m_bRestart && (((m_selId % CACHE_INTERVAL == 0) && node->Next()) || !m_selId) )
		{
			m_restartPointerCache.insert(make_pair(m_selId, node));
		}

		if( ((CWinLogData*)node->GetUserData())->IsDiskNode(m_disknodeEntryInfo) )
		{
			m_diskNode = node;
		}

		CWinLogData* data = (CWinLogData*)node->GetUserData();
		if( data->Selected() )
		{
			if( !m_sel1 )
			{
				m_sel1 = node;
			}
			else if( !m_sel2 )
			{
				m_sel2 = node;
			}
		
			data->SetSelected(m_view, false);
		}

		if( m_sel1 && !m_newSel )
		{
			if( m_offset < 0 )
			{
				int target = m_selId + m_offset;
				target = (target > 0) ? target : 0;
				typedef map<int, CLogNode*>::iterator ITER_t_;
				typedef pair<ITER_t_, ITER_t_> ITERPAIR_t_;
				ITERPAIR_t_ itpair = m_restartPointerCache.equal_range(target);
				if( itpair.first != itpair.second )
				{
					m_newSel = (*(itpair.first)).second;
				}
				else
				{
					ITER_t_ it = itpair.first; // lower_bound
					if( it != m_restartPointerCache.begin() )
					{
						--it;
					}

					TGraphCurSelector internal(target - (*it).first, m_view, m_sel1, m_sel2);
					internal.TraverseGraph((*it).second);
					m_newSel = internal.m_newSel ? internal.m_newSel : m_lastSel;
				}
			}
			else if( m_offset == 0 )
			{
				m_newSel = node;
			}
			else
			{
				--m_offset;
			}
		}

		m_lastSel = node;
		
		return m_newSel && m_bRestart ? kTStop : kTGetDown;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CGraphView

CImageList CGraphView::m_imageList;

IMPLEMENT_DYNCREATE(CGraphView, CScrollView)

CGraphView::CGraphView()
{
	m_scrollToCurrentRev = true;

	if( m_imageList.m_hImageList == 0L )
		m_imageList.Create(IDB_SMALLICONS, 16, 1, RGB(255, 255, 255));

	LOGFONT lf = gGraphFont;
	if( *lf.lfFaceName )
	{
		m_font = new CFont();
		m_font->CreateFontIndirect(&lf);
	}
	else
	{
		m_font = NULL;
	}
}

CGraphView::~CGraphView()
{
	if( m_font )
	{
		m_font->DeleteObject();
		delete m_font;
	}
}


BEGIN_MESSAGE_MAP(CGraphView, CScrollView)
	//{{AFX_MSG_MAP(CGraphView)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_VIEW_RELOAD, OnViewReload)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RELOAD, OnUpdateViewReload)
	ON_COMMAND(ID_VIEW_DIFF, OnViewDiff)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DIFF, OnUpdateViewDiff)
	ON_COMMAND(ID_VIEW_UPDATE, OnViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE, OnUpdateViewUpdate)
	ON_COMMAND(ID_VIEW_TAG, OnViewTag)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAG, OnUpdateViewTag)
	ON_COMMAND(ID_VIEW_DELREV, OnViewDelrev)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DELREV, OnUpdateViewDelrev)
	ON_COMMAND(ID_VIEW_SELNONSIG, OnViewSelnonsig)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELNONSIG, OnUpdateViewSelnonsig)
	ON_COMMAND(ID_VIEW_RETRIEVE, OnViewRetrieve)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RETRIEVE, OnUpdateViewRetrieve)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RECURSIVE, DisableUpdate)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_QUERY_ANNOTATE, OnAnnotate)
	ON_COMMAND(ID_GRAPH_GRAPHOPTIONS, OnGraphGraphoptions)
	ON_COMMAND(ID_FORMAT_FONT, OnGraphFont)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ADMINOPTIONS_CHANGELOGMESSAGE, OnUpdateGraphChangelogmessage)
	ON_COMMAND(ID_GRAPH_ADMINOPTIONS_CHANGELOGMESSAGE, OnGraphChangelogmessage)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ADMINOPTIONS_SETDESCRIPTION, OnUpdateGraphSetdescription)
	ON_COMMAND(ID_GRAPH_ADMINOPTIONS_SETDESCRIPTION, OnGraphSetdescription)
	ON_COMMAND(ID_GRAPH_ADMINOPTIONS_SETSTATE, OnGraphSetstate)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_ADMINOPTIONS_SETSTATE, OnUpdateGraphSetstate)
	ON_COMMAND(ID_VIEW_UNLOCKF, OnViewUnlockf)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNLOCKF, OnUpdateViewUnlockf)
	ON_COMMAND(ID_VIEW_EDITSEL, OnViewEditsel)
	ON_COMMAND(ID_VIEW_EDITSELDEF, OnViewEditseldef)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_VIEW_RETRIEVEAS, OnViewRetrieveAs)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_VIEW_OPENSEL, OnViewOpensel)
	ON_COMMAND(ID_VIEW_OPENSELAS, OnViewOpenselas)
	ON_COMMAND(ID_QUERY_SHELLACTION_DEFAULT, OnQueryShellactionDefault)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGNEW, OnUpdateViewTagnew)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGDELETE, OnUpdateViewTagdelete)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGBRANCH, OnUpdateViewTagbranch)
	ON_COMMAND(ID_VIEW_TAGNEW, OnViewTagnew)
	ON_COMMAND(ID_VIEW_TAGDELETE, OnViewTagdelete)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITSELDEF, OnUpdateViewRetrieve)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RETRIEVEAS, OnUpdateViewRetrieve)
	ON_UPDATE_COMMAND_UI(ID_QUERY_ANNOTATE, OnUpdateViewRetrieve)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MISSING, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODIFIED, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONFLICT, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNKNOWN, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDEUNKNOWN, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IGNORE, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ADDED, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REMOVED, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHANGED, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILTERMASK, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITSEL, OnUpdateViewRetrieve)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLEARALL, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDEMISSING, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTERBAR_CLEARALL, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTERBAR_ENABLE, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTERBAR_OPTIONS, DisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPENSEL, OnUpdateViewRetrieve)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPENSELAS, OnUpdateViewRetrieve)
	ON_UPDATE_COMMAND_UI(ID_QUERY_SHELLACTION_DEFAULT, OnUpdateViewRetrieve)
	ON_COMMAND(ID_VIEW_TAGBRANCH, OnViewTagbranch)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphView drawing

/// Calculate the size of the graph and set the scroll bars
void CGraphView::CalcImageSize(void)
{
	const CPoint startPoint(STARTPOINT_PT);
	CWindowDC dc(this);
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	
	CLogNode* root = pDoc->GetNode();
	if( root == 0L )
		return;

	// Set all the bounds locations
	CWinLogData* const data = CWinLogData::CreateNewData(root);

	//root->SetUserData(data);
	_ASSERT(data != NULL);
	_ASSERT(root->GetUserData() == data);

	CFont* oldFont = NULL;
	if( m_font )
		oldFont = dc.SelectObject(m_font);

	data->ComputeBounds(startPoint, dc, pDoc->GetEntryInfo());
	
	if( oldFont)
		dc.SelectObject(oldFont);

	// Re-offset from (0, 0)
	CRect bounds = data->Bounds();
	data->Offset(startPoint - bounds.TopLeft());
	bounds = data->Bounds();

	// Get client rect to calculate the page size
	CRect client;
	GetClientRect(&client);

	// Set the scroll size, we add the margin of the start point offset on each side
	SetScrollSizes(MM_TEXT, 
		bounds.Size() + CSize(startPoint) + CSize(startPoint) + CSize(SHADOW_OFFSET_PT),
		client.Size() - data->SelfBounds().Size() - data->SelfBounds().Size());
}

/// OnInitialUpdate virtual override, set the initial scroll sizes
void CGraphView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	SetScrollSizes(MM_TEXT, CSize(200, 100));
	//CalcImageSize();
}

/// OnDraw virtual override, paint the graph
void CGraphView::OnDraw(CDC* const pDC)
{
	CGraphDoc* const pDoc = (CGraphDoc*)GetDocument();
	CLogNode*  const root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;

	CWinLogData* const data = (CWinLogData*)root->GetUserData();

	CFont* oldFont = NULL;
	if( m_font )
		oldFont = pDC->SelectObject(m_font);

	pDC->SetTextAlign(TA_CENTER);
	pDC->SetBkMode(TRANSPARENT);
	data->Update(*pDC, pDoc->GetEntryInfo());
	data->UpdateMergePoints(*pDC);

	if( oldFont)
		pDC->SelectObject(oldFont);

	if( m_scrollToCurrentRev )
	{
		m_scrollToCurrentRev = false;

		// Set the current revision to be selected
		TGraphCurSelector trav(0, this);
		trav.TraverseGraph(root);
		
		if( CWinLogData* const sel = trav.GetDiskNodeSel() )
		{
			sel->SetSelected(this, true);
			sel->ScrollTo(this);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGraphView diagnostics

#ifdef _DEBUG
void CGraphView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGraphView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphView message handlers

/// WM_LBUTTONDOWN message handler, update selection
void CGraphView::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;

	CWinLogData* data = (CWinLogData*)root->GetUserData();

	if( nFlags & MK_LBUTTON )
	{
		if((nFlags & MK_SHIFT) == 0 && (nFlags & MK_CONTROL) == 0)
			data->UnselectAll(this);
		
		point += GetDeviceScrollPosition();
		
		CWinLogData* hit = data->HitTest(point);
		if( hit != 0L )
		{
			if( (nFlags & MK_SHIFT) == 0 && (nFlags & MK_CONTROL) == 0 )
			{
				if( !hit->Selected() )
				{
					hit->SetSelected(this, true);
					CWincvsApp* app = (CWincvsApp*)AfxGetApp();
					CWincvsView* view = app->GetConsoleView();
					
					if( view != 0L )
					{
						CColorConsole out(view);
						CvsLogOutput(out, hit->Node());
					}
				}
			}
			else
			{
				hit->SetSelected(this, !hit->Selected());
			}
		}
		else
		{
			CGraphRectTracker track(this);
			point -= GetDeviceScrollPosition();
			if( track.TrackRubberBand(this, point, true) )
			{
				// Traverse and mark
				track.m_rect.OffsetRect(GetDeviceScrollPosition());
				track.m_rect.NormalizeRect();
				
				TGraphRectSelect traversal((nFlags & MK_CONTROL) != 0, this, track.m_rect);
				traversal.TraverseGraph(root);
			}
		}
	}
	else if( nFlags & MK_RBUTTON )
	{
		// nothing to do
	}

	CScrollView::OnLButtonDown(nFlags, point);
}

/// WM_COMMAND message handler, reload view
void CGraphView::OnViewReload() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();

	// Mark the reloading start
	pDoc->SetReloading(true);

	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;

	CvsArgs args;
	args.add("log");
	args.add(pDoc->GetName());
	args.print(pDoc->GetDir());

	string dir(pDoc->GetDir());
	CvsLogParse(dir.c_str(), args, true, pDoc);

	// Mark the end of reloading
	pDoc->SetReloading(false);
}

/// Update command UI state
void CGraphView::OnUpdateViewReload(CCmdUI* pCmdUI)
{
	if( DisableCommon() )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

/// WM_COMMAND message handler, diff the selection
void CGraphView::OnViewDiff() 
{
	KoGraphDiffHandler handler;
	TGraphCmdStatus trav;
	
	ProcessSelectionCommand(&handler, &trav);
}

/// Update command UI state
void CGraphView::OnUpdateViewDiff(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();

	TGraphCmdStatus trav;
	trav.TraverseGraph(root);
	
	const BOOL outEnabled = (trav.GetSelCount() == 2 && trav.GetSel1()->GetType() != kNodeHeader &&
		trav.GetSel2()->GetType() != kNodeHeader) ||
		(trav.GetSelCount() == 1 && trav.GetSel1()->GetType() != kNodeHeader);

	pCmdUI->Enable(outEnabled);
}

/// WM_COMMAND message handler, update
void CGraphView::OnViewUpdate() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;

	CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);

	CvsArgs args;
	args.add("update");
	args.add("-A");
	
	args.startfiles();
	args.add(pDoc->GetName());

	args.print(pDoc->GetDir());

	CGraphConsole cons;
	launchCVS(pDoc->GetDir(), args.Argc(), args.Argv(), &cons);
}

/// Update command UI state
void CGraphView::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

/// WM_COMMAND message handler, tag update
void CGraphView::OnViewTag() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	
	if( root == 0L || root->GetUserData() == 0L )
		return;

	CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);

	TGraphCmdStatus trav;
	trav.TraverseGraph(root);
	
	CvsArgs args;
	args.add("update");
	
	CLogNode* node = trav.GetSel1()->Node();
	switch( node->GetType() )
	{
	case kNodeHeader:
		args.add("-A");
		break;
	case kNodeRev:
	case kNodeBranch:
	case kNodeTag:
		args.add("-r");
		args.add(trav.GetSel1()->GetStr());
		break;
	}

	args.startfiles();
	args.add(pDoc->GetName());

	args.print(pDoc->GetDir());
	
	CGraphConsole cons;
	launchCVS(pDoc->GetDir(), args.Argc(), args.Argv(), &cons);
}

/// Update command UI state
void CGraphView::OnUpdateViewTag(CCmdUI* pCmdUI) 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();

	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	TGraphCmdStatus trav;
	trav.TraverseGraph(root);
	
	const BOOL outEnabled = trav.GetSelCount() == 1;
	pCmdUI->Enable(outEnabled);
}

/// OnBeginPrinting virtual override
void CGraphView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnBeginPrinting(pDC, pInfo);

	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L )
		return;

	// set all the bounds locations
	CWinLogData* const data = CWinLogData::CreateNewData(root);

	//root->SetUserData(data);
	_ASSERT(data != NULL);
	_ASSERT(root->GetUserData() == data);

	CPoint start(0, 0);
	data->ComputeBounds(start, *pDC, pDoc->GetEntryInfo());

	// compute the number of pages
	CRect bounds = data->Bounds();
	m_pageHeight = pDC->GetDeviceCaps(VERTRES);
	m_numberOfPages = bounds.Height() / m_pageHeight + 1;

	pInfo->SetMaxPage(m_numberOfPages);
}

/// OnEndPrinting virtual override
void CGraphView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnEndPrinting(pDC, pInfo);

	CalcImageSize();
	Invalidate();
}

/// OnPreparePrinting virtual override
BOOL CGraphView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	BOOL res = DoPreparePrinting(pInfo);
	if( !res )
		return res;

	return TRUE;
}

/// OnPrint virtual override
void CGraphView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if( !pDC->IsPrinting() ) 
		return;

	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;

	CWinLogData* data = (CWinLogData*)root->GetUserData();
	
	// Orient the viewport so that the first row to be printed
	// has a viewport coordinate of (0,0).
	int yTopOfPage = (pInfo->m_nCurPage -1) * m_pageHeight;
	pDC->SetViewportOrg(0, -yTopOfPage);
	pDC->SetTextAlign(TA_CENTER);
	pDC->SetBkMode(TRANSPARENT);
	data->Update(*pDC, pDoc->GetEntryInfo());
	
	//	CScrollView::OnPrint(pDC, pInfo);
}

/*!
	Check whether the commands should be disabled
	\return true if commands should be disabled, false otherwise
*/
bool CGraphView::DisableCommon()
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();

	return root == 0L || root->GetUserData() == 0L || app->IsCvsRunning() || this != CWnd::GetFocus();
}

/*!
	Check whether a single node is selected
	\param type Node type
	\param equal true if selection should be equal, false otherwise
	\return true if the selection is a single node, false otherwise
*/
bool CGraphView::IsSelSingleNode(kLogNode type, bool equal /*= true*/)
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	
	TGraphCmdStatus trav;
	trav.TraverseGraph(root);
	
	return (trav.GetSelCount() == 1 && 
		(equal ? trav.GetSel1()->GetType() == type : trav.GetSel1()->GetType() != type));
}

/// WM_COMMAND message handler, delete selected revisions
void CGraphView::OnViewDelrev() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;

	CvsAlert cvsAlert(kCvsAlertQuestionIcon, 
		"Do you really want to delete selected revisions?", "Deleting file revisions will permanently remove the revisions on the server.", 
		BUTTONTITLE_YES, BUTTONTITLE_NO);

	if( cvsAlert.ShowAlert(kCvsAlertCancelButton) != kCvsAlertOKButton )
	{
		return;
	}

	CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);

	TGraphDelCmd trav;
	trav.TraverseGraph(root);

	CProgress progress(true, "Deleting revisions...");

	int count = 0;
	for(vector<string>::const_iterator i = trav.GetAllRevs().begin(); i != trav.GetAllRevs().end(); ++i, ++count)
	{
		progress.SetProgress((int)(((float)count / (float)trav.GetAllRevs().size()) * 100.0 + .5));
		if( progress.HasAborted() )
			break;

		CvsArgs args;
		args.add("admin");
		args.add("-o");
		args.add(i->c_str());

		args.startfiles();
		args.add(pDoc->GetName());

		args.print(pDoc->GetDir());
		
		CGraphConsole cons;
		launchCVS(pDoc->GetDir(), args.Argc(), args.Argv(), &cons);
	}
	
	if( !progress.HasAborted() )
		progress.SetProgress(100);
}

/// Update command UI state
void CGraphView::OnUpdateViewDelrev(CCmdUI* pCmdUI) 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	TGraphDelCmd trav;
	trav.TraverseGraph(root);

	BOOL outEnabled = trav.GetCountSel() >= 1 && trav.GetOnlyRev();
	pCmdUI->Enable(outEnabled);
}

/// WM_COMMAND message handler, select non-significant revisions
void CGraphView::OnViewSelnonsig() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;

	CWinLogData* data = (CWinLogData*)root->GetUserData();
	data->UnselectAll(this);

	TGraphSelNonSig trav(this);
	trav.TraverseGraph(root);
}

/// Update command UI state
void CGraphView::OnUpdateViewSelnonsig(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

/// WM_COMMAND message handler, retrieve revisions
void CGraphView::OnViewRetrieve() 
{
	SaveSel();
}

void CGraphView::OnViewRetrieveAs() 
{
	SaveSel(true);
}

void CGraphView::SaveSel(bool queryName /*= false*/)
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();

	if( root == 0L || root->GetUserData() == 0L )
		return;

	TGraphCmdStatus trav;
	trav.TraverseGraph(root);

	CvsArgs args;

	string fileName;
	fileName = pDoc->GetDir();
	fileName += pDoc->GetName();
	fileName += ".#.";
	fileName += trav.GetSel1()->GetStr();

	if( !queryName || BrowserGetSaveasFile("Select the file name to save to", fileName, kSelectAny) )
	{
		args.add("update");
		args.add("-p");
		
		args.add("-r");
		args.add(trav.GetSel1()->GetStr());
		
		args.startfiles();
		args.add(pDoc->GetName());
		
		args.print(pDoc->GetDir());

		string outDir, outFile;
		SplitPath(fileName.c_str(), outDir, outFile);

		CRevisionConsole console;
		console.Init(outDir.c_str(), outFile.c_str()); 

		int code = launchCVS(pDoc->GetDir(), args.Argc(), args.Argv(), &console);
	}
}

/// Update command UI state
void CGraphView::OnUpdateViewRetrieve(CCmdUI* pCmdUI) 
{
	BOOL enabled = FALSE;
	if( !DisableCommon() )
	{
		enabled = IsSelSingleNode(kNodeHeader, false);
	}

	pCmdUI->Enable(enabled);
}

/// Update command UI state, disable command
void CGraphView::DisableUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);	
}

/*!
	Display context menu
	\param pWnd Window
	\param point Mouse position
*/
void CGraphView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// Comply with windows UI standard (Windows Explorer file view used as a reference),
	// change selections according to the state of Control and Shift keys
	BYTE abKeyboardState[256];
	BOOL bControlState = FALSE;
	BOOL bShiftState = FALSE;
	memset(abKeyboardState, 0, 256);
	
	if( GetKeyboardState(abKeyboardState) )
	{
		bControlState = abKeyboardState[VK_CONTROL] & 0x80;
		bShiftState = abKeyboardState[VK_SHIFT] & 0x80;
	}
	
	if( (!bControlState || (bControlState && bShiftState)) && !DisableCommon() )
	{	
		CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
		CLogNode* root = pDoc->GetNode();
		
		TGraphCmdStatus trav;
		trav.TraverseGraph(root);

		CWinLogData* data = (CWinLogData*)root->GetUserData();

		// Set the track point in case it's not the mouse click
		if( -1 == point.x && -1 == point.y )
		{
			// Set to top-left corner
			point = STARTPOINT_PT;
			point.x /= 2;
			point.y /= 2;

			ClientToScreen(&point);
		}
		
		CPoint hitPoint(point);
		
		ScreenToClient(&hitPoint);
		hitPoint += GetDeviceScrollPosition();
		
		CWinLogData* hit = data->HitTest(hitPoint);

		if( hit != 0L )
		{
			if( trav.GetSelCount() == 0 )
			{
				hit->SetSelected(this, true);
			}
			else if( trav.GetSelCount() >= 1 && trav.GetSel1()->GetType() != kNodeHeader )
			{
				if( !(hit->Selected() && bShiftState) && 
					!hit->Selected() &&
					!(bControlState && bShiftState && hit->Selected()) )
				{
					data->UnselectAll(this);
				}

				hit->SetSelected(this, true);
			}
		}
	}
	
	// Get the menu to display from the main app window, which for graph view
	// will be IDR_GRAPH,  The submenu to display is the menu under "Graph".
	CMenu* pSubMenu = GetTopPopupMenu("&Graph");
	if( pSubMenu )
	{
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	else
	{
		ASSERT(FALSE); // Popup menu not found
	}
}

/// WM_INITMENUPOPUP message handler
void CGraphView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if( AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu )
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if( (hParentMenu = ::GetMenu(m_hWnd)) != NULL )
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if( pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL )
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for(int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if( ::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu )
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for(state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if( state.m_nID == 0 )
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if( state.m_nID == (UINT)-1 )
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if( state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1 )
			{
				continue;       // First item of popup can't be routed to.
			}

			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}
		
		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if( nCount < state.m_nIndexMax )
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while( state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID )
			{
				state.m_nIndex++;
			}
		}

		state.m_nIndexMax = nCount;
	}
}

/// WM_COMMAND message handler, annotate selection
void CGraphView::OnAnnotate() 
{
	KoGraphAnnotateHandler handler;
	TGraphCmdStatus trav;

	ProcessSelectionCommand(&handler, &trav);
}

/// WM_COMMAND message handler, display graph options menu
void CGraphView::OnGraphGraphoptions() 
{
	if( CompatGetGraphOptions(gGraphSelColor, gGraphShadowColor, 
		gGraphHeaderColor, gGraphTagColor, gGraphBranchColor, 
		gGraphNodeColor, gGraphDeadNodeColor, gGraphUserStateNodeColor, 
		gGraphMergeColor) )
	{
		// add any additional processing here
	}

	if( CWincvsApp* pApp = (CWincvsApp*)AfxGetApp() )
	{
		pApp->UpdateAllDocsView();
	}
}

/// WM_COMMAND message handler, select graph font
void CGraphView::OnGraphFont() 
{
	LOGFONT lf = gGraphFont;

	CFontDialog dlg(&lf);
	dlg.m_cf.Flags &= ~CF_EFFECTS;
	
	if( dlg.DoModal() != IDOK )
		return;
	
	gGraphFont = *dlg.m_cf.lpLogFont;
	
	if( m_font )
		m_font->DeleteObject();
	else
		m_font = new CFont();
	
	m_font->CreateFontIndirect(dlg.m_cf.lpLogFont);
	
	CalcImageSize();
	Invalidate();
	
	if( CWincvsApp* pApp = (CWincvsApp*)AfxGetApp() )
	{
		pApp->UpdateAllDocsView(this, kGraphFontChanged);
	}
}

/// Update command UI state
void CGraphView::OnUpdateGraphChangelogmessage(CCmdUI* pCmdUI) 
{
	BOOL enabled = FALSE;
	if( !DisableCommon() )
	{
		enabled = IsSelSingleNode(kNodeRev);
	}

	pCmdUI->Enable(enabled);
}

/// WM_COMMAND message handler, change the log message
void CGraphView::OnGraphChangelogmessage() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;
	
	TGraphCmdStatus trav;
	trav.TraverseGraph(root);
	
	CLogNode* node = trav.GetSel1()->Node();
	ASSERT(node->GetType() == kNodeRev);
	const CRevFile& rev = **(CLogNodeRev*)node;
	
	string msg = rev.DescLog().c_str();
	
	MultiFiles mf;
	
	mf.newdir(pDoc->GetDir());
	mf.newfile(pDoc->GetName());

	if( CompatGetAdminOptions(&mf, kChangeLog, msg) )
	{
		CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);

		CvsArgs args;
		args.add("admin");

		string changeOption = string("-m") + trav.GetSel1()->GetStr() + string(":") + NormalizeLogMsg(msg);
		args.add(changeOption.c_str());

		args.startfiles();
		args.add(pDoc->GetName());

		args.print(pDoc->GetDir());
		
		CGraphConsole cons;
		launchCVS(pDoc->GetDir(), args.Argc(), args.Argv(), &cons);
	}
}

/// Update command UI state
void CGraphView::OnUpdateGraphSetdescription(CCmdUI* pCmdUI) 
{
	BOOL enabled = FALSE;
	if( !DisableCommon() )
	{
		enabled = IsSelSingleNode(kNodeHeader);
	}

	pCmdUI->Enable(enabled);
}

/// WM_COMMAND message handler, set the description
void CGraphView::OnGraphSetdescription() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;
	
	TGraphCmdStatus trav;
	trav.TraverseGraph(root);
	
	CLogNode* node = trav.GetSel1()->Node();
	ASSERT(node->GetType() == kNodeHeader);
	const CRcsFile& rev = **(CLogNodeHeader*)node;
	
	string msg = rev.DescLog().c_str();
	
	MultiFiles mf;
	
	mf.newdir(pDoc->GetDir());
	mf.newfile(pDoc->GetName());
	
	if( CompatGetAdminOptions(&mf, kSetDescription, msg) )
	{
		CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);

		CvsArgs args;
		args.add("admin");

		string changeOption = string("-t-") + NormalizeLogMsg(msg);
		args.add(changeOption.c_str());
		
		args.startfiles();
		args.add(pDoc->GetName());

		args.print(pDoc->GetDir());
		
		CGraphConsole cons;
		launchCVS(pDoc->GetDir(), args.Argc(), args.Argv(), &cons);
	}
}

/// Update command UI state
void CGraphView::OnUpdateGraphSetstate(CCmdUI* pCmdUI) 
{
	BOOL enabled = FALSE;
	if( !DisableCommon() )
	{
		enabled = IsSelSingleNode(kNodeRev);
	}

	pCmdUI->Enable(enabled);
}

/// WM_COMMAND message handler, set the state
void CGraphView::OnGraphSetstate() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;
	
	TGraphCmdStatus trav;
	trav.TraverseGraph(root);

	CLogNode* node = trav.GetSel1()->Node();
	ASSERT(node->GetType() == kNodeRev);
	const CRevFile& rev = **(CLogNodeRev*)node;

	string state = rev.State().c_str();

	CvsAlert cvsAlert(kCvsAlertStopIcon, 
		_i18n("Are you sure you want to continue?"), _i18n("The revision state is: \"dead\".\nYou should not alter it manually."),
		BUTTONTITLE_CONTINUE, BUTTONTITLE_CANCEL);

	if( strcmp("dead", state.c_str()) == 0 && cvsAlert.ShowAlert() != kCvsAlertOKButton )
	{
		return;
	}

	MultiFiles mf;
	
	mf.newdir(pDoc->GetDir());
	mf.newfile(pDoc->GetName());

	if( CompatGetAdminOptions(&mf, kSetState, state) )
	{
		CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);

		CvsArgs args;
		args.add("admin");
		args.add("-s");

		string changeOption = NormalizeLogMsg(state) + string(":") + trav.GetSel1()->GetStr();
		args.add(changeOption.c_str());
		
		args.startfiles();
		args.add(pDoc->GetName());

		args.print(pDoc->GetDir());
		
		CGraphConsole cons;
		launchCVS(pDoc->GetDir(), args.Argc(), args.Argv(), &cons);
	}
}

/// Update command UI state
void CGraphView::OnUpdateViewUnlockf(CCmdUI* pCmdUI) 
{
	BOOL enabled = FALSE;
	if( !DisableCommon() )
	{
		enabled = IsSelSingleNode(kNodeRev);
	}

	pCmdUI->Enable(enabled);
}

/// WM_COMMAND message handler, unlock the selection
void CGraphView::OnViewUnlockf() 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == 0L || root->GetUserData() == 0L )
		return;
	
	CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);

	TGraphCmdStatus trav;
	trav.TraverseGraph(root);
	
	CvsArgs args;
	args.add("admin");
	args.add(string(string("-u") + trav.GetSel1()->GetStr()).c_str());
	
	args.startfiles();
	args.add(pDoc->GetName());

	args.print(pDoc->GetDir());
	
	CGraphConsole cons;
	launchCVS(pDoc->GetDir(), args.Argc(), args.Argv(), &cons);
}

/// WM_COMMAND message handler, edit selection
void CGraphView::OnViewEditsel() 
{
	string file;
	if( RetrieveSel(file) )
	{
		LaunchHandler(kLaunchEdit, file.c_str());
	}
}

/// WM_COMMAND message handler, edit selection using default editor
void CGraphView::OnViewEditseldef() 
{
	string file;
	if( RetrieveSel(file) )
	{
		LaunchHandler(kLaunchDefaultEdit, file.c_str());
	}
}

/// WM_COMMAND message handler, launch default associated shell action
void CGraphView::OnQueryShellactionDefault() 
{
	string file;
	if( RetrieveSel(file) )
	{
		LaunchHandler(kLaunchDefault, file.c_str());
	}
}

/*!
	Retrieve the selected revision
	\param file Return the file name of the retrieved revision
	\return true on success, false otherwise
*/
bool CGraphView::RetrieveSel(std::string& file)
{
	bool res = false;

	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	
	if( root && root->GetUserData() )
	{
		TGraphCmdStatus trav;
		trav.TraverseGraph(root);
		
		string prf, ext, base;
		CvsArgs args;
		
		SplitFilename(pDoc->GetName(), base, ext);
		
		prf = base;
		prf += "_view_";
		prf += trav.GetSel1()->GetStr();
		prf += '_';
		
		args.add("update");
		args.add("-p");
		
		args.add("-r");
		args.add(trav.GetSel1()->GetStr());
		
		args.startfiles();
		args.add(pDoc->GetName());
		
		args.print(pDoc->GetDir());
		
		file = launchCVS(pDoc->GetDir(), args, prf.c_str(), ext.c_str());
		if( !file.empty() )
		{
			res = true;
		}
	}

	return res;
}

/// WM_LBUTTONDBLCLK message handler, view the selection via file action
void CGraphView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	FileAction();
	
	CScrollView::OnLButtonDblClk(nFlags, point);
}

/*!
	OnScroll virtual override to fix MFC bug when graph window is larger than 32K
	\note See kb166473 (http://support.microsoft.com/default.aspx?scid=kb;en-us;166473)
*/
BOOL CGraphView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll) 
{
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_TRACKPOS;
	
	if( LOBYTE(nScrollCode) == SB_THUMBTRACK )
	{
		GetScrollInfo(SB_HORZ, &info);
		nPos = info.nTrackPos;
	}
	
	if( HIBYTE(nScrollCode) == SB_THUMBTRACK )
	{
		GetScrollInfo(SB_VERT, &info);
		nPos = info.nTrackPos;
	}
	
	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}

/*!
	WM_KEYDOWN message handler, move the selection thru the nodes:
	- Esc to cancel selection
	- Enter to edit selection (just as mouse double-click does)
	- Space to print the log info (just as mouse click does)
	- Ctrl+Home to go to the current revision
	- Up, Down, PgUp, PgDown to navigate thru the nodes vertically
	- Left, Right to navigate thru branches and tags horizontally
	- Home, End to go to the bottom or top of the graph
*/
void CGraphView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* node = pDoc->GetNode();
	UINT nCharTmp = (nChar == VK_LEFT ? VK_UP : (nChar == VK_RIGHT ? VK_DOWN : nChar));

	switch( nCharTmp )
	{
	case VK_ESCAPE:
		// Unselect all
		if( node && node->GetUserData() /*&& GetAsyncKeyState(VK_CONTROL) != 0*/ )
		{
			CWinLogData* data = (CWinLogData*)node->GetUserData();
			data->UnselectAll(this);
		}
		break;
	case VK_RETURN:
		FileAction();
		break;
	case VK_SPACE:
		// Print selection info
		{
			CWincvsApp* app = (CWincvsApp*)AfxGetApp();
			CWincvsView* view = app->GetConsoleView();
			
			if( view != 0L )
			{
				TGraphCmdStatus trav;
				trav.TraverseGraph(node);

				if( trav.GetSel1() )
				{
					CColorConsole out(view);
					CvsLogOutput(out, trav.GetSel1()->Node());
				}
			}
		}
		break;
	case VK_PRIOR:
	case VK_NEXT:
	case VK_UP:
	case VK_DOWN:
	case VK_HOME: // FIXME -> go to current rev., Ctrl+Home, Ctrl+End -> go to top/bottom of graph
	case VK_END:
		// Navigation
		{
			if( node == NULL || node->GetUserData() == NULL || pDoc == NULL )
			{
				break;
			}

			int offset = (nCharTmp == VK_DOWN || nCharTmp == VK_NEXT) ? 1 : -1;
			offset *= (nCharTmp == VK_NEXT || nCharTmp == VK_PRIOR) ? 10 : 1;
			
			const bool ctrlOn = GetAsyncKeyState(VK_CONTROL) < 0;
			const bool shiftOn = GetAsyncKeyState(VK_SHIFT) < 0;
			
			TGraphCurSelector trav(offset, this);
			trav.TraverseGraph(node);
			
			//CWinLogData* origsel1 = trav.GetSel1();
			CWinLogData* origsel2 = trav.GetSel2() ? trav.GetSel2() : (ctrlOn ? trav.GetSel1() : 0L);
			CWinLogData* newsel;
			
			if( nCharTmp == VK_HOME && ctrlOn )
			{
				newsel = trav.GetDiskNodeSel();
			}
			else if( nCharTmp == VK_END ||
				((nCharTmp == VK_DOWN || nCharTmp == VK_NEXT) && trav.GetNewSel() && trav.GetNewSel() == node->GetUserData()) )
			{
				newsel = trav.GetLastSel();
			}
			else if( nCharTmp != VK_HOME && nCharTmp != VK_END )
			{
				newsel = trav.GetNewSel();
			}
			else
			{
				newsel = (CWinLogData*)node->GetUserData();
			}

			if( !newsel )
			{
				break;
			}
			
			if( !shiftOn )
			{
				CWinLogData* data = (CWinLogData*)node->GetUserData();
				data->UnselectAll(this);
			}
			else if( origsel2 && ctrlOn )
			{
				origsel2->SetSelected(this, true);
			}
			
			newsel->SetSelected(this, true);
			newsel->ScrollTo(this);
		}
		break;
	}
	
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/// WM_COMMAND message handler, open selection
void CGraphView::OnViewOpensel() 
{
	string file;
	if( RetrieveSel(file) )
	{
		LaunchHandler(kLaunchOpen, file.c_str());
	}
}

/// WM_COMMAND message handler, open selection using Open With... dialog
void CGraphView::OnViewOpenselas() 
{
	string file;
	if( RetrieveSel(file) )
	{
		LaunchHandler(kLaunchOpenAs, file.c_str());
	}
}

/*!
	File action
*/
void CGraphView::FileAction()
{
	if( !DisableCommon() && IsSelSingleNode(kNodeHeader, false) )
	{
		string file;
		if( RetrieveSel(file) )
		{
			MultiFiles mf;
			
			string uppath, folder;
			if( SplitPath(file.c_str(), uppath, folder) )
			{
				mf.newdir(uppath.c_str());
				mf.newfile(folder.c_str());
				
				KoFileActionHandler handler;
				handler.OnFiles(&mf);
			}
		}
	}
}

/// OnUpdate virtual override, update after graph options change (e.g. fonts)
void CGraphView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if( kGraphFontChanged == (tUpdateHint)lHint )
	{
		LOGFONT lf = gGraphFont;
		if( *lf.lfFaceName )
		{
			if( m_font )
			{
				m_font->DeleteObject();
				delete m_font;
			}
			
			m_font = new CFont();
			m_font->CreateFontIndirect(&lf);
		}
		
		CalcImageSize();
		Invalidate();
	}
}

/// Update command UI state
void CGraphView::OnUpdateViewTagnew(CCmdUI* pCmdUI) 
{
	BOOL enabled = FALSE;
	if( !DisableCommon() )
	{
		enabled = IsSelSingleNode(kNodeHeader, false);
	}
	
	pCmdUI->Enable(enabled);
}

/// Update command UI state
void CGraphView::OnUpdateViewTagdelete(CCmdUI* pCmdUI) 
{
	BOOL enabled = FALSE;
	if( !DisableCommon() )
	{
		enabled = IsSelSingleNode(kNodeTag, true);
	}
	
	pCmdUI->Enable(enabled);
}

/// Update command UI state
void CGraphView::OnUpdateViewTagbranch(CCmdUI* pCmdUI) 
{
	BOOL enabled = FALSE;
	if( !DisableCommon() )
	{
		enabled = IsSelSingleNode(kNodeHeader, false);
	}
	
	pCmdUI->Enable(enabled);
}

/// WM_COMMAND message handler, create tag
void CGraphView::OnViewTagnew() 
{
	CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);
	
	CGraphConsole cons;
	KoGraphCreateTagHandler handler(&cons);
	TGraphCmdStatus trav;
	
	ProcessSelectionCommand(&handler, &trav);
}

/// WM_COMMAND message handler, delete tag
void CGraphView::OnViewTagdelete() 
{
	CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);
	
	CGraphConsole cons;
	KoGraphDeleteTagHandler handler(&cons);
	TGraphCmdStatus trav;
	
	ProcessSelectionCommand(&handler, &trav);
}

/// WM_COMMAND message handler, create branch tag
void CGraphView::OnViewTagbranch() 
{
	CWndAutoCommand oWndAutoCommand(this, ID_VIEW_RELOAD);
	
	CGraphConsole cons;
	KoGraphBranchTagHandler handler(&cons);
	TGraphCmdStatus trav;
	
	ProcessSelectionCommand(&handler, &trav);
}

/*!
	Process the selection handler command
	\param handler Command selection handler
	\param trav Traverse object
	\return true if the command was processed, false otherwise
*/
bool CGraphView::ProcessSelectionCommand(KiSelectionHandler* handler, TGraphCmdStatus* trav)
{
	bool res = false;

	CGraphDoc* pDoc = (CGraphDoc*)GetDocument();
	CLogNode* root = pDoc->GetNode();
	if( root == NULL || root->GetUserData() == NULL )
		return false;
	
	trav->TraverseGraph(root);
	
	MultiFiles mf;
	
	mf.newdir(pDoc->GetDir());
	mf.newfile(pDoc->GetName(), 0L, trav->GetSel1()->GetStr());

	if( trav->GetSelCount() == 2 )
	{
		mf.newfile(pDoc->GetName(), 0L, trav->GetSel2()->GetStr());
	}

	if( mf.TotalNumFiles() != 0 )
	{
		handler->OnFiles(&mf);
		res = true;
	}
	
	return res;
}
