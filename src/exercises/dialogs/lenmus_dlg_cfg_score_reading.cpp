//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

//lenmus
#include "lenmus_dlg_cfg_score_reading.h"

#include "lenmus_utilities.h"

//lomse
#include <lomse_pitch.h>
#include <lomse_analyser.h>
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/xrc/xmlres.h>


namespace lenmus
{


/*
    This dialog is called from to different exercises:
        - Notes reading (key = "single_clefs_reading")
        - Music reading (user configured) (key="single_music_reading")

    For 'Clefs reading' the dialog allows user to configure:
        - Clefs and notes range
        - Key signatures
        - Max interval

    For 'Music Reading' the dialog allows user to configure all parameters
    and to invoke the Pattern Editor Dialog
*/

//---------------------------------------------------------------------------------------
enum {
	ID_BOOK = 100,
};

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(DlgCfgScoreReading, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), DlgCfgScoreReading::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), DlgCfgScoreReading::OnCancelClicked )

    //
    // panel 0: Clefs and notes
    //

    // Clefs check boxes
    EVT_CHECKBOX( XRCID( "chkGClef" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkF4Clef" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkF3Clef" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkC1Clef" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkC2Clef" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkC3Clef" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkC4Clef" ), DlgCfgScoreReading::OnControlClicked )

    // combos for min and max notes
    EVT_TEXT( XRCID( "cboMinNoteG" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMinNoteF4" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMinNoteF3" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMinNoteC1" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMinNoteC2" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMinNoteC3" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMinNoteC4" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMaxNoteG" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMaxNoteF4" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMaxNoteF3" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMaxNoteC1" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMaxNoteC2" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMaxNoteC3" ), DlgCfgScoreReading::OnControlClicked )
    EVT_TEXT( XRCID( "cboMaxNoteC4" ), DlgCfgScoreReading::OnControlClicked )

    //
    // panel 1: Time signatures
    //

    // Time signatures check boxes
    EVT_CHECKBOX( XRCID( "chkTime24" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime34" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime44" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime28" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime22" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime32" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime68" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime98" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime128" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkTime38" ), DlgCfgScoreReading::OnControlClicked )

    //
    // panel 2: Key signatures
    //

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), DlgCfgScoreReading::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), DlgCfgScoreReading::OnControlClicked )


wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
DlgCfgScoreReading::DlgCfgScoreReading(wxWindow * parent,
                                       ScoreConstrains* pConstrains,
                                       wxString sSettingsKey)
    : m_fCreatingDlg(true)
{
    // save received data
    m_pConstrains = pConstrains;
    m_sSettingsKey = sSettingsKey;

    if (m_sSettingsKey == "single_clefs_reading") {
        m_nDialogType = eDlgNotesReading;
    }
    else if (m_sSettingsKey == "single_music_reading") {
        m_nDialogType = eDlgMusicReading;
    }
    else {
        m_nDialogType = eDlgCfgError;
        LOMSE_LOG_ERROR("Bad key received: '%s'",
                        m_sSettingsKey.ToStdString().c_str() );
    }

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, "DlgCfgScoreReading");

        //
        //get pointers to all controls
        //

    // the notebook control
    m_pBook = XRCCTRL(*this, "bookCtrol", wxNotebook);

        // Panel 0: Clefs & Notes

    // Error messages and bitmaps
    m_pLblClefError = XRCCTRL(*this, "lblClefError", wxStaticText);
    m_pBmpClefError = XRCCTRL(*this, "bmpClefError", wxStaticBitmap);
    m_pBmpRangeError = XRCCTRL(*this, "bmpRangeError", wxStaticBitmap);
    m_pLblRangeError = XRCCTRL(*this, "lblRangeError", wxStaticText);
    m_pBmpErrorRange[0] = XRCCTRL(*this, "bmpErrorRangeG", wxStaticBitmap);
    m_pBmpErrorRange[1] = XRCCTRL(*this, "bmpErrorRangeF4", wxStaticBitmap);
    m_pBmpErrorRange[2] = XRCCTRL(*this, "bmpErrorRangeF3", wxStaticBitmap);
    m_pBmpErrorRange[3] = XRCCTRL(*this, "bmpErrorRangeC1", wxStaticBitmap);
    m_pBmpErrorRange[4] = XRCCTRL(*this, "bmpErrorRangeC2", wxStaticBitmap);
    m_pBmpErrorRange[5] = XRCCTRL(*this, "bmpErrorRangeC3", wxStaticBitmap);
    m_pBmpErrorRange[6] = XRCCTRL(*this, "bmpErrorRangeC4", wxStaticBitmap);

    // Allowed clefs
    m_pChkClef[0] = XRCCTRL(*this, "chkGClef", wxCheckBox);
    m_pChkClef[1] = XRCCTRL(*this, "chkF4Clef", wxCheckBox);
    m_pChkClef[2] = XRCCTRL(*this, "chkF3Clef", wxCheckBox);
    m_pChkClef[3] = XRCCTRL(*this, "chkC1Clef", wxCheckBox);
    m_pChkClef[4] = XRCCTRL(*this, "chkC2Clef", wxCheckBox);
    m_pChkClef[5] = XRCCTRL(*this, "chkC3Clef", wxCheckBox);
    m_pChkClef[6] = XRCCTRL(*this, "chkC4Clef", wxCheckBox);

    // Note ranges for each clef
    m_pCboMinNote[0] = XRCCTRL(*this, "cboMinNoteG", wxComboBox);
    m_pCboMinNote[1] = XRCCTRL(*this, "cboMinNoteF4", wxComboBox);
    m_pCboMinNote[2] = XRCCTRL(*this, "cboMinNoteF3", wxComboBox);
    m_pCboMinNote[3] = XRCCTRL(*this, "cboMinNoteC1", wxComboBox);
    m_pCboMinNote[4] = XRCCTRL(*this, "cboMinNoteC2", wxComboBox);
    m_pCboMinNote[5] = XRCCTRL(*this, "cboMinNoteC3", wxComboBox);
    m_pCboMinNote[6] = XRCCTRL(*this, "cboMinNoteC4", wxComboBox);

    m_pCboMaxNote[0] = XRCCTRL(*this, "cboMaxNoteG", wxComboBox);
    m_pCboMaxNote[1] = XRCCTRL(*this, "cboMaxNoteF4", wxComboBox);
    m_pCboMaxNote[2] = XRCCTRL(*this, "cboMaxNoteF3", wxComboBox);
    m_pCboMaxNote[3] = XRCCTRL(*this, "cboMaxNoteC1", wxComboBox);
    m_pCboMaxNote[4] = XRCCTRL(*this, "cboMaxNoteC2", wxComboBox);
    m_pCboMaxNote[5] = XRCCTRL(*this, "cboMaxNoteC3", wxComboBox);
    m_pCboMaxNote[6] = XRCCTRL(*this, "cboMaxNoteC4", wxComboBox);

    // max interval between two consecutive notes
    m_pSpinMaxInterval = XRCCTRL(*this, "spinMaxInterval", wxSpinCtrl);

        // Panel 1: Time signatures

    // Error messages and bitmaps
    m_pLblTimeError = XRCCTRL(*this, "lblTimeError", wxStaticText);
    m_pBmpTimeError = XRCCTRL(*this, "bmpTimeError", wxStaticBitmap);

    // Time signatures check boxes
    m_pChkTime[k_time_2_4 - k_min_time_signature] = XRCCTRL(*this, "chkTime24", wxCheckBox);
    m_pChkTime[k_time_3_4 - k_min_time_signature] = XRCCTRL(*this, "chkTime34", wxCheckBox);
    m_pChkTime[k_time_4_4 - k_min_time_signature] = XRCCTRL(*this, "chkTime44", wxCheckBox);
    m_pChkTime[k_time_6_8 - k_min_time_signature] = XRCCTRL(*this, "chkTime68", wxCheckBox);
    m_pChkTime[k_time_9_8 - k_min_time_signature] = XRCCTRL(*this, "chkTime98", wxCheckBox);
    m_pChkTime[k_time_12_8 - k_min_time_signature] = XRCCTRL(*this, "chkTime128", wxCheckBox);
    m_pChkTime[k_time_2_8 - k_min_time_signature] = XRCCTRL(*this, "chkTime28", wxCheckBox);
    m_pChkTime[k_time_3_8 - k_min_time_signature] = XRCCTRL(*this, "chkTime38", wxCheckBox);
    m_pChkTime[k_time_2_2 - k_min_time_signature] = XRCCTRL(*this, "chkTime22", wxCheckBox);
    m_pChkTime[k_time_3_2 - k_min_time_signature] = XRCCTRL(*this, "chkTime32", wxCheckBox);

        // Panel 2: Key signatures

    // Error messages and bitmaps
    m_pBmpKeySignError = XRCCTRL(*this, "bmpKeySignError", wxStaticBitmap);
    m_pLblKeySignError = XRCCTRL(*this, "lblKeySignError", wxStaticText);

    // Allowed key signatures
    m_pChkKeySign[k_key_C] = XRCCTRL(*this, "chkKeyC", wxCheckBox);
    m_pChkKeySign[k_key_G] = XRCCTRL(*this, "chkKeyG", wxCheckBox);
    m_pChkKeySign[k_key_D] = XRCCTRL(*this, "chkKeyD", wxCheckBox);
    m_pChkKeySign[k_key_A] = XRCCTRL(*this, "chkKeyA", wxCheckBox);
    m_pChkKeySign[k_key_E] = XRCCTRL(*this, "chkKeyE", wxCheckBox);
    m_pChkKeySign[k_key_B] = XRCCTRL(*this, "chkKeyB", wxCheckBox);
    m_pChkKeySign[k_key_Fs] = XRCCTRL(*this, "chkKeyFSharp", wxCheckBox);
    m_pChkKeySign[k_key_Cs] = XRCCTRL(*this, "chkKeyCSharp", wxCheckBox);
    m_pChkKeySign[k_key_Cf] = XRCCTRL(*this, "chkKeyCFlat", wxCheckBox);
    m_pChkKeySign[k_key_Gf] = XRCCTRL(*this, "chkKeyGFlat", wxCheckBox);
    m_pChkKeySign[k_key_Df] = XRCCTRL(*this, "chkKeyDFlat", wxCheckBox);
    m_pChkKeySign[k_key_Af] = XRCCTRL(*this, "chkKeyAFlat", wxCheckBox);
    m_pChkKeySign[k_key_Ef] = XRCCTRL(*this, "chkKeyEFlat", wxCheckBox);
    m_pChkKeySign[k_key_Bf] = XRCCTRL(*this, "chkKeyBFlat", wxCheckBox);
    m_pChkKeySign[k_key_F] = XRCCTRL(*this, "chkKeyF", wxCheckBox);




        //
        //set error icons and hide them
        //

    int i;
    wxBitmap bmpError =
         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));

    //page 0: clefs and notes
    m_pBmpClefError->SetBitmap(bmpError);
    m_pBmpClefError->Show(false);
    m_pBmpRangeError->SetBitmap(bmpError);
    m_pBmpRangeError->Show(false);
    for (i=0; i < 7; i++) {
        m_pBmpErrorRange[i]->SetBitmap(bmpError);
        m_pBmpErrorRange[i]->Show(false);
    }

    // page 1: time signatures
    m_pBmpTimeError->SetBitmap(bmpError);
    m_pBmpTimeError->Show(false);

    // page 2: Key signatures
    m_pBmpKeySignError->SetBitmap(bmpError);
    m_pBmpKeySignError->Show(false);

        //
        //hide all error messages
        //

    //page 0: clefs and notes
    m_pLblClefError->Show(false);
    m_pLblRangeError->Show(false);

    // page 1: time signatures
    m_pLblTimeError->Show(false);

    // page 2: Key signatures
    m_pLblKeySignError->Show(false);

        //
        // initialize all controls with current constraints data
        //

    //page 0: clefs and notes

    // combos for minimum and maximun notes
    ClefConstrains* pClefs = m_pConstrains->GetClefConstrains();
    for (i=0; i < 7; i++)
    {
        load_combobox_with_note_names(m_pCboMinNote[i], pClefs->GetLowerPitch((EClef)((int)k_min_clef_in_exercises+i)));
        load_combobox_with_note_names(m_pCboMaxNote[i], pClefs->GetUpperPitch((EClef)((int)k_min_clef_in_exercises+i)));
    }

    //check boxes for allowed clefs
    bool fSelected;
    for (i=0; i < 7; i++) {
        fSelected = m_pConstrains->IsValidClef( (EClef)((int)k_min_clef_in_exercises+i) );
        m_pChkClef[i]->SetValue( fSelected );
        m_pCboMinNote[i]->Enable(fSelected);
        m_pCboMaxNote[i]->Enable(fSelected);
    }

    // max_interval
    m_pSpinMaxInterval->SetValue(m_pConstrains->GetMaxInterval());

    // page 1: time signatures
    TimeSignConstrains* pTimeSigns = m_pConstrains->GetTimeSignConstrains();
    for (i=0; i < k_max_time_signature - k_min_time_signature + 1; i++) {
        fSelected = pTimeSigns->IsValid((ETimeSignature)(i+k_min_time_signature));
        m_pChkTime[i]->SetValue( fSelected );
    }

    // page 2: key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < k_key_F+1; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignature)i) );
    }

    //Reconfigure dialog depending on needed type
    if (m_nDialogType == eDlgNotesReading) {
        m_pBook->DeletePage(1);    // the second page: Time signatures
        m_pBook->DeletePage(1);    // Key Sigantures (note that after deleting a page pages get renumbered)
        m_pBook->DeletePage(1);    // Rhythm signatures
    }
    else {    //single_music_reading key
        // all panels visible
    }

    //TODO open last used panel

    CentreOnParent();
    m_fCreatingDlg = false;
    VerifyData();

}

//---------------------------------------------------------------------------------------
DlgCfgScoreReading::~DlgCfgScoreReading()
{
}

//---------------------------------------------------------------------------------------
/*! Accept button will be enabled only if all data habe been validated and is Ok. So
    when accept button is clicked we can proceed to save data.
*/
void DlgCfgScoreReading::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    //
    // Save data from panel 0: allowed clefs, notes ranges and max_interval
    //

    // save allowed clefs and notes ranges
    EClef nClef;
    int i;
    for (i=0; i < 7; i++) {
        nClef = (EClef)((int)k_min_clef_in_exercises+i);
        m_pConstrains->SetClef(nClef, m_pChkClef[i]->GetValue());
        m_pConstrains->SetMinNote(nClef, m_pCboMinNote[i]->GetValue());
        m_pConstrains->SetMaxNote(nClef, m_pCboMaxNote[i]->GetValue());
    }

    // save maxInterval
    m_pConstrains->SetMaxInterval( m_pSpinMaxInterval->GetValue() );

    //save data from next pages only when they exists!
    if (m_nDialogType != eDlgNotesReading) {

        //
        // Save data from panel 1: allowed time signatures
        //
        TimeSignConstrains* pTimeSigns = m_pConstrains->GetTimeSignConstrains();
        for (i=0; i < k_max_time_signature - k_min_time_signature + 1; i++) {
            pTimeSigns->SetValid((ETimeSignature)(i+k_min_time_signature), m_pChkTime[i]->GetValue() );
        }

        //
        // Save data from panel 2: allowed key signatures
        //
        KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
        for (i=0; i < k_key_F+1; i++) {
            pKeyConstrains->SetValid((EKeySignature)i, m_pChkKeySign[i]->GetValue());
        }

    }

    //terminate the dialog
    //TODO save the currently shown panel number to open this panel next time
    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
/*! Returns true if there are errors. If there are no
    errors the Accept button is enabled. Otherwise it is disabled.
*/
bool DlgCfgScoreReading::VerifyData()
{
    if (m_fCreatingDlg)
        return false;

    int i;

    //assume no errors
    bool fError = false;
    bool fGlobalError = false;

        //
        //  Data in panel 0: clefs and notes
        //

    // hide error messages and error icons
    m_pLblClefError->Show(false);
    m_pBmpClefError->Show(false);
    m_pLblRangeError->Show(false);
    m_pBmpRangeError->Show(false);
    for (i=0; i < 7; i++)
        m_pBmpErrorRange[i]->Show(false);

    // check that at least one clef is allowed and enable/disable combos for that clef
    bool fAtLeastOne = false;
    bool fEnable;
    for (i=0; i < 7; i++)
    {
        fEnable = false;
        if (m_pChkClef[i]->GetValue())
        {
            fAtLeastOne = true;
            fEnable = true;
        }
        m_pCboMinNote[i]->Enable(fEnable);
        m_pCboMaxNote[i]->Enable(fEnable);
    }
    fError = !fAtLeastOne;
    if (fError)
    {
        m_pLblClefError->Show(true);
        m_pBmpClefError->Show(true);
    }
    fGlobalError |= fError;

    //For selected clefs, verify that min note is lower than max note
    fError = false;
    for (i=0; i < 7; i++)
    {
        FPitch fpMinPitch( to_std_string( m_pCboMinNote[i]->GetValue() ) );
        FPitch fpMaxPitch( to_std_string( m_pCboMaxNote[i]->GetValue() ) );
        if (m_pChkClef[i]->GetValue())
        {
            if (fpMinPitch > fpMaxPitch)
            {
                m_pLblRangeError->Show(true);
                m_pBmpRangeError->Show(true);
                m_pBmpErrorRange[i]->Show(true);
                fError = true;
            }
        }
    }
    fGlobalError |= fError;

    //verify next pages only when they exists!
    if (m_nDialogType != eDlgNotesReading)
    {

            //
            //  Data in panel 1: allowed time signatures
            //

        // hide error messages and error icons
        m_pLblTimeError->Show(false);
        m_pBmpTimeError->Show(false);

        // check that at least one time signature is selected
        fAtLeastOne = false;
        for (i=0; i < 7; i++)
        {
            if (m_pChkTime[i]->GetValue())
                fAtLeastOne = true;
        }
        fError = !fAtLeastOne;
        if (fError)
        {
            m_pLblTimeError->Show(true);
            m_pBmpTimeError->Show(true);
        }
        fGlobalError |= fError;

            //
            //  Data in panel 2: allowed key signatures
            //

        // hide error messages and error icons
        m_pBmpKeySignError->Show(false);
        m_pLblKeySignError->Show(false);

        // check that at least one key signature has been choosen
        fAtLeastOne = false;
        for (i=0; i < k_key_F+1; i++)
            fAtLeastOne |= m_pChkKeySign[i]->GetValue();
        fError = !fAtLeastOne;
        if (fError)
        {
            m_pLblKeySignError->Show(true);
            m_pBmpKeySignError->Show(true);
        }
        fGlobalError |= fError;

    }

    //TODO verify that there are fragments for the chosen time signatures

    //
    //enable / disable accept button
    //
    wxButton* pButtonAccept = XRCCTRL(*this, "buttonAccept", wxButton);
    pButtonAccept->Enable(!fGlobalError);

    return fGlobalError;
}

//---------------------------------------------------------------------------------------
void DlgCfgScoreReading::OnControlClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}


}   //namespace lenmus
