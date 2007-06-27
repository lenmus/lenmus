//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//    This file contains some code copied (and modified) from file include/wx/gdicmn.h
//    from wxWidgets 2.8.0 project:
//      - class lmURect is based in wxRect code.
//      - class lmUSize is based in wxSize code.
//    Author:       Julian Smart
//    Copyright (c) Julian Smart
//
//-------------------------------------------------------------------------------------

// Common definitions for all score kernel

#ifndef __DEFS_H__        //to avoid nested includes
#define __DEFS_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "defs.cpp"
#endif

#include "wx/gdicmn.h"

//// Types used for portability and legibility

//units
#define lmLUnits    float           // lmPaper logical units.
#define lmTenths    float           // Staff relative units.
#define lmPixels    int             // Device units (usually pixels).  32 bits int

#define lmUPoint    wxRealPoint     // a point in logical units
#define lmDPoint    wxPoint         // a point in device units


// prefixes for variables:
//
//  a) standard types:
//      p - pointer
//      n - integer, long
//      s - string
//      r - real (float, double)
//
//  b) program specific types:
//      u - logical units (lmLUnits)
//      v - device units (lmPixels)
//      ? - tenths (lmTenths)
//      ux, uy - logical units. coordinates
//      vx, vy - device units. coordinates
//



//pitch
#define lmPitch        int        // Midi pitch or diatonic pitch

//// Other definitions

// Pitch conversion
#define lmC0PITCH         1        // Pitch assigned to C0
#define lmC1PITCH         8        // Pitch assigned to C1
#define lmC2PITCH        15        // Pitch assigned to C2
#define lmC3PITCH        22        // Pitch assigned to C3
#define lmC4PITCH        29        // Pitch assigned to C4
#define lmC5PITCH        36        // Pitch assigned to C5
#define lmC6PITCH        43        // Pitch assigned to C6
#define lmC7PITCH        50        // Pitch assigned to C7
#define lmC8PITCH        57        // Pitch assigned to C8


// DC user mode
#define lmDC_MODE    wxMM_LOMETRIC    // mode for DC (DC unit: one tenth of mm)

// factor to multiply the program internal unit (logical units) so that
// they became DC units: one tenth of mm

//#define lmSCALE      1.0              // logical units: one tenth of millimiter
#define lmSCALE      0.1              // logical units: one cent of millimiter
//#define lmSCALE      0.01             // logical units: one micron

enum lmEUnits {
    lmMICRONS = 0,
    lmMILLIMETERS,
    lmCENTIMETERS,
    lmINCHES,
    lmTENTHS
};

// defined in TheApp.cpp
extern lmLUnits lmToLogicalUnits(int nValue, lmEUnits nUnits);
extern lmLUnits lmToLogicalUnits(double rValue, lmEUnits nUnits);



//-------------------------------------------------------------------------------------
// class lmUSize. Size object in lmLUnits. Based on wxSize
//-------------------------------------------------------------------------------------

class lmUSize
{
public:
    // members are public for compatibility, don't use them directly.
    lmLUnits x, y;

    // constructors
    lmUSize() : x(0.0), y(0.0) { }
    lmUSize(lmLUnits ux, lmLUnits uy) : x(ux), y(uy) { }

    //// no copy ctor or assignment operator - the defaults are ok

    //bool operator==(const lmUSize& sz) const { return x == sz.x && y == sz.y; }
    //bool operator!=(const lmUSize& sz) const { return x != sz.x || y != sz.y; }

    //lmUSize operator+(const lmUSize& sz) const { return lmUSize(x + sz.x, y + sz.y); }
    //lmUSize operator-(const lmUSize& sz) const { return lmUSize(x - sz.x, y - sz.y); }
    //lmUSize operator/(lmLUnits i) const { return lmUSize(x / i, y / i); }
    //lmUSize operator*(lmLUnits i) const { return lmUSize(x * i, y * i); }

    //lmUSize& operator+=(const lmUSize& sz) { x += sz.x; y += sz.y; return *this; }
    //lmUSize& operator-=(const lmUSize& sz) { x -= sz.x; y -= sz.y; return *this; }
    //lmUSize& operator/=(const lmLUnits i) { x /= i; y /= i; return *this; }
    //lmUSize& operator*=(const lmLUnits i) { x *= i; y *= i; return *this; }

    //void IncTo(const lmUSize& sz)
    //    { if ( sz.x > x ) x = sz.x; if ( sz.y > y ) y = sz.y; }
    //void DecTo(const lmUSize& sz)
    //    { if ( sz.x < x ) x = sz.x; if ( sz.y < y ) y = sz.y; }

    //void IncBy(lmLUnits dx, lmLUnits dy) { x += dx; y += dy; }
    //void IncBy(const lmUSize& sz) { IncBy(sz.x, sz.y); }
    //void IncBy(lmLUnits d) { IncBy(d, d); }

    //void DecBy(lmLUnits dx, lmLUnits dy) { IncBy(-dx, -dy); }
    //void DecBy(const lmUSize& sz) { DecBy(sz.x, sz.y); }
    //void DecBy(lmLUnits d) { DecBy(d, d); }


    //lmUSize& Scale(float xscale, float yscale)
    //    { x = (lmLUnits)(x*xscale); y = (lmLUnits)(y*yscale); return *this; }

    //// accessors
    //void Set(lmLUnits xx, lmLUnits yy) { x = xx; y = yy; }
    //void SetWidth(lmLUnits w) { x = w; }
    //void SetHeight(lmLUnits h) { y = h; }

    lmLUnits GetWidth() const { return x; }
    lmLUnits GetHeight() const { return y; }

    //bool IsFullySpecified() const { return x != wxDefaultCoord && y != wxDefaultCoord; }

    //// combine this size with the other one replacing the default (i.e. equal
    //// to wxDefaultCoord) components of this object with those of the other
    //void SetDefaults(const lmUSize& size)
    //{
    //    if ( x == wxDefaultCoord )
    //        x = size.x;
    //    if ( y == wxDefaultCoord )
    //        y = size.y;
    //}

    //// compatibility
    //lmLUnits GetX() const { return x; }
    //lmLUnits GetY() const { return y; }
};


//-------------------------------------------------------------------------------------
// class lmURect code. A real rectangle (lmLUnits). Based on wxRect
//-------------------------------------------------------------------------------------

class lmURect
{
public:
    lmURect()
        : x(0.0), y(0.0), width(0.0), height(0.0)
        { }
    lmURect(float xx, float yy, float ww, float hh)
        : x(xx), y(yy), width(ww), height(hh)
        { }
    lmURect(const wxRealPoint& topLeft, const wxRealPoint& bottomRight);
    lmURect(const wxRealPoint& pt, const wxSize& size)
        : x(pt.x), y(pt.y), width(size.x), height(size.y)
        { }
    lmURect(const wxSize& size)
        : x(0.0), y(0.0), width(size.x), height(size.y)
        { }

    // default copy ctor and assignment operators ok

    float GetX() const { return x; }
    void SetX(float xx) { x = xx; }

    float GetY() const { return y; }
    void SetY(float yy) { y = yy; }

    float GetWidth() const { return width; }
    void SetWidth(float w) { width = w; }

    float GetHeight() const { return height; }
    void SetHeight(float h) { height = h; }

    wxRealPoint GetPosition() const { return wxRealPoint(x, y); }
    void SetPosition( const wxRealPoint &p ) { x = p.x; y = p.y; }

    lmUSize GetSize() const { return lmUSize(width, height); }
    void SetSize( const lmUSize &s ) { width = s.GetWidth(); height = s.GetHeight(); }

    bool IsEmpty() const { return (width <= 0) || (height <= 0); }

    float GetLeft()   const { return x; }
    float GetTop()    const { return y; }
    float GetBottom() const { return y + height - 1; }
    float GetRight()  const { return x + width - 1; }

    void SetLeft(float left) { x = left; }
    void SetRight(float right) { width = right - x + 1; }
    void SetTop(float top) { y = top; }
    void SetBottom(float bottom) { height = bottom - y + 1; }

    wxRealPoint GetTopLeft() const { return GetPosition(); }
    wxRealPoint GetLeftTop() const { return GetTopLeft(); }
    void SetTopLeft(const wxRealPoint &p) { SetPosition(p); }
    void SetLeftTop(const wxRealPoint &p) { SetTopLeft(p); }

    wxRealPoint GetBottomRight() const { return wxRealPoint(GetRight(), GetBottom()); }
    wxRealPoint GetRightBottom() const { return GetBottomRight(); }
    void SetBottomRight(const wxRealPoint &p) { SetRight(p.x); SetBottom(p.y); }
    void SetRightBottom(const wxRealPoint &p) { SetBottomRight(p); }

    wxRealPoint GetTopRight() const { return wxRealPoint(GetRight(), GetTop()); }
    wxRealPoint GetRightTop() const { return GetTopRight(); }
    void SetTopRight(const wxRealPoint &p) { SetRight(p.x); SetTop(p.y); }
    void SetRightTop(const wxRealPoint &p) { SetTopLeft(p); }

    wxRealPoint GetBottomLeft() const { return wxRealPoint(GetLeft(), GetBottom()); }
    wxRealPoint GetLeftBottom() const { return GetBottomLeft(); }
    void SetBottomLeft(const wxRealPoint &p) { SetLeft(p.x); SetBottom(p.y); }
    void SetLeftBottom(const wxRealPoint &p) { SetBottomLeft(p); }

    //// operations with rect
    //lmURect& Inflate(wxCoord dx, wxCoord dy);
    //lmURect& Inflate(const wxSize& d) { return Inflate(d.x, d.y); }
    //lmURect& Inflate(wxCoord d) { return Inflate(d, d); }
    //lmURect Inflate(wxCoord dx, wxCoord dy) const
    //{
    //    lmURect r = *this;
    //    r.Inflate(dx, dy);
    //    return r;
    //}

    //lmURect& Deflate(wxCoord dx, wxCoord dy) { return Inflate(-dx, -dy); }
    //lmURect& Deflate(const wxSize& d) { return Inflate(-d.x, -d.y); }
    //lmURect& Deflate(wxCoord d) { return Inflate(-d); }
    //lmURect Deflate(wxCoord dx, wxCoord dy) const
    //{
    //    lmURect r = *this;
    //    r.Deflate(dx, dy);
    //    return r;
    //}

    //void Offset(wxCoord dx, wxCoord dy) { x += dx; y += dy; }
    //void Offset(const wxRealPoint& pt) { Offset(pt.x, pt.y); }

    lmURect& Intersect(const lmURect& rect);
    lmURect Intersect(const lmURect& rect) const
    {
        lmURect r = *this;
        r.Intersect(rect);
        return r;
    }

    lmURect& Union(const lmURect& rect);
    lmURect Union(const lmURect& rect) const
    {
        lmURect r = *this;
        r.Union(rect);
        return r;
    }

    //// compare rectangles
    //bool operator==(const lmURect& rect) const;
    //bool operator!=(const lmURect& rect) const { return !(*this == rect); }

    // return true if the point is (not strictly) inside the rect
    bool Contains(float x, float y) const;
    bool Contains(const wxRealPoint& pt) const { return Contains(pt.x, pt.y); }
    // return true if the rectangle is (not strictly) inside the rect
    bool Contains(const lmURect& rect) const;

    // return true if the rectangles have a non empty intersection
    bool Intersects(const lmURect& rect) const;


    //// these are like Union() but don't ignore empty rectangles
    //lmURect operator+(const lmURect& rect) const;
    //lmURect& operator+=(const lmURect& rect)
    //{
    //    *this = *this + rect;
    //    return *this;
    //}


    //// centre this rectangle in the given (usually, but not necessarily,
    //// larger) one
    //lmURect CentreIn(const lmURect& r, float dir = wxBOTH) const
    //{
    //    return lmURect(dir & wxHORIZONTAL ? r.x + (r.width - width)/2 : x,
    //                  dir & wxVERTICAL ? r.y + (r.height - height)/2 : y,
    //                  width, height);
    //}

    //lmURect CenterIn(const lmURect& r, float dir = wxBOTH) const
    //{
    //    return CentreIn(r, dir);
    //}

public:
    float x, y, width, height;
};


// conversions
#define lmUPointToPoint(uPoint) \
        wxPoint((int)floor(uPoint.x + 0.5), (int)floor(uPoint.y + 0.5))
#define lmDPointToPoint(vPoint) \
        vPoint

#define lmUSizeToSize(uSize) \
        wxSize((int)floor(uSize.x + 0.5), (int)floor(uSize.y + 0.5))



#endif    // __DEFS_H__
