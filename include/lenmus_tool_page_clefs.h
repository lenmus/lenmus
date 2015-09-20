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

#ifndef __LENMUS_TOOL_CLEF_H__
#define __LENMUS_TOOL_CLEF_H__

//lomse
#include <lomse_internal_model.h>
using namespace lomse;

//lenmus
#include "lenmus_tool_group.h"
#include "lenmus_tool_page.h"

//wxWidgets
#include <wx/bmpcbox.h>
class wxRadioBox;
class wxListBox;


using namespace std;


namespace lenmus
{

class CheckButton;


//---------------------------------------------------------------------------------------
// Group for clef type
//---------------------------------------------------------------------------------------

class GrpClefType: public ToolGroup
{
public:
    GrpClefType(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpClefType() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_ClefType; }
    EToolID get_selected_tool_id() { return (EToolID)m_pClefList->GetSelection(); }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

	//access to selected clef
	inline EClef GetSelectedClef() { return (EClef)m_pClefList->GetSelection(); }

    //event handlers
    void OnClefList(wxCommandEvent& event);

private:

    void LoadClefList();

	wxBitmapComboBox*   m_pClefList;

    wxDECLARE_EVENT_TABLE();
};


//---------------------------------------------------------------------------------------
// Group for time signature type
//---------------------------------------------------------------------------------------

class GrpTimeType: public ToolButtonsGroup
{
public:
    GrpTimeType(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpTimeType() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_TimeType; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

	void OnButton(wxCommandEvent& event);

    //selected time signature
    int GetTimeBeats();
    int GetTimeBeatType();

};


//---------------------------------------------------------------------------------------
// Group for key signature type
//---------------------------------------------------------------------------------------
class GrpKeyType: public ToolGroup
{
public:
    GrpKeyType(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpKeyType() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    inline EToolGroupID get_group_id() { return k_grp_KeyType; }
    inline EToolID get_selected_tool_id() { return (EToolID)m_pKeyList->GetSelection(); }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

    void OnKeyType(wxCommandEvent& event);
    void OnKeyList(wxCommandEvent& event);

    //selected key
    bool IsMajorKeySignature();
    int GetFifths();

    //keys data
    typedef struct lmKeysStruct
    {
        wxString            sKeyName;
        int                 nFifths;
        EKeySignature    nKeyType;

    } lmKeysData;

private:
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    void LoadKeyList(int nType);
    void NotifyToolChange();

    wxRadioButton*      m_pKeyRad[2];   //rad.buttons for Major/Minor selection
	wxBitmapComboBox*   m_pKeyList;
    wxButton*           m_pBtnAddKey;

    wxDECLARE_EVENT_TABLE();
};


//---------------------------------------------------------------------------------------
// The page
//---------------------------------------------------------------------------------------
class ToolPageClefs : public ToolPage
{
	wxDECLARE_DYNAMIC_CLASS(ToolPageClefs);

public:
    ToolPageClefs() {}
    ToolPageClefs(wxWindow* parent);
    ~ToolPageClefs() {}
    void Create(wxWindow* parent);

    //mandatory overrides
    void create_tool_groups();

//    //current tool/group info
//    wxString GetToolShortDescription();

//    //interface with groups
//        //clefs
//    inline EClef GetSelectedClef() { return m_pGrpClefType->GetSelectedClef(); }
//        //time signatures
//    inline int GetTimeBeats() { return m_pGrpTimeType->GetTimeBeats(); }
//    inline int GetTimeBeatType() { return m_pGrpTimeType->GetTimeBeatType(); }
//        //key signatures
//    inline bool IsMajorKeySignature() { return m_pGrpKeyType->IsMajorKeySignature(); }
//    inline int GetFifths() { return m_pGrpKeyType->GetFifths(); }

protected:
    //mandatory overrides
    long get_key_translation_context() { return k_key_context_clefs; }


//    //groups
//    GrpClefType*      m_pGrpClefType;
//    GrpTimeType*      m_pGrpTimeType;
//    GrpKeyType*       m_pGrpKeyType;

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_CLEF_H__
