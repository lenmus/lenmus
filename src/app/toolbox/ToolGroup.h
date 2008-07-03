//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_TOOLGROUP_H__
#define __LM_TOOLGROUP_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolGroup.cpp"
#endif

#include <vector>

class lmToolPage;
class lmCheckButton;

class lmToolGroup: public wxPanel
{    
public:
    lmToolGroup(lmToolPage* pParent);
    virtual ~lmToolGroup();

    //creation
    wxBoxSizer* CreateGroup(wxBoxSizer* pParentSizer, wxString sTitle);

	//info
	int GetGroupWitdh();

protected:

	lmToolPage*		m_pParent;		//owner ToolPage
};



class lmToolButtonsGroup: public lmToolGroup
{    
public:
    lmToolButtonsGroup(lmToolPage* pParent, int nNumButtons, bool fAllowNone,
                       wxBoxSizer* pMainSizer);
    ~lmToolButtonsGroup();

	//buttons
    inline int GetSelectedButton() { return m_nSelButton; }

	void SelectButton(int iB);
    void SelectNextButton();
    void SelectPrevButton();

protected:
    //creation
    virtual void CreateControls(wxBoxSizer* pMainSizer)=0;


    bool                m_fAllowNone;               //allow no button selected
    int                 m_nNumButtons;              //number of buttons in this group
	int			        m_nSelButton;               //selected button (0..n). -1 = none selected
    std::vector<lmCheckButton*> m_pButton;          //buttons
};

#endif   // __LM_TOOLGROUP_H__
