//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
#include "FontManager.h"
#include "Printout.h"

class lmScoreObj;
class lmEditFrame;
class lmScoreCanvas;
class lmScoreDocument;
class lmRuler;



//Abstract class. All views must derive from it
class lmView : public wxView
{
public:
	lmView() : wxView() {}
	virtual ~lmView() {}

	virtual lmController* GetController()=0;

private:

    //DECLARE_EVENT_TABLE()
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

    // options
    void SetScale(double rScale);
    void SetScaleFitWidth();
    void SetScaleFitFull();
    double GetScale() { return m_rScale / lmSCALE; }
    void SetRulersVisible(bool fVisible);

    // debug options
    void DumpBitmaps();

	//event handlers
	void OnCursorTimer(wxTimerEvent& event);

	//methods for dealing with user interaction
    void OnMouseEvent(wxMouseEvent& event, wxDC* pDC);
    void OnMouseWheel(wxMouseEvent& event);
    void OnScroll(wxScrollEvent& event);

    //scrolling and painting
    void AdjustScrollbars();
    void ResizeControls();
    void GetViewStart (int* x, int* y) const;
    void GetScrollPixelsPerUnit (int* x_unit, int* y_unit) const;
    void RepaintScoreRectangle(wxDC* pDC, wxRect& repaintRect);
	wxPoint GetDCOriginForPage(int nNumPage);

    // print/preview/export as image
    void GetPageInfo(int* pMinPage, int* pMaxPage, int* pSelPageFrom, int* pSelPageTo);
    void DrawPage(wxDC* pDC, int nPage, lmPrintout* pPrintout);
    void SaveAsImage(wxString& sFilename, wxString& sExt, int nImgType);

    //access to components
    lmController* GetController() { return (lmController*)m_pCanvas; }
    lmScoreDocument* GetDocument() { return m_pDoc; }

    // sound related methods
    void OnVisualHighlight(lmScoreHighlightEvent& event);

	//visual cursor management
	void CursorRight();
	void CursorLeft();
	void CursorUp();
	void CursorDown();
	void CursorAtPoint(lmUPoint& point);



private:

	// units conversion
	lmDPoint GetScrollOffset();
    lmDPoint GetPageOffset(int nNumPage);
    void DeviceToLogical(lmDPoint& posDevice, lmUPoint& posLogical,
                         lmDPoint* pPagePosD,
						 lmDPoint* pPaperOrgD, lmDPoint* pOffsetD,
						 int* pNumPage, bool* pfInInterpageGap);
	void LogicalToDevice(lmUPoint& posLogical, lmDPoint& posDevice);

    // Auxiliary for scrolling
    int CalcScrollInc(wxScrollEvent& event);
    void DoScroll(int orientation, int nScrollSteps);

	//Dealing with the cursor
	void DrawCursor();
    void UpdateCursor();
    void EnableCursor(bool fEnable);
    void SetInitialCursorPosition();


        //-- variables ---

    // parents, managers and related
    lmScoreDocument*    m_pDoc;             //the MVC document (M)
    lmScoreCanvas*      m_pCanvas;          //the MVC controller (C) and the window for rendering the view
    lmFontManager       m_fontManager;      //font management
    lmGraphicManager    m_graphMngr;        //rederization manager
    lmEditFrame*        m_pFrame;           //the frame for the view

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

    double      m_rScale;       // presentation scale
    lmPaper     m_Paper;        // the lmPaper object to use

    // visual options
    bool            m_fRulers;      // draw rulers
    wxColour        m_colorBg;      // colour for background

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
    lmUPoint        m_dragStartPosL;
    lmDPoint        m_dragHotSpot;        // pixels
    wxDragImage*    m_pDragImage;
    lmScoreObj*     m_pSoDrag;            // lmScoreObj being dragged

	    //cursor display and positioning

    //cursor: current position in score
    bool            m_fCursorEnabled;       //to supress cursor display
    bool            m_fCursorShown;         //to know its state
	lmInstrument*	m_pCursorInstr;			//current position: instrument
	int				m_nCursorStaff;			//current position: staff
	int				m_nCursorMeasure;		//current position: measure
	int				m_nCursorTime;			//current position: time (from measure start)
	lmStaffObj*		m_pCursorSO;			//staff object pointed by the cursor
	int		        m_nCursorIdSO;		    //previous staff object pointed by the cursor
	lmStaffObjIterator* m_pCursorIT;        //iterator, to speed up cursor moves

    //timer for cursor blinking
	wxTimer			m_oCursorTimer;			//for cursor blinking

    //cursor position in screen and geometry
    lmUPoint        m_oCursorPos;           //to remove old cursor image
	lmLUnits        m_udyLength;
	lmLUnits        m_udxSegment;




    DECLARE_EVENT_TABLE()
};


#endif    // __LM_SCOREVIEW_H__
