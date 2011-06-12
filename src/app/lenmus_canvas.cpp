//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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
//---------------------------------------------------------------------------------------

#include "lenmus_canvas.h"

#include <wx/wxprec.h>
#include <wx/panel.h>
#include <wx/menu.h>

namespace lenmus
{

//=======================================================================================
// ContentFrame implementation
//=======================================================================================

//IMPLEMENT_DYNAMIC_CLASS(ContentFrame, wxFrame)
//
//BEGIN_EVENT_TABLE(ContentFrame, wxFrame)
//    EVT_SIZE(ContentFrame::OnSize)
//END_EVENT_TABLE()


//---------------------------------------------------------------------------------------
ContentFrame::ContentFrame(wxWindow* parent, wxWindowID id, const wxString& title,
                           const wxPoint& pos, const wxSize& size, long style,
                           const wxString& name)
    : wxFrame(parent, id, title, pos, size, style, name)
    , m_pContentWindow(NULL)
{
}

//---------------------------------------------------------------------------------------
ContentFrame::~ContentFrame()
{
}

//---------------------------------------------------------------------------------------
void ContentFrame::add_canvas(Canvas* pCanvas, const wxString& title)
{
    if (m_pContentWindow)
        m_pContentWindow->add_canvas(pCanvas, title);
}

//---------------------------------------------------------------------------------------
Canvas* ContentFrame::get_active_canvas()
{
    if (m_pContentWindow)
        return m_pContentWindow->get_active_canvas();
    else
        return NULL;
}

//void ContentFrame::SetMenuBar(wxMenuBar* pMenuBar)
//{
//    wxFrame::SetMenuBar(pMenuBar);
//}
//
//bool ContentFrame::ProcessEvent(wxEvent& event)
//{
//    /*
//    *  Redirect events to active child first.
//    * /
//
//    // Stops the same event being processed repeatedly
//    static wxEventType inEvent = wxEVT_NULL;
//    if (inEvent == event.GetEventType())
//        return false;
//
//    inEvent = event.GetEventType();
//
//    // Let the active child (if any) process the event first.
//    bool res = false;
//    if (get_active_canvas() && event.IsKindOf(CLASSINFO(wxCommandEvent))
//#if 0
//        /* This is sure to not give problems...* /
//        && (event.GetEventType() == wxEVT_COMMAND_MENU_SELECTED ||
//            event.GetEventType() == wxEVT_UPDATE_UI )
//#else
//        /* This was tested on wxMSW and worked...* /
//        && event.GetEventObject() != m_pContentWindow
//        && !(event.GetEventType() == wxEVT_ACTIVATE ||
//             event.GetEventType() == wxEVT_SET_FOCUS ||
//             event.GetEventType() == wxEVT_KILL_FOCUS ||
//             event.GetEventType() == wxEVT_CHILD_FOCUS ||
//             event.GetEventType() == wxEVT_COMMAND_SET_FOCUS ||
//             event.GetEventType() == wxEVT_COMMAND_KILL_FOCUS )
//#endif
//       )
//    {
//        res = get_active_canvas()->GetEventHandler()->ProcessEvent(event);
//    }
//
//    // If the event was not handled this frame will handle it!
//    if (!res)
//    {
//        res = GetEventHandler()->wxEvtHandler::ProcessEvent(event);
//    }
//
//    inEvent = wxEVT_NULL;
//
//    return res;
//}

//void ContentFrame::ActivateNext()
//{
//    if (m_pContentWindow && m_pContentWindow->GetSelection() != -1)
//    {
//        size_t active = m_pContentWindow->GetSelection() + 1;
//        if (active >= m_pContentWindow->GetPageCount())
//            active = 0;
//
//        m_pContentWindow->SetSelection(active);
//    }
//}
//
//void ContentFrame::ActivatePrevious()
//{
//    if (m_pContentWindow && m_pContentWindow->GetSelection() != -1)
//    {
//        int active = m_pContentWindow->GetSelection() - 1;
//        if (active < 0)
//            active = m_pContentWindow->GetPageCount() - 1;
//
//        m_pContentWindow->SetSelection(active);
//    }
//}
//
//bool ContentFrame::CloseAll()
//{
//    //Returns true if all windows get closed
//
//    int nNumPages = (int)m_pContentWindow->GetPageCount();
//    if (nNumPages == 0) return true;     //nothing to close.
//    int iActive = m_pContentWindow->GetSelection();
//
//    //loop to close all pages but not the active one. This is to avoid having
//    //to activate (and repaint) a new page if we close the current active one.
//    bool fAllClosed = true;     //assume it
//    for (int i=nNumPages-1; i >= 0; i--)
//    {
//        if (i != iActive)
//        {
//            bool fClosed = m_pContentWindow->GetPage(i)->Close();
//            if (fClosed)
//                m_pContentWindow->RemovePage(i);
//            fAllClosed &= fClosed;
//        }
//    }
//
//    //Now the only remaining page is the active one. Close it.
//    iActive = m_pContentWindow->GetSelection();
//    bool fClosed = m_pContentWindow->GetPage(iActive)->Close();
//    if (fClosed)
//        m_pContentWindow->RemovePage(iActive);
//    fAllClosed &= fClosed;
//
//    return fAllClosed;
//}
//
//void ContentFrame::CloseActive()
//{
//    if(!m_pContentWindow) return;
//    int iActive = m_pContentWindow->GetSelection();
//    m_pContentWindow->GetPage(iActive)->Close();
//    m_pContentWindow->RemovePage(iActive);
//
//}
//

//void ContentFrame::OnSize(wxSizeEvent& event)
//{
//    if (m_pContentWindow)
//        m_pContentWindow->OnSize(event);
//    else
//        event.Skip();      //continue processing the  event
//}
//
//void ContentFrame::DoGetClientSize(int* width, int* height) const
//{
//    wxFrame::DoGetClientSize( width, height );
//}



//=======================================================================================
// ContentWindow implementation
//=======================================================================================

//#define lmID_NOTEBOOK wxID_HIGHEST + 100
//
//IMPLEMENT_DYNAMIC_CLASS(ContentWindow, wxAuiNotebook)
//
//BEGIN_EVENT_TABLE(ContentWindow, wxAuiNotebook)
//    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, ContentWindow::OnChildClose)
//    EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, ContentWindow::OnPageChanged)
//END_EVENT_TABLE()
//

ContentWindow::ContentWindow(ContentFrame* parent, long style)
    : wxAuiNotebook(parent, -1 /*lmID_NOTEBOOK*/ , wxDefaultPosition,
                    wxDefaultSize, style)
{
}

//---------------------------------------------------------------------------------------
ContentWindow::~ContentWindow()
{
}

//---------------------------------------------------------------------------------------
void ContentWindow::add_canvas(Canvas* pCanvas, const wxString& title)
{
    AddPage(pCanvas, title, true);
    Refresh();
}

//---------------------------------------------------------------------------------------
Canvas* ContentWindow::get_active_canvas()
{
    if (GetPageCount() > 0)
        return dynamic_cast<Canvas*>( GetPage(GetSelection()) );
    else
        return NULL;
}

//int ContentWindow::SetSelection(size_t nPage)
//{
//    int oldSelection = wxAuiNotebook::SetSelection(nPage);
//    return oldSelection;
//}

//void ContentWindow::OnPageChanged(wxAuiNotebookEvent& event)
//{
//	int OldSelection = event.GetOldSelection();
//	int newSelection = event.GetSelection();
//    if (OldSelection == newSelection) return;		//nothing to do
//    if (newSelection != -1)
//    {
//        Canvas* child;
//        if (OldSelection != -1)
//        {
//            child = (Canvas*)GetPage(OldSelection);
//            child->OnChildFrameDeactivated();
//        }
//        child = (Canvas*)GetPage(newSelection);
//		child->OnChildFrameActivated();
//    }
//}
//
//void ContentWindow::OnChildClose(wxAuiNotebookEvent& evt)
//{
//    //Do not allow direct closing of Canvas by wxAuiNotebook as it deletes
//    //the child frames and this causes problems with the view/doc model.
//    //So lets veto page closing and proceed to a controlled close.
//    evt.Veto();
//
//    //proceed to a controlled close
//    int iPage = GetSelection();
//    GetPage(iPage)->Close();
//    RemovePage(iPage);
//
//}



//=======================================================================================
// Canvas implementation
//=======================================================================================

//IMPLEMENT_DYNAMIC_CLASS(Canvas, wxPanel)

//BEGIN_EVENT_TABLE(Canvas, wxPanel)
//    EVT_SIZE(Canvas::OnSize)
//END_EVENT_TABLE()

//---------------------------------------------------------------------------------------
Canvas::Canvas(ContentFrame* parent,
               wxWindowID id, const wxString& title,
               const wxPoint& WXUNUSED(pos), const wxSize& size,
               long style, const wxString& name )
    : m_pContentFrame(parent)
{
    ContentWindow* pContentWindow = parent->get_content_window();
    wxPanel::Create(pContentWindow, id, wxDefaultPosition, size, style, name);
    set_content_frame(parent);
    m_Title = title;
    parent->add_canvas(this, title);
}

//---------------------------------------------------------------------------------------
Canvas::~Canvas()
{
//    //The Child frame has been deleted by the view.
//    //Inform the parent so that it can remove the tab form the Notebook
//    ContentFrame* pParentFrame = get_content_frame();
//    if (pParentFrame)
//        pParentFrame->remove_canvas(this);
//
}


//void Canvas::SetTitle(const wxString& title)
//{
//    m_Title = title;
//
//    ContentFrame* pParentFrame = get_content_frame();
//
//    if (pParentFrame != NULL)
//    {
//        ContentWindow*  pClientWindow = pParentFrame->get_content_window();
//
//        if (pClientWindow != NULL)
//        {
//            size_t pos;
//            for (pos = 0; pos < pClientWindow->GetPageCount(); pos++)
//            {
//                if (pClientWindow->GetPage(pos) == this)
//                {
//                    pClientWindow->SetPageText(pos, m_Title);
//                    break;
//                }
//            }
//        }
//    }
//}
//
//wxString Canvas::GetTitle() const
//{
//    return m_Title;
//}
//
//void Canvas::Activate()
//{
//    ContentFrame* pParentFrame = get_content_frame();
//
//    if (pParentFrame != NULL)
//    {
//        ContentWindow*  pClientWindow = pParentFrame->get_content_window();
//
//        if (pClientWindow != NULL)
//        {
//            size_t pos;
//            for (pos = 0; pos < pClientWindow->GetPageCount(); pos++)
//            {
//                if (pClientWindow->GetPage(pos) == this)
//                {
//                    pClientWindow->SetSelection(pos);
//                    break;
//                }
//            }
//        }
//    }
//}
//

}   //namespace lenmus
