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

#ifndef __LENMUS_TOOL_CLEF_H__
#define __LENMUS_TOOL_CLEF_H__

//lomse
#include "lomse_internal_model.h"
using namespace lomse;

//lenmus
#include "lenmus_tool_group.h"
#include "lenmus_tool_page.h"
//#include "../../score/defs.h"

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

#define lmUSE_CLEF_COMBO    1       //use combo (1) or buttons (0)

//---------------------------------------------------------------------------------------
//class to implement the tool group for clefs
class GrpClefType: public ToolGroup
{
public:
    GrpClefType(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpClefType() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_ClefType; }
    inline EToolID GetCurrentToolID() { return (EToolID)m_pClefList->GetSelection(); }

	//access to selected clef
	inline EClef GetSelectedClef() { return (EClef)m_pClefList->GetSelection(); }

#if lmUSE_CLEF_COMBO

    //event handlers
    void OnClefList(wxCommandEvent& event);

private:

    void LoadClefList();

	wxBitmapComboBox*   m_pClefList;

    DECLARE_EVENT_TABLE()
#endif

};


//---------------------------------------------------------------------------------------
// Group for time signature type
//---------------------------------------------------------------------------------------

#define lmUSE_TIME_OLD    0       //use ToolButtonsGroup (0) or ToolGroup (1)

#if lmUSE_TIME_OLD
class GrpTimeType: public ToolGroup
{
public:
    GrpTimeType(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpTimeType() {}

    //implement virtual methods
    inline EToolGroupID GetToolGroupID() { return k_grp_TimeType; }
    inline EToolID GetCurrentToolID() { return (EToolID)m_nSelButton; }

	void OnButton(wxCommandEvent& event);

    //selected time signature
    int GetTimeBeats();
    int GetTimeBeatType();

private:
    void CreateGroupControls(wxBoxSizer* pMainSizer);

	wxBitmapButton* m_pButton[12];      //buttons
	int             m_nSelButton;       //selected button (0..n). -1 = none selected


    DECLARE_EVENT_TABLE()
};

#else
class GrpTimeType: public ToolButtonsGroup
{
public:
    GrpTimeType(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpTimeType() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_TimeType; }

	void OnButton(wxCommandEvent& event);

    //selected time signature
    int GetTimeBeats();
    int GetTimeBeatType();

};

#endif


//---------------------------------------------------------------------------------------
// Group for key signature type
//---------------------------------------------------------------------------------------
class GrpKeyType: public ToolGroup
{
public:
    GrpKeyType(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpKeyType() {}

    //implement virtual methods
    inline EToolGroupID GetToolGroupID() { return k_grp_KeyType; }
    inline EToolID GetCurrentToolID() { return (EToolID)m_pKeyList->GetSelection(); }

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
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    void LoadKeyList(int nType);
    void NotifyToolChange();

    wxRadioButton*      m_pKeyRad[2];   //rad.buttons for Major/Minor selection
	wxBitmapComboBox*   m_pKeyList;
    wxButton*           m_pBtnAddKey;

    DECLARE_EVENT_TABLE()
};


//---------------------------------------------------------------------------------------
// The page
//---------------------------------------------------------------------------------------
class ToolPageClefs : public ToolPage
{
	DECLARE_DYNAMIC_CLASS(ToolPageClefs)

public:
    ToolPageClefs();
    ToolPageClefs(wxWindow* parent);
    ~ToolPageClefs();
    void Create(wxWindow* parent);

    //implementation of virtual methods
    void CreateGroups();

    //current tool/group info
    wxString GetToolShortDescription();

    //interface with groups
        //clefs
    inline EClef GetSelectedClef() { return m_pGrpClefType->GetSelectedClef(); }
        //time signatures
    inline int GetTimeBeats() { return m_pGrpTimeType->GetTimeBeats(); }
    inline int GetTimeBeatType() { return m_pGrpTimeType->GetTimeBeatType(); }
        //key signatures
    inline bool IsMajorKeySignature() { return m_pGrpKeyType->IsMajorKeySignature(); }
    inline int GetFifths() { return m_pGrpKeyType->GetFifths(); }

private:

    //groups
    GrpClefType*      m_pGrpClefType;
    GrpTimeType*      m_pGrpTimeType;
    GrpKeyType*       m_pGrpKeyType;

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_CLEF_H__
