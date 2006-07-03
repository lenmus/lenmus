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
/*! @file scoreview.cpp
    @brief Implementation file for class lmScoreView
    @ingroup app_gui
*/
/*
The view displays the data and manages user interaction with it, including selection and 
editing.
A view is attached to a document and acts as an intermediary between the document and the
user: the view renders an image of the document on the screen and interprets user input as
operations upon the document. The view also renders the image for both printing and print 
preview.




*/

/*

Units
---------------

    Logical units
        The "logical coordinates space" represents the virtual paper on which real music
        sheets are arranged according to the layout chossen by the user.

    Device units
        The "device coordinates space" representes the physical redering device (display,
        printer, ...) on which the score is rendered. The pixel is the usual unit in this
        space. 
    
    Program will operate on the logical coordinates space. Operation in device coordinates
    and transformation to/from device coordinates will be restricted to low program
    layers.



Considerations for the choice of logical units and data types:
--------------------------------------------------------------

    1. Precision
    -------------------------------
    Los atributos de posición, en MusicXML (common.dtd), utilizan unidades relativas: 
    décimas del espacio entre líneas de pentagrama . Ahora bien, espaciado entre líneas y, 
    en consecuencia, todos los demas valores relativos a esta medida de un pentagrama, debe 
    poder establecerse para cada pentagrama, ya que en una misma pieza pueden coexistir 
    pentagramas de distinto tamaño; por ejemplo, en partituras de piano en obras para varios 
    instrumentos, el piano va en grande mientras que la línea para el otro instrumento va en 
    pequeñito. En consecuencia, no parece una unidad adecuada para una página en la que se 
    mezclen pentagramas de distinto tamaño y habrá que utilizar una unidad que no varíe 
    entre pentagramas, al menos para  parámetros no asociados a un solo pentagrama, 
    tales como espaciados entre pentagramas o márgenes de página. Para estos parámetros 
    se decide utilizar como unidad  una décima de milímetro .  

    En PDF el posicionamiento en el user space se mide por defecto en 1/72 de pulgada, 
    ya que esa unidad es ampliamente utilizada en la industria tipográfica y equivale, 
    aproximadamente, a un punto tipográfico. Su valor es de 0,35 mm. (parece poco precisa). 
    Para fonts utiliza una resolución 1000 veces mayor (¿sería 0,001 mm?). Además, las unidades en el user 
    space pueden definirse con otra precisión distinta, según necesidades. 

    In MusicXML, units are fixed and relative
    las unidades son fijas y relativas, y utiliza como unidad una décima del espacio entre 
    líneas de pentagrama. Suponiendo un espaciado entre líneas de 3mm tendríamos una 
    precisión de 0,3 mm, del estilo de la estándar en PDF. Como cuesta lo mismo, me decanto 
    por permitir más precisión y que pueda variarse según necesidades, según hace PDF.

    One tenth of a millimeter seems to be enough precision for positioning objects but
    cumulative truncation errors are inaceptable. Therefore, at least a couple of decimal 
    figures should be taken into account during computations.
    Another posibilitry is to the use of one thousandth of a mm (one micron) as the choice
    for logical units and operate always with integers.

    2. Paper size
    ---------------------------
    Paper size: let's consider at maximum a DIN A2 paper. This is:
        42.0 x 59.4 cm
        420 x 590 in mm
        4200 x 5900 in tenths of a mm
        42,000 x 59,400 in hundredths of a mm
        420,000 x 594,000 in thousandths of a mm (one micron)
    so a variable of type int32 (-2,146,483,648 to +2,147,483,647) has enough precision 
    and no overflow problems (maximum paper size would be  2.1 Km using the micron
    as logical unit !).
    Another posibility is to use float, as 6 significative digits is enough to deal with the
    greatest numbers expresed either in tenths of a mm or in microns.

    3. wxWidgets data types
    ---------------------------
    wxCoord is the basic type used by wxWidgets for all screen and DC coordinates. It is
    allways an int32 to allow for big virtual canvases. Other types extensively used in
    DC and screen methods are wxPoint, wxRect and wxSize. They use internally type int to
    store coordinates. In conclusion, the use of wxCoord (int32) for storing logical units
    is adequate.

    4. Device Context mapping mode
    ------------------------------
  -    Maximum resolution wxWidgets mapping mode is MM_LOMETRIC, whose units are tenths of a mm.
    But greater precision can be easily achieved just by using MM_LOMETRIC and using a
    scaling factor. For example, to use the micron as logical unit the scaling factor 
    would be 0.01. This scaling factor must multiply the scaling factor used for zooming
    the the view.


    Final conclusions
    ------------------------------
    We are going to use two units:
      - Logical units: to use in those cases in which it is necessary to refer to real world, 
        such as when specifying the physical size of the paper to use. The choosen unit will
        be the micron (one thousand of a millimeter) and will be called "Micron" in the
        program.

      - Relative units (tenths of distance between staff lines) for all other cases. For 
      example to specify a note position. This unit will be called "Tenth" in the program.

    For future portability and to improve program legibility we are going to use our own
    types:
        lmLUnits - for logical units. Mapped to int32
        lmPixels - for device units. Mapped to int32
        lmTenths - for staff relative units. Mapped to int32

    For calculations, all methods will operate (unless strictly necessary) in logical units.
    As the precision is one micron, cumulative truncation errors are neglectable. We can 
    safely operate with integers.


Logical units
----------------------
  - All internal units (the so called 'logical units') are int and represent tenths of
    millimeter (scale mode wxMM_LOMETRIC).

  - The 'logical coordinates space' represents the virtual paper on which real music
    sheets are arranged according to the layout chossen by the user.

  -    Internal cursor positions are, therefore, representing positions on this virtual paper.



Virtual paper layout
----------------------

               +--- container: lmScoreView object ('logical space')
               V
+-------------------------------------------------------------------------------------
|                                                           VIRTUAL PAPER
|   +-- pageOrg
|   |
|   |    <-------------------------- pageSize.width ----------------------->
|   +-->+-------------------------------------------------------------------+
|       |                                   A               (lmPaper object)|
|       |                                   |                               |
|       |                               nTopMarging                         |      
|       |   +-- nLeftMarging                |             nRightMarging-+   |
|       |   |                               V                           |   |
|       |   |   + - - - - - - - - - - - - - - - - - - - - - - - - - - + |   |
|       |   |   |                           A                         | |   |
|       |   |                      MargenSupPentagrama                  |   |      dyAntesPentagrama
|       |   |   |                           V                         | |   |
|       |   |    -----------------------------------------------------  |   |
|       |   V   |-----------------------------------------------------| V   |
|       |        ----- 1er Pentagrama --------------------------------      |
|       |       |-----------------------------------------------------|     |
|       |        -----------------------------------------------------      |
|       |       |                           A                         |     |
|       |                           MargeInfPentagrama                      |      dyTrasPentagrama
|       |       + - - - - - - - - - - - - - - - - - - - - - - - - - - +     |
|       |                          MargenSupPentagrama                      |      dyAntesPentagrama
|       |       |                           V                         |     |
|       |     / ------------------------------------------------------      |
|       |     | ------------------------------------------------------|     |
|       |     | ------ 2º  Pentagrama --------------------------------      |
|       |     | ------------------------------------------------------|     |
|       |     | ------------------------------------------------------      |
|       |    /  |                                                     |     |
|       |   <                          Pentagramas                          |      dyEntrePentagramas
|       |    \  |                                                     |     |
|       |     | ------------------------------------------------------      |
|       |     | ------------------------------------------------------|     |
|       |     | ------ 3er Pentagrama --------------------------------      |
|       |     | ------------------------------------------------------|     |
|       |     \ ------------------------------------------------------      |
|       |       |                           A                         |     |
|       |                           MargeInfPentagrama                      |      dyTrasPentagrama
|       |       + - - - - - - - - - - - - - - - - - - - - - - - - - - +     |
|       |                                                                   |
|       |                                                                   |
|       |                                                                   |
|       |                           EspacioEntreSistemas                    |      dyEntreSistemas
|       |                                                                   |
|       |                                                                   |
|       |                                                                   |
|       |                                                                   |
|       |                                                                   |
|       |                                                                   |
|       |                                                                   |
|       |                                                                   |

*/

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/scrolbar.h"

#include "TheApp.h"
#include "MainFrame.h"
#include "ScoreDoc.h"
#include "scoreView.h"
#include "../score/ScoreCommand.h"
#include "../widgets/Ruler.h"
#include "FontManager.h"
#include "global.h"

// access to main frame
extern lmMainFrame *GetMainFrame();

// IDs windows and others
enum
{
  // windows IDs
    CTROL_HScroll = 1000,
    CTROL_VScroll
};

IMPLEMENT_DYNAMIC_CLASS(lmScoreView, wxView)

// global variables
bool gfDrawSelRec;        //draw selection rectangles around staff objects


BEGIN_EVENT_TABLE(lmScoreView, wxView)
    EVT_COMMAND_SCROLL(CTROL_HScroll, lmScoreView::OnScroll)
    EVT_COMMAND_SCROLL(CTROL_VScroll, lmScoreView::OnScroll)
    EVT_MOUSEWHEEL(lmScoreView::OnMouseWheel)
END_EVENT_TABLE()

lmScoreView::lmScoreView() {
    m_pFrame = (lmEditFrame *) NULL;
    m_pCanvas = (lmScoreCanvas *) NULL;
    m_pHRuler = (lmRuler *) NULL;
    m_pVRuler = (lmRuler *) NULL;
    m_pHScroll = (wxScrollBar *) NULL;
    m_pVScroll = (wxScrollBar *) NULL;
    m_xScrollPosition = 0;
    m_yScrollPosition = 0;
    m_rScale = 1.0 * lmSCALE;

    // drag state control initializations
    m_dragState = DRAG_NONE;
    m_pDragImage = (wxDragImage*) NULL;

    //options
    m_fRulers = false;

    // view layout
    m_xBorder = 13;
    m_yBorder = 13;
    m_yInterpageGap = 17;

    // initializations
    m_numPages = 0;            // no pages yet
    m_xDisplayPixelsPerLU = 1.0;
    m_yDisplayPixelsPerLU = 1.0;
    m_xScrollPosition = 0;
    m_yScrollPosition = 0;
}

lmScoreView::~lmScoreView()
{
    m_pFrame = (lmEditFrame *) NULL;
    m_pCanvas = (lmScoreCanvas *) NULL;
    m_pHRuler = (lmRuler *) NULL;
    m_pVRuler = (lmRuler *) NULL;
    m_pHScroll = (wxScrollBar *) NULL;
    m_pVScroll = (wxScrollBar *) NULL;

}

// The OnCreate function, called when the window is created
// When a view is created (via main menu 'file > new'  or 'file > open') class wxDocTemplate
// invokes ::CreateDocument and ::CreateView. This last one invokes ::OnCreate
// In this method a child MDI frame is created  (.CreateProjectFrame), populated with the
// needed controls and shown.
bool lmScoreView::OnCreate(wxDocument* doc, long WXUNUSED(flags) )
{
    m_pFrame = wxGetApp().CreateProjectFrame(doc, this);
    m_pFrame->SetTitle(_T("lmScoreView"));
    //wxColour colorBg(10,36,106);        //deep blue
    //wxColour colorBg(200, 200, 200);    // light grey
    wxColour colorBg(127, 127, 127);    // dark grey

    m_pFrame->SetBackgroundColour(colorBg);

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
    m_pCanvas = new lmScoreCanvas(this, m_pFrame, wxPoint(0, 0), m_pFrame->GetSize(),
                        wxNO_BORDER, colorBg );

    // create the scrollbars
    m_pHScroll = new wxScrollBar(m_pFrame, CTROL_HScroll, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    m_pVScroll = new wxScrollBar(m_pFrame, CTROL_VScroll, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);

    SetScale(1.0);            // to create the font and resize controls and scrollbars

    //show the frame
    m_pFrame->Show(true);
    Activate(true);

    return true;
}

void lmScoreView::ResizeControls()
{

    // Get client area
    int dxFrame, dyFrame;
    m_pFrame->GetClientSize(&dxFrame, &dyFrame);
    int dxFree = dxFrame,
            dyFree = dyFrame;

    // Discount scrollbars
    // default value is ugly (14 pixels). Lets have wider scrollbars 
    int dxVScroll = 16,    //m_pVScroll->GetSize().GetWidth(),
            dyHScroll = 16;    //m_pHScroll->GetSize().GetHeight();

    dxFree -= dxVScroll;
    dyFree -= dyHScroll;

    // Discount rulers witdth plus 2 pixels for separation
    int vrWidth = 0, hrWidth = 0;
    if (m_fRulers) {
        wxASSERT(m_pHRuler && m_pVRuler);
        vrWidth = m_pVRuler->GetWidth() + 2;
        hrWidth = m_pHRuler->GetWidth() + 2;
        dxFree -= vrWidth;
        dyFree -= hrWidth;
    }

    // Compute space available for canvas
    int cvMaxDx = Min(m_xPageSizeD+m_xBorder+m_xBorder, dxFree),
            cvMaxDy = Min(m_yPageSizeD, dyFree);

    // Compute view origin, to center everything
    dxFree -= cvMaxDx;
    int left = (dxFree > 0 ? dxFree/2 : 0);

    // Move controls to the computed positions
    if (m_fRulers) {
        int dxHR = (cvMaxDx >= m_xPageSizeD+m_xBorder ? m_xPageSizeD : cvMaxDx-m_xBorder);
        m_pHRuler->NewSize(left+vrWidth+m_xBorder, 0, dxHR);
        m_pVRuler->NewSize(left, hrWidth, cvMaxDy);
    }
    m_pCanvas->SetSize(left+vrWidth, hrWidth, cvMaxDx, cvMaxDy);
    m_pHScroll->SetSize(0, dyFrame - dyHScroll, dxFrame - dxVScroll, dyHScroll);
    m_pVScroll->SetSize(dxFrame - dxVScroll, 0, dxVScroll, dyFrame - dyHScroll);

    //wxLogStatus(_T("Frame size(%d,%d) dxVScroll=%d, dyHScroll=%d"),
    //    dxFrame, dyFrame,
    //    dxVScroll, dyHScroll);

    // compute new scrollbars
    AdjustScrollbars();

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
    m_pixelsPerStepX = wxMax(lmToLogicalUnits(5, lmMILLIMETERS) * m_xDisplayPixelsPerLU, 1);
    m_pixelsPerStepY = wxMax(lmToLogicalUnits(5, lmMILLIMETERS) * m_yDisplayPixelsPerLU, 1);

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
    //wxLogStatus(_T("nPag=%d,lmPage(%d,%d),Px/Step(%d,%d),View(%d,%d),MaxSteps(%d,%d),StepsPage(%d,%d),Canvas(%d,%d)"),
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

// OnDraw is a mandatory override of wxView. So we must define an OnDraw method. But the
// repaint behaviour is controled by the OnPaint event on lmScoreCanvas and is redirected
// to lmScoreView.RedrawScoreRectangle().
// So OnDraw is empty. It is only invoked by the print/preview architecture, for print/preview
// the document.
void lmScoreView::OnDraw(wxDC* pDC)
{
    wxLogMessage(wxT("Error: llega a lmScoreView.OnDraw()"));
}

void lmScoreView::GetPageInfo(int* pMinPage, int* pMaxPage, int* pSelPageFrom, int* pSelPageTo)
{
    // inform the paper that we are going to use it, and get the number of
    // pages needed to draw the score
    lmScore *pScore = ((lmScoreDocument *)GetDocument())->GetScore();
    m_Paper.Prepare(pScore, m_xPageSizeD, m_yPageSizeD, m_rScale);
    int nTotalPages = m_Paper.GetNumPages();

    *pMinPage = 1;
    *pMaxPage = nTotalPages;
    *pSelPageFrom = 1;
    *pSelPageTo = nTotalPages;
}

void lmScoreView::DrawPage(wxDC* pDC, int nPage)
{
    // Calculate a suitable scaling factor for drawing the page
    int dxDC, dyDC;
    pDC->GetSize(&dxDC, &dyDC);        // size of the DC in pixels
    float scaleX = (float)dxDC / (float)m_xPageSizeD;
    float scaleY = (float)dyDC / (float)m_yPageSizeD;
    
    // Use x or y scaling factor, whichever fits on the DC
    float actualScale = wxMin(scaleX, scaleY) * m_rScale;
    
    // for printing lets use a x2 scale. Otherwise if printer resolution is high there
    // will be no enough memory for preparing bitmaps of the needed size
    int dxBitmap = dxDC, dyBitmap = dyDC;
    if (actualScale > 2.5) {
        //let's asume it is for printing, as print preview only admits 200%
        actualScale = 2.0;
        dxBitmap = m_xPageSizeD * actualScale;
        dyBitmap = m_yPageSizeD * actualScale;
    }

    // inform the paper that we are going to use it
    lmScore *pScore = ((lmScoreDocument *)GetDocument())->GetScore();
    m_Paper.Prepare(pScore, dxBitmap, dyBitmap, (double)actualScale);

    // allocate a DC in memory for using the offscreen bitmaps
    wxMemoryDC memoryDC;

    // ask paper for the offscreen bitmap of this page
    wxBitmap* pPageBitmap = m_Paper.GetOffscreenBitmap(nPage-1);
    wxASSERT(pPageBitmap && pPageBitmap->Ok());
    memoryDC.SelectObject(*pPageBitmap);

    // Copy the page onto the device DC
    if (dyBitmap != dyDC)
        pDC->SetUserScale((float)dxDC / (float)dxBitmap, (float)dyDC / (float)dyBitmap);
    pDC->Blit(0, 0, dxDC, dyDC, &memoryDC, 0, 0);

    //DEBUG: Write page number
    wxChar buf[200];
    wxSprintf(buf, wxT("Pagina %d"), nPage);
    pDC->DrawText(buf, 10, 10);
    //----------------------------------

    // deselect the las bitmap
    memoryDC.SelectObject(wxNullBitmap);

}


// Called from the document when an update is needed. i.e. when UpdateAllViews() has been invoked
void lmScoreView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
    if (m_pFrame) {
        InvalidateBitmap();
        m_pCanvas->Refresh();
        ResizeControls();
    }

}

// Clean up all windows used for displaying this view.
bool lmScoreView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close()) return false;

    //SetFrame((wxFrame*)NULL);
    Activate(false);

    if (deleteWindow) {
        delete m_pFrame;
        m_pFrame = (lmEditFrame*) NULL;
        return true;
    }
    return true;
}

void lmScoreView::InvalidateBitmap()
{
    //delete m_paperBitmap;    
    //m_paperBitmap = (wxBitmap *) NULL;
    m_Paper.ForceRedraw();

}

void lmScoreView::SetScale(double rScale)
{
    wxASSERT(rScale > 0);
    m_rScale = rScale * lmSCALE;
    InvalidateBitmap();        //to force its creation for the new scale

    if (m_pCanvas) {
        // compute new paper size in pixels
        wxClientDC dc(m_pCanvas);
        dc.SetMapMode(lmDC_MODE);
        dc.SetUserScale( m_rScale, m_rScale );
        wxSize pageSize = m_Paper.GetPaperSize();
        m_xPageSizeD = dc.LogicalToDeviceXRel(pageSize.GetWidth());
        m_yPageSizeD = dc.LogicalToDeviceYRel(pageSize.GetHeight());

        // store new conversion factors
        m_xDisplayPixelsPerLU = (double)dc.LogicalToDeviceXRel(100000) / 100000.0;
        m_yDisplayPixelsPerLU = (double)dc.LogicalToDeviceYRel(100000) / 100000.0;

        //reposition controls
        ResizeControls();    

        //wxLogMessage(_T("[lmScoreView::SetScale] scale=%f, m_rScale=%f, DisplayPixelsPerLU=(%f, %f)"),
        //    rScale, m_rScale, m_xDisplayPixelsPerLU, m_yDisplayPixelsPerLU);
    }

    m_pCanvas->Refresh(true);    //erase background

}

void lmScoreView::PlayScore()
{
    //get the score
    lmScoreDocument* pDoc = (lmScoreDocument*) GetDocument();
    lmScore* pScore = pDoc->GetScore();

    //play the score. Use current metronome setting
    pScore->Play(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, ePM_NormalInstrument,
                 0, m_pCanvas);

}

void lmScoreView::StopPlaying()
{
    //get the score
    lmScoreDocument* pDoc = (lmScoreDocument*) GetDocument();
    lmScore* pScore = pDoc->GetScore();

    //request it to stop playing
    pScore->Stop();

}

void lmScoreView::PausePlaying()
{
    //get the score
    lmScoreDocument* pDoc = (lmScoreDocument*) GetDocument();
    lmScore* pScore = pDoc->GetScore();

    //request it to pause playing
    pScore->Pause();

}

void lmScoreView::OnVisualHighlight(lmScoreHighlightEvent& event)
{
    //get the score
    lmScoreDocument* pDoc = (lmScoreDocument*) GetDocument();
    lmScore* pScore = pDoc->GetScore();

    //prepare paper DC
    wxClientDC dc(m_pCanvas);
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );
    m_Paper.SetDC(&dc);

    //! @todo this will fail when more than one page
    //position DC origing according to current scrolling and page position
    int dx = m_xBorder - m_xScrollPosition* m_pixelsPerStepX;
    int dy = m_yBorder - m_yScrollPosition* m_pixelsPerStepY;
    dc.SetDeviceOrigin(dx, dy);

    //do the highlight / unhighlight
    lmStaffObj* pSO = event.GetStaffObj();
    EHighlightType nHighlightType = event.GetHighlightType();
    pScore->ScoreHighlight(pSO, &m_Paper, nHighlightType);
}

void lmScoreView::OnMouseEvent(wxMouseEvent& event, wxDC* pDC)
{

    // get logical coordinates of point pointed by mouse

    // as wxDragImage works with unscrolled device coordinates we need current position
    // in device units. All device coordinates are referred to the lmScoreCanvas window
    wxPoint canvasPosD(event.GetPosition());

    // Leave DC in logical units and scaled, so that
    // transformations logical/device and viceversa can be computed
    pDC->SetMapMode(lmDC_MODE);
    pDC->SetUserScale( m_rScale, m_rScale );

    // We need to know how much the window has been scrolled (in pixels)
    int xScrollUnits, yScrollUnits, xOrg, yOrg;
    GetViewStart(&xOrg, &yOrg);
    GetScrollPixelsPerUnit(&xScrollUnits, &yScrollUnits);
    xOrg *= xScrollUnits;
    yOrg *= yScrollUnits;
    wxPoint canvasOrgD(xOrg, yOrg);

    // the origin of current page is 
    wxPoint pageOrgD(m_xBorder, m_yBorder);
    //! @todo pageOrgD is valid only for the first page.

    // let's compute the position (pixels and logical) referred to the page origin
    wxPoint pagePosD(canvasPosD.x + canvasOrgD.x - pageOrgD.x,
                     canvasPosD.y + canvasOrgD.y - pageOrgD.y);
    wxPoint pagePosL(pDC->DeviceToLogicalXRel(pagePosD.x),
                     pDC->DeviceToLogicalYRel(pagePosD.y));

    // For transformations from page to canvas and viceversa we need to combine both origins
    wxPoint offsetD(pageOrgD.x - canvasOrgD.x, pageOrgD.y - canvasOrgD.y);

    //wxLogStatus(_T("canvasPosD=(%d, %d), pagePosD=(%d, %d), pagePosL=(%d, %d)"),
    //    canvasPosD.x, canvasPosD.y, pagePosD.x, pagePosD.y, pagePosL.x, pagePosL.y);

    // draw markers on the rulers
    if (m_fRulers) {
        //wxPoint ptR(pDC->LogicalToDeviceX(pt.x), pDC->LogicalToDeviceY(pt.y));
        if (m_pHRuler) m_pHRuler->ShowPosition(pagePosD);
        if (m_pVRuler) m_pVRuler->ShowPosition(pagePosD);
    }

    if (event.LeftDClick() ) {
        // mouse left double click: Select/deselect the object pointed by mouse
        //--------------------------------------------------------------------------

        // locate the object
        lmScoreDocument *doc = (lmScoreDocument *)GetDocument();
        lmScoreObj* pScO = doc->FindSelectableObject(pagePosL);

        // If we've got a valid object on mouse left double click, select/deselect it.
        if (pScO) {
            wxCommandProcessor* pCP = doc->GetCommandProcessor();
            pCP->Submit(new lmScoreCommand(_T("Select object"), CMD_SelectObject, doc, pScO));
        }


    } else if (event.LeftDown() ) {
        // mouse left button down: if pointing an object posible start of dragging.
        // ---------------------------------------------------------------------------

        // locate the object
        lmScoreDocument *doc = (lmScoreDocument *)GetDocument();
        lmScoreObj* pScO = doc->FindSelectableObject(pagePosL);

        // if we've got a valid object, tentatively start dragging
       if (pScO && pScO->IsDraggable()) {
             m_pSoDrag = pScO;
            m_dragState = DRAG_START;
            m_dragStartPosL = pagePosL;        // save mouse position (page logical coordinates)
            // compute the location of the drag position relative to the upper-left 
            // corner of the image (pixels)
            wxPoint hotSpot = pagePosL - pScO->GetGlyphPosition();
            m_dragHotSpot.x = pDC->LogicalToDeviceXRel(hotSpot.x);
            m_dragHotSpot.y = pDC->LogicalToDeviceYRel(hotSpot.y);
       }

    } 
    else if ((event.LeftUp() && m_dragState != DRAG_NONE )) {
        // Left up & dragging: Finish dragging
        //---------------------------------------------------

        m_dragState = DRAG_NONE;

        if (!m_pSoDrag || !m_pDragImage) return;

        // delete the image used for dragging
        m_pDragImage->Hide();
        m_pDragImage->EndDrag();
        delete m_pDragImage;
        m_pDragImage = (wxDragImage *) NULL;

        // Generate move command to move lmStaffObj and update document
        lmScoreDocument *doc = (lmScoreDocument *)GetDocument();
        wxCommandProcessor* pCP = doc->GetCommandProcessor();
        wxPoint finalPos = m_pSoDrag->GetGlyphPosition() + pagePosL - m_dragStartPosL;
        pCP->Submit(new lmScoreCommandMove(_T("Move object"), doc, m_pSoDrag, finalPos));

        ////update document to draw final image and clean up pointers
        //m_pSoDrag->SetFixed(true);
        //m_pSoDrag->EndDrag(m_pSoDrag->GetGlyphPosition() + pagePosL - m_dragStartPosL);
        //lmScoreDocument *doc = (lmScoreDocument *)GetDocument();
        //doc->UpdateAllViews();

        m_pSoDrag = (lmScoreObj *) NULL;

    } 

    else if (event.Dragging() && (m_dragState == DRAG_START)) {
        // The mouse was clicked and now has started to drag
        //-----------------------------------------------------------

        m_dragState = DRAG_DRAGGING;

        // prepare the image to drag
        if (m_pDragImage) delete m_pDragImage;
        wxBitmap* pBitmap = m_pSoDrag->GetBitmap(m_rScale);
        m_pDragImage = new wxDragImage(*pBitmap);    //, wxCursor(wxCURSOR_HAND));
        delete pBitmap;

        // show drag image
        bool fOK = m_pDragImage->BeginDrag(m_dragHotSpot, m_pCanvas);
        if (!fOK) {
            delete m_pDragImage;
            m_pDragImage = (wxDragImage*) NULL;
            m_dragState = DRAG_NONE;
            
        } else {
            //drag image started OK. Move image to current cursor position
            //and show it (was hidden until now)
            wxPoint offset(offsetD.x + m_dragHotSpot.x, offsetD.y + m_dragHotSpot.y);
            m_Paper.SetDC(pDC);
            m_pSoDrag->MoveDragImage(&m_Paper, m_pDragImage, offset, pagePosL, m_dragStartPosL, canvasPosD);
            m_pDragImage->Show();
        }

    }
    else if (event.Dragging() && (m_dragState == DRAG_DRAGGING)) {
        // We're currently dragging. Move the image
        //------------------------------------------------------
        if (!m_pDragImage) return;

        // If mouse outside of canvas window let's force autoscrolling.
        bool fDoScroll = false;
        wxSize canvasSize = m_pCanvas->GetSize();
        int nUnits=0, orientation=0;

        if (canvasPosD.x < 0) {
            fDoScroll = true;
            nUnits = -1;
            orientation = wxHORIZONTAL;
        } else if (canvasPosD.y < 0) {
            fDoScroll = true;
            nUnits = -1;
            orientation = wxVERTICAL;
        } else if (canvasPosD.x > canvasSize.GetWidth()) {
            fDoScroll = true;
            nUnits = 1;
            orientation = wxHORIZONTAL;
        } else if (canvasPosD.y > canvasSize.GetHeight()) {
            fDoScroll = true;
            nUnits = 1;
            orientation = wxVERTICAL;
        }

        if (fDoScroll) {
            m_pDragImage->Hide();
            DoScroll(orientation, nUnits);
            m_pDragImage->Show();
            //wxLogStatus(_T("Scrolling(%d), canvasPosD=(%d, %d), canvasSize=(%d, %d)"),
            //    nUnits, canvasPosD.x, canvasPosD.y,
            //    canvasSize.GetWidth(), canvasSize.GetHeight());

            // The user could held the mouse outside the window for a few seconds to force
            // to scroll several pages. As the mouse is not getting moved no new events
            // would be generated. Therefore we must generate them.
            wxPoint oldPos = ::wxGetMousePosition();    // get mouse position (screen origin)
            ::wxMilliSleep(100);                        // wait 100ms to slow autoscrolling
            wxPoint newPos = ::wxGetMousePosition();    // get new position
            if (oldPos.x == newPos.x && oldPos.y==newPos.y) {
                // mouse is held outside the window. Generate a new mouse drag event
                wxEvtHandler* pEvtH = m_pCanvas->GetEventHandler();
                pEvtH->AddPendingEvent(event);
            }

        } else {
            // just move the image
            wxPoint offset(offsetD.x + m_dragHotSpot.x, offsetD.y + m_dragHotSpot.y);
            m_Paper.SetDC(pDC);
            m_pSoDrag->MoveDragImage(&m_Paper, m_pDragImage, offset, pagePosL, m_dragStartPosL, canvasPosD);
        }

    }

    else if (event.GetEventType() == wxEVT_MOUSEWHEEL ) {
        OnMouseWheel(event);
    }

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
    if ( nScrollSteps == 0 ) return;        // can't scroll further
    DoScroll(event.GetOrientation(), nScrollSteps);
}

void lmScoreView::DoScroll(int orientation, int nScrollSteps)
{
    // verify limits
    int newPos;
    int xMaxSteps = m_xMaxScrollSteps - m_thumbX,
            yMaxSteps = m_yMaxScrollSteps - m_thumbY;

    if (orientation == wxHORIZONTAL) {
        newPos = nScrollSteps + m_xScrollPosition;
        nScrollSteps = (newPos < 0 ? -m_xScrollPosition : 
            ((newPos > xMaxSteps) ? (xMaxSteps - m_xScrollPosition) : nScrollSteps) );
    } else {
        newPos = nScrollSteps + m_yScrollPosition;
        nScrollSteps = (newPos < 0 ? -m_yScrollPosition : 
            ((newPos > yMaxSteps) ? (yMaxSteps - m_yScrollPosition) : nScrollSteps) );
    }

    if (nScrollSteps == 0) return;        // can't scroll further
 

    // save data and transform steps into pixels
    if (orientation == wxHORIZONTAL) {
        m_xScrollPosition += nScrollSteps;
    } else {
        m_yScrollPosition += nScrollSteps;
    }

    // compute scroll displacement in pixels
    int dx = 0, dy = 0;
    if (orientation == wxHORIZONTAL) {
        dx = -m_pixelsPerStepX * nScrollSteps;
    } else {
        dy = -m_pixelsPerStepY * nScrollSteps;
    }

    // reposition scrollbars
    m_pHScroll->SetThumbPosition(m_xScrollPosition);
    m_pVScroll->SetThumbPosition(m_yScrollPosition);

    // proceed to do the scrolling
    m_pCanvas->ScrollWindow(dx, dy, (wxRect *)NULL);    // we have to scroll all the window
    if (m_fRulers) {
        if (m_pHRuler) m_pHRuler->ScrollWindow(dx, 0, (wxRect *)NULL);    //rect );
        if (m_pVRuler) m_pVRuler->ScrollWindow(0, dy, (wxRect *)NULL);    //rect );
    }

#ifdef __WXMAC__
    m_pCanvas->MacUpdateImmediately() ;
#endif

}

//------------------------------------------------------------------------------------------
// Helper methods for scrolling
//------------------------------------------------------------------------------------------

// Where the current view starts from
void lmScoreView::GetViewStart (int *x, int *y) const
{
    if ( x )
        *x = m_xScrollPosition;
    if ( y )
        *y = m_yScrollPosition;
}

void lmScoreView::GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const
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

    } else if (event.GetEventType() == wxEVT_SCROLL_ENDSCROLL) {
        // Move focus to canvas
        m_pCanvas->SetFocus();
    }

    return nScrollInc;
}

// This method is invoked by lmScoreCanvas::OnPaint to repaint a rectangle of the score
// The DC is not scrolled.
// The rectangle to redraw is in pixels and unscrolled
void lmScoreView::RepaintScoreRectangle(wxDC* pDC, wxRect& repaintRect)
{

    // Here in OnPaint we want to know which page
    // to redraw so that we prevent redrawing pages that don't
    // need to get redrawn.

    // Pages measure (xPageSize, yPageSize) pixels.
    // There is a gap between pages of  yInterpageGap  pixels.
    // There is a left margin:  xLeftMargin  pixels
    // And there is a top margin before the first page:  yTopMargin  pixels

    // Let's set these values
    lmPixels xPageSize = m_xPageSizeD,
            yPageSize = m_yPageSizeD,
            yInterpageGap = m_yInterpageGap,
            xLeftMargin = m_xBorder,
            yTopMargin = m_yBorder;


    //First page at (xLeftMargin, yTopMargin), size (xPageSize, yPageSize)
    //Second page at (xLeftMargin, yTopMargin+yPageSize+yInterpageGap)
    //...
    //lmPage n at (xLeftMargin, yTopMargin + n * (yPageSize+yInterpageGap))
    // all this coordinates are referred to view origin (0,0), a virtual infinite
    // paper on which all pages are rendered one after the other.

    // We need to know how much the window has been scrolled (in pixels)
    int xScrollUnits, yScrollUnits, xOrg, yOrg;
    GetViewStart(&xOrg, &yOrg);
    GetScrollPixelsPerUnit(&xScrollUnits, &yScrollUnits);
    xOrg *= xScrollUnits;
    yOrg *= yScrollUnits;

    //// We also need to know the size of the canvas window to see which
    //// pages are completely hidden and must not get redrawn
    //int dxCanvas = 0, dyCanvas = 0;
    //m_pCanvas->GetClientSize(&dxCanvas, &dyCanvas);

    // inform the paper that we are going to use it, and get the number of
    // pages needed to draw the score
    lmScore *pScore = ((lmScoreDocument *)GetDocument())->GetScore();
    if (!pScore) return;
    m_Paper.Prepare(pScore, xPageSize, yPageSize, m_rScale);
    int nTotalPages = m_Paper.GetNumPages();
    if (nTotalPages != m_numPages) {
        // number of pages has changed. Adjust scrollbars
        m_numPages = nTotalPages;
        AdjustScrollbars();
    }

    // allocate a DC in memory for using the offscreen bitmaps
    wxMemoryDC memoryDC;

    // the repaintRect is referred to canvas window origin and is unscrolled.
    // To refer it to view origin it is necessary to add scrolling origin
    wxRect drawRect(repaintRect.x + xOrg, repaintRect.y + yOrg, 
                    repaintRect.width, repaintRect.height );

    // loop to verify if page nPag (0..n-1) is visible and needs redrawing.
    // To optimize, the loop is exited as soon as we find a non-visible page after 
    // a visible one.
    bool fPreviousPageVisible = false;
    int nPag=0;
    for (nPag=0; nPag < m_numPages; nPag++) {
        // Let's compute this page rectangle, referred to view origin (0,0)
        wxRect pageRect(xLeftMargin,
                        yTopMargin + nPag * (yPageSize+yInterpageGap),
                        xPageSize,
                        yPageSize);
        //wxLogStatus(wxT("pageRect(%d,%d, %d, %d)"), 
        //    pageRect.x, pageRect.y, pageRect.width, pageRect.height);

        // Lets intersect pageRect with drawRect to verify if this page is affected
        wxRect interRect = pageRect;
        interRect.Intersect(drawRect);

        // if intersection is not null this page needs repainting.
        if (interRect.width > 0 && interRect.height > 0) {

            // ask paper for the offscreen bitmap of this page
            wxBitmap* pPageBitmap = m_Paper.GetOffscreenBitmap(nPag);
            wxASSERT(pPageBitmap && pPageBitmap->Ok());
            memoryDC.SelectObject(*pPageBitmap);
            //wxLogStatus(wxT("bitmap size (%d,%d)"), 
            //    pPageBitmap->GetWidth(), pPageBitmap->GetHeight());

            // Intersection rectangle is referred to view origin. To refer it
            // to bitmap coordinates we need to substract page origin
            int xBitmap = interRect.x - pageRect.x,
                    yBitmap = interRect.y - pageRect.y;
            // and to refer it to canvas window coordinates we need to 
            // substract scroll origin
            int xCanvas = interRect.x - xOrg,
                    yCanvas = interRect.y - yOrg;

            // Copy the damaged rectangle onto the device DC
            pDC->Blit(xCanvas, yCanvas, interRect.width, interRect.height,
                        &memoryDC, xBitmap, yBitmap);

            ////DEBUG: draw red rectangle to show updated rectangle
            //pDC->SetBrush(*wxTRANSPARENT_BRUSH);
            //pDC->SetPen(*wxRED_PEN);
            //pDC->DrawRectangle(xCanvas, yCanvas, interRect.width, interRect.height);
        }

        //verify if we should finish the loop
        if (fPreviousPageVisible) break;
    }

    ////DEBUG: draw cyan rectangle to show updated rectangle
    //pDC->SetBrush(*wxTRANSPARENT_BRUSH);
    //pDC->SetPen(*wxCYAN_PEN);
    //pDC->DrawRectangle(repaintRect);

    // deselect the bitmap
    memoryDC.SelectObject(wxNullBitmap);

}

