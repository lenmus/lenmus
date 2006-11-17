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

#include "wx/htmllbox.h"
#include "wx/treectrl.h"

class lmBookContentsBox : public wxHtmlListBox
{
public:
    lmBookContentsBox() { }
    lmBookContentsBox(wxWindow* parent, wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = 0, const wxString& name = wxHtmlListBoxNameStr);
    ~lmBookContentsBox();

    // wxTreeCtrol compatibility methods
    long AddRoot(const wxString& text, int image = -1, int selImage = -1,
                    wxTreeItemData* data = NULL);
    long AppendItem(const long& parent, const wxString& text, wxString& sImgPath,
                    int image = -1, int selImage = -1, wxTreeItemData* data = NULL);
    void AssignImageList(wxImageList* imageList);
    void DeleteAllItems();
    void EnsureVisible(const long& item);
    wxTreeItemData* GetItemData(const long& item) const;
    void SelectItem(const long& item, bool select = true);
    void SetItemBold(const long& item, bool bold = true);
    void SetItemImage(const long& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);


private:
    wxString OnGetItem(size_t n) const;


    wxArrayString   m_cItems;       // items to display


    DECLARE_NO_COPY_CLASS(lmBookContentsBox)
    DECLARE_DYNAMIC_CLASS(lmBookContentsBox)
};

#endif  // __LM_BOOKCONTENTSBOX_H__

