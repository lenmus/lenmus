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

#include "../app/global.h"

#if lmUSE_NOTEBOOK_MDI


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "NotebookMDI.h"

#ifndef WX_PRECOMP
    #include "wx/panel.h"
    #include "wx/menu.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif //WX_PRECOMP

//#include "wx/stockitem.h"

//-----------------------------------------------------------------------------
// lmMDIClientWindow
//    The client window is the area where MDI child windows exist. It doesn't 
//    have to cover the whole parent frame; other windows such as toolbars and 
//    a help window might coexist with it. There can be scrollbars on a client 
//    window, which are controlled by the parent window style.
//    As client I will use wxNotebook
//
//    The MDIClientWindow object is created by the MDI parent frame, in the 
//    constructor. And is deleted also by MDIParentFrame in its destructor.
//
//-----------------------------------------------------------------------------

#define lmID_NOTEBOOK wxID_HIGHEST + 100

IMPLEMENT_DYNAMIC_CLASS(lmMDIClientWindow, wxNotebook)

BEGIN_EVENT_TABLE(lmMDIClientWindow, wxNotebook)
    //EVT_NOTEBOOK_PAGE_CHANGED(lmID_NOTEBOOK, lmMDIClientWindow::OnPageChanged)
    EVT_SIZE(lmMDIClientWindow::OnSize)
END_EVENT_TABLE()


lmMDIClientWindow::lmMDIClientWindow()
{
}

lmMDIClientWindow::lmMDIClientWindow( lmMDIParentFrame *parent, long style )
{
    SetWindowStyleFlag(style);
    wxNotebook::Create(parent, lmID_NOTEBOOK, wxPoint(0,0), 
        wxSize(100, 100), 0);
}

lmMDIClientWindow::~lmMDIClientWindow()
{
    DestroyChildren();
}

int lmMDIClientWindow::SetSelection(size_t nPage)
{
    int oldSelection = wxNotebook::SetSelection(nPage);

#if !defined(__WXMSW__) // No need to do this for wxMSW as wxNotebook::SetSelection()
                        // will already cause this to be done!
    // Handle the page change.
    PageChanged(oldSelection, nPage);
#endif

    return oldSelection;
}

void lmMDIClientWindow::PageChanged(int OldSelection, int newSelection)
{
    // Don't do to much work, only when something realy should change!
    if (OldSelection == newSelection)
        return;
    // Again check if we realy need to do this...
    if (newSelection != -1)
    {
        lmMDIChildFrame* child = (lmMDIChildFrame *)GetPage(newSelection);

        if (child->GetMDIParentFrame()->GetActiveChild() == child)
            return;
    }

    //// Notify old active child that it has been deactivated
    //if (OldSelection != -1)
    //{
    //    lmMDIChildFrame* oldChild = (lmMDIChildFrame *)GetPage(OldSelection);
    //    if (oldChild)
    //    {
    //        wxActivateEvent event(wxEVT_ACTIVATE, false, oldChild->GetId());
    //        event.SetEventObject( oldChild );
    //        oldChild->GetEventHandler()->ProcessEvent(event);
    //    }
    //}

    //// Notify new active child that it has been activated
    //if (newSelection != -1)
    //{
    //    lmMDIChildFrame* activeChild = (lmMDIChildFrame *)GetPage(newSelection);
    //    if (activeChild)
    //    {
    //        wxActivateEvent event(wxEVT_ACTIVATE, true, activeChild->GetId());
    //        event.SetEventObject( activeChild );
    //        activeChild->GetEventHandler()->ProcessEvent(event);

    //        if (activeChild->GetMDIParentFrame())
    //        {
    //            activeChild->GetMDIParentFrame()->SetActiveChild(activeChild);
    //        }
    //    }
    //}
}

void lmMDIClientWindow::OnPageChanged(wxNotebookEvent& event)
{
    PageChanged(event.GetOldSelection(), event.GetSelection());
    event.Skip();
}

void lmMDIClientWindow::OnSize(wxSizeEvent& event)
{
    wxNotebook::OnSize(event);

    size_t pos;
    for (pos = 0; pos < GetPageCount(); pos++)
    {
        ((lmMDIChildFrame *)GetPage(pos))->ApplyMDIChildFrameRect();
    }
}


#endif  //lmUSE_NOTEBOOK_MDI
