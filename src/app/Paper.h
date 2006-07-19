// RCS-ID: $Id: Paper.h,v 1.3 2006/02/23 19:17:12 cecilios Exp $
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

class lmScore;

// offscreen bitmaps will be maintained in a list structure. Let's declare it
WX_DECLARE_LIST(wxBitmap, BitmapList);


class lmPaper
{
public:
    lmPaper();
    ~lmPaper();

    // drawing control
    wxDC* GetDC() const { return m_pDC; }
    void SetDC(wxDC* pDC) { m_pDC = pDC; }

    void Prepare(lmScore* pScore, lmLUnits paperWidth, lmLUnits paperHeight, double rScale);
    void NewPage();
    void NewLine(lmLUnits nSpace);

    void ForceRedraw() { m_fRedraw = true; }
    void RestartPageCursors();

    int GetNumPages() { return m_numPages; }

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
    void SetPageSize(lmLUnits nWidth, lmLUnits nHeight)
            {
                m_Page.SetPageSize(nWidth, nHeight);
                m_fRedraw = true;       //force to redraw the score
            }

    //public access to the offscreen bitmap of page nPage
    wxBitmap* GetOffscreenBitmap(int nPage = 0);

    // unit conversion
    lmLUnits DeviceToLogicalX(lmPixels x) { return m_pDC->DeviceToLogicalXRel(x); }
    lmLUnits DeviceToLogicalY(lmPixels y) { return m_pDC->DeviceToLogicalYRel(y); }

    lmPixels LogicalToDeviceX(lmLUnits x) { return m_pDC->LogicalToDeviceXRel(x); }
    lmPixels LogicalToDeviceY(lmLUnits y) { return m_pDC->LogicalToDeviceYRel(y); }
    //wxPoint LogicalToDevice(wxPoint pt) {
    //        return wxPoint(m_pDC->LogicalToDeviceX(pt.x), m_pDC->LogicalToDeviceY(pt.y) );
    //    }

    lmLUnits GetRightMarginXPos();
    lmLUnits GetLeftMarginXPos();

    double GetScale() { return m_rScale; }

    wxFont* GetFont(int nPointSize,
                    wxString sFontName = _T("LeMus Notas"),
                    int nFamily = wxDEFAULT,    //wxDEFAULT, wxDECORATIVE, wxROMAN, wxSCRIPT, wxSWISS, wxMODERN
                    int nStyle = wxNORMAL,        //wxNORMAL, wxSLANT or wxITALIC
                    int nWeight = wxNORMAL,        //wxNORMAL, wxLIGHT or wxBOLD
                    bool fUnderline = false);


private:
    void DeleteBitmaps();
    wxBitmap* GetPageBitmap(int nPage);


    wxDC*       m_pDC;              // the DC to use
    lmPage      m_Page;             // page layout settings

    // offscreen bitmaps management
    BitmapList  m_cBitmaps;            // list of bitmaps for offscreen painting
    lmPixels    m_xBitmapSize, m_yBitmapSize;    // size of bitmaps in pixels
    lmPixels    m_xPageSize, m_yPageSize;        // size of page in pixels
    int         m_numPages;            // num pages that current score has
    bool        m_fRedraw;             // force to redraw the score

    lmScore*    m_pScore;              // the score to draw in this paper
    long        m_nLastScoreID;        // the ID of the last rendered score
    double      m_rScale;              // drawing scale

    // page cursors
    lmLUnits   m_xCursor, m_yCursor;       // current default drawing position. Logical units
                                            // relative to origin of paper

    // miscelaneous 
    lmFontManager   m_fontManager;          // font manager for this paper

};
    
#endif    // _PAPER_H