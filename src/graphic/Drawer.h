//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"      // lmLUnits, lmPixels


class lmDrawer
{
public:
    lmDrawer(wxDC* pDC);
    virtual ~lmDrawer() {}

    //draw shapes
    virtual void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) = 0;
    virtual void DrawRectangle(wxPoint point, wxSize size) = 0;
    virtual void DrawRectangle(wxCoord left, wxCoord top, wxCoord width, wxCoord height) = 0;
    virtual void DrawCircle(wxCoord x, wxCoord y, wxCoord radius) = 0;
    virtual void DrawCircle(const wxPoint& pt, wxCoord radius) = 0;
    virtual void DrawPolygon(int n, wxPoint points[]) = 0;

    //brushes, colors, fonts, ...
    virtual void SetBrush(wxBrush brush) = 0;
    virtual void SetFont(wxFont& font) = 0;
    virtual void SetPen(wxPen& pen) = 0;
    virtual const wxPen& GetPen() const = 0;
    virtual void SetLogicalFunction(int function) = 0;

    //text
    virtual void DrawText(const wxString& text, wxCoord x, wxCoord y) = 0;
    virtual void SetTextForeground(const wxColour& colour) = 0;
    virtual void SetTextBackground(const wxColour& colour) = 0;
    virtual void GetTextExtent(const wxString& string, wxCoord* w, wxCoord* h) = 0; 

    // units conversion
    virtual lmLUnits DeviceToLogicalX(lmPixels x) = 0;
    virtual lmLUnits DeviceToLogicalY(lmPixels y) = 0;
    virtual lmPixels LogicalToDeviceX(lmLUnits x) = 0;
    virtual lmPixels LogicalToDeviceY(lmLUnits y) = 0;

protected:
    wxDC*       m_pDC;              // the DC to use

};


// DirectDrawer is just a drawer that paints directly on the DC
class lmDirectDrawer : public lmDrawer
{
public:
    lmDirectDrawer(wxDC* pDC) : lmDrawer(pDC) {};
    ~lmDirectDrawer() {};

    //draw shapes
    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
            { m_pDC->DrawLine(x1, y1, x2, y2); }
    void DrawRectangle(wxPoint point, wxSize size) { m_pDC->DrawRectangle(point, size); }
    void DrawRectangle(wxCoord left, wxCoord top, wxCoord width, wxCoord height)
            { m_pDC->DrawRectangle(left, top, width, height); }
    void DrawCircle(wxCoord x, wxCoord y, wxCoord radius) { m_pDC->DrawCircle(x, y, radius); }
    void DrawCircle(const wxPoint& pt, wxCoord radius) { m_pDC->DrawCircle(pt, radius); }
    void DrawPolygon(int n, wxPoint points[]) { m_pDC->DrawPolygon(n, points); }

    //brushes, colors, fonts, ...
    void SetBrush(wxBrush brush) { m_pDC->SetBrush(brush); }
    void SetFont(wxFont& font) {m_pDC->SetFont(font); }
    void SetPen(wxPen& pen) { m_pDC->SetPen(pen); }
    const wxPen& GetPen() const { return m_pDC->GetPen(); }
    void SetLogicalFunction(int function) { m_pDC->SetLogicalFunction(function); }

    //text
    void DrawText(const wxString& text, wxCoord x, wxCoord y) {m_pDC->DrawText(text, x, y); }
    void SetTextForeground(const wxColour& colour) {m_pDC->SetTextForeground(colour); }
    void SetTextBackground(const wxColour& colour) {m_pDC->SetTextBackground(colour); }
    void GetTextExtent(const wxString& string, wxCoord* w, wxCoord* h) 
            { m_pDC->GetTextExtent(string, w, h); }

    // units conversion
    lmLUnits DeviceToLogicalX(lmPixels x) { return m_pDC->DeviceToLogicalXRel(x); }
    lmLUnits DeviceToLogicalY(lmPixels y) { return m_pDC->DeviceToLogicalYRel(y); }
    lmPixels LogicalToDeviceX(lmLUnits x) { return m_pDC->LogicalToDeviceXRel(x); }
    lmPixels LogicalToDeviceY(lmLUnits y) { return m_pDC->LogicalToDeviceYRel(y); }

private:

};

#endif  // __DRAWER_H__

