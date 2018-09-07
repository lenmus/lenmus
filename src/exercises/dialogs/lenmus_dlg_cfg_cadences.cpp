//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2018 LenMus project
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
#include "lenmus_dlg_cfg_cadences.h"
#include "lenmus_cadence.h"
#include "lenmus_utilities.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/xrc/xmlres.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
enum {
    lmBT_PERFECT = 0,
    lmBT_PLAGAL,
    lmBT_HALF,
    lmBT_DECEPTIVE,
    lmBT_IMPERFECT,
};

//---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//---------------------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(DlgCfgIdfyCadence, wxDialog)
    // Buttons
    EVT_BUTTON( XRCID( "buttonAccept" ), DlgCfgIdfyCadence::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), DlgCfgIdfyCadence::OnCancelClicked )
    EVT_BUTTON( XRCID( "btnCheckAllPerfect" ), DlgCfgIdfyCadence::OnCheckAllPerfect )
    EVT_BUTTON( XRCID( "btnCheckAllPlagal" ), DlgCfgIdfyCadence::OnCheckAllPlagal )
    EVT_BUTTON( XRCID( "btnCheckAllHalf" ), DlgCfgIdfyCadence::OnCheckAllHalf )
    EVT_BUTTON( XRCID( "btnCheckAllDeceptive" ), DlgCfgIdfyCadence::OnCheckAllDeceptive )
    EVT_BUTTON( XRCID( "btnCheckAllMajor" ), DlgCfgIdfyCadence::OnCheckAllMajor )
    EVT_BUTTON( XRCID( "btnCheckAllMinor" ), DlgCfgIdfyCadence::OnCheckAllMinor )

    // Radio button boxes
    EVT_RADIOBOX( XRCID( "radBoxShowKey" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_RADIOBOX( XRCID( "radBoxAnswerType" ), DlgCfgIdfyCadence::OnRadAnswerType )

    // answer buttons
    EVT_CHECKBOX( XRCID( "chkButtonPerfect" ), DlgCfgIdfyCadence::OnAnswerButton )
    EVT_CHECKBOX( XRCID( "chkButtonPlagal" ), DlgCfgIdfyCadence::OnAnswerButton )
    EVT_CHECKBOX( XRCID( "chkButtonImperfect" ), DlgCfgIdfyCadence::OnAnswerButton )
    EVT_CHECKBOX( XRCID( "chkButtonDeceptive" ), DlgCfgIdfyCadence::OnAnswerButton )
    EVT_CHECKBOX( XRCID( "chkButtonHalf" ), DlgCfgIdfyCadence::OnAnswerButton )

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), DlgCfgIdfyCadence::OnDataChanged )
        // Minor keys
    EVT_CHECKBOX( XRCID( "chkKeyAMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFSharpMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCSharpMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGSharpMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDSharpMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyASharpMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyAFlatMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEFlatMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBFlatMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGMinor" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDMinor" ), DlgCfgIdfyCadence::OnDataChanged )

    // Allowed cadences check boxes
    EVT_CHECKBOX( XRCID( "chkCad_V_I" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V7_I" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_Ic64_V" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_Va5_I" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_Vd5_I" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IV_I" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IVm_I" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_II_I" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IIm_I" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_VI_I" ), DlgCfgIdfyCadence::OnDataChanged )
	EVT_CHECKBOX( XRCID( "chkCad_V_I_inv" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_IV" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_IVm" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_VI" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_VIm" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_IIm" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_III" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_VII" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IImc6_V" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IV_V" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_I_V" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IV6_V" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_II_V" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_N6_V" ), DlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_VIa_V" ), DlgCfgIdfyCadence::OnDataChanged )

wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
DlgCfgIdfyCadence::DlgCfgIdfyCadence(wxWindow* parent,
                           CadencesConstrains* pConstrains,
                           bool fTheoryMode)
{
    // save received data
    m_pConstrains = pConstrains;
    m_fTheoryMode = fTheoryMode;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, "DlgCfgIdfyCadence");

        //
        //get pointers to all controls
        //

    // error messages and bitmaps
    m_pBmpKeySignError = XRCCTRL(*this, "bmpKeySignError", wxStaticBitmap);
    m_pLblKeySignError = XRCCTRL(*this, "lblKeySignError", wxStaticText);
    m_pBmpAllowedCadencesError = XRCCTRL(*this, "bmpAllowedCadencesError", wxStaticBitmap);
    m_pLblAllowedCadencesError = XRCCTRL(*this, "lblAllowedCadencesError", wxStaticText);
    m_pBmpButtonsError = XRCCTRL(*this, "bmpButtonsError", wxStaticBitmap);
    m_pLblButtonsError = XRCCTRL(*this, "lblButtonsError", wxStaticText);

    // allowed cadences checkboxes
    // correspondence with ECadenceType
        // Perfect authentic cadences
    m_pChkCadence[k_cadence_perfect_V_I] = XRCCTRL(*this, "chkCad_V_I", wxCheckBox);
    m_pChkCadence[k_cadence_perfect_V7_I] = XRCCTRL(*this, "chkCad_V7_I", wxCheckBox);
    m_pChkCadence[k_cadence_perfect_Va5_I] = XRCCTRL(*this, "chkCad_Va5_I", wxCheckBox);
    m_pChkCadence[k_cadence_perfect_Vd5_I] = XRCCTRL(*this, "chkCad_Vd5_I", wxCheckBox);

        // Plagal cadences
    m_pChkCadence[k_cadence_plagal_IV_I] = XRCCTRL(*this, "chkCad_IV_I", wxCheckBox);
    m_pChkCadence[k_cadence_plagal_IVm_I] = XRCCTRL(*this, "chkCad_IVm_I", wxCheckBox);
    m_pChkCadence[k_cadence_plagal_IIc6_I] = XRCCTRL(*this, "chkCad_IIc6_I", wxCheckBox);
    m_pChkCadence[k_cadence_plagal_IImc6_I] = XRCCTRL(*this, "chkCad_IImc6_I", wxCheckBox);

        // Imperfect authentic cadences
	m_pChkCadence[k_cadence_imperfect_V_I] = XRCCTRL(*this, "chkCad_V_I_inv", wxCheckBox);

        // Deceptive cadences
    m_pChkCadence[k_cadence_deceptive_V_IV] = XRCCTRL(*this, "chkCad_V_IV", wxCheckBox);
    m_pChkCadence[k_cadence_deceptive_V_IVm] = XRCCTRL(*this, "chkCad_V_IVm", wxCheckBox);
    m_pChkCadence[k_cadence_deceptive_V_VI] = XRCCTRL(*this, "chkCad_V_VI", wxCheckBox);
    m_pChkCadence[k_cadence_deceptive_V_VIm] = XRCCTRL(*this, "chkCad_V_VIm", wxCheckBox);
    m_pChkCadence[k_cadence_deceptive_V_IIm] = XRCCTRL(*this, "chkCad_V_IIm", wxCheckBox);
    m_pChkCadence[k_cadence_deceptive_V_III] = XRCCTRL(*this, "chkCad_V_III", wxCheckBox);
    m_pChkCadence[k_cadence_deceptive_V_VII] = XRCCTRL(*this, "chkCad_V_VII", wxCheckBox);

    // Half cadences
    m_pChkCadence[k_cadence_half_IImc6_V] = XRCCTRL(*this, "chkCad_IImc6_V", wxCheckBox);
    m_pChkCadence[k_cadence_half_IV_V] = XRCCTRL(*this, "chkCad_IV_V", wxCheckBox);
    m_pChkCadence[k_cadence_half_I_V] = XRCCTRL(*this, "chkCad_I_V", wxCheckBox);
    m_pChkCadence[k_cadence_half_Ic64_V] = XRCCTRL(*this, "chkCad_Ic64_V", wxCheckBox);
    m_pChkCadence[k_cadence_half_IV6_V] = XRCCTRL(*this, "chkCad_IV6_V", wxCheckBox);
    m_pChkCadence[k_cadence_half_II_V] = XRCCTRL(*this, "chkCad_II_V", wxCheckBox);
    m_pChkCadence[k_cadence_half_IIdimc6_V] = XRCCTRL(*this, "chkCad_N6_V", wxCheckBox);
    m_pChkCadence[k_cadence_half_VdeVdim5c64_V] = XRCCTRL(*this, "chkCad_VIa_V", wxCheckBox);

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
        // Minor keys
    m_pChkKeySign[k_key_a] = XRCCTRL(*this, "chkKeyAMinor", wxCheckBox);
    m_pChkKeySign[k_key_e] = XRCCTRL(*this, "chkKeyEMinor", wxCheckBox);
    m_pChkKeySign[k_key_b] = XRCCTRL(*this, "chkKeyBMinor", wxCheckBox);
    m_pChkKeySign[k_key_fs] = XRCCTRL(*this, "chkKeyFSharpMinor", wxCheckBox);
    m_pChkKeySign[k_key_cs] = XRCCTRL(*this, "chkKeyCSharpMinor", wxCheckBox);
    m_pChkKeySign[k_key_gs] = XRCCTRL(*this, "chkKeyGSharpMinor", wxCheckBox);
    m_pChkKeySign[k_key_ds] = XRCCTRL(*this, "chkKeyDSharpMinor", wxCheckBox);
    m_pChkKeySign[k_key_as] = XRCCTRL(*this, "chkKeyASharpMinor", wxCheckBox);
    m_pChkKeySign[k_key_af] = XRCCTRL(*this, "chkKeyAFlatMinor", wxCheckBox);
    m_pChkKeySign[k_key_ef] = XRCCTRL(*this, "chkKeyEFlatMinor", wxCheckBox);
    m_pChkKeySign[k_key_bf] = XRCCTRL(*this, "chkKeyBFlatMinor", wxCheckBox);
    m_pChkKeySign[k_key_f] = XRCCTRL(*this, "chkKeyFMinor", wxCheckBox);
    m_pChkKeySign[k_key_c] = XRCCTRL(*this, "chkKeyCMinor", wxCheckBox);
    m_pChkKeySign[k_key_g] = XRCCTRL(*this, "chkKeyGMinor", wxCheckBox);
    m_pChkKeySign[k_key_d] = XRCCTRL(*this, "chkKeyDMinor", wxCheckBox);

    // Allowed answer buttons
    m_pChkAnswerButton[lmBT_PERFECT] = XRCCTRL(*this, "chkButtonPerfect", wxCheckBox);
    m_pChkAnswerButton[lmBT_PLAGAL] = XRCCTRL(*this, "chkButtonPlagal", wxCheckBox);
    m_pChkAnswerButton[lmBT_HALF] = XRCCTRL(*this, "chkButtonHalf", wxCheckBox);
    m_pChkAnswerButton[lmBT_DECEPTIVE] = XRCCTRL(*this, "chkButtonDeceptive", wxCheckBox);
    m_pChkAnswerButton[lmBT_IMPERFECT] = XRCCTRL(*this, "chkButtonImperfect", wxCheckBox);

    // Check / uncheck all buttons
    m_pBtnCheckAll[lmBT_PERFECT] = XRCCTRL(*this, "btnCheckAllPerfect", wxButton);
    m_pBtnCheckAll[lmBT_PLAGAL] = XRCCTRL(*this, "btnCheckAllPlagal", wxButton);
    m_pBtnCheckAll[lmBT_HALF] = XRCCTRL(*this, "btnCheckAllHalf", wxButton);
    m_pBtnCheckAll[lmBT_DECEPTIVE] = XRCCTRL(*this, "btnCheckAllDeceptive", wxButton);

    //other controls
    m_pBoxAnswerType = XRCCTRL(*this, "radBoxAnswerType", wxRadioBox);
    m_pBoxShowKey = XRCCTRL(*this, "radBoxShowKey", wxRadioBox);

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));
    m_pBmpKeySignError->SetBitmap(bmpError);
    m_pBmpAllowedCadencesError->SetBitmap(bmpError);
    m_pBmpButtonsError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblAllowedCadencesError->Show(false);
    m_pBmpAllowedCadencesError->Show(false);
    m_pLblButtonsError->Show(false);
    m_pBmpButtonsError->Show(false);

        //
        // initialize all controls with current constraints data
        //

    // allowed key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (int i=0; i <= k_max_key; i++)
    {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignature)i) );
    }

    //initialize check boxes for allowed cadences with current settings
    for (int i=0; i < k_cadence_max; i++)
    {
        m_pChkCadence[i]->SetValue( m_pConstrains->IsCadenceValid((ECadenceType)i) );
    }

    // allowed answer buttons and cadence groups
    if (pConstrains->IsValidButton(lm_eCadButtonTerminal))
    {
 		//option 0 selected: use only terminal/transient answer buttons
        m_pBoxAnswerType->SetSelection(0);

		//1. disable all individual answer buttons and uncheck all them
		for (int i=0; i < 5; i++)
        {
			m_pChkAnswerButton[i]->Enable(false);
			m_pChkAnswerButton[i]->SetValue(false);
		}

		//2. In cadences tab, enable all check boxes and 'check all' buttons. Check status
		//is not be changed.
		for (int iCad=0; iCad < 5; iCad++)
        {
			SetCadenceCheckBoxes(iCad, true);
			if (iCad != lmBT_IMPERFECT)
                m_pBtnCheckAll[iCad]->Enable(true);
		}
   }
    else
    {
		//option 1: use an answer button for each cadence
        m_pBoxAnswerType->SetSelection(1);

		//1. Enable all individual answer buttons. Check buttons according settings
        for (int iCad=0; iCad < 5; iCad++)
        {
        		m_pChkAnswerButton[iCad]->Enable(true);
        }
        m_pChkAnswerButton[lmBT_PERFECT]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonPerfect) );
        m_pChkAnswerButton[lmBT_PLAGAL]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonPlagal) );
        m_pChkAnswerButton[lmBT_IMPERFECT]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonImperfect) );
        m_pChkAnswerButton[lmBT_DECEPTIVE]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonDeceptive) );
        m_pChkAnswerButton[lmBT_HALF]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonHalf) );

		//2. In cadences tab, disable all check boxes and 'check all' buttons for those
		//	 answer buttons not selected. When disabling then also uncheck them.
		for (int iCad=0; iCad < 5; iCad++)
        {
			if (!m_pChkAnswerButton[iCad]->IsChecked())
            {
				SetCadenceCheckBoxes(iCad, false, true, false);
				if (iCad != lmBT_IMPERFECT)
                    m_pBtnCheckAll[iCad]->Enable(false);
			}
		}
    }

    // This dialog is shared by EarTraining and Theory exercises.
    // Flag m_fTheoryMode controls whether to show/hide
    // specific controls used only in one of the exercises
    if (m_fTheoryMode)
    {
        //Hide controls used only in ear trainig mode
        m_pBoxShowKey->Show(false);
    }
    else
    {
        m_pBoxShowKey->Show(true);
        m_pBoxShowKey->SetSelection( m_pConstrains->GetKeyDisplayMode() );
    }


    //center dialog on screen
    CentreOnScreen();

}

//---------------------------------------------------------------------------------------
DlgCfgIdfyCadence::~DlgCfgIdfyCadence()
{
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnDataChanged(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.

    //save allowed cadences
    int i;
    for (i=0; i < k_cadence_max; i++)
    {
        m_pConstrains->SetCadenceValid((ECadenceType)i, m_pChkCadence[i]->GetValue());
    }

    // save selected key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i <= k_max_key; i++)
    {
        pKeyConstrains->SetValid((EKeySignature)i, m_pChkKeySign[i]->GetValue());
    }

    // save answer buttons
    if (m_pBoxAnswerType->GetSelection() == 0)
    {
        //Identify cadence group (terminal/transient)
        m_pConstrains->SetValidButton(lm_eCadButtonTerminal, true);
        m_pConstrains->SetValidButton(lm_eCadButtonTransient, true);
        m_pConstrains->SetValidButton(lm_eCadButtonPerfect, false);
	    m_pConstrains->SetValidButton(lm_eCadButtonPlagal, false);
        m_pConstrains->SetValidButton(lm_eCadButtonImperfect, false);
        m_pConstrains->SetValidButton(lm_eCadButtonDeceptive, false);
        m_pConstrains->SetValidButton(lm_eCadButtonHalf, false);
    }
    else
    {
        //Identify cadence type
        m_pConstrains->SetValidButton(lm_eCadButtonTerminal, false);
        m_pConstrains->SetValidButton(lm_eCadButtonTransient, false);
        m_pConstrains->SetValidButton(lm_eCadButtonPerfect, m_pChkAnswerButton[lmBT_PERFECT]->GetValue());
	    m_pConstrains->SetValidButton(lm_eCadButtonPlagal, m_pChkAnswerButton[lmBT_PLAGAL]->GetValue());
        m_pConstrains->SetValidButton(lm_eCadButtonImperfect, m_pChkAnswerButton[lmBT_IMPERFECT]->GetValue());
        m_pConstrains->SetValidButton(lm_eCadButtonDeceptive, m_pChkAnswerButton[lmBT_DECEPTIVE]->GetValue());
        m_pConstrains->SetValidButton(lm_eCadButtonHalf, m_pChkAnswerButton[lmBT_HALF]->GetValue());
    }

    //save other options

    // save options depending on mode: theory/ear training
    if (!m_fTheoryMode)
    {
        //ear training. Save setting about how to play key signature
        m_pConstrains->SetKeyDisplayMode(m_pBoxShowKey->GetSelection());
    }

    //terminate the dialog
    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
bool DlgCfgIdfyCadence::VerifyData()
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

    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblAllowedCadencesError->Show(false);
    m_pBmpAllowedCadencesError->Show(false);
    m_pLblButtonsError->Show(false);
    m_pBmpButtonsError->Show(false);

    // check answer buttons and enable/disable not applicable ones
    if (m_pBoxAnswerType->GetSelection() == 1)
    {
        //at least two buttons checked
        int nButtons = 0;
        for (int i=0; i < 5; i++)
        {
            if (m_pChkAnswerButton[i]->GetValue())
                nButtons++;
        }
        fError = (nButtons < 2);
        if (fError)
        {
            m_pLblButtonsError->Show(true);
            m_pBmpButtonsError->Show(true);
        }
        fLocalError |= fError;
    }


    // check that at least one cadence is selected
    fError = false;
    fAtLeastOne = false;
    for (i=0; i < k_cadence_max; i++)
    {
        if (m_pChkCadence[i]->GetValue())
        {
            fAtLeastOne = true;
            break;
        }
    }
    fError = !fAtLeastOne;
    if (fError)
    {
        m_pLblAllowedCadencesError->Show(true);
        m_pBmpAllowedCadencesError->Show(true);
    }
    fLocalError |= fError;


    // check that at least one key signature has been choosen
    fAtLeastOne = false;
    for (i=0; i <= k_max_key; i++)
    {
        fAtLeastOne |= m_pChkKeySign[i]->GetValue();
    }
    fError = !fAtLeastOne;
    if (fError)
    {
        m_pLblKeySignError->Show(true);
        m_pBmpKeySignError->Show(true);
    }
    fLocalError |= fError;


        //
        // Check for global errors
        //

    fGlobalError = false;   //no global checkings in this dlg


    //enable / disable accept button
    wxButton* pButtonAccept = XRCCTRL(*this, "buttonAccept", wxButton);
    pButtonAccept->Enable(!fLocalError && !fGlobalError);

    return fLocalError;

}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnRadAnswerType(wxCommandEvent& WXUNUSED(event))
{
	//One of the two radio buttons to select the group of anser buttons has been changed.
	//If option 0 (use only terminal/transient answer buttons) is selected we must:
	//	- Disable all group 1 check boxes (Perfect, plagal, half, ...) and uncheck all them
	//	- In cadences tab, enable all check boxes and 'check all' buttons. Check status
	//		should not be changed.
	//
	//If option 1 (use an answer button for each cadence) is selected we must:
	//	- Enable all group 1 check boxes (Perfect, plagal, half, ...). Check buttons according
	//		checks in cadences tab (i.e. if there at least one perfect cadence checked, check
	//		button 'perfect')
	//	- In cadences tab, disable all check boxes and 'check all' buttons for those
	//		answer buttons not selected. When disabling then also uncheck them.
	//


	if (m_pBoxAnswerType->GetSelection() == 0)
	{
		//option 0 selected: use only terminal/transient answer buttons

		//1. disable all individual answer buttons and uncheck all them
		for (int i=0; i < 5; i++)
        {
			m_pChkAnswerButton[i]->Enable(false);
			m_pChkAnswerButton[i]->SetValue(false);
		}

		//2. In cadences tab, enable all check boxes and 'check all' buttons. Check status
		//is not be changed.
		for (int iCad=0; iCad < 5; iCad++)
        {
			SetCadenceCheckBoxes(iCad, true);
			if (iCad != lmBT_IMPERFECT)
                m_pBtnCheckAll[iCad]->Enable(true);
		}
	}
	else
	{
		//option 1: use an answer button for each cadence

		//1. Enable all individual answer buttons. Check buttons according
		//	checks in cadences tab (i.e. if there at least one perfect cadence
		//	checked, check button 'perfect')
		bool fCheck = false;
		m_pChkAnswerButton[lmBT_PERFECT]->Enable(true);
		for (int i=k_cadence_perfect; i < k_cadence_last_perfect; i++)
        {
			if (m_pChkCadence[i]->IsChecked())
            {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_PERFECT]->SetValue(fCheck);

		fCheck = false;
		m_pChkAnswerButton[lmBT_PLAGAL]->Enable(true);
		for (int i=k_cadence_plagal; i < k_cadence_last_plagal; i++)
        {
			if (m_pChkCadence[i]->IsChecked())
            {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_PLAGAL]->SetValue(fCheck);

		fCheck = false;
		m_pChkAnswerButton[lmBT_HALF]->Enable(true);
		for (int i=k_cadence_half; i < k_cadence_last_half; i++)
        {
			if (m_pChkCadence[i]->IsChecked())
            {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_HALF]->SetValue(fCheck);

		fCheck = false;
		m_pChkAnswerButton[lmBT_DECEPTIVE]->Enable(true);
		for (int i=k_cadence_deceptive; i < k_cadence_last_deceptive; i++)
        {
			if (m_pChkCadence[i]->IsChecked())
            {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_DECEPTIVE]->SetValue(fCheck);

		fCheck = false;
		m_pChkAnswerButton[lmBT_IMPERFECT]->Enable(true);
		for (int i=k_cadence_imperfect; i < k_cadence_last_imperfect; i++)
        {
			if (m_pChkCadence[i]->IsChecked())
            {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_IMPERFECT]->SetValue(fCheck);

		//2. In cadences tab, disable all check boxes and 'check all' buttons for those
		//	 answer buttons not selected. When disabling then also uncheck them.
		for (int iCad=0; iCad < 5; iCad++)
        {
			if (!m_pChkAnswerButton[iCad]->IsChecked())
            {
				SetCadenceCheckBoxes(iCad, false, true, false);
				if (iCad != lmBT_IMPERFECT)
                    m_pBtnCheckAll[iCad]->Enable(false);
			}
		}
	}

    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnCheckAllMajor(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkKeySign[k_min_major_key]->GetValue();

    for (int i=k_min_major_key; i <= k_max_major_key; i++)
    {
        m_pChkKeySign[i]->SetValue(fCheck);
    }
    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnCheckAllMinor(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkKeySign[k_min_minor_key]->GetValue();

    for (int i=k_min_minor_key; i <= k_max_minor_key; i++)
    {
        m_pChkKeySign[i]->SetValue(fCheck);
    }
    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnCheckAllPerfect(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkCadence[k_cadence_perfect]->GetValue();
    SetCadenceCheckBoxes(lmBT_PERFECT, true, true, fCheck);
    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnCheckAllPlagal(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkCadence[k_cadence_plagal]->GetValue();
    SetCadenceCheckBoxes(lmBT_PLAGAL, true, true, fCheck);
    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnCheckAllHalf(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkCadence[k_cadence_half]->GetValue();
    SetCadenceCheckBoxes(lmBT_HALF, true, true, fCheck);
    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnCheckAllDeceptive(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkCadence[k_cadence_deceptive]->GetValue();
    SetCadenceCheckBoxes(lmBT_DECEPTIVE, true, true, fCheck);
    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::OnAnswerButton(wxCommandEvent& WXUNUSED(event))
{
	// A cadence answer button has been checked/unchecked.

	for (int iCad=0; iCad < 5; iCad++)
	{
		bool fEnabled = m_pChkAnswerButton[iCad]->IsChecked();

		if (fEnabled)
        {
			//if button is checked, in cadences tab, enable all check boxes and 'check all'
			//button. Check status is not changed.
			SetCadenceCheckBoxes(iCad, true);
			if (iCad != lmBT_IMPERFECT)
                m_pBtnCheckAll[iCad]->Enable(true);
		}
		else
        {
			//if button is uncecked, in cadences tab, disable all check boxes and 'check all'
			//button. Check status is set to 'unchecked'
			SetCadenceCheckBoxes(iCad, false, true, false);
			if (iCad != lmBT_IMPERFECT)
                m_pBtnCheckAll[iCad]->Enable(false);
		}
    }
    VerifyData();
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::SetAnswerButton(int iButton, bool fEnable)
{
	//Enable/disable the check boxes for cadences in the group and the 'check all' button
	//It doesn't alter the check boxes content (tick mark)

	m_pChkAnswerButton[iButton]->Enable(fEnable);
	m_pChkAnswerButton[iButton]->SetValue(false);
	SetCadenceCheckBoxes(iButton, fEnable);
	if (iButton != lmBT_IMPERFECT)
        m_pBtnCheckAll[iButton]->Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void DlgCfgIdfyCadence::SetCadenceCheckBoxes(int iCad, bool fEnable, bool fChangeCheck,
											   bool fNewCheckValue)
{
	//Enable/disable the check boxes for cadences in a cadences group and, optionaly
	//if enable operation, a new check value (tick mark) is set

	int iStart;
	int iEnd;
	switch (iCad)
    {
		case lmBT_PERFECT:
			iStart = k_cadence_perfect;
			iEnd = k_cadence_last_perfect;
			break;

		case lmBT_PLAGAL:
			iStart = k_cadence_plagal;
			iEnd = k_cadence_last_plagal;
			break;

		case lmBT_HALF:
			iStart = k_cadence_half;
			iEnd = k_cadence_last_half;
			break;

		case lmBT_DECEPTIVE:
			iStart = k_cadence_deceptive;
			iEnd = k_cadence_last_deceptive;
			break;

		case lmBT_IMPERFECT:
			iStart = k_cadence_imperfect;
			iEnd = k_cadence_last_imperfect;
			break;

        default:
            LOMSE_LOG_ERROR("Invalid cadence %d", iCad);
            iCad = lmBT_PERFECT;
			iStart = k_cadence_perfect;
			iEnd = k_cadence_last_perfect;
	}

    for (int i=iStart; i < iEnd; i++)
    {
        m_pChkCadence[i]->Enable(fEnable);
        if (fChangeCheck) m_pChkCadence[i]->SetValue(fNewCheckValue);
    }
}


}  //namespace lenmus
