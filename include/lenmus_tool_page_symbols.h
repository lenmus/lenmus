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

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_Symbols; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

};



//---------------------------------------------------------------------------------------
// Group for figured bass and harmony symbols
//---------------------------------------------------------------------------------------
class GrpHarmony: public ToolButtonsGroup
{
public:
    GrpHarmony(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpHarmony() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_Harmony; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

};



//---------------------------------------------------------------------------------------
// The panel
//---------------------------------------------------------------------------------------
class ToolPageSymbols : public ToolPage
{
	wxDECLARE_DYNAMIC_CLASS(ToolPageSymbols);

public:
    ToolPageSymbols() {}
    ToolPageSymbols(wxWindow* parent);
    ~ToolPageSymbols() {}
    void Create(wxWindow* parent);

    //mandatory overrides
    void create_tool_groups();

//    //interface with symbols group
//	inline EToolID GetToolID() { return m_pGrpSymbols->GetSelectedToolID(); }
//    inline void SetTool(EToolID nTool) { m_pGrpSymbols->SelectButton(nTool); }

//    //current tool/group info
//    wxString GetToolShortDescription();


private:
    //mandatory overrides
    long get_key_translation_context() { return k_key_context_symbols; }

//    //groups
//    GrpSymbols*           m_pGrpSymbols;
//    GrpHarmony*           m_pGrpHarmony;

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_SYMBOLS_H__
