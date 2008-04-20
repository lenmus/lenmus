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

//--------------------------------------------------------------------------------
// Group for clef type
//--------------------------------------------------------------------------------
class lmGrpClefType: public lmToolGroup
{
public:
    lmGrpClefType(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpClefType() {}

	//access to options
	lmEClefType GetClefType();

	void OnButton(wxCommandEvent& event);
	void SelectClef(int iB);

    enum {
        lm_NUM_BUTTONS = 3
    };

private:
    void CreateControls(wxBoxSizer* m_pMainSizer);

	int			        m_nSelButton;               //selected button
	lmCheckButton*		m_pButton[lm_NUM_BUTTONS];  //buttons


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------
// Group for time signature type
//--------------------------------------------------------------------------------
class lmGrpTimeType: public lmToolGroup
{
public:
    lmGrpTimeType(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpTimeType() {}

	void OnButton(wxCommandEvent& event);

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

	wxBitmapButton*		m_pButton[lm_NUM_BUTTONS];  //buttons


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------
// The panel
//--------------------------------------------------------------------------------

class lmToolClef: public lmToolPage
{
public:
    lmToolClef(wxWindow* parent);
    ~lmToolClef();

	//access to options
    inline lmEClefType GetClefType() { m_pGrpClefType->GetClefType(); }


private:

    //groups
    lmGrpClefType*    m_pGrpClefType;
    lmGrpTimeType*    m_pGrpTimeType;

};

#endif    // __LM_TOOLCLEF_H__
