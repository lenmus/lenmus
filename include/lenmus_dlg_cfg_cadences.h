//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#ifndef __LENMUS_DLG_CFG_CADENCES_H__        //to avoid nested includes
#define __LENMUS_DLG_CFG_CADENCES_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_cadences_constrains.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
// class definition
class DlgCfgIdfyCadence : public wxDialog
{
public:
    DlgCfgIdfyCadence(wxWindow* parent, CadencesConstrains* pConstrains,
                      bool fTheoryMode);
    virtual ~DlgCfgIdfyCadence();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));
    void OnRadAnswerType(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllPerfect(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllPlagal(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllHalf(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllDeceptive(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllMajor(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllMinor(wxCommandEvent& WXUNUSED(event));
    void OnAnswerButton(wxCommandEvent& WXUNUSED(event));

private:
    bool VerifyData();
    void SetCadenceCheckBoxes(int iCad, bool fEnable, bool fChangeCheck=false,
							  bool fNewCheckValue=false);
    void SetAnswerButton(int iButton, bool fEnable);


    CadencesConstrains*   m_pConstrains;      // the constraints to set up
    bool                  m_fTheoryMode;      // exercise type: theory / aural training

    //controls
    wxCheckBox*     m_pChkCadence[k_cadence_max];   // Allowed cadences check boxes
    wxCheckBox*     m_pChkKeySign[k_max_key+1];     // Allowed key signatures check boxes
    wxRadioBox*     m_pBoxShowKey;                  // box with show key radio buttons
    wxRadioBox*     m_pBoxAnswerType;               // box with answer type radio buttons
    wxCheckBox*     m_pChkAnswerButton[5];          // Perfect, Plagal, Half, Deceptive, Imperfect
	wxButton*       m_pBtnCheckAll[5];			    // idem.

    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;
    wxStaticBitmap* m_pBmpAllowedCadencesError;
    wxStaticText*   m_pLblAllowedCadencesError;
    wxStaticBitmap* m_pBmpButtonsError;
    wxStaticText*   m_pLblButtonsError;


    DECLARE_EVENT_TABLE()
};


}   //namespace lenmus

#endif    // __LENMUS_DLG_CFG_CADENCES_H__
