//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

#ifndef __LENMUS_CONTENT_BOX_H__        //to avoid nested includes
#define __LENMUS_CONTENT_BOX_H__

//wxWidgets
#include <wx/treectrl.h>
#include <wx/dynarray.h>

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_content_box_ctrol.h"
#include "lenmus_book_reader.h"


class wxWindow;

typedef struct lmTreeContentRecordStruct {
    int nLevel;
    bool fVisible;          // this item is visible
    bool fHasChildren;      // this item has children nodes
    bool fOpen;             // this item is open (its children are shown)
    wxString sImage;
    wxString sTitle;
    wxString sTitlenum;
} lmTreeContentRecord;

WX_DECLARE_OBJARRAY(lmTreeContentRecord, lmTreeArray);

// declare a hash map with string keys and int values
WX_DECLARE_STRING_HASH_MAP( int, lmPagesHash );


namespace lenmus
{
class DocumentFrame;


class BookContentBox : public ContentBoxCtrol
{
public:
    BookContentBox(wxWindow* parent, DocumentFrame* pFrame, ApplicationScope& appScope,
            wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = 0, const wxString& name = "ContentBoxCtrol");
    ~BookContentBox();

    // wxTreeCtrol compatibility methods
    void DeleteAllItems();
    void EnsureVisible(const long& item);
    wxTreeItemData* GetItemData(const long& item) const;
    void SelectItem(const long& item, bool select = true);
    void SetItemBold(const long& item, bool bold = true);
    void SetItemImage(const long& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);

    // Content creation
    void CreateContents(BooksCollection* pBooksData);

    // navigation
    //void ChangePage();
    int PageNext();
    int PagePrev();
    void Expand(int nItem, bool fRefresh=true);
    void Collapse(int nItem);

    // Information
    int FindNextPage(int nTree);
    bool IsLastPage();
    int FindPagePrev(int nTree);
    bool IsFirstPage();
    int find_page(const wxString& fullpath);

protected:
    //event handlers
    void OnContentsLinkClicked(wxHtmlLinkEvent& event);

private:
    wxString OnGetItem(size_t n) const;
    int LocateTreeItem(int nEntry) const;
    int LocateEntry(int nTree) const;
    wxString FormatItem(int nTree) const;
    void UpdateItemCount();

    // virtual methods overrides
    void DoHandleItemClick(int item, int flags);


    DocumentFrame*    m_pFrame;

    // content data
    lmPagesHash     m_PagesHash;    // to locate pages index from its URL
    lmTreeArray     m_aTree;        // items to display


    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(BookContentBox);
};


}   //namespace lenmus

#endif  // __LENMUS_CONTENT_BOX_H__

