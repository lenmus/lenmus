//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_TOOLBARLINES_H__
#define __LM_TOOLBARLINES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolBarlines.cpp"
#endif

#include "ToolGroup.h"
#include "ToolPage.h"
#include "../../score/defs.h"

class wxBitmapComboBox;
class lmCheckButton;
class wxRadioBox;
class wxListBox;


//--------------------------------------------------------------------------------
// Group for barlines type
//--------------------------------------------------------------------------------
class lmGrpBarlines: public lmToolGroup
{
public:
    lmGrpBarlines(lmToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~lmGrpBarlines() {}

    //implement virtual methods
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_BarlineType; }
    lmEToolID GetCurrentToolID();

	//access to selected barline
	lmEBarline GetSelectedBarline();

	//event handlers
    void OnBarlinesList(wxCommandEvent& event);


private:
    void CreateGroupControls(wxBoxSizer* pMainSizer);

	wxBitmapComboBox*   m_pBarlinesList;

    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------
// The page panel
//--------------------------------------------------------------------------------

class lmToolPageBarlines : public lmToolPage
{
	DECLARE_DYNAMIC_CLASS(lmToolPageBarlines)

public:
    lmToolPageBarlines();
    lmToolPageBarlines(wxWindow* parent);
    ~lmToolPageBarlines();
    void Create(wxWindow* parent);

    //implementation of virtual methods
    void CreateGroups();

    //current tool/group info
    wxString GetToolShortDescription();

    //interface with groups
        //barlines
    inline lmEBarline GetSelectedBarline() { return m_pGrpBarlines->GetSelectedBarline(); }


private:

    //groups
    lmGrpBarlines*		m_pGrpBarlines;

};

#endif    // __LM_TOOLBARLINES_H__
