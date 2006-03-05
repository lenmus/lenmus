// RCS-ID: $Id: DlgCfgScoreReading.h,v 1.7 2006/02/23 19:15:54 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file DlgCfgScoreReading.h
    @brief Header file for class lmDlgCfgScoreReading
    @ingroup app_gui
*/
#ifndef __DLGCFGSCOREREADING_H__        //to avoid nested includes
#define __DLGCFGSCOREREADING_H__

// GCC interface
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "DlgCfgScoreReading.h"
#endif

// headers
#include "wx/dialog.h"
#include "wx/spinctrl.h"        //to use wxSpinCtrl

#include "../exercises/ScoreConstrains.h"

//Dialog types
enum EScoreReadingDlg
{
    eDlgNotesReading = 0,       //single_notes_reading key
    eDlgSideReading,            //single_side_reading key
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

    lmScoreConstrains*  m_pConstrains;      // the constrains to set up
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

#endif    // __DLGCFGSCOREREADING_H__
