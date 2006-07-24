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
/*! @file scoreview.h
    @brief Header file for class lmScoreView
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __SCOREVIEWH__        //to avoid nested includes
#define __SCOREVIEWH__

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif


#include "wx/docview.h"
#include "ScoreCanvas.h"
#include "EditFrame.h"
#include "Paper.h"
#include "../widgets/Ruler.h"
#include "FontManager.h"
#include "../sound/SoundEvents.h"
#include "../graphic/GraphicManager.h"
#include "Printout.h"

class lmScoreObj;

class lmScoreView: public wxView
{
public:
    lmScoreView();
    ~lmScoreView();

    bool OnCreate(wxDocument* doc, long WXUNUSED(flags));
    void OnDraw(wxDC *dc);
    void RepaintScoreRectangle(wxDC* pDC, wxRect& repaintRect);
    void OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint));
    bool OnClose(bool deleteWindow = TRUE);

    void OnCut(wxCommandEvent& event);

    // options

    void SetScale(double rScale);
    void SetScaleFitWidth();
    void SetScaleFitFull();
    double GetScale() { return m_rScale; }
    void SetRulersVisible(bool fVisible);

    // debug options
    void DumpBitmaps();

    //methods for dealing with user interaction
    void OnMouseEvent(wxMouseEvent& event, wxDC* pDC);
    void OnMouseWheel(wxMouseEvent& event);
    void OnScroll(wxScrollEvent& event);

    //scrolling and painting
    void AdjustScrollbars();
    void ResizeControls();
    void GetViewStart (int *x, int *y) const;
    void GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const;

    // print/preview/export as image
    void GetPageInfo(int* pMinPage, int* pMaxPage, int* pSelPageFrom, int* pSelPageTo);
    void DrawPage(wxDC* pDC, int nPage, lmPrintout* pPrintout);
    void SaveAsImage(wxString& sFilename, wxString& sExt, int nImgType);


    // sound related methods
    void PlayScore();
    void StopPlaying(bool fWait=false);
    void PausePlaying();
    void OnVisualHighlight(lmScoreHighlightEvent& event);


private:
        ////-- methods ---

    // Auxiliary for rendering the view
    void InvalidateBitmap();

    // Auxiliary for scrolling
    int CalcScrollInc(wxScrollEvent& event);
    void DoScroll(int orientation, int nScrollSteps);

        ////-- variables ---

    lmEditFrame*        m_pFrame;       // the frame for the view
    lmScoreCanvas*      m_pCanvas;      // the window for rendering the view

    // controls on the window
    lmRuler*        m_pHRuler;    //rulers
    lmRuler*        m_pVRuler;
    wxScrollBar*    m_pHScroll;    // scrollbars
    wxScrollBar*    m_pVScroll;

    // scrolling management
    int        m_xScrollPosition, m_yScrollPosition;    // current display origin (scroll units)
    int        m_pixelsPerStepX, m_pixelsPerStepY;        // pixels per scroll unit
    int        m_xMaxScrollSteps, m_yMaxScrollSteps;            // num of scroll units to scroll the full view
    int        m_xScrollStepsPerPage, m_yScrollStepsPerPage;    // scroll units to scroll a page
    int        m_thumbX, m_thumbY;                        // scrollbars thumbs size

    double          m_rScale;       // presentation scale
    lmPaper         m_Paper;        // the lmPaper object to use

    // visual options
    bool            m_fRulers;      // draw rulers

    // positioning of page images (in pixels) onto the view space. This is 
    // a virtual infinite paper on which all pages are rendered one after the other.
    lmPixels    m_xPageSizeD, m_yPageSizeD;    // pages size in pixels
    lmPixels    m_xBorder;              // margin on both sides, left and rigth, of the page
    lmPixels    m_yBorder;              // top margin before the first page
    lmPixels    m_yInterpageGap;        // gap between pages
    int         m_numPages;             // the current number of pages contained in the view space

    // scaling factors between display pixels and logical units
    double        m_xDisplayPixelsPerLU;
    double        m_yDisplayPixelsPerLU;

    // dragging control variables
    int             m_dragState;
    wxPoint         m_dragStartPosL;
    wxPoint         m_dragHotSpot;        // pixels
    wxDragImage*    m_pDragImage;
    lmScoreObj*     m_pSoDrag;            // lmScoreObj being dragged

    // managers
    lmFontManager       m_fontManager;      //font management
    lmGraphicManager    m_graphMngr;        //rederization manager


    DECLARE_DYNAMIC_CLASS(lmScoreView)
    DECLARE_EVENT_TABLE()
};



#endif    // __SCOREVIEWH__
