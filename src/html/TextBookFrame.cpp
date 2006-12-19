//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This file was initially is based on file helpfrm.cpp from wxWidgets 2.6.2 project
//    although now it must be something quite different
//    wxWidgets licence is compatible with GNU GPL.
//    Author:      Harm van der Heijden and Vaclav Slavik
//    Copyright (c) Harm van der Heijden and Vaclav Slavik
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
#pragma implementation "TextBookFrame.h"
#endif

// For compilers that support precompilation, includes "wx.h"

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"

    #include "wx/object.h"
    #include "wx/sizer.h"

    #include "wx/bmpbuttn.h"
    #include "wx/statbox.h"
    #include "wx/radiobox.h"
#endif // WXPRECOMP

#ifdef __WXMAC__
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
#endif

#include "TextBookFrame.h"
#include "TextBookController.h"
#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/html/htmlwin.h"
#include "wx/busyinfo.h"
#include "wx/progdlg.h"
#include "wx/toolbar.h"
#include "wx/fontenum.h"
#include "wx/stream.h"
#include "wx/filedlg.h"
#include "wx/artprov.h"
#include "wx/spinctrl.h"
#include "wx/dynarray.h"
#include "wx/choicdlg.h"
#include "wx/settings.h"


#include "../app/global.h"
#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;


#include "../app/Preferences.h"         // access to user preferences

// what is considered "small index"?
#define INDEX_IS_SMALL 100

// Motif defines this as a macro 
#ifdef Below
#undef Below
#endif

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


//--------------------------------------------------------------------------
// lmTextBookHelpHtmlWindow (private)
//--------------------------------------------------------------------------

class lmTextBookHelpHtmlWindow : public lmHtmlWindow
{
    public:
        lmTextBookHelpHtmlWindow(lmTextBookFrame *fr, wxWindow *parent)
            : lmHtmlWindow(parent), m_Frame(fr)
        {
            SetStandardFonts();
        }

        virtual void OnLinkClicked(const wxHtmlLinkInfo& link)
        {
            lmHtmlWindow::OnLinkClicked(link);
            const wxMouseEvent *e = link.GetEvent();
            if (e == NULL || e->LeftUp())
                m_Frame->NotifyPageChanged();
        }

        // Returns full location with anchor (helper)
        static wxString GetOpenedPageWithAnchor(lmHtmlWindow *win)
        {
            if(!win)
                return wxEmptyString;

            wxString an = win->GetOpenedAnchor();
            wxString pg = win->GetOpenedPage();
            if(!an.empty())
            {
                pg << wxT("#");
                pg << an;
            }
            return pg;
        }

    private:
        lmTextBookFrame *m_Frame;

    DECLARE_NO_COPY_CLASS(lmTextBookHelpHtmlWindow)
};


//---------------------------------------------------------------------------
// lmTextBookFrame::m_mergedIndex
//---------------------------------------------------------------------------

WX_DEFINE_ARRAY_PTR(const lmBookIndexItem*, wxHtmlHelpDataItemPtrArray);

struct TextBookHelpMergedIndexItem
{
    TextBookHelpMergedIndexItem *parent;
    wxString                   name;
    wxHtmlHelpDataItemPtrArray items;
};

WX_DECLARE_OBJARRAY(TextBookHelpMergedIndexItem, TextBookHelpMergedIndex);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(TextBookHelpMergedIndex)

void lmTextBookFrame::UpdateMergedIndex()
{
    delete m_mergedIndex;
    m_mergedIndex = new TextBookHelpMergedIndex;
    TextBookHelpMergedIndex& merged = *m_mergedIndex;

    const lmBookIndexArray& items = m_pBookData->GetIndexArray();
    size_t len = items.size();

    TextBookHelpMergedIndexItem *history[128] = {NULL};

    for (size_t i = 0; i < len; i++)
    {
        const lmBookIndexItem& item = items[i];
        wxASSERT_MSG( item.level < 128, _T("nested index entries too deep") );

        if (history[item.level] &&
            history[item.level]->items[0]->name == item.name)
        {
            // same index entry as previous one, update list of items
            history[item.level]->items.Add(&item);
        }
        else
        {
            // new index entry
            TextBookHelpMergedIndexItem *mi = new TextBookHelpMergedIndexItem();
            mi->name = item.GetIndentedName();
            mi->items.Add(&item);
            mi->parent = (item.level == 0) ? NULL : history[item.level - 1];
            history[item.level] = mi;
            merged.Add(mi);
        }
    }
}


//---------------------------------------------------------------------------
// lmTextBookFrame
//---------------------------------------------------------------------------

// windows' IDs :
enum
{
    ID_BOOKMARKS_LIST = 2000,
    ID_BOOKMARKS_ADD,
    ID_BOOKMARKS_REMOVE,
    ID_TREECTRL,
    ID_INDEXPAGE,
    ID_INDEXLIST,
    ID_INDEXTEXT,
    ID_INDEXBUTTON,
    ID_INDEXBUTTONALL,
    ID_NOTEBOOK,
    ID_SEARCHPAGE,
    ID_SEARCHTEXT,
    ID_SEARCHLIST,
    ID_SEARCHBUTTON,
    ID_SEARCHCHOICE,
    ID_COUNTINFO
};


IMPLEMENT_DYNAMIC_CLASS(lmTextBookFrame, lmMDIChildFrame)


BEGIN_EVENT_TABLE(lmTextBookFrame, lmMDIChildFrame)
    EVT_ACTIVATE(lmTextBookFrame::OnActivate)
    EVT_TOOL_RANGE(MENU_eBookPanel, MENU_eBook_Options, lmTextBookFrame::OnToolbar)

    EVT_BUTTON      (ID_BOOKMARKS_REMOVE, lmTextBookFrame::OnToolbar)
    EVT_BUTTON      (ID_BOOKMARKS_ADD, lmTextBookFrame::OnToolbar)
    //EVT_TREE_SEL_CHANGED(ID_TREECTRL, lmTextBookFrame::OnContentsSel)
    EVT_LISTBOX     (ID_INDEXLIST, lmTextBookFrame::OnIndexSel)
    EVT_LISTBOX     (ID_SEARCHLIST, lmTextBookFrame::OnSearchSel)
    EVT_BUTTON      (ID_SEARCHBUTTON, lmTextBookFrame::OnSearch)
    EVT_TEXT_ENTER  (ID_SEARCHTEXT, lmTextBookFrame::OnSearch)
    EVT_BUTTON      (ID_INDEXBUTTON, lmTextBookFrame::OnIndexFind)
    EVT_TEXT_ENTER  (ID_INDEXTEXT, lmTextBookFrame::OnIndexFind)
    EVT_BUTTON      (ID_INDEXBUTTONALL, lmTextBookFrame::OnIndexAll)
    EVT_COMBOBOX    (ID_BOOKMARKS_LIST, lmTextBookFrame::OnBookmarksSel)
    EVT_CLOSE       (lmTextBookFrame::OnCloseWindow)
    EVT_HTML_LINK_CLICKED(ID_TREECTRL, lmTextBookFrame::OnContentsLinkClicked)

END_EVENT_TABLE()


lmTextBookFrame::lmTextBookFrame(wxWindow* parent, wxWindowID id, const wxString& title,
                                 int style, lmBookData* data)
{
    Init(data);
    Create(parent, id, title, style);
}

void lmTextBookFrame::Init(lmBookData* data)
{
    wxASSERT(data);
    m_pBookData = data;
    m_DataCreated = false;

    m_pContentsBox = NULL;
    m_IndexList = NULL;
    m_IndexButton = NULL;
    m_IndexButtonAll = NULL;
    m_IndexText = NULL;
    m_SearchList = NULL;
    m_SearchButton = NULL;
    m_SearchText = NULL;
    m_SearchChoice = NULL;
    m_IndexCountInfo = NULL;
    m_Splitter = NULL;
    m_NavigPan = NULL;
    m_NavigNotebook = NULL;
    m_HtmlWin = NULL;
    m_Bookmarks = NULL;
    m_SearchCaseSensitive = NULL;
    m_SearchWholeWords = NULL;

    m_mergedIndex = NULL;

    m_Config = NULL;
    m_ConfigRoot = wxEmptyString;

    m_Cfg.x = m_Cfg.y = -1;
    m_Cfg.w = 700;
    m_Cfg.h = 480;
    m_Cfg.sashpos = 240;
    m_Cfg.navig_on = true;

    m_NormalFonts = m_FixedFonts = NULL;
    m_NormalFace = m_FixedFace = wxEmptyString;
#ifdef __WXMSW__
    m_FontSize = 10;
#else
    m_FontSize = 14;
#endif

#if wxUSE_PRINTING_ARCHITECTURE
    m_Printer = NULL;
#endif

    //m_PagesHash = NULL;
    m_UpdateContents = true;
    m_pBookController = (lmTextBookController*) NULL;

    //CSG Added
    m_pToolbar = (wxToolBar*) NULL;
}


bool lmTextBookFrame::Create(wxWindow* parent, wxWindowID id,
                             const wxString& WXUNUSED(title), int style)
{
    // Create: builds the GUI components.
    // m_HtmlWin will *always* be created, but it's important to realize that
    // m_pContentsBox, m_IndexList, m_SearchList, m_SearchButton, m_SearchText and
    // m_SearchButton may be NULL.
    // moreover, if no contents, index or searchpage is needed, m_Splitter and
    // m_NavigPan will be NULL too (with m_HtmlWin directly connected to the frame)
    m_hfStyle = style;

    wxImageList *ContentsImageList = new wxImageList(16, 16);
    ContentsImageList->Add(wxArtProvider::GetIcon(wxART_HELP_BOOK,
                                                  wxART_HELP_BROWSER,
                                                  wxSize(16, 16)));
    ContentsImageList->Add(wxArtProvider::GetIcon(wxART_HELP_FOLDER,
                                                  wxART_HELP_BROWSER,
                                                  wxSize(16, 16)));
    ContentsImageList->Add(wxArtProvider::GetIcon(wxART_HELP_PAGE,
                                                  wxART_HELP_BROWSER,
                                                  wxSize(16, 16)));

    // Do the config in two steps. We read the HtmlWindow customization after we
    // create the window.
    if (m_Config)
        ReadCustomization(m_Config, m_ConfigRoot);

    lmMDIChildFrame::Create((lmMDIParentFrame*)parent, id, _("Help"),
                    wxPoint(m_Cfg.x, m_Cfg.y), wxSize(m_Cfg.w, m_Cfg.h),
                    wxDEFAULT_FRAME_STYLE, wxT("TextBookHelp") );

#if lmUSE_NOTEBOOK_MDI
    ////Notebook: is always maximized
#else
    lmMDIChildFrame::Maximize(true);
#endif  // lmUSE_NOTEBOOK_MDI

    GetPosition(&m_Cfg.x, &m_Cfg.y);

    SetIcon(wxArtProvider::GetIcon(wxART_HELP, wxART_HELP_BROWSER));


    int notebook_page = 0;

    wxSizer *navigSizer = NULL;

    if (style & (wxHF_CONTENTS | wxHF_INDEX | wxHF_SEARCH))
    {
        // traditional help controller; splitter window with html page on the
        // right and a notebook containing various pages on the left
        m_Splitter = new wxSplitterWindow(this);

        m_HtmlWin = new lmTextBookHelpHtmlWindow(this, m_Splitter);
        m_NavigPan = new wxPanel(m_Splitter, wxID_ANY); //, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
        m_NavigNotebook = new wxNotebook(m_NavigPan, ID_NOTEBOOK,
                                         wxDefaultPosition, wxDefaultSize); 

        navigSizer = new wxBoxSizer(wxVERTICAL);
        navigSizer->Add(m_NavigNotebook, 1, wxEXPAND);

        m_NavigPan->SetSizer(navigSizer);
    }
    else
    { // only html window, no notebook with index,contents etc
        m_HtmlWin = new lmHtmlWindow(this);
    }

    m_HtmlWin->SetRelatedFrame((wxFrame*)this, m_TitleFormat);
    g_pMainFrame->SetHtmlWindow(m_HtmlWin);

    if ( m_Config )
        m_HtmlWin->ReadCustomization(m_Config, m_ConfigRoot);

    // contents tree panel?
    if ( style & wxHF_CONTENTS )
    {
        wxWindow *pPanel = new wxPanel(m_NavigNotebook, ID_INDEXPAGE);
        wxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

        topsizer->Add(0, 10);

        pPanel->SetSizer(topsizer);

        if ( style & wxHF_BOOKMARKS )
        {
            m_Bookmarks = new wxComboBox(pPanel, ID_BOOKMARKS_LIST,
                                         wxEmptyString,
                                         wxDefaultPosition, wxDefaultSize,
                                         0, NULL, wxCB_READONLY | wxCB_SORT);
            m_Bookmarks->Append(_("(bookmarks)"));
            for (unsigned i = 0; i < m_BookmarksNames.GetCount(); i++)
                m_Bookmarks->Append(m_BookmarksNames[i]);
            m_Bookmarks->SetSelection(0);

            wxBitmapButton *bmpbt1, *bmpbt2;
            bmpbt1 = new wxBitmapButton(pPanel, ID_BOOKMARKS_ADD,
                                 wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK,
                                                          wxART_BUTTON));
            bmpbt2 = new wxBitmapButton(pPanel, ID_BOOKMARKS_REMOVE,
                                 wxArtProvider::GetBitmap(wxART_DEL_BOOKMARK,
                                                          wxART_BUTTON));
            bmpbt1->SetToolTip(_("Add current page to bookmarks"));
            bmpbt2->SetToolTip(_("Remove current page from bookmarks"));

            wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

            sizer->Add(m_Bookmarks, 1, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
            sizer->Add(bmpbt1, 0, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 2);
            sizer->Add(bmpbt2, 0, wxALIGN_CENTRE_VERTICAL, 0);

            topsizer->Add(sizer, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 10);
        }

        m_pContentsBox = new lmBookContentsBox(pPanel, this, ID_TREECTRL, 
                                wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER );
        m_pContentsBox->AssignImageList(ContentsImageList);
        topsizer->Add(m_pContentsBox, 1,
                      wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT,
                      2);

        m_NavigNotebook->AddPage(pPanel, _("Contents"));
        m_ContentsPage = notebook_page++;
    }

    // index listbox panel?
    if ( style & wxHF_INDEX )
    {
        wxWindow *pPanel = new wxPanel(m_NavigNotebook, ID_INDEXPAGE);
        wxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

        pPanel->SetSizer(topsizer);

        m_IndexText = new wxTextCtrl(pPanel, ID_INDEXTEXT, wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     wxTE_PROCESS_ENTER);
        m_IndexButton = new wxButton(pPanel, ID_INDEXBUTTON, _("Find"));
        m_IndexButtonAll = new wxButton(pPanel, ID_INDEXBUTTONALL,
                                        _("Show all"));
        m_IndexCountInfo = new wxStaticText(pPanel, ID_COUNTINFO,
                                            wxEmptyString, wxDefaultPosition,
                                            wxDefaultSize,
                                            wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
        m_IndexList = new wxListBox(pPanel, ID_INDEXLIST,
                                    wxDefaultPosition, wxSize(125,-1),
                                    0, NULL, wxLB_SINGLE);

        m_IndexButton->SetToolTip(_("Display all index items that contain given substring. Search is case insensitive."));
        m_IndexButtonAll->SetToolTip(_("Show all items in index"));

        topsizer->Add(m_IndexText, 0, wxEXPAND | wxALL, 10);
        wxSizer *btsizer = new wxBoxSizer(wxHORIZONTAL);
        btsizer->Add(m_IndexButton, 0, wxRIGHT, 2);
        btsizer->Add(m_IndexButtonAll);
        topsizer->Add(btsizer, 0,
                      wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, 10);
        topsizer->Add(m_IndexCountInfo, 0, wxEXPAND | wxLEFT | wxRIGHT, 2);
        topsizer->Add(m_IndexList, 1, wxEXPAND | wxALL, 2);

        m_NavigNotebook->AddPage(pPanel, _("Index"));
        m_IndexPage = notebook_page++;
    }

    // search list panel?
    if ( style & wxHF_SEARCH )
    {
        wxWindow *pPanel = new wxPanel(m_NavigNotebook, ID_INDEXPAGE);
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        pPanel->SetSizer(sizer);

        m_SearchText = new wxTextCtrl(pPanel, ID_SEARCHTEXT,
                                      wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_PROCESS_ENTER);
        m_SearchChoice = new wxChoice(pPanel, ID_SEARCHCHOICE,
                                      wxDefaultPosition, wxSize(125,-1));
        m_SearchCaseSensitive = new wxCheckBox(pPanel, wxID_ANY, _("Case sensitive"));
        m_SearchWholeWords = new wxCheckBox(pPanel, wxID_ANY, _("Whole words only"));
        m_SearchButton = new wxButton(pPanel, ID_SEARCHBUTTON, _("Search"));
        m_SearchButton->SetToolTip(_("Search contents of help book(s) for all occurences of the text you typed above"));
        m_SearchList = new wxListBox(pPanel, ID_SEARCHLIST,
                                     wxDefaultPosition, wxSize(125,-1),
                                     0, NULL, wxLB_SINGLE);

        sizer->Add(m_SearchText, 0, wxEXPAND | wxALL, 10);
        sizer->Add(m_SearchChoice, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
        sizer->Add(m_SearchCaseSensitive, 0, wxLEFT | wxRIGHT, 10);
        sizer->Add(m_SearchWholeWords, 0, wxLEFT | wxRIGHT, 10);
        sizer->Add(m_SearchButton, 0, wxALL | wxALIGN_RIGHT, 8);
        sizer->Add(m_SearchList, 1, wxALL | wxEXPAND, 2);

        m_NavigNotebook->AddPage(pPanel, _("Search"));
        m_SearchPage = notebook_page;
    }

    m_HtmlWin->Show();

    RefreshLists();

    if ( navigSizer )
    {
        navigSizer->SetSizeHints(m_NavigPan);
        m_NavigPan->Layout();
    }

    // showtime
    if ( m_NavigPan && m_Splitter )
    {
        m_Splitter->SetMinimumPaneSize(5);  //minimum size: 5%
        if ( m_Cfg.navig_on )
            m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);

        if ( m_Cfg.navig_on )
        {
            m_NavigPan->Show();
            m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
        }
        else
        {
            m_NavigPan->Show(false);
            m_Splitter->Initialize(m_HtmlWin);
        }
    }

    // Reduce flicker by updating the splitter pane sizes before the
    // frame is shown
    wxSizeEvent sizeEvent(GetSize(), GetId());
    ProcessEvent(sizeEvent);

    m_Splitter->UpdateSize();

    return true;
}

lmTextBookFrame::~lmTextBookFrame()
{
    delete m_mergedIndex;

    // PopEventHandler(); // wxhtmlhelpcontroller (not any more!)
    if (m_DataCreated)
        delete m_pBookData;
    if (m_NormalFonts) delete m_NormalFonts;
    if (m_FixedFonts) delete m_FixedFonts;
    //if (m_PagesHash)
    //{
    //    WX_CLEAR_HASH_TABLE(*m_PagesHash);
    //    delete m_PagesHash;
    //}
#if wxUSE_PRINTING_ARCHITECTURE
    if (m_Printer) delete m_Printer;
#endif
}


void lmTextBookFrame::SetTitleFormat(const wxString& format)
{
    if (m_HtmlWin)
        m_HtmlWin->SetRelatedFrame((wxFrame*)this, format);
    m_TitleFormat = format;
}


bool lmTextBookFrame::Display(const wxString& x)
{
    wxString url = m_pBookData->FindPageByName(x);
    if (!url.empty())
    {
        m_HtmlWin->LoadPage(url);
        NotifyPageChanged();
        return true;
    }

    return false;
}

bool lmTextBookFrame::Display(const int id)
{
    wxString url = m_pBookData->FindPageById(id);
    if (!url.empty())
    {
        m_HtmlWin->LoadPage(url);
        NotifyPageChanged();
        return true;
    }

    return false;
}



bool lmTextBookFrame::DisplayContents()
{
    if (!m_pContentsBox) return false;

    if (!m_Splitter->IsSplit()) {
        m_NavigPan->Show();
        m_HtmlWin->Show();
        m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
        m_Cfg.navig_on = true;
    }

    m_NavigNotebook->SetSelection(0);

    if (m_pBookData->GetBookRecArray().GetCount() > 0)
    {
        lmBookRecord& book = m_pBookData->GetBookRecArray()[0];
        if (!book.GetStart().empty())
            m_HtmlWin->LoadPage(book.GetFullPath(book.GetStart()));
    }

    return true;
}



bool lmTextBookFrame::DisplayIndex()
{
    if (! m_IndexList)
        return false;

    if (!m_Splitter->IsSplit())
    {
        m_NavigPan->Show();
        m_HtmlWin->Show();
        m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }

    m_NavigNotebook->SetSelection(1);

    if (m_pBookData->GetBookRecArray().GetCount() > 0)
    {
        lmBookRecord& book = m_pBookData->GetBookRecArray()[0];
        if (!book.GetStart().empty())
            m_HtmlWin->LoadPage(book.GetFullPath(book.GetStart()));
    }

    return true;
}

void lmTextBookFrame::DisplayIndexItem(const TextBookHelpMergedIndexItem *it)
{
    if (it->items.size() == 1)
    {
        if (!it->items[0]->page.empty())
        {
            m_HtmlWin->LoadPage(it->items[0]->GetFullPath());
            NotifyPageChanged();
        }
    }
    else
    {
        wxBusyCursor busy_cursor;

        // more pages associated with this index item -- let the user choose
        // which one she/he wants from a list:
        wxArrayString arr;
        size_t len = it->items.size();
        for (size_t i = 0; i < len; i++)
        {
            wxString page = it->items[i]->page;
            // try to find page's title in contents:
            const lmBookIndexArray& contents = m_pBookData->GetContentsArray();
            size_t clen = contents.size();
            for (size_t j = 0; j < clen; j++)
            {
                if (contents[j].page == page)
                {
                    page = contents[j].name;
                    break;
                }
            }
            arr.push_back(page);
        }

        wxSingleChoiceDialog dlg(this,
                                 _("Please choose the page to display:"),
                                 _("Help Topics"),
                                 arr, NULL, wxCHOICEDLG_STYLE & ~wxCENTRE);
        if (dlg.ShowModal() == wxID_OK)
        {
            m_HtmlWin->LoadPage(it->items[dlg.GetSelection()]->GetFullPath());
            NotifyPageChanged();
        }
    }
}


bool lmTextBookFrame::KeywordSearch(const wxString& keyword,
                                    wxHelpSearchMode mode)
{
//    if (mode == wxHELP_SEARCH_ALL)
//    {
//        if ( !(m_SearchList &&
//               m_SearchButton && m_SearchText && m_SearchChoice) )
//            return false;
//    }
//    else if (mode == wxHELP_SEARCH_INDEX)
//    {
//        if ( !(m_IndexList &&
//               m_IndexButton && m_IndexButtonAll && m_IndexText) )
//            return false;
//    }
//
//    int foundcnt = 0;
//    wxString foundstr;
//    wxString book = wxEmptyString;
//
//    if (!m_Splitter->IsSplit())
//    {
//        m_NavigPan->Show();
//        m_HtmlWin->Show();
//        m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
//    }
//
//    if (mode == wxHELP_SEARCH_ALL)
//    {
//        m_NavigNotebook->SetSelection(m_SearchPage);
//        m_SearchList->Clear();
//        m_SearchText->SetValue(keyword);
//        m_SearchButton->Disable();
//
//        if (m_SearchChoice->GetSelection() != 0)
//            book = m_SearchChoice->GetStringSelection();
//
//        wxHtmlSearchStatus status(m_pBookData, keyword,
//                                  m_SearchCaseSensitive->GetValue(),
//                                  m_SearchWholeWords->GetValue(),
//                                  book);
//
//#if wxUSE_PROGRESSDLG
//        wxProgressDialog progress(_("Searching..."),
//                                  _("No matching page found yet"),
//                                  status.GetMaxIndex(), this,
//                                  wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
//#endif
//
//        int curi;
//        while (status.IsActive())
//        {
//            curi = status.GetCurIndex();
//            if (curi % 32 == 0
//#if wxUSE_PROGRESSDLG
//                && !progress.Update(curi)
//#endif
//               )
//                break;
//            if (status.Search())
//            {
//                foundstr.Printf(_("Found %i matches"), ++foundcnt);
//#if wxUSE_PROGRESSDLG
//                progress.Update(status.GetCurIndex(), foundstr);
//#endif
//                m_SearchList->Append(status.GetName(), (void*)status.GetCurItem());
//            }
//        }
//
//        m_SearchButton->Enable();
//        m_SearchText->SetSelection(0, keyword.Length());
//        m_SearchText->SetFocus();
//    }
//    else if (mode == wxHELP_SEARCH_INDEX)
//    {
//        m_NavigNotebook->SetSelection(m_IndexPage);
//        m_IndexList->Clear();
//        m_IndexButton->Disable();
//        m_IndexButtonAll->Disable();
//        m_IndexText->SetValue(keyword);
//
//        wxCommandEvent pPanel;
//        OnIndexFind(pPanel); // what a hack...
//        m_IndexButton->Enable();
//        m_IndexButtonAll->Enable();
//        foundcnt = m_IndexList->GetCount();
//    }
//
//    if (foundcnt)
//    {
//        switch ( mode )
//        {
//            default:
//                wxFAIL_MSG( _T("unknown help search mode") );
//                // fall back
//
//            case wxHELP_SEARCH_ALL:
//            {
//                lmBookIndexItem *it =
//                    (lmBookIndexItem*) m_SearchList->GetClientData(0);
//                if (it)
//                {
//                    m_HtmlWin->LoadPage(it->GetFullPath());
//                    NotifyPageChanged();
//                }
//                break;
//            }
//
//            case wxHELP_SEARCH_INDEX:
//            {
//                TextBookHelpMergedIndexItem* it =
//                    (TextBookHelpMergedIndexItem*) m_IndexList->GetClientData(0);
//                if (it)
//                    DisplayIndexItem(it);
//                break;
//            }
//        }
//
//    }
//
//    return foundcnt > 0;
    return false;
}

void lmTextBookFrame::CreateContents()
{
    if (m_pContentsBox) {
        m_pContentsBox->CreateContents(m_pBookData);
    }
}

void lmTextBookFrame::CreateIndex()
{
    if (! m_IndexList)
        return ;

    m_IndexList->Clear();

    size_t cnt = m_mergedIndex->size();

    wxString cnttext;
    if (cnt > INDEX_IS_SMALL)
        cnttext.Printf(_("%i of %i"), 0, cnt);
    else
        cnttext.Printf(_("%i of %i"), cnt, cnt);
    m_IndexCountInfo->SetLabel(cnttext);
    if (cnt > INDEX_IS_SMALL)
        return;

    for (size_t i = 0; i < cnt; i++)
        m_IndexList->Append((*m_mergedIndex)[i].name,
                            (char*)(&(*m_mergedIndex)[i]));
}

void lmTextBookFrame::CreateSearch()
{
    if (! (m_SearchList && m_SearchChoice))
        return ;
    m_SearchList->Clear();
    m_SearchChoice->Clear();
    m_SearchChoice->Append(_("Search in all books"));
    const lmBookRecArray& bookrec = m_pBookData->GetBookRecArray();
    int i, cnt = bookrec.GetCount();
    for (i = 0; i < cnt; i++)
        m_SearchChoice->Append(bookrec[i].GetTitle());
    m_SearchChoice->SetSelection(0);
}


void lmTextBookFrame::RefreshLists()
{
    // Update m_mergedIndex:
    UpdateMergedIndex();
    // Update the controls
    CreateContents();
    CreateIndex();
    CreateSearch();
}

void lmTextBookFrame::ReadCustomization(wxConfigBase *cfg, const wxString& path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString)
    {
        oldpath = cfg->GetPath();
        cfg->SetPath(_T("/") + path);
    }

    m_Cfg.navig_on = cfg->Read(wxT("tbcNavigPanel"), m_Cfg.navig_on) != 0;
    m_Cfg.sashpos = cfg->Read(wxT("tbcSashPos"), m_Cfg.sashpos);
    m_Cfg.x = cfg->Read(wxT("tbcX"), m_Cfg.x);
    m_Cfg.y = cfg->Read(wxT("tbcY"), m_Cfg.y);
    m_Cfg.w = cfg->Read(wxT("tbcW"), m_Cfg.w);
    m_Cfg.h = cfg->Read(wxT("tbcH"), m_Cfg.h);

    m_FixedFace = cfg->Read(wxT("tbcFixedFace"), m_FixedFace);
    m_NormalFace = cfg->Read(wxT("tbcNormalFace"), m_NormalFace);
    m_FontSize = cfg->Read(wxT("tbcBaseFontSize"), m_FontSize);

    {
        int i;
        int cnt;
        wxString val, s;

        cnt = cfg->Read(wxT("tbcBookmarksCnt"), 0L);
        if (cnt != 0)
        {
            m_BookmarksNames.Clear();
            m_BookmarksPages.Clear();
            if (m_Bookmarks)
            {
                m_Bookmarks->Clear();
                m_Bookmarks->Append(_("(bookmarks)"));
            }

            for (i = 0; i < cnt; i++)
            {
                val.Printf(wxT("tbcBookmark_%i"), i);
                s = cfg->Read(val);
                m_BookmarksNames.Add(s);
                if (m_Bookmarks) m_Bookmarks->Append(s);
                val.Printf(wxT("tbcBookmark_%i_url"), i);
                s = cfg->Read(val);
                m_BookmarksPages.Add(s);
            }
        }
    }

    if (m_HtmlWin)
        m_HtmlWin->ReadCustomization(cfg);

    if (path != wxEmptyString)
        cfg->SetPath(oldpath);
}

void lmTextBookFrame::WriteCustomization(wxConfigBase *cfg, const wxString& path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString)
    {
        oldpath = cfg->GetPath();
        cfg->SetPath(_T("/") + path);
    }

    cfg->Write(wxT("tbcNavigPanel"), m_Cfg.navig_on);
    cfg->Write(wxT("tbcSashPos"), (long)m_Cfg.sashpos);
    if ( !IsIconized() )
    {
        //  Don't write if iconized as this would make the window
        //  disappear next time it is shown!
        cfg->Write(wxT("tbcX"), (long)m_Cfg.x);
        cfg->Write(wxT("tbcY"), (long)m_Cfg.y);
        cfg->Write(wxT("tbcW"), (long)m_Cfg.w);
        cfg->Write(wxT("tbcH"), (long)m_Cfg.h);
    }
    cfg->Write(wxT("tbcFixedFace"), m_FixedFace);
    cfg->Write(wxT("tbcNormalFace"), m_NormalFace);
    cfg->Write(wxT("tbcBaseFontSize"), (long)m_FontSize);

    if (m_Bookmarks)
    {
        int i;
        int cnt = m_BookmarksNames.GetCount();
        wxString val;

        cfg->Write(wxT("tbcBookmarksCnt"), (long)cnt);
        for (i = 0; i < cnt; i++)
        {
            val.Printf(wxT("tbcBookmark_%i"), i);
            cfg->Write(val, m_BookmarksNames[i]);
            val.Printf(wxT("tbcBookmark_%i_url"), i);
            cfg->Write(val, m_BookmarksPages[i]);
        }
    }

    if (m_HtmlWin)
        m_HtmlWin->WriteCustomization(cfg);

    if (path != wxEmptyString)
        cfg->SetPath(oldpath);
}

static void SetFontsToHtmlWin(lmHtmlWindow *win, wxString scalf, wxString fixf, int size)
{
    int f_sizes[7];
    f_sizes[0] = int(size * 0.6);
    f_sizes[1] = int(size * 0.8);
    f_sizes[2] = size;
    f_sizes[3] = int(size * 1.2);
    f_sizes[4] = int(size * 1.4);
    f_sizes[5] = int(size * 1.6);
    f_sizes[6] = int(size * 1.8);

    win->SetFonts(scalf, fixf, f_sizes);
}

class TextBookHelpFrameOptionsDialog : public wxDialog
{
public:
    wxComboBox *NormalFont, *FixedFont;
    wxSpinCtrl *FontSize;
    lmHtmlWindow *TestWin;

    TextBookHelpFrameOptionsDialog(wxWindow *parent)
        : wxDialog(parent, wxID_ANY, wxString(_("Help Browser Options")))
    {
        wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
        wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 3, 2, 5);

        sizer->Add(new wxStaticText(this, wxID_ANY, _("Normal font:")));
        sizer->Add(new wxStaticText(this, wxID_ANY, _("Fixed font:")));
        sizer->Add(new wxStaticText(this, wxID_ANY, _("Font size:")));

        sizer->Add(NormalFont = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                      wxSize(200, wxDefaultCoord),
                      0, NULL, wxCB_DROPDOWN | wxCB_READONLY));

        sizer->Add(FixedFont = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                      wxSize(200, wxDefaultCoord),
                      0, NULL, wxCB_DROPDOWN | wxCB_READONLY));

        sizer->Add(FontSize = new wxSpinCtrl(this, wxID_ANY));
        FontSize->SetRange(2, 100);

        topsizer->Add(sizer, 0, wxLEFT|wxRIGHT|wxTOP, 10);

        topsizer->Add(new wxStaticText(this, wxID_ANY, _("Preview:")),
                        0, wxLEFT | wxTOP, 10);
        topsizer->Add(TestWin = new lmHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxSize(20, 150),
                                                 wxHW_SCROLLBAR_AUTO | wxSUNKEN_BORDER),
                        1, wxEXPAND | wxLEFT|wxTOP|wxRIGHT, 10);

        wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
        wxButton *ok;
        sizer2->Add(ok = new wxButton(this, wxID_OK), 0, wxALL, 10);
        ok->SetDefault();
        sizer2->Add(new wxButton(this, wxID_CANCEL), 0, wxALL, 10);
        topsizer->Add(sizer2, 0, wxALIGN_RIGHT);

        SetSizer(topsizer);
        topsizer->Fit(this);
        Centre(wxBOTH);
    }


    void UpdateTestWin()
    {
        wxBusyCursor bcur;
        SetFontsToHtmlWin(TestWin,
                          NormalFont->GetStringSelection(),
                          FixedFont->GetStringSelection(),
                          FontSize->GetValue());

        wxString content(_("font size"));

        content = _T("<font size=-2>") + content + _T(" -2</font><br>")
                  _T("<font size=-1>") + content + _T(" -1</font><br>")
                  _T("<font size=+0>") + content + _T(" +0</font><br>")
                  _T("<font size=+1>") + content + _T(" +1</font><br>")
                  _T("<font size=+2>") + content + _T(" +2</font><br>")
                  _T("<font size=+3>") + content + _T(" +3</font><br>")
                  _T("<font size=+4>") + content + _T(" +4</font><br>") ;

        content = wxString( _T("<html><body><table><tr><td>") ) +
                  _("Normal face<br>and <u>underlined</u>. ") +
                  _("<i>Italic face.</i> ") +
                  _("<b>Bold face.</b> ") +
                  _("<b><i>Bold italic face.</i></b><br>") +
                  content +
                  wxString( _T("</td><td><tt>") ) +
                  _("Fixed size face.<br> <b>bold</b> <i>italic</i> ") +
                  _("<b><i>bold italic <u>underlined</u></i></b><br>") +
                  content +
                  _T("</tt></td></tr></table></body></html>");

        TestWin->SetPage( content );
    }

    void OnUpdate(wxCommandEvent& WXUNUSED(event))
    {
        UpdateTestWin();
    }
    void OnUpdateSpin(wxSpinEvent& WXUNUSED(event))
    {
        UpdateTestWin();
    }

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(TextBookHelpFrameOptionsDialog)
};

BEGIN_EVENT_TABLE(TextBookHelpFrameOptionsDialog, wxDialog)
    EVT_COMBOBOX(wxID_ANY, TextBookHelpFrameOptionsDialog::OnUpdate)
    EVT_SPINCTRL(wxID_ANY, TextBookHelpFrameOptionsDialog::OnUpdateSpin)
END_EVENT_TABLE()

void lmTextBookFrame::OptionsDialog()
{
    TextBookHelpFrameOptionsDialog dlg(this);
    unsigned i;

    if (m_NormalFonts == NULL)
    {
        wxFontEnumerator enu;
        enu.EnumerateFacenames();
        m_NormalFonts = new wxArrayString;
        *m_NormalFonts = enu.GetFacenames();
        m_NormalFonts->Sort(); // ascending sort
    }
    if (m_FixedFonts == NULL)
    {
        wxFontEnumerator enu;
        enu.EnumerateFacenames(wxFONTENCODING_SYSTEM, true /*enum fixed width only*/);
        m_FixedFonts = new wxArrayString;
        *m_FixedFonts = enu.GetFacenames();
        m_FixedFonts->Sort(); // ascending sort
    }

    // VS: We want to show the font that is actually used by wxHtmlWindow.
    //     If customization dialog wasn't used yet, facenames are empty and
    //     wxHtmlWindow uses default fonts -- let's find out what they
    //     are so that we can pass them to the dialog:
    if (m_NormalFace.empty())
    {
        wxFont fnt(m_FontSize, wxSWISS, wxNORMAL, wxNORMAL, false);
        m_NormalFace = fnt.GetFaceName();
    }
    if (m_FixedFace.empty())
    {
        wxFont fnt(m_FontSize, wxMODERN, wxNORMAL, wxNORMAL, false);
        m_FixedFace = fnt.GetFaceName();
    }

    for (i = 0; i < m_NormalFonts->GetCount(); i++)
        dlg.NormalFont->Append((*m_NormalFonts)[i]);
    for (i = 0; i < m_FixedFonts->GetCount(); i++)
        dlg.FixedFont->Append((*m_FixedFonts)[i]);
    if (!m_NormalFace.empty())
        dlg.NormalFont->SetStringSelection(m_NormalFace);
    else
        dlg.NormalFont->SetSelection(0);
    if (!m_FixedFace.empty())
        dlg.FixedFont->SetStringSelection(m_FixedFace);
    else
        dlg.FixedFont->SetSelection(0);
    dlg.FontSize->SetValue(m_FontSize);
    dlg.UpdateTestWin();

    if (dlg.ShowModal() == wxID_OK)
    {
        m_NormalFace = dlg.NormalFont->GetStringSelection();
        m_FixedFace = dlg.FixedFont->GetStringSelection();
        m_FontSize = dlg.FontSize->GetValue();
        SetFontsToHtmlWin(m_HtmlWin, m_NormalFace, m_FixedFace, m_FontSize);
    }
}



void lmTextBookFrame::NotifyPageChanged()
{
    if (m_UpdateContents && m_pContentsBox) {
        m_UpdateContents = false;   //to reject new updates until this one is done
        m_pContentsBox->ChangePage();
        m_UpdateContents = true;
    }

}

wxString lmTextBookFrame::GetOpenedPageWithAnchor()
{
    return lmTextBookHelpHtmlWindow::GetOpenedPageWithAnchor(m_HtmlWin);
}


/*
EVENT HANDLING :
*/


void lmTextBookFrame::OnActivate(wxActivateEvent& event)
{
    // This saves one mouse click when using the
    // wxHTML for context sensitive help systems
#ifndef __WXGTK__
    // NB: wxActivateEvent is a bit broken in wxGTK
    //     and is sometimes sent when it should not be
    if (event.GetActive() && m_HtmlWin)
        m_HtmlWin->SetFocus();
#endif

    event.Skip();
}

void lmTextBookFrame::OnToolbar(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case MENU_eBook_GoBack :
            m_HtmlWin->HistoryBack();
            NotifyPageChanged();
            break;

        case MENU_eBook_GoForward :
            m_HtmlWin->HistoryForward();
            NotifyPageChanged();
            break;

        case MENU_eBook_Up :
            //if (m_PagesHash)
            //{
            //    wxString page = lmTextBookHelpHtmlWindow::GetOpenedPageWithAnchor(m_HtmlWin);
            //    TextBookHelpHashData *ha = NULL;
            //    if (!page.empty())
            //        ha = (TextBookHelpHashData*) m_PagesHash->Get(page);
            //    if (ha && ha->m_Index > 0)
            //    {
            //        const lmBookIndexItem& it = m_pBookData->GetContentsArray()[ha->m_Index - 1];
            //        if (!it.page.empty())
            //        {
            //            m_HtmlWin->LoadPage(it.GetFullPath());
            //            NotifyPageChanged();
            //        }
            //    }
            //}
            break;

        case MENU_eBook_UpNode :
            //if (m_PagesHash)
            //{
            //    wxString page = lmTextBookHelpHtmlWindow::GetOpenedPageWithAnchor(m_HtmlWin);
            //    TextBookHelpHashData *ha = NULL;
            //    if (!page.empty())
            //        ha = (TextBookHelpHashData*) m_PagesHash->Get(page);
            //    if (ha && ha->m_Index > 0)
            //    {
            //        int level =
            //            m_pBookData->GetContentsArray()[ha->m_Index].level - 1;
            //        int ind = ha->m_Index - 1;

            //        const lmBookIndexItem *it =
            //            &m_pBookData->GetContentsArray()[ind];
            //        while (ind >= 0 && it->level != level)
            //        {
            //            ind--;
            //            it = &m_pBookData->GetContentsArray()[ind];
            //        }
            //        if (ind >= 0)
            //        {
            //            if (!it->page.empty())
            //            {
            //                m_HtmlWin->LoadPage(it->GetFullPath());
            //                NotifyPageChanged();
            //            }
            //        }
            //    }
            //}
            break;

        case MENU_eBook_Down :
            //if (m_PagesHash)
            //{
            //    wxString page = lmTextBookHelpHtmlWindow::GetOpenedPageWithAnchor(m_HtmlWin);
            //    TextBookHelpHashData *ha = NULL;
            //    if (!page.empty())
            //        ha = (TextBookHelpHashData*) m_PagesHash->Get(page);

            //    const lmBookIndexArray& contents = m_pBookData->GetContentsArray();
            //    if (ha && ha->m_Index < (int)contents.size() - 1)
            //    {
            //        size_t idx = ha->m_Index + 1;

            //        while (contents[idx].GetFullPath() == page) idx++;

            //        if (!contents[idx].page.empty())
            //        {
            //            m_HtmlWin->LoadPage(contents[idx].GetFullPath());
            //            NotifyPageChanged();
            //        }
            //    }
            //}
            break;

        case MENU_eBookPanel :
            {
                if (! (m_Splitter && m_NavigPan))
                    return ;
                if (m_Splitter->IsSplit())
                {
                    m_Cfg.sashpos = m_Splitter->GetSashPosition();
                    m_Splitter->Unsplit(m_NavigPan);
                    m_Cfg.navig_on = false;
                }
                else
                {
                    m_NavigPan->Show();
                    m_HtmlWin->Show();
                    m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
                    m_Cfg.navig_on = true;
                }
            }
            break;

        case MENU_eBook_Options :
            OptionsDialog();
            break;

        case ID_BOOKMARKS_ADD :
            {
                wxString item;
                wxString url;

                item = m_HtmlWin->GetOpenedPageTitle();
                url = m_HtmlWin->GetOpenedPage();
                if (item == wxEmptyString)
                    item = url.AfterLast(wxT('/'));
                if (m_BookmarksPages.Index(url) == wxNOT_FOUND)
                {
                    m_Bookmarks->Append(item);
                    m_BookmarksNames.Add(item);
                    m_BookmarksPages.Add(url);
                }
            }
            break;

        case ID_BOOKMARKS_REMOVE :
            {
                wxString item;
                int pos;

                item = m_Bookmarks->GetStringSelection();
                pos = m_BookmarksNames.Index(item);
                if (pos != wxNOT_FOUND)
                {
                    m_BookmarksNames.RemoveAt(pos);
                    m_BookmarksPages.RemoveAt(pos);
                    m_Bookmarks->Delete(m_Bookmarks->GetSelection());
                }
            }
            break;

#if wxUSE_PRINTING_ARCHITECTURE
        case MENU_eBook_Print :
            {
                if (m_Printer == NULL)
                    m_Printer = new wxHtmlEasyPrinting(_("Help Printing"), this);
                if (!m_HtmlWin->GetOpenedPage())
                    wxLogWarning(_("Cannot print empty page."));
                else
                    m_Printer->PrintFile(m_HtmlWin->GetOpenedPage());
            }
            break;
#endif

        case MENU_eBook_OpenFile :
            {
                wxString filemask = wxString(
                    _("HTML files (*.html;*.htm)|*.html;*.htm|")) +
                    _("Help books (*.htb)|*.htb|Help books (*.zip)|*.zip|") +
                    _("HTML Help Project (*.hhp)|*.hhp|") +
#if wxUSE_LIBMSPACK
                    _("Compressed HTML Help file (*.chm)|*.chm|") +
#endif
                    _("All files (*.*)|*");
                wxString s = wxFileSelector(_("Open HTML document"),
                                            wxEmptyString,
                                            wxEmptyString,
                                            wxEmptyString,
                                            filemask,
                                            wxOPEN | wxFILE_MUST_EXIST,
                                            this);
                if (!s.empty())
                {
                    wxString ext = s.Right(4).Lower();
                    if (ext == _T(".zip") || ext == _T(".htb") ||
#if wxUSE_LIBMSPACK
                        ext == _T(".chm") ||
#endif
                        ext == _T(".hhp"))
                    {
                        wxBusyCursor bcur;
                        m_pBookData->AddBook(s);
                        RefreshLists();
                    }
                    else
                        m_HtmlWin->LoadPage(s);
                }
            }
            break;
    }
}



void lmTextBookFrame::OnContentsLinkClicked(wxHtmlLinkEvent& event)
{
    enum {
        eOpen = 0,
        eClose,
        eGo,
        eOpenGo,
        eCloseGo,
        eCloseBlank,
    };

    //extract type and item
    wxString sLink = event.GetLinkInfo().GetHref();
    int nAction;
    wxString sItem;

    if (sLink.StartsWith(_T("item"), &sItem)) {
        nAction = eGo;
    }
    else if (sLink.StartsWith(_T("open&go"), &sItem)) {
        nAction = eOpenGo;
    }
    else if (sLink.StartsWith(_T("close&go"), &sItem)) {
        nAction = eCloseGo;
    }
    else if (sLink.StartsWith(_T("close&blank"), &sItem)) {
        nAction = eCloseBlank;
    }
    else if (sLink.StartsWith(_T("open"), &sItem)) {
        nAction = eOpen;
    }
    else if (sLink.StartsWith(_T("close"), &sItem)) {
        nAction = eClose;
    }
    else {
        wxLogMessage(_T("[lmTextBookFrame::OnContentsLinkClicked]Invalid link type"));
        return;
    }

    long nItem;
    if (!sItem.ToLong(&nItem)) {
        wxLogMessage(_T("[lmTextBookFrame::OnContentsLinkClicked] Invalid link number"));
        return;
    }

    switch (nAction) {
        case eGo:
            if (m_UpdateContents) {
                const lmBookIndexArray& contents = m_pBookData->GetContentsArray();
                m_UpdateContents = false;
                if (!contents[nItem].page.empty()) {
                    m_HtmlWin->LoadPage(contents[nItem].GetFullPath());
                }
                m_UpdateContents = true;
            }
            break;

        case eOpen:
            if (m_pContentsBox) m_pContentsBox->Expand(nItem);
            break;

        case eClose:
            if (m_pContentsBox) m_pContentsBox->Collapse(nItem);
            break;

        case eOpenGo:
            if (m_UpdateContents) {
                const lmBookIndexArray& contents = m_pBookData->GetContentsArray();
                m_UpdateContents = false;
                if (!contents[nItem].page.empty()) {
                    m_HtmlWin->LoadPage(contents[nItem].GetFullPath());
                }
                m_UpdateContents = true;
            }
            if (m_pContentsBox) m_pContentsBox->Expand(nItem);
            break;

        case eCloseGo:
            if (m_UpdateContents) {
                const lmBookIndexArray& contents = m_pBookData->GetContentsArray();
                m_UpdateContents = false;
                if (!contents[nItem].page.empty()) {
                    m_HtmlWin->LoadPage(contents[nItem].GetFullPath());
                }
                m_UpdateContents = true;
            }
            if (m_pContentsBox) m_pContentsBox->Collapse(nItem);
            break;

        case eCloseBlank:
            if (m_UpdateContents) {
                m_UpdateContents = false;
                m_HtmlWin->SetPage(_T("<html><body bgcolor='#808080'></body></hmtl>"));
                m_UpdateContents = true;
            }
            if (m_pContentsBox) m_pContentsBox->Collapse(nItem);
            break;

        default:
            wxASSERT(false);
    }

}



void lmTextBookFrame::OnIndexSel(wxCommandEvent& WXUNUSED(event))
{
    TextBookHelpMergedIndexItem *it = (TextBookHelpMergedIndexItem*)
        m_IndexList->GetClientData(m_IndexList->GetSelection());
    if (it)
        DisplayIndexItem(it);
}


void lmTextBookFrame::OnIndexFind(wxCommandEvent& event)
{
    wxString sr = m_IndexText->GetLineText(0);
    sr.MakeLower();
    if (sr == wxEmptyString)
    {
        OnIndexAll(event);
    }
    else
    {
        wxBusyCursor bcur;

        m_IndexList->Clear();
        const TextBookHelpMergedIndex& index = *m_mergedIndex;
        size_t cnt = index.size();

        int displ = 0;
        for (size_t i = 0; i < cnt; i++)
        {
            if (index[i].name.Lower().find(sr) != wxString::npos)
            {
                int pos = m_IndexList->Append(index[i].name,
                                              (char*)(&index[i]));

                if (displ++ == 0)
                {
                    // don't automatically show topic selector if this
                    // item points to multiple pages:
                    if (index[i].items.size() == 1)
                    {
                        m_IndexList->SetSelection(0);
                        DisplayIndexItem(&index[i]);
                    }
                }

                // if this is nested item of the index, show its parent(s)
                // as well, otherwise it would not be clear what entry is
                // shown:
                TextBookHelpMergedIndexItem *parent = index[i].parent;
                while (parent)
                {
                    if (pos == 0 ||
                        (index.Index(*(TextBookHelpMergedIndexItem*)m_IndexList->GetClientData(pos-1))) < index.Index(*parent))
                    {
                        m_IndexList->Insert(parent->name,
                                            pos, (char*)parent);
                        parent = parent->parent;
                    }
                    else break;
                }

                // finally, it the item we just added is itself a parent for
                // other items, show them as well, because they are refinements
                // of the displayed index entry (i.e. it is implicitly contained
                // in them: "foo" with parent "bar" reads as "bar, foo"):
                int level = index[i].items[0]->level;
                i++;
                while (i < cnt && index[i].items[0]->level > level)
                {
                    m_IndexList->Append(index[i].name, (char*)(&index[i]));
                    i++;
                }
                i--;
            }
        }

        wxString cnttext;
        cnttext.Printf(_("%i of %i"), displ, cnt);
        m_IndexCountInfo->SetLabel(cnttext);

        m_IndexText->SetSelection(0, sr.Length());
        m_IndexText->SetFocus();
    }
}

void lmTextBookFrame::OnIndexAll(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor bcur;

    m_IndexList->Clear();
    const TextBookHelpMergedIndex& index = *m_mergedIndex;
    size_t cnt = index.size();
    bool first = true;

    for (size_t i = 0; i < cnt; i++)
    {
        m_IndexList->Append(index[i].name, (char*)(&index[i]));
        if (first)
        {
            // don't automatically show topic selector if this
            // item points to multiple pages:
            if (index[i].items.size() == 1)
            {
                DisplayIndexItem(&index[i]);
            }
            first = false;
        }
    }

    wxString cnttext;
    cnttext.Printf(_("%i of %i"), cnt, cnt);
    m_IndexCountInfo->SetLabel(cnttext);
}


void lmTextBookFrame::OnSearchSel(wxCommandEvent& WXUNUSED(event))
{
    lmBookIndexItem *it = (lmBookIndexItem*) m_SearchList->GetClientData(m_SearchList->GetSelection());
    if (it)
    {
        if (!it->page.empty())
            m_HtmlWin->LoadPage(it->GetFullPath());
        NotifyPageChanged();
    }
}

void lmTextBookFrame::OnSearch(wxCommandEvent& WXUNUSED(event))
{
    wxString sr = m_SearchText->GetLineText(0);

    if (!sr.empty())
        KeywordSearch(sr, wxHELP_SEARCH_ALL);
}

void lmTextBookFrame::OnBookmarksSel(wxCommandEvent& WXUNUSED(event))
{
    wxString sr = m_Bookmarks->GetStringSelection();

    if (sr != wxEmptyString && sr != _("(bookmarks)"))
    {
       m_HtmlWin->LoadPage(m_BookmarksPages[m_BookmarksNames.Index(sr)]);
       NotifyPageChanged();
    }
}

void lmTextBookFrame::OnCloseWindow(wxCloseEvent& evt)
{
    GetSize(&m_Cfg.w, &m_Cfg.h);
    GetPosition(&m_Cfg.x, &m_Cfg.y);

#ifdef __WXGTK__
    if (IsGrabbed())
    {
        RemoveGrab();
    }
#endif

    if (m_Splitter && m_Cfg.navig_on) m_Cfg.sashpos = m_Splitter->GetSashPosition();

    if (m_Config)
        WriteCustomization(m_Config, m_ConfigRoot);

    if (m_pBookController && m_pBookController->IsKindOf(CLASSINFO(lmTextBookController)))
    {
        ((lmTextBookController*) m_pBookController)->OnCloseFrame(evt);
    }

    evt.Skip();
}


