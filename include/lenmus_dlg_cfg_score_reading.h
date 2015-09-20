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

#ifndef __LENMUS_DLG_CFG_SCORE_READING_H__        //to avoid nested includes
#define __LENMUS_DLG_CFG_SCORE_READING_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_scores_constrains.h"

//wxWidgets
#include <wx/dialog.h>
#include <wx/spinctrl.h>        //to use wxSpinCtrl
#include <wx/notebook.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
//Dialog types
enum EScoreReadingDlg
{
    eDlgNotesReading = 0,       //single_clefs_reading key
    eDlgMusicReading,            //single_music_reading key
    eDlgCfgError,               //bad key
};

// class definition
class DlgCfgScoreReading : public wxDialog {

public:
    DlgCfgScoreReading(wxWindow * parent, ScoreConstrains* pConstrains,
                       wxString sSettingsKey);
    virtual ~DlgCfgScoreReading();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnControlClicked(wxCommandEvent& WXUNUSED(event));

private:
    bool VerifyData();

    ScoreConstrains*  m_pConstrains;      // the constraints to set up
    wxString            m_sSettingsKey;     // key to use (implies type of dialog)
    EScoreReadingDlg    m_nDialogType;      // type of dialog
    wxNotebook*         m_pBook;

    bool        m_fCreatingDlg;         //to avoid problems at dlg creation

    //
    //controls
    //

    //page 0: clefs & notes

    wxCheckBox*     m_pChkClef[7];              // Allowed clefs check boxes
    wxComboBox*     m_pCboMinNote[7];
    wxComboBox*     m_pCboMaxNote[7];
    wxSpinCtrl*     m_pSpinMaxInterval;

    wxStaticBitmap* m_pBmpClefError;            // error icons and messages
    wxStaticText*   m_pLblClefError;
    wxStaticBitmap* m_pBmpRangeError;
    wxStaticText*   m_pLblRangeError;
    wxStaticBitmap* m_pBmpErrorRange[7];

    // page 1: time signatures

    wxStaticText*   m_pLblTimeError;            // error icons and messages
    wxStaticBitmap* m_pBmpTimeError;

    wxCheckBox*     m_pChkTime[k_max_time_signature - k_min_time_signature + 1]; // Allowed time signatures check boxes

    // page 2: key signatures

    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;

    wxCheckBox*     m_pChkKeySign[k_key_F+1];        // Allowed key signatures check boxes


    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_DLG_CFG_SCORE_READING_H__
