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
    #pragma implementation BookContentsBox.h
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "BookContentsBox.h"
#include "TextBookFrame.h"

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(lmTreeArray)


// to use lmHtmlListBox you must derive a new class from it as you must
// implement pure OnGetItem()


// ============================================================================
// lmBookContentsBox implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(lmBookContentsBox, lmHtmlListBox)

lmBookContentsBox::lmBookContentsBox(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                                     const wxSize& size, long style,
                                     const wxString& name)
             : lmHtmlListBox(parent, id, pos, size, style, name)
{
    m_pParent = parent;
    m_PagesHash = (wxHashTable*)NULL;

    SetMargins(5, 5);
    SetItemCount(1);
    //SetSelection(1);
}

lmBookContentsBox::~lmBookContentsBox() 
{
    m_cItems.Clear();
}

wxString lmBookContentsBox::OnGetItem(size_t n) const
{
    int i = LocateItem(int(n));
    if (i == -1) return wxEmptyString;
    return FormatItem(i);

}


// ============================================================================
// wxTreeCtrol compatibility methods
// ============================================================================

long lmBookContentsBox::AddRoot(const wxString& text, int image, int selImage,
                                        wxTreeItemData* data)
{

    //Adds the root node to the tree, returning the new item.
    //
    //The image and selImage parameters are an index within the normal image
    //list specifying the image to use for unselected and selected items, respectively.
    //If image > -1 and selImage is -1, the same image is used for both selected and
    //unselected items.

    return (long)m_cItems.Add(text);

}

long lmBookContentsBox::AppendItem(const long& parent, 
                                           const wxString& text, wxString& sImgPath, int image,
                                           int selImage, wxTreeItemData* data)
{
    //Appends an item to the end of the branch identified by parent, return a new item id.

    //The image and selImage parameters are an index within the normal image list
    //specifying the image to use for unselected and selected items, respectively.
    //If image > -1 and selImage is -1, the same image is used for both selected 
    //and unselected items.
    //wxFileSystem& oFS = GetFileSystem();
    //oFS.ChangePathTo(sImgPath);
    return (long)m_cItems.Add(text);
}

void lmBookContentsBox::AssignImageList(wxImageList* imageList)
{
    //Sets the normal image list. Image list assigned with this method will be 
    //automatically deleted by wxTreeCtrl as appropriate (i.e. it takes ownership 
    //of the list).
}

void lmBookContentsBox::DeleteAllItems()
{
    //Deletes all items in the control.
    m_cItems.Clear();
}

void lmBookContentsBox::EnsureVisible(const long& item)
{
    //Scrolls and/or expands items to ensure that the given item is visible.
}

wxTreeItemData* lmBookContentsBox::GetItemData(const long& item) const
{
    //Returns the tree item data associated with the item.
    return (wxTreeItemData*)NULL;
}

void lmBookContentsBox::SelectItem(const long& item, bool select)
{
    //Selects the given item. In multiple selection controls, can be also used
    //to deselect a currently selected item if the value of select is false.
    Select(item, select);
}

void lmBookContentsBox::SetItemBold(const long& item, bool bold)
{
    //Makes item appear in bold font if bold parameter is true or resets it to 
    //the normal state.
}

void lmBookContentsBox::SetItemImage(const long& item, int image,
                                     wxTreeItemIcon which)
{
    //Sets the specified item image. See GetItemImage for the description of 
    //the which parameter.
}



// ============================================================================
// contents creation and management
// ============================================================================

void lmBookContentsBox::CreateContents(lmBookData* pBookData)
{
    DeleteAllItems();   //remove all previous contents
    const lmBookIndexArray& contents = pBookData->GetContentsArray();
    int nNumItems = (int)contents.size();
    wxString sImagePath = wxEmptyString;

    // As m_pBookData has flat structure there is no simple way of recognizing
    // if some item has subitems or not. If we find an item with level=n, we know
    // that the last item with level=n-1 is its parent, so we are going to
    // use and array (aParents) to hold the current parent at a given level.
    const int MAX_TREE_DEPTH = 64;
    long aParents[MAX_TREE_DEPTH];
    int nLevelPrev = 0;

    aParents[0] = -1;    //nodes at level 0 doesn't have parent

    for (int i = 0; i < nNumItems; i++)
    {
        lmBookIndexItem *it = &contents[i];

        // set path for images
        if (i == 0)
        {
            wxFileSystem& oFS = GetFileSystem();
            wxFileName oFN( (it->pBookRecord)->GetBasePath() );
            oFN.AppendDir( _T("img") );
            sImagePath = oFN.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        }

        lmTreeContentRecord rItem;
        rItem.nLevel = it->level;
        rItem.fVisible = (it->level == 0);
        rItem.sImage = (it->image != wxEmptyString ? sImagePath + it->image : wxEmptyString);
        rItem.sTitle = it->name;
        rItem.fHasChildren = false;     //it is updated when processing next entry
        rItem.fOpen = false;

        m_aTree.Add(rItem);

        //parent for coming deeper levels is this node
        aParents[rItem.nLevel + 1] = i;

        // If we current item has level=n, we know
        // that the last item with level=n-1 was folder with subitems, so we
        // set its fHasChildren flag accordingly
        if (nLevelPrev < rItem.nLevel) {
            int nParent = aParents[rItem.nLevel];
            if (nParent != -1) {
                m_aTree[nParent].fHasChildren = true;
                m_aTree[nParent].fOpen = false;
            }
        }
        nLevelPrev = rItem.nLevel;

    }

    //re-build page hash table
    if (m_PagesHash)
    {
        WX_CLEAR_HASH_TABLE(*m_PagesHash);
        delete m_PagesHash;
    }

    m_PagesHash = new wxHashTable(wxKEY_STRING, 2 * nNumItems);

    for (long i = 0; i < nNumItems; i++)
    {
        lmBookIndexItem *it = &contents[i];
        m_PagesHash->Put(it->GetFullPath(), (wxObject*)i);
    }

    SetItemCount(nNumItems);

}

int lmBookContentsBox::LocateItem(int n) const
{
    //returns index to visible item # n
    int nVisible = -1;
    for (int i = 0; i < (int)m_aTree.size(); i++)
    {
        if (m_aTree[i].fVisible) nVisible++;
        if (nVisible == n) return i;
    }
    return -1;
}

wxString lmBookContentsBox::FormatItem(int nTree) const
{
    wxString sLine;
    wxString sImgPlus = _T("<table cellpadding='0' cellspacing='0'><tr><td nowrap><img src='");
        sImgPlus += g_pPaths->GetImagePath() + _T("nav_plus_16.png'>");

    wxString sImgMinus = _T("<table cellpadding='0' cellspacing='0'><tr><td nowrap><img src='");
        sImgMinus += g_pPaths->GetImagePath() + _T("nav_minus_16.png'>");

    const wxString sEndLine = _T("</td></tr></table>");

    wxString sItemImg = _T("<table cellpadding='0' cellspacing='0'><tr><td nowrap><img src='");
        sItemImg += g_pPaths->GetImagePath() + _T("nav_space_36.png' height='36' width='");

    wxString sItemNoImg = _T("<table cellpadding='0' cellspacing='0'><tr><td nowrap><img src='");
        sItemNoImg += g_pPaths->GetImagePath() + _T("nav_space_36.png' height='16' width='");

    wxString sNavPageImg = _T("<img border='0' src='");
        sNavPageImg += g_pPaths->GetImagePath() + _T("nav_page_36.png'>");

    wxString sNavPageNoImg = _T("<img border='0' src='");
        sNavPageNoImg += g_pPaths->GetImagePath() + _T("nav_page_16.png'>");

    sLine = wxEmptyString;

    //If this node has children add expand/collapse icon
    if (m_aTree[nTree].fHasChildren) {
        if (m_aTree[nTree].fOpen) {
            sLine += wxString::Format(_T("<ax href=\"close%d\">"), nTree);
            sLine += sImgMinus + _T("<img src=\"");
            sLine += g_pPaths->GetImagePath();
            sLine += _T("nav_book_open_16.png\"></ax><b>");
        }
        else {
            sLine += wxString::Format(_T("<ax href=\"open%d\">"), nTree);
            sLine += sImgPlus + _T("<img src=\"");
            sLine += g_pPaths->GetImagePath();
            sLine += _T("nav_book_closed_16.png\"></ax><b>");
        }
    }

    if (m_aTree[nTree].nLevel == 0) {
        // It is a book node
        sLine += m_aTree[nTree].sTitle + _T("</b>") + sEndLine;

    }
    else {
        // it is a content node
        sLine += ((m_aTree[nTree].sImage).IsEmpty() ? sItemNoImg : sItemImg);
        sLine += wxString::Format(_T("%d'>"), (1+m_aTree[nTree].nLevel)*16);   //16 pixels per level
        sLine += ((m_aTree[nTree].sImage).IsEmpty() ? sNavPageNoImg : sNavPageImg);

        if (!(m_aTree[nTree].sImage).IsEmpty()) {
            sLine += wxString::Format(_T("<ax href=\"item%d\">"), nTree);
            sLine += _T("<img border='0' src='");
            sLine += m_aTree[nTree].sImage;
            sLine += _T("'></ax><br /><img src='");
            sLine += g_pPaths->GetImagePath();
            sLine += _T("nav_space_36.png' height='16' width='");
            sLine += wxString::Format(_T("%d'>"), 40+16*m_aTree[nTree].nLevel);
        }
        else {
            sLine += _T("&nbsp;&nbsp;");
        }
        sLine += wxString::Format(_T("<ax href=\"item%d\">"), nTree);
        sLine += m_aTree[nTree].sTitle + _T("</ax>") + sEndLine;
    }

    return sLine;

}

void lmBookContentsBox::ChangePage()
{
    if (m_PagesHash)
    {

        wxString page = ((lmTextBookFrame*)m_pParent)->GetOpenedPageWithAnchor();
        long nTree = -1;
        if (!page.empty())
            nTree = (long)m_PagesHash->Get(page);

        if (nTree != -1) 
        {
            SelectItem(nTree);
            EnsureVisible(nTree);
        }
    }
}

void lmBookContentsBox::Expand(int nItem)
{
    int nIdx = LocateItem(nItem);
    int nCurLevel = m_aTree[nIdx].nLevel;

    m_aTree[nIdx].fOpen = true;     // mark it as 'open'

    //mark all its children as closed but visible
    for(int i=nIdx+1; i < (int)m_aTree.GetCount(); i++)
    {
        if (m_aTree[i].nLevel == nCurLevel) break;
        m_aTree[i].fOpen = false;
        m_aTree[i].fVisible = true;
    }
    RefreshAll();
}

void lmBookContentsBox::Collapse(int nItem)
{
    int nIdx = LocateItem(nItem);
    int nCurLevel = m_aTree[nIdx].nLevel;

    m_aTree[nIdx].fOpen = false;     // mark it as 'closed'

    //mark all its children as closed and not visible
    for(int i=nIdx+1; i < (int)m_aTree.GetCount(); i++)
    {
        if (m_aTree[i].nLevel == nCurLevel) break;
        m_aTree[i].fOpen = false;
        m_aTree[i].fVisible = false;
    }
    RefreshAll();

}



//    if (m_PagesHash)
//    {
//        WX_CLEAR_HASH_TABLE(*m_PagesHash);
//        delete m_PagesHash;
//    }
//
//    const lmBookIndexArray& contents = m_pBookData->GetContentsArray();
//
//    size_t cnt = contents.size();
//
//    m_PagesHash = new wxHashTable(wxKEY_STRING, 2 * cnt);
//
//    const int MAX_ROOTS = 64;
//    long roots[MAX_ROOTS];
//    // VS: this array holds information about whether we've set item icon at
//    //     given level. This is neccessary because m_pBookData has flat structure
//    //     and there's no way of recognizing if some item has subitems or not.
//    //     We set the icon later: when we find an item with level=n, we know
//    //     that the last item with level=n-1 was folder with subitems, so we
//    //     set its icon accordingly
//    bool imaged[MAX_ROOTS];
//    m_pContentsBox->DeleteAllItems();
//
//    roots[0] = m_pContentsBox->AddRoot(_("(Help)"));
//    imaged[0] = true;
//
//    wxString sImagePath = wxEmptyString;
//    wxString sLine;
//    wxString sImgPlus = _T("<table cellpadding='0' cellspacing='0'><tr><td nowrap><img src='");
//        sImgPlus += g_pPaths->GetImagePath() + _T("nav_plus_16.png'>");
//
//    wxString sImgMinus = _T("<table cellpadding='0' cellspacing='0'><tr><td nowrap><img src='");
//        sImgMinus += g_pPaths->GetImagePath() + _T("nav_minus_16.png'>");
//
//    const wxString sEndLine = _T("</td></tr></table>");
//
//    wxString sItemImg = _T("<table cellpadding='0' cellspacing='0'><tr><td nowrap><img src='");
//        sItemImg += g_pPaths->GetImagePath() + _T("nav_space_36.png' height='36' width='");
//
//    wxString sItemNoImg = _T("<table cellpadding='0' cellspacing='0'><tr><td nowrap><img src='");
//        sItemNoImg += g_pPaths->GetImagePath() + _T("nav_space_36.png' height='16' width='");
//
//    wxString sNavPageImg = _T("<img border='0' src='");
//        sNavPageImg += g_pPaths->GetImagePath() + _T("nav_page_36.png'>");
//
//    wxString sNavPageNoImg = _T("<img border='0' src='");
//        sNavPageNoImg += g_pPaths->GetImagePath() + _T("nav_page_16.png'>");
//
//    for (size_t i = 0; i < cnt; i++)
//    {
//        lmBookIndexItem *it = &contents[i];
//
//        if (it->level == 0) {
//            // It is a book node
//            sLine = sImgMinus + _T("<img src=\"");
//            sLine += g_pPaths->GetImagePath();
//            sLine += _T("nav_book_open_16.png\"><b>") +
//                it->name + _T("</b>") + sEndLine;
//
//            roots[1] = m_pContentsBox->AppendItem(roots[0],
//                                        sLine, sImagePath, IMG_Book, -1,
//                                        new TextBookHelpTreeItemData(i));
//            m_pContentsBox->SetItemBold(roots[1], true);
//            imaged[1] = true;
//
//            // set path for images
//            wxFileSystem& oFS = m_pContentsBox->GetFileSystem();
//            lmBookRecord* pBookr = it->pBookRecord; 
//            wxFileName oFN( pBookr->GetBasePath() );
//            oFN.AppendDir( _T("img") );
//            sImagePath = oFN.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
//        }
//        else {
//            // it is a content node
//            sLine = ((it->image).IsEmpty() ? sItemNoImg : sItemImg);
//            sLine += wxString::Format(_T("%d'>"), (1+it->level)*16);   //16 pixels per level
//            sLine += ((it->image).IsEmpty() ? sNavPageNoImg : sNavPageImg);
//
//            if (!(it->image).IsEmpty()) {
//                sLine += wxString::Format(_T("<ax href=\"item%d\">"), i);
//                sLine += _T("<img border='0' src='");
//                sLine += sImagePath;
//                sLine += it->image;
//                sLine += _T("'></ax><br /><img src='");
//                sLine += g_pPaths->GetImagePath();
//                sLine += _T("nav_space_36.png' height='16' width='");
//                sLine += wxString::Format(_T("%d'>"), 40+16*it->level);
//            }
//            else {
//                sLine += _T("&nbsp;&nbsp;");
//            }
//            sLine += wxString::Format(_T("<ax href=\"item%d\">"), i);
//            sLine += it->name + _T("</ax>") + sEndLine;
//
//            roots[it->level + 1] = m_pContentsBox->AppendItem(
//                                     roots[it->level], sLine, sImagePath, IMG_Page,
//                                     -1, new TextBookHelpTreeItemData(i));
//            imaged[it->level + 1] = false;
//        }
//
//        m_PagesHash->Put(it->GetFullPath(),
//                         new TextBookHelpHashData(i, roots[it->level + 1]));
//
//        // Set the icon for the node one level up in the hiearachy,
//        // unless already done (see comment above imaged[] declaration)
//        if (!imaged[it->level])
//        {
//            int image = IMG_Folder;
//            if (m_hfStyle & wxHF_ICONS_BOOK)
//                image = IMG_Book;
//            else if (m_hfStyle & wxHF_ICONS_BOOK_CHAPTER)
//                image = (it->level == 1) ? IMG_Book : IMG_Folder;
//            m_pContentsBox->SetItemImage(roots[it->level], image);
//            m_pContentsBox->SetItemImage(roots[it->level], image,
//                                        wxTreeItemIcon_Selected);
//            imaged[it->level] = true;
//        }
//    }
//}
//
