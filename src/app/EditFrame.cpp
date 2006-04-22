// RCS-ID: $Id: EditFrame.cpp,v 1.4 2006/02/23 19:16:31 cecilios Exp $
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



BEGIN_EVENT_TABLE(lmEditFrame, wxDocMDIChildFrame)
  EVT_SIZE(lmEditFrame::OnSize)
  EVT_ACTIVATE(lmEditFrame::OnActivate)
END_EVENT_TABLE()


IMPLEMENT_CLASS(lmEditFrame, wxDocMDIChildFrame)


lmEditFrame::lmEditFrame(wxDocument* doc, wxView* view, lmMainFrame *mainFrame,
                     wxPoint& pos, wxSize& size) :
    wxDocMDIChildFrame(doc, view, mainFrame, wxID_ANY, _T("Child Frame"), pos, size, 
                        wxDEFAULT_FRAME_STYLE ) //| wxNO_FULL_REPAINT_ON_RESIZE)
{
    m_pView = (lmScoreView *) view;

}

void lmEditFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    //inform the view
    m_pView->ResizeControls();
}

void lmEditFrame::OnActivate(wxActivateEvent& event)
{
    //this window is being activated/deactivated
    //Inform the parent frame to update menu and toolbar
    //wxLogMessage(_T("[lmEditFrame::OnActivate] event.GetActive = %s"), (event.GetActive() ? _T("True") : _T("false")));
    g_pMainFrame->UpdateMenuAndToolbar();

}

