//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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

#ifndef __LENMUS_TOOL_SYMBOLS_H__
#define __LENMUS_TOOL_SYMBOLS_H__

//lenmus
#include "lenmus_tool_group.h"
#include "lenmus_tool_page.h"
//#include "../../score/defs.h"

using namespace std;


namespace lenmus
{

class BitmapButton;


//---------------------------------------------------------------------------------------
// Group for texts, symbols & graphic objects
//---------------------------------------------------------------------------------------
//one entry in the buttons table
typedef struct
{
    int         nEventID;
    wxString    sToolTip;
    wxString    sBitmapName;
}
ToolButtonData;

class GrpSymbols: public ToolButtonsGroup
{
public:
    GrpSymbols(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpSymbols() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_Symbols; }

};



//---------------------------------------------------------------------------------------
// Group for figured bass and harmony symbols
//---------------------------------------------------------------------------------------
class GrpHarmony: public ToolButtonsGroup
{
public:
    GrpHarmony(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpHarmony() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_Harmony; }

};



//---------------------------------------------------------------------------------------
// The panel
//---------------------------------------------------------------------------------------
class ToolPageSymbols : public ToolPage
{
	DECLARE_DYNAMIC_CLASS(ToolPageSymbols)

public:
    ToolPageSymbols();
    ToolPageSymbols(wxWindow* parent);
    ~ToolPageSymbols();
    void Create(wxWindow* parent);

    //implementation of virtual methods
    void CreateGroups();

    //interface with symbols group
	inline EToolID GetToolID() { return m_pGrpSymbols->GetSelectedToolID(); }
    inline void SetTool(EToolID nTool) { m_pGrpSymbols->SelectButton(nTool); }

    //current tool/group info
    wxString GetToolShortDescription();

private:

    //groups
    GrpSymbols*           m_pGrpSymbols;
    GrpHarmony*           m_pGrpHarmony;

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_SYMBOLS_H__
