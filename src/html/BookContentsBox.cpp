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

lmBookContentsBox::lmBookContentsBox(wxWindow* parent,
                                     wxFrame* pFrame,
                                     wxWindowID id, 
                                     const wxPoint& pos,
                                     const wxSize& size, long style,
                                     const wxString& name)
             : lmHtmlListBox(parent, id, pos, size, style, name)
{
    m_pFrame = pFrame;

    SetSelectionBackground(*wxWHITE);

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
    //SetSelection(item);
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
    m_PagesHash.clear();
    for (long i = 0; i < nNumItems; i++)
    {
        lmBookIndexItem *it = &contents[i];
        wxFileName oFN(it->GetFullPath());
        m_PagesHash[oFN.GetFullPath()] = i;
        //wxLogMessage(_T("Full Path = '%s', item=%d"), it->GetFullPath(), i);
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
#if 0
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
#else
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
        sLine += _T("no' icon='page' ");
    }

    // add image
    if (!(m_aTree[nTree].sImage).IsEmpty()) {
        sLine += _T("img='") + m_aTree[nTree].sImage + _T("' ");
    }

    // add references
    sLine += wxString::Format(_T("level='%d' item='%d'>"),
                    m_aTree[nTree].nLevel, nTree );

    // add the title
    sLine += m_aTree[nTree].sTitle + _T("</tocitem>");

    return sLine;

#endif
}

void lmBookContentsBox::ChangePage()
{
    if (m_PagesHash.size() > 0)
    {

        wxString page = ((lmTextBookFrame*)m_pFrame)->GetOpenedPageWithAnchor();
        int nTree = -1;
        if (!page.empty()) {
            wxFileName oFN(page);
            nTree = m_PagesHash[oFN.GetFullPath()];

            //// iterate over all the elements in the class
            //lmPagesHash::iterator it;
            //for( it = m_PagesHash.begin(); it != m_PagesHash.end(); ++it )
            //{
            //    wxLogMessage(_T("key='%s', value=%d"), it->first, it->second);
            //}
        }

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
        if (m_aTree[i].nLevel == nCurLevel+1) {
            m_aTree[i].fOpen = false;
            m_aTree[i].fVisible = true;
        }
    }
    RefreshAll();

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
