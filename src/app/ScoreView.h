//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_SCOREVIEW_H__        //to avoid nested includes
#define __LM_SCOREVIEW_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScoreView.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include "wx/docview.h"

#include "Paper.h"
#include "../sound/SoundEvents.h"
#include "../graphic/GraphicManager.h"
#include "Printout.h"

class lmComponentObj;
class lmEditFrame;
class lmScoreCanvas;
class lmScoreDocument;
class lmRuler;
class lmCaret;
class lmMainFrame;
class lmBoxSystem;
class lmShapeStaff;
class lmBoxSliceVStaff;
class lmGMObject;


//Abstract class. All views must derive from it
class lmView : public wxView
{
public:
	lmView() : wxView() {}
	virtual ~lmView() {}

	virtual lmController* GetController()=0;
    inline lmScoreDocument* GetDocument() { return m_pDoc; }


protected:
    lmScoreDocument*    m_pDoc;             //the MVC document (M)

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
    void OnMouseEvent(wxMouseEvent& event, wxDC* pDC);
    void OnMouseWheel(wxMouseEvent& event);
    void OnScroll(wxScrollEvent& event);

    //scrolling and painting
    void AdjustScrollbars();
    void ResizeControls();
    void GetViewStart (int* x, int* y) const;
    void GetScrollPixelsPerUnit (int* x_unit, int* y_unit) const;
    void RepaintScoreRectangle(wxDC* pDC, wxRect& repaintRect, int nRepaintOptions=0);
	wxPoint GetDCOriginForPage(int nNumPage);


    // print/preview/export as image
    void GetPageInfo(int* pMinPage, int* pMaxPage, int* pSelPageFrom, int* pSelPageTo);
    void DrawPage(wxDC* pDC, int nPage, lmPrintout* pPrintout);
    void SaveAsImage(wxString& sFilename, wxString& sExt, int nImgType);

    //access to components
    inline lmController* GetController() { return (lmController*)m_pCanvas; }
    inline lmScoreDocument* GetDocument() { return m_pDoc; }
	inline lmBoxScore* GetBoxScore() { return m_graphMngr.GetBoxScore(); }


    // call backs: sound related methods
    void OnVisualHighlight(lmScoreHighlightEvent& event);

	//caret management
	void CaretRight(bool fNextObject = true);
	void CaretLeft(bool fPrevObject = true);
	void CaretUp();
	void CaretDown();
    lmVStaffCursor* GetVCursor();

	void LogicalToDevice(lmUPoint& posLogical, lmDPoint& posDevice);


private:

	//mouse behaviour
	lmLUnits GetMouseTolerance();

	//dragging on canvas with left button: selection
	void OnCanvasBeginDragLeft(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnCanvasContinueDragLeft(bool fDraw, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnCanvasEndDragLeft(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void DrawSelectionArea(wxDC& dc, lmPixels vX1, lmPixels vY1, lmPixels vX2, lmPixels vY2);

	//dragging on canvas with right button
	void OnCanvasBeginDragRight(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnCanvasContinueDragRight(bool fDraw, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnCanvasEndDragRight(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);

	//dragging object with left button
	void OnObjectBeginDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							   lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);
	void OnObjectContinueDragLeft(wxMouseEvent& event, wxDC* pDC, bool fDraw,
								  lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
								  lmUPoint uPagePos, int nKeys);
	void OnObjectEndDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							 lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);

	//dragging object with right button
	void OnObjectBeginDragRight(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							   lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);
	void OnObjectContinueDragRight(wxMouseEvent& event, wxDC* pDC, bool fDraw,
								  lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
								  lmUPoint uPagePos, int nKeys);
	void OnObjectEndDragRight(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
							 lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys);

	//non-dragging events: click on an object
	void OnLeftClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnLeftDoubleClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnRightClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnRightDoubleClickOnObject(lmGMObject* pGMO, lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);

	//non-dragging events: click on canvas
	void OnRightClickOnCanvas(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);
	void OnLeftClickOnCanvas(lmDPoint vCanvasPos, lmUPoint uPagePos, int nKeys);

	// units conversion
	lmDPoint GetScrollOffset();
    lmDPoint GetPageOffset(int nNumPage);
    void DeviceToLogical(lmDPoint& posDevice, lmUPoint& posLogical,
                         lmDPoint* pPagePosD,
						 lmDPoint* pPaperOrgD, lmDPoint* pOffsetD,
						 int* pNumPage, bool* pfInInterpageGap);

    // Auxiliary for scrolling
    int CalcScrollInc(wxScrollEvent& event);
    void DoScroll(int orientation, int nScrollSteps);

	//caret management
    void SetInitialCaretPosition();
    void UpdateCaret();
    void HideCaret();
    void ShowCaret();
	void MoveCaretNearTo(lmUPoint uPos, lmVStaff* pVStaff, int nStaff, int nMeasure);
    void MoveCaretToObject(lmStaffObj* pSO);

	//Mouse commands
	void OnClickOnStaff(lmBoxSystem* pBS, lmShapeStaff* pSS, lmBoxSliceVStaff* pBSV,
						lmUPoint uPos);
    void OnClickOnObject(lmGMObject* pGMO);

    //dealing with selections
    void DeselectAllGMObjects(bool fRedraw = false);
    void SelectGMObject(lmGMObject* pGMO, bool fRedraw = false);
    void SelectGMObjectsInArea(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
                               lmLUnits uYMin, lmLUnits uYMax, bool fRedraw = false);




	//-- variables ---

    // parents, managers and related
    lmScoreCanvas*      m_pCanvas;          //the MVC controller (C) and the window for rendering the view
    lmGraphicManager    m_graphMngr;        //rederization manager
    lmEditFrame*        m_pFrame;           //the frame for the view
    lmMainFrame*        m_pMainFrame;       //for accesing StatusBar


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
    int             m_nDragState;
    lmUPoint        m_uDragStartPos;
    lmDPoint        m_vDragHotSpot;			//absolute point (pixels)
    lmUPoint        m_uHotSpotShift;		//distance from shape origin
    wxDragImage*    m_pDragImage;
	lmGMObject*		m_pDraggedGMO;			//GMObject being dragged

    //cursor
    lmCaret*            m_pCaret;
    lmScoreCursor*      m_pScoreCursor;
	bool				m_fCaretInit;
    lmStaffObj*         m_pCursorSO;       //for visual feedback
    int                 m_nCursorStaff;


    DECLARE_EVENT_TABLE()
};


#endif    // __LM_SCOREVIEW_H__
