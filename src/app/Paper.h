//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

    //drawer
    void SetDrawer(lmDrawer* pDrawer);
    inline lmDrawer* GetDrawer() { return m_pDrawer; }
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
    inline void SketchLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                    wxColour color, int style = wxSOLID)
						{ m_pDrawer->SketchLine(x1, y1, x2, y2, color, style); }
    inline void SketchRectangle(lmUPoint point, lmUSize uSize, wxColour color)
						{ m_pDrawer->SketchRectangle(point, uSize, color); }

    // Aliased shapes, even when anti-alising is supported.
    inline void SolidCircle(lmLUnits x, lmLUnits y, lmLUnits radius)
            { m_pDrawer->SolidCircle(x, y, radius); }
    inline void SolidPolygon(int n, lmUPoint points[], wxColor color)
            { m_pDrawer->SolidPolygon(n, points, color); }
    inline void SolidLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                  lmLUnits width, lmELineEdges nEdge, wxColour color)
            { m_pDrawer->SolidLine(x1, y1, x2, y2, width, nEdge, color); }
    inline void SolidShape(lmShape* pShape, wxColor color)
            { m_pDrawer->SolidShape(pShape, color); }
    inline void DecoratedLine(lmUPoint& start, lmUPoint& end, lmLUnits width,
                              lmELineCap nStartCap, lmELineCap nEndCap, wxColor color)
            { m_pDrawer->DecoratedLine(start, end, width, nStartCap, nEndCap, color); }

    //settings: line width, colors, fonts, ...
    inline void SetFont(wxFont& font) { m_pDrawer->SetFont(font); }
    inline void SetLogicalFunction(int function) { m_pDrawer->SetLogicalFunction(function); }

    inline wxColour GetFillColor() 
            { return m_pDrawer->GetFillColor(); }
    inline void SetFillColor(wxColour color)
            { m_pDrawer->SetFillColor(color); }
    inline wxColour GetLineColor() 
            { return m_pDrawer->GetLineColor(); }
    inline void SetLineColor(wxColour color)
            { m_pDrawer->SetLineColor(color); }
    inline lmLUnits GetLineWidth() 
            { return m_pDrawer->GetLineWidth(); }
    inline void SetLineWidth(lmLUnits uWidth) 
            { m_pDrawer->SetLineWidth(uWidth); }
    inline void SetPen(wxColour color, lmLUnits uWidth)
            { m_pDrawer->SetPen(color, uWidth); }

    //clipping
    inline void SetClippingRegion(const lmURect& uRect)
            { m_pDrawer->SetClippingRegion(uRect); }

    //text
    inline void DrawText(const wxString& text, lmLUnits x, lmLUnits y)
            { m_pDrawer->DrawText(text, x, y); }
    inline void SetTextForeground(const wxColour& colour)
            { m_pDrawer->SetTextForeground(colour); }
    inline void SetTextBackground(const wxColour& colour) 
            { m_pDrawer->SetTextBackground(colour); }
    inline void GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h)
            { m_pDrawer->GetTextExtent(string, w, h); }

    //text (FreeType with AGG rederization)
    inline bool FtLoadFont(wxString sFontName) 
            { return false; }
    inline int FtDrawChar(unsigned int nChar) 
            { return m_pDrawer->FtDrawChar(nChar); }
    inline int FtDrawText(wxString& sText) 
            { return m_pDrawer->FtDrawText(sText); }
    inline int FtDrawText(unsigned int* pText, size_t nLength)
            { return m_pDrawer->FtDrawText(pText, nLength); }
    inline void FtSetFontSize(double rPoints) 
            { m_pDrawer->FtSetFontSize(rPoints); }
    inline void FtSetFontHeight(double rPoints) 
            { m_pDrawer->FtSetFontHeight(rPoints); }
    inline void FtSetFontWidth(double rPoints) 
            { m_pDrawer->FtSetFontWidth(rPoints); }
    inline void FtSetTextPosition(lmLUnits uxPos, lmLUnits uyPos) 
            { m_pDrawer->FtSetTextPosition(uxPos, uyPos); }
    inline void FtGetTextExtent(const wxString& sText, lmLUnits* pWidth, lmLUnits* pHeight,
                         lmLUnits* pDescender = NULL, lmLUnits* pAscender = NULL) 
            { m_pDrawer->FtGetTextExtent(sText, pWidth, pHeight, pDescender, pAscender); }


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
