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

#ifndef __LENMUS_DLG_CFG_IDFY_CHORD_H__        //to avoid nested includes
#define __LENMUS_DLG_CFG_IDFY_CHORD_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_chords_constrains.h"

//wxWidgets
#include <wx/dialog.h>
#include <wx/spinctrl.h>        //to use wxSpinCtrl



namespace lenmus
{

//---------------------------------------------------------------------------------------
class DlgCfgIdfyChord : public wxDialog
{
public:
    DlgCfgIdfyChord(wxWindow* parent, ChordConstrains* pConstrains,
                      bool fTheoryMode);
    virtual ~DlgCfgIdfyChord();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));


private:
    bool VerifyData();

    ChordConstrains*  m_pConstrains;          // the constraints to set up
    bool                m_fTheoryMode;

    //controls
    wxCheckBox*     m_pChkAllowInversions;
    wxCheckBox*     m_pChkDisplayKey;
    wxCheckBox*     m_pChkPlayMode[3];          // allowed play modes
    wxCheckBox*     m_pChkChord[ect_MaxInExercises];       // Allowed chords check boxes
    wxCheckBox*     m_pChkKeySign[k_key_F+1];    // Allowed key signatures check boxes
    wxStaticBox*    m_pBoxPlayModes;            // box with play mode check boxes

    wxStaticBitmap* m_pBmpPlayModeError;        // error icons and messages
    wxStaticText*   m_pLblPlayModeError;
    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;
    wxStaticBitmap* m_pBmpAllowedChordsError;
    wxStaticText*   m_pLblAllowedChordsError;


    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_DLG_CFG_IDFY_CHORD_H__
