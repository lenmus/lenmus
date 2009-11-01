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

#ifndef __LM_TOOLSYMBOLS_H__
#define __LM_TOOLSYMBOLS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolSymbols.cpp"
#endif

#include "ToolGroup.h"
#include "ToolPage.h"
#include "../../score/defs.h"

class lmBitmapButton;


//--------------------------------------------------------------------------------
// Group for texts, figured bass, symbols & graphic objects
//--------------------------------------------------------------------------------
//one entry in the buttons table
typedef struct
{
    int         nEventID;
    wxString    sToolTip;
    wxString    sBitmapName;
}
lmToolButtonData;

class lmGrpSymbols: public lmToolButtonsGroup
{
public:
    lmGrpSymbols(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpSymbols() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_Symbols; }

};



//--------------------------------------------------------------------------------
// The panel
//--------------------------------------------------------------------------------

class lmToolPageSymbols : public lmToolPage
{
	DECLARE_DYNAMIC_CLASS(lmToolPageSymbols)

public:
    lmToolPageSymbols();
    lmToolPageSymbols(wxWindow* parent);
    ~lmToolPageSymbols();
    void Create(wxWindow* parent);

    //implementation of virtual methods
    lmToolGroup* GetToolGroup(lmEToolGroupID nGroupID);
    void CreateGroups();
    bool DeselectRelatedGroups(lmEToolGroupID nGroupID);

    //interface with symbols group
	inline lmEToolID GetToolID() { return m_pGrpSymbols->GetSelectedToolID(); }
    inline void SetTool(lmEToolID nTool) { m_pGrpSymbols->SelectButton(nTool); }

    //current tool/group info
    wxString GetToolShortDescription();

private:

    //groups
    lmGrpSymbols*           m_pGrpSymbols;

};

#endif    // __LM_TOOLSYMBOLS_H__
