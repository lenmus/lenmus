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

//lenmus
#include "lenmus_dlg_cfg_idfy_chords.h"
#include "lenmus_utilities.h"

//lomse
#include "lomse_pitch.h"
#include "lomse_internal_model.h"
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/xrc/xmlres.h>


namespace lenmus
{

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(DlgCfgIdfyChord, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), DlgCfgIdfyChord::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), DlgCfgIdfyChord::OnCancelClicked )

    // Type of interval check boxes
    EVT_CHECKBOX( XRCID( "chkModeHarmonic" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkModeMelodicAsc" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkModeMelodicDesc" ), DlgCfgIdfyChord::OnDataChanged )

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), DlgCfgIdfyChord::OnDataChanged )

    // Allowed chords check boxes
    EVT_CHECKBOX( XRCID( "chkChordMajorTriad" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMinorTriad" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordAugTriad" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordDimTriad" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordSus4th" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordSus2nd" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMajor7" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordDominant7" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMinor7" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordDim7" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordHalfDim7" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordAugMajor7" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordAug7" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMinorMajor7" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMajor6" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMinor6" ), DlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordAug6" ), DlgCfgIdfyChord::OnDataChanged )

wxEND_EVENT_TABLE()



DlgCfgIdfyChord::DlgCfgIdfyChord(wxWindow* parent,
                           ChordConstrains* pConstrains,
                           bool fTheoryMode)
{
    // save received data
    m_pConstrains = pConstrains;
    m_fTheoryMode = fTheoryMode;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, "DlgCfgIdfyChord");

        //
        //get pointers to all controls
        //

    // error messages and bitmaps
    m_pBmpPlayModeError = XRCCTRL(*this, "bmpPlayModeError", wxStaticBitmap);
    m_pLblPlayModeError = XRCCTRL(*this, "lblPlayModeError", wxStaticText);
    m_pBmpKeySignError = XRCCTRL(*this, "bmpKeySignError", wxStaticBitmap);
    m_pLblKeySignError = XRCCTRL(*this, "lblKeySignError", wxStaticText);
    m_pBmpAllowedChordsError = XRCCTRL(*this, "bmpAllowedChordsError", wxStaticBitmap);
    m_pLblAllowedChordsError = XRCCTRL(*this, "lblAllowedChordsError", wxStaticText);

    // allowed chords checkboxes
    // correspondence with EChordTypes
    m_pChkChord[ect_MajorTriad] = XRCCTRL(*this, "chkChordMajorTriad", wxCheckBox);
    m_pChkChord[ect_MinorTriad] = XRCCTRL(*this, "chkChordMinorTriad", wxCheckBox);
    m_pChkChord[ect_AugTriad] = XRCCTRL(*this, "chkChordAugTriad", wxCheckBox);
    m_pChkChord[ect_DimTriad] = XRCCTRL(*this, "chkChordDimTriad", wxCheckBox);
    m_pChkChord[ect_Suspended_4th] = XRCCTRL(*this, "chkChordSus4th", wxCheckBox);
    m_pChkChord[ect_Suspended_2nd] = XRCCTRL(*this, "chkChordSus2nd", wxCheckBox);
    m_pChkChord[ect_MajorSeventh] = XRCCTRL(*this, "chkChordMajor7", wxCheckBox);
    m_pChkChord[ect_DominantSeventh] = XRCCTRL(*this, "chkChordDominant7", wxCheckBox);
    m_pChkChord[ect_MinorSeventh] = XRCCTRL(*this, "chkChordMinor7", wxCheckBox);
    m_pChkChord[ect_DimSeventh] = XRCCTRL(*this, "chkChordDim7", wxCheckBox);
    m_pChkChord[ect_HalfDimSeventh] = XRCCTRL(*this, "chkChordHalfDim7", wxCheckBox);
    m_pChkChord[ect_AugMajorSeventh] = XRCCTRL(*this, "chkChordAugMajor7", wxCheckBox);
    m_pChkChord[ect_AugSeventh] = XRCCTRL(*this, "chkChordAug7", wxCheckBox);
    m_pChkChord[ect_MinorMajorSeventh] = XRCCTRL(*this, "chkChordMinorMajor7", wxCheckBox);
    m_pChkChord[ect_MajorSixth] = XRCCTRL(*this, "chkChordMajor6", wxCheckBox);
    m_pChkChord[ect_MinorSixth] = XRCCTRL(*this, "chkChordMinor6", wxCheckBox);
    m_pChkChord[ect_AugSixth] = XRCCTRL(*this, "chkChordAug6", wxCheckBox);

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

    // allowed types of intervals
    m_pChkPlayMode[0] = XRCCTRL(*this, "chkModeHarmonic", wxCheckBox);
    m_pChkPlayMode[1] = XRCCTRL(*this, "chkModeMelodicAsc", wxCheckBox);
    m_pChkPlayMode[2] = XRCCTRL(*this, "chkModeMelodicDesc", wxCheckBox);

    //other controls
    m_pChkAllowInversions = XRCCTRL(*this, "chkAllowInversions", wxCheckBox);
    m_pChkDisplayKey = XRCCTRL(*this, "chkDisplayKey", wxCheckBox);
    m_pBoxPlayModes = XRCCTRL(*this, "boxPlayModes", wxStaticBox);

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));
    m_pBmpPlayModeError->SetBitmap(bmpError);
    m_pBmpKeySignError->SetBitmap(bmpError);
    m_pBmpAllowedChordsError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pLblPlayModeError->Show(false);
    m_pBmpPlayModeError->Show(false);
    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblAllowedChordsError->Show(false);
    m_pBmpAllowedChordsError->Show(false);

        //
        // initialize all controls with current constraints data
        //

    //initialize check boxes for allowed chords with current settings
    int i;
    for (i=0; i < ect_MaxInExercises; i++) {
        m_pChkChord[i]->SetValue( m_pConstrains->IsChordValid((EChordType)i) );
    }

    //play modes
    for (i=0; i < 3; i++) {
        m_pChkPlayMode[i]->SetValue( m_pConstrains->IsModeAllowed(i) );
    }

    // allowed key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < k_key_F+1; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignature)i) );
    }

    // other
    m_pChkAllowInversions->SetValue( m_pConstrains->AreInversionsAllowed() );
    m_pChkDisplayKey->SetValue( m_pConstrains->DisplayKey() );

    // As this dialog is shared by EarTraining and Theory.
    // Flag m_fTheoryMode controls whether to show/hide
    // specific controls used only in one of the exercises
    m_pBoxPlayModes->Enable(true);
    if (m_fTheoryMode)
    {
        // This dialog is being used for Theory so, hide play modes
        m_pBoxPlayModes->Show(false);
        for (i=0; i < 3; i++)
            m_pChkPlayMode[i]->Show(false);
    }


    //center dialog on screen
    CentreOnScreen();

}

DlgCfgIdfyChord::~DlgCfgIdfyChord()
{
}

void DlgCfgIdfyChord::OnDataChanged(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void DlgCfgIdfyChord::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.

    //save allowed chords
    int i;
    for (i=0; i < ect_MaxInExercises; i++) {
        m_pConstrains->SetChordValid((EChordType)i, m_pChkChord[i]->GetValue());
    }

    // save selected key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < k_key_F+1; i++) {
        pKeyConstrains->SetValid((EKeySignature)i, m_pChkKeySign[i]->GetValue());
    }

    //save other options
    m_pConstrains->SetDisplayKey( m_pChkDisplayKey->GetValue() );
    m_pConstrains->SetInversionsAllowed( m_pChkAllowInversions->GetValue() );

    // save options depending on mode: theory/ear training
    if (!m_fTheoryMode) {
        //ear training. Save play modes
        for (i=0; i < 3; i++) {
            m_pConstrains->SetModeAllowed(i, m_pChkPlayMode[i]->GetValue());
        }
    }

    //terminate the dialog
    EndModal(wxID_OK);
}

bool DlgCfgIdfyChord::VerifyData()
{
    // Returns a boolean to enable or not a tab change. That is: returns true if there are
    // local errors (errors affecting only to the data in a tab) so not to enable a tab
    // change. If there are no tab local errors then returns false (althought it there might
    // be global errors -- coherence between data in different tabs --).
    //
    // Anyway, global errors al also checked. If there are no global neither local
    // errors the Accept button is enabled. Otherwise it is disabled.

    bool fAtLeastOne;
    int i;

    //assume no errors
    bool fError = false;
    bool fLocalError = false;
    bool fGlobalError = false;

    m_pLblPlayModeError->Show(false);
    m_pBmpPlayModeError->Show(false);
    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblAllowedChordsError->Show(false);
    m_pBmpAllowedChordsError->Show(false);

    // check that at least one chord type is selected
    fError = false;
    fAtLeastOne = false;
    for (i=0; i < ect_MaxInExercises; i++) {
        if (m_pChkChord[i]->GetValue()) {
            fAtLeastOne = true;
            break;
        }
    }
    fError = !fAtLeastOne;
    if (fError) {
        m_pLblAllowedChordsError->Show(true);
        m_pBmpAllowedChordsError->Show(true);
    }
    fLocalError |= fError;

    // check that at least one key signature has been choosen
    fAtLeastOne = false;
    for (i=0; i < k_key_F+1; i++) {
        fAtLeastOne |= m_pChkKeySign[i]->GetValue();
    }
    fError = !fAtLeastOne;
    if (fError) {
        m_pLblKeySignError->Show(true);
        m_pBmpKeySignError->Show(true);
    }
    fLocalError |= fError;

    //check that at least one play mode is choosen
    if (!m_fTheoryMode) {
        fAtLeastOne = false;
        for (i=0; i < 3; i++) {
            fAtLeastOne |= m_pChkPlayMode[i]->GetValue();
        }
        fError = !fAtLeastOne;
        if (fError) {
            m_pLblPlayModeError->Show(true);
            m_pBmpPlayModeError->Show(true);
        }
        fLocalError |= fError;
    }

        //
        // Check for global errors
        //

    fGlobalError = false;   //no global checkings in this dlg


    //enable / disable accept button
    wxButton* pButtonAccept = XRCCTRL(*this, "buttonAccept", wxButton);
    pButtonAccept->Enable(!fLocalError && !fGlobalError);

    return fLocalError;

}


}   //namespace lenmus
