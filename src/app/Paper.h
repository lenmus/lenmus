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
/*! @file Paper.h
    @brief Header file for class lmPaper
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef _PAPER_H        //to avoid nested includes
#define _PAPER_H


#include "wx/dc.h"
#include "../score/defs.h"
#include "Page.h"
#include "FontManager.h"


class lmPaper
{
public:
    lmPaper();
    ~lmPaper();

    // drawing control
    wxDC* GetDC() const { return m_pDC; }
    void SetDC(wxDC* pDC) { m_pDC = pDC; }

    void NewLine(lmLUnits nSpace);

    void RestartPageCursors();

    // page cursor position
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

    // unit conversion
    lmLUnits DeviceToLogicalX(lmPixels x) { return m_pDC->DeviceToLogicalXRel(x); }
    lmLUnits DeviceToLogicalY(lmPixels y) { return m_pDC->DeviceToLogicalYRel(y); }

    lmPixels LogicalToDeviceX(lmLUnits x) { return m_pDC->LogicalToDeviceXRel(x); }
    lmPixels LogicalToDeviceY(lmLUnits y) { return m_pDC->LogicalToDeviceYRel(y); }

    lmLUnits GetRightMarginXPos();
    lmLUnits GetLeftMarginXPos();

    wxFont* GetFont(int nPointSize,
                    wxString sFontName = _T("LeMus Notas"),
                    int nFamily = wxDEFAULT,    //wxDEFAULT, wxDECORATIVE, wxROMAN, wxSCRIPT, wxSWISS, wxMODERN
                    int nStyle = wxNORMAL,        //wxNORMAL, wxSLANT or wxITALIC
                    int nWeight = wxNORMAL,        //wxNORMAL, wxLIGHT or wxBOLD
                    bool fUnderline = false);


        //methods to encapsulate the DC

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




private:
    wxDC*       m_pDC;              // the DC to use
    lmPage      m_Page;             // page layout settings

    // page cursors
    lmLUnits   m_xCursor, m_yCursor;       // current default drawing position. Logical units
                                            // relative to origin of paper

    // miscelaneous 
    lmFontManager   m_fontManager;          // font manager for this paper

};
    
#endif    // _PAPER_H