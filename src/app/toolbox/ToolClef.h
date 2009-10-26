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

#ifndef __LM_TOOLCLEF_H__
#define __LM_TOOLCLEF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolClef.cpp"
#endif

#include "ToolGroup.h"
#include "ToolPage.h"
#include "../../score/defs.h"

class wxBitmapComboBox;
class lmCheckButton;
class wxRadioBox;
class wxListBox;


//--------------------------------------------------------------------------------
// Group for clef type
//--------------------------------------------------------------------------------

#define lmUSE_CLEF_COMBO    1       //use combo (1) or buttons (0)

//class to implement to tool group
class lmGrpClefType: public lmToolGroup
{
public:
    lmGrpClefType(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpClefType() {}

    //implement virtual methods
    void CreateControls(wxBoxSizer* m_pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_ClefType; }

	//access to selected clef
	lmEClefType GetSelectedClef();

#if lmUSE_CLEF_COMBO

    //event handlers
    void OnClefList(wxCommandEvent& event);

private:

    void LoadClefList();

	wxBitmapComboBox*   m_pClefList;

    DECLARE_EVENT_TABLE()
#endif

};


//--------------------------------------------------------------------------------
// Group for time signature type
//--------------------------------------------------------------------------------
class lmGrpTimeType: public lmToolGroup
{
public:
    lmGrpTimeType(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpTimeType() {}

    //implement virtual methods
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_TimeType; }

	void OnButton(wxCommandEvent& event);

    //selected time signature
    int GetTimeBeats();
    int GetTimeBeatType();

    enum {
        lm_NUM_BUTTONS = 12
    };

    //buttons data
    typedef struct lmButtonStruct
    {
        wxString    sBitmap;
        int         nBeats;
	    int     	nBeatType;

    } lmButton;

private:
    void CreateControls(wxBoxSizer* m_pMainSizer);

	wxBitmapButton* m_pButton[lm_NUM_BUTTONS];  //buttons
	int             m_nSelButton;               //selected button (0..n). -1 = none selected


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------
// Group for key signature type
//--------------------------------------------------------------------------------
class lmGrpKeyType: public lmToolGroup
{
public:
    lmGrpKeyType(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpKeyType() {}

    //implement virtual methods
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_KeyType; }

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
        lmEKeySignatures    nKeyType;

    } lmKeysData;

private:
    void CreateControls(wxBoxSizer* m_pMainSizer);
    void LoadKeyList(int nType);
    void NotifyToolChange();

    wxRadioButton*      m_pKeyRad[2];   //rad.buttons for Major/Minor selection
	wxBitmapComboBox*   m_pKeyList;
    wxButton*           m_pBtnAddKey;

    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------
// The page
//--------------------------------------------------------------------------------

class lmToolPageClefs : public lmToolPage
{
	DECLARE_DYNAMIC_CLASS(lmToolPageClefs)

public:
    lmToolPageClefs();
    lmToolPageClefs(wxWindow* parent);
    ~lmToolPageClefs();
    void Create(wxWindow* parent);

    //implementation of virtual methods
    lmToolGroup* GetToolGroup(lmEToolGroupID nGroupID);
    void CreateGroups();
    bool DeselectRelatedGroups(lmEToolGroupID nGroupID);

    //current tool/group info
    wxString GetToolShortDescription();

    //interface with groups
        //clefs
    inline lmEClefType GetSelectedClef() { return m_pGrpClefType->GetSelectedClef(); }
        //time signatures
    inline int GetTimeBeats() { return m_pGrpTimeType->GetTimeBeats(); }
    inline int GetTimeBeatType() { return m_pGrpTimeType->GetTimeBeatType(); }
        //key signatures
    inline bool IsMajorKeySignature() { return m_pGrpKeyType->IsMajorKeySignature(); }
    inline int GetFifths() { return m_pGrpKeyType->GetFifths(); }

private:

    //groups
    lmGrpClefType*      m_pGrpClefType;
    lmGrpTimeType*      m_pGrpTimeType;
    lmGrpKeyType*       m_pGrpKeyType;

};

#endif    // __LM_TOOLCLEF_H__
