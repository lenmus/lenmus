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

#include "lenmus_dlg_cfg_ear_intervals.h"
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

wxBEGIN_EVENT_TABLE(DlgCfgEarIntervals, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), DlgCfgEarIntervals::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), DlgCfgEarIntervals::OnCancelClicked )
    EVT_TEXT( XRCID( "cboFromNote" ), DlgCfgEarIntervals::OnCboFromNote )
    EVT_TEXT( XRCID( "cboToNote" ), DlgCfgEarIntervals::OnCboToNote )
    EVT_NOTEBOOK_PAGE_CHANGING( XRCID( "noteBook" ), DlgCfgEarIntervals::OnPageChanging )

    // Type of interval check boxes
    EVT_CHECKBOX( XRCID( "chkIntvalTypeHarmonic" ), DlgCfgEarIntervals::OnChkIntvalTypeClicked )
    EVT_CHECKBOX( XRCID( "chkIntvalTypeMelodicAsc" ), DlgCfgEarIntervals::OnChkIntvalTypeClicked )
    EVT_CHECKBOX( XRCID( "chkIntvalTypeMelodicDesc" ), DlgCfgEarIntervals::OnChkIntvalTypeClicked )

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), DlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), DlgCfgEarIntervals::OnChkKeyClicked )

    // Accidentals type: radio buttons
    EVT_RADIOBOX( XRCID( "radAccidentals" ), DlgCfgEarIntervals::OnRadAccidentalsClicked )

    // Alloved intervals check boxes
    EVT_CHECKBOX( XRCID( "chkIntval1" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval2min" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval2maj" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval3min" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval3maj" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval4" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval4aug" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval5" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval6min" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval6maj" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval7min" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval7maj" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval8" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval9min" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval9maj" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval10min" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval10maj" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval11" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval11aug" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval12" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval13min" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval13maj" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval14min" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval14maj" ), DlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval2oct" ), DlgCfgEarIntervals::OnChkIntvalClicked )

    wxEND_EVENT_TABLE()



DlgCfgEarIntervals::DlgCfgEarIntervals(wxWindow * parent,
                           EarIntervalsConstrains* pConstrains,
                           bool fEnableFirstEqual)
{
    // save received data
    m_pConstrains = pConstrains;
    m_fEnableFirstEqual = fEnableFirstEqual;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, "DlgCfgEarIntervals");

        //
        //get pointers to all controls
        //

    // error messages and bitmaps
    m_pLblRangeError = XRCCTRL(*this, "lblRangeError", wxStaticText);
    m_pBmpRangeError = XRCCTRL(*this, "bmpRangeError", wxStaticBitmap);
    m_pBmpIntvalTypeError = XRCCTRL(*this, "bmpIntvalTypeError", wxStaticBitmap);
    m_pLblIntvalTypeError = XRCCTRL(*this, "lblIntvalTypeError", wxStaticText);
    m_pChkStartSameNote = XRCCTRL(*this, "chkStartSameNote", wxCheckBox);
    m_pBmpKeySignError = XRCCTRL(*this, "bmpKeySignError", wxStaticBitmap);
    m_pLblKeySignError = XRCCTRL(*this, "lblKeySignError", wxStaticText);
    m_pBmpAllowedIntvalError = XRCCTRL(*this, "bmpAllowedIntvalError", wxStaticBitmap);
    m_pLblAllowedIntvalError = XRCCTRL(*this, "lblAllowedIntvalError", wxStaticText);
    m_pLblGeneralError = XRCCTRL(*this, "lblGeneralError", wxStaticText);
    m_pBmpGeneralError = XRCCTRL(*this, "bmpGeneralError", wxStaticBitmap);

    // allowed intervals checkboxes
    m_pChkIntval[ein_1] = XRCCTRL(*this, "chkIntval1", wxCheckBox);
    m_pChkIntval[ein_2min] = XRCCTRL(*this, "chkIntval2min", wxCheckBox);
    m_pChkIntval[ein_2maj] = XRCCTRL(*this, "chkIntval2maj", wxCheckBox);
    m_pChkIntval[ein_3min] = XRCCTRL(*this, "chkIntval3min", wxCheckBox);
    m_pChkIntval[ein_3maj] = XRCCTRL(*this, "chkIntval3maj", wxCheckBox);
    m_pChkIntval[ein_4] = XRCCTRL(*this, "chkIntval4", wxCheckBox);
    m_pChkIntval[ein_4aug] = XRCCTRL(*this, "chkIntval4aug", wxCheckBox);
    m_pChkIntval[ein_5] = XRCCTRL(*this, "chkIntval5", wxCheckBox);
    m_pChkIntval[ein_6min] = XRCCTRL(*this, "chkIntval6min", wxCheckBox);
    m_pChkIntval[ein_6maj] = XRCCTRL(*this, "chkIntval6maj", wxCheckBox);
    m_pChkIntval[ein_7min] = XRCCTRL(*this, "chkIntval7min", wxCheckBox);
    m_pChkIntval[ein_7maj] = XRCCTRL(*this, "chkIntval7maj", wxCheckBox);
    m_pChkIntval[ein_8] = XRCCTRL(*this, "chkIntval8", wxCheckBox);
    m_pChkIntval[ein_9min] = XRCCTRL(*this, "chkIntval9min", wxCheckBox);
    m_pChkIntval[ein_9maj] = XRCCTRL(*this, "chkIntval9maj", wxCheckBox);
    m_pChkIntval[ein_10min] = XRCCTRL(*this, "chkIntval10min", wxCheckBox);
    m_pChkIntval[ein_10maj] = XRCCTRL(*this, "chkIntval10maj", wxCheckBox);
    m_pChkIntval[ein_11] = XRCCTRL(*this, "chkIntval11", wxCheckBox);
    m_pChkIntval[ein_11aug] = XRCCTRL(*this, "chkIntval11aug", wxCheckBox);
    m_pChkIntval[ein_12] = XRCCTRL(*this, "chkIntval12", wxCheckBox);
    m_pChkIntval[ein_13min] = XRCCTRL(*this, "chkIntval13min", wxCheckBox);
    m_pChkIntval[ein_13maj] = XRCCTRL(*this, "chkIntval13maj", wxCheckBox);
    m_pChkIntval[ein_14min] = XRCCTRL(*this, "chkIntval14min", wxCheckBox);
    m_pChkIntval[ein_14maj] = XRCCTRL(*this, "chkIntval14maj", wxCheckBox);
    m_pChkIntval[ein_2oct] = XRCCTRL(*this, "chkIntval2oct", wxCheckBox);

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

    // notes range
    m_pCboFromNote = XRCCTRL(*this, "cboFromNote", wxComboBox);
    m_pCboToNote = XRCCTRL(*this, "cboToNote", wxComboBox);

    // allowed types of intervals
    m_pChkIntvalType[0] = XRCCTRL(*this, "chkIntvalTypeHarmonic", wxCheckBox);
    m_pChkIntvalType[1] = XRCCTRL(*this, "chkIntvalTypeMelodicAsc", wxCheckBox);
    m_pChkIntvalType[2] = XRCCTRL(*this, "chkIntvalTypeMelodicDesc", wxCheckBox);

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));
    m_pBmpRangeError->SetBitmap(bmpError);
    m_pBmpIntvalTypeError->SetBitmap(bmpError);
    m_pBmpKeySignError->SetBitmap(bmpError);
    m_pBmpAllowedIntvalError->SetBitmap(bmpError);
    m_pBmpGeneralError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pLblRangeError->Show(false);
    m_pBmpRangeError->Show(false);
    m_pLblIntvalTypeError->Show(false);
    m_pBmpIntvalTypeError->Show(false);
    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblAllowedIntvalError->Show(false);
    m_pBmpAllowedIntvalError->Show(false);
    m_pLblGeneralError->Show(false);
    m_pBmpGeneralError->Show(false);

        //
        // initialize all controls with current constraints data
        //

    //initialize check boxes for allowed intervals with current settings
    int i;
    for (i=0; i < lmNUM_INTVALS; i++) {
        m_pChkIntval[i]->SetValue( m_pConstrains->IsIntervalAllowed(i) );
    }

    // populate combos for minimum and maximum notes
    load_combobox_with_note_names(m_pCboFromNote, m_pConstrains->MinNote());
    load_combobox_with_note_names(m_pCboToNote, m_pConstrains->MaxNote());

    //interval types
    for (i=0; i < 3; i++) {
        m_pChkIntvalType[i]->SetValue( m_pConstrains->IsTypeAllowed(i) );
    }

    //accidentals: only in key signature or chromatic accidentals also allowed
    wxRadioBox* pAccidentals = XRCCTRL(*this, "radAccidentals", wxRadioBox);
    if (m_pConstrains->OnlyNatural()) {
        pAccidentals->SetSelection(0);
        EnableKeySignCheckBoxes(true);
    } else {
        pAccidentals->SetSelection(1);
        EnableKeySignCheckBoxes(false);
    }

    // selected key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < k_key_F+1; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignature)i) );
    }

    // As this dialog is shared by two exercises (EarCompareIntvCtrol and
    // EarIntervalsCtrol) flag m_fEnableFirstEqual controls whether to show/hide
    // specific controls used only in one of the exercises
    if (m_fEnableFirstEqual) {
        // This dialog is being used by EarCompareIntvCtrol
        m_pChkStartSameNote->SetValue( m_pConstrains->FirstNoteEqual() );
    }
    else {
        // This dialog is being used by EarIntervalsCtrol
        // So, hide checkbox 'Start both intervals with the same note
        m_pChkStartSameNote->Show(false);
    }



    //center dialog on screen
    CentreOnScreen();

}

DlgCfgEarIntervals::~DlgCfgEarIntervals()
{
}

void DlgCfgEarIntervals::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    //Accept button will be enabled only if all data has been validated and is Ok. So
    //when accept button is clicked we can proceed to save data.

    //save allowed intervals
    for (int i=0; i < lmNUM_INTVALS; i++)
        m_pConstrains->SetIntervalAllowed(i, m_pChkIntval[i]->GetValue());

    //save notes range
    wxString sPitch = m_pCboFromNote->GetValue();
    FPitch fp1( to_std_string(sPitch) );
    DiatonicPitch dp1 = fp1.to_diatonic_pitch();
    m_pConstrains->SetMinNote(dp1);

    sPitch = m_pCboToNote->GetValue();
    FPitch fp2( to_std_string(sPitch) );
    DiatonicPitch dp2 = fp2.to_diatonic_pitch();
    m_pConstrains->SetMaxNote(dp2);

    // save intervals' type
    for (int i=0; i < 3; i++)
        m_pConstrains->SetTypeAllowed(i, m_pChkIntvalType[i]->GetValue());

    // save accidentals option and selected key signatures
    wxRadioBox* pAccidentals = XRCCTRL(*this, "radAccidentals", wxRadioBox);
    bool fOnlyNatural = (pAccidentals->GetSelection() == 0);
    m_pConstrains->SetOnlyNatural( fOnlyNatural );
    if (fOnlyNatural)
    {
        // store selected key signatures
        KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
        for (int i=0; i < k_key_F+1; i++)
            pKeyConstrains->SetValid((EKeySignature)i, m_pChkKeySign[i]->GetValue());
    }

    // If this dialog is being used by EarCompareIntvCtrol, save first note equal value
    if (m_fEnableFirstEqual)
        m_pConstrains->SetFirstNoteEqual( m_pChkStartSameNote->GetValue() );

    //terminate the dialog
    EndModal(wxID_OK);
}

void DlgCfgEarIntervals::OnCboFromNote(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void DlgCfgEarIntervals::OnCboToNote(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}


bool DlgCfgEarIntervals::VerifyData()
{
    //Returns a boolean to enable or not a tab change. That is: returns true if there are
    //local errors (errors affecting only to the data in a tab) so not to enable a tab
    //change. If there are no tab local errors then returns false (although it there might
    //be global errors -- coherence between data in different tabs --).
    //
    //Anyway, global errors are also checked. If there are no global neither local
    //errors the Accept button is enabled. Otherwise it is disabled.

    bool fAtLeastOne;
    int i;

    //assume no errors
    bool fError = false;
    bool fLocalError = false;
    bool fGlobalError = false;
    m_pLblIntvalTypeError->Show(false);
    m_pBmpIntvalTypeError->Show(false);
    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblAllowedIntvalError->Show(false);
    m_pBmpAllowedIntvalError->Show(false);
    m_pLblGeneralError->Show(false);
    m_pBmpGeneralError->Show(false);
    m_pLblRangeError->Show(false);
    m_pBmpRangeError->Show(false);

    //verify that notes range is valid
    fError = false;
    wxString sFrom = m_pCboFromNote->GetValue();
    wxString sTo = m_pCboToNote->GetValue();
    FPitch fpFrom( to_std_string( m_pCboFromNote->GetValue() ));
    FPitch fpTo( to_std_string( m_pCboToNote->GetValue() ));
    if (fpFrom >= fpTo)
    {
        m_pLblRangeError->SetLabel(_("Min. note must be lower than max. note"));
        m_pLblRangeError->Show(true);
        m_pBmpRangeError->Show(true);
        fError = true;
    }
    fLocalError |= fError;

    // check that at least one interval is allowed
    bool fAtLeastOneIntval = false;
    for (i=0; i < lmNUM_INTVALS; i++) {
        if (m_pChkIntval[i]->GetValue()) {
            fAtLeastOneIntval = true;
            break;
        }
    }
    fError = !fAtLeastOneIntval;
    if (fError) {
        m_pLblAllowedIntvalError->Show(true);
        m_pBmpAllowedIntvalError->Show(true);
    }
    fLocalError |= fError;

    //check that at least one interval type is chosen
    fAtLeastOne = false;
    for (i=0; i < 3; i++) {
        fAtLeastOne |= m_pChkIntvalType[i]->GetValue();
    }

    fError = !fAtLeastOne;
    if (fError) {
        m_pLblIntvalTypeError->Show(true);
        m_pBmpIntvalTypeError->Show(true);
    }
    fLocalError |= fError;

    // accidentals: if only natural intervals allowed check that at least one
    // key signature has been chosen
    fAtLeastOne = true;     // assume no error
    wxRadioBox* pAccidentals = XRCCTRL(*this, "radAccidentals", wxRadioBox);
    if (pAccidentals->GetSelection() == 0) {
        fAtLeastOne = false;
        for (i=0; i < k_key_F+1; i++) {
            fAtLeastOne |= m_pChkKeySign[i]->GetValue();
        }
    }

    fError = !fAtLeastOne;
    if (fError) {
        m_pLblKeySignError->Show(true);
        m_pBmpKeySignError->Show(true);
    }
    fLocalError |= fError;

        //
        // Check for global errors
        //

    fGlobalError = false;

    // check that notes range allow to generate at least one of the selected intervals
    fError = true;      // assume error
    //compute max number of semitones in the allowed note range
    //AWARE: fpFrom and fpTo were computed when checking the notes range
    int ntMidiMin = int(fpFrom.to_midi_pitch());
    int ntMidiMax = int(fpTo.to_midi_pitch());
    int nRange = wxMin(ntMidiMax - ntMidiMin, int(lmNUM_INTVALS));
    for (i=0; i <= nRange; i++)
    {
        if (m_pChkIntval[i]->GetValue()) {
            fError = false;
            break;
        }
    }

    if (fError) {
        m_pLblGeneralError->SetLabel(
_("It is not possible to generate any interval. \
Selected notes' range interval (tab 'Other settings') is lower than \
minimum allowed interval (tab 'Intervals')"));
    }
    else {
        //check that it is possible to generate the maximum allowed interval
        if (nRange < lmNUM_INTVALS) {
            for (i = nRange+1; i < lmNUM_INTVALS; i++) {
                if (m_pChkIntval[i]->GetValue()) {
                    fError = true;
                    m_pLblGeneralError->SetLabel(
_("It is nor possible to generate all the selected intervals. \
Selected notes range interval (tab 'Other settings') is lower than \
maximum allowed interval (tab 'Intervals')"));
                    break;
                }
            }
        }
    }

    if (fError && fAtLeastOneIntval && (nRange >= 0)) {
        m_pBmpAllowedIntvalError->Show(true);
        m_pBmpRangeError->Show(true);
        m_pLblGeneralError->Show(true);
        m_pBmpGeneralError->Show(true);
        m_pLblGeneralError->Wrap(300);   //length of field: 300px. Do word wrap if greather
    }
    fGlobalError |= fError;


    //enable / disable accept button
    wxButton* pButtonAccept = XRCCTRL(*this, "buttonAccept", wxButton);
    pButtonAccept->Enable(!fLocalError && !fGlobalError);

    return fLocalError;

}

void DlgCfgEarIntervals::OnPageChanging(wxNotebookEvent& event)
{
    //Do nothing when the notebook is being displayed, at dialog construction
    if ( event.GetOldSelection() == wxNOT_FOUND) return;

    //If execution reaches this point it is a real tab change. Verify data
    bool fError = VerifyData();
    if (fError) {
        event.Veto();
        wxMessageBox( "Correct errors before leaving this page.");
    }

}

void DlgCfgEarIntervals::OnChkKeyClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void DlgCfgEarIntervals::OnChkIntvalTypeClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void DlgCfgEarIntervals::EnableKeySignCheckBoxes(bool fEnable)
{
    int i;
    for (i=0; i < k_key_F+1; i++) {
        m_pChkKeySign[i]->Enable(fEnable);
    }
}

void DlgCfgEarIntervals::OnRadAccidentalsClicked(wxCommandEvent& WXUNUSED(event))
{
    wxRadioBox* pAccidentals = XRCCTRL(*this, "radAccidentals", wxRadioBox);
    bool fEnable = (pAccidentals->GetSelection() == 0);
    EnableKeySignCheckBoxes(fEnable);

    VerifyData();
}

void DlgCfgEarIntervals::OnChkIntvalClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}


}  //namespace lenmus
