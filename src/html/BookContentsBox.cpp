//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
#pragma implementation "BookContentsBox.h"
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


// to use lmContentsBoxCtrol you must derive a new class from it as you must
// implement pure OnGetItem()


// ============================================================================
// lmBookContentsBox implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(lmBookContentsBox, lmContentsBoxCtrol)

lmBookContentsBox::lmBookContentsBox(wxWindow* parent,
                                     lmTextBookFrame* pFrame,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size, long style,
                                     const wxString& name)
             : lmContentsBoxCtrol(parent, id, pos, size, style, name)
{
    m_pFrame = pFrame;

    SetSelectionBackground(*wxWHITE);

    SetMargins(5, 0);       //5px left, 0px items separation
    SetItemCount(1);
    //SetSelection(1);
}

lmBookContentsBox::~lmBookContentsBox()
{
    m_aTree.Clear();

}

wxString lmBookContentsBox::OnGetItem(size_t n) const
{
    int i = LocateTreeItem(int(n));
    if (i == -1) return wxEmptyString;
    return FormatItem(i);

}


// ============================================================================
// Overrides
// ============================================================================

void lmBookContentsBox::DoHandleItemClick(int item, int flags)
{
    // the item should become the current one only if it is a final node
    int nItem = LocateTreeItem(item);
    if (m_aTree[nItem].nLevel==0 || !m_aTree[nItem].fHasChildren)
        SetSelection(item);

}


// ============================================================================
// wxTreeCtrol compatibility methods
// ============================================================================

void lmBookContentsBox::DeleteAllItems()
{
    //Deletes all items in the control.
    m_aTree.Clear();
}

void lmBookContentsBox::EnsureVisible(const long& nTree)
{
    //Scrolls and/or expands items to ensure that the given item is visible.

    // Locate its parents and open them
    int nCurLevel = m_aTree[nTree].nLevel;

    for(int i=nTree-1; i >= 0 && nCurLevel > 0; i--)
    {
        if (m_aTree[i].nLevel < nCurLevel) {
           if (!m_aTree[i].fOpen) Expand(i, false);        //false: do not refresh yet
           nCurLevel = m_aTree[i].nLevel;
        }
    }

    SetSelection( LocateEntry(nTree) );
    RefreshAll();

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
    SetSelection(item);
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
        lmBookIndexItem *it = contents[i];

        // set path for images
        if (it->level == 0)
        {
            sImagePath = (it->pBookRecord)->GetBasePath();
        }

        lmTreeContentRecord rItem;
        rItem.nLevel = it->level;
        rItem.fVisible = (it->level == 0);
        if (it->image != wxEmptyString)
            rItem.sImage = sImagePath + it->image;
        else
            rItem.sImage = wxEmptyString;
        rItem.sTitle = it->title;
        rItem.sTitlenum = it->titlenum;
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

    //// DBG ------------------------------------------------------------------
    //// Dump m_aTree
    //wxLogMessage(_T("[lmBookContentsBox::CreateContents]:"));
    //wxLogMessage(_T("          level  visible open    children  titlenum"));
    //for(int i=0; i < (int)m_aTree.size(); i++) {
    //    wxLogMessage(_T("entry %d : %d      %s      %s      %s      %s"),
    //        i,
    //        m_aTree[i].nLevel,
    //        (m_aTree[i].fOpen ? _T("yes") : _T("no")),
    //        (m_aTree[i].fVisible ? _T("yes") : _T("no")),
    //        (m_aTree[i].fHasChildren ? _T("yes") : _T("no")),
    //        m_aTree[i].sTitlenum );
    //}
    //// DBG end --------------------------------------------------------------

    //re-build page hash table
    m_PagesHash.clear();
    for (long i = 0; i < nNumItems; i++)
    {
        lmBookIndexItem *it = contents[i];
        wxFileName oFN(it->GetFullPath());
        m_PagesHash[oFN.GetFullPath()] = i;
        //wxLogMessage(_T("Full Path = '%s', item=%d"), it->GetFullPath(), i);
    }

    UpdateItemCount();

}

void lmBookContentsBox::UpdateItemCount()
{
    int nEntries = 0;
    for (int i = 0; i < (int)m_aTree.size(); i++)
    {
        if (m_aTree[i].fVisible) nEntries++;
    }
    SetItemCount(nEntries);
}

int lmBookContentsBox::LocateTreeItem(int nEntry) const
{
    //returns index to visible item # nEntry
    int nVisible = -1;
    for (int i = 0; i < (int)m_aTree.size(); i++)
    {
        if (m_aTree[i].fVisible) nVisible++;
        if (nVisible == nEntry) return i;
    }
    return -1;
}

int lmBookContentsBox::LocateEntry(int nTree) const
{
    // Given the index to m_aTree returns the visible entry number
    int nEntry = -1;
    for (int i = 0; i < (int)m_aTree.size(); i++)
    {
        if (m_aTree[i].fVisible) nEntry++;
        if (i == nTree) break;
    }
    return nEntry;


}

wxString lmBookContentsBox::FormatItem(int nTree) const
{
    wxString sLine;
    sLine = _T("<tocitem expand='");

    //If this node has children add expand/collapse icon
    if (m_aTree[nTree].fHasChildren) {
        if (m_aTree[nTree].fOpen) {
            sLine += _T("-' icon='");
            sLine += (m_aTree[nTree].nLevel == 0 ? _T("open_book' ") : _T("open_folder' "));
        }
        else {
            sLine += _T("+' icon='");
            sLine += (m_aTree[nTree].nLevel == 0 ? _T("closed_book' ") : _T("closed_folder' "));
        }
    }
    else {
        if (m_aTree[nTree].nLevel == 0)
            sLine += _T("no' icon='leaflet' ");
        else
            sLine += _T("no' icon='page' ");
    }

    // add image
    if (!(m_aTree[nTree].sImage).IsEmpty()) {
        sLine += _T("img='") + m_aTree[nTree].sImage + _T("' ");
    }

    // add references
    sLine += wxString::Format(_T("level='%d' item='%d'"),
                    m_aTree[nTree].nLevel, nTree );

    // add title number and close the tag
    sLine += _T(" titlenum='") + m_aTree[nTree].sTitlenum + _T("'>");

    // add the title only if no image
    if ((m_aTree[nTree].sImage).IsEmpty()) {
        bool fTitleBold = (m_aTree[nTree].nLevel == 0) && m_aTree[nTree].fHasChildren;
        if (fTitleBold) sLine += _T("<b>");
        sLine += m_aTree[nTree].sTitle;
        if (fTitleBold) sLine += _T("</b>");
    }

    // close item
    sLine += _T("</tocitem>");

    return sLine;

}

void lmBookContentsBox::ChangePage()
{
    if (m_PagesHash.size() > 0)
    {

        wxString page = m_pFrame->GetOpenedPageWithAnchor();
        int nTree = -1;
        if (!page.empty()) {
            wxFileName oFN(page);
            nTree = m_PagesHash[oFN.GetFullPath()];
        }

        if (nTree != -1) {
            EnsureVisible(nTree);
            SelectItem( LocateEntry(nTree) );
        }
    }
}

int lmBookContentsBox::PageNext()
{
    //locate current item
    int nTree = LocateTreeItem( GetSelection() );

    if (nTree != -1) {
        //find the next page
        nTree = FindNextPage(nTree);
        if (nTree != -1) {
            EnsureVisible(nTree);
            SelectItem( LocateEntry(nTree) );
        }
    }
    return nTree;
}

int lmBookContentsBox::FindNextPage(int nTree)
{
    if (nTree != -1) {
        //find the next page
        nTree++;
        if (nTree < (int)m_aTree.GetCount() && m_aTree[nTree].nLevel > 0) {
            while (nTree < (int)m_aTree.GetCount() &&
                   m_aTree[nTree].fHasChildren &&
                   m_aTree[nTree].nLevel > 0)
            {
                nTree++;
            }

            //verify that the item is in current book
            if (m_aTree[nTree].nLevel == 0) nTree = -1;
        }
        else {
            nTree = -1;    //it is the last page of eBook
        }
    }
    return nTree;
}

bool lmBookContentsBox::IsLastPage()
{
    int nTree = LocateTreeItem( GetSelection() );
    return (FindNextPage(nTree) == -1);
}

int lmBookContentsBox::FindPagePrev(int nTree)
{
    if (nTree != -1) {
        //if not the cover page
        if (m_aTree[nTree].nLevel > 0) {
            //find the previous page
            nTree--;
            while (nTree >= 0 && m_aTree[nTree].fHasChildren && m_aTree[nTree].nLevel > 0)
                nTree--;
        }
        else {
            // it is the first page of a book
            nTree = -1;
        }
    }
    return nTree;
}

bool lmBookContentsBox::IsFirstPage()
{
    int nTree = LocateTreeItem( GetSelection() );
    return (FindPagePrev(nTree) == -1);
}

int lmBookContentsBox::PagePrev()
{
    //locate current item
    int nTree = LocateTreeItem( GetSelection() );

    if (nTree != -1) {
        nTree = FindPagePrev(nTree);
        if (nTree != -1) {
            EnsureVisible(nTree);
            SelectItem( LocateEntry(nTree) );
        }
    }
    return nTree;
}

void lmBookContentsBox::Expand(int nItem, bool fRefresh)
{
    int nCurLevel = m_aTree[nItem].nLevel;
    m_aTree[nItem].fOpen = true;     // mark it as 'open'

    //mark all its children as closed but visible
    for(int i=nItem+1; i < (int)m_aTree.GetCount(); i++)
    {
        if (m_aTree[i].nLevel == nCurLevel) break;
        if (m_aTree[i].nLevel == nCurLevel+1) {
            m_aTree[i].fOpen = false;
            m_aTree[i].fVisible = true;
        }
    }
    UpdateItemCount();
    if (fRefresh) RefreshAll();

    //wxLogMessage(_T("[lmBookContentsBox::Expand] Table m_aTree:"));
    //wxLogMessage(_T("i\tlevel    visible\topen\tchildren"));
    //for (long i = 0; i < (int)m_aTree.GetCount(); i++)
    //{
    //    wxLogMessage(_T("%d\t%d\t%s\t%s\t%s"),
    //        i, m_aTree[i].nLevel,
    //        (m_aTree[i].fVisible ? _T("yes") : _T("no")),
    //        (m_aTree[i].fOpen ? _T("yes") : _T("no")),
    //        (m_aTree[i].fHasChildren ? _T("yes") : _T("no")) );
    //}

}

void lmBookContentsBox::Collapse(int nItem)
{
    int nCurLevel = m_aTree[nItem].nLevel;
    m_aTree[nItem].fOpen = false;     // mark it as 'closed'

    //mark all its children as closed and not visible
    for(int i=nItem+1; i < (int)m_aTree.GetCount(); i++)
    {
        if (m_aTree[i].nLevel <= nCurLevel) break;
        m_aTree[i].fOpen = false;
        m_aTree[i].fVisible = false;
    }
    UpdateItemCount();
    RefreshAll();

}
