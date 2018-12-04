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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 2000
 */

#ifndef UDRAW_H
#define UDRAW_H

#include "uconfig.h"
#include "umain.h"

#if qGTK
#	include <gtk/gtk.h>
#endif

#ifdef __cplusplus

extern UEXPORT bool PtInRect(const URECT *r, UPOINT pt);
extern UEXPORT void SetRect(URECT *r, int x1, int y1, int x2, int y2);
extern UEXPORT void SetRectEmpty(URECT *r);
extern UEXPORT bool EqualRect(const URECT *r1, const URECT *r2);
extern UEXPORT void InflateRect(URECT *r, int x, int y);
extern UEXPORT void OffsetRect(URECT *r, int x, int y);
extern UEXPORT bool UnionRect(URECT *dest, const URECT *src1, const URECT *src2);
extern UEXPORT bool IntersectRect(URECT *dest, const URECT *src1, const URECT *src2);

inline void CopyRect(URECT *dst, const URECT *src) { *dst = *src; }

class UPoint;
class URect;

class USize : public USIZE
{
public:

/* Constructors */
	/* construct an uninitialized size */
	USize();
	/* create from two integers */
	USize(int initCX, int initCY);
	/* create from another size */
	USize(USIZE initSize);
	/* create from a point */
	USize(UPOINT initPt);

/* Operations */
	bool operator==(USIZE size) const;
	bool operator!=(USIZE size) const;
	void operator+=(USIZE size);
	void operator-=(USIZE size);

/* Operators returning USize values */
	USize operator+(USIZE size) const;
	USize operator-(USIZE size) const;
	USize operator-() const;

/* Operators returning UPoint values */
	UPoint operator+(UPOINT point) const;
	UPoint operator-(UPOINT point) const;

/* Operators returning URect values */
	URect operator+(const URECT* lpRect) const;
	URect operator-(const URECT* lpRect) const;
};

class UPoint : public UPOINT
{
public:
/* Constructors */

	/* create an uninitialized point */
	UPoint();
	/* create from two integers */
	UPoint(int initX, int initY);
	/* create from another point */
	UPoint(UPOINT initPt);
	/* create from a size */
	UPoint(USIZE initSize);

/* Operations */

/* translate the point */
	void Offset(int xOffset, int yOffset);
	void Offset(UPOINT point);
	void Offset(USIZE size);

	bool operator==(UPOINT point) const;
	bool operator!=(UPOINT point) const;
	void operator+=(USIZE size);
	void operator-=(USIZE size);
	void operator+=(UPOINT point);
	void operator-=(UPOINT point);

/* Operators returning UPoint values */
	UPoint operator+(USIZE size) const;
	UPoint operator-(USIZE size) const;
	UPoint operator-() const;
	UPoint operator+(UPOINT point) const;

/* Operators returning USize values */
	USize operator-(UPOINT point) const;

/* Operators returning URect values */
	URect operator+(const URECT* lpRect) const;
	URect operator-(const URECT* lpRect) const;
};

class URect : public URECT
{
public:

/* Constructors */

	/* uninitialized rectangle */
	URect();
	/* from left, top, right, and bottom */
	URect(int l, int t, int r, int b);
	/* copy constructor */
	URect(const URECT& srURect);
	/* from a pointer to another rect */
	URect(const URECT* lpSrURect);
	/* from a point and size */
	URect(UPOINT point, USIZE size);
	/* from two points */
	URect(UPOINT topLeft, UPOINT bottomRight);
#ifdef __QUICKDRAW__
	URect(const Rect & macRect);
#endif
#ifdef __AFX_H__
	URect(const CRect & winRect);
#endif

/* Attributes (in addition to RECT members) */

	/* retrieves the width */
	int Width() const;
	/* returns the height */
	int Height() const;
	/* returns the size */
	USize Size() const;
	/* reference to the top-left point */
	UPoint& TopLeft();
	/* reference to the bottom-right point */
	UPoint& BottomRight();
	/* const reference to the top-left point */
	const UPoint& TopLeft() const;
	/* const reference to the bottom-right point */
	const UPoint& BottomRight() const;
	/* the geometric center point of the rectangle */
	UPoint CenterPoint() const;

	/* convert between URect and LPRECT/const RECT* (no need for &) */
	operator URECT*();
	operator const URECT*() const;
#ifdef __QUICKDRAW__
	operator Rect() const;
#endif
#ifdef __AFX_H__
	operator CRect() const;
#endif

	/* returns TRUE if rectangle has no area */
	bool IsRectEmpty() const;
	/* returns TRUE if rectangle is at (0,0) and has no area */
	bool IsRectNull() const;
	/* returns TRUE if point is within rectangle */
	bool PtInRect(UPOINT point) const;

/* Operations */

	/* set rectangle from left, top, right, and bottom */
	void SetRect(int x1, int y1, int x2, int y2);
	void SetRect(UPOINT topLeft, UPOINT bottomRight);
	/* empty the rectangle */
	void SetRectEmpty();
	/* copy from another rectangle */
	void CopyRect(const URECT* lpSrURect);
	/* TRUE if exactly the same as another rectangle */
	bool EqualRect(const URECT* lpRect) const;

	/* inflate rectangle's width and height without */
	/* moving its top or left */
	void InflateRect(int x, int y);
	void InflateRect(USIZE size);
	void InflateRect(const URECT* lpRect);
	void InflateRect(int l, int t, int r, int b);
	/* deflate the rectangle's width and height without */
	/* moving its top or left */
	void DeflateRect(int x, int y);
	void DeflateRect(USIZE size);
	void DeflateRect(const URECT* lpRect);
	void DeflateRect(int l, int t, int r, int b);

	/* translate the rectangle by moving its top and left */
	void OffsetRect(int x, int y);
	void OffsetRect(USIZE size);
	void OffsetRect(UPOINT point);
	void NormalizeRect();

	/* set this rectangle to intersection of two others */
	bool IntersectRect(const URECT* lpRect1, const URECT* lpRect2);

	/* set this rectangle to bounding union of two others */
	bool UnionRect(const URECT* lpRect1, const URECT* lpRect2);

	/* set this rectangle to minimum of two others */
/*  	bool SubtractRect(const RECT* lpRectSrc1, const RECT* lpRectSrc2); */

/* Additional Operations */
	void operator=(const URECT& srURect);
	bool operator==(const URECT& rect) const;
	bool operator!=(const URECT& rect) const;
	void operator+=(UPOINT point);
	void operator+=(USIZE size);
	void operator+=(const URECT* lpRect);
	void operator-=(UPOINT point);
	void operator-=(USIZE size);
	void operator-=(const URECT* lpRect);
	void operator&=(const URECT& rect);
	void operator|=(const URECT& rect);

/* Operators returning URect values */
	URect operator+(UPOINT point) const;
	URect operator-(UPOINT point) const;
	URect operator+(const URECT* lpRect) const;
	URect operator+(USIZE size) const;
	URect operator-(USIZE size) const;
	URect operator-(const URECT* lpRect) const;
	URect operator&(const URECT& rect2) const;
	URect operator|(const URECT& rect2) const;
	URect MulDiv(int nMultiplier, int nDivisor) const;
};

/* USize */
inline USize::USize()
	{ /* random filled */ }
inline USize::USize(int initCX, int initCY)
	{ cx = initCX; cy = initCY; }
inline USize::USize(USIZE initSize)
	{ *(USIZE*)this = initSize; }
inline USize::USize(UPOINT initPt)
	{ *(UPOINT*)this = initPt; }
inline bool USize::operator==(USIZE size) const
	{ return (cx == size.cx && cy == size.cy); }
inline bool USize::operator!=(USIZE size) const
	{ return (cx != size.cx || cy != size.cy); }
inline void USize::operator+=(USIZE size)
	{ cx += size.cx; cy += size.cy; }
inline void USize::operator-=(USIZE size)
	{ cx -= size.cx; cy -= size.cy; }
inline USize USize::operator+(USIZE size) const
	{ return USize(cx + size.cx, cy + size.cy); }
inline USize USize::operator-(USIZE size) const
	{ return USize(cx - size.cx, cy - size.cy); }
inline USize USize::operator-() const
	{ return USize(-cx, -cy); }
inline UPoint USize::operator+(UPOINT point) const
	{ return UPoint(cx + point.x, cy + point.y); }
inline UPoint USize::operator-(UPOINT point) const
	{ return UPoint(cx - point.x, cy - point.y); }
inline URect USize::operator+(const URECT* lpRect) const
	{ return URect(lpRect) + *this; }
inline URect USize::operator-(const URECT* lpRect) const
	{ return URect(lpRect) - *this; }

/* UPoint */
inline UPoint::UPoint()
	{ /* random filled */ }
inline UPoint::UPoint(int initX, int initY)
	{ x = initX; y = initY; }
inline UPoint::UPoint(UPOINT initPt)
	{ *(UPOINT*)this = initPt; }
inline UPoint::UPoint(USIZE initSize)
	{ *(USIZE*)this = initSize; }
inline void UPoint::Offset(int xOffset, int yOffset)
	{ x += xOffset; y += yOffset; }
inline void UPoint::Offset(UPOINT point)
	{ x += point.x; y += point.y; }
inline void UPoint::Offset(USIZE size)
	{ x += size.cx; y += size.cy; }
inline bool UPoint::operator==(UPOINT point) const
	{ return (x == point.x && y == point.y); }
inline bool UPoint::operator!=(UPOINT point) const
	{ return (x != point.x || y != point.y); }
inline void UPoint::operator+=(USIZE size)
	{ x += size.cx; y += size.cy; }
inline void UPoint::operator-=(USIZE size)
	{ x -= size.cx; y -= size.cy; }
inline void UPoint::operator+=(UPOINT point)
	{ x += point.x; y += point.y; }
inline void UPoint::operator-=(UPOINT point)
	{ x -= point.x; y -= point.y; }
inline UPoint UPoint::operator+(USIZE size) const
	{ return UPoint(x + size.cx, y + size.cy); }
inline UPoint UPoint::operator-(USIZE size) const
	{ return UPoint(x - size.cx, y - size.cy); }
inline UPoint UPoint::operator-() const
	{ return UPoint(-x, -y); }
inline UPoint UPoint::operator+(UPOINT point) const
	{ return UPoint(x + point.x, y + point.y); }
inline USize UPoint::operator-(UPOINT point) const
	{ return USize(x - point.x, y - point.y); }
inline URect UPoint::operator+(const URECT* lpRect) const
	{ return URect(lpRect) + *this; }
inline URect UPoint::operator-(const URECT* lpRect) const
	{ return URect(lpRect) - *this; }

/* URect */
inline URect::URect()
	{ /* random filled */ }
inline URect::URect(int l, int t, int r, int b)
	{ left = l; top = t; right = r; bottom = b; }
inline URect::URect(const URECT& srURect)
	{ ::CopyRect(this, &srURect); }
inline URect::URect(const URECT* lpSrURect)
	{ ::CopyRect(this, lpSrURect); }
inline URect::URect(UPOINT point, USIZE size)
	{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
inline URect::URect(UPOINT topLeft, UPOINT bottomRight)
	{ left = topLeft.x; top = topLeft.y;
		right = bottomRight.x; bottom = bottomRight.y; }
#ifdef __QUICKDRAW__
inline URect::URect(const Rect & macRect)
	{ left = macRect.left; right = macRect.right; top = macRect.top; bottom = macRect.bottom; }
#endif
#ifdef __AFX_H__
inline URect::URect(const CRect & winRect)
	{ left = winRect.left; right = winRect.right; top = winRect.top; bottom = winRect.bottom; }
#endif
inline int URect::Width() const
	{ return right - left; }
inline int URect::Height() const
	{ return bottom - top; }
inline USize URect::Size() const
	{ return USize(right - left, bottom - top); }
inline UPoint& URect::TopLeft()
	{ return *((UPoint*)this); }
inline UPoint& URect::BottomRight()
	{ return *((UPoint*)this+1); }
inline const UPoint& URect::TopLeft() const
	{ return *((UPoint*)this); }
inline const UPoint& URect::BottomRight() const
	{ return *((UPoint*)this+1); }
inline UPoint URect::CenterPoint() const
	{ return UPoint((left+right)/2, (top+bottom)/2); }
inline URect::operator URECT*()
	{ return this; }
inline URect::operator const URECT*() const
	{ return this; }
#ifdef __QUICKDRAW__
inline URect::operator Rect() const
{
	Rect r; ::MacSetRect(&r, left, top, right, bottom); return r;
}
#endif
#ifdef __AFX_H__
inline URect::operator CRect() const
{
	return CRect(left, top, right, bottom);
}
#endif
inline bool URect::IsRectEmpty() const
	{ return left == right || top == bottom; }
inline bool URect::IsRectNull() const
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
inline bool URect::PtInRect(UPOINT point) const
	{ return ::PtInRect(this, point); }
inline void URect::SetRect(int x1, int y1, int x2, int y2)
	{ ::SetRect(this, x1, y1, x2, y2); }
inline void URect::SetRect(UPOINT topLeft, UPOINT bottomRight)
	{ ::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
inline void URect::SetRectEmpty()
	{ ::SetRectEmpty(this); }
inline void URect::CopyRect(const URECT* lpSrURect)
	{ ::CopyRect(this, lpSrURect); }
inline bool URect::EqualRect(const URECT* lpRect) const
	{ return ::EqualRect(this, lpRect); }
inline void URect::InflateRect(int x, int y)
	{ ::InflateRect(this, x, y); }
inline void URect::InflateRect(USIZE size)
	{ ::InflateRect(this, size.cx, size.cy); }
inline void URect::DeflateRect(int x, int y)
	{ ::InflateRect(this, -x, -y); }
inline void URect::DeflateRect(USIZE size)
	{ ::InflateRect(this, -size.cx, -size.cy); }
inline void URect::OffsetRect(int x, int y)
	{ ::OffsetRect(this, x, y); }
inline void URect::OffsetRect(UPOINT point)
	{ ::OffsetRect(this, point.x, point.y); }
inline void URect::OffsetRect(USIZE size)
	{ ::OffsetRect(this, size.cx, size.cy); }
inline bool URect::IntersectRect(const URECT* lpRect1, const URECT* lpRect2)
	{ return ::IntersectRect(this, lpRect1, lpRect2);}
inline bool URect::UnionRect(const URECT* lpRect1, const URECT* lpRect2)
	{ return ::UnionRect(this, lpRect1, lpRect2); }
inline void URect::operator=(const URECT& srURect)
	{ ::CopyRect(this, &srURect); }
inline bool URect::operator==(const URECT& rect) const
	{ return ::EqualRect(this, &rect); }
inline bool URect::operator!=(const URECT& rect) const
	{ return !::EqualRect(this, &rect); }
inline void URect::operator+=(UPOINT point)
	{ ::OffsetRect(this, point.x, point.y); }
inline void URect::operator+=(USIZE size)
	{ ::OffsetRect(this, size.cx, size.cy); }
inline void URect::operator+=(const URECT* lpRect)
	{ InflateRect(lpRect); }
inline void URect::operator-=(UPOINT point)
	{ ::OffsetRect(this, -point.x, -point.y); }
inline void URect::operator-=(USIZE size)
	{ ::OffsetRect(this, -size.cx, -size.cy); }
inline void URect::operator-=(const URECT* lpRect)
	{ DeflateRect(lpRect); }
inline void URect::operator&=(const URECT& rect)
	{ ::IntersectRect(this, this, &rect); }
inline void URect::operator|=(const URECT& rect)
	{ ::UnionRect(this, this, &rect); }
inline URect URect::operator+(UPOINT pt) const
	{ URect rect(*this); ::OffsetRect(&rect, pt.x, pt.y); return rect; }
inline URect URect::operator-(UPOINT pt) const
	{ URect rect(*this); ::OffsetRect(&rect, -pt.x, -pt.y); return rect; }
inline URect URect::operator+(USIZE size) const
	{ URect rect(*this); ::OffsetRect(&rect, size.cx, size.cy); return rect; }
inline URect URect::operator-(USIZE size) const
	{ URect rect(*this); ::OffsetRect(&rect, -size.cx, -size.cy); return rect; }
inline URect URect::operator+(const URECT* lpRect) const
	{ URect rect(this); rect.InflateRect(lpRect); return rect; }
inline URect URect::operator-(const URECT* lpRect) const
	{ URect rect(this); rect.DeflateRect(lpRect); return rect; }
inline URect URect::operator&(const URECT& rect2) const
	{ URect rect; ::IntersectRect(&rect, this, &rect2);
		return rect; }
inline URect URect::operator|(const URECT& rect2) const
	{ URect rect; ::UnionRect(&rect, this, &rect2);
		return rect; }
/*  inline bool URect::SubtractRect(const URECT* lpRectSrc1, const URECT* lpRectSrc2)
  	{ return ::SubtractRect(this, lpRectSrc1, lpRectSrc2); }*/

typedef long UCOLORREF;
#define URGB(r,g,b) ((UCOLORREF)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(b))<<16)))
#define UGetRValue(rgb)      ((unsigned char)(rgb))
#define UGetGValue(rgb)      ((unsigned char)(((unsigned short)(rgb)) >> 8))
#define UGetBValue(rgb)      ((unsigned char)((rgb)>>16))

#define UPS_SOLID 0

class UPen
{
public:
	UPen();
	~UPen();

	bool DeleteObject(void);

	bool CreatePen(int nPenStyle, int nWidth, UCOLORREF crColor);
protected:
	int m_width;
	int m_style;
#if qGTK
	GdkColor m_col;
	bool m_set;
#endif

friend class UDC;
};

class UBrush
{
public:
	UBrush();
	~UBrush();

	bool DeleteObject(void);

	bool CreateSolidBrush(UCOLORREF crColor);
protected:
#if qGTK
	GdkColor m_col;
	bool m_set;
#endif

friend class UDC;
};

class UWidget;

class UDC
{
public:
	UDC(int widid, int cmdid);
	~UDC();

	USize GetTextExtent(const char *lpszString, int nCount) const;
	int GetTextHeight(void) const;
	bool TextOutClipped(int x, int y, const URECT *lpRect,
						const char *lpszString, int nCount);
	
	void MoveTo(int x, int y);
	void LineTo(int x, int y);
	bool DrawBitmap(void *bitmap, UPOINT where);

	UPen *SelectObject(UPen* pPen);
	UBrush *SelectObject(UBrush* pBrush);

	bool Rectangle(const URECT *lpRect);
	bool RoundRect(const URECT *lpRect, UPOINT point);
protected:
	void *m_wid;
	UPen *m_pen;
	UBrush *m_brush;
#if qGTK
	GdkGC *m_gc;
	UPoint m_penloc;
	GdkColor m_foreground, m_background;
	GdkFont *m_font;
	GdkDrawable *m_drawable;
	UPoint m_orig;
#endif
};
#endif /* __cplusplus */

#endif /* UDRAW_H */
