//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
//----------------------------------------------------------------------------
// The main menu is fixed. Child window dosen't change it

//  An MDI child frame is a frame that can only exist on a MDIClientWindow,
//  which is itself a child of MDIParentFrame.
//  It is going to be a notebook page, to contains either a score (lmEditFrame) or
//  the eBookManager (lmTextBookFrame)
//
//  The ChildFrame is created by the aplication (either the doc/view manager, for
//  scores, or lmMainFrame, for the eBookManager) On creation, it must inform to
//  ClientWindow. Is is responsibility of ClientWindow to manage the new Child and
//  take control of it (i.e. add it to the notebook).

//  The ChildFrame is deleted by the application. For scores, it is the doc/view
//  manager, in response to a wxID_CLOSE event. The view deletes the ChildFrame.
//  For eBooks, when the eBooksManager is closed by the MainFrame, it must delete
//  the TextBookManager.
//  The ChildFrame, on its destructor, informs the ClientWindow to manage the
//  Child deletetion (i.e. to remove it form the Notebook).
//
//

//----------------------------------------------------------------------------

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

#include "wx/panel.h"
#include "wx/menu.h"

//-----------------------------------------------------------------------------
// lmMDIChildFrame implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(lmMDIChildFrame, wxPanel)

BEGIN_EVENT_TABLE(lmMDIChildFrame, wxPanel)
    EVT_CLOSE(lmMDIChildFrame::OnCloseWindow)
    EVT_SIZE(lmMDIChildFrame::OnSize)
END_EVENT_TABLE()

lmMDIChildFrame::lmMDIChildFrame()
{
    Init();
}

lmMDIChildFrame::lmMDIChildFrame( lmMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
    Init();

    Create( parent, id, title, wxDefaultPosition, size, style, name );
}

lmMDIChildFrame::~lmMDIChildFrame()
{
    //The Child frame has been deleted by the view.
    //Inform the parent so that it can remove the tab form the Notebook
    lmMDIParentFrame* pParentFrame = GetMDIParentFrame();
    if (pParentFrame)
        pParentFrame->RemoveChildFrame(this);

}

bool lmMDIChildFrame::Create( lmMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
    lmMDIClientWindow* pClientWindow = parent->GetClientWindow();

    wxASSERT_MSG((pClientWindow != (wxWindow*) NULL), wxT("Missing MDI client window.") );

    wxPanel::Create(pClientWindow, id, wxDefaultPosition, size, style, name);

    SetMDIParentFrame(parent);

    //// This is the currently active child
    //parent->SetActiveChild(this);

    m_Title = title;

    pClientWindow->AddPage(this, title, true);
    ApplyMDIChildFrameRect();   // Ok confirme the size change!
    pClientWindow->Refresh();

    return true;
}


void lmMDIChildFrame::SetTitle(const wxString& title)
{
    m_Title = title;

    lmMDIParentFrame *pParentFrame = GetMDIParentFrame();

    if (pParentFrame != NULL)
    {
        lmMDIClientWindow * pClientWindow = pParentFrame->GetClientWindow();

        if (pClientWindow != NULL)
        {
            size_t pos;
            for (pos = 0; pos < pClientWindow->GetPageCount(); pos++)
            {
                if (pClientWindow->GetPage(pos) == this)
                {
                    pClientWindow->SetPageText(pos, m_Title);
                    break;
                }
            }
        }
    }
}

wxString lmMDIChildFrame::GetTitle() const
{
    return m_Title;
}

void lmMDIChildFrame::Activate()
{
    lmMDIParentFrame *pParentFrame = GetMDIParentFrame();

    if (pParentFrame != NULL)
    {
        lmMDIClientWindow * pClientWindow = pParentFrame->GetClientWindow();

        if (pClientWindow != NULL)
        {
            size_t pos;
            for (pos = 0; pos < pClientWindow->GetPageCount(); pos++)
            {
                if (pClientWindow->GetPage(pos) == this)
                {
                    pClientWindow->SetSelection(pos);
                    break;
                }
            }
        }
    }
}

/*** Copied from top level..! ***/
// default resizing behaviour - if only ONE subwindow, resize to fill the
// whole client area
void lmMDIChildFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // if we're using constraints or sizers - do use them
    if ( GetAutoLayout() )
    {
        Layout();
    }
    else
    {
        // do we have _exactly_ one child?
        wxWindow *child = (wxWindow *)NULL;
        for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow *win = node->GetData();

            // exclude top level and managed windows (status bar isn't
            // currently in the children list except under wxMac anyhow, but
            // it makes no harm to test for it)
            if ( !win->IsTopLevel() /*&& !IsOneOfBars(win)*/ )
            {
                if ( child )
                {
                    return;     // it's our second subwindow - nothing to do
                }

                child = win;
            }
        }

        // do we have any children at all?
        if ( child )
        {
            // exactly one child - set it's size to fill the whole frame
            int clientW, clientH;
            DoGetClientSize(&clientW, &clientH);

            // for whatever reasons, wxGTK wants to have a small offset - it
            // probably looks better with it?
#ifdef __WXGTK__
            static const int ofs = 1;
#else
            static const int ofs = 0;
#endif

            child->SetSize(ofs, ofs, clientW - 2*ofs, clientH - 2*ofs);
        }
    }
}

void lmMDIChildFrame::SetIcon(const wxIcon& icon)
{

    lmMDIClientWindow* pNotebook = m_pMDIParentFrame->GetClientWindow();

    //get index to this page
    size_t pos;
    for (pos = 0; pos < pNotebook->GetPageCount(); pos++)
    {
        if (pNotebook->GetPage(pos) == this)
        {
            pNotebook->SetSelection(pos);
            break;
        }
    }
    wxASSERT(pos < pNotebook->GetPageCount());

    //set bitmap
    wxBitmap bitmap;
    bitmap.CopyFromIcon(icon);
    pNotebook->SetPageBitmap(pos, bitmap);

}


/*** Copied from top level..! ***/
// The default implementation for the close window event.
void lmMDIChildFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void lmMDIChildFrame::SetMDIParentFrame(lmMDIParentFrame* parentFrame)
{
    m_pMDIParentFrame = parentFrame;
}

lmMDIParentFrame* lmMDIChildFrame::GetMDIParentFrame() const
{
    return m_pMDIParentFrame;
}

void lmMDIChildFrame::Init()
{
    m_pMDIParentFrame = (lmMDIParentFrame *) NULL;
}

void lmMDIChildFrame::DoMoveWindow(int x, int y, int width, int height)
{
    m_MDIRect = wxRect(x, y, width, height);
}

void lmMDIChildFrame::ApplyMDIChildFrameRect()
{
    wxPanel::DoMoveWindow(m_MDIRect.x, m_MDIRect.y, m_MDIRect.width, m_MDIRect.height);
}

#endif  //lmUSE_NOTEBOOK_MDI
