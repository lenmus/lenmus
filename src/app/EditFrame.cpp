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
#pragma implementation "EditFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "global.h"
#include "TheApp.h"
#include "MainFrame.h"
#include "ScoreDoc.h"
#include "ScoreView.h"
#include "EditFrame.h"

#include "global.h"

#include "MainFrame.h"
extern lmMainFrame* g_pMainFrame;



BEGIN_EVENT_TABLE(lmEditFrame, lmDocMDIChildFrame)
	EVT_SIZE		(lmEditFrame::OnSize)
	EVT_CLOSE		(lmEditFrame::OnClose)
	EVT_CHAR        (lmEditFrame::OnKeyPress) 
END_EVENT_TABLE()


IMPLEMENT_CLASS(lmEditFrame, lmDocMDIChildFrame)


lmEditFrame::lmEditFrame(wxDocument* doc, wxView* view, lmMainFrame *pMainFrame,
                     const wxPoint& uPos, const wxSize& size) :
    lmDocMDIChildFrame(doc, view, pMainFrame, wxID_ANY, _T("EditFrame"), uPos, size,
                        wxDEFAULT_FRAME_STYLE ), m_pView((lmScoreView *)view)
{
}

void lmEditFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    //inform the view
    m_pView->ResizeControls();
}

void lmEditFrame::OnClose(wxCloseEvent& event)
{
    //just in case the score is being played back, to avoid a crash
    m_pView->GetController()->StopPlaying(true); //true -> wait for termination
    event.Skip();       //continue normal processing of the OnClose event

}

double lmEditFrame::GetActiveViewScale()
{
	return m_pView->GetScale();
}

bool lmEditFrame::SetActiveViewScale(double rScale)
{
    m_pView->SetScale(rScale);
	return true;
}

void lmEditFrame::OnChildFrameActivated()
{
	//this frame is now the active frame. Inform main frame.
	g_pMainFrame->OnActiveChildChanged(this);
}

void lmEditFrame::OnChildFrameDeactivated()
{
    //this frame is going to be inactivated. Stop the score if playing back
    m_pView->GetController()->StopPlaying(true);     //true -> wait for termination
}


void lmEditFrame::OnKeyPress(wxKeyEvent& event)
{
    //route event to the controller
    m_pView->GetController()->OnKeyPress(event);
}

