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
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef _PAPER_H        //to avoid nested includes
#define _PAPER_H


#include "wx/dc.h"
#include "../score/defs.h"
#include "Page.h"
#include "FontManager.h"
#include "../graphic/Drawer.h"


class lmPaper
{
public:
    lmPaper();
    ~lmPaper();

    //Drawer to use
    void SetDrawer(lmDrawer* pDrawer);

    // page cursors positioning
    void NewLine(lmLUnits nSpace);
    void RestartPageCursors();
    lmLUnits GetCursorX() { return m_xCursor; }
    lmLUnits GetCursorY() { return m_yCursor; }
    void SetCursorX(lmLUnits rValor) { m_xCursor = rValor; }
    void SetCursorY(lmLUnits rValor) { m_yCursor = rValor; }
    void IncrementCursorX(lmLUnits rValor) { m_xCursor += rValor; }
    void IncrementCursorY(lmLUnits rValor) { m_yCursor += rValor; }

    // page size and margings
    lmLUnits GetPageTopMargin() { return m_Page.TopMargin(); }
    lmLUnits GetPageLeftMargin() { return m_Page.LeftMargin(); }
    lmLUnits GetPageRightMargin() { return m_Page.RightMargin(); }
    wxSize& GetPaperSize();
    lmLUnits GetMaximumY() {return m_Page.GetUsableHeight() + m_Page.TopMargin(); }

    void SetPageTopMargin(lmLUnits nValue) { m_Page.SetTopMargin(nValue); }
    void SetPageLeftMargin(lmLUnits nValue) { m_Page.SetLeftMargin(nValue); }
    void SetPageRightMargin(lmLUnits nValue) { m_Page.SetRightMargin(nValue); }
    void SetPageSize(lmLUnits nWidth, lmLUnits nHeight) { m_Page.SetPageSize(nWidth, nHeight); }

    lmLUnits GetRightMarginXPos();
    lmLUnits GetLeftMarginXPos();

    // unit conversion
    lmLUnits DeviceToLogicalX(lmPixels x) { return m_pDrawer->DeviceToLogicalX(x); }
    lmLUnits DeviceToLogicalY(lmPixels y) { return m_pDrawer->DeviceToLogicalY(y); }

    lmPixels LogicalToDeviceX(lmLUnits x) { return m_pDrawer->LogicalToDeviceX(x); }
    lmPixels LogicalToDeviceY(lmLUnits y) { return m_pDrawer->LogicalToDeviceY(y); }

    // fonts
    wxFont* GetFont(int nPointSize,
                    wxString sFontName = _T("LeMus Notas"),
                    int nFamily = wxDEFAULT,    //wxDEFAULT, wxDECORATIVE, wxROMAN, wxSCRIPT, wxSWISS, wxMODERN
                    int nStyle = wxNORMAL,        //wxNORMAL, wxSLANT or wxITALIC
                    int nWeight = wxNORMAL,        //wxNORMAL, wxLIGHT or wxBOLD
                    bool fUnderline = false);


        //methods to encapsulate the DC

    //draw shapes
    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
            { m_pDrawer->DrawLine(x1, y1, x2, y2); }
    void DrawRectangle(wxPoint point, wxSize size) { m_pDrawer->DrawRectangle(point, size); }
    void DrawRectangle(wxCoord left, wxCoord top, wxCoord width, wxCoord height)
            { m_pDrawer->DrawRectangle(left, top, width, height); }
    void DrawCircle(wxCoord x, wxCoord y, wxCoord radius) { m_pDrawer->DrawCircle(x, y, radius); }
    void DrawCircle(const lmUPoint& pt, wxCoord radius) { m_pDrawer->DrawCircle(pt, radius); }

    void DrawRectangle(lmUPoint point, wxSize size) { m_pDrawer->DrawRectangle(point, size); }
    void RenderCircle(const lmUPoint& pt, lmLUnits radius) { m_pDrawer->RenderCircle(pt, radius); }
    void RenderPolygon(int n, lmUPoint points[]) { m_pDrawer->RenderPolygon(n, points); }
    void RenderLine(lmLUnits x1, lmLUnits y1, lmLUnits x2, lmLUnits y2,
                  lmLUnits width, lmELineEdges nEdge, wxColour color)
        { m_pDrawer->RenderLine(x1, y1, x2, y2, width, nEdge, color); }

    //brushes, colors, fonts, ...
    void SetBrush(wxBrush brush) { m_pDrawer->SetBrush(brush); }
    void SetFont(wxFont& font) {m_pDrawer->SetFont(font); }
    void SetPen(wxPen& pen) { m_pDrawer->SetPen(pen); }
    const wxPen& GetPen() const { return m_pDrawer->GetPen(); }
    void SetLogicalFunction(int function) { m_pDrawer->SetLogicalFunction(function); }

    //text
    void DrawText(const wxString& text, wxCoord x, wxCoord y) {m_pDrawer->DrawText(text, x, y); }
    void SetTextForeground(const wxColour& colour) {m_pDrawer->SetTextForeground(colour); }
    void SetTextBackground(const wxColour& colour) {m_pDrawer->SetTextBackground(colour); }
    void GetTextExtent(const wxString& string, lmLUnits* w, lmLUnits* h) 
            { m_pDrawer->GetTextExtent(string, w, h); }




private:
    lmDrawer*   m_pDrawer;
    lmPage      m_Page;             // page layout settings

    // page cursors
    lmLUnits   m_xCursor, m_yCursor;       // current default drawing position. Logical units
                                            // relative to origin of paper

    // miscelaneous 
    lmFontManager   m_fontManager;          // font manager for this paper

};
    
#endif    // _PAPER_H