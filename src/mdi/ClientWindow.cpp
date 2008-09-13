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
#pragma implementation "ClientWindow.h"
#endif

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
    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, lmMDIClientWindow::OnChildClose)
    EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, lmMDIClientWindow::OnPageChanged)
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

void lmMDIClientWindow::OnPageChanged(wxAuiNotebookEvent& event)
{
	int OldSelection = event.GetOldSelection();
	int newSelection = event.GetSelection();
    if (OldSelection == newSelection) return;		//nothing to do
    if (newSelection != -1)
    {
        lmMDIChildFrame* child;
        if (OldSelection != -1)
        {
            child = (lmMDIChildFrame*)GetPage(OldSelection);
            child->OnChildFrameDeactivated();
        }
        child = (lmMDIChildFrame*)GetPage(newSelection);
		child->OnChildFrameActivated();
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

void lmMDIClientWindow::OnChildClose(wxAuiNotebookEvent& evt)
{    
    //Do not allow direct closing of lmMDIChildFrame by wxAuiNotebook as it deletes
    //the child frames and this causes problems with the view/doc model.
    //So lets veto page closing and proceed to a controlled close.
    evt.Veto();

    //proceed to a controlled close
    int iPage = GetSelection();
    GetPage(iPage)->Close();
    RemovePage(iPage);

}
