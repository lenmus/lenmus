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

#include "lenmus_content_box.h"
#include "lenmus_standard_header.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

#include "lenmus_content_box_ctrol.h"
#include "lenmus_document_frame.h"


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(lmTreeArray)


// to use ContentBoxCtrol you must derive a new class from it as you must
// implement pure OnGetItem()

namespace lenmus
{


//=======================================================================================
// BookContentBox implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(BookContentBox, ContentBoxCtrol)
    EVT_HTML_LINK_CLICKED(wxID_ANY, BookContentBox::OnContentsLinkClicked)
wxEND_EVENT_TABLE()


BookContentBox::BookContentBox(wxWindow* parent, DocumentFrame* pFrame,
                               ApplicationScope& appScope,
                               wxWindowID id, const wxPoint& pos,
                               const wxSize& size, long style,
                               const wxString& name)
    : ContentBoxCtrol(parent, appScope, id, pos, size, style, name)
    , m_pFrame(pFrame)
{
    SetSelectionBackground(*wxWHITE);

    SetMargins(5, 0);       //5px left, 0px items separation
    SetItemCount(1);
    //SetSelection(1);
}

//---------------------------------------------------------------------------------------
BookContentBox::~BookContentBox()
{
    m_aTree.Clear();

}

//---------------------------------------------------------------------------------------
wxString BookContentBox::OnGetItem(size_t n) const
{
    int i = LocateTreeItem(int(n));
    if (i == -1) return wxEmptyString;
    return FormatItem(i);

}

//---------------------------------------------------------------------------------------
int BookContentBox::find_page(const wxString& fullpath)
{
    //returns page number 0...n-1 or .1 if not found

    const lmPagesHash::iterator it = m_PagesHash.find(fullpath);
    if (it == m_PagesHash.end())
        return -1;
    else
        return it->second;
}

//---------------------------------------------------------------------------------------
void BookContentBox::OnContentsLinkClicked(wxHtmlLinkEvent& event)
{
    //m_pFrame->OnContentsLinkClicked(event);
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

    if (sLink.StartsWith("item", &sItem)) {
        nAction = eGo;
    }
    else if (sLink.StartsWith("open&go", &sItem)) {
        nAction = eOpenGo;
    }
    else if (sLink.StartsWith("close&go", &sItem)) {
        nAction = eCloseGo;
    }
    else if (sLink.StartsWith("close&blank", &sItem)) {
        nAction = eCloseBlank;
    }
    else if (sLink.StartsWith("open", &sItem)) {
        nAction = eOpen;
    }
    else if (sLink.StartsWith("close", &sItem)) {
        nAction = eClose;
    }
    else
    {
        LOMSE_LOG_ERROR("Invalid link type");
        return;
    }

    long nItem;
    if (!sItem.ToLong(&nItem))
    {
        LOMSE_LOG_ERROR("Invalid link number");
        return;
    }

    switch (nAction)
    {
        case eGo:
            m_pFrame->load_page(nItem);
            break;

        case eOpen:
            Expand(nItem);
            break;

        case eClose:
            Collapse(nItem);
            break;

        case eOpenGo:
            Expand(nItem);
            m_pFrame->load_page(nItem);
            break;

        case eCloseGo:
            m_pFrame->load_page(nItem);
            Collapse(nItem);
            break;

        case eCloseBlank:
            m_pFrame->load_page(nItem);
            Collapse(nItem);
            break;

        default:
            wxASSERT(false);
    }
}


// ============================================================================
// Overrides
// ============================================================================

void BookContentBox::DoHandleItemClick(int item, int WXUNUSED(flags))
{
    // the item should become the current one only if it is a final node
    int nItem = LocateTreeItem(item);
    if (nItem > 0 && (m_aTree[nItem].nLevel==0 || !m_aTree[nItem].fHasChildren))
        SetSelection(item);

}


// ============================================================================
// wxTreeCtrol compatibility methods
// ============================================================================

void BookContentBox::DeleteAllItems()
{
    //Deletes all items in the control.
    m_aTree.Clear();
}

void BookContentBox::EnsureVisible(const long& nTree)
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

wxTreeItemData* BookContentBox::GetItemData(const long& WXUNUSED(item)) const
{
    //Returns the tree item data associated with the item.
    return (wxTreeItemData*)nullptr;
}

void BookContentBox::SelectItem(const long& item, bool WXUNUSED(select))
{
    //Selects the given item. In multiple selection controls, can be also used
    //to deselect a currently selected item if the value of select is false.
    SetSelection(item);
}

void BookContentBox::SetItemBold(const long& WXUNUSED(item), bool WXUNUSED(bold))
{
    //Makes item appear in bold font if bold parameter is true or resets it to
    //the normal state.
}

void BookContentBox::SetItemImage(const long& WXUNUSED(item), int WXUNUSED(image),
                                     wxTreeItemIcon WXUNUSED(which))
{
    //Sets the specified item image. See GetItemImage for the description of
    //the which parameter.
}



// ============================================================================
// contents creation and management
// ============================================================================

void BookContentBox::CreateContents(BooksCollection* pBooksData)
{
    DeleteAllItems();   //remove all previous contents
    const BookIndexArray& contents = pBooksData->GetContentsArray();
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
        BookIndexItem *it = contents[i];

        // set path for images
        if (it->level == 0)
        {
            sImagePath = (it->pBookRecord)->GetBasePath();
            if (sImagePath.Contains("content/") )
                sImagePath.Replace("content/", "images/");
        }

        lmTreeContentRecord rItem;
        rItem.nLevel = it->level;
        rItem.fVisible = (it->level <= 1);  //level 0 & 1 visible
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
                m_aTree[nParent].fOpen = (it->level == 0);      //open level 0
            }
        }
        nLevelPrev = rItem.nLevel;

    }

//    // DBG ------------------------------------------------------------------
//    // Dump m_aTree
//    wxLogMessage("[BookContentBox::CreateContents]:");
//    wxLogMessage("          level  visible open    children  titlenum");
//    for(int i=0; i < (int)m_aTree.size(); i++) {
//        wxLogMessage("entry %d : %d      %s      %s      %s      %s",
//            i,
//            m_aTree[i].nLevel,
//            (m_aTree[i].fOpen ? "yes" : "no"),
//            (m_aTree[i].fVisible ? "yes" : "no"),
//            (m_aTree[i].fHasChildren ? "yes" : "no"),
//            m_aTree[i].sTitlenum.wx_str() );
//    }
//    // DBG end --------------------------------------------------------------

    //re-build page hash table
    m_PagesHash.clear();
    for (long i = 0; i < nNumItems; i++)
    {
        BookIndexItem *it = contents[i];
        wxFileName oFN(it->GetFullPath());
        m_PagesHash[oFN.GetFullPath()] = i;
//        wxLogMessage("Full Path = '%s', item=%d", it->GetFullPath().wx_str(), i);
    }

    UpdateItemCount();

}

void BookContentBox::UpdateItemCount()
{
    int nEntries = 0;
    for (int i = 0; i < (int)m_aTree.size(); i++)
    {
        if (m_aTree[i].fVisible) nEntries++;
    }
    SetItemCount(nEntries);
}

int BookContentBox::LocateTreeItem(int nEntry) const
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

int BookContentBox::LocateEntry(int nTree) const
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

wxString BookContentBox::FormatItem(int nTree) const
{
    wxString sLine;
    sLine = "<tocitem expand='";

    //If this node has children add expand/collapse icon
    if (m_aTree[nTree].fHasChildren) {
        if (m_aTree[nTree].fOpen) {
            sLine += "-' icon='";
            sLine += (m_aTree[nTree].nLevel == 0 ? "open_book' " : "open_folder' ");
        }
        else {
            sLine += "+' icon='";
            sLine += (m_aTree[nTree].nLevel == 0 ? "closed_book' " : "closed_folder' ");
        }
    }
    else {
        if (m_aTree[nTree].nLevel == 0)
            sLine += "no' icon='leaflet' ";
        else
            sLine += "no' icon='page' ";
    }

    // add image
    if (!(m_aTree[nTree].sImage).IsEmpty()) {
        sLine += "img='" + m_aTree[nTree].sImage + "' ";
    }

    // add references
    sLine += wxString::Format("level='%d' item='%d'",
                    m_aTree[nTree].nLevel, nTree );

    // add title number and close the tag
    sLine += " titlenum='" + m_aTree[nTree].sTitlenum + "'>";

    // add the title only if no image
    if ((m_aTree[nTree].sImage).IsEmpty()) {
        bool fTitleBold = (m_aTree[nTree].nLevel == 0) && m_aTree[nTree].fHasChildren;
        if (fTitleBold) sLine += "<b>";
        sLine += m_aTree[nTree].sTitle;
        if (fTitleBold) sLine += "</b>";
    }

    // close item
    sLine += "</tocitem>";

    return sLine;

}

//void BookContentBox::ChangePage()
//{
//    if (m_PagesHash.size() > 0)
//    {
//
//        wxString page = m_pFrame->GetOpenedPageWithAnchor();
//        int nTree = -1;
//        if (!page.empty())
//        {
//            wxFileName oFN(page);
//            nTree = m_PagesHash[oFN.GetFullPath()];
//        }
//
//        if (nTree != -1)
//        {
//            EnsureVisible(nTree);
//            SelectItem( LocateEntry(nTree) );
//        }
//    }
//}

int BookContentBox::PageNext()
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

int BookContentBox::FindNextPage(int nTree)
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

bool BookContentBox::IsLastPage()
{
    int nTree = LocateTreeItem( GetSelection() );
    return (FindNextPage(nTree) == -1);
}

int BookContentBox::FindPagePrev(int nTree)
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

bool BookContentBox::IsFirstPage()
{
    int nTree = LocateTreeItem( GetSelection() );
    return (FindPagePrev(nTree) == -1);
}

int BookContentBox::PagePrev()
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

void BookContentBox::Expand(int nItem, bool fRefresh)
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

//    wxLogMessage("[BookContentBox::Expand] Table m_aTree:");
//    wxLogMessage(_T("i\tlevel    visible\topen\tchildren"));
//    for (long i = 0; i < (int)m_aTree.GetCount(); i++)
//    {
//        wxLogMessage(_T("%d\t%d\t%s\t%s\t%s"),
//            i, m_aTree[i].nLevel,
//            (m_aTree[i].fVisible ? "yes" : "no"),
//            (m_aTree[i].fOpen ? "yes" : "no"),
//            (m_aTree[i].fHasChildren ? "yes" : "no") );
//    }

}

void BookContentBox::Collapse(int nItem)
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


}   //namespace lenmus
