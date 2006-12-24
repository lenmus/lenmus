//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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

#ifdef __GNUG__
    #pragma interface BookContentsBox.h
#endif

#ifndef __LM_BOOKCONTENTSBOX_H__        //to avoid nested includes
#define __LM_BOOKCONTENTSBOX_H__

#include "wx/treectrl.h"
#include "wx/dynarray.h"

//#include "HtmlListBox.h"
#include "ContentsBoxCtrol.h"
#include "BookData.h"

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


class wxWindow;

class lmBookContentsBox : public lmContentsBoxCtrol
{
public:
    lmBookContentsBox() { }
    lmBookContentsBox(wxWindow* parent, wxFrame* pFrame, wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = 0, const wxString& name = lmHtmlListBoxNameStr);
    ~lmBookContentsBox();

    // wxTreeCtrol compatibility methods
    void DeleteAllItems();
    void EnsureVisible(const long& item);
    wxTreeItemData* GetItemData(const long& item) const;
    void SelectItem(const long& item, bool select = true);
    void SetItemBold(const long& item, bool bold = true);
    void SetItemImage(const long& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);

    // Content creation
    void CreateContents(lmBookData* pBookData);

    // navigation
    void ChangePage();
    int PageNext();
    int PagePrev();
    void Expand(int nItem, bool fRefresh=true);
    void Collapse(int nItem);

    // Information
    int FindNextPage(int nTree);
    bool IsLastPage();
    int FindPagePrev(int nTree);
    bool IsFirstPage();





private:
    wxString OnGetItem(size_t n) const;
    int LocateTreeItem(int nEntry) const;
    int LocateEntry(int nTree) const;
    wxString FormatItem(int nTree) const;
    void UpdateItemCount();

    // virtual methods overrides
    void DoHandleItemClick(int item, int flags);


    wxFrame*        m_pFrame;


    // content data
    lmPagesHash     m_PagesHash;    // to locate pages index from its URL
    lmTreeArray     m_aTree;        // items to display


    DECLARE_NO_COPY_CLASS(lmBookContentsBox)
    DECLARE_DYNAMIC_CLASS(lmBookContentsBox)
};

#endif  // __LM_BOOKCONTENTSBOX_H__

