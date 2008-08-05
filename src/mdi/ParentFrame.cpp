//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This file is derived from file src/generic/mdig.cpp from wxWidgets 2.7.1 project.
//    Author:       Hans Van Leemputten
//    Copyright (c) Hans Van Leemputten
// 
//    Modified by:
//        Cecilio Salmeron
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
#pragma implementation "ParentFrame.h"
#endif

#include "../app/global.h"

#if lmUSE_NOTEBOOK_MDI

//----------------------------------------------------------------------------
// The main menu is fixed. Child window dosen't change it



//----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "ParentFrame.h"
#include "ChildFrame.h"
#include "ClientWindow.h"

//-----------------------------------------------------------------------------
// lmMDIParentFrame
//  An MDI (Multiple Document Interface) parent frame is a window which can 
//  contain MDI child frames in its own 'desktop'. It is a convenient way to 
//  avoid window clutter.
//  An MDI parent frame always has a MDIClientWindow associated with it, 
//  which is the parent for MDI child frames. This client window may be resized 
//  to accommodate non-MDI windows, as seen in Microsoft Visual C++ (TM) and 
//  Microsoft Publisher (TM), where a documentation window is placed to one 
//  side of the workspace.


//
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(lmMDIParentFrame, wxFrame)

BEGIN_EVENT_TABLE(lmMDIParentFrame, wxFrame)
    EVT_SIZE(lmMDIParentFrame::OnSize)
END_EVENT_TABLE()


lmMDIParentFrame::lmMDIParentFrame()
{
    Init();
}

lmMDIParentFrame::lmMDIParentFrame(wxWindow *parent,
                                   wxWindowID id,
                                   const wxString& title,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name)
{
    Init();

    (void)Create(parent, id, title, pos, size, style, name);
}

lmMDIParentFrame::~lmMDIParentFrame()
{
//    // Make sure the client window is destructed before the menu bars are!
//    //wxDELETE(m_pClientWindow);
//    if (m_pClientWindow) delete m_pClientWindow;
//
//#if wxUSE_MENUS
//    if (m_pMyMenuBar)
//    {
//        delete m_pMyMenuBar;
//        m_pMyMenuBar = (wxMenuBar *) NULL;
//    }
//
//
//#endif // wxUSE_MENUS
}

bool lmMDIParentFrame::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& title,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    //creates the parent frame
    wxFrame::Create( parent, id, title, pos, size, style, name );

    ////creates the client window on it
    m_pClientWindow  = (lmMDIClientWindow*) NULL;
    //m_pClientWindow = new lmMDIClientWindow( this, wxVSCROLL | wxHSCROLL );
    return true;
}

void lmMDIParentFrame::SetMenuBar(wxMenuBar *pMenuBar)
{
    wxFrame::SetMenuBar(pMenuBar);
}

bool lmMDIParentFrame::ProcessEvent(wxEvent& event)
{
    /*
     * Redirect events to active child first.
     */

    // Stops the same event being processed repeatedly
    static wxEventType inEvent = wxEVT_NULL;
    if (inEvent == event.GetEventType())
        return false;

    inEvent = event.GetEventType();

    // Let the active child (if any) process the event first.
    bool res = false;
    if (GetActiveChild() && event.IsKindOf(CLASSINFO(wxCommandEvent))
#if 0
        /* This is sure to not give problems... */
        && (event.GetEventType() == wxEVT_COMMAND_MENU_SELECTED ||
            event.GetEventType() == wxEVT_UPDATE_UI )
#else
        /* This was tested on wxMSW and worked... */
        && event.GetEventObject() != m_pClientWindow
        && !(event.GetEventType() == wxEVT_ACTIVATE ||
             event.GetEventType() == wxEVT_SET_FOCUS ||
             event.GetEventType() == wxEVT_KILL_FOCUS ||
             event.GetEventType() == wxEVT_CHILD_FOCUS ||
             event.GetEventType() == wxEVT_COMMAND_SET_FOCUS ||
             event.GetEventType() == wxEVT_COMMAND_KILL_FOCUS )
#endif
       )
    {
        res = GetActiveChild()->GetEventHandler()->ProcessEvent(event);
    }

    // If the event was not handled this frame will handle it!
    if (!res)
    {
        res = GetEventHandler()->wxEvtHandler::ProcessEvent(event);
    }

    inEvent = wxEVT_NULL;

    return res;
}

lmMDIChildFrame *lmMDIParentFrame::GetActiveChild() const
{
    if (m_pClientWindow)
        return m_pClientWindow->GetSelectedPage();
    else
        return (lmMDIChildFrame*)NULL;
}

void lmMDIParentFrame::SetActiveChild(lmMDIChildFrame* pChildFrame)
{
    //m_pActiveChild = pChildFrame;
}

lmMDIClientWindow *lmMDIParentFrame::GetClientWindow() const
{
    return m_pClientWindow;
}

void lmMDIParentFrame::ActivateNext()
{
    if (m_pClientWindow && m_pClientWindow->GetSelection() != -1)
    {
        size_t active = m_pClientWindow->GetSelection() + 1;
        if (active >= m_pClientWindow->GetPageCount())
            active = 0;

        m_pClientWindow->SetSelection(active);
    }
}

void lmMDIParentFrame::ActivatePrevious()
{
    if (m_pClientWindow && m_pClientWindow->GetSelection() != -1)
    {
        int active = m_pClientWindow->GetSelection() - 1;
        if (active < 0)
            active = m_pClientWindow->GetPageCount() - 1;

        m_pClientWindow->SetSelection(active);
    }
}

bool lmMDIParentFrame::CloseAll()
{
    //Returns true if all windows get closed

    int nNumPages = (int)m_pClientWindow->GetPageCount();
    if (nNumPages == 0) return true;     //nothing to close. 
    int iActive = m_pClientWindow->GetSelection();

    //loop to close all pages but not the active one. This is to avoid having
    //to activate (and repaint) a new page if we close the current active one.
    bool fAllClosed = true;     //assume it
    for (int i=nNumPages-1; i >= 0; i--) {
        if (i != iActive) {
            bool fClosed = m_pClientWindow->GetPage(i)->Close();
            if (fClosed)
                m_pClientWindow->RemovePage(i);
            fAllClosed &= fClosed;
        }
    }

    //Now the only remaining page is the active one. Close it.
    iActive = m_pClientWindow->GetSelection();
    bool fClosed = m_pClientWindow->GetPage(iActive)->Close();
    if (fClosed)
        m_pClientWindow->RemovePage(iActive);
    fAllClosed &= fClosed;

    return fAllClosed;
}

void lmMDIParentFrame::CloseActive()
{
    if(!m_pClientWindow) return;
    int iActive = m_pClientWindow->GetSelection();
    m_pClientWindow->GetPage(iActive)->Close();
    m_pClientWindow->RemovePage(iActive);

}


void lmMDIParentFrame::Init()
{
    m_pClientWindow = (lmMDIClientWindow *) NULL;
#if wxUSE_MENUS
    m_pMyMenuBar = (wxMenuBar*) NULL;
#endif // wxUSE_MENUS
}

void lmMDIParentFrame::OnSize(wxSizeEvent& event)
{
    if (m_pClientWindow)
        m_pClientWindow->OnSize(event);
    else
        event.Skip();      //continue processing the  event
}

void lmMDIParentFrame::DoGetClientSize(int *width, int *height) const
{
    wxFrame::DoGetClientSize( width, height );
}

void lmMDIParentFrame::RemoveChildFrame(lmMDIChildFrame* pChild)
{
    //This code is no longer needed as wxAuiNotebook manages the removal of a tab
    //So here nothing else must be done

    ////Check if this child was the active one
    //bool fIsActive = (GetActiveChild() == pChild);
    //
    ////locate the page position
    //int nPos;
    //if (fIsActive)
    //    nPos = (int)m_pClientWindow->GetSelection();
    //else {
    //    for (nPos = 0; nPos < (int)m_pClientWindow->GetPageCount(); nPos++) {
    //        if (m_pClientWindow->GetPage(nPos) == pChild) break;    //found.
    //    }
    //    wxASSERT(nPos < (int)m_pClientWindow->GetPageCount());
    //}

    ////delete the page
    //m_pClientWindow->RemovePage(nPos);
    ////m_pClientWindow->Refresh();

    ////if the deleted page was the active one, select the next one as active
    //int nNextPage = -1;
    //if (fIsActive) {
    //    // If there are more, chose the next page
    //    int nNumPages = (int)m_pClientWindow->GetPageCount();
    //    if (nNumPages > nPos) {
    //        nNextPage = nPos;
    //    }
    //    // If there are pages, choose the previous one
    //    else if (nNumPages != 0) {
    //        nNextPage = nNumPages - 1;
    //    }
    //}

    //// if there are still pages, activate the choosen one
    //if (nNextPage != -1) {
    //    m_pClientWindow->SetSelection(nNextPage);
    //    //SetActiveChild( (lmMDIChildFrame*)m_pClientWindow->GetCurrentPage() );
    //}
    ////else {
    ////    SetActiveChild( (lmMDIChildFrame*) NULL );
    ////}

}

#endif  //lmUSE_NOTEBOOK_MDI
