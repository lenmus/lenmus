//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_DRAWER_H__        //to avoid nested includes
#define __LM_DRAWER_H__

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

class lmShape;


class lmDrawer
{
public:
    virtual ~lmDrawer() {}

    virtual bool IsDirectDrawer() = 0;

    // Aliased shapes, even when anti-alising is supported.
    virtual void SketchLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
							wxColour color, int style = wxSOLID) = 0;
    virtual void SketchRectangle(lmUPoint uPoint, lmUSize uSize, wxColour color) = 0;

    //solid shapes, anti-aliased when supported.
    virtual void SolidLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                          lmLUnits width, lmELineEdges nEdge, wxColour color);
    virtual void SolidPolygon(int n, lmUPoint points[], wxColour color) = 0;
    virtual void SolidCircle(lmLUnits x, lmLUnits y, lmLUnits radius) = 0;
    virtual void SolidShape(lmShape* pShape, wxColor color) = 0;
    virtual void DecoratedLine(lmUPoint& start, lmUPoint& end, lmLUnits width,
                               lmELineCap nStartCap, lmELineCap nEndCap, wxColor color)= 0;

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


    //text (platform native rederization)
    virtual void DrawText(const wxString& text, lmLUnits x, lmLUnits y) = 0;
    virtual void SetTextForeground(const wxColour& colour) = 0;
    virtual void SetTextBackground(const wxColour& colour) = 0;
    virtual void GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h) = 0;

    //text (FreeType with AGG rederization)
    //default implementation does nothing and returns error value
    virtual bool FtLoadFont(wxString sFontName) { return false; }
    virtual int FtDrawChar(unsigned int nChar) { return 0; }
    virtual int FtDrawText(wxString& sText) { return 0; }
    virtual int FtDrawText(unsigned int* pText, size_t nLength) { return 0; }
    virtual void FtSetFontSize(double rPoints) {}
    virtual void FtSetFontHeight(double rPoints) {}
    virtual void FtSetFontWidth(double rPoints) {}
    virtual void FtSetTextPosition(lmLUnits uxPos, lmLUnits uyPos) {}
    virtual void FtGetTextExtent(const wxString& sText, lmLUnits* pWidth, lmLUnits* pHeight,
                         lmLUnits* pDescender = NULL, lmLUnits* pAscender = NULL) {}

    // units conversion
    virtual lmLUnits DeviceToLogicalX(lmPixels x) = 0;
    virtual lmLUnits DeviceToLogicalY(lmPixels y) = 0;
    virtual lmPixels LogicalToDeviceX(lmLUnits x) = 0;
    virtual lmPixels LogicalToDeviceY(lmLUnits y) = 0;

protected:
    lmDrawer(wxDC* pDC);

    wxDC*       m_pDC;              // the DC to use
    lmLUnits    m_uLineWidth;       // pen width

};


// DirectDrawer is just a drawer that paints directly on the DC
class lmDirectDrawer : public lmDrawer
{
public:
    lmDirectDrawer(wxDC* pDC) : lmDrawer(pDC) {};
    ~lmDirectDrawer() {};

    bool IsDirectDrawer() { return true; }

    // Aliased shapes, even when anti-alising is supported.
    void SketchLine(lmLUnits ux1, lmLUnits uy1, lmLUnits ux2, lmLUnits uy2,
					wxColour color, int style = wxSOLID);
    void SketchRectangle(lmUPoint uPoint, lmUSize uSize, wxColour color);

    //solid shapes, anti-aliased when supported.
    void SolidPolygon(int n, lmUPoint points[], wxColour color);
    void SolidCircle(lmLUnits ux, lmLUnits uy, lmLUnits uRadius);
    void SolidShape(lmShape* pShape, wxColor color) {};     //not supported
    inline void DecoratedLine(lmUPoint& start, lmUPoint& end, lmLUnits width,
                              lmELineCap nStartCap, lmELineCap nEndCap, wxColor color)
                { SolidLine(start.x, start.y, end.x, end.y, width, lm_eEdgeNormal, color); }     //not supported


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

#endif  // __LM_DRAWER_H__

