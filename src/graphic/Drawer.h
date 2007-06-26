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
//-------------------------------------------------------------------------------------

#ifndef __DRAWER_H__        //to avoid nested includes
#define __DRAWER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Drawer.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"      // lmLUnits, lmPixels

enum lmELineEdges
{
    eEdgeNormal = 0,        // edge line is perpendicular to line
    eEdgeVertical,          // edge is always a vertical line
    eEdgeHorizontal         // edge is always a horizontal line
};

class lmDrawer
{
public:
    lmDrawer(wxDC* pDC);
    virtual ~lmDrawer() {}

    // Aliased shapes, even when anti-alising is supported.
    virtual void SketchLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2, wxColour color) = 0;
    virtual void SketchRectangle(lmUPoint uPoint, lmUSize uSize, wxColour color) = 0;

    //solid shapes, anti-aliased when supported.
    virtual void SolidLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                          lmLUnits width, lmELineEdges nEdge, wxColour color);
    virtual void SolidPolygon(int n, lmUPoint points[], wxColour color) = 0;
    virtual void SolidCircle(lmLUnits x, lmLUnits y, lmLUnits radius) = 0;

    //settings: line width, colors, fonts, ...
    virtual void SetFont(wxFont& font) = 0;
    virtual void SetLogicalFunction(int function) = 0;

    virtual wxColour GetFillColor() = 0;
    virtual void SetFillColor(wxColour color) = 0;
    virtual wxColour GetLineColor() = 0;
    virtual void SetLineColor(wxColour color) = 0;
    virtual lmLUnits GetLineWidth() { return m_uLineWidth; }
    virtual void SetLineWidth(lmLUnits uWidth) = 0;
    virtual void SetPen(wxColour color, lmLUnits uWidth) = 0;


    //text
    virtual void DrawText(const wxString& text, lmLUnits x, lmLUnits y) = 0;
    virtual void SetTextForeground(const wxColour& colour) = 0;
    virtual void SetTextBackground(const wxColour& colour) = 0;
    virtual void GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h) = 0;

    // units conversion
    virtual lmLUnits DeviceToLogicalX(lmPixels x) = 0;
    virtual lmLUnits DeviceToLogicalY(lmPixels y) = 0;
    virtual lmPixels LogicalToDeviceX(lmLUnits x) = 0;
    virtual lmPixels LogicalToDeviceY(lmLUnits y) = 0;

protected:
    wxDC*       m_pDC;              // the DC to use
    lmLUnits    m_uLineWidth;       // pen width

};


// DirectDrawer is just a drawer that paints directly on the DC
class lmDirectDrawer : public lmDrawer
{
public:
    lmDirectDrawer(wxDC* pDC) : lmDrawer(pDC) {};
    ~lmDirectDrawer() {};

    // Aliased shapes, even when anti-alising is supported.
    void SketchLine(lmLUnits ux1, lmLUnits uy1, lmLUnits ux2, lmLUnits uy2, wxColour color);
    void SketchRectangle(lmUPoint uPoint, lmUSize uSize, wxColour color);

    //solid shapes, anti-aliased when supported.
    void SolidPolygon(int n, lmUPoint points[], wxColour color);
    void SolidCircle(lmLUnits ux, lmLUnits uy, lmLUnits uRadius);


    //settings: line width, colors, fonts, ...
    void SetFont(wxFont& font) {m_pDC->SetFont(font); }
    void SetLogicalFunction(int function) { m_pDC->SetLogicalFunction(function); }

    wxColour GetFillColor();
    void SetFillColor(wxColour color);
    wxColour GetLineColor();
    void SetLineColor(wxColour color);
    void SetLineWidth(lmLUnits uWidth);
    void SetPen(wxColour color, lmLUnits uWidth);

    //text
    void DrawText(const wxString& text, lmLUnits ux, lmLUnits uy) {m_pDC->DrawText(text, (int)ux, (int)uy); }
    void SetTextForeground(const wxColour& colour) {m_pDC->SetTextForeground(colour); }
    void SetTextBackground(const wxColour& colour) {m_pDC->SetTextBackground(colour); }
    void GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h);

    // units conversion
    lmLUnits DeviceToLogicalX(lmPixels vx) { return m_pDC->DeviceToLogicalXRel(vx); }
    lmLUnits DeviceToLogicalY(lmPixels vy) { return m_pDC->DeviceToLogicalYRel(vy); }
    lmPixels LogicalToDeviceX(lmLUnits ux) { return m_pDC->LogicalToDeviceXRel((int)ux); }
    lmPixels LogicalToDeviceY(lmLUnits uy) { return m_pDC->LogicalToDeviceYRel((int)uy); }

private:
    //current brush/pen/color settings
    wxColour        m_textColorF;       //text foreground color;
    wxColour        m_textColorB;       //text background color;
    wxColour        m_lineColor;        //pen color
    wxColour        m_fillColor;        //brush color

};

#endif  // __DRAWER_H__

