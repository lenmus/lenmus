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

#ifndef __LM_PAPER_H__        //to avoid nested includes
#define __LM_PAPER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Paper.cpp"
#endif


#include <wx/dc.h>

#include "FontManager.h"
#include "../score/defs.h"
#include "../graphic/Drawer.h"

class lmShape;


class lmPaper
{
public:
    lmPaper();
    ~lmPaper();

    //settings
    void SetDrawer(lmDrawer* pDrawer);
    bool IsDirectDrawer();

    // page cursors positioning
    inline lmLUnits GetCursorX() { return m_uxCursor; }
    inline lmLUnits GetCursorY() { return m_uyCursor; }

    inline void SetCursorX(lmLUnits uValue) { m_uxCursor = uValue; }
    inline void SetCursorY(lmLUnits uValue) { m_uyCursor = uValue; }
    inline void SetCursor(lmUPoint uPos) { m_uxCursor = uPos.x; m_uyCursor = uPos.y; }
    inline void SetCursor(lmLUnits uxPos, lmLUnits uyPos)
                    { m_uxCursor = uxPos; m_uyCursor = uyPos; }

    inline void IncrementCursorX(lmLUnits uValue) { m_uxCursor += uValue; }
    inline void IncrementCursorY(lmLUnits uValue) { m_uyCursor += uValue; }

    // unit conversion
    inline lmLUnits DeviceToLogicalX(lmPixels x) { return m_pDrawer->DeviceToLogicalX(x); }
    inline lmLUnits DeviceToLogicalY(lmPixels y) { return m_pDrawer->DeviceToLogicalY(y); }

    inline lmPixels LogicalToDeviceX(lmLUnits x) { return m_pDrawer->LogicalToDeviceX(x); }
    inline lmPixels LogicalToDeviceY(lmLUnits y) { return m_pDrawer->LogicalToDeviceY(y); }

    // fonts
    wxFont* GetFont(int nPointSize,
                    wxString sFontName = _T("LeMus Notas"),
                    int nFamily = wxDEFAULT,    //wxDEFAULT, wxDECORATIVE, wxROMAN, wxSCRIPT, wxSWISS, wxMODERN
                    int nStyle = wxNORMAL,        //wxNORMAL, wxSLANT or wxITALIC
                    int nWeight = wxNORMAL,        //wxNORMAL, wxLIGHT or wxBOLD
                    bool fUnderline = false);


        //methods to encapsulate the DC

    //solid shapes, anti-aliased when supported.
    void SketchLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2, wxColour color)
            { m_pDrawer->SketchLine(x1, y1, x2, y2, color); }
    void SketchRectangle(lmUPoint point, lmUSize uSize, wxColour color)
            { m_pDrawer->SketchRectangle(point, uSize, color); }

    // Aliased shapes, even when anti-alising is supported.
    void SolidCircle(lmLUnits x, lmLUnits y, lmLUnits radius)
            { m_pDrawer->SolidCircle(x, y, radius); }
    void SolidPolygon(int n, lmUPoint points[], wxColor color)
            { m_pDrawer->SolidPolygon(n, points, color); }
    void SolidLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                  lmLUnits width, lmELineEdges nEdge, wxColour color)
            { m_pDrawer->SolidLine(x1, y1, x2, y2, width, nEdge, color); }
    void SolidShape(lmShape* pShape, wxColor color)
            { m_pDrawer->SolidShape(pShape, color); }

    //settings: line width, colors, fonts, ...
    void SetFont(wxFont& font) {m_pDrawer->SetFont(font); }
    void SetLogicalFunction(int function) { m_pDrawer->SetLogicalFunction(function); }

    wxColour GetFillColor() {return m_pDrawer->GetFillColor(); };
    void SetFillColor(wxColour color) {m_pDrawer->SetFillColor(color); };
    wxColour GetLineColor() {return m_pDrawer->GetLineColor(); };
    void SetLineColor(wxColour color) {m_pDrawer->SetLineColor(color); };
    lmLUnits GetLineWidth() {return m_pDrawer->GetLineWidth(); };
    void SetLineWidth(lmLUnits uWidth) {m_pDrawer->SetLineWidth(uWidth); };
    void SetPen(wxColour color, lmLUnits uWidth) {m_pDrawer->SetPen(color, uWidth); };

    //text
    void DrawText(const wxString& text, lmLUnits x, lmLUnits y) {m_pDrawer->DrawText(text, x, y); }
    void SetTextForeground(const wxColour& colour) {m_pDrawer->SetTextForeground(colour); }
    void SetTextBackground(const wxColour& colour) {m_pDrawer->SetTextBackground(colour); }
    void GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h)
            { m_pDrawer->GetTextExtent(string, w, h); }



private:
    //device context (DC)
    lmDrawer*       m_pDrawer;

    //paper cursors
    lmLUnits   m_uxCursor, m_uyCursor;  //current default drawing position. Logical units
                                        //    relative to origin of paper
    //font manager
    lmFontManager   m_fontManager;      // font manager for this paper

};

#endif    // __LM_PAPER_H__
