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
#include "lenmus_score_canvas.h"
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


////--------------------------------------------------------------------------
//// lmHtmlWindow (private)
////--------------------------------------------------------------------------
//class lmHtmlWindow : public wxHtmlWindow
//{
//public:
//    lmHtmlWindow() : wxHtmlWindow() { m_rScale = 1.0; }
//    lmHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY,
//                 const wxPoint& pos = wxDefaultPosition,
//                 const wxSize& size = wxDefaultSize,
//                 long style = wxHW_DEFAULT_STYLE | wxBORDER_SIMPLE ,
//                 const wxString& name = wxT("htmlWindow"))
//        : wxHtmlWindow(parent, id, pos, size, style, name) { m_rScale = 1.0; }
//    ~lmHtmlWindow() {}
//
//    // -- overrides --
//
//    // Called when user clicked on hypertext link. The idea is to intercept and
//    // deal with user commands
//    void OnLinkClicked(const wxHtmlLinkInfo& link);
//
//    //In tabbed interface this method fails. I suppose it fails as it is not possible
//    // to set up the frame title. So I override it to save the title but not change
//    // the frame title
//    void OnSetTitle(const wxString& title)
//        {
//            m_OpenedPageTitle = title;
//        }
//
//    // to deal with scale for scores
//    void SetScale(double rScale) { m_rScale = rScale; }
//    double GetScale() { return m_rScale; }
//
//
//private:
//    double      m_rScale;
//
//};

////--------------------------------------------------------------------------
//// TextBookHelpHtmlWindow (private)
////--------------------------------------------------------------------------
//class TextBookHelpHtmlWindow : public lmHtmlWindow
//{
//    public:
//        TextBookHelpHtmlWindow(DocumentFrame *fr)
//            : lmHtmlWindow(fr)
//            , m_Frame(fr)
//        {
//            SetStandardFonts();
//        }
//
//        virtual void OnLinkClicked(const wxHtmlLinkInfo& link)
//        {
//            lmHtmlWindow::OnLinkClicked(link);
//            const wxMouseEvent *e = link.GetEvent();
//            if (e == NULL || e->LeftUp())
//                m_Frame->NotifyPageChanged();
//        }
//
//        // Returns full location with anchor (helper)
//        static wxString GetOpenedPageWithAnchor(lmHtmlWindow *win)
//        {
//            if(!win)
//                return wxEmptyString;
//
//            wxString an = win->GetOpenedAnchor();
//            wxString pg = win->GetOpenedPage();
//            if(!an.empty())
//            {
//                pg << _T("#");
//                pg << an;
//            }
//            return pg;
//        }
//
//    private:
//        DocumentFrame *m_Frame;
//
//    DECLARE_NO_COPY_CLASS(TextBookHelpHtmlWindow)
//};


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
//    m_mergedIndex = new TextBookHelpMergedIndex;
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
//            // new index entry
//            TextBookHelpMergedIndexItem *mi = new TextBookHelpMergedIndexItem();
//            mi->name = pItem->GetIndentedName();
//            mi->items.Add(pItem);
//            mi->parent = (pItem->level == 0) ? NULL : history[pItem->level - 1];
//            history[pItem->level] = mi;
//            merged.Add(mi);
//        }
//    }
//}


//=======================================================================================
// DocumentFrame implementation
//=======================================================================================

BEGIN_EVENT_TABLE(DocumentFrame, Canvas)
    EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, DocumentFrame::on_splitter_moved)
    //EVT_HTML_LINK_CLICKED(wxID_ANY, DocumentFrame::OnContentsLinkClicked)
END_EVENT_TABLE()

DocumentFrame::DocumentFrame(ContentFrame* parent, ApplicationScope& appScope,
                     LomseDoorway& lomse)
    : Canvas(parent, wxNewId(), _T("DocumentFrame"), wxSP_3D | wxCLIP_CHILDREN)
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
void DocumentFrame::create_toc_pane()
{
    m_left = new BookContentBox(this, this, m_appScope, wxID_ANY, wxDefaultPosition,
                                wxDefaultSize, 0, _T("The TOC"));
    m_left->CreateContents(m_pBookReader);
}

//---------------------------------------------------------------------------------------
void DocumentFrame::create_content_pane(const string& filename)
{
    m_right = new DocumentCanvas(this, m_appScope, m_lomse);
    load_page(filename);
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
    //use filename (without path) as page title
    wxFileName document( to_wx_string(filename) );
    set_title( document.GetName() );
    bool fIsBook = (document.GetExt().Upper() == _T("LMB"));

    if (fIsBook)
    {
        //read book (.lmb)
        m_pBookReader = new BookReader();
        if (!m_pBookReader->AddBook(document))
        {
            //TODO better error handling
            wxMessageBox(wxString::Format(_("Failed adding book %s"),
                document.GetFullPath().c_str() ));
            return;
        }
        create_toc_pane();
        #if (LENMUS_PLATFORM_WIN32 == 1)
            create_content_pane("C:\\USR\\Desarrollo_wx\\lenmus\\locale\\en\\books\\00.302-exercise_theo_intervals.lms");
        #else
            create_content_pane("/datos/USR/Desarrollo_wx/lenmus/locale/en/books/00.302-exercise_theo_intervals.lms");
        #endif


        m_left->Show(true);
        SplitVertically(m_left, m_right, 100);
    }
    else
    {
        //read score (.lms)
        create_content_pane(filename);
        Initialize(m_right);
    }
}

//---------------------------------------------------------------------------------------
void DocumentFrame::on_splitter_moved(wxSplitterEvent& event)
{
    m_right->zoom_fit_width();
    event.Skip();
}




//---------------------------------------------------------------------------------------
wxString DocumentFrame::GetOpenedPageWithAnchor()
{
    //Returns full location with anchor
    //Format is path to file to open: "#LenMusPage/PageName.htm"
    //URI:   <scheme name> : <hierarchical part> [ ? <query> ] [ # <fragment> ]

    return _T("#LenMusPage/TheoryHarmony_cover.htm");


    //wxString an = GetOpenedAnchor();
    //wxString pg = GetOpenedPage();
    //if(!an.empty())
    //{
    //    pg << _T("#");
    //    pg << an;
    //}
    //return pg;
}


//---------------------------------------------------------------------------------------
void DocumentFrame::load_page(int iTocItem)
{
    if (m_UpdateContents)
    {
        wxString fullpath = get_path_for_toc_item(iTocItem);
        if (!fullpath.empty())
        {
            m_UpdateContents = false;

            LdpZipReader reader( to_std_string(fullpath) );
            int viewType = ViewFactory::k_view_vertical_book;
            string title = "test";
            m_right->display_document(reader, viewType, title);
            //wxMessageBox(fullpath);
            m_UpdateContents = true;
        }
    }
}

//---------------------------------------------------------------------------------------
void DocumentFrame::load_page(const string& filename)
{
    int viewType = ViewFactory::k_view_vertical_book;
    m_right->display_document(filename, viewType);
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
void DocumentFrame::clear_page()
{
    //if (m_UpdateContents) {
    //    m_UpdateContents = false;
    //    //FFF
    //    //m_HtmlWin->SetPage(_T("<html><body bgcolor='#808080'></body></hmtl>"));
    //    m_UpdateContents = true;
    //}
}



}   //namespace lenmus
