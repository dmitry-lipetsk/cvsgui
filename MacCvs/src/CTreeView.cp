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
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- June 1998
 */

/*
 * CTreeView.cpp --- main tree view
 */
#include <unistd.h>
#include <sys/stat.h>

#include "CTreeView.h"

#include <LStaticText.h>

#include "MacCvsApp.h"
#include "MacCvsConstant.h"
#include "AppGlue.h"
#include "MultiFiles.h"
#include "CvsLog.h"
#include "CvsEntries.h"
#include "LogParse.h"
#include "AppConsole.h"
#include "CBrowserTable.h"
#include "CvsCommands.h"
#include "CvsArgs.h"
#include "BrowseViewHandlers.h"
#include "MacMisc.h"
#include <algorithm>

#if (defined(__MWERKS__) && __MSL__ >= 0x6000) || defined(__GNUC__)
	using std::min;
	using std::max;
#endif

static RGBColor kdblue = {0x0, 0x0, 0x8000};
static RGBColor kblue = {0x0, 0x0, 0xFFFF};
static RGBColor kwhi = {0xFFFF, 0xFFFF, 0xFFFF};
static RGBColor kselc = {0xa000, 0xa000, 0xa000};
static RGBColor kbl = {0x0, 0x0, 0x0};
static RGBColor kdgray = {0x8000, 0x8000, 0x8000};
static RGBColor kdred = {0x8000, 0x0, 0x0};
static RGBColor kred = {0xFFFF, 0x0, 0x0};

class CPoint : public SPoint32
{
public:
	inline CPoint() {}

	inline CPoint(SInt32 x, SInt32 y)
	{
		h = x;
		v = y;
	}

	inline CPoint(const Point & pt)
	{
		h = pt.h;
		v = pt.v;
	}

	inline CPoint(const CPoint & pt)
	{
		h = pt.h;
		v = pt.v;
	}

	inline CPoint operator-(const CPoint & pt) const
	{
		return CPoint(h - pt.h, v - pt.v);
	}

	inline CPoint & operator+=(const CPoint & pt)
	{
		h += pt.h;
		v += pt.v;
		return *this;
	}

	inline CPoint operator+(const CPoint & pt)
	{
	  CPoint  result(*this);
		result.h += pt.h;
		result.v += pt.v;
		return result;
	}

	inline CPoint & operator+=(const Point & pt)
	{
		h += pt.h;
		v += pt.v;
		return *this;
	}
};

class CRect : public SRect32
{
public:
	inline CRect() {}
	inline CRect(SInt32 x1, SInt32 y1, SInt32 x2, SInt32 y2)
	{
		left = x1;
		top = y1;
		right = x2;
		bottom = y2;
	}

	inline CPoint TopLeft(void) const { return CPoint(left, top); }
	inline CPoint BottomRight(void) const { return CPoint(right, bottom); }
	inline CPoint CenterPoint(void) const
	{
		return CPoint((left + right) / 2, (top + bottom) / 2);
	}
	
	inline void SetRectEmpty(void)
	{
		left = right = bottom = top = 0;
	}

	inline bool IsEmpty(void) const
	{
		return right <= left || bottom <= top;
	}

	inline void SetRect(SInt32 x1, SInt32 y1, SInt32 x2, SInt32 y2)
	{
		left = x1;
		top = y1;
		right = x2;
		bottom = y2;
	}

	inline void OffsetRect(const CPoint & off)
	{
		left += off.h;
		top += off.v;
		right += off.h;
		bottom += off.v;
	}
	
	inline SInt32 Width(void) const
	{
		return right - left + 1;
	}

	inline SInt32 Height(void) const
	{
		return bottom - top + 1;
	}
	
	inline void UnionRect(const CRect & r1, const CRect & r2)
	{
		if(r1.IsEmpty())
		{
			*this = r2;
			return;
		}
		if(r2.IsEmpty())
		{
			*this = r1;
			return;
		}

		left = min(r1.left, r2.left);
		right = max(r1.right, r2.right);
		top = min(r1.top, r2.top);
		bottom = max(r1.bottom, r2.bottom);
	}
	
	inline bool PtInRect(const CPoint & pt) const
	{
		return pt.h >= left && pt.h < right &&
			pt.v >= top && pt.v < bottom;
	}
	
	inline operator Rect *(void) const
	{
		static Rect res;
		::SetRect(&res, left, top, right, bottom);
		return &res;
	}

	inline CRect & operator=(const CRect & r)
	{
		left = r.left;
		right = r.right;
		top = r.top;
		bottom = r.bottom;
		return *this;
	}
	
	CRect IntersectWith(const CRect& inRhs) const
	{
  	return CRect(std::max(left,inRhs.left), std::max(top,inRhs.top),
  									std::min(right,inRhs.right), std::min(bottom,inRhs.bottom));
  }
  
	bool Intersects(const CRect& inRhs) const
	{
    return !this->IntersectWith(inRhs).IsEmpty();
	}
};

class CSize : public SDimension32
{
public:
	inline CSize(SInt32 x, SInt32 y)
	{
		width = x;
		height = y;
	}
};

const int kVChildSpace = 8;
const int kHChildSpace = 40;
const int kInflateNodeSpace = 8;

namespace {
  
  Rect ImageToLocalRect(const LView& inView, const CRect& inImageRect)
  {
    Rect    localRect;
    inView.ImageToLocalPoint(inImageRect.TopLeft(), *(Point*)&localRect.top);
    inView.ImageToLocalPoint(inImageRect.BottomRight(), *(Point*)&localRect.bottom);
    return localRect;
  }
  void ImageToLocalPoint(const LView& inView, const CPoint& inImagePoint, Point& outLocalPoint)
  {
    inView.ImageToLocalPoint(inImagePoint, outLocalPoint);
  }
  Point ImageToLocalPoint(const LView& inView, const CPoint& inImagePoint)
  {
    Point   localPoint;
    ImageToLocalPoint(inView, inImagePoint, localPoint);
    return localPoint;
  }
  void PortToImagePoint(const LView& inView, const Point& inPortPoint, CPoint& outImagePoint)
  {
    Point   pt(inPortPoint);
    inView.PortToLocalPoint(pt);
    inView.LocalToImagePoint(pt, outImagePoint);
  }
  CRect LocalToImageRect(const LView& inView, const Rect& inLocalRect)
  {
    CPoint    tl, br;
    inView.LocalToImagePoint(*(const Point*)&inLocalRect.top, tl);
    inView.LocalToImagePoint(*(const Point*)&inLocalRect.bottom, br);
    return CRect(tl.h, tl.v, br.h, br.v);
  }
  CRect PortToImageRect(const LView& inView, const Rect& inPortRect)
  {
    CPoint    tl, br;
    PortToImagePoint(inView, *(const Point*)&inPortRect.top, tl);
    PortToImagePoint(inView, *(const Point*)&inPortRect.bottom, br);
    return CRect(tl.h, tl.v, br.h, br.v);
  }
  void ImageToPortPoint(const LView& inView, const CPoint& inImagePoint, Point& outPortPoint)
  {
    inView.ImageToLocalPoint(inImagePoint, outPortPoint);
    inView.LocalToPortPoint(outPortPoint);
  }
  Point ImageToPortPoint(const LView& inView, const CPoint& inImagePoint)
  {
    Point   portPoint;
    ImageToPortPoint(inView, inImagePoint, portPoint);
    return portPoint;
  }
  Rect ImageToPortRect(const LView& inView, const CRect& inImageRect)
  {
    Rect    portRect;
    ImageToPortPoint(inView, inImageRect.TopLeft(), *(Point*)&portRect.top);
    ImageToPortPoint(inView, inImageRect.BottomRight(), *(Point*)&portRect.bottom);
    return portRect;
  }

  void Polyline(const LView& view, const CRect& updateRect, const CPoint pts[], int numPoints)
  {
    ///\todo honor updateRect; 16-bit coordinate space limitations
		Point   pt;
		ImageToLocalPoint(view, pts[0], pt);
		MoveTo(pt.h, pt.v);
		for ( int i = 1; i < numPoints; ++i ) {
		  ImageToLocalPoint(view, pts[i], pt);
		  LineTo(pt.h, pt.v);
		}
  }

} // end of anonymous namespace

class CMacLogData : public CLogNodeData
{
public:
	CMacLogData(CLogNode *node);
	virtual ~CMacLogData();

	inline kLogNode GetType(void) const { return fNode->GetType(); }
	const char *GetStr(void);

	void ComputeBounds(const CPoint & topLeft, CTreeView * view,
		EntnodeData *entryInfo = 0L);
	void Offset(const CPoint & o);
	void Update(CTreeView * view, const CRect& inUpdateRect, EntnodeData *entryInfo = 0L);
  void UpdateMergePoints(CTreeView * view, const CRect& updateRect);
	
	inline CRect & Bounds(void) { return totB; }
	inline CRect & SelfBounds(void) { return selfB; }

	inline bool Selected(void) const { return sel; }
	void SetSelected(CTreeView * view, bool state);
	void UnselectAll(CTreeView * view);
	CMacLogData *HitTest(const CPoint & point);

	static CMacLogData *CreateNewData(CLogNode *node);

	bool IsDiskNode(EntnodeData *entryInfo);

  CMacLogData* GetFirstSelected(CMacLogData **inOutParentNode=NULL, CMacLogData **inOutPreviousNode=NULL);
  CMacLogData* GetLastSelected(CMacLogData **inOutParentNode=NULL, CMacLogData **inOutPreviousNode=NULL);
  CMacLogData* GetDiskNode(EntnodeData *entryInfo);

  CMacLogData* Next();
  CMacLogData* NextChild(CMacLogData* inCurChild=NULL);
  CMacLogData* PrevChild(CMacLogData* inCurChild=NULL);
  
protected:
	virtual void UpdateSelf(CTreeView * view, EntnodeData *entryInfo) = 0;
	virtual CSize GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo) = 0;

  virtual void CollectMergePointPairPass1Self(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair);
  virtual void CollectMergePointPairPass2Self(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair);

  void CollectMergePointPairPass1(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair);
  void CollectMergePointPairPass2(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair);

	inline CMacLogData *GetData(CLogNode *node)
		{ return (CMacLogData *)node->GetUserData(); }

	CSize GetTextExtent(const char *str, int size);

	CRect selfB;
	CRect totB;
	bool sel;
	bool fontSet;
	FontInfo fontInfo;
};

class CMacLogHeaderData : public CMacLogData
{
public:
	CMacLogHeaderData(CLogNode *node) : CMacLogData(node) {}
	virtual ~CMacLogHeaderData() {}

protected:
	virtual void UpdateSelf(CTreeView * view, EntnodeData *entryInfo);
	virtual CSize GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo);
};

class CMacLogRevData : public CMacLogData
{
public:
	CMacLogRevData(CLogNode *node) : CMacLogData(node) {}
	virtual ~CMacLogRevData() {}
	
protected:
	virtual void UpdateSelf(CTreeView * view, EntnodeData *entryInfo);
	virtual CSize GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo);

  virtual void CollectMergePointPairPass1Self(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair); //Override
  virtual void CollectMergePointPairPass2Self(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair); //Override
};

class CMacLogTagData : public CMacLogData
{
public:
	CMacLogTagData(CLogNode *node) : CMacLogData(node) {}
	virtual ~CMacLogTagData() {}
	
protected:
	virtual void UpdateSelf(CTreeView * view, EntnodeData *entryInfo);
	virtual CSize GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo);
};

class CMacLogBranchData : public CMacLogData
{
public:
	CMacLogBranchData(CLogNode *node) : CMacLogData(node) {}
	virtual ~CMacLogBranchData() {}
	
protected:
	virtual void UpdateSelf(CTreeView * view, EntnodeData *entryInfo);
	virtual CSize GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo);
};

CSize CMacLogHeaderData::GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo)
{
	CLogNodeHeader & header = *(CLogNodeHeader *)fNode;
	return GetTextExtent((*header).WorkingFile(),
		(*header).WorkingFile().length());
}

void CMacLogHeaderData::UpdateSelf(CTreeView * view, EntnodeData *entryInfo)
{
	// draw the contour of the node
	CRect shadow;
	Rect  r;
	
	RGBForeColor(&kdred);
	shadow = selfB;
	shadow.OffsetRect(CPoint(2, 2));
	r = ImageToLocalRect(*view, shadow);
	PaintRoundRect(&r, 6, 6);

	r = ImageToLocalRect(*view, selfB);
	RGBForeColor(sel ? &kselc : &kwhi);
	PaintRoundRect(&r, 6, 6);
	RGBForeColor(&kred);
	FrameRoundRect(&r, 6, 6);

	// restore colors
	RGBForeColor(&kbl);

	// draw the content of the node
	CLogNodeHeader & header = *(CLogNodeHeader *)fNode;
	Point where;
	ImageToLocalPoint(*view, selfB.TopLeft() + CPoint(kInflateNodeSpace/2, kInflateNodeSpace/2), where);
	MoveTo(where.h, where.v + fontInfo.ascent);
	LStr255 pstr(UTF8_to_LString((*header).WorkingFile()));
	DrawString(pstr);
}

CSize CMacLogRevData::GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo)
{
	CLogNodeRev & rev = *(CLogNodeRev *)fNode;
	CSize size = GetTextExtent((*rev).RevNum().c_str(),
		strlen((*rev).RevNum().c_str()));
	if(IsDiskNode(entryInfo))
	{
		size.width += 2 + 16 + 2;
		size.height = max((long)16, size.height);
	}
	return size;
}

void CMacLogRevData::UpdateSelf(CTreeView * view, EntnodeData *entryInfo)
{
	// draw the contour of the node
	CRect shadow;
	CRect txtRect = selfB;
	Rect  r;
	
	if(IsDiskNode(entryInfo))
	{
		txtRect.left += 2 + 16 + 2;
	}

	RGBForeColor(&kdgray);
	shadow = selfB;
	shadow.OffsetRect(CPoint(2, 2));
	r = ImageToLocalRect(*view, shadow);
	PaintRect(&r);

	RGBForeColor(sel ? &kselc : &kwhi);
	r = ImageToLocalRect(*view, selfB);
	PaintRect(&r);
	RGBForeColor(&kblue);
	FrameRect(&r);

	// restore colors
	RGBForeColor(&kbl);

	// draw the state icon if it is the rev. of the disk
	if(IsDiskNode(entryInfo))
	{
		CPoint w(selfB.TopLeft());
		w.h += 2;
		w.v += (selfB.Height() - 16) / 2;
		CRect ir(w.h, w.v, w.h + 16, w.v + 16);

	  r = ImageToLocalRect(*view, ir);
		::PlotIconSuite(&r, 0/*outIconAlign*/, sel ? ttSelected : ttNone,
			CBrowserTable::GetImageForEntry(entryInfo));
	}

	// draw the content of the node
	CLogNodeRev & rev = *(CLogNodeRev *)fNode;
	Point where;
	ImageToLocalPoint(*view, txtRect.TopLeft() + CPoint(kInflateNodeSpace/2, kInflateNodeSpace/2), where);
	MoveTo(where.h, where.v + fontInfo.ascent);
	CPStr pstr((*rev).RevNum().c_str());
	DrawString(pstr);
}

void CMacLogRevData::CollectMergePointPairPass1Self(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair)
{
	CRevFile rev = *(*((CLogNodeRev*)Node()));
	
	if( !rev.MergePoint().empty() )
	{
		mergePair.push_back(std::make_pair<CMacLogData*, CMacLogData*>(0L, this));
	}
}

void CMacLogRevData::CollectMergePointPairPass2Self(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair)
{
	CRevFile rev = *(*((CLogNodeRev*)Node()));
	
	for(int i = 0; i < (int)mergePair.size(); i++)
	{
		if( mergePair[i].first == 0L &&  (*(*((CLogNodeRev*)(mergePair[i].second->Node())))).MergePoint() == rev.RevNum() )
		{
			mergePair[i].first = this;
		}
	}
}



CSize CMacLogTagData::GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo)
{
	CLogNodeTag & tag = *(CLogNodeTag *)fNode;
	return GetTextExtent(*tag, (*tag).length());
}

void CMacLogTagData::UpdateSelf(CTreeView * view, EntnodeData *entryInfo)
{
	// draw the contour of the node
	CRect shadow;
	Rect  r;
	
	RGBForeColor(&kdgray);
	shadow = selfB;
	shadow.OffsetRect(CPoint(2, 2));
	r = ImageToLocalRect(*view, shadow);
	PaintRoundRect(&r, 6, 6);

	r = ImageToLocalRect(*view, selfB);
	RGBForeColor(sel ? &kselc : &kwhi);
	PaintRoundRect(&r, 6, 6);
	RGBForeColor(&kbl);
	FrameRoundRect(&r, 6, 6);

	// draw the content of the node
	CLogNodeTag & tag = *(CLogNodeTag *)fNode;
	Point where;
	ImageToLocalPoint(*view, selfB.TopLeft() + CPoint(kInflateNodeSpace/2, kInflateNodeSpace/2), where);
	MoveTo(where.h, where.v + fontInfo.ascent);
	CPStr pstr(*tag);
	DrawString(pstr);
}

CSize CMacLogBranchData::GetBoundsSelf(CTreeView * view, EntnodeData *entryInfo)
{
	CLogNodeBranch & branch = *(CLogNodeBranch *)fNode;
	return GetTextExtent(*branch, (*branch).length());
}

void CMacLogBranchData::UpdateSelf(CTreeView * view, EntnodeData *entryInfo)
{
	// draw the contour of the node
	CRect   shadow;
	Rect    r;
	
	RGBForeColor(&kdblue);
	shadow = selfB;
	shadow.OffsetRect(CPoint(2, 2));
	r = ImageToLocalRect(*view, shadow);
	PaintRoundRect(&r, 6, 6);

  r = ImageToLocalRect(*view, selfB);
	RGBForeColor(sel ? &kselc : &kwhi);
	PaintRoundRect(&r, 6, 6);
	RGBForeColor(&kblue);
	FrameRoundRect(&r, 6, 6);

	// restore colors
	RGBForeColor(&kbl);

	// draw the content of the node
	CLogNodeBranch & branch = *(CLogNodeBranch *)fNode;
	Point where;
	ImageToLocalPoint(*view, selfB.TopLeft() + CPoint(kInflateNodeSpace/2, kInflateNodeSpace/2), where);
	MoveTo(where.h, where.v + fontInfo.ascent);
	CPStr pstr(*branch);
	DrawString(pstr);
}

CMacLogData::CMacLogData(CLogNode *node) : CLogNodeData(node)
{
	selfB.SetRectEmpty();
	totB.SetRectEmpty();
	sel = false;
	fontSet = false;
}

CMacLogData::~CMacLogData()
{
}

const char *CMacLogData::GetStr(void)
{
	switch(GetType())
	{
		case kNodeHeader :
		{
			CLogNodeHeader & header = *(CLogNodeHeader *)fNode;
			return (*header).WorkingFile();
			break;
		}
		case kNodeBranch :
		{
			CLogNodeBranch & branch = *(CLogNodeBranch *)fNode;
			return *branch;
			break;
		}
		case kNodeRev :
		{
			CLogNodeRev & rev = *(CLogNodeRev *)fNode;
			return (*rev).RevNum().c_str();
			break;
		}
		case kNodeTag :
		{
			CLogNodeTag & tag = *(CLogNodeTag *)fNode;
			return *tag;
			break;
		}
	}
	return 0L;
}

CMacLogData * CMacLogData::CreateNewData(CLogNode *node)
{
	if(node->GetUserData() != 0L)
		return (CMacLogData *)node->GetUserData();

	CMacLogData *res = 0L;
	switch(node->GetType())
	{
	case kNodeHeader :
		res = NEW CMacLogHeaderData(node);
		break;
	case kNodeBranch :
		res = NEW CMacLogBranchData(node);
		break;
	case kNodeRev :
		res = NEW CMacLogRevData(node);
		break;
	case kNodeTag :
		res = NEW CMacLogTagData(node);
		break;
	}
	node->SetUserData(res);
	return res;
}

void CMacLogData::ComputeBounds(const CPoint & topLeft, CTreeView * view,
								EntnodeData *entryInfo)
{
	CLogNode *subNode;
	CLogNode *nextNode = fNode->Next();

	// first compute childrens bounds
	std::vector<CLogNode *>::iterator i;
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		subNode = *i;
		CMacLogData::CreateNewData(subNode);
		GetData(subNode)->ComputeBounds(topLeft, view, entryInfo);
	}
	if(nextNode != 0L)
		CMacLogData::CreateNewData(nextNode);

	// compute self place
	selfB.SetRectEmpty();
	CSize size = GetBoundsSelf(view, entryInfo);
	size.width += kInflateNodeSpace;
	size.height += kInflateNodeSpace;
	selfB.SetRect(0, 0, size.width, size.height);

	// offset to the place assigned to this node
	selfB.OffsetRect(topLeft - selfB.TopLeft());

	// calculate the total height of the childrens
	int vSize = 0;
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		subNode = *i;
		CRect & b = GetData(subNode)->Bounds();
		vSize += b.Height();
	}
	if(!fNode->Childs().empty())
		vSize += (fNode->Childs().size() - 1) * kVChildSpace;

	// offset the children relative to self
	CPoint startChilds(topLeft.h + selfB.Width() + kHChildSpace,
		/*selfB.CenterPoint().v - vSize / 2*/topLeft.v);
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		subNode = *i;
		CRect curPos = GetData(subNode)->Bounds();
		GetData(subNode)->Offset(startChilds - curPos.TopLeft());
		startChilds.v += curPos.Height() + kVChildSpace;
	}

	// calculate the total bounds of the childrens
	CRect bc;
	bc.SetRectEmpty();
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		subNode = *i;
		CRect & b = GetData(subNode)->Bounds();
		bc.UnionRect(bc, b);
	}

	// now we got the complete size
	totB.UnionRect(selfB, bc);

	if(nextNode != 0L)
	{
		CPoint nextTopLeft;
		nextTopLeft.h = totB.TopLeft().h;
		nextTopLeft.v = totB.BottomRight().v + kVChildSpace;
		GetData(nextNode)->ComputeBounds(nextTopLeft, view, entryInfo);

		totB.UnionRect(totB, GetData(nextNode)->Bounds());
	}
}

void CMacLogData::SetSelected(CTreeView *view, bool state)
{
	sel = state;

	Rect r;
	r = ImageToPortRect(*view, selfB);
	view->InvalPortRect(&r);
}

bool CMacLogData::IsDiskNode(EntnodeData *entryInfo)
{
	if(entryInfo == 0L || GetType() != kNodeRev)
		return false;
	const char *vn = (*entryInfo)[EntnodeFile::kVN];
	if(vn == 0L)
		return false;

	return strcmp(vn, (**(CLogNodeRev *)fNode).RevNum().c_str()) == 0;
}

void CMacLogData::Offset(const CPoint & o)
{
	selfB.OffsetRect(o);
	totB.OffsetRect(o);
	std::vector<CLogNode *>::iterator i;
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		CLogNode *subNode = *i;
		GetData(subNode)->Offset(o);
	}
	if(fNode->Next() != 0L)
		GetData(fNode->Next())->Offset(o);
}

CSize CMacLogData::GetTextExtent(const char *str, int size)
{
	if(!fontSet)
	{
		fontSet = true;
		GetFontInfo(&fontInfo);
	}
	
	int width = TextWidth(str, 0, strlen(str));
	int height = fontInfo.ascent + fontInfo.descent;
	return CSize(width, height);
}

void CMacLogData::Update(CTreeView * view, const CRect& inUpdateRect, EntnodeData *entryInfo)
{
	CLogNode *subNode;
	CLogNode *nextNode = fNode->Next();

	if ( inUpdateRect.Intersects(selfB) ) UpdateSelf(view, entryInfo);

	int topY = selfB.top + 2;
	int botY = selfB.bottom - 2;
	int lefX = selfB.right + 4;
	int count = 1;
	int tot = fNode->Childs().size() + 1;

	// draw the children
	std::vector<CLogNode *>::iterator i;
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i, ++count)
	{
		subNode = *i;
		
		// draw the link
		CRect & cb = GetData(subNode)->SelfBounds();
		if ( cb.bottom >= inUpdateRect.top ) {
  		if(subNode->GetType() == kNodeRev || subNode->GetType() == kNodeBranch)
  		{
  			RGBForeColor(&kblue);
  			PenSize(2, 2);
  		}
  		else
  		{
  			RGBForeColor(&kbl);
  		  PenSize(1, 1);
  		}
  		float y = (float)topY + (float)(botY - topY) *
  			(float)count / (float)tot;
  		int rigX = cb.left - 4;
  		float x = (float)rigX - (float)(rigX - lefX) *
  			(float)count / (float)tot;
  		//int halfX = (cb.TopLeft().h - selfB.BottomRight().h) / 2;
  		Point   pt;
  		ImageToLocalPoint(*view, CPoint(selfB.right, (int)y), pt);
  		MoveTo(pt.h, pt.v);
  		ImageToLocalPoint(*view, CPoint((int)x, (int)y), pt);
  		LineTo(pt.h, pt.v);
  		ImageToLocalPoint(*view, CPoint(x, cb.CenterPoint().v), pt);
  		LineTo(pt.h, pt.v);
  		ImageToLocalPoint(*view, CPoint(cb.left, cb.CenterPoint().v), pt);
  		LineTo(pt.h, pt.v);
  	}
  	if ( GetData(subNode)->Bounds().Intersects(inUpdateRect) ) {
  		// draw the sub-node
  		PenSize(1, 1);
  		RGBForeColor(&kbl);
  		GetData(subNode)->Update(view, inUpdateRect, entryInfo);
    }
	}

	// draw the next node
	if(nextNode != 0L)
	{
		// draw the link
		CRect & nb = GetData(nextNode)->SelfBounds();
		if(nextNode->GetType() == kNodeRev || nextNode->GetType() == kNodeBranch)
		{
			RGBForeColor(&kblue);
			PenSize(2, 2);
		}
		else
		{
			RGBForeColor(&kbl);
  		PenSize(1, 1);
		}
		int centerX = selfB.Width() < nb.Width() ?
			selfB.CenterPoint().h : nb.CenterPoint().h;
		Point   pt;
		ImageToLocalPoint(*view, CPoint(centerX, selfB.bottom), pt);
		MoveTo(pt.h, pt.v);
		ImageToLocalPoint(*view, CPoint(centerX, nb.top), pt);
		LineTo(pt.h, pt.v);
		
		// draw the next node
		if ( nb.top < inUpdateRect.bottom ) {
  		PenSize(1, 1);
  		RGBForeColor(&kbl);
  		GetData(nextNode)->Update(view, inUpdateRect, entryInfo);
    }
	}
	
	RGBForeColor(&kbl);
	PenSize(1, 1);
}

/*!
	Draw the merges relationship
	\param view         view to draw into
	\param updateRect   rectangle to update
*/
void CMacLogData::UpdateMergePoints(CTreeView * view, const CRect& updateRect)
{
	std::vector<std::pair<CMacLogData*, CMacLogData*> > mergePair;
	
	CollectMergePointPairPass1(mergePair);
	CollectMergePointPairPass2(mergePair);

  RGBForeColor(&kred);
  PenSize(1, 1);
  
	try {
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
					pts[1] = CPoint(pts[0].h - 8, pts[0].v);
					pts[2] = CPoint(rect_target.right + 8, rect_target.top + rect_target.Height()/2);
					pts[3] = CPoint(pts[2].h - 8, pts[2].v);
					pts[4] = CPoint(pts[3].h + 4, pts[3].v-2);
					pts[5] = CPoint(pts[4].h, pts[4].v+4);
					pts[6] = pts[3];
					
					Polyline(*view, updateRect, pts, 7);
				}
				else if( rect_source.left == rect_target.left )
				{
					CPoint pts[7];
					
					pts[0] = CPoint(rect_source.right, rect_source.top + rect_source.Height()/2);
					pts[1] = CPoint(pts[0].h + 8, pts[0].v);
					pts[2] = CPoint(rect_target.right + 8, rect_target.top + rect_target.Height()/2);
					pts[3] = CPoint(pts[2].h - 8, pts[2].v);
					pts[4] = CPoint(pts[3].h + 4, pts[3].v-2);
					pts[5] = CPoint(pts[4].h, pts[4].v+4);
					pts[6] = pts[3];
					
					Polyline(*view, updateRect, pts, 7);
				}
				else
				{
					CPoint pts[7];
					
					pts[0] = CPoint(rect_source.right, rect_source.top + rect_source.Height()/2);
					pts[1] = CPoint(pts[0].h + 8, pts[0].v);
					pts[2] = CPoint(rect_target.left - 8, rect_target.top + rect_target.Height()/2);
					pts[3] = CPoint(pts[2].h + 8, pts[2].v);
					pts[4] = CPoint(pts[3].h - 4, pts[3].v-2);
					pts[5] = CPoint(pts[4].h, pts[4].v+4);
					pts[6] = pts[3];
					
					Polyline(*view, updateRect, pts, 7);
				}
			}
		}
	}
	catch(...)
	{
	}

	RGBForeColor(&kbl);
	PenSize(1, 1);
}

void CMacLogData::CollectMergePointPairPass1(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair)
{
	CollectMergePointPairPass1Self(mergePair);
	
	std::vector<CLogNode*>::iterator it = Node()->Childs().begin();
	
	while( it != Node()->Childs().end() )
	{
		((CMacLogData*)(*it)->GetUserData())->CollectMergePointPairPass1(mergePair);

		it++;
	}
	
	if( Node()->Next() != NULL )
	{
		((CMacLogData*)Node()->Next()->GetUserData())->CollectMergePointPairPass1(mergePair);
	}
}

void CMacLogData::CollectMergePointPairPass2(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair)
{
	CollectMergePointPairPass2Self(mergePair);
	
	std::vector<CLogNode*>::iterator it = Node()->Childs().begin();
	
	while( it != Node()->Childs().end() )
	{
		((CMacLogData*)(*it)->GetUserData())->CollectMergePointPairPass2(mergePair);
		
		it++;
	}
	
	if( Node()->Next() != NULL )
	{
		((CMacLogData*)Node()->Next()->GetUserData())->CollectMergePointPairPass2(mergePair);
	}
}

void CMacLogData::UnselectAll(CTreeView *view)
{
	if(sel)
	{
		SetSelected(view, false);
	}

	std::vector<CLogNode *>::iterator i;
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		CLogNode *subNode = *i;
		GetData(subNode)->UnselectAll(view);
	}
	if(fNode->Next() != 0L)
		GetData(fNode->Next())->UnselectAll(view);
}

CMacLogData *CMacLogData::HitTest(const CPoint & point)
{
	if(!totB.PtInRect(point))
		return 0L;
	if(selfB.PtInRect(point))
		return this;

	CMacLogData *result;
	std::vector<CLogNode *>::iterator i;
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		CLogNode *subNode = *i;
		result = GetData(subNode)->HitTest(point);
		if(result != 0L)
			return result;
	}
	if(fNode->Next() != 0L)
	{
		result = GetData(fNode->Next())->HitTest(point);
		if(result != 0L)
			return result;
	}
	return 0L;
}

CMacLogData* CMacLogData::Next() 
{ 
  CLogNode* next(fNode->Next());
  return next ? GetData(next) : NULL;
}

CMacLogData* CMacLogData::NextChild(CMacLogData *inCurChild)
{
  if ( fNode->Childs().empty() ) return NULL;
	std::vector<CLogNode *>::iterator i;
	if ( inCurChild )
	{
	  for ( i=fNode->Childs().begin(); i != fNode->Childs().end(); ++i )
	  {
		  CLogNode *subNode = *i;
		  if ( GetData(subNode) == inCurChild ) {
		    ++i;
		    break;
		  }
	  }
	}
	else i = fNode->Childs().begin();
  return i == fNode->Childs().end() ? NULL : GetData(*i);
}

CMacLogData* CMacLogData::PrevChild(CMacLogData *inCurChild)
{
  if ( fNode->Childs().empty() ) return NULL;
	std::vector<CLogNode *>::iterator i;
	if ( inCurChild )
	{
    CMacLogData*    prevChild = NULL;
	  for ( i=fNode->Childs().begin(); i != fNode->Childs().end(); ++i )
	  {
		  CLogNode *subNode = *i;
		  if ( GetData(subNode) == inCurChild )
		    return prevChild;
		  prevChild = GetData(*i);
	  }
	  return NULL;
	}
	else return GetData(fNode->Childs().back());
}

CMacLogData* CMacLogData::GetFirstSelected(CMacLogData **inOutParentNode, CMacLogData **inOutPreviousNode)
{
	CMacLogData *result;
  
  if ( this->Selected() ) return this;
	if(fNode->Next() != 0L)
	{
    CMacLogData *oldPrevNode;
	  if ( inOutPreviousNode ) { 
	    oldPrevNode = *inOutPreviousNode;
	    *inOutPreviousNode = this;
	  }
		result = GetData(fNode->Next())->GetFirstSelected(inOutParentNode, inOutPreviousNode);
		if(result != 0L)
			return result;
    if ( inOutPreviousNode ) *inOutPreviousNode = oldPrevNode;
	}
	if ( !fNode->Childs().empty() )
	{
    CMacLogData *oldParentNode;
    CMacLogData *oldPrevNode;
  	if ( inOutParentNode ) {
  	  oldParentNode = *inOutParentNode; 
  	  *inOutParentNode = this;
  	}
	  if ( inOutPreviousNode ) { 
	    oldPrevNode = *inOutPreviousNode;
	    *inOutPreviousNode = NULL;
	  }
  	std::vector<CLogNode *>::iterator i;
  	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
  	{
  		CLogNode *subNode = *i;
  		result = GetData(subNode)->GetFirstSelected(inOutParentNode, inOutPreviousNode);
  		if(result != 0L)
  			return result;
  	}
    if ( inOutParentNode ) *inOutParentNode = oldParentNode;
  }
  return NULL;
}

CMacLogData* CMacLogData::GetLastSelected(CMacLogData **inOutParentNode, CMacLogData **inOutPreviousNode)
{
	CMacLogData *result;
	if(fNode->Next() != 0L)
	{
		result = GetData(fNode->Next())->GetLastSelected(inOutParentNode, inOutPreviousNode);
		if(result != 0L)
	  {
	    if ( inOutPreviousNode && *inOutPreviousNode == NULL ) *inOutPreviousNode = this;
			return result;
		}
	}
	std::vector<CLogNode *>::iterator i;
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		CLogNode *subNode = *i;
		result = GetData(subNode)->GetLastSelected(inOutParentNode, inOutPreviousNode);
		if(result != 0L)
		{
		  if ( inOutParentNode && *inOutParentNode == NULL ) *inOutParentNode = this;
			return result;
		}
	}
  if ( this->Selected() ) return this;
  return NULL;
}

CMacLogData* CMacLogData::GetDiskNode(EntnodeData *entryInfo)
{
	CMacLogData *result;
  if ( this->IsDiskNode(entryInfo) ) return this;
	if(fNode->Next() != 0L)
	{
		result = GetData(fNode->Next())->GetDiskNode(entryInfo);
		if(result != 0L)
			return result;
	}
	std::vector<CLogNode *>::iterator i;
	for(i = fNode->Childs().begin(); i != fNode->Childs().end(); ++i)
	{
		CLogNode *subNode = *i;
		result = GetData(subNode)->GetDiskNode(entryInfo);
		if(result != 0L)
			return result;
	}
  return NULL;
}

/*!
	Get a vector of merge points pair (source-->Target).
	This is the first pass, which collects only the target
	\param mergePair Vector of merge pair
*/
void CMacLogData::CollectMergePointPairPass1Self(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair)
{
}

/*!
	Get a vector of merge points pair (source-->Target).
	This is the second pass, which makes the link between source and target
	\param mergePair Vector of merge pair
*/
void CMacLogData::CollectMergePointPairPass2Self(std::vector<std::pair<CMacLogData*, CMacLogData*> >& mergePair)
{
}


// traversal class
typedef enum
{
	kTContinue,
	kTGetDown,
	kTStop
} kTGraph;

class TGraph
{
public:

	kTGraph TraverseGraph(CLogNode * node);

	virtual kTGraph TraverseNode(CLogNode *node) = 0;
protected:
};

kTGraph TGraph::TraverseGraph(CLogNode * node)
{
	kTGraph res;

	res = TraverseNode(node);
	if(res == kTStop)
		return kTStop;

	if(res == kTGetDown)
	{
		std::vector<CLogNode *>::iterator i;
		for(i = node->Childs().begin(); i != node->Childs().end(); ++i)
		{
			res = TraverseGraph(*i);
			if(res == kTStop)
				return kTStop;
		}
	}

	if(node->Next() != 0L)
	{
		res = TraverseGraph(node->Next());
		if(res == kTStop)
			return kTStop;
	}

	return kTContinue;
};

class TGraphCmdStatus : public TGraph
{
public:
	TGraphCmdStatus() :
		fSel1(0L), fSel2(0L), fCountSel(0) {}

	virtual kTGraph TraverseNode(CLogNode *node)
	{
		CMacLogData *data = (CMacLogData *)node->GetUserData();

		if(data->Selected())
		{
			if(fSel1 == 0L)
				fSel1 = data;
			else if(fSel2 == 0L)
				fSel2 = data;
			fCountSel++;
		}

		return kTGetDown;
	}

	CMacLogData *fSel1;
	CMacLogData *fSel2;
	int fCountSel;
};

CTreeView::CTreeView(LStream *inStream) : fNode(0L), fData(0L), LView( inStream )
{
}

CTreeView::~CTreeView()
{
	CMacCvsApp::gApp->RemoveWindowFromListByView(this);
	if(fNode != 0L)
		delete fNode;
	if(fData != 0L)
		fData->UnRef();
}

void CTreeView::SetNode(CLogNode *node, const char *dir)
{
	if(fNode != 0L)
		delete fNode;
	fNode = 0L;
	if(fData != 0L)
		fData->UnRef();
	fData = 0L;
	fDir = dir;
	
	// get the CVS/Entries infos for this file
	CSortList<ENTNODE> entries(200, ENTNODE::Compare);
	if(!Entries_Open (entries, fDir))
	{
		cvs_err("Warning : Error while accessing the CVS folder in '%s'\n", (const char *)fDir);
	}
	else
	{
		if(node->GetType() != kNodeHeader)
		{
			cvs_err("Warning : Top node is not a RCS file description !\n");
		}
		else
		{
			const char *name = (**(CLogNodeHeader *)node).WorkingFile();
			struct stat sb;
			if (stat(name, &sb) == -1)
			{
				cvs_err("Warning : Unknown file '%s'\n", name);
			}
			else
			{
				fData = Entries_SetVisited(fDir, entries, name, sb, false);
				fData->Ref();
				fName = name;
			}
		}
	}

	fNode = node;
	
	CalcImageSize();
	
	CMacLogData*  diskNode = ((CMacLogData*)fNode->GetUserData())->GetDiskNode(fData);
	if ( diskNode ) 
	{
	  this->SelectNode(diskNode, true);
	  this->ScrollNodeIntoView(diskNode);
	}
}

void CTreeView::CalcImageSize(void)
{
	if(fNode == 0L)
		return;

	FocusDraw();
	TextFont(kFontIDGeneva);
	TextSize(9);

	// set all the bounds locations
	CMacLogData *data = CMacLogData::CreateNewData(fNode);
	fNode->SetUserData(data);
	CPoint start(5, 5);
	data->ComputeBounds(start, this, fData);

	// reoffset from (0, 0)
	CRect bounds = data->Bounds();
	data->Offset(start - bounds.TopLeft());
	bounds = data->Bounds();

	// set the scroll size
	ResizeImageTo(bounds.Width(), bounds.Height(), false);
	
	// refresh
	Rect refreshRect;
	CalcPortFrameRect(refreshRect);
	InvalPortRect(&refreshRect);
}

void CTreeView::DrawSelf(void)
{
	if(fNode == 0L || fNode->GetUserData() == 0L)
		return;

	CRect	updateRect;
	{
	  Rect      r;
		StRegion	localUpdateRgn( GetLocalUpdateRgn(), false );
		localUpdateRgn.GetBounds(r);
	  updateRect = LocalToImageRect(*this, r);
	}
	TextFont(kFontIDGeneva);
	TextSize(9);
  
	CMacLogData *data = (CMacLogData *)fNode->GetUserData();
	data->Update(this, updateRect, fData);
	data->UpdateMergePoints(this, updateRect);
}

void CTreeView::ClickSelf(const SMouseDownEvent & inMouseDown)
{
	if(fNode == 0L || fNode->GetUserData() == 0L)
		return;

	if (!IsTarget())
		SwitchTarget(this);
	
	CMacLogData *data = (CMacLogData *)fNode->GetUserData();		
	CPoint mouseLoc;
	PortToImagePoint(*this, inMouseDown.wherePort, mouseLoc);

	if((inMouseDown.macEvent.modifiers & shiftKey) == 0)
		data->UnselectAll(this);
	CMacLogData *hit = data->HitTest(mouseLoc);

	if(hit != 0L)
	{
		if((inMouseDown.macEvent.modifiers & shiftKey) == 0)
		{
			if(!hit->Selected()) this->SelectNode(hit, true);
		}
		else
		{
			this->SelectNode(hit, !hit->Selected());
		}
	}
	else
	{
		// lasso ?
	}
}

void CTreeView::SelectNode(CMacLogData* inNode, bool inSelect)
{
  if ( inSelect )
  {
		CColorConsole out;
		CvsLogOutput(out, inNode->Node());
		FocusDraw();
		inNode->SetSelected(this, true);
  }
  else inNode->SetSelected(this, false);
}

void CTreeView::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	if(fNode != 0L)
	{
		switch(inCommand)
		{
		case cmd_SelReload :
		case cmd_GraphRemoveSticky:
			outEnabled = !CMacCvsApp::gApp->IsCvsRunning();
			break;
		
		case cmd_SelTag :
		{
			if(CMacCvsApp::gApp->IsCvsRunning())
			{
				outEnabled = false;
				return;
			}
			
			TGraphCmdStatus trav;
			trav.TraverseGraph(fNode);
			outEnabled = trav.fCountSel == 1;
			break;
		}
		
		case cmd_SelDiff :
		{
			if(CMacCvsApp::gApp->IsCvsRunning())
			{
				outEnabled = false;
				return;
			}
			
			TGraphCmdStatus trav;
			trav.TraverseGraph(fNode);
			outEnabled = (trav.fCountSel == 2 && trav.fSel1->GetType() != kNodeHeader &&
				trav.fSel2->GetType() != kNodeHeader) ||
				(trav.fCountSel == 1 && trav.fSel1->GetType() != kNodeHeader);
			break;
		}
		
		case cmd_SelAnnotate :
		{
			if(CMacCvsApp::gApp->IsCvsRunning())
			{
				outEnabled = false;
				return;
			}
			
			TGraphCmdStatus trav;
			trav.TraverseGraph(fNode);
			outEnabled = (trav.fCountSel == 1 && trav.fSel1->GetType() != kNodeHeader);
			break;
		}
		
		default:
			// Call inherited.
			LCommander::FindCommandStatus( inCommand,
				outEnabled, outUsesMark, outMark, outName );
			break;
		}
	}
	else
	{
		LCommander::FindCommandStatus( inCommand,
			outEnabled, outUsesMark, outMark, outName );
	}
}

Boolean CTreeView::ObeyCommand(CommandT inCommand, void *ioParam)
{
	if(fNode != 0L)
	{
		switch(inCommand)
		{
		case cmd_SelReload :
		{
			CvsArgs args;
			args.add("log");
			args.add(fName);
			args.print(fDir);
			CvsLogParse(fDir, args, true, this);
			break;
		}
		case cmd_SelTag :
		{
			TGraphCmdStatus trav;
			trav.TraverseGraph(fNode);
			CvsArgs args;
			args.add("update");
			CLogNode *node = trav.fSel1->Node();
			switch(node->GetType())
			{
				case kNodeHeader :
				{
					args.add("-A");
					break;
				}
				case kNodeRev :
				case kNodeBranch :
				case kNodeTag :
				{
					args.add("-r");
					args.add(trav.fSel1->GetStr());
					break;
				}
			}
			args.add(fName);
			args.print(fDir);
			launchCVS(fDir, args.Argc(), args.Argv());
			
			ProcessCommand(cmd_SelReload);
			break;
		}
		case cmd_GraphRemoveSticky:
		{
			CvsArgs args;
			args.add("update");
			args.add("-A");
			args.add(fName);
			args.print(fDir);
			launchCVS(fDir, args.Argc(), args.Argv());
			
			ProcessCommand(cmd_SelReload);
			break;
		}
		case cmd_SelDiff :
		{
			TGraphCmdStatus trav;
			trav.TraverseGraph(fNode);
    	MultiFiles mf;
    	
    	mf.newdir(fDir);
    	mf.newfile(fName, 0L, trav.fSel1->GetStr());

    	if( trav.fCountSel == 2 )
    	{
    		mf.newfile(fName, 0L, trav.fSel2->GetStr());
    	}

    	KoGraphDiffHandler handler;
    	handler.OnFiles(&mf);

			break;
		}
		case cmd_SelAnnotate :
		{
			TGraphCmdStatus trav;
			trav.TraverseGraph(fNode);
    	MultiFiles mf;
    	
    	mf.newdir(fDir);
    	mf.newfile(fName, 0L, trav.fSel1->GetStr());

    	KoGraphAnnotateHandler handler;
    	handler.OnFiles(&mf);

			break;
		}
		default:
			// Call inherited.
			return LCommander::ObeyCommand(inCommand, ioParam);
		}
	}
	else
	{
		return LCommander::ObeyCommand(inCommand, ioParam);
	}
	
	return true;
}

void CTreeView::ActivateSelf()
{
	if (!IsTarget())
		SwitchTarget(this);
}

void CTreeView::DeactivateSelf()
{
	if (!IsTarget())
		SwitchTarget(this);
}

Boolean
CTreeView::HandleKeyPress(
	const EventRecord&	inKeyEvent)
{
  enum {
		ePageUpKey 		= 0x74, 
		ePageDownKey 	= 0x79,
		eHomeKey 		= 0x73,
		eEndKey 		= 0x77,
		eUpArrowKey 	= 0x7e,
		eDownArrowKey 	= 0x7d,
		eLeftArrowKey 	= 0x7b,
		eRightArrowKey 	= 0x7c
  };

  UInt8  keyCode((inKeyEvent.message & keyCodeMask)>>8);
  switch ( keyCode )
  {
    case ePageUpKey:
    case ePageDownKey:
    {
      SDimension16    mySize;
      this->GetFrameSize(mySize);
      this->ScrollPinnedImageBy(0, mySize.height * (keyCode == ePageUpKey ? -1 : 1), true);
      return true;
    }
    case eHomeKey:
    case eEndKey:
      this->ScrollPinnedImageBy(-LONG_MAX, keyCode == eHomeKey ? -LONG_MAX : LONG_MAX, true);
      return true;
    case eDownArrowKey:
    {
	    CMacLogData *data = (CMacLogData *)fNode->GetUserData();	
	    CMacLogData*  parentNode = NULL;	
      CMacLogData*  lastSelected = data->GetLastSelected(&parentNode);
      CMacLogData*  toSelect = NULL;
      
      if ( lastSelected )
      {
        toSelect = lastSelected->Next();
        if ( toSelect == NULL && parentNode ) toSelect = parentNode->NextChild(lastSelected);
      }
      else toSelect = data;
      
      if ( toSelect )
      {
        data->UnselectAll(this);
        this->SelectNode(toSelect, true);
        this->ScrollNodeIntoView(toSelect);
      }
      return true;
    }
    break;
    case eUpArrowKey:
    {
	    CMacLogData *data = (CMacLogData *)fNode->GetUserData();	
	    CMacLogData *prevNode = NULL;	
	    CMacLogData *parentNode = NULL;	
      CMacLogData*  firstSelected = data->GetFirstSelected(&parentNode, &prevNode);
      CMacLogData*  toSelect = NULL;
      
      if ( firstSelected )
      {
        toSelect = prevNode;
        if ( toSelect == NULL && parentNode ) toSelect = parentNode->PrevChild(firstSelected);
      }
      else toSelect = data;
      
      if ( toSelect )
      {
        data->UnselectAll(this);
        this->SelectNode(toSelect, true);
        this->ScrollNodeIntoView(toSelect);
      }
      return true;
    }
    break;
    case eLeftArrowKey:
    {
	    CMacLogData *data = (CMacLogData *)fNode->GetUserData();		
	    CMacLogData*  parentNode = NULL;
	    CMacLogData*  prevNode = NULL;
      CMacLogData*  firstSelected = data->GetFirstSelected(&parentNode, &prevNode);
      CMacLogData*  toSelect = NULL;
      
      if ( firstSelected ) 
      {
        if ( prevNode == NULL ) toSelect = parentNode;
      }
      else toSelect = data;
      
      if ( toSelect )
      {
        data->UnselectAll(this);
        this->SelectNode(toSelect, true);
        this->ScrollNodeIntoView(toSelect);
      }
      return true;
    }
    break;
    case eRightArrowKey:
    {
	    CMacLogData *data = (CMacLogData *)fNode->GetUserData();		
      CMacLogData*  firstSelected = data->GetFirstSelected();
      CMacLogData*  toSelect = NULL;
      
      if ( firstSelected ) toSelect = firstSelected->NextChild(NULL);
      else toSelect = data;
      
      if ( toSelect )
      {
        data->UnselectAll(this);
        this->SelectNode(toSelect, true);
        this->ScrollNodeIntoView(toSelect);
      }
      return true;
    }
    break;
    
    default:
      return LCommander::HandleKeyPress(inKeyEvent);
  }
}

void CTreeView::ScrollNodeIntoView(CMacLogData* inNodeToDisplay)
{
	CRect	nodeBounds;
	
	nodeBounds = inNodeToDisplay->SelfBounds();

	if (ImagePointIsInFrame(nodeBounds.left, nodeBounds.top) &&
		ImagePointIsInFrame(nodeBounds.right, nodeBounds.bottom)) {

		return;						// Entire node is already within Frame
	}

	Rect	frame;					// Get Frame in Image coords
	CalcLocalFrameRect(frame);
	SPoint32	frameTopLeft, frameBotRight;
	LocalToImagePoint(topLeft(frame), frameTopLeft);
	LocalToImagePoint(botRight(frame), frameBotRight);

	SInt32	horizScroll = 0;		// Find horizontal scroll amount
	if (nodeBounds.right > frameBotRight.h) {
										// node is to the right of frame
		horizScroll = nodeBounds.right - frameBotRight.h;
	}

	if ((nodeBounds.left - horizScroll) < frameTopLeft.h) {
										// Node is to the left of frame
										//   or Node is wider than frame
		horizScroll = nodeBounds.left - frameTopLeft.h;
	}

	SInt32	vertScroll = 0;			// Find vertical scroll amount
	if (nodeBounds.bottom > frameBotRight.v) {
										// Node is below bottom of frame
		vertScroll = nodeBounds.bottom - frameBotRight.v;
	}

	if ((nodeBounds.top - vertScroll) < frameTopLeft.v) {
										// Node is above top of frame
										//   or Cell is taller than frame
		vertScroll = nodeBounds.top - frameTopLeft.v;
	}

	ScrollPinnedImageBy(horizScroll, vertScroll, Refresh_Yes);
}
 