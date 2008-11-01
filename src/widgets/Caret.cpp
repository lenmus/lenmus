//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "Caret.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/colour.h"

#include "Caret.h"
#include "../app/ScoreView.h"
#include "../app/ScoreCanvas.h"
#include "../score/Score.h"
#include "../score/Staff.h"



//====================================================================================
// implementation of class lmCaret
//====================================================================================


// IDs for events, windows, etc.
enum
{
	// caret timer
	lmID_TIMER_CURSOR = 1050,       //AWARE: check to avoid conflicts with the View
};

#define lmVISIBLE true
#define lmHIDDEN  false

IMPLEMENT_CLASS(lmCaret, wxEvtHandler)

BEGIN_EVENT_TABLE(lmCaret, wxEvtHandler)
	EVT_TIMER   (lmID_TIMER_CURSOR, lmCaret::OnCaretTimer)

END_EVENT_TABLE()


lmCaret::lmCaret(lmView* pParent, lmCanvas* pCanvas, lmScore* pScore)
{ 
    m_pView = pParent;
    m_pCanvas = pCanvas;
    m_pScore = pScore;

	//caret initializations
	m_oCaretTimer.SetOwner(this, lmID_TIMER_CURSOR);
	m_oCaretPos.x = -1;         //means: no position   
    m_fCaretDrawn = false;
    m_nCountVisible = 0;
    m_fInvisible = false;       //visible
    SetColour(*wxBLUE);
    m_nBlinkingRate = 750;		//caret blinking rate = 750ms
}

lmCaret::~lmCaret()
{
    if (m_oCaretTimer.IsRunning())
        m_oCaretTimer.Stop();
}

void lmCaret::OnCaretTimer(wxTimerEvent& event)
{
    if (!IsHidden())
    {
    	RenderCaret(!m_fCaretDrawn);
        m_oCaretTimer.Start(m_nBlinkingRate, wxTIMER_ONE_SHOT);
    }
}

void lmCaret::Show(double rScale, int nPage, lmUPoint uPos, lmStaff* pStaff)
{
    //wxLogMessage(_T("[lmCaret::Show] SetCaretPosition --> calls Show()"));
    SetCaretPosition(rScale, nPage, uPos, pStaff);
    Show();
}

void lmCaret::Show(bool fShow)
{
    //wxLogMessage(_T("[lmCaret::Show] fShow=%s, m_nCountVisible=%d"),
    //            (fShow?_T("yes"):_T("no")), m_nCountVisible);

    //Shows or hides the caret.
    //AWARE:
    //  Note that Show() and Hide() operates on a counter. If you invoke Hide() N times 
    //  then you must invoke Show() N times before the caret is displayed again
    //  on the screen.
    //  But invoking Hide() inmediately hides the caret.
    if (fShow)
    {
        if (++m_nCountVisible > 0)
            DoShow();
    }
    else
    {
        if (--m_nCountVisible >= 0)
            DoHide();
    }
}

void lmCaret::DoShow()
{
    m_nCountVisible = 1;
    //wxLogMessage(_T("[lmCaret::DoShow] m_nCountVisible=%d"), m_nCountVisible);
    m_oCaretTimer.Start(m_nBlinkingRate, wxTIMER_ONE_SHOT);
    RenderCaret(lmVISIBLE);
}

void lmCaret::DoHide()
{
    //wxLogMessage(_T("[lmCaret::DoHide] m_nCountVisible=%d"), m_nCountVisible);
    m_oCaretTimer.Stop();
    RenderCaret(lmHIDDEN);
}

void lmCaret::SetCaretPosition(double rScale, int nPage, lmUPoint uPos, lmStaff* pStaff) 
{ 
    //nPage (1..n)


    //if position and scale don't change, return. Nothing to do.
    if (!pStaff || (uPos == m_oCaretPos && rScale == m_rScale && nPage == m_oCaretPage))
        return;

    Hide();

    //set new position
    m_rScale = rScale;
    m_oCaretPage = nPage;
    m_oCaretPos = uPos;
    m_oCaretPos.y -= pStaff->TenthsToLogical(10.0);

	m_udyLength = pStaff->TenthsToLogical(60.0);
	m_udxSegment = pStaff->TenthsToLogical(5.0);

    //render it
    Show();
}

void lmCaret::RenderCaret(bool fVisible)
{
    //AWARE: This method is the only allowed to change m_fCaretDrawn status.

    //if caret invisible, ensure it will never be drawn visible
    if (m_fInvisible && !m_fCaretDrawn) return;

    //if current state == desired state, nothing to do
    if (m_fCaretDrawn == fVisible) return;

    //if not yet positioned, finish
	if (m_oCaretPos.x == -1) return;

    Refresh();

    //set new status
    m_fCaretDrawn = fVisible;       
}

void lmCaret::Refresh()
{
	// prepare DC
    wxClientDC dc((wxWindow*)m_pCanvas);
	//dc.SetBrush(*wxBLUE_BRUSH);
	int vxlineWidth = 1;
	wxPen pen(m_color, vxlineWidth);
	dc.SetPen(pen);
	dc.SetLogicalFunction(wxXOR);

	//caret geometry
	lmDPoint pointD;
	lmUPoint cursorPos(m_oCaretPos.x, m_oCaretPos.y);
	((lmScoreView*)m_pView)->LogicalToDevice(cursorPos, m_oCaretPage, pointD);
	lmPixels vxLine = pointD.x;
	lmPixels vyTop = pointD.y;

    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( m_rScale, m_rScale );
	lmPixels vyBottom = vyTop + dc.LogicalToDeviceYRel((wxCoord)m_udyLength);
	lmPixels vdxSegment = dc.LogicalToDeviceYRel((wxCoord)m_udxSegment);
    dc.SetMapMode(wxMM_TEXT);
    dc.SetUserScale(1.0, 1.0);

	//draw vertical line
	dc.DrawLine(vxLine, vyTop, vxLine, vyBottom);

	//draw horizontal segments
	dc.DrawLine(vxLine-vdxSegment, vyTop-1, vxLine+vdxSegment+1, vyTop-1);
	dc.DrawLine(vxLine-vdxSegment, vyBottom, vxLine+vdxSegment+1, vyBottom);

}

void lmCaret::SetColour(wxColour color)
{
    // as painting uses XOR we need the complementary
    m_color = wxColour(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );

}

void lmCaret::SetBlinkingRate(int nMillisecs)
{
    m_nBlinkingRate = nMillisecs;
}

void lmCaret::SetInvisible(bool fInvisible)
{ 
    m_fInvisible = fInvisible;

    //if caret is shown, draw it visible/invisible acoording to the new state
    if (m_nCountVisible >= 0)
    {
        if (fInvisible)
        {
            DoHide();
        }
        else
        {
            DoShow();
        }
    }
}
