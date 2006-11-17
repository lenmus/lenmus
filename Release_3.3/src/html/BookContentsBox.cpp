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

// to use wxHtmlListBox you must derive a new class from it as you must
// implement pure OnGetItem()


// ============================================================================
// lmBookContentsBox implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(lmBookContentsBox, wxHtmlListBox)

lmBookContentsBox::lmBookContentsBox(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                                     const wxSize& size, long style,
                                     const wxString& name)
             : wxHtmlListBox(parent, id, pos, size, style, name)
{
    SetMargins(5, 5);
    SetItemCount(100);
    //SetSelection(1);
}

lmBookContentsBox::~lmBookContentsBox() 
{
    m_cItems.Clear();
}

wxString lmBookContentsBox::OnGetItem(size_t n) const
{
    if (n >= m_cItems.Count()) return wxEmptyString;
    return m_cItems[n];
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











