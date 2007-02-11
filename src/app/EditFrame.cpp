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
/*! @file EditFrame.cpp
    @brief Implementation file for class lmEditFrame
    @ingroup app_gui
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

#include "global.h"
#include "TheApp.h"
#include "MainFrame.h"
#include "ScoreDoc.h"
#include "scoreView.h"
#include "EditFrame.h"

#include "global.h"

#include "MainFrame.h"
extern lmMainFrame* g_pMainFrame;



BEGIN_EVENT_TABLE(lmEditFrame, lmDocMDIChildFrame)
  EVT_SIZE      (lmEditFrame::OnSize)
  EVT_CLOSE     (lmEditFrame::OnClose)
END_EVENT_TABLE()


IMPLEMENT_CLASS(lmEditFrame, lmDocMDIChildFrame)


lmEditFrame::lmEditFrame(wxDocument* doc, wxView* view, lmMainFrame *mainFrame,
                     const wxPoint& pos, const wxSize& size) :
    lmDocMDIChildFrame(doc, view, mainFrame, wxID_ANY, _T("EditFrame"), pos, size, 
                        wxDEFAULT_FRAME_STYLE ) //| wxNO_FULL_REPAINT_ON_RESIZE)
{
    m_pView = (lmScoreView *) view;

}

void lmEditFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    //inform the view
    m_pView->ResizeControls();
}

void lmEditFrame::OnClose(wxCloseEvent& event)
{
    //just in case the score is being played back, to avoid a crash
    m_pView->StopPlaying(true); //true -> wait for termination
    event.Skip();               //continue normal processing of the OnClose event

}

