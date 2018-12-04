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

#include "stdafx.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "udraw.h"
#include "uwidget.h"

#ifndef MIN
#	define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#	define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

bool PtInRect(const URECT *r, UPOINT pt)
{
	return pt.x >= r->left && pt.x < r->right &&
		pt.y >= r->top && pt.y < r->bottom;
}

void SetRect(URECT *r, int x1, int y1, int x2, int y2)
{
	r->left = x1;
	r->top = y1;
	r->right = x2;
	r->bottom = y2;
	if(r->left > r->right)
		r->left = r->right;
	if(r->top > r->bottom)
		r->top = r->bottom;
}

void SetRectEmpty(URECT *r)
{
	r->left = 0;
	r->top = 0;
	r->right = 0;
	r->bottom = 0;
}

bool EqualRect(const URECT *r1, const URECT *r2)
{
	return r1->left == r2->left && r1->top == r2->top &&
		r1->right == r2->right && r1->bottom == r2->bottom;
}
void InflateRect(URECT *r, int x, int y)
{
	r->left += x;
	r->top += y;
	r->right -= x;
	r->bottom -= y;
	if(r->left > r->right)
		r->left = r->right;
	if(r->top > r->bottom)
		r->top = r->bottom;
}

void OffsetRect(URECT *r, int x, int y)
{
	r->left += x;
	r->right += x;
	r->top += y;
	r->bottom += y;
}

inline bool IsRectEmpty(const URECT *r)
{
	return r->left == r->right || r->top == r->bottom;
}

bool UnionRect(URECT *dest, const URECT *src1, const URECT *src2)
{
	if(::IsRectEmpty(src1))
	{
		*dest = *src2;
		return true;
	}
	if(::IsRectEmpty(src2))
	{
		*dest = *src1;
		return true;
	}
	dest->left = MIN (src1->left, src2->left);
	dest->top = MIN (src1->top, src2->top);
	dest->right = MAX (src1->right, src2->right);
	dest->bottom = MAX (src1->bottom, src2->bottom);
	return true;
}

bool IntersectRect(URECT *dest, const URECT *src1, const URECT *src2)
{
	const URECT *temp;
	bool return_val = false;
	
	if (src2->left < src1->left)
    {
		temp = src1;
		src1 = src2;
		src2 = temp;
    }
	dest->left = src2->left;
	
	if (src2->left < src1->right)
    {
		if (src1->right < src2->right)
			dest->right = src1->right;
		else
			dest->right = src2->right;
		
		if (src2->top < src1->top)
		{
			temp = src1;
			src1 = src2;
			src2 = temp;
		}
		dest->top = src2->top;
		
		if (src2->top < src1->bottom)
		{
			return_val = true;
			
			if (src1->bottom < src2->bottom)
				dest->bottom = src1->bottom;
			else
				dest->bottom = src2->bottom;
			
			if (dest->left == dest->right)
				return_val = false;
			if (dest->top == dest->bottom)
				return_val = false;
		}
    }
	
	return return_val;
}

UDC::UDC(int widid, int cmdid) : m_wid(0L), m_pen(0L), m_brush(0L)
#if qGTK
	, m_gc(0L), m_penloc(0, 0), m_drawable(0L), m_orig(0, 0)
#endif
{	
	UCmdTarget *target = UEventQueryWidget(widid);
	if(target == 0L || !target->IsKindOf(URUNTIME_CLASS(UWidget)))
		return;

	m_wid = ((UWidget *)target)->GetWidget(cmdid);
#if qGTK
	if(m_wid != 0L)
	{
		GtkWidget *widget = GTK_WIDGET(m_wid);

		if(GTK_IS_DRAWING_AREA(widget))
		{
			m_drawable = GTK_DRAWING_AREA(widget)->widget.window;
			m_orig.x = 0;
			m_orig.y = 0;
		}
		else
			m_drawable = m_drawable;
		m_gc = gdk_gc_new(m_drawable);
		if(m_gc == 0L)
			m_wid = 0L;
		else
		{
			GdkGCValues values;
			gdk_gc_get_values(m_gc, &values);
			m_foreground = values.foreground;
			m_background = values.background;
			m_font = gdk_font_load ("-adobe-*-medium-r-normal--*-120-*-*-*-*-iso8859-*");
			if(m_font == 0L)
				m_font = gdk_font_load ("-adobe-courier-medium-r-normal--*-120-*-*-*-*-*-*");
			if(m_font != 0L)
				gdk_gc_set_font(m_gc, m_font);
		}
	}
#endif
}

UPen *UDC::SelectObject(UPen* pPen)
{
	if(m_wid == 0L)
		return 0L;

	UPen *oldpen = m_pen;
	m_pen = pPen;

#if qGTK
	if(m_pen != 0L && m_pen->m_set)
		gdk_gc_set_foreground(m_gc, &m_pen->m_col);
	else
		gdk_gc_set_foreground(m_gc, &m_foreground);
#endif
	
	return oldpen;
}
 
UBrush *UDC::SelectObject(UBrush* pBrush)
{
	if(m_wid == 0L)
		return 0L;

	UBrush *oldbrush = m_brush;
	m_brush = pBrush;

#if qGTK
	if(m_brush != 0L && m_brush->m_set)
		gdk_gc_set_background(m_gc, &m_brush->m_col);
	else
		gdk_gc_set_background(m_gc, &m_background);
#endif
	
	return oldbrush;
}

UDC::~UDC()
{
#if qGTK
	if(m_gc != 0L)
		gdk_gc_destroy(m_gc);
	if(m_font != 0L)
		gdk_font_unref (m_font);
#endif
}

bool UDC::Rectangle(const URECT *lpRect)
{
	if(m_wid == 0L)
		return false;

#if qGTK	
	URect r(*lpRect);
	if(m_brush != 0L && m_brush->m_set)
	{
		gdk_gc_set_foreground(m_gc, &m_brush->m_col);
		gdk_draw_rectangle (m_drawable, m_gc, TRUE,
							r.left + m_orig.x, r.top + m_orig.y, r.Width(), r.Height());
		SelectObject(m_pen);
	}
	gdk_draw_rectangle (m_drawable, m_gc, FALSE,
						r.left + m_orig.x, r.top + m_orig.y, r.Width(), r.Height());
#endif
	return true;
}

bool UDC::RoundRect(const URECT *lpRect, UPOINT point)
{
	if(m_wid == 0L)
		return false;

#if qGTK
	URect r(*lpRect);
	if(m_brush != 0L && m_brush->m_set)
	{
		gdk_gc_set_foreground(m_gc, &m_brush->m_col);
		gdk_draw_rectangle (m_drawable, m_gc, TRUE,
							r.left + m_orig.x, r.top + m_orig.y, r.Width(), r.Height());
		SelectObject(m_pen);
	}
	gdk_draw_rectangle (m_drawable, m_gc, FALSE,
						r.left + m_orig.x, r.top + m_orig.y, r.Width(), r.Height());
#endif
	return true;
}

void UDC::MoveTo(int x, int y)
{
	if(m_wid == 0L)
		return;

#if qGTK
	m_penloc.x = x + m_orig.x;
	m_penloc.y = y + m_orig.y;
#endif
}

void UDC::LineTo(int x, int y)
{
	if(m_wid == 0L)
		return;

#if qGTK
	gdk_draw_line(m_drawable, m_gc,
				  m_penloc.x, m_penloc.y, x + m_orig.x, y + m_orig.y);
	m_penloc.x = x + m_orig.x;
	m_penloc.y = y + m_orig.y;
#endif
}

USize UDC::GetTextExtent(const char *lpszString, int nCount) const
{
	USize res(0, 0);
	if(m_wid == 0L)
		return res;

#if qGTK
	if(m_font == 0L)
		return res;

	res.cx = gdk_text_width(m_font, lpszString, nCount);
	res.cy = gdk_text_height(m_font, lpszString, nCount);
#endif

	return res;
}

int UDC::GetTextHeight(void) const
{
	int res = 0;
	if(m_wid == 0L)
		return res;

#if qGTK
	if(m_font == 0L)
		return res;

	res = m_font->ascent + m_font->descent;
#endif

	return res;
}

bool UDC::TextOutClipped(int x, int y, const URECT *lpRect,
						 const char *lpszString, int nCount)
{
	if(m_wid == 0L)
		return false;

#if qGTK
	if(m_font == 0L)
		return false;

	USize size = GetTextExtent(lpszString, nCount);
	URect r(*lpRect);
	UPoint out(r.left, r.bottom);
	out += UPoint((r.Width() - size.cx) / 2, -(r.Height() - size.cy) / 2);
	gdk_draw_text(m_drawable, m_font, m_gc, out.x + m_orig.x, out.y + m_orig.y,
				  lpszString, nCount);
#endif

	return true;
}

bool UDC::DrawBitmap(void *bitmap, UPOINT where)
{
	if(m_wid == 0L)
		return false;

#if qGTK
	if(m_font == 0L)
		return false;

	gdk_draw_pixmap(m_drawable, m_gc, GTK_PIXMAP(bitmap)->pixmap,
					0, 0, where.x + m_orig.x, where.y + m_orig.y, -1, -1);
#endif

	return true;
}

UPen::UPen()
#if qGTK
	: m_set(false)
#endif
{
}

UPen::~UPen()
{
	DeleteObject();
}

bool UPen::DeleteObject(void)
{
	bool res = false;
#if qGTK
	if(m_set)
	{
		m_set = false;
		GdkColormap *cmap = gdk_colormap_get_system();
		if(cmap != 0L)
		{
			gdk_colormap_free_colors(cmap, &m_col, 1);
			res = true;
		}
	}
	else
		res = true;
#endif
	return res;
}

bool UPen::CreatePen(int nPenStyle, int nWidth, UCOLORREF crColor)
{
	bool res = false;

	m_width = nWidth;
	m_style = nPenStyle;

#if qGTK
	DeleteObject();
	GdkColormap *cmap = gdk_colormap_get_system();
	if(cmap != 0L)
	{
		m_col.red = UGetRValue(crColor) << 8;
		m_col.green = UGetGValue(crColor) << 8;
		m_col.blue = UGetBValue(crColor) << 8;
		if(gdk_color_alloc(cmap, &m_col))
			m_set = true;
	}
	res = m_set;
#endif
	return res;
}

UBrush::UBrush()
#if qGTK
	: m_set(false)
#endif
{
}

UBrush::~UBrush()
{
	DeleteObject();
}

bool UBrush::DeleteObject(void)
{
	bool res = false;
#if qGTK
	if(m_set)
	{
		m_set = false;
		GdkColormap *cmap = gdk_colormap_get_system();
		if(cmap != 0L)
		{
			gdk_colormap_free_colors(cmap, &m_col, 1);
			res = true;
		}
	}
	else
		res = true;
#endif
	return res;
}

bool UBrush::CreateSolidBrush(UCOLORREF crColor)
{
	bool res = false;

#if qGTK
	DeleteObject();
	GdkColormap *cmap = gdk_colormap_get_system();
	if(cmap != 0L)
	{
		m_col.red = UGetRValue(crColor) << 8;
		m_col.green = UGetGValue(crColor) << 8;
		m_col.blue = UGetBValue(crColor) << 8;
		if(gdk_color_alloc(cmap, &m_col))
			m_set = true;
	}
	res = m_set;
#endif
	return res;
}
