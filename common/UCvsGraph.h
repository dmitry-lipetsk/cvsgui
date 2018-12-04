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

#ifndef UCVSGRAPH_H
#define UCVSGRAPH_H

#include "uwidget.h"
#include "CvsLog.h"
#include "udraw.h"

class CLogNode;
class EntnodeData;

class CScrollView
{
public:
	UPoint GetDeviceScrollPosition() { return UPoint(0, 0); }
	void InvalidateRect(const URECT *r) {}
};

class CWinLogData : public CLogNodeData
{
public:
	CWinLogData(CLogNode *node);
	virtual ~CWinLogData();

	inline kLogNode GetType(void) const { return m_node->GetType(); }
	const char *GetStr(void);

	void ComputeBounds(const UPoint & topLeft, UDC & hdc,
		EntnodeData *entryInfo = 0L);
	void Offset(UPoint o);
	void Update(UDC & hdc, EntnodeData *entryInfo = 0L);
	
	inline URect & Bounds(void) { return totB; }
	inline URect & SelfBounds(void) { return selfB; }

	inline bool Selected(void) const { return sel; }
	void SetSelected(CScrollView *view, bool state);
	inline void SetSelected(bool state) { sel = state; }
	void UnselectAll(CScrollView *view);
	void UnselectAll(void);
	CWinLogData *HitTest(UPoint point);

	static CWinLogData *CreateNewData(CLogNode *node);

	bool IsDiskNode(EntnodeData *entryInfo);

protected:
	virtual void UpdateSelf(UDC & hdc, EntnodeData *entryInfo) = 0;
	virtual USize GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo) = 0;

	inline CWinLogData *GetData(CLogNode *node)
		{ return (CWinLogData *)node->GetUserData(); }

	URect selfB;
	URect totB;
	bool sel;
};

class CWinLogHeaderData : public CWinLogData
{
public:
	CWinLogHeaderData(CLogNode *node) : CWinLogData(node) {}
	virtual ~CWinLogHeaderData() {}

protected:
	virtual void UpdateSelf(UDC & hdc, EntnodeData *entryInfo);
	virtual USize GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo);
};

class CWinLogRevData : public CWinLogData
{
public:
	CWinLogRevData(CLogNode *node) : CWinLogData(node) {}
	virtual ~CWinLogRevData() {}
	
protected:
	virtual void UpdateSelf(UDC & hdc, EntnodeData *entryInfo);
	virtual USize GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo);
};

class CWinLogTagData : public CWinLogData
{
public:
	CWinLogTagData(CLogNode *node) : CWinLogData(node) {}
	virtual ~CWinLogTagData() {}
	
protected:
	virtual void UpdateSelf(UDC & hdc, EntnodeData *entryInfo);
	virtual USize GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo);
};

class CWinLogBranchData : public CWinLogData
{
public:
	CWinLogBranchData(CLogNode *node) : CWinLogData(node) {}
	virtual ~CWinLogBranchData() {}
	
protected:
	virtual void UpdateSelf(UDC & hdc, EntnodeData *entryInfo);
	virtual USize GetBoundsSelf(UDC & hdc, EntnodeData *entryInfo);
};

class UCvsGraph : public UWidget 
{
	UDECLARE_DYNAMIC(UCvsGraph)
public:
	UCvsGraph(CLogNode *node, const char *dir);
	virtual ~UCvsGraph();

	enum
	{
		kDrawing = EV_COMMAND_START // 0		
	};
protected:
	CLogNode *m_node;
	std::string  m_dir;
	std::string  m_name;
	EntnodeData *m_data;

	void CalcImageSize(void);

	ev_msg void OnDestroy();
	ev_msg void OnCreate();
	ev_msg void OnDrawing(void *info);
	ev_msg void OnBtnClick(void *info);
	
	UDECLARE_MESSAGE_MAP()
};

#endif
