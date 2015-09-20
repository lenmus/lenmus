//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

#ifndef __LENMUS_DLG_CFG_THEO_INTERVALS_H__        //to avoid nested includes
#define __LENMUS_DLG_CFG_THEO_INTERVALS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_constrains.h"
#include "lenmus_theo_intervals_constrains.h"

//wxWidgets
#include <wx/dialog.h>

class wxButton;
class wxCheckBox;
class wxComboBox;
class wxNotebook;
class wxPanel;
class wxRadioBox;
class wxStaticBitmap;
class wxStaticText;
class wxWindow;


namespace lenmus
{

//---------------------------------------------------------------------------------------
class DlgCfgTheoIntervals : public wxDialog
{

public:
    DlgCfgTheoIntervals(wxWindow* parent, TheoIntervalsConstrains* pConstrains);
    virtual ~DlgCfgTheoIntervals();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnControlClicked(wxCommandEvent& WXUNUSED(event));

private:
    bool VerifyData();
    void create_controls();

    TheoIntervalsConstrains*   m_pConstrains; // the constraints to set up

    wxStaticBoxSizer*   m_pBoxIntvalTypes;

    // ATTENTION: Following code has been generated with wxFormBuider.
    // Replaced m_pChkClef, m_pChkIntvalType and m_pChkKeySign by arrays
		wxNotebook* noteBook;
		wxPanel* m_pPanelOther;
		wxRadioBox* m_pRadLevel;
		wxRadioBox* m_pRadProblemType;
		wxStaticText* wxID_STATIC2;
		wxChoice* m_pCboAboveLines;
		wxStaticText* wxID_STATIC3;
		wxChoice* m_pCboBelowLines;
		wxCheckBox* m_pChkIntvalType[3];
		wxStaticBitmap* m_pBmpIntvalTypeError;
		wxStaticText* wxID_STATIC4;
		wxStaticText* m_pLblIntvalTypeError;
		wxPanel* m_pPanelClefs;
		wxCheckBox* m_pChkClef[7];
		wxStaticBitmap* m_pBmpClefError;
		wxStaticText* m_pLblClefError;
		wxStaticText* m_pSpace5;
		wxPanel* m_pPanelKeys;
		wxCheckBox* m_pChkKeySign[15];
		wxStaticBitmap* m_pBmpKeySignError;
		wxStaticText* wxID_STATIC1;
		wxStaticText* m_pLblKeySignError;
		wxButton* m_pBtnAccept;
		wxButton* m_pBtnCancel;

    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_DLG_CFG_THEO_INTERVALS_H__
