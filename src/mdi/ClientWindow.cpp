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


#include "ParentFrame.h"
#include "ChildFrame.h"
#include "ClientWindow.h"


#ifndef WX_PRECOMP
    #include "wx/panel.h"
    #include "wx/menu.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif


//-----------------------------------------------------------------------------
// lmMDIClientWindow
//    The client window is the area where MDI child windows exist. It doesn't 
//    have to cover the whole parent frame; other windows such as toolbars and 
//    a help window might coexist with it. There can be scrollbars on a client 
//    window, which are controlled by the parent window style.
//    As client I will use wxAuiNotebook
//
//    The MDIClientWindow object is created by the MDI parent frame, in the 
//    constructor. And is deleted also by MDIParentFrame in its destructor.
//
//-----------------------------------------------------------------------------

#define lmID_NOTEBOOK wxID_HIGHEST + 100

IMPLEMENT_DYNAMIC_CLASS(lmMDIClientWindow, wxAuiNotebook)

BEGIN_EVENT_TABLE(lmMDIClientWindow, wxAuiNotebook)
    EVT_SIZE(lmMDIClientWindow::OnSize)
END_EVENT_TABLE()


lmMDIClientWindow::lmMDIClientWindow()
{
}

lmMDIClientWindow::lmMDIClientWindow( lmMDIParentFrame *parent, long style )
{
    //SetWindowStyleFlag(style);
    wxAuiNotebook::Create(parent, lmID_NOTEBOOK, wxPoint(0,0), 
        wxSize(100, 100), style);
}

lmMDIClientWindow::~lmMDIClientWindow()
{
}

int lmMDIClientWindow::SetSelection(size_t nPage)
{
    int oldSelection = wxAuiNotebook::SetSelection(nPage);
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
}

void lmMDIClientWindow::OnSize(wxSizeEvent& event)
{
    wxAuiNotebook::OnSize(event);

    size_t pos;
    for (pos = 0; pos < GetPageCount(); pos++)
    {
        ((lmMDIChildFrame *)GetPage(pos))->ApplyMDIChildFrameRect();
    }
}

lmMDIChildFrame* lmMDIClientWindow::GetSelectedPage()
{
    if (GetPageCount() > 0)
        return (lmMDIChildFrame*)GetPage(GetSelection());
    else
        return (lmMDIChildFrame*) NULL;
}

#endif  //lmUSE_NOTEBOOK_MDI
