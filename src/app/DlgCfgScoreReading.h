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

#ifndef __LM_DLGCFGSCOREREADING_H__        //to avoid nested includes
#define __LM_DLGCFGSCOREREADING_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgCfgScoreReading.cpp"
#endif

// headers
#include "wx/dialog.h"
#include "wx/spinctrl.h"        //to use wxSpinCtrl
#include "wx/notebook.h"

#include "../exercises/ScoreConstrains.h"

//Dialog types
enum EScoreReadingDlg
{
    eDlgNotesReading = 0,       //single_clefs_reading key
    eDlgMusicReading,            //single_music_reading key
    eDlgCfgError,               //bad key           
};

// class definition
class lmDlgCfgScoreReading:public wxDialog {

public:
    lmDlgCfgScoreReading(wxWindow * parent, lmScoreConstrains* pConstrains,
                         wxString sSettingsKey);
    virtual ~lmDlgCfgScoreReading();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnControlClicked(wxCommandEvent& WXUNUSED(event));

private:
    bool VerifyData();

    lmScoreConstrains*  m_pConstrains;      // the constraints to set up
    wxString            m_sSettingsKey;     // key to use (implies type of dialog)
    EScoreReadingDlg    m_nDialogType;      // type of dialog
    wxNotebook*         m_pBook;

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

    wxCheckBox*     m_pChkTime[lmMAX_TIME_SIGN - lmMIN_TIME_SIGN + 1]; // Allowed time signatures check boxes

    // page 2: key signatures

    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;

    wxCheckBox*     m_pChkKeySign[earmFa+1];        // Allowed key signatures check boxes


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGCFGSCOREREADING_H__
