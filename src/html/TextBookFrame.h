//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This file is a modified copy of file helpfrm.h from wxWidgets 2.6.2 project.
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

#ifndef __TEXTBOOKFRAME__H_
#define __TEXTBOOKFRAME__H_

#include "wx/defs.h"

#if wxUSE_WXHTML_HELP

#include "wx/helpbase.h"
#include "wx/html/helpdata.h"
#include "wx/window.h"
#include "wx/frame.h"
#include "wx/config.h"
#include "wx/splitter.h"
#include "wx/notebook.h"
#include "wx/listbox.h"
#include "wx/choice.h"
#include "wx/combobox.h"
#include "wx/checkbox.h"
#include "wx/stattext.h"
#include "wx/html/htmlwin.h"
#include "wx/html/htmprint.h"

#include "HtmlWindow.h"

#include "../mdi/ChildFrame.h"
#include "BookData.h"

#include "BookContentsBox.h"

class wxButton;
class wxTextCtrl;
class wxTreeEvent;
class wxTreeCtrl;


// style flags for the Help Frame
#define wxHF_TOOLBAR                0x0001
#define wxHF_CONTENTS               0x0002
#define wxHF_INDEX                  0x0004
#define wxHF_SEARCH                 0x0008
#define wxHF_BOOKMARKS              0x0010
#define wxHF_OPEN_FILES             0x0020
#define wxHF_PRINT                  0x0040
#define wxHF_FLAT_TOOLBAR           0x0080
#define wxHF_MERGE_BOOKS            0x0100
#define wxHF_ICONS_BOOK             0x0200
#define wxHF_ICONS_BOOK_CHAPTER     0x0400
#define wxHF_ICONS_FOLDER           0x0000 // this is 0 since it is default
#define wxHF_DEFAULT_STYLE          (wxHF_TOOLBAR | wxHF_CONTENTS | \
                                     wxHF_INDEX | wxHF_SEARCH | \
                                     wxHF_BOOKMARKS | wxHF_PRINT)
//compatibility:
#define wxHF_OPENFILES               wxHF_OPEN_FILES
#define wxHF_FLATTOOLBAR             wxHF_FLAT_TOOLBAR
#define wxHF_DEFAULTSTYLE            wxHF_DEFAULT_STYLE


struct TextBookFrameCfg
{
    int x, y, w, h;
    long sashpos;
    bool navig_on;
};

struct TextBookHelpMergedIndexItem;
class TextBookHelpMergedIndex;

class lmTextBookController;

class lmTextBookFrame : public lmMDIChildFrame
{
    DECLARE_DYNAMIC_CLASS(lmTextBookFrame)

public:
    lmTextBookFrame(lmBookData* data = NULL) { Init(data); }
    lmTextBookFrame(wxWindow* parent, wxWindowID wxWindowID,
                    const wxString& title = wxEmptyString,
                    int style = wxHF_DEFAULT_STYLE, lmBookData* data = NULL);

    bool Create(wxWindow* parent, wxWindowID id, const wxString& title = wxEmptyString,
                int style = wxHF_DEFAULT_STYLE);
    ~lmTextBookFrame();

    lmBookData* GetData() { return m_pBookData; }
    lmTextBookController* GetController() const { return m_pBookController; }
    void SetController(lmTextBookController* controller) { m_pBookController = controller; }

    // Sets format of title of the frame. Must contain exactly one "%s"
    // (for title of displayed HTML page)
    void SetTitleFormat(const wxString& format);
    void SetTitle(const wxString& title) {}         //CSG AQUI_FALLA

    bool Display(const wxString& x);
    bool Display(const int id);

    // Displays help window and focuses contents.
    bool DisplayContents();

    // Displays help window and focuses index.
    bool DisplayIndex();

    // Searches for keyword. Returns true and display page if found, return
    // false otherwise
    // Syntax of keyword is Altavista-like:
    //*  words are separated by spaces
    //   (but "\"hello world\"" is only one world "hello world")
    //*  word may be pretended by + or -
    //   (+ : page must contain the word ; - : page can't contain the word)
    //*  if there is no + or - before the word, + is default
    bool KeywordSearch(const wxString& keyword,
                       wxHelpSearchMode mode = wxHELP_SEARCH_ALL);

    void UseConfig(wxConfigBase* config, const wxString& rootpath = wxEmptyString)
        {
            m_Config = config;
            m_ConfigRoot = rootpath;
            ReadCustomization(config, rootpath);
        }

    // Saves custom settings into cfg config. it will use the path 'path'
    // if given, otherwise it will save info into currently selected path.
    // saved values : things set by SetFonts, SetBorders.
    void ReadCustomization(wxConfigBase* cfg, const wxString& path = wxEmptyString);
    void WriteCustomization(wxConfigBase* cfg, const wxString& path = wxEmptyString);

    // call this to let lmTextBookFrame know page changed
    void NotifyPageChanged();
    wxString GetOpenedPageWithAnchor();


    // Refreshes Contents and Index tabs
    void RefreshLists();

    //CSG Added
    void OnToolbar(wxCommandEvent& event);
    bool IsNavPanelVisible() { return m_Cfg.navig_on; }
    void UpdateUIEvent(wxUpdateUIEvent& event, wxToolBar* pToolBar);
    double GetScale() { return m_rScale; }


protected:
    void Init(lmBookData* data = NULL);
    void CreateContents();
    void CreateIndex();
    void CreateSearch();
    void UpdateMergedIndex();
    void IncreaseFontSize();
    void DecreaseFontSize();

    void OnContentsLinkClicked(wxHtmlLinkEvent& event);
    void OnIndexSel(wxCommandEvent& event);
    void OnIndexFind(wxCommandEvent& event);
    void OnIndexAll(wxCommandEvent& event);
    void OnSearchSel(wxCommandEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnBookmarksSel(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnActivate(wxActivateEvent& event);


    // Images:
    enum {
        IMG_Book = 0,
        IMG_Folder,
        IMG_Page
    };

protected:
    lmBookData*         m_pBookData;
    bool                m_DataCreated;  // m_pBookData created by frame, or supplied?
    wxString            m_TitleFormat;  // title of the help frame

    // below are various pointers to GUI components
    lmHtmlWindow*       m_HtmlWin;
    wxSplitterWindow*   m_Splitter;
    wxPanel*            m_NavigPan;
    wxNotebook*         m_NavigNotebook;

    // contents panel
    lmBookContentsBox*  m_pContentsBox; 

    // index panel
    wxTextCtrl*         m_IndexText;
    wxButton*           m_IndexButton;
    wxButton*           m_IndexButtonAll;
    wxListBox*          m_IndexList;

    // search panel
    wxTextCtrl*         m_SearchText;
    wxButton*           m_SearchButton;
    wxListBox*          m_SearchList;
    wxChoice*           m_SearchChoice;
    wxStaticText*       m_IndexCountInfo;
    wxCheckBox*         m_SearchCaseSensitive;
    wxCheckBox*         m_SearchWholeWords;

    wxComboBox*         m_Bookmarks;
    wxArrayString       m_BookmarksNames, m_BookmarksPages;

    TextBookFrameCfg m_Cfg;

    wxConfigBase* m_Config;
    wxString m_ConfigRoot;

    // pagenumbers of controls in notebook (usually 0,1,2)
    int m_ContentsPage;
    int m_IndexPage;
    int m_SearchPage;

    // lists of available fonts (used in options dialog)
    wxArrayString* m_NormalFonts,* m_FixedFonts;
    int         m_nFontSize;
    wxString    m_NormalFace, m_FixedFace;

    bool m_UpdateContents;

    //CSG Added
    wxToolBar*  m_pToolbar;
    double      m_rScale;

#if wxUSE_PRINTING_ARCHITECTURE
    wxHtmlEasyPrinting* m_Printer;
#endif
    wxHashTable* m_PagesHash;
    lmTextBookController* m_pBookController;

    int m_hfStyle;

private:
    void DisplayIndexItem(const TextBookHelpMergedIndexItem* it);
    TextBookHelpMergedIndex* m_mergedIndex;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(lmTextBookFrame)
};

#endif

#endif // __TEXTBOOKFRAME__H_
