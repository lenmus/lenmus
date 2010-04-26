//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_SCOREVIEW_H__        //to avoid nested includes
#define __LM_SCOREVIEW_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScoreView.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if wxUSE_GENERIC_DRAGIMAGE
#include <wx/generic/dragimgg.h>
#define wxDragImage wxGenericDragImage
#else
#include <wx/dragimag.h>
#endif

#include <wx/docview.h>

#include "Paper.h"
#include "../sound/SoundEvents.h"
#include "../graphic/GraphicManager.h"
#include "Printout.h"

class lmComponentObj;
class lmEditFrame;
class lmScoreCanvas;
class lmDocument;
class lmRuler;
class lmCaret;
class lmMainFrame;
class lmBoxSystem;
class lmBoxSliceInstr;
class lmGMObject;
class lmGMSelection;
class lmScoreProcessor;
class lmToolBoxConfiguration;


//Abstract class. All views must derive from it
class lmView : public wxView
{
    DECLARE_ABSTRACT_CLASS(lmView)

public:
	virtual ~lmView() {}

	virtual lmController* GetController()=0;
    inline lmDocument* GetDocument() { return m_pDoc; }
    virtual void OnDraw(wxDC* dc)=0;


protected:
    lmView() : wxView() {}
    lmDocument*    m_pDoc;             //the MVC document (M)

};




class lmScoreView : public lmView
{
   DECLARE_DYNAMIC_CLASS(lmScoreView)

public:
    lmScoreView();
    ~lmScoreView();

	//overrides of virtual methods in wxView
    bool OnClose(bool deleteWindow = true);
    void OnUpdate(wxView* sender, wxObject* hint);
    bool OnCreate(wxDocument* doc, long WXUNUSED(flags));
    void OnDraw(wxDC* dc);
    void OnChangeFilename();
    void OnSetFocus(wxFocusEvent& WXUNUSED(event));
    void OnKillFocus(wxFocusEvent& WXUNUSED(event));

    // options
    void SetScale(double rScale);
    void SetScaleFitWidth();
    void SetScaleFitFull();
    double GetScale() { return m_rScale / lmSCALE; }
    void SetRulersVisible(bool fVisible);

    // debug options
    void DumpBitmaps();

	//methods for dealing with user interaction
    //void OnMouseEvent(wxMouseEvent& event, wxDC* pDC);
    void OnMouseWheel(wxMouseEvent& event);
    void OnScroll(wxScrollEvent& event);

    //scrolling and painting
    void AdjustScrollbars();
    void ResizeControls();
    void GetViewStart (int* x, int* y) const;
    void GetScrollPixelsPerUnit (int* x_unit, int* y_unit) const;
    void RepaintScoreRectangle(wxDC* pDC, wxRect& repaintRect, int nRepaintOptions=0);
    void PrepareForRepaint(wxDC* pDC, int nRepaintOptions=0);
    void TerminateRepaint(wxDC* pDC);
	wxPoint GetDCOriginForPage(int nNumPage);


    // print/preview/export as image
    void GetPageInfo(int* pMinPage, int* pMaxPage, int* pSelPageFrom, int* pSelPageTo);
    void DrawPage(wxDC* pDC, int nPage, lmPrintout* pPrintout);
    void SaveAsImage(wxString& sFilename, wxString& sExt, int nImgType);

    //access to components
    inline lmController* GetController() { return (lmController*)m_pCanvas; }
    inline lmDocument* GetDocument() { return m_pDoc; }
	inline lmBoxScore* GetBoxScore() { return m_graphMngr.GetBoxScore(); }

    //controller related
    void SaveToolBoxConfiguration();
    void RestoreToolBoxConfiguration();

    // call backs: sound related methods
    void OnVisualHighlight(lmScoreHighlightEvent& event);

	//caret management
    void CaretOn();
    void CaretOff();
	void CaretRight(bool fAlsoChordNotes = true);
	void CaretLeft(bool fAlsoChordNotes = true);
	void CaretUp();
	void CaretDown();
    void HideCaret();
    void ShowCaret();
    void DeleteCaret();
	void MoveCaretNearTo(lmUPoint uPos, lmVStaff* pVStaff, int nStaff, int nMeasure);

    //cursor management
    void MoveCursorTo(lmVStaff* pVStaff, int nStaff, int nMeasure, float rTime,
                      bool fEndOfTime);
    void MoveCursorToTime(float rTime, bool fEndOfTime);
    void MoveCursorNearTo(lmUPoint uPos, lmVStaff* pVStaff, int nStaff, int nMeasure);

    //Score cursor information
    inline lmScoreCursor* GetScoreCursor() { return m_pScoreCursor; }
    inline int GetCursorMeasure() { return m_pScoreCursor->GetSegment() + 1; }
    inline lmStaff* GetCursorStaff() { return m_pScoreCursor->GetCursorStaff(); }
	    //get object pointed by the cursor
    inline lmStaffObj* GetCursorStaffObj() { return m_pScoreCursor->GetStaffObj(); }


	void LogicalToDevice(lmUPoint& posLogical, int nPage, lmDPoint& posDevice);

	//selections
    bool SomethingSelected();
    lmGMSelection* GetSelection();
    void DeselectAllGMObjects(bool fRedraw = false);

    //on mouse event related--------------------------------------------------

    //new
    void ScaleDC(wxDC* pDC);
    void UpdateRulerMarkers(lmDPoint vPagePos);
    lmGMObject* FindShapeAt(int nNumPage, lmUPoint uPos, bool fSelectable);
    lmBox* FindBoxAt(int nNumPage, lmUPoint uPos);
    void MoveCaretToObject(lmGMObject* pGMO);

	// units conversion
	lmDPoint GetScrollOffset();
    lmDPoint GetPageOffset(int nNumPage);
    void DeviceToLogical(lmDPoint& posDevice, lmUPoint& posLogical,
                         lmDPoint* pPagePosD,
						 lmDPoint* pPaperOrgD, lmDPoint* pOffsetD,
						 int* pNumPage, bool* pfInInterpageGap);

	//mouse behaviour
	lmLUnits GetMouseTolerance();

	//dragging object with left button
	bool OnObjectBeginDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							   lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys,
                               lmGMObject* pDraggedGMO, lmDPoint vDragHotSpot,
                               lmUPoint uHotSpotShift);
	void OnObjectContinueDragLeft(wxMouseEvent& event, wxDC* pDC, bool fDraw,
								  lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
								  lmUPoint uPagePos, int nKeys);
	void OnObjectEndDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							 lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);

    //dealing with selections
	void DrawSelectionArea(wxDC& dc, lmPixels vX1, lmPixels vY1, lmPixels vX2, lmPixels vY2);
    void SelectionDone(bool fRedraw);
    void SelectGMObject(lmGMObject* pGMO, bool fRedraw = false);
    void SelectGMObjectsInArea(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
                               lmLUnits uYMin, lmLUnits uYMax, bool fRedraw = false);

    //base methods for dragging an image
    bool OnImageBeginDrag(bool fMouseTool, wxDC* pDC,
						  lmDPoint vCanvasOffset, lmUPoint uPagePos,
                          lmGMObject* pDraggedGMO, lmDPoint vDragHotSpot,
                          lmUPoint uHotSpotShift, wxBitmap* pBitmap);
    void OnImageContinueDrag(wxMouseEvent& event, bool fMouseTool, wxDC* pDC,
							 lmDPoint vCanvasOffset, lmUPoint uPagePos,
                             lmDPoint vCanvasPos);
	void OnImageEndDrag(bool fMouseTool, wxDC* pDC, lmDPoint vCanvasOffset,
                        lmUPoint uPagePos);

    //set/update information
    void UpdateNumPage(int nNumPage);

private:

    // Auxiliary for scrolling
    int CalcScrollInc(wxScrollEvent& event);
    void DoScroll(int xScrollSteps, int yScrollSteps);
    void ScrollTo(int nNumPage, lmURect visibleRect);

	//caret management
    void UpdateCaret();
    void MoveCaretToObject(lmStaffObj* pSO);

    //internal call backs
    void OnNewGraphicalModel();

	//auxiliary for dragging
	void PreparePaperForDirectDrawing(wxDC* pDC, lmDPoint vCanvasOffset);

    //housekeeping
    void ComputeVisiblePagesInfo();
    void ClearVisiblePagesInfo();


	//-- variables ---

    // parents, managers and related
    lmScoreCanvas*          m_pCanvas;          //the MVC controller (C) and the window for rendering the view
    lmGraphicManager        m_graphMngr;        //rederization manager
    lmEditFrame*            m_pFrame;           //the frame for the view
    lmMainFrame*            m_pMainFrame;       //for accesing StatusBar
    lmToolBoxConfiguration* m_pToolBoxConfig;   //to save ToolBox configuration

    // controls on the window
    lmRuler*        m_pHRuler;    //rulers
    lmRuler*        m_pVRuler;
    wxScrollBar*    m_pHScroll;    // scrollbars
    wxScrollBar*    m_pVScroll;

    // scrolling management
    int         m_xScrollPosition, m_yScrollPosition;           // current display origin (scroll units)
    int         m_pixelsPerStepX, m_pixelsPerStepY;             // pixels per scroll unit
    int         m_xMaxScrollSteps, m_yMaxScrollSteps;           // num of scroll units to scroll the full view
    int         m_xScrollStepsPerPage, m_yScrollStepsPerPage;   // scroll units to scroll a page
    int         m_thumbX, m_thumbY;                             // scrollbars thumbs size

    double      m_rScale;               // presentation scale
    lmPaper     m_Paper;                // the lmPaper object to use
    bool        m_fScaleSet;

    // visual options
    bool        m_fRulers;              // draw rulers
    wxColour    m_colorBg;              // colour for background

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
    lmDPoint        m_vDragHotSpot;			//absolute point (pixels)
    lmUPoint        m_uHotSpotShift;		//distance from shape origin
    wxDragImage*    m_pDragImage;
	lmGMObject*		m_pDraggedGMO;			//GMObject being dragged
    bool            m_fDraggingTool;        //currently dragging a mouse tool
    lmURect         m_uClippedRect;

    //cursor
    lmCaret*            m_pCaret;
    bool                m_fDisplayCaret;        //To hide caret in drag tools with mouse
    lmScoreCursor*      m_pScoreCursor;
    lmStaffObj*         m_pCursorSO;            //for visual feedback
    int                 m_nCursorStaff;

    //repaint control
    bool                m_fRelayoutPending;     //to delay relayouts

    //information about currently displayed pages
    typedef struct
    {
        int     nNumPage;       //0..n-1
        wxRect  vPageRect;      //full page rectangle (pixels, referred to view org)
        wxRect  vVisibleRect;   //displayed page rectangle (pixels, referred to view org)
        lmURect uVisibleRect;   //displayed page rectangle (LUnits, referred to page org)
        bool    fRepainted;     //during repaint, a rectangle in this page has been repainted
    }
    lmVisiblePageInfo;

    std::vector<lmVisiblePageInfo*>   m_VisiblePages;




    DECLARE_EVENT_TABLE()
};


#endif    // __LM_SCOREVIEW_H__
