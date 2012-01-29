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

//lenmus
#include "lenmus_document_frame.h"

#include "lenmus_canvas.h"
#include "lenmus_document_canvas.h"
#include "lenmus_string.h"
#include "lenmus_midi_server.h"
#include "lenmus_dyncontrol.h"
#include "lenmus_standard_header.h"
#include "lenmus_book_reader.h"
#include "lenmus_content_box.h"
#include "lenmus_content_box_ctrol.h"
#include "lenmus_zip_reader.h"

//lomse
#include <lomse_shapes.h>

//wxWidgets
#include <wx/filename.h>


//toc
#include <wx/textctrl.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <wx/html/htmlwin.h>
#include <wx/busyinfo.h>
#include <wx/progdlg.h>
#include <wx/toolbar.h>
#include <wx/fontenum.h>
#include <wx/stream.h>
#include <wx/filedlg.h>
#include <wx/artprov.h>
#include <wx/spinctrl.h>
#include <wx/dynarray.h>
#include <wx/choicdlg.h>
#include <wx/settings.h>



namespace lenmus
{
//--------------------------------------------------------------------------
// TextBookHelpTreeItemData (private)
//--------------------------------------------------------------------------

class TextBookHelpTreeItemData : public wxTreeItemData
{
    public:
        TextBookHelpTreeItemData(int id) : wxTreeItemData()
            { m_Id = id;}

        int m_Id;
};


//--------------------------------------------------------------------------
// TextBookHelpHashData (private)
//--------------------------------------------------------------------------

class TextBookHelpHashData : public wxObject
{
    public:
        TextBookHelpHashData(int index, long id) : wxObject()
            { m_Index = index; m_Id = id;}
        ~TextBookHelpHashData() {}

        int m_Index;
        long m_Id;
};


//---------------------------------------------------------------------------
// DocumentFrame::m_mergedIndex
//---------------------------------------------------------------------------

WX_DEFINE_ARRAY_PTR(const BookIndexItem*, wxHtmlHelpDataItemPtrArray);

struct TextBookHelpMergedIndexItem
{
    TextBookHelpMergedIndexItem *parent;
    wxString                   name;
    wxHtmlHelpDataItemPtrArray items;
};

WX_DECLARE_OBJARRAY(TextBookHelpMergedIndexItem, TextBookHelpMergedIndex);
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(TextBookHelpMergedIndex)

//void DocumentFrame::UpdateMergedIndex()
//{
//    // Updates "merged index" structure that combines indexes of all books
//    // into better searchable structure
//
//    delete m_mergedIndex;
//    m_mergedIndex = LENMUS_NEW TextBookHelpMergedIndex;
//    TextBookHelpMergedIndex& merged = *m_mergedIndex;
//
//    const BookIndexArray& items = m_pBookReader->GetIndexArray();
//    size_t len = items.size();
//
//    TextBookHelpMergedIndexItem *history[128] = {NULL};
//
//    for (size_t i = 0; i < len; i++)
//    {
//        const BookIndexItem* pItem = items[i];
//        wxASSERT_MSG( pItem->level < 128, _T("nested index entries too deep") );
//
//        if (history[pItem->level] &&
//            history[pItem->level]->items[0]->title == pItem->title)
//        {
//            // same index entry as previous one, update list of items
//            history[pItem->level]->items.Add(pItem);
//        }
//        else
//        {
//            // LENMUS_NEW index entry
//            TextBookHelpMergedIndexItem *mi = LENMUS_NEW TextBookHelpMergedIndexItem();
//            mi->name = pItem->GetIndentedName();
//            mi->items.Add(pItem);
//            mi->parent = (pItem->level == 0) ? NULL : history[pItem->level - 1];
//            history[pItem->level] = mi;
//            merged.Add(mi);
//        }
//    }
//}

//=======================================================================================
// DocumentLoader implementation
//=======================================================================================
DocumentLoader::DocumentLoader(ContentWindow* parent, ApplicationScope& appScope,
                               LomseDoorway& lomse)
    : m_pContentWindow(parent)
    , m_appScope(appScope)
    , m_lomse(lomse)
{
}

//---------------------------------------------------------------------------------------
CanvasInterface* DocumentLoader::create_canvas(const string& filename, int viewType)
{
    //use filename (without path) as page title
    wxFileName document( to_wx_string(filename) );
    bool fIsBook = (document.GetExt().Upper() == _T("LMB"));

    if (fIsBook)
    {
        DocumentFrame* pCanvas = LENMUS_NEW DocumentFrame(m_pContentWindow, m_appScope, m_lomse);
        m_pContentWindow->add_canvas(pCanvas, document.GetName());
        pCanvas->display_document(filename, viewType);
        return pCanvas;
    }
    else
    {
        DocumentCanvas* pCanvas = LENMUS_NEW DocumentCanvas(m_pContentWindow, m_appScope, m_lomse);
        m_pContentWindow->add_canvas(pCanvas, document.GetName());
        pCanvas->display_document(filename, viewType);
        return pCanvas;
    }
}


//=======================================================================================
// DocumentFrame implementation
//=======================================================================================

BEGIN_EVENT_TABLE(DocumentFrame, wxSplitterWindow)
    EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, DocumentFrame::on_splitter_moved)
    LM_EVT_PAGE_REQUEST(DocumentFrame::on_page_change_requested)
END_EVENT_TABLE()

DocumentFrame::DocumentFrame(ContentWindow* parent, ApplicationScope& appScope,
                             LomseDoorway& lomse)
    : wxSplitterWindow(parent, wxNewId(), wxDefaultPosition,
                       wxDefaultSize, 0, _T("Canvas"))
    , CanvasInterface(parent)
    , m_appScope(appScope)
    , m_lomse(lomse)
    , m_left(NULL)
    , m_right(NULL)
    , m_pBookReader(NULL)
    , m_UpdateContents(true)
{
}

//---------------------------------------------------------------------------------------
DocumentFrame::~DocumentFrame()
{
    delete m_pBookReader;
    delete m_left;
    delete m_right;
}

////---------------------------------------------------------------------------------------
//void DocumentFrame::on_show_toc(wxCommandEvent& WXUNUSED(event) )
//{
//    if (IsSplit())
//        Unsplit();
//    m_left->Show(true);
//    m_right->Show(true);
//    SplitVertically(m_left, m_right, 100);
//}
//
////---------------------------------------------------------------------------------------
//void DocumentFrame::on_hide_toc(wxCommandEvent& WXUNUSED(event) )
//{
//    if (IsSplit())
//        Unsplit();
//}

//---------------------------------------------------------------------------------------
void DocumentFrame::on_page_change_requested(PageRequestEvent& event)
{
    wxString url = to_wx_string(event.get_url());
    change_to_page(url);
}

//---------------------------------------------------------------------------------------
void DocumentFrame::create_toc_pane()
{
    m_left = LENMUS_NEW BookContentBox(this, this, m_appScope, wxID_ANY, wxDefaultPosition,
                                wxDefaultSize, 0, _T("The TOC"));
    m_left->CreateContents(m_pBookReader);
}

//---------------------------------------------------------------------------------------
void DocumentFrame::create_content_pane(const string& filename)
{
    m_right = LENMUS_NEW DocumentWindow(this, m_appScope, m_lomse);
    load_page(filename);
}

//---------------------------------------------------------------------------------------
void DocumentFrame::create_content_pane(int iTocItem)
{
    m_right = LENMUS_NEW DocumentWindow(this, m_appScope, m_lomse);
    load_page(iTocItem);
}

//---------------------------------------------------------------------------------------
ImoScore* DocumentFrame::get_active_score()
{
    if (m_right)
        return m_right->get_active_score();
    else
        return NULL;
}

//---------------------------------------------------------------------------------------
Interactor* DocumentFrame::get_interactor()
{
    if (m_right)
        return m_right->get_interactor();
    else
        return NULL;
}

//---------------------------------------------------------------------------------------
void DocumentFrame::display_document(const string& filename, int viewType)
{
    wxFileName document( to_wx_string(filename) );
    bool fIsBook = (document.GetExt().Upper() == _T("LMB"));

    m_bookPath = document.GetFullPath();

   if (fIsBook)
    {
        //read book (.lmb)
        m_pBookReader = LENMUS_NEW BookReader();
        if (!m_pBookReader->AddBook(document))
        {
            //TODO better error handling
            wxMessageBox(wxString::Format(_("Failed adding book %s"),
                document.GetFullPath().c_str() ));
            return;
        }
        create_toc_pane();
        create_content_pane(0);

        m_left->Show(true);

        //split at 25%/75%
        wxSize size = this->GetClientSize();
        int leftPixels = size.GetWidth() / 4;
        SplitVertically(m_left, m_right, leftPixels);
    }
    else
    {
        //read page (.lms)
        create_content_pane(filename);
        Initialize(m_right);
    }
}

//---------------------------------------------------------------------------------------
void DocumentFrame::on_hyperlink_event(SpEventInfo pEvent)
{
    SpEventMouse pEv = boost::static_pointer_cast<EventMouse>(pEvent);
    ImoLink* pLink = static_cast<ImoLink*>( pEv->get_imo_object() );
    wxString url = to_wx_string( pLink->get_url() );

    wxString pagename;
    if (url.StartsWith(_T("#LenMusPage/"), &pagename))
        change_to_page(pagename);
    else
        wxMessageBox(url);
}

//---------------------------------------------------------------------------------------
void DocumentFrame::change_to_page(wxString& pagename)
{
    wxString fullpath = m_pBookReader->FindPageByName(pagename);
    if (fullpath != _T(""))
    {
        load_page( to_std_string(fullpath) );
        m_right->Refresh(false /* don't erase background */);
    }
    else
    {
        wxString msg = wxString::Format(_T("Invalid link='%s'\nPage='%s' not found."),
                                        pagename.c_str(), fullpath.c_str() );
        wxMessageBox(msg);
    }
}

//---------------------------------------------------------------------------------------
void DocumentFrame::load_page(int iTocItem)
{
    wxLogMessage(_T("DocumentFrame::load_page (by toc item, item %d) %s"), iTocItem, GetLabel().c_str());
    //try
    //{
        if (m_UpdateContents)
        {
            wxString fullpath = get_path_for_toc_item(iTocItem);
            wxLogMessage(_T("[DocumentFrame::load_page] page: <%s>"), fullpath.c_str());

            if (!fullpath.empty())
            {
                m_UpdateContents = false;

                LdpZipReader reader( to_std_string(fullpath) );
                int viewType = ViewFactory::k_view_vertical_book;
                string title = "test";
                m_right->display_document(reader, viewType, title);
                m_right->Refresh();
                m_UpdateContents = true;
            }
        }
    //}
    //catch(std::exception& e)
    //{
    //    wxString msg = to_wx_string(e.what());
    //    msg += _T(" (in DocumentFrame::load_page(iTocItem)");
    //    wxMessageBox(msg);
    //}
    //catch(...)
    //{
    //    wxMessageBox(_T("Unknown non-standard exception in in DocumentFrame::load_page(iTocItem)"));
    //}
}

//---------------------------------------------------------------------------------------
void DocumentFrame::load_page(const string& filename)
{
    wxLogMessage(_T("DocumentFrame::load_page (by filename) %s. Filename='%s'"), GetLabel().c_str(), to_wx_string(filename).c_str());
    try
    {
        int viewType = ViewFactory::k_view_vertical_book;
        m_right->display_document(filename, viewType);
    }
    catch(std::exception& e)
    {
        wxString msg = to_wx_string(e.what());
        msg += _T(" (in DocumentFrame::load_page(filename)");
        wxMessageBox(msg);
    }
    catch(...)
    {
        wxMessageBox(_T("Unknown non-standard exception in in DocumentFrame::load_page(filename)"));
    }
}

//---------------------------------------------------------------------------------------
wxString DocumentFrame::get_path_for_toc_item(int iItem)
{
    const BookIndexArray& contents = m_pBookReader->GetContentsArray();
    if (!contents[iItem]->page.empty())
        return contents[iItem]->GetFullPath();
    else
        return wxEmptyString;
}

//---------------------------------------------------------------------------------------
void DocumentFrame::on_splitter_moved(wxSplitterEvent& WXUNUSED(event))
{
    wxLogMessage(_T("DocumentFrame::on_splitter_moved %s"), GetLabel().c_str());
    if (m_right)
        m_right->zoom_fit_width();
}


}   //namespace lenmus
