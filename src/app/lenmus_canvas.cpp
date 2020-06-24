//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
#include <wx/msgdlg.h>
#include <wx/log.h>

namespace lenmus
{

//=======================================================================================
// ContentFrame implementation
//=======================================================================================
ContentFrame::ContentFrame(wxWindow* parent, wxWindowID id, const wxString& title,
                           const wxPoint& pos, const wxSize& size, long style,
                           const wxString& name)
    : wxFrame(parent, id, title, pos, size, style, name)
    , m_pContentWindow(nullptr)
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
        return nullptr;
}

//---------------------------------------------------------------------------------------
int ContentFrame::get_canvas_index(Canvas* pCanvas)
{
    if (m_pContentWindow)
        return m_pContentWindow->get_canvas_index(pCanvas);
    else
        return wxNOT_FOUND;
}

//---------------------------------------------------------------------------------------
Canvas* ContentFrame::get_canvas_for_index(int iCanvas) const
{
    if (m_pContentWindow)
        return m_pContentWindow->get_canvas(iCanvas);
    else
        return nullptr;
}

//---------------------------------------------------------------------------------------
void ContentFrame::close_all()
{
    if (m_pContentWindow)
        m_pContentWindow->close_all();
}

//int wxAuiSimpleTabArt::ShowDropDown(wxWindow* wnd,
//                                    const wxAuiNotebookPageArray& pages,
//                                    int active_idx)
//{
//    wxMenu menuPopup;
//
//    size_t i, count = pages.GetCount();
//    for (i = 0; i < count; ++i)
//    {
//        const wxAuiNotebookPage& page = pages.Item(i);
//        menuPopup.AppendCheckItem(1000+i, page.caption);
//    }
//
//    if (active_idx != -1)
//    {
//        menuPopup.Check(1000+active_idx, true);
//    }



//=======================================================================================
// ContentWindow implementation
//=======================================================================================

const int k_id_notebook = wxNewId();

wxBEGIN_EVENT_TABLE(ContentWindow, wxAuiNotebook)
    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, ContentWindow::on_window_closing)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
ContentWindow::ContentWindow(ContentFrame* parent, long style)
    : wxAuiNotebook(parent, k_id_notebook, wxDefaultPosition, wxDefaultSize, style)
{
}

//---------------------------------------------------------------------------------------
ContentWindow::~ContentWindow()
{
}

//---------------------------------------------------------------------------------------
void ContentWindow::add_canvas(Canvas* pCanvas, const wxString& title)
{
    AddPage(pCanvas, title, true /*change to this new page*/);
    //wxLogMessage("[ContentWindow::add_canvas] canvas=%.08x, title=%s",
    //             pCanvas, title.wx_str());
}

//---------------------------------------------------------------------------------------
Canvas* ContentWindow::get_active_canvas()
{
    int iActive = GetSelection();
    if (iActive != -1)
        return dynamic_cast<Canvas*>( GetPage(iActive) );
    else
        return nullptr;
}

//---------------------------------------------------------------------------------------
Canvas* ContentWindow::get_canvas(int iCanvas) const
{
    return dynamic_cast<Canvas*>( GetPage(iCanvas) );
}

//---------------------------------------------------------------------------------------
int ContentWindow::get_canvas_index(Canvas* pCanvas)
{
    return GetPageIndex(pCanvas);
}

//---------------------------------------------------------------------------------------
void ContentWindow::close_all()
{
    if (GetPageCount() == 0) return;     //nothing to close.

    int iActive = GetSelection();
    while(iActive != -1)
    {
        DeletePage(iActive);
        iActive = GetSelection();
    }
}

//---------------------------------------------------------------------------------------
void ContentWindow::close_active_canvas()
{
    int iActive = GetSelection();
    if (iActive != -1)
        DeletePage(iActive);
}

//---------------------------------------------------------------------------------------
void ContentWindow::on_window_closing(wxAuiNotebookEvent& WXUNUSED(event))
{
    Canvas* pCanvas = get_active_canvas();
    if (pCanvas)
        pCanvas->Close();       //this triggers a wxCloseEvent to allow for
                                //checking if need to save the Document
//wxMessageBox("Closing");
//event.Veto();
}



//=======================================================================================
// CanvasInterface implementation
//=======================================================================================
void CanvasInterface::set_title(wxWindow* pWnd, const wxString& title)
{
    m_title = title;

    if (m_pClientWindow != nullptr)
    {
        size_t pos;
        for (pos = 0; pos < m_pClientWindow->GetPageCount(); pos++)
        {
            if (m_pClientWindow->GetPage(pos) == pWnd)
            {
                m_pClientWindow->SetPageText(pos, m_title);
                break;
            }
        }
    }
}


}   //namespace lenmus
