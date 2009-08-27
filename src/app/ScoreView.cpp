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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ScoreView.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/scrolbar.h"

#include <vector>

#include "../score/Score.h"
#include "../score/Staff.h"
#include "global.h"
#include "TheApp.h"
#include "ScoreDoc.h"
#include "ScoreView.h"
#include "ScoreCanvas.h"
#include "EditFrame.h"
#include "Processor.h"
#include "FontManager.h"
#include "toolbox/ToolsBox.h"
#include "ArtProvider.h"
#include "../widgets/Ruler.h"
#include "../widgets/Caret.h"
#include "../graphic/GMObject.h"
#include "../graphic/BoxScore.h"
#include "../graphic/BoxPage.h"
#include "../graphic/BoxSystem.h"
#include "../graphic/BoxSlice.h"
#include "../graphic/BoxSliceInstr.h"
#include "../graphic/Handlers.h"


// access to main frame and to status bar
#include "MainFrame.h"
extern lmMainFrame* GetMainFrame();

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

IMPLEMENT_ABSTRACT_CLASS(lmView, wxView)

// IDs for events, windows, etc.
enum
{
	// windows
    lmID_HSCROLL = 1000,
    lmID_VSCROLL,
};

// Dragging states
enum
{
	lmDRAG_NONE = 0,
	lmDRAG_START_LEFT,
	lmDRAG_CONTINUE_LEFT,
	lmDRAG_START_RIGHT,
	lmDRAG_CONTINUE_RIGHT,
};

// keys pressed when a mouse event
enum {
    lmKEY_NONE = 0x0000,
    lmKEY_ALT = 0x0001,
    lmKEY_CTRL = 0x0002,
    lmKEY_SHIFT = 0x0004,
};


#define lmCURSOR_BLINKING_RATE  750		//cursor blinking rate = 750ms

//temporary data for OnMouseEvent method
static int          m_nNumPage;         //score page number (1..n) on which the mouse is placed


// To draw a cast shadow for each page we need the shadow sizes
const lmPixels m_nRightShadowWidth = 3;       //pixels
const lmPixels m_nBottomShadowHeight = 3;     //pixels


IMPLEMENT_DYNAMIC_CLASS(lmScoreView, lmView)

BEGIN_EVENT_TABLE(lmScoreView, lmView)
    EVT_COMMAND_SCROLL	(lmID_HSCROLL, lmScoreView::OnScroll)
    EVT_COMMAND_SCROLL	(lmID_VSCROLL, lmScoreView::OnScroll)
    EVT_MOUSEWHEEL		(lmScoreView::OnMouseWheel)
    EVT_SET_FOCUS       (lmScoreView::OnSetFocus)
    EVT_KILL_FOCUS      (lmScoreView::OnKillFocus)
END_EVENT_TABLE()



lmScoreView::lmScoreView()
    : m_pToolBoxConfig((lmToolBoxConfiguration*)NULL)
    , m_pFrame((lmEditFrame*) NULL)
    , m_pCanvas((lmScoreCanvas*) NULL)
    , m_pHRuler((lmRuler*) NULL)
    , m_pVRuler((lmRuler*) NULL)
    , m_pHScroll((wxScrollBar*) NULL)
    , m_pVScroll((wxScrollBar*) NULL)
    , m_xScrollPosition(0)
    , m_yScrollPosition(0)
    , m_rScale(1.0 * lmSCALE)
    , m_fDraggingTool(false)
    , m_fDisplayCaret(true)
{
    m_pMainFrame = GetMainFrame();          //for accesing StatusBar
    m_pDoc = (lmDocument*) NULL;

    // drag state control initializations
    m_pDragImage = (wxDragImage*) NULL;

    //options
    m_fRulers = false;

    // view layout
    m_xBorder = 6;
    m_yBorder = 6;
    m_yInterpageGap = 8;

    //TODO: user option
    //m_colorBg = wxColour(10,36,106);		//deep blue
    //m_colorBg = wxColour(200, 200, 200);	//light grey
    //m_colorBg = wxColour(127, 127, 127);	//dark grey
    m_colorBg = wxColour(100, 100, 100);    //deep grey

    // initializations
    m_numPages = 0;            // no pages yet
    m_xDisplayPixelsPerLU = 1.0;
    m_yDisplayPixelsPerLU = 1.0;
    m_xScrollPosition = 0;
    m_yScrollPosition = 0;
    m_fRelayoutPending = false;         //no pending relayout

	//cursor initializations
    m_pScoreCursor = (lmScoreCursor*)NULL;
    m_pCaret = (lmCaret*)NULL;
}

lmScoreView::~lmScoreView()
{
    if (m_pCaret)
        delete m_pCaret;

    if (m_pToolBoxConfig)
        delete m_pToolBoxConfig;

    ClearVisiblePagesInfo();
}

void lmScoreView::ClearVisiblePagesInfo()
{
    std::vector<lmVisiblePageInfo*>::iterator it;
    for(it = m_VisiblePages.begin(); it != m_VisiblePages.end(); ++it)
        delete *it;
    m_VisiblePages.clear();
}

bool lmScoreView::OnCreate(wxDocument* doc, long WXUNUSED(flags) )
{
    // The OnCreate function, called when the window is created
    // When a view is created (via main menu 'file > new'  or 'file > open') class wxDocTemplate
    // invokes ::CreateDocument and ::CreateView. This last one invokes ::OnCreate
    // In this method a child MDI frame is created, populated with the
    // needed controls and shown.

    // AWARE
    // When a document is created (but BEFORE the score is created) the view is created and
    // method OnCreate() is invoked. Therefore, pScore will be always NULL at this point.

    //save the document
    m_pDoc = (lmDocument*)doc;

    // create the frame and set its icon and default title
    m_pFrame = new lmEditFrame(doc, this, GetMainFrame());
    m_pFrame->SetIcon( wxArtProvider::GetIcon(_T("app_score"), wxART_TOOLBAR, wxSize(16,16)) );


    //rulers
    m_fRulers = GetMainFrame()->ShowRulers();

#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    m_pFrame->GetSize(&x, &y);
    m_pFrame->SetSize(-1, -1, x, y);
#endif

    //-- Create the controls on the frame's window

    // create the two rulers, and hide them if user has not selected rulers
    m_pHRuler = new lmRuler(m_pFrame, this, -1, wxHORIZONTAL, wxPoint(0,0), 300);
    m_pVRuler = new lmRuler(m_pFrame, this, -1, wxVERTICAL, wxPoint(0,0), 300);
    if (!m_fRulers) {
        m_pHRuler->Hide();
        m_pVRuler->Hide();
    }

    // add an offset of 2 pixels to the rulers to take into account the canvas border
    m_pHRuler->SetOffset(2 - m_xBorder);
    m_pVRuler->SetOffset(2);

    // create the canvas for the score to edit
    m_pCanvas = new lmScoreCanvas(this, m_pFrame, m_pDoc, wxPoint(0, 0), m_pFrame->GetSize(),
                        wxBORDER_SUNKEN, m_colorBg );

    // create the scrollbars
    m_pHScroll = new wxScrollBar(m_pFrame, lmID_HSCROLL, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    m_pVScroll = new wxScrollBar(m_pFrame, lmID_VSCROLL, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);

    SetScale(m_rScale / lmSCALE);            // to create the font and resize controls and scrollbars

    return true;
}

void lmScoreView::ResizeControls()
{
    if (!m_pCanvas) return;

    // Get client area
    lmPixels dxFrame, dyFrame;
    m_pFrame->GetClientSize(&dxFrame, &dyFrame);
    lmPixels dxFree = dxFrame,
             dyFree = dyFrame;

    // Discount scrollbars
    lmPixels dxVScroll = 0;
    lmPixels dyHScroll = 0;
    if (m_pVScroll)
        dxVScroll = m_pVScroll->GetSize().GetWidth();
    if (m_pHScroll)
        dyHScroll = m_pHScroll->GetSize().GetHeight();
    dxFree -= dxVScroll;
    dyFree -= dyHScroll;

    // Discount rulers width plus 2 pixels for separation
    lmPixels dxVRuler = 0, dyHRuler = 0;
    if (m_fRulers) {
        wxASSERT(m_pHRuler && m_pVRuler);
        dxVRuler = m_pVRuler->GetWidth() + 2;
        dyHRuler = m_pHRuler->GetWidth() + 2;
        dxFree -= dxVRuler;
        dyFree -= dyHRuler;
    }

    // Compute available space for the view and set view margin so that
    // the view is centered
    dxFree -= m_xPageSizeD;
    m_xBorder = (dxFree > 0 ? dxFree/2 : 13);


    // Move controls to the computed positions
    lmPixels xLeft = 0, yTop = 3;
    lmPixels dyCanvas = dyFrame - yTop - dyHRuler - dyHScroll;
    lmPixels dxCanvas = dxFrame - xLeft - dxVRuler - dxVScroll;
    if (m_fRulers) {
        lmPixels dxHRuler = m_xPageSizeD;
        m_pHRuler->NewSize(xLeft + dxVRuler + m_xBorder, yTop, dxHRuler);
        m_pVRuler->NewSize(xLeft, yTop + dyHRuler + m_yBorder, dyCanvas - m_yBorder);
    }
    m_pCanvas->SetSize(xLeft + dxVRuler, yTop + dyHRuler, dxCanvas, dyCanvas);
    m_pHScroll->SetSize(xLeft, yTop + dyHRuler + dyCanvas, dxFrame - xLeft - dxVScroll, dyHScroll);
    m_pVScroll->SetSize(xLeft + dxVRuler + dxCanvas, yTop, dxVScroll, dyCanvas + dyHRuler);

    // compute new scrollbars
    AdjustScrollbars();
    m_pCanvas->Refresh();
}

void lmScoreView::AdjustScrollbars()
{
    if (!m_pCanvas) return;

    // Lets verify if scrollbars are needed
    int dxCanvas, dyCanvas;
    m_pCanvas->GetSize(&dxCanvas, &dyCanvas);
    //bool fHScroll = (dxCanvas < m_xPageSizeD),
    //     fVScroll = (dyCanvas < m_yPageSizeD);

    // scroll step size will be 5 mm . transform into device units (pixels)
    m_pixelsPerStepX = (int)wxMax(lmToLogicalUnits(5, lmMILLIMETERS) * m_xDisplayPixelsPerLU, 1);
    m_pixelsPerStepY = (int)wxMax(lmToLogicalUnits(5, lmMILLIMETERS) * m_yDisplayPixelsPerLU, 1);

    // compute height and width of the whole view (all pages in the view plus margins)
    lmPixels xViewD = m_xPageSizeD + 2 * m_xBorder,
            yViewD = m_numPages * (m_yPageSizeD + m_yInterpageGap)- m_yInterpageGap + 2 * m_yBorder;

    // The range is the total number of units associated with the view represented by the scrollbar.
    // the scrollable space is the whole view minus the visible area (the canvas window)
    // Let's compute how many steps are required
    m_xMaxScrollSteps = 1 + xViewD / m_pixelsPerStepX;        // add 1 to take into account rounding errors
    m_yMaxScrollSteps = 1 + yViewD / m_pixelsPerStepY;

    // compute how many steps are required to scroll one page and the thumb size.
    // The thumb size is the number of units that are currently visible
    if (dxCanvas >= xViewD) {
        // full view visible on current canvas size. No X scrolling needed
        m_xScrollStepsPerPage = m_xMaxScrollSteps;
        m_thumbX = m_xScrollStepsPerPage;
    } else {
        m_thumbX = dxCanvas / m_pixelsPerStepX;
        m_xScrollStepsPerPage = wxMin( m_xPageSizeD / m_pixelsPerStepX , m_xMaxScrollSteps);
    }
    if (dyCanvas >= yViewD) {
        // full view visible on current canvas size. No X scrolling needed
        m_yScrollStepsPerPage = m_yMaxScrollSteps;
        m_thumbY = m_yScrollStepsPerPage;
    } else {
        // full view visible on current canvas size. No X scrolling needed
        m_yScrollStepsPerPage = wxMin((m_yPageSizeD + m_yInterpageGap) / m_pixelsPerStepY , m_yMaxScrollSteps);
        m_thumbY = dyCanvas / m_pixelsPerStepY;
    }

    // Finally, lets set up the scroll bars according to previous calculations
    m_pHScroll->SetScrollbar(m_xScrollPosition, m_thumbX, m_xMaxScrollSteps, m_pixelsPerStepX);
    m_pVScroll->SetScrollbar(m_yScrollPosition, m_thumbY, m_yMaxScrollSteps, m_pixelsPerStepY);

    ////DEBUG
    //wxLogStatus(_T("nPag=%d, Page(%d,%d),Px/Step(%d,%d),View(%d,%d),MaxSteps(%d,%d),StepsPage(%d,%d),Canvas(%d,%d)"),
    //    m_numPages, m_xPageSizeD, m_yPageSizeD, m_pixelsPerStepX, m_pixelsPerStepY,
    //    xViewD, yViewD,
    //    m_xMaxScrollSteps, m_yMaxScrollSteps,
    //    m_xScrollStepsPerPage, m_yScrollStepsPerPage,
    //    dxCanvas, dyCanvas);


}

void lmScoreView::SetRulersVisible(bool fVisible)
{
    m_fRulers = fVisible;
    if (m_pFrame) {
        wxASSERT(m_pVRuler && m_pHRuler);
        m_pHRuler->Show(m_fRulers);
        m_pVRuler->Show(m_fRulers);
        ResizeControls();
    }

}

void lmScoreView::RestoreToolBoxConfiguration()
{
    m_pMainFrame->ShowToolBox(true);
	lmToolBox* pToolBox = m_pMainFrame->GetActiveToolBox();
	wxASSERT(pToolBox);
    pToolBox->SetConfiguration(m_pToolBoxConfig);
}

void lmScoreView::SaveToolBoxConfiguration()
{
    lmToolBox* pToolBox = m_pMainFrame->GetActiveToolBox();
	wxASSERT(pToolBox);
    if (!m_pToolBoxConfig)
        m_pToolBoxConfig = new lmToolBoxConfiguration();
    pToolBox->GetConfiguration(m_pToolBoxConfig);
}

void lmScoreView::OnDraw(wxDC* pDC)
{
    // OnDraw is a mandatory override of wxView. So we must define an OnDraw method. But the
    // repaint behaviour is controled by the OnPaint event on lmScoreCanvas and is redirected
    // to lmScoreView.RepaintScoreRectangle().
    // So OnDraw is empty. It is only invoked by the print/preview architecture, for print/preview
    // the document.

    wxLogMessage(wxT("Error: llega a lmScoreView.OnDraw()"));
}

void lmScoreView::GetPageInfo(int* pMinPage, int* pMaxPage, int* pSelPageFrom, int* pSelPageTo)
{
    // This method is only invoked for print and print-preview. It is invoked from
    // lmPrintout to get the number of pages needed to print the score

    lmScore* pScore = ((lmDocument*)GetDocument())->GetScore();
    if (m_graphMngr.PrepareToRender(pScore, m_xPageSizeD, m_yPageSizeD, m_rScale, &m_Paper))
        OnNewGraphicalModel();

    int nTotalPages = m_graphMngr.GetNumPages();

    *pMinPage = 1;
    *pMaxPage = nTotalPages;
    *pSelPageFrom = 1;
    *pSelPageTo = nTotalPages;
}

void lmScoreView::DrawPage(wxDC* pDC, int nPage, lmPrintout* pPrintout)
{
    // This method is only invoked for print and print-preview. It is invoked from
    // lmPrintout. It is responsible of drawing the requested page.
    // In some displays, the preview at 100% scale does not have the real size.
    // After several attempts and research I discovered that this is due to the
    // fact that changing display resolution doesn't change reported ppi
    // resolution. The effect also affects to other programs (i.e. Adobe Acrobat
    // Reader) that presents the same behaviour that LenMus.


    lmScore* pScore = ((lmDocument*)GetDocument())->GetScore();

    // Get paper size and real usable size of printer paper (in world units)
    lmUSize uPaperSize = pScore->GetPaperSize();     // in lmLUnits
    int printerWidthMM, printerHeightMM;            // in millimeters
    pPrintout->GetPageSizeMM(&printerWidthMM, &printerHeightMM);
    lmLUnits uPrinterSizeX = lmToLogicalUnits(printerWidthMM, lmMILLIMETERS);
    lmLUnits uPrinterSizeY = lmToLogicalUnits(printerHeightMM, lmMILLIMETERS);

    // Get DC size in pixels
    int nDCPixelsW, nDCPixelsH;
    pDC->GetSize(&nDCPixelsW, &nDCPixelsH);

    // Calculate the scaling factor to fit score page in printer paper
    // It should be 1.00 if printer paper is the same size than the
    // intended score paper, but in practise it will be a litle less
    // than 1.00 as wxPrintout reports paper size discounting printer
    // margings
    double marginScaleX = (double)(uPrinterSizeX / uPaperSize.GetWidth());
    double marginScaleY = (double)(uPrinterSizeY / uPaperSize.GetHeight());


    bool fPreview = pPrintout->IsPreview();
    double overallScale;

    if (fPreview) {
        // We have to compute the scaling factor between the preview DC size and
        // the current view size
        double xScale = (double)(nDCPixelsW/(double)m_xPageSizeD) * m_rScale / marginScaleX;
        double yScale = (double)(nDCPixelsH/(double)m_yPageSizeD) * m_rScale / marginScaleY;
        overallScale = wxMin(xScale, yScale);
    }
    else {
        // For printing, as the DC is set in LOMETRIC mode, all scaling
        // is autoatically handled by the DC
        overallScale = lmSCALE;
    }

    pDC->SetUserScale(overallScale, overallScale);
    pDC->SetMapMode(lmDC_MODE);

    bool fNewModel;
    if (fPreview) {
        // use anti-aliasing
        wxMemoryDC memoryDC;
        //m_Paper.SetDrawer(new lmDirectDrawer(&memoryDC));
        fNewModel = m_graphMngr.PrepareToRender(pScore, nDCPixelsW, nDCPixelsH, (double)overallScale, &m_Paper);
        wxBitmap* pPageBitmap = m_graphMngr.RenderScore(nPage);
        wxASSERT(pPageBitmap && pPageBitmap->Ok());
        memoryDC.SelectObject(*pPageBitmap);
        pDC->SetUserScale(1.0, 1.0);
        pDC->SetMapMode(wxMM_TEXT);
        pDC->Blit(0, 0, nDCPixelsW, nDCPixelsH, &memoryDC, 0, 0);
        memoryDC.SelectObject(wxNullBitmap);
    }
    else {
        //Direct renderization on printer DC
        m_Paper.SetDrawer(new lmDirectDrawer(pDC));
        fNewModel = m_graphMngr.PrepareToRender(pScore, nDCPixelsW, nDCPixelsH, (double)overallScale, &m_Paper);
        m_graphMngr.RenderScore(nPage, lmHINT_NO_BITMAPS);
    }

    if (fNewModel)
        OnNewGraphicalModel();
}

void lmScoreView::OnNewGraphicalModel()
{
    //Called when the graphical model has been recreated. This implies that any
    //saved pointer to a lmObject is no longer valid.
    //This method should deal with these pointer.

	m_pDraggedGMO = (lmGMObject*)NULL;	    //object being dragged
	//m_pMouseOverGMO = (lmGMObject*)NULL;	//object on which mouse was flying over
    //wxLogMessage(_T("[lmScoreView::OnNewGraphicalModel]"));
    //inform the controller
    m_pCanvas->OnNewGraphicalModel();
}

void lmScoreView::OnUpdate(wxView* sender, wxObject* hint)
{
    //Called from the document when UpdateAllViews() has been invoked.
    //Re-paint the score and update caret and status bar info

	WXUNUSED(sender)
    if (!m_pFrame) return;

    //if score replaced in document update score related information
    lmUpdateHint* pHints = (lmUpdateHint*)hint;
    if (pHints && pHints->Options() == lmHINT_NEW_SCORE)
    {
        //reload score cursor
        m_pScoreCursor = m_pDoc->GetScore()->GetCursor();
        //m_pCaret->NeedsUpdate(true);
        DeleteCaret();
    }

    //re-paint the score
    HideCaret();

    //do repaint now
    wxClientDC dc(m_pCanvas);
    int width, height;
    m_pCanvas->GetClientSize(&width, &height);
    wxRect rect(0, 0, width, height);
    PrepareForRepaint(&dc, (pHints ? pHints->Options() : 0) );
    RepaintScoreRectangle(&dc, rect, (pHints ? pHints->Options() : 0) );
    TerminateRepaint(&dc);

    //get the score
    lmScore* pScore = m_pDoc->GetScore();
    if (pScore)
    {
        if (m_fDisplayCaret)
        {
            //hide caret if there are objects selected. Otherwise, show it
            int nSel = ((lmBoxScore*)pScore->GetGraphicObject())->GetNumObjectsSelected();
            //wxLogMessage(_T("[lmScoreView::OnUpdate] NumSelected = %d"), nSel);
            m_pCaret->SetInvisible(nSel > 0);
            UpdateCaret();
            ShowCaret();
        }

        //delete the MIDI event table
        pScore->DeleteMidiEvents();
    }

    //delete hints
    if (pHints) delete pHints;

    //inform the controller
    m_pCanvas->OnViewUpdated();

    //clear mouse information
    m_pDragImage = (wxDragImage*) NULL;
}

bool lmScoreView::OnClose(bool deleteWindow)
{
    // Clean up all windows used for displaying this view.

    if (!GetDocument()->Close()) return false;

    Activate(false);

    if (deleteWindow) {
        if (m_pFrame) {
            //m_pFrame->Close();
            delete m_pFrame;
            m_pFrame = (lmEditFrame*) NULL;
        }
        return true;
    }
    return true;
}

void lmScoreView::OnChangeFilename()
{
    // Called when the filename has changed (i.e. File > Save As ..)
    //
    // AWARE:
    // As SetFilename() is invoked in both lmScoreDoc::OnOpenDocument() and
    // lmScoreDoc::OnNewDocument() this causes that the first point at which lmScoreView
    // is invoked (after creation) with a valid pointer to the score is in this method!
    //
    // Change the title of the view frame and initialize things if not done before.

    if (!m_pDoc) return;

    wxString sNewName;
    m_pDoc->GetPrintableName(sNewName);
    lmScore* pScore = m_pDoc->GetScore();
    if (!pScore) return;

    pScore->SetScoreName(sNewName);
    m_pFrame->SetTitle(sNewName);

    SetScale(1.0 / lmSCALE);         //force to compute m_xPageSizeD & m_yPageSizeD for reference
    //TODO: User option te determine if initially 100% or fit width
    SetScaleFitWidth();     //set scale to fit width
    m_pMainFrame->UpdateZoomControls(m_rScale / lmSCALE);  //update MainFrame zoom controls
}

void lmScoreView::OnSetFocus(wxFocusEvent& WXUNUSED(event))
{
    ShowCaret();
}

void lmScoreView::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    HideCaret();
}


void lmScoreView::SetScale(double rScale)
{
    wxASSERT(rScale > 0);
    m_rScale = rScale * lmSCALE;

    if (m_pCanvas)
    {
        // set paper size and margins
        lmScore* pScore = m_pDoc->GetScore();
        //if (pScore)
        //    pScore->SetNumPage(1);

        // compute new paper size in pixels
        wxClientDC dc(m_pCanvas);
        dc.SetMapMode(lmDC_MODE);
        dc.SetUserScale( m_rScale, m_rScale );
        lmUSize uPageSize;
        if (pScore)
            uPageSize = pScore->GetPaperSize();
        //else
            //TODO: posible problem?
        m_xPageSizeD = dc.LogicalToDeviceXRel((int)uPageSize.GetWidth());
        m_yPageSizeD = dc.LogicalToDeviceYRel((int)uPageSize.GetHeight());

        // ----------------------------------------------------------------------------
        // This commented out code produces the same results than the
        // following code. The problem for not geting real size (1 : 1) on
        // screen is due to ppi resolution doesn't change when pixels
        // resolution is changed.

        //// Get the logical pixels per inch of screen
        //wxSize ppiScreen = dc.GetPPI();
        //wxLogMessage(_T("[lmScoreView::SetScale] ppiScreenX=%f, ppiScreenY=%f"),
        //    ppiScreen.GetWidth(), ppiScreen.GetHeight() );

        //// There are approx. 25.4 mm to the inch. There are ppi
        //// device units to the inch. Therefore 1 mm corresponds to
        //// ppi/25.4 device units.
        //lmLUnits oneMM = lmToLogicalUnits(1, lmMILLIMETERS);
        //m_xDisplayPixelsPerLU = (double)ppiScreen.x / (25.4 * (double)oneMM);
        //m_yDisplayPixelsPerLU = (double)ppiScreen.y / (25.4 * (double)oneMM);

        //-----------------------------------------------------------------------------

        // store new conversion factors
        m_xDisplayPixelsPerLU = (double)dc.LogicalToDeviceXRel(100000) / 100000.0;
        m_yDisplayPixelsPerLU = (double)dc.LogicalToDeviceYRel(100000) / 100000.0;

        //reposition controls
        ResizeControls();

        //wxLogMessage(_T("[lmScoreView::SetScale] scale=%f, m_rScale=%f, DisplayPixelsPerLU=(%f, %f), pageSize LU(%d, %d), pageSize pixels(%d, %d)"),
        //    rScale, m_rScale, m_xDisplayPixelsPerLU, m_yDisplayPixelsPerLU,
        //    uPageSize.GetWidth(), uPageSize.GetHeight(),
        //    m_xPageSizeD, m_yPageSizeD);
    }

    //delete the caret. It will be created with new scale at repaint
    if (m_pCaret)
    {
        delete m_pCaret;
        m_pCaret = (lmCaret*)NULL;
    }

    if (m_pCanvas)
        m_pCanvas->Refresh(true);    //true erase background

}

void lmScoreView::SetScaleFitWidth()
{
    if (!m_pCanvas) return;

    int xScreen, yScreen;
    m_pFrame->GetClientSize(&xScreen, &yScreen);

    double xScale = m_rScale * (double)(xScreen-50) / (double)m_xPageSizeD;

    //wxLogMessage(_T("[lmScoreView::SetScaleFitWidth] xScreen=%d, xPageSizeD=%d, rScale=%f, scale=%f"),
    //        xScreen, m_xPageSizeD, m_rScale, xScale );

    SetScale(xScale / lmSCALE);
}

void lmScoreView::SetScaleFitFull()
{
    if (!m_pCanvas) return;

    int xScreen, yScreen;
    m_pFrame->GetClientSize(&xScreen, &yScreen);

    double xScale = m_rScale * (double)(xScreen-50) / (double)m_xPageSizeD;
    double yScale = m_rScale * (double)(yScreen-20) / (double)m_yPageSizeD;
    double rScale = wxMin(xScale, yScale) / lmSCALE;
    SetScale(rScale);
}

void lmScoreView::OnVisualHighlight(lmScoreHighlightEvent& event)
{
	lmScore* pScore = m_pDoc->GetScore();
    lmEHighlightType nHighlightType = event.GetHighlightType();
    switch (nHighlightType) {
        case ePrepareForHighlight:
        {
            m_graphMngr.PrepareForHighlight();
            return;
        }
        break;

        case eRemoveAllHighlight:
        {
			pScore->RemoveAllHighlight((wxWindow*)m_pCanvas);
			return;
        }
        break;

        case eVisualOff:
        case eVisualOn:
        {
        }
        break;

        default:
            wxASSERT(false);
    }

    //AWARE: Only eVisualOff and eVisualOn events reach this point

    //prepare DC
    wxClientDC dc(m_pCanvas);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );
    //m_Paper.SetDrawer(new lmDirectDrawer(&dc));

	//Obtain the StaffObject
	//For events of type eRemoveAllHighlight the pSO is NULL
	lmStaffObj* pSO = event.GetStaffObj();
	int nNumPage = pSO->GetPageNumber();        // nNumPage = 1..n
	//position DC origing according to current scrolling and page position
	wxPoint org = GetDCOriginForPage(nNumPage);
	dc.SetDeviceOrigin(org.x, org.y);

	//do the requested action:  highlight (eVisualOn) / unhighlight (eVisualOff)
	pScore->ScoreHighlight(pSO, &dc, nHighlightType);
}

wxPoint lmScoreView::GetDCOriginForPage(int nNumPage)
{
    // Pages measure (m_xPageSizeD, m_yPageSizeD) pixels.
    // There is a gap between pages of  m_yInterpageGap  pixels.
    // There is a left margin:  m_xBorder  pixels
    // And there is a top margin before the first page:  m_yBorder  pixels

    //First page at (m_xBorder, m_yBorder), size (m_xPageSizeD, m_yPageSizeD)
    //Second page at (m_xBorder, m_yBorder+m_yPageSizeD+m_yInterpageGap)
    //...
    //Page n (1..n) at (m_xBorder, m_yBorder + (n-1) * (m_yPageSizeD+m_yInterpageGap))
    // all this coordinates are referred to view origin (0,0), a virtual infinite
    // paper on which all pages are rendered one after the other.

    lmPixels xPage = m_xBorder;
    lmPixels yPage = m_yBorder + (nNumPage-1) * (m_yPageSizeD + m_yInterpageGap);

    //position DC origing according to current scrolling and page position
    int dx = xPage - m_xScrollPosition* m_pixelsPerStepX;
    int dy = yPage - m_yScrollPosition* m_pixelsPerStepY;
	return wxPoint(dx, dy);
}

void lmScoreView::DeviceToLogical(lmDPoint& posDevice, lmUPoint& posLogical,
                            lmDPoint* pPageNPosD, lmDPoint* pPageNOrgD,
                            lmDPoint* pOffsetD, int* pNumPage, bool* pfInInterpageGap)
{
	//converts a device position (pixels), referred to the lmScoreCanvas window,
	//to logical position (lmLUnits) referred to current page origin.
    //Optionally (if not null pointers) returns:
    // pPageNPosD - device position (lmPixels) referred to current page origin.
    // pPageNOrgD - origing of current page (lmPixels) referred to view origin.
    // pNumPage - number of current page (1..n)
    // pfInInterpageGap - flag informing if position is out of page

    // Set DC in logical units and scaled, so that
    // transformations logical/device and viceversa can be computed
    wxClientDC dc(m_pCanvas);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );

    // We need to know how much the window has been scrolled (in pixels)
    int xScrollUnits, yScrollUnits, xOrg, yOrg;
    GetViewStart(&xOrg, &yOrg);
    GetScrollPixelsPerUnit(&xScrollUnits, &yScrollUnits);
    xOrg *= xScrollUnits;
    yOrg *= yScrollUnits;
    lmDPoint canvasOrgD(xOrg, yOrg);

    //Pages measure (m_xPageSizeD, m_yPageSizeD) pixels.
    //There is a gap between pages of  m_yInterpageGap  pixels.
    //There is a left margin:  m_xBorder  pixels
    //And there is a top margin before the first page:  m_yBorder  pixels
    //Therefore, first page is at (m_xBorder, m_yBorder), size (m_xPageSizeD, m_yPageSizeD)
    //Second page at (m_xBorder, m_yBorder+m_yPageSizeD+m_yInterpageGap)
    //...
    //Page n (1..n) at (m_xBorder, m_yBorder + (n-1) * (m_yPageSizeD+m_yInterpageGap))
    //All this coordinates are referred to view origin (0,0), a virtual infinite
    //paper on which all pages are rendered one after the other.

	lmPixels yPage = posDevice.y + canvasOrgD.y;
	int nNumPage = ((yPage - m_yBorder) / (m_yPageSizeD + m_yInterpageGap)) + 1;
    lmPixels yStartPage = m_yBorder + (nNumPage-1) * (m_yPageSizeD + m_yInterpageGap);
    lmPixels yEndPage = yStartPage + m_yPageSizeD;
	bool fInInterpageGap = (yPage < m_yBorder || yPage > yEndPage);

	// the origin of current page is at (pixels)
    lmDPoint pageNOrgD(m_xBorder, yStartPage);

    // let's compute the position (pixels and logical) referred to current page origin
    lmDPoint pageNPosD(posDevice.x + canvasOrgD.x - pageNOrgD.x,
                     posDevice.y + canvasOrgD.y - pageNOrgD.y);
    lmUPoint pageNPosL(dc.DeviceToLogicalXRel(pageNPosD.x),
                     dc.DeviceToLogicalYRel(pageNPosD.y));


	//move requested answers
    posLogical.x = pageNPosL.x;
    posLogical.y = pageNPosL.y;

    if (pPageNPosD)
        *pPageNPosD = pageNPosD;

    if (pNumPage)
        *pNumPage = nNumPage;

    if (pfInInterpageGap)
        *pfInInterpageGap = fInInterpageGap;

    if (pPageNOrgD) {
        (*pPageNOrgD).x = pageNOrgD.x;
        (*pPageNOrgD).y = pageNOrgD.y;
    }

    if (pOffsetD) {
        (*pOffsetD).x = pageNOrgD.x - canvasOrgD.x;
        (*pOffsetD).y = pageNOrgD.y - canvasOrgD.y;
    }

  //  wxLogMessage(_T("[lmScoreView::DeviceToLogical] coverting canvas point (%d, %d) pixels\n")
		//		 _T("     Point referred to first paper page origin (%d, %d) pixels\n")
		//		 _T("     Point referred to first paper page origin (%.2f, %.2f) lmLUnits\n")
		//		 _T("     Point referred to this paper page origin (%.2f, %.2f) lmLUnits\n")
		//		 _T("     Point is at page %d (yStartPage=%d, yPage=%d, yEndPage=%d) %s"),
  //      posDevice.x, posDevice.y, pageNPosD.x, pageNPosD.y, posLogical.x, pagePosL.y,
		//pageNPosL.x, pageNPosL.y,
		//nNumPage, yStartPage, yPage, yEndPage, (fInInterpageGap ? _T("in gap between pages") : _T("")) );



}

void lmScoreView::UpdateNumPage(int nNumPage)
{
    m_nNumPage = nNumPage;
    GetMainFrame()->SetStatusBarNumPage(nNumPage);
}

lmDPoint lmScoreView::GetPageOffset(int nNumPage)
{
	// Returns the offset to add to a display point (that is, a point in pixels referred to
	// CanvasOrg) to convert it, so that it become referred to current PageOrg

    // Set DC in logical units and scaled, so that
    // transformations logical/device and viceversa can be computed
    wxClientDC dc(m_pCanvas);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );

    // We need to know how much the window has been scrolled (in pixels)
    lmDPoint canvasOrgD = GetScrollOffset();

    //Pages measure (m_xPageSizeD, m_yPageSizeD) pixels.
    //There is a gap between pages of  m_yInterpageGap  pixels.
    //There is a left margin:  m_xBorder  pixels
    //And there is a top margin before the first page:  m_yBorder  pixels
    //Therefore, first page is at (m_xBorder, m_yBorder), size (m_xPageSizeD, m_yPageSizeD)
    //Second page at (m_xBorder, m_yBorder+m_yPageSizeD+m_yInterpageGap)
    //...
    //Page n (1..n) at (m_xBorder, m_yBorder + (n-1) * (m_yPageSizeD+m_yInterpageGap))
    //All this coordinates are referred to view origin (0,0), a virtual infinite
    //paper on which all pages are rendered one after the other.

    lmPixels xStartPage = m_xBorder;
    lmPixels yStartPage = m_yBorder + (nNumPage-1) * (m_yPageSizeD + m_yInterpageGap);

	// the origin of current page is at (pixels)
    lmDPoint pageNOrgD(xStartPage, yStartPage);

    //terefore the offset is
    return lmDPoint(pageNOrgD.x - canvasOrgD.x, pageNOrgD.y - canvasOrgD.y);

}

lmDPoint lmScoreView::GetScrollOffset()
{
	// Returns the offset to add to a display point (that is, a point in pixels referred to
	// CanvasOrg) to convert it, so that it become referred to ViewOrg

    // We need to know how much the window has been scrolled (in pixels)
    int xScrollUnits, yScrollUnits, xOrg, yOrg;
    GetViewStart(&xOrg, &yOrg);
    GetScrollPixelsPerUnit(&xScrollUnits, &yScrollUnits);
    xOrg *= xScrollUnits;
    yOrg *= yScrollUnits;
    return lmDPoint(xOrg, yOrg);
}

void lmScoreView::LogicalToDevice(lmUPoint& posLogical, int nPage, lmDPoint& posDevice)
{
	//converts a logical position (lmLUnits), referred to nPage (1..n) origin to
	//a device position (pixels), referred to the lmScoreCanvas virtual window.

    // Set DC in logical units and scaled, so that
    // transformations logical/device and viceversa can be computed
    wxClientDC dc(m_pCanvas);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );

    //We need to know how much the window has been scrolled (in pixels)
    lmDPoint canvasOrgD = GetScrollOffset();

	//convert logical point to pixels, referred to start of first page origin
	lmDPoint pointRelD(dc.LogicalToDeviceXRel((int)posLogical.x),
                     dc.LogicalToDeviceYRel((int)posLogical.y));

	//the origin of first page is at (pixels)
    lmDPoint pageOrgD(m_xBorder, m_yBorder);

	//therefore the point, referred to canvas origin and first page, is at
	lmDPoint pointAbsD(pointRelD.x + pageOrgD.x, pointRelD.y + pageOrgD.y);

    //add the displacement to current page
    pointAbsD.y += (nPage - 1) * (m_yInterpageGap + m_yPageSizeD);

	//substract canvas scrolling
	posDevice.x = pointAbsD.x - canvasOrgD.x;
	posDevice.y = pointAbsD.y - canvasOrgD.y;


    //// For transformations from page to canvas and viceversa we need to combine both origins
    //lmDPoint vCanvasOffset(pageOrgD.x - canvasOrgD.x, pageOrgD.y - canvasOrgD.y);

  //  wxLogMessage(_T("[lmScoreView::LogicalToDevice] coverting logical point (%.2f, %.2f), nPage %d\n")
		//		 _T("     Point referred to first paper page origin (%d, %d) pixels\n")
		//		 _T("     Point referred to view origin (%d, %d) pixels\n")
		//		 _T("     Point referred to canvas origin (%d, %d) pixels\n"),
  //      posLogical.x, posLogical.y, nPage, pointRelD.x, pointRelD.y, pointAbsD.x, pointAbsD.y,
		//posDevice.x, posDevice.y );
}

void lmScoreView::OnMouseWheel(wxMouseEvent& event)
{

    int nWheelRotation = event.GetWheelRotation();
    int lines = nWheelRotation / event.GetWheelDelta();
    nWheelRotation -= lines * event.GetWheelDelta();

    if (lines != 0) {

        wxScrollEvent newEvent;

        newEvent.SetPosition(0);
        newEvent.SetOrientation(wxVERTICAL);
        newEvent.SetEventObject(m_pCanvas);

        if (event.IsPageScroll())
        {
            if (lines > 0)
                newEvent.SetEventType(wxEVT_SCROLL_PAGEUP);
            else
                newEvent.SetEventType(wxEVT_SCROLL_PAGEDOWN);

            OnScroll(newEvent);
        }
        else
        {
            lines *= event.GetLinesPerAction();
            if (lines > 0)
                newEvent.SetEventType(wxEVT_SCROLL_LINEUP);
            else
                newEvent.SetEventType(wxEVT_SCROLL_LINEDOWN);

            int times = abs(lines);
            for (; times > 0; times--)
                OnScroll(newEvent);
        }
    }

}

void lmScoreView::OnScroll(wxScrollEvent& event)
{
    int nScrollSteps = CalcScrollInc(event);
    if (nScrollSteps == 0)
        return;        // can't scroll further

    if (event.GetOrientation() == wxHORIZONTAL)
        DoScroll(nScrollSteps, 0);
    else
        DoScroll(0, nScrollSteps);
}

void lmScoreView::DoScroll(int xScrollSteps, int yScrollSteps)
{
    // verify limits
    int xNewPos = m_xScrollPosition;
    int yNewPos = m_yScrollPosition;
    int xMaxSteps = m_xMaxScrollSteps - m_thumbX;
    int yMaxSteps = m_yMaxScrollSteps - m_thumbY;

    if (xScrollSteps != 0)
    {
        xNewPos += xScrollSteps;
        xScrollSteps = (xNewPos < 0 ? -m_xScrollPosition :
            ((xNewPos > xMaxSteps) ? (xMaxSteps - m_xScrollPosition) : xScrollSteps) );
    }
    if (yScrollSteps != 0)
    {
        yNewPos += yScrollSteps;
        yScrollSteps = (yNewPos < 0 ? -m_yScrollPosition :
            ((yNewPos > yMaxSteps) ? (yMaxSteps - m_yScrollPosition) : yScrollSteps) );
    }

    if (xScrollSteps == 0 && yScrollSteps == 0)
        return;        // can't scroll further

    //wxLogMessage(_T("[] DoScroll. m_fDraggingTool = %s"), (m_fDraggingTool ? _T("yes") : _T("no")) );

    //hide caret and terminate any drag operation, so that scrolling doesn't create ghost images
    HideCaret();
    bool fDragging = m_fDraggingTool;
    if (fDragging)
        m_pCanvas->TerminateToolDrag();
    else
	    if (m_pDragImage)
            m_pDragImage->Hide();

    //now we can safely do the scroll
    // save data and transform steps into pixels
    m_xScrollPosition += xScrollSteps;
    m_yScrollPosition += yScrollSteps;

    // compute scroll displacement in pixels
    int dx = -m_pixelsPerStepX * xScrollSteps;
    int dy = -m_pixelsPerStepY * yScrollSteps;

    // reposition scrollbars
    m_pHScroll->SetThumbPosition(m_xScrollPosition);
    m_pVScroll->SetThumbPosition(m_yScrollPosition);

    // proceed to do the scrolling
    m_pCanvas->ScrollWindow(dx, dy, (wxRect*)NULL);    // we have to scroll all the window
    if (m_fRulers) {
        if (m_pHRuler) m_pHRuler->ScrollWindow(dx, 0, (wxRect*)NULL);    //rect );
        if (m_pVRuler) m_pVRuler->ScrollWindow(0, dy, (wxRect*)NULL);    //rect );
    }

    m_pCanvas->Update() ;       //force to repaint before painting new tool marks

    //Restore caret
    if (m_fDisplayCaret)
    {
        lmStaff* pStaff = m_pScoreCursor->GetCursorStaff();
        int nPage;
        lmUPoint uPos = m_pScoreCursor->GetCursorPoint(&nPage);
        m_pCaret->Show(m_rScale, nPage, uPos, pStaff);
    }

    //restore the drag operation
    if (fDragging)
        m_pCanvas->StartToolDrag();
    else
	    if (m_pDragImage)
            m_pDragImage->Show();

}


//------------------------------------------------------------------------------------------
// Helper methods for scrolling
//------------------------------------------------------------------------------------------

// Where the current view starts from
void lmScoreView::GetViewStart (int* x, int* y) const
{
    if ( x )
        *x = m_xScrollPosition;
    if ( y )
        *y = m_yScrollPosition;
}

void lmScoreView::GetScrollPixelsPerUnit (int* x_unit, int* y_unit) const
{
    if ( x_unit )
        *x_unit = m_pixelsPerStepX;
    if ( y_unit )
        *y_unit = m_pixelsPerStepY;
}

int lmScoreView::CalcScrollInc(wxScrollEvent& event)
{
    int pos = event.GetPosition();
    int orient = event.GetOrientation();
    int xMaxSteps = m_xMaxScrollSteps - m_thumbX,
        yMaxSteps = m_yMaxScrollSteps - m_thumbY;

    int nScrollInc = 0;
    if (event.GetEventType() == wxEVT_SCROLL_TOP) {
        if (orient == wxHORIZONTAL)
            nScrollInc = - m_xScrollPosition;        // so that total scroll becomes zero
        else
            nScrollInc = - m_yScrollPosition;

    } else if (event.GetEventType() == wxEVT_SCROLL_BOTTOM) {
        if (orient == wxHORIZONTAL)
            nScrollInc = xMaxSteps - m_xScrollPosition;
        else
            nScrollInc = yMaxSteps - m_yScrollPosition;

    } else if (event.GetEventType() == wxEVT_SCROLL_LINEUP) {
        nScrollInc = -1;

    } else if (event.GetEventType() == wxEVT_SCROLL_LINEDOWN) {
        nScrollInc = 1;

    } else if (event.GetEventType() == wxEVT_SCROLL_PAGEUP) {
            if (orient == wxHORIZONTAL)
                nScrollInc = -m_xScrollStepsPerPage;
            else
                nScrollInc = -m_yScrollStepsPerPage;

    } else if (event.GetEventType() == wxEVT_SCROLL_PAGEDOWN) {
            if (orient == wxHORIZONTAL)
                nScrollInc = m_xScrollStepsPerPage;
            else
                nScrollInc = m_yScrollStepsPerPage;

    } else if ((event.GetEventType() == wxEVT_SCROLL_THUMBTRACK) ||
        (event.GetEventType() == wxEVT_SCROLL_THUMBRELEASE))
    {
        if (orient == wxHORIZONTAL)
            nScrollInc = pos - m_xScrollPosition;
        else
            nScrollInc = pos - m_yScrollPosition;

    } else if (event.GetEventType() == wxEVT_SCROLL_CHANGED) {
        // Move focus to canvas
        m_pCanvas->SetFocus();
    }

    return nScrollInc;
}

void lmScoreView::PrepareForRepaint(wxDC* pDC, int nRepaintOptions)
{
    // This method is invoked by lmScoreCanvas::OnPaint to inform that one or
    // more screen rectangles are damaged and need repaint. After this call,
    // one or more RepaintScoreRectangle invocations will follow.
    // The DC is a wxPaintDC and is neither scaled nor scrolled.


	// hide the cursor so repaint doesn't interfere
    //wxLogMessage(_T("[lmScoreView::PrepareForRepaint] Calls HideCaret()"));
    //HideCaret();
    DeleteCaret();

    // allocate a DC in memory for using the offscreen bitmaps
    wxMemoryDC memoryDC;

    // inform the paper that we are going to use it, and get the number of
    // pages needed to draw the score
    lmScore* pScore = m_pDoc->GetScore();
    if (!pScore) return;
    //m_Paper.SetDrawer(new lmDirectDrawer(&memoryDC));
    if (m_graphMngr.PrepareToRender(pScore, m_xPageSizeD, m_yPageSizeD,
                                    m_rScale, &m_Paper, nRepaintOptions) )
        OnNewGraphicalModel();

    int nTotalPages = m_graphMngr.GetNumPages();

    if (nTotalPages != m_numPages) {
        // number of pages has changed. Adjust scrollbars
        m_numPages = nTotalPages;
        AdjustScrollbars();
    }
    ComputeVisiblePagesInfo();

    //prepare colours, brushes and pens
    wxBrush bgBrush(m_colorBg, wxSOLID);
    wxPen bgPen(m_colorBg);
    pDC->SetBrush(bgBrush);
    pDC->SetPen(bgPen);

}

void lmScoreView::TerminateRepaint(wxDC* pDC)
{
    // This method is invoked by lmScoreCanvas::OnPaint to inform there are no
    // more screen rectangles to repaint.
    // The DC is a wxPaintDC and is neither scaled nor scrolled.


    //Get canvas offset
    wxPoint canvasOffset = GetScrollOffset();

    //set a DirectDrawer
    m_Paper.SetDrawer(new lmDirectDrawer(pDC));
    pDC->SetMapMode(lmDC_MODE);
    pDC->SetUserScale( m_rScale, m_rScale );

    //paint handlers on modified pages
    std::vector<lmVisiblePageInfo*>::iterator it;
    for(it = m_VisiblePages.begin(); it != m_VisiblePages.end(); ++it)
    {
        if ((*it)->fRepainted)
        {
            lmBoxPage* pBP = m_graphMngr.GetBoxScore()->GetPage((*it)->nNumPage + 1);

            //set origin on page origin
            pDC->SetDeviceOrigin((*it)->vPageRect.x - canvasOffset.x,
                                 (*it)->vPageRect.y - canvasOffset.y );

            //draw all the handlers booked to be drawn
            pBP->DrawAllHandlers(&m_Paper);
        }
    }

	//Restore caret
    //wxLogMessage(_T("[lmScoreView::TerminateRepaint] Calls ShowCaret()"));
    ShowCaret();
}

void lmScoreView::RepaintScoreRectangle(wxDC* pDC, wxRect& repaintRect, int nRepaintOptions)
{
    // This method is invoked by lmScoreCanvas::OnPaint to repaint a rectangle of the score
    // The DC is a wxPaintDC and is neither scaled nor scrolled.
    // The rectangle to redraw is in pixels and unscrolled

    //if no score, it should'n have arrived here !!
    lmScore* pScore = m_pDoc->GetScore();
    if (!pScore) return;

    // Here in OnPaint we want to know which page
    // to redraw so that we prevent redrawing pages that don't
    // need to get redrawn.

    // Pages measure (m_xPageSizeD, m_yPageSizeD) pixels.
    // There is a gap between pages of  m_yInterpageGap  pixels.
    // There is a left margin:  m_xBorder  pixels
    // And there is a top margin before the first page:  m_yBorder pixels

    //First page at (m_xBorder, m_yBorder), size (m_xPageSizeD, m_yPageSizeD)
    //Second page at (m_xBorder, m_yBorder + m_yPageSizeD + m_yInterpageGap)
    //...
    //Page n+1 at (m_xBorder, m_yBorder + n * (m_yPageSizeD + m_yInterpageGap))
    // all this coordinates are referred to view origin (0,0), a virtual infinite
    // paper on which all pages are rendered one after the other.

    // We need to know how much the window has been scrolled (in pixels)
    wxPoint canvasOffset = GetScrollOffset();

    // allocate a DC in memory for using the offscreen bitmaps
    wxMemoryDC memoryDC;

    std::vector<bool> fModifiedPages( m_numPages, false );

    // the repaintRect is referred to canvas window origin and is unscrolled.
    // To refer it to view origin it is necessary to add scrolling origin
    wxRect drawRect(repaintRect.x + canvasOffset.x, repaintRect.y + canvasOffset.y,
                    repaintRect.width, repaintRect.height );

    //wxLogMessage(_T("Repainting rectangle (%d, %d, %d, %d), drawRect=(%d, %d, %d, %d)"),
    //    repaintRect.x, repaintRect.y, repaintRect.width, repaintRect.height,
    //    drawRect.x, drawRect.y, drawRect.width, drawRect.height );

    //verify if left background needs repaint
    if (drawRect.x < m_xBorder)
    {
        //compute left background rectangle and paint it
        wxRect bgRect(repaintRect.x, repaintRect.y,
                      m_xBorder - repaintRect.x - canvasOffset.x, repaintRect.height );
        pDC->DrawRectangle(bgRect);
    }

    //verify if rigth background needs repaint.
	//Right of page is at (referred to ViewOrg):
    lmPixels xRight = m_xBorder + m_xPageSizeD;
    if (drawRect.x + drawRect.width  > xRight)
    {
		//Right of page, refered to CanvasOrg, is at:
		xRight -= canvasOffset.x;
        //so the rectangle to repaint is
        wxRect bgRect(xRight, repaintRect.y,
                      repaintRect.width + repaintRect.x - xRight,
					  repaintRect.height );
        pDC->DrawRectangle(bgRect);
    }

    //verify if top margin needs repaint
    //top margin rectangle, referred to ViewOrg, is at:
    wxRect bgTopMargin(m_xBorder, 0, m_xPageSizeD, m_yBorder);
	bgTopMargin.Intersect(drawRect);
    if (bgTopMargin.width > 0 && bgTopMargin.height > 0)
    {
        //rectangle, referred to CanvasOrg is at:
        wxRect bgRect(bgTopMargin.x - canvasOffset.x, bgTopMargin.y - canvasOffset.y, bgTopMargin.width, bgTopMargin.height);
		//and referred to canvas is
		bgRect.x -= canvasOffset.x;
		bgRect.y -= canvasOffset.y;
        pDC->DrawRectangle(bgRect);
    }

    // loop to verify if page nPag (0..n-1) is visible and needs redrawing.
    int nPag=0;
    std::vector<lmVisiblePageInfo*>::iterator it;
    for(it = m_VisiblePages.begin(); it != m_VisiblePages.end(); ++it)
    {
        wxRect pageRect = (*it)->vPageRect;
        wxRect interRect = (*it)->vVisibleRect;
        interRect.Intersect(drawRect);

        // if intersection is not null this page needs repainting.
        if (interRect.width > 0 && interRect.height > 0)
        {
            nPag = (*it)->nNumPage;

            //mark page as repainted
            fModifiedPages[nPag] = true;
            (*it)->fRepainted = true;

            // ask paper for the offscreen bitmap of this page
            wxBitmap* pPageBitmap
                = m_graphMngr.RenderScore(nPag+1, nRepaintOptions,
                                          m_pCanvas, (*it)->vPageRect.GetTopLeft());
            wxASSERT(pPageBitmap && pPageBitmap->Ok());
            memoryDC.SelectObject(*pPageBitmap);

            // Intersection rectangle is referred to view origin. To refer it
            // to bitmap coordinates we need to substract page origin
            int xBitmap = interRect.x - pageRect.x,
                yBitmap = interRect.y - pageRect.y;
            // and to refer it to canvas window coordinates we need to
            // substract scroll origin
            int xCanvas = interRect.x - canvasOffset.x,
                yCanvas = interRect.y - canvasOffset.y;

            //wxLogMessage(_T("nPag=%d, canvasOrg (%d,%d), bitmapOrg (%d, %d), interRec (%d, %d, %d, %d), pageRect (%d, %d, %d, %d)"),
            //    nPag, xCanvas, yCanvas, xBitmap, yBitmap,
            //    interRect.x, interRect.y, interRect.width, interRect.height,
            //    pageRect.x, pageRect.y, pageRect.width, pageRect.height);

            // Copy the damaged rectangle onto the device DC
            pDC->Blit(xCanvas, yCanvas, interRect.width, interRect.height,
                        &memoryDC, xBitmap, yBitmap);

            // deselect the bitmap
            memoryDC.SelectObject(wxNullBitmap);

            ////DEBUG: draw red rectangle to show updated rectangle
            //pDC->SetBrush(*wxTRANSPARENT_BRUSH);
            //pDC->SetPen(*wxRED_PEN);
            //pDC->DrawRectangle(xCanvas, yCanvas, interRect.width, interRect.height);

            //paint backgroud in the remaining area
            //TODO

            ////draw page cast shadow
            //// to refer page rectangle to canvas window coordinates we need to
            //// substract scroll origin
            //int xRight = pageRect.x + pageRect.width - canvasOffset.x,
            //    yTop = pageRect.y - canvasOffset.y,
            //    xLeft = pageRect.x - canvasOffset.x,
            //    yBottom = pageRect.y + pageRect.height - canvasOffset.y;
            //pDC->SetBrush(*wxBLACK_BRUSH);
            //pDC->SetPen(*wxBLACK_PEN);
            //pDC->DrawRectangle(xRight, yTop + m_nBottomShadowHeight,
            //                   m_nRightShadowWidth, pageRect.height);
            //pDC->DrawRectangle(xLeft + m_nRightShadowWidth, yBottom,
            //                   pageRect.width, m_nBottomShadowHeight);

        }

		//verify if background after page needs repaint
		//intergap rectangle, referred to ViewOrg, is at:
		wxRect bgRect(m_xBorder,
					  m_yBorder + nPag * (m_yPageSizeD+m_yInterpageGap) + m_yPageSizeD,
					  m_xPageSizeD,
					  m_yInterpageGap);
		if (nPag == m_numPages -1)
		{
			//As this is the last page, instead of using the intergap rectangle we are enlarge
			//the rectangle to an arbitrarily high height: two pages.
			bgRect.height = m_yPageSizeD + m_yPageSizeD;
		}
		bgRect.Intersect(drawRect);
		if (bgRect.width > 0 && bgRect.height > 0)
		{
			//refer rectangle to canvas
			bgRect.x -= canvasOffset.x;
			bgRect.y -= canvasOffset.y;

			//and paint it
			//pDC->SetBrush(bgBrush);
			//pDC->SetPen(bgPen);
			pDC->DrawRectangle(bgRect);
		}
    }

	////DEBUG: draw cyan rectangle to show updated rectangle
    //pDC->SetBrush(*wxTRANSPARENT_BRUSH);
    //pDC->SetPen(*wxCYAN_PEN);
    //pDC->DrawRectangle(repaintRect);
}

void lmScoreView::SaveAsImage(wxString& sFilename, wxString& sExt, int nImgType)
{
    //compute required screen size (pixels) for 1:1 renderization
    lmScore* pScore = ((lmDocument*)GetDocument())->GetScore();
    wxClientDC dc(m_pCanvas);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( 1.0, 1.0 );
    lmUSize pageSize = pScore->GetPaperSize();
    int paperWidth = dc.LogicalToDeviceXRel((int)pageSize.GetWidth());
    int paperHeight = dc.LogicalToDeviceYRel((int)pageSize.GetHeight());

    //Prepare the GraphicManager
    //m_Paper.SetDC(&dc);           //the layout phase requires a DC
    //m_Paper.SetDrawer(new lmDirectDrawer(&dc));
    if (m_graphMngr.PrepareToRender(pScore, paperWidth, paperHeight, 1.0, &m_Paper))
        OnNewGraphicalModel();

    //Now proceed to export images
    m_graphMngr.ExportAsImage(sFilename, sExt, nImgType);
}

void lmScoreView::DumpBitmaps()
{
    wxString sFilename = _T("lenmus_bitmap");
    wxString sExt = _T("jpg");
    m_graphMngr.BitmapsToFile(sFilename, sExt, wxBITMAP_TYPE_JPEG);
}

void lmScoreView::MoveCaretToObject(lmGMObject* pGMO)
{
    //move caret to object
    if (m_pCaret)
    {
        lmStaffObj* pSO = (lmStaffObj*)NULL;
        lmScoreObj* pSCO = pGMO->GetScoreOwner();
        if (pSCO->IsStaffObj())
        {
            //it is a staffobj. Position cursor on it
            pSO = (lmStaffObj*)pSCO;
        }
        else if (pSCO->IsAuxObj())
        {
            //it is an auxobj. locate parent staffobj
            lmScoreObj* pParent = pSCO->GetParentScoreObj();
            while(pParent->IsAuxObj())
            {
                pParent = pParent->GetParentScoreObj();
            }
            if (pParent->IsStaffObj())
                pSO = (lmStaffObj*)pParent;
            //else
                //TODO: should we do anything when owner is an instrument or the score?
        }

        //if staffobj identified, move cursor to it
        if (pSO)
        {
	        MoveCaretToObject((lmStaffObj*)pSO);
        }
    }
}

void lmScoreView::PreparePaperForDirectDrawing(wxDC* pDC, lmDPoint vCanvasOffset)
{
	//prepare paper for direct drawing. This implies to allocate a direct drawer DC and set
    //it to use logical units referred to current page origin and scrolling position

    lmDirectDrawer* pDrawer = new lmDirectDrawer(pDC);
	m_Paper.SetDrawer(pDrawer);
	wxPoint uOrg = GetDCOriginForPage(m_nNumPage);
	pDC->SetDeviceOrigin(uOrg.x, uOrg.y);

    //clip DC to visible page rectangle
    std::vector<lmVisiblePageInfo*>::iterator it;
    for(it = m_VisiblePages.begin(); it != m_VisiblePages.end(); ++it)
    {
        if ((*it)->nNumPage == m_nNumPage-1)
        {
            m_uClippedRect = (*it)->uVisibleRect;
            pDrawer->SetClippingRegion(m_uClippedRect);
            return;
        }
    }
}

//------------------------------------------------------------------------------------------
// caret management
//------------------------------------------------------------------------------------------

void lmScoreView::CaretOn() 
{ 
    m_fDisplayCaret = true;
    if (m_pCaret)
    {
        m_pCaret->SetInvisible(false); 
        ShowCaret();    //force to show it
    }
}

void lmScoreView::CaretOff() 
{ 
    m_fDisplayCaret = false;
    if (m_pCaret)
    {
        m_pCaret->SetInvisible(true); 
        HideCaret();    //force to hide it
    }
}

void lmScoreView::HideCaret()
{
    //wxLogMessage(_T("[lmScoreView::HideCaret] Calls Caret::Hide()"));
    if (m_pCaret && m_fDisplayCaret)
        m_pCaret->Hide();
}

void lmScoreView::ShowCaret()
{
    //wxLogMessage(_T("[lmScoreView::ShowCaret] Calls Caret::Show()"));
    if (!m_fDisplayCaret)
        return;

	//do nothing if the view doesn't contains a score (view creation time) or if
    //the document is being edited
    lmScore* pScore = m_pDoc->GetScore();
    if (!pScore || m_pDoc->IsBeingEdited())
        return;

	//if the caret is not yet created, do it
    if (!m_pCaret)
        m_pCaret = new lmCaret(this, (lmCanvas*)m_pCanvas);

	//if no pointer to score cursor, get it and reposition it
    if (!m_pScoreCursor)
    {
        m_pScoreCursor = pScore->MoveCursorToStart();
        m_nNumPage = 1;
    }

    //finally, display caret
    if (m_pCaret)
    {
        //if caret is not updated, do it
        if (m_pCaret->NeedsUpdate())
        {
            lmStaff* pStaff = m_pScoreCursor->GetCursorStaff();
            int nPage;
            lmUPoint uPos = m_pScoreCursor->GetCursorPoint(&nPage);
            m_pCaret->Show(m_rScale, nPage, uPos, pStaff);
        }
        else
            m_pCaret->Show();
    }
}

void lmScoreView::DeleteCaret()
{   
    if (m_pCaret)
        delete m_pCaret;
    m_pCaret = (lmCaret*)NULL;
}


void lmScoreView::CaretRight(bool fAlsoChordNotes)
{
	//advance to next staff obj.

    if (!m_pCaret) return;

	//advance to next staff obj.
    //wxLogMessage(_T("[lmScoreView::CaretRight] Calls HideCaret()"));
    HideCaret();
    m_pScoreCursor->MoveRight(fAlsoChordNotes);
    //wxLogMessage(_T("[lmScoreView::CaretRight] Calls UpdateCaret()"));
    UpdateCaret();
    //wxLogMessage(_T("[lmScoreView::CaretRight] Calls ShowCaret()"));
    ShowCaret();
}

void lmScoreView::CaretLeft(bool fPrevObject)
{
	//go back to previous staff obj.

    if (!m_pCaret) return;

    //wxLogMessage(_T("[lmScoreView::CaretLeft] Calls HideCaret()"));
    HideCaret();
    m_pScoreCursor->MoveLeft(fPrevObject);
    //wxLogMessage(_T("[lmScoreView::CaretLeft] Calls UpdateCaret()"));
    UpdateCaret();
    //wxLogMessage(_T("[lmScoreView::CaretLeft] Calls ShowCaret()"));
    ShowCaret();
}

void lmScoreView::CaretUp()
{
	//go up to previous staff

    if (!m_pCaret) return;

    HideCaret();
    m_pScoreCursor->MoveUp();
    UpdateCaret();
    ShowCaret();
}

void lmScoreView::CaretDown()
{
	//go down to next staff

    if (!m_pCaret) return;

    HideCaret();
    m_pScoreCursor->MoveDown();
    UpdateCaret();
    ShowCaret();
}

void lmScoreView::MoveCaretNearTo(lmUPoint uPos, lmVStaff* pVStaff, int nStaff, int nMeasure)
{
    if (!m_pCaret) return;

    //wxLogMessage(_T("[lmScoreView::MoveCaretNearTo]"));
    HideCaret();
	m_pScoreCursor->MoveNearTo(uPos, pVStaff, nStaff, nMeasure);
    UpdateCaret();
    ShowCaret();
}

void lmScoreView::MoveCaretToObject(lmStaffObj* pSO)
{
    if (!m_pCaret) return;

    //wxLogMessage(_T("[lmScoreView::MoveCaretToObject]"));
    HideCaret();
	m_pScoreCursor->MoveCursorToObject(pSO);
    UpdateCaret();
    ShowCaret();
}

void lmScoreView::MoveCursorTo(lmVStaff* pVStaff, int nStaff, int nMeasure,
                              float rTime, bool fEndOfTime)
{
    m_pScoreCursor->MoveTo(pVStaff, nStaff, nMeasure, rTime, fEndOfTime);
}

void lmScoreView::MoveCursorToTime(float rTime, bool fEndOfTime)
{
    m_pScoreCursor->MoveToTime(rTime, fEndOfTime);
}

void lmScoreView::UpdateCaret()
{
    //updates caret position and status bar related info.

    if (!m_pScoreCursor) return;
    if (!m_pCaret) return;

    //Hide cursor at old position
    //wxLogMessage(_T("[lmScoreView::UpdateCaret] Calls Caret::Hide()"));
    if (m_fDisplayCaret)
	    m_pCaret->Hide();

	//status bar: timepos
	m_pMainFrame->SetStatusBarCursorRelPos( m_pScoreCursor->GetCursorTime(), 0);

    ////DBG ------------------------------------------------------------------------------
    //wxString sType = _T("end of collection");
    //lmStaffObj* pSO = m_pScoreCursor->GetStaffObj();
    //if (pSO)
    //{
    //    sType = pSO->GetName();
    //    if (pSO->IsNote())
    //    {
    //        lmNote* pN = (lmNote*)pSO;
    //        lmFPitch fp = FPitch(pN->GetAPitch());
    //        lmKeySignature* pKey = pN->GetApplicableKeySignature();
    //        lmEKeySignatures nKey = (pKey ? pKey->GetKeyType() : earmDo);
    //        sType += _T("-");
    //        sType += FPitch_ToRelLDPName(fp, nKey);
    //    }
    //}
    //m_pMainFrame->SetStatusBarMsg(wxString::Format(_T("cursor pointing to %s"), sType.c_str()));
    ////END DBG --------------------------------------------------------------------------

	//Get cursor new position
    int nPage;
    lmUPoint uPos = m_pScoreCursor->GetCursorPoint(&nPage);
    m_pMainFrame->SetStatusBarNumPage(nPage);

    //if new cursos position is out of currently displayed page/area it is necesary
    //to adjust scrolling to ensure that cursor is visible and that it is displayed at
    //right place.
    //cursorRect is the area that should be visible.
    lmStaff* pStaff = m_pScoreCursor->GetCursorStaff();
    lmLUnits uStaffHeight = pStaff->GetHeight();
    lmURect cursorRect(wxMax(0.0f, uPos.x - 2.0f * uStaffHeight),
                       wxMax(0.0f, uPos.y - uStaffHeight),
                       4.0f * uStaffHeight,                         //width
                       3.0f * uStaffHeight );                       //height

    ScrollTo(nPage, cursorRect);

    //now we can safely display the caret
    if (m_fDisplayCaret)
        m_pCaret->Show(m_rScale, nPage, uPos, pStaff);

    //inform the controller, for updating other windows (i.e. toolsbox)
    GetController()->SynchronizeToolBox();
}

void lmScoreView::DrawSelectionArea(wxDC& dc, lmPixels x1, lmPixels y1, lmPixels x2, lmPixels y2)
{
	//draw a dotted rectangle to show the selected area

	if (x1 != x2 && y1 != y2)
	{
		wxPen dottedPen(*wxBLACK, 1, wxDOT);
		dc.SetPen(dottedPen);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle((long)x1, (long)y1, (long)(x2 - x1), (long)(y2 - y1));
	}
}

lmLUnits lmScoreView::GetMouseTolerance()
{
    //mouse tolerance: when dragging, movements smaller than this value will be ignored
    //The returned value is in lmLUnits

    //TODO: User option?
    if (!m_pDoc) return 1.0f;
	lmScore* pScore = m_pDoc->GetScore();
    if (!pScore) return 1.0f;

    return pScore->TenthsToLogical(5.0f);
}


//-------------------------------------------------------------------------------------
//dragging object (m_pDraggedGMO) with left button
//-------------------------------------------------------------------------------------

bool lmScoreView::OnObjectBeginDragLeft(wxMouseEvent& event, wxDC* pDC,
                                        lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
                                        lmUPoint uPagePos, int nKeys,
                                        lmGMObject* pDraggedGMO,
                                        lmDPoint vDragHotSpot,
                                        lmUPoint uHotSpotShift)
{
    // The mouse was clicked and now has started to drag
    //Returns false if error

	WXUNUSED(vCanvasPos);
    WXUNUSED(nKeys);

    return OnImageBeginDrag(false, pDC, vCanvasOffset, uPagePos, pDraggedGMO, vDragHotSpot,
                            uHotSpotShift, (wxBitmap*)NULL);

}

void lmScoreView::OnObjectContinueDragLeft(wxMouseEvent& event, wxDC* pDC, bool fDraw,
										   lmDPoint vCanvasPos, lmDPoint vCanvasOffset,
										   lmUPoint uPagePos, int nKeys)
{
    // We're currently dragging an object. Move the image

    WXUNUSED(fDraw);
    WXUNUSED(vCanvasPos);
    WXUNUSED(nKeys);

    OnImageContinueDrag(event, false, pDC, vCanvasOffset, uPagePos, vCanvasPos);
}

void lmScoreView::OnObjectEndDragLeft(wxMouseEvent& event, wxDC* pDC, lmDPoint vCanvasPos,
									  lmDPoint vCanvasOffset, lmUPoint uPagePos, int nKeys)
{
    // Left up & dragging: Finish dragging

    WXUNUSED(event);
    WXUNUSED(pDC);
    WXUNUSED(vCanvasPos);
	WXUNUSED(vCanvasOffset);
    WXUNUSED(nKeys);

	OnImageEndDrag(false, pDC, vCanvasOffset, uPagePos);
}



//-------------------------------------------------------------------------------------
// Base methods for dragging an image
//-------------------------------------------------------------------------------------

bool lmScoreView::OnImageBeginDrag(bool fMouseTool, wxDC* pDC,
						  lmDPoint vCanvasOffset, lmUPoint uPagePos,
                          lmGMObject* pDraggedGMO, lmDPoint vDragHotSpot,
                          lmUPoint uHotSpotShift, wxBitmap* pBitmap)
{
    //Start to drag an image. Returns false if error
    //Parameters:
    //  fMouseTool - flag to signal that we are not dragging a shape generated from an StaffObj,
    //              but an image for a mouse tool.
    //  pBitmap -   If starting to drag a mouse tool image pBitmap must point to the bitmap to
    //              use to create the image to drag. This bitmap is deleted in this method,
    //              once the wxImage is created from it.
    //              When dragging a GMO, pBitmap must be NULL, and the necessary bitmap must be
    //              obtained by invoking pDraggedGMO->OnBeginDrag()
    // pDraggedGMO - When dragging a GMO, this parameter is the GMO to drag. When dragging a
    //              mouse tool this parameter is NULL.
    //


	HideCaret();
	m_pCanvas->SetFocus();
	//m_pMainFrame->SetStatusBarMsg(_T("[lmScoreView::OnMouseEvent] Starting dragging"));

	#ifdef __WXDEBUG__
	g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnImageBeginDrag()"));
	#endif

	//to draw and move the drag image two mechanism are possible:
	//1. The object can return a bitmap and the view will take care of moving this bitmap
	//2. The object receive a Drawer DC and can draw (XOR) any image
	//Both mechanisms can be used simultaneously (i.e., in Notes, to draw leger lines)

    m_pDraggedGMO = pDraggedGMO;
    m_vDragHotSpot = vDragHotSpot;
    m_uHotSpotShift = uHotSpotShift;

    // prepare the image to drag
    if (m_pDragImage)
    {
		delete m_pDragImage;
        m_pDragImage = (wxDragImage*) NULL;
    }

    if (!fMouseTool)
        pBitmap = m_pDraggedGMO->OnBeginDrag(m_rScale, pDC);

    if (pBitmap)
    {
        m_pDragImage = new wxDragImage(*pBitmap);
        delete pBitmap;

        // show drag image
        if (!m_pDragImage->BeginDrag(m_vDragHotSpot, m_pCanvas))
	    {
            //wxLogMessage(_T("[lmScoreView::OnImageBeginDrag] m_pDragImage->BeginDrag returns error!"));
            delete m_pDragImage;
            m_pDragImage = (wxDragImage*) NULL;
            //m_nDragState = lmDRAG_NONE;
            return false;       //error
        }
    }

	//prepare paper for direct drawing
	PreparePaperForDirectDrawing(pDC, vCanvasOffset);

	//inform shape:
    // - request the nearest valid position to current position
    // - allow to draw (XOR) whatever it likes
    lmUPoint uFinalPos = uPagePos - m_uHotSpotShift;
    if (fMouseTool)
    {
        m_fDraggingTool = true;
        uFinalPos = m_pCanvas->OnDrawToolMarks(&m_Paper, uFinalPos) + m_uHotSpotShift;
    }
    else
        uFinalPos = m_pDraggedGMO->OnDrag(&m_Paper, uFinalPos) + m_uHotSpotShift;

	lmDPoint vNewPos( m_Paper.LogicalToDeviceX(uFinalPos.x) + vCanvasOffset.x,
					m_Paper.LogicalToDeviceY(uFinalPos.y) + vCanvasOffset.y );

    //if exists, move drag image to received point
    if (m_pDragImage)
    {
		m_pDragImage->Move(vNewPos);
        m_pDragImage->Show();
    }

    return true;        //no error
}

void lmScoreView::OnImageContinueDrag(wxMouseEvent& event, bool fMouseTool, wxDC* pDC,
							          lmDPoint vCanvasOffset, lmUPoint uPagePos,
                                      lmDPoint vCanvasPos)
{
    // We're currently dragging an image. Do it

    // If mouse outside of canvas window let's force autoscrolling.
    bool fDoScroll = false;
    wxSize canvasSize = m_pCanvas->GetSize();
    int xUnits=0, yUnits=0;

    if (vCanvasPos.x < 0)
    {
        fDoScroll = true;
        xUnits = -1;
    }
    else if (vCanvasPos.y < 0)
    {
        fDoScroll = true;
        yUnits = -1;
    }
    else if (vCanvasPos.x > canvasSize.GetWidth())
    {
        fDoScroll = true;
        xUnits = 1;
    }
    else if (vCanvasPos.y > canvasSize.GetHeight())
    {
        fDoScroll = true;
        yUnits = 1;
    }

    if (fDoScroll)
	{
        DoScroll(xUnits, yUnits);

        // The user could held the mouse outside the window for a few seconds to force
        // to scroll several pages. As the mouse is not getting moved no new events
        // would be generated. Therefore we must generate them.
        wxPoint oldPos = ::wxGetMousePosition();    // get mouse position (screen origin)
        ::wxMilliSleep(100);                        // wait 100ms to slow autoscrolling
        wxPoint newPos = ::wxGetMousePosition();    // get new position
        if (oldPos.x == newPos.x && oldPos.y==newPos.y)
		{
            // mouse is held outside the window. Generate a new mouse drag event
            wxEvtHandler* pEvtH = m_pCanvas->GetEventHandler();
            pEvtH->AddPendingEvent(event);
        }
    }
	else
	{
		//prepare paper for direct drawing
		PreparePaperForDirectDrawing(pDC, vCanvasOffset);

		//hide image to not interfere with direct drawing
        if (m_pDragImage)
            m_pDragImage->Hide();

        //Give the shape the opportunity to change final pos and to draw (XOR)
        //whatever it likes
        lmUPoint uFinalPos = uPagePos - m_uHotSpotShift;
        if (fMouseTool)
            uFinalPos = m_pCanvas->OnRedrawToolMarks(&m_Paper, uFinalPos) + m_uHotSpotShift;
        else
            uFinalPos = m_pDraggedGMO->OnDrag(&m_Paper, uFinalPos) + m_uHotSpotShift;

        //move drag image to final point
        if (m_pDragImage)
        {
            lmDPoint vNewPos( m_Paper.LogicalToDeviceX(uFinalPos.x) + vCanvasOffset.x,
                              m_Paper.LogicalToDeviceY(uFinalPos.y) + vCanvasOffset.y );
		    m_pDragImage->Move(vNewPos);
            m_pDragImage->Show();
        }
    }
}

void lmScoreView::OnImageEndDrag(bool fMouseTool, wxDC* pDC, lmDPoint vCanvasOffset,
                                 lmUPoint uPagePos)
{
    // Left up & dragging: Finish dragging

    //When dragging a mouse tool, the controller GMO must erase any XOR draw.
    //When dragging a shpe, it must not render anything. It should only issue
    //the necessary commands to move the dragged object to its new position.


    #ifdef __WXDEBUG__
    g_pLogger->LogTrace(_T("OnMouseEvent"), _T("OnImageEndDrag(). Mouse tool: "), 
                        (fMouseTool ? _T("true") : _T("false")) );
	#endif

    //terminate drag image and delete the image used for dragging
    if (m_pDragImage)
    {
        m_pDragImage->Hide();
        m_pDragImage->EndDrag();
        delete m_pDragImage;
        m_pDragImage = (wxDragImage*) NULL;
    }

    //prepare paper for direct drawing
    PreparePaperForDirectDrawing(pDC, vCanvasOffset);
    //set clip region to current clip region (old)

    //inform the shape. It must not render anything. It should only issue
    //the necessary commands to move the dragged object to its new position.
    lmUPoint uFinalPos = uPagePos - m_uHotSpotShift;
    if (fMouseTool)
    {
        m_fDraggingTool = false;
        m_pCanvas->OnRemoveToolMarks(&m_Paper, uFinalPos);
    }
    else
        m_pDraggedGMO->OnEndDrag(&m_Paper, m_pCanvas, uFinalPos);


    //m_pCanvas->DoReleaseMouse();
    ShowCaret();
}


//-------------------------------------------------------------------------------------
// Selecting/deselecting objects
//-------------------------------------------------------------------------------------

void lmScoreView::SelectGMObject(lmGMObject* pGMO, bool fRedraw)
{
    //deselect all currently selected objects, if any, and select the received object

    if (!pGMO->IsSelectable()) return;
    m_graphMngr.NewSelection(pGMO);
    SelectionDone(fRedraw);
}

void lmScoreView::SelectGMObjectsInArea(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
                                        lmLUnits uYMin, lmLUnits uYMax, bool fRedraw)
{
    //deselect all currently selected objects, if any, and select all objects
    //in page nNumPage, within specified area

    m_graphMngr.NewSelection(nNumPage, uXMin, uXMax, uYMin, uYMax);
    SelectionDone(fRedraw);
}

void lmScoreView::SelectionDone(bool fRedraw)
{
    //A selection has just been prepared. Do several houskeeping tasks

    //move cursor to first object in the selection
    lmStaffObj* pSO = m_graphMngr.GetBoxScore()->GetSelection()->GetFirstOwnerStaffObj();
    if (pSO)
        MoveCaretToObject(pSO);

    //redraw the view to show the selection
    if (fRedraw)
        OnUpdate(this, new lmUpdateHint(lmHINT_NO_LAYOUT));

    //synchronize toolbox selected options with current selected object properties
    GetController()->SynchronizeToolBox();
}

void lmScoreView::DeselectAllGMObjects(bool fRedraw)
{
    if (m_graphMngr.GetNumObjectsSelected() > 0)
    {
        m_graphMngr.ClearSelection();
        if (fRedraw)
            OnUpdate(this, new lmUpdateHint(lmHINT_NO_LAYOUT));
    }

    //restore toolbox selected options to those previously selected by user
    GetController()->RestoreToolBoxSelections();
}

bool lmScoreView::SomethingSelected()
{
    //returns true if there are objects currently selected

    return m_graphMngr.GetNumObjectsSelected() > 0;
}

lmGMSelection* lmScoreView::GetSelection()
{
    return m_graphMngr.GetBoxScore()->GetSelection();
}


void lmScoreView::ComputeVisiblePagesInfo()
{
    //We want to know which pages are displayed and the page rectangle displayed.
    //All this information is going to be stored in m_VisiblePages array

    //clear previous info
    ClearVisiblePagesInfo();

    //prepare a DC for units conversion
    wxClientDC dc(m_pCanvas);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );

    // We need to know how much the window has been scrolled (in pixels)
    wxPoint canvasOffset = GetScrollOffset();

    //and the width and height of the canvas (the visible area)
    int width, height;
    m_pCanvas->GetClientSize(&width, &height);
    wxRect displayedRect = wxRect(0, 0, width, height);

    // displayedRect is referred to canvas window origin and is unscrolled.
    // To refer it to view origin it is necessary to add scrolling origin
    displayedRect.x += canvasOffset.x;
    displayedRect.y += canvasOffset.y;


    // Pages measure (m_xPageSizeD, m_yPageSizeD) pixels.
    // There is a gap between pages of  m_yInterpageGap  pixels.
    // There is a left margin:  m_xBorder  pixels
    // And there is a top margin before the first page:  m_yBorder pixels

    //First page at (m_xBorder, m_yBorder), size (m_xPageSizeD, m_yPageSizeD)
    //Second page at (m_xBorder, m_yBorder + m_yPageSizeD + m_yInterpageGap)
    //...
    //Page n+1 at (m_xBorder, m_yBorder + n * (m_yPageSizeD + m_yInterpageGap))
    // all this coordinates are referred to view origin (0,0), a virtual infinite
    // paper on which all pages are rendered one after the other.

    //loop to verify if page nPag (0..n-1) is visible.
    std::vector<bool> fVisiblePages( m_numPages, false );
    int nPag=0;
    for (nPag=0; nPag < m_numPages; nPag++)
    {
        //compute this page rectangle, referred to view origin (0,0)
        wxRect pageRect(m_xBorder,
                        m_yBorder + nPag * (m_yPageSizeD+m_yInterpageGap),
                        m_xPageSizeD,
                        m_yPageSizeD);

        //intersect pageRect with displayedRect to verify if this page is displayed
        wxRect interRect(pageRect.x,
                         pageRect.y,
                         pageRect.width + m_nRightShadowWidth,
                         pageRect.height + m_nBottomShadowHeight);
        interRect.Intersect(displayedRect);

        // if intersection is not null this page is displayed.
        if (interRect.width > 0 && interRect.height > 0)
        {
            //mark page as displayed
            fVisiblePages[nPag] = true;

            //intersection rectangle is referred to view origin. We have to refer it
            //to page origin and convert to logical units
            lmURect uVisibleRect(dc.DeviceToLogicalXRel(interRect.x - pageRect.x),
                                 dc.DeviceToLogicalYRel(interRect.y - pageRect.y),
                                 dc.DeviceToLogicalXRel(interRect.width),
                                 dc.DeviceToLogicalYRel(interRect.height));

            ////DBG --------------------------------
            ////intersection rectangle is referred to view origin. To refer it
            ////to bitmap coordinates we need to substract page origin
            //int xBitmap = interRect.x - pageRect.x,
            //    yBitmap = interRect.y - pageRect.y;

            //// and to refer it to canvas window coordinates we need to
            //// substract scroll origin
            //int xCanvas = interRect.x - canvasOffset.x,
            //    yCanvas = interRect.y - canvasOffset.y;

            //wxLogMessage(_T("nPag=%d, canvasOrg (%d,%d), bitmapOrg (%d, %d), interRec (%d, %d, %d, %d), pageRect (%d, %d, %d, %d), uVisible(%.2f, %.2f, %.2f, %.2f)"),
            //    nPag, xCanvas, yCanvas, xBitmap, yBitmap,
            //    interRect.x, interRect.y, interRect.width, interRect.height,
            //    pageRect.x, pageRect.y, pageRect.width, pageRect.height,
            //    uVisibleRect.x, uVisibleRect.y, uVisibleRect.width, uVisibleRect.height);
            ////END DBG ----------------------------

            //store info about this page
            lmVisiblePageInfo* pInfo = new lmVisiblePageInfo;
            pInfo->nNumPage = nPag;
            pInfo->vVisibleRect = interRect;
            pInfo->uVisibleRect = uVisibleRect;
            pInfo->vPageRect = pageRect;
            pInfo->fRepainted = false;
            m_VisiblePages.push_back(pInfo);
        }

        //to optimize, we will exit the loop as soon as we find a non-visible page after
        //a visible one.
        if (nPag > 0
            && fVisiblePages[nPag-1]
            && !fVisiblePages[nPag] )    break;
    }
}

void lmScoreView::ScrollTo(int nNumPage, lmURect uNewRect)
{
    //scroll to make uVisibleRectangle visible
    //Remember that view layout is as follows:
    //  Pages measure (m_xPageSizeD, m_yPageSizeD) pixels.
    //  There is a gap between pages of  m_yInterpageGap  pixels.
    //  There is a left margin:  m_xBorder  pixels
    //  And there is a top margin before the first page:  m_yBorder pixels
    //
    //  First page at (m_xBorder, m_yBorder), size (m_xPageSizeD, m_yPageSizeD)
    //  Second page at (m_xBorder, m_yBorder + m_yPageSizeD + m_yInterpageGap)
    //      ...
    //  Page n+1 at (m_xBorder, m_yBorder + n * (m_yPageSizeD + m_yInterpageGap))
    //  all this coordinates are referred to view origin (0,0), a virtual infinite
    //  paper on which all pages are rendered one after the other.


    //refer nNumPage to zero (0..n-1)
    --nNumPage;

    //prepare a DC for units conversion
    wxClientDC dc(m_pCanvas);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );

    //convert new rectangle to pixels
    wxRect vNewRect(dc.LogicalToDeviceXRel(uNewRect.x),
                    dc.LogicalToDeviceYRel(uNewRect.y),
                    dc.LogicalToDeviceXRel(uNewRect.width),
                    dc.LogicalToDeviceYRel(uNewRect.height) );

    //locate relevant page info
    int nVisiblePage;
    std::vector<lmVisiblePageInfo*>::iterator it;
    for(it = m_VisiblePages.begin(); it != m_VisiblePages.end(); ++it)
    {
        nVisiblePage = (*it)->nNumPage;
        if (nVisiblePage > nNumPage)
        {
            //new rectangle is above first displayed page. 'it' points to first page
            break;
        }
        else if  (nVisiblePage == nNumPage)
        {
            //new rectangle is in displayed page. 'it' points to the page
            break;
        }
    }

    if (it == m_VisiblePages.end())
    {
        //new rectangle is after last displayed page.
        --it;       // 'it' points to last displayed page
    }

    //here 'it' points to the relevant page, and nVisiblePage contains its number (0..n-1)
    wxRect vPageRect = (*it)->vPageRect;
    wxRect vVisibleRect = (*it)->vVisibleRect;
    lmURect uDisplayedRect = (*it)->uVisibleRect;

    //refer vNewRect to the same origin (canvas) than vPageRect and vVisibleRect
    vNewRect.x += vPageRect.x;
    vNewRect.y += vPageRect.y;

    //analyze case
    lmPixels vxRight=0, vyDown=0;           //amount to scroll (pixels)

    //Compute vertical scroll
    if (nNumPage > nVisiblePage)
    {
        //new rectangle is in another page, after visible one
        //case V1B

        //advance to start of next page
        vyDown = (vPageRect.y + vPageRect.height) - (vVisibleRect.y + vVisibleRect.height);

        //add page intergap and page sizes
        int nPages = nNumPage - (m_nNumPage-1);
        vyDown += m_yInterpageGap * nPages + vPageRect.height * (nPages - 1);

        //add displacement to end of new rectangle
        vyDown += vNewRect.y + vNewRect.height;
    }

    else if (nNumPage == nVisiblePage)
    {
        //new rectangle is in the same page

        //if new rectangle is inside visible rectangle, nothing to do
        if (vNewRect.y < vVisibleRect.y ||
            uNewRect.y + uNewRect.height > (uDisplayedRect.y + uDisplayedRect.height) )
        {
            lmLUnits uDown;
            if (vNewRect.y > vVisibleRect.y)
            {
                //new rectangle is below visible rectangle. Vertical scroll, down
                //case V2B
                uDown = uNewRect.y + uNewRect.height - (uDisplayedRect.y + uDisplayedRect.height);
            }
            else
            {
                //new rectangle is above visible rectangle. Vertical scroll, up
                //case V2A
                uDown = uNewRect.y - uDisplayedRect.y;
            }
            vyDown = dc.LogicalToDeviceYRel(uDown);
        }
    }

    else
    {
        //new rectangle is in another page, before visible one
        //case V1A

        //advance to start of next page
        vyDown = (vPageRect.y + vPageRect.height) - (vNewRect.y + vNewRect.height);

        //add page intergap and page sizes
        int nPages = nNumPage - (m_nNumPage-1);
        vyDown += m_yInterpageGap * nPages + vPageRect.height * (nPages - 1);

        //add displacement to start of displayed rectangle
        vyDown += vVisibleRect.y;
        vyDown = - vyDown;          //change sign to do up scroll
    }


    //Compute horizontal scroll
    //if new rectangle is inside visible rectangle, nothing to do
    if (vNewRect.x < vVisibleRect.x ||
        uNewRect.x + uNewRect.width > (uDisplayedRect.x + uDisplayedRect.width) )
    {
        lmLUnits uRight;
        if (vNewRect.x > vVisibleRect.x)
        {
            //new rectangle is at right of visible rectangle. Horizontal scroll, right
            //case V1R
            uRight = uNewRect.x + uNewRect.width - (uDisplayedRect.x + uDisplayedRect.width);
        }
        else
        {
            //new rectangle is at left of visible rectangle. Horizontal scroll, left
            //case V1L
            uRight = uNewRect.x - uDisplayedRect.x;
        }
        vxRight = dc.LogicalToDeviceYRel(uRight);
    }

    //finally, do scroll
    if (vxRight != 0 || vyDown != 0)
        DoScroll(vxRight / m_pixelsPerStepX, vyDown / m_pixelsPerStepY);
}

void lmScoreView::ScaleDC(wxDC* pDC)
{
    //Set the map mode and the scale

    pDC->SetMapMode(lmDC_MODE);
    pDC->SetUserScale( m_rScale, m_rScale );
}

void lmScoreView::UpdateRulerMarkers(lmDPoint vPagePos)
{
    if (m_fRulers) {
        //lmDPoint ptR(pDC->LogicalToDeviceX(pt.x), pDC->LogicalToDeviceY(pt.y));
        if (m_pHRuler) m_pHRuler->ShowPosition(vPagePos);
        if (m_pVRuler) m_pVRuler->ShowPosition(vPagePos);
    }
}

lmGMObject* lmScoreView::FindShapeAt(int nNumPage, lmUPoint uPos, bool fSelectable)
{
    return m_graphMngr.FindShapeAtPagePos(nNumPage, uPos, fSelectable);
}

lmBox* lmScoreView::FindBoxAt(int nNumPage, lmUPoint uPos)
{
    return m_graphMngr.FindBoxAtPagePos(nNumPage, uPos);
}

