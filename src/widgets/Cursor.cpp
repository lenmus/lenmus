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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Cursor.h"
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

#include "Cursor.h"
#include "../app/ScoreView.h"
#include "../app/ScoreCanvas.h"
#include "../score/Score.h"



//====================================================================================
// implementation of class lmScoreViewCursor
//====================================================================================


// IDs for events, windows, etc.
enum
{
	// cursor timer
	lmID_TIMER_CURSOR = 1050,       //AWARE: check to avoid conflicts with the View
};

#define lmVISIBLE true
#define lmHIDDEN  false

IMPLEMENT_CLASS(lmScoreViewCursor, wxEvtHandler)

BEGIN_EVENT_TABLE(lmScoreViewCursor, wxEvtHandler)
	EVT_TIMER   (lmID_TIMER_CURSOR, lmScoreViewCursor::OnCursorTimer)

END_EVENT_TABLE()


lmScoreViewCursor::lmScoreViewCursor(lmView* pParent, lmCanvas* pCanvas, lmScore* pScore)
{ 
    m_pView = pParent;
    m_pCanvas = pCanvas;
    m_pScore = pScore;

	//cursor initializations
	m_oCursorTimer.SetOwner(this, lmID_TIMER_CURSOR);
	m_oCursorPos.x = -1;    //means: no position   
    m_fDisplayed = false;
    m_fVisible = false;
    SetColour(*wxBLUE);
    m_nBlinkingRate = 750;		//cursor blinking rate = 750ms
}

lmScoreViewCursor::~lmScoreViewCursor()
{
    if (m_oCursorTimer.IsRunning())
        m_oCursorTimer.Stop();
}

void lmScoreViewCursor::OnCursorTimer(wxTimerEvent& event)
{
    if (m_fDisplayed)
    {
        m_locker.Enter();
    	RenderCursor(!m_fVisible);
        m_locker.Leave();
        m_oCursorTimer.Start(m_nBlinkingRate, wxTIMER_ONE_SHOT);
    }
}

void lmScoreViewCursor::RemoveCursor()
{
    //When scrolling and other operations that could affect cursor, it is necessary
    //to ensure that the cursor is not displayed while doing the operation.
    //This method stops the timer and ensures that the cursor is erased

    //stop cursor timer
    m_fDisplayed = false;
    if (m_oCursorTimer.IsRunning())
        m_oCursorTimer.Stop();

    //hide old cursor
    m_locker.Enter();
    RenderCursor(lmHIDDEN);
    m_locker.Leave();

    //remove position (otherwise, Display (in fact, SetCursorPosition) will skip
    //repainting it and the cursor will never get displayed)
	m_oCursorPos.x = -1;

}

void lmScoreViewCursor::DisplayCursor(double rScale, lmUPoint uPos, lmStaff* pStaff)
{
    m_fDisplayed = true;
    m_fVisible = false;
    m_rScale = rScale;
    SetCursorPosition(uPos, pStaff);
}

void lmScoreViewCursor::SetCursorPosition(lmUPoint uPos, lmStaff* pStaff) 
{ 
    //if position doesn't change, return. Nothing to do.
    if (!pStaff || uPos == m_oCursorPos) return;

    if (m_oCursorTimer.IsRunning())
        m_oCursorTimer.Stop();
    m_locker.Enter();
    RenderCursor(lmHIDDEN);     //hide old cursor

    //set new position
    m_oCursorPos = uPos;
    m_oCursorPos.y -= pStaff->TenthsToLogical(10.0);

	m_udyLength = pStaff->TenthsToLogical(60.0);
	m_udxSegment = pStaff->TenthsToLogical(5.0);

    //render it
    RenderCursor(lmVISIBLE);

    m_locker.Leave();
    m_oCursorTimer.Start(m_nBlinkingRate, wxTIMER_ONE_SHOT);
}

void lmScoreViewCursor::RenderCursor(bool fVisible)
{
    //AWARE. This code is execute protected by critical section m_locker to
    //avoid inconsistencies while changing m_fVisible status. This method is
    //the only allowed to change m_fVisible status.

    //if current state == desired state, nothing to do
    if (m_fVisible == fVisible) return;

    //if not yet positioned, finish
	if (m_oCursorPos.x == -1) return;

    m_fVisible = fVisible;       //new status

	// prepare DC
    wxClientDC dc((wxWindow*)m_pCanvas);
	//dc.SetBrush(*wxBLUE_BRUSH);
	int vxlineWidth = 1;
	wxPen pen(m_color, vxlineWidth);
	dc.SetPen(pen);
	dc.SetLogicalFunction(wxXOR);

	//cursor geometry
	lmDPoint pointD;
	lmUPoint cursorPos(m_oCursorPos.x, m_oCursorPos.y);
	((lmScoreView*)m_pView)->LogicalToDevice(cursorPos, pointD);
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

void lmScoreViewCursor::SetColour(wxColour color)
{
    // as painting uses XOR we need the complementary
    m_color = wxColour(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );

}

void lmScoreViewCursor::SetBlinkingRate(int nMillisecs)
{
    m_nBlinkingRate = nMillisecs;
}

