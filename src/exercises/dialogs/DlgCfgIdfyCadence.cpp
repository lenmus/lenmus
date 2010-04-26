//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//    for (any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DlgCfgIdfyCadence.h"
#endif

// for (compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dialog.h>
#include <wx/button.h>

#include <wx/xrc/xmlres.h>

// access to paths
#include "../../globals/Paths.h"
extern lmPaths* g_pPaths;



#include "DlgCfgIdfyCadence.h"
#include "../../ldp_parser/AuxString.h"
#include "../../auxmusic/Conversion.h"

enum {
    lmBT_PERFECT = 0,
    lmBT_PLAGAL,
    lmBT_HALF,
    lmBT_DECEPTIVE,
    lmBT_IMPERFECT,
};

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDlgCfgIdfyCadence, wxDialog)
    // Buttons
    EVT_BUTTON( XRCID( "buttonAccept" ), lmDlgCfgIdfyCadence::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), lmDlgCfgIdfyCadence::OnCancelClicked )
    EVT_BUTTON( XRCID( "btnCheckAllPerfect" ), lmDlgCfgIdfyCadence::OnCheckAllPerfect )
    EVT_BUTTON( XRCID( "btnCheckAllPlagal" ), lmDlgCfgIdfyCadence::OnCheckAllPlagal )
    EVT_BUTTON( XRCID( "btnCheckAllHalf" ), lmDlgCfgIdfyCadence::OnCheckAllHalf )
    EVT_BUTTON( XRCID( "btnCheckAllDeceptive" ), lmDlgCfgIdfyCadence::OnCheckAllDeceptive )
    EVT_BUTTON( XRCID( "btnCheckAllMajor" ), lmDlgCfgIdfyCadence::OnCheckAllMajor )
    EVT_BUTTON( XRCID( "btnCheckAllMinor" ), lmDlgCfgIdfyCadence::OnCheckAllMinor )

    // Radio button boxes
    EVT_RADIOBOX( XRCID( "radBoxShowKey" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_RADIOBOX( XRCID( "radBoxAnswerType" ), lmDlgCfgIdfyCadence::OnRadAnswerType )

    // answer buttons
    EVT_CHECKBOX( XRCID( "chkButtonPerfect" ), lmDlgCfgIdfyCadence::OnAnswerButton )
    EVT_CHECKBOX( XRCID( "chkButtonPlagal" ), lmDlgCfgIdfyCadence::OnAnswerButton )
    EVT_CHECKBOX( XRCID( "chkButtonImperfect" ), lmDlgCfgIdfyCadence::OnAnswerButton )
    EVT_CHECKBOX( XRCID( "chkButtonDeceptive" ), lmDlgCfgIdfyCadence::OnAnswerButton )
    EVT_CHECKBOX( XRCID( "chkButtonHalf" ), lmDlgCfgIdfyCadence::OnAnswerButton )

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), lmDlgCfgIdfyCadence::OnDataChanged )
        // Minor keys
    EVT_CHECKBOX( XRCID( "chkKeyAMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFSharpMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCSharpMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGSharpMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDSharpMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyASharpMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyAFlatMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEFlatMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBFlatMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDMinor" ), lmDlgCfgIdfyCadence::OnDataChanged )

    // Allowed cadences check boxes
    EVT_CHECKBOX( XRCID( "chkCad_V_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V7_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_Ic64_V" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_Va5_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_Vd5_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IV_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IVm_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_II_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IIm_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_VI_I" ), lmDlgCfgIdfyCadence::OnDataChanged )
	EVT_CHECKBOX( XRCID( "chkCad_V_I_inv" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_IV" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_IVm" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_VI" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_VIm" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_IIm" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_III" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_V_VII" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IImc6_V" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IV_V" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_I_V" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_IV6_V" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_II_V" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_N6_V" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkCad_VIa_V" ), lmDlgCfgIdfyCadence::OnDataChanged )

END_EVENT_TABLE()



lmDlgCfgIdfyCadence::lmDlgCfgIdfyCadence(wxWindow* parent,
                           lmCadencesConstrains* pConstrains,
                           bool fTheoryMode)
{
    // save received data
    m_pConstrains = pConstrains;
    m_fTheoryMode = fTheoryMode;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("DlgCfgIdfyCadence"));

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
    // correspondence with lmECadenceType
        // Perfect authentic cadences
    m_pChkCadence[lm_eCadPerfect_V_I] = XRCCTRL(*this, "chkCad_V_I", wxCheckBox);
    m_pChkCadence[lm_eCadPerfect_V7_I] = XRCCTRL(*this, "chkCad_V7_I", wxCheckBox);
    m_pChkCadence[lm_eCadPerfect_Va5_I] = XRCCTRL(*this, "chkCad_Va5_I", wxCheckBox);
    m_pChkCadence[lm_eCadPerfect_Vd5_I] = XRCCTRL(*this, "chkCad_Vd5_I", wxCheckBox);

        // Plagal cadences
    m_pChkCadence[lm_eCadPlagal_IV_I] = XRCCTRL(*this, "chkCad_IV_I", wxCheckBox);
    m_pChkCadence[lm_eCadPlagal_IVm_I] = XRCCTRL(*this, "chkCad_IVm_I", wxCheckBox);
    m_pChkCadence[lm_eCadPlagal_IIc6_I] = XRCCTRL(*this, "chkCad_IIc6_I", wxCheckBox);
    m_pChkCadence[lm_eCadPlagal_IImc6_I] = XRCCTRL(*this, "chkCad_IImc6_I", wxCheckBox);

        // Imperfect authentic cadences
	m_pChkCadence[lm_eCadImperfect_V_I] = XRCCTRL(*this, "chkCad_V_I_inv", wxCheckBox);

        // Deceptive cadences
    m_pChkCadence[lm_eCadDeceptive_V_IV] = XRCCTRL(*this, "chkCad_V_IV", wxCheckBox);
    m_pChkCadence[lm_eCadDeceptive_V_IVm] = XRCCTRL(*this, "chkCad_V_IVm", wxCheckBox);
    m_pChkCadence[lm_eCadDeceptive_V_VI] = XRCCTRL(*this, "chkCad_V_VI", wxCheckBox);
    m_pChkCadence[lm_eCadDeceptive_V_VIm] = XRCCTRL(*this, "chkCad_V_VIm", wxCheckBox);
    m_pChkCadence[lm_eCadDeceptive_V_IIm] = XRCCTRL(*this, "chkCad_V_IIm", wxCheckBox);
    m_pChkCadence[lm_eCadDeceptive_V_III] = XRCCTRL(*this, "chkCad_V_III", wxCheckBox);
    m_pChkCadence[lm_eCadDeceptive_V_VII] = XRCCTRL(*this, "chkCad_V_VII", wxCheckBox);

    // Half cadences
    m_pChkCadence[lm_eCadHalf_IImc6_V] = XRCCTRL(*this, "chkCad_IImc6_V", wxCheckBox);
    m_pChkCadence[lm_eCadHalf_IV_V] = XRCCTRL(*this, "chkCad_IV_V", wxCheckBox);
    m_pChkCadence[lm_eCadHalf_I_V] = XRCCTRL(*this, "chkCad_I_V", wxCheckBox);
    m_pChkCadence[lm_eCadHalf_Ic64_V] = XRCCTRL(*this, "chkCad_Ic64_V", wxCheckBox);
    m_pChkCadence[lm_eCadHalf_IV6_V] = XRCCTRL(*this, "chkCad_IV6_V", wxCheckBox);
    m_pChkCadence[lm_eCadHalf_II_V] = XRCCTRL(*this, "chkCad_II_V", wxCheckBox);
    m_pChkCadence[lm_eCadHalf_IIdimc6_V] = XRCCTRL(*this, "chkCad_N6_V", wxCheckBox);
    m_pChkCadence[lm_eCadHalf_VdeVdim5c64_V] = XRCCTRL(*this, "chkCad_VIa_V", wxCheckBox);

    // Allowed key signatures
    m_pChkKeySign[earmDo] = XRCCTRL(*this, "chkKeyC", wxCheckBox);
    m_pChkKeySign[earmSol] = XRCCTRL(*this, "chkKeyG", wxCheckBox);
    m_pChkKeySign[earmRe] = XRCCTRL(*this, "chkKeyD", wxCheckBox);
    m_pChkKeySign[earmLa] = XRCCTRL(*this, "chkKeyA", wxCheckBox);
    m_pChkKeySign[earmMi] = XRCCTRL(*this, "chkKeyE", wxCheckBox);
    m_pChkKeySign[earmSi] = XRCCTRL(*this, "chkKeyB", wxCheckBox);
    m_pChkKeySign[earmFas] = XRCCTRL(*this, "chkKeyFSharp", wxCheckBox);
    m_pChkKeySign[earmDos] = XRCCTRL(*this, "chkKeyCSharp", wxCheckBox);
    m_pChkKeySign[earmDob] = XRCCTRL(*this, "chkKeyCFlat", wxCheckBox);
    m_pChkKeySign[earmSolb] = XRCCTRL(*this, "chkKeyGFlat", wxCheckBox);
    m_pChkKeySign[earmReb] = XRCCTRL(*this, "chkKeyDFlat", wxCheckBox);
    m_pChkKeySign[earmLab] = XRCCTRL(*this, "chkKeyAFlat", wxCheckBox);
    m_pChkKeySign[earmMib] = XRCCTRL(*this, "chkKeyEFlat", wxCheckBox);
    m_pChkKeySign[earmSib] = XRCCTRL(*this, "chkKeyBFlat", wxCheckBox);
    m_pChkKeySign[earmFa] = XRCCTRL(*this, "chkKeyF", wxCheckBox);
        // Minor keys
    m_pChkKeySign[earmLam] = XRCCTRL(*this, "chkKeyAMinor", wxCheckBox);
    m_pChkKeySign[earmMim] = XRCCTRL(*this, "chkKeyEMinor", wxCheckBox);
    m_pChkKeySign[earmSim] = XRCCTRL(*this, "chkKeyBMinor", wxCheckBox);
    m_pChkKeySign[earmFasm] = XRCCTRL(*this, "chkKeyFSharpMinor", wxCheckBox);
    m_pChkKeySign[earmDosm] = XRCCTRL(*this, "chkKeyCSharpMinor", wxCheckBox);
    m_pChkKeySign[earmSolsm] = XRCCTRL(*this, "chkKeyGSharpMinor", wxCheckBox);
    m_pChkKeySign[earmResm] = XRCCTRL(*this, "chkKeyDSharpMinor", wxCheckBox);
    m_pChkKeySign[earmLasm] = XRCCTRL(*this, "chkKeyASharpMinor", wxCheckBox);
    m_pChkKeySign[earmLabm] = XRCCTRL(*this, "chkKeyAFlatMinor", wxCheckBox);
    m_pChkKeySign[earmMibm] = XRCCTRL(*this, "chkKeyEFlatMinor", wxCheckBox);
    m_pChkKeySign[earmSibm] = XRCCTRL(*this, "chkKeyBFlatMinor", wxCheckBox);
    m_pChkKeySign[earmFam] = XRCCTRL(*this, "chkKeyFMinor", wxCheckBox);
    m_pChkKeySign[earmDom] = XRCCTRL(*this, "chkKeyCMinor", wxCheckBox);
    m_pChkKeySign[earmSolm] = XRCCTRL(*this, "chkKeyGMinor", wxCheckBox);
    m_pChkKeySign[earmRem] = XRCCTRL(*this, "chkKeyDMinor", wxCheckBox);

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
         wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, wxSize(16,16));
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
    lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (int i=0; i <= lmMAX_KEY; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((lmEKeySignatures)i) );
    }

    //initialize check boxes for allowed cadences with current settings
    for (int i=0; i < lm_eCadMaxCadence; i++) {
        m_pChkCadence[i]->SetValue( m_pConstrains->IsCadenceValid((lmECadenceType)i) );
    }

    // allowed answer buttons and cadence groups
    if (pConstrains->IsValidButton(lm_eCadButtonTerminal))
    {
 		//option 0 selected: use only terminal/transient answer buttons
        m_pBoxAnswerType->SetSelection(0);

		//1. disable all individual answer buttons and uncheck all them
		for (int i=0; i < 5; i++) {
			m_pChkAnswerButton[i]->Enable(false);
			m_pChkAnswerButton[i]->SetValue(false);
		}

		//2. In cadences tab, enable all check boxes and 'check all' buttons. Check status
		//is not be changed.
		for (int iCad=0; iCad < 5; iCad++) {
			SetCadenceCheckBoxes(iCad, true);
			if (iCad != lmBT_IMPERFECT) m_pBtnCheckAll[iCad]->Enable(true);
		}
   }
    else
    {
		//option 1: use an answer button for each cadence
        m_pBoxAnswerType->SetSelection(1);

		//1. Enable all individual answer buttons. Check buttons according settings
        for (int iCad=0; iCad < 5; iCad++) {
        		m_pChkAnswerButton[iCad]->Enable(true);
        }
        m_pChkAnswerButton[lmBT_PERFECT]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonPerfect) );
        m_pChkAnswerButton[lmBT_PLAGAL]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonPlagal) );
        m_pChkAnswerButton[lmBT_IMPERFECT]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonImperfect) );
        m_pChkAnswerButton[lmBT_DECEPTIVE]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonDeceptive) );
        m_pChkAnswerButton[lmBT_HALF]->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonHalf) );

		//2. In cadences tab, disable all check boxes and 'check all' buttons for those
		//	 answer buttons not selected. When disabling then also uncheck them.
		for (int iCad=0; iCad < 5; iCad++) {
			if (!m_pChkAnswerButton[iCad]->IsChecked()) {
				SetCadenceCheckBoxes(iCad, false, true, false);
				if (iCad != lmBT_IMPERFECT) m_pBtnCheckAll[iCad]->Enable(false);
			}
		}
    }

    // This dialog is shared by EarTraining and Theory exercises.
    // Flag m_fTheoryMode controls whether to show/hide
    // specific controls used only in one of the exercises
    if (m_fTheoryMode) {
        //Hide controls used only in ear trainig mode
        m_pBoxShowKey->Show(false);
    }
    else {
        m_pBoxShowKey->Show(true);
        m_pBoxShowKey->SetSelection( m_pConstrains->GetKeyDisplayMode() );
    }


    //center dialog on screen
    CentreOnScreen();

}

lmDlgCfgIdfyCadence::~lmDlgCfgIdfyCadence()
{
}

void lmDlgCfgIdfyCadence::OnDataChanged(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCfgIdfyCadence::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.

    //save allowed cadences
    int i;
    for (i=0; i < lm_eCadMaxCadence; i++) {
        m_pConstrains->SetCadenceValid((lmECadenceType)i, m_pChkCadence[i]->GetValue());
    }

    // save selected key signatures
    lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i <= lmMAX_KEY; i++) {
        pKeyConstrains->SetValid((lmEKeySignatures)i, m_pChkKeySign[i]->GetValue());
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
    if (!m_fTheoryMode) {
        //ear training. Save setting about how to play key signature
        m_pConstrains->SetKeyDisplayMode(m_pBoxShowKey->GetSelection());
    }

    //terminate the dialog
    EndModal(wxID_OK);
}

bool lmDlgCfgIdfyCadence::VerifyData()
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
        for (int i=0; i < 5; i++) {
            if (m_pChkAnswerButton[i]->GetValue())
                nButtons++;
        }
        fError = (nButtons < 2);
        if (fError) {
            m_pLblButtonsError->Show(true);
            m_pBmpButtonsError->Show(true);
        }
        fLocalError |= fError;
    }


    // check that at least one cadence is selected
    fError = false;
    fAtLeastOne = false;
    for (i=0; i < lm_eCadMaxCadence; i++) {
        if (m_pChkCadence[i]->GetValue()) {
            fAtLeastOne = true;
            break;
        }
    }
    fError = !fAtLeastOne;
    if (fError) {
        m_pLblAllowedCadencesError->Show(true);
        m_pBmpAllowedCadencesError->Show(true);
    }
    fLocalError |= fError;


    // check that at least one key signature has been choosen
    fAtLeastOne = false;
    for (i=0; i <= lmMAX_KEY; i++) {
        fAtLeastOne |= m_pChkKeySign[i]->GetValue();
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

    fGlobalError = false;   //no global checkings in this dlg


    //enable / disable accept button
    wxButton* pButtonAccept = XRCCTRL(*this, "buttonAccept", wxButton);
    pButtonAccept->Enable(!fLocalError && !fGlobalError);

    return fLocalError;

}

void lmDlgCfgIdfyCadence::OnRadAnswerType(wxCommandEvent& WXUNUSED(event))
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
		for (int i=0; i < 5; i++) {
			m_pChkAnswerButton[i]->Enable(false);
			m_pChkAnswerButton[i]->SetValue(false);
		}

		//2. In cadences tab, enable all check boxes and 'check all' buttons. Check status
		//is not be changed.
		for (int iCad=0; iCad < 5; iCad++) {
			SetCadenceCheckBoxes(iCad, true);
			if (iCad != lmBT_IMPERFECT) m_pBtnCheckAll[iCad]->Enable(true);
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
		for (int i=lm_eCadPerfect; i < lm_eCadLastPerfect; i++) {
			if (m_pChkCadence[i]->IsChecked()) {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_PERFECT]->SetValue(fCheck);

		fCheck = false;
		m_pChkAnswerButton[lmBT_PLAGAL]->Enable(true);
		for (int i=lm_eCadPlagal; i < lm_eCadLastPlagal; i++) {
			if (m_pChkCadence[i]->IsChecked()) {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_PLAGAL]->SetValue(fCheck);

		fCheck = false;
		m_pChkAnswerButton[lmBT_HALF]->Enable(true);
		for (int i=lm_eCadHalf; i < lm_eCadLastHalf; i++) {
			if (m_pChkCadence[i]->IsChecked()) {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_HALF]->SetValue(fCheck);

		fCheck = false;
		m_pChkAnswerButton[lmBT_DECEPTIVE]->Enable(true);
		for (int i=lm_eCadDeceptive; i < lm_eCadLastDeceptive; i++) {
			if (m_pChkCadence[i]->IsChecked()) {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_DECEPTIVE]->SetValue(fCheck);

		fCheck = false;
		m_pChkAnswerButton[lmBT_IMPERFECT]->Enable(true);
		for (int i=lm_eCadImperfect; i < lm_eCadLastImperfect; i++) {
			if (m_pChkCadence[i]->IsChecked()) {
				fCheck = true;
				break;
			}
		}
		m_pChkAnswerButton[lmBT_IMPERFECT]->SetValue(fCheck);

		//2. In cadences tab, disable all check boxes and 'check all' buttons for those
		//	 answer buttons not selected. When disabling then also uncheck them.
		for (int iCad=0; iCad < 5; iCad++) {
			if (!m_pChkAnswerButton[iCad]->IsChecked()) {
				SetCadenceCheckBoxes(iCad, false, true, false);
				if (iCad != lmBT_IMPERFECT) m_pBtnCheckAll[iCad]->Enable(false);
			}
		}
	}

    VerifyData();
}

void lmDlgCfgIdfyCadence::OnCheckAllMajor(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkKeySign[earmDo]->GetValue();
    for (int i=0; i <= earmFa; i++) {
        m_pChkKeySign[i]->SetValue(fCheck);
    }
    VerifyData();
}

void lmDlgCfgIdfyCadence::OnCheckAllMinor(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkKeySign[earmLam]->GetValue();
    for (int i=earmLam; i <= lmMAX_KEY; i++) {
        m_pChkKeySign[i]->SetValue(fCheck);
    }
    VerifyData();
}

void lmDlgCfgIdfyCadence::OnCheckAllPerfect(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkCadence[lm_eCadPerfect]->GetValue();
    SetCadenceCheckBoxes(lmBT_PERFECT, true, true, fCheck);
    VerifyData();
}

void lmDlgCfgIdfyCadence::OnCheckAllPlagal(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkCadence[lm_eCadPlagal]->GetValue();
    SetCadenceCheckBoxes(lmBT_PLAGAL, true, true, fCheck);
    VerifyData();
}

void lmDlgCfgIdfyCadence::OnCheckAllHalf(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkCadence[lm_eCadHalf]->GetValue();
    SetCadenceCheckBoxes(lmBT_HALF, true, true, fCheck);
    VerifyData();
}

void lmDlgCfgIdfyCadence::OnCheckAllDeceptive(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkCadence[lm_eCadDeceptive]->GetValue();
    SetCadenceCheckBoxes(lmBT_DECEPTIVE, true, true, fCheck);
    VerifyData();
}

void lmDlgCfgIdfyCadence::OnAnswerButton(wxCommandEvent& WXUNUSED(event))
{
	// A cadence answer button has been checked/unchecked.

	for (int iCad=0; iCad < 5; iCad++)
	{
		bool fEnabled = m_pChkAnswerButton[iCad]->IsChecked();

		if (fEnabled) {
			//if button is checked, in cadences tab, enable all check boxes and 'check all'
			//button. Check status is not changed.
			SetCadenceCheckBoxes(iCad, true);
			if (iCad != lmBT_IMPERFECT) m_pBtnCheckAll[iCad]->Enable(true);
		}
		else {
			//if button is uncecked, in cadences tab, disable all check boxes and 'check all'
			//button. Check status is set to 'unchecked'
			SetCadenceCheckBoxes(iCad, false, true, false);
			if (iCad != lmBT_IMPERFECT) m_pBtnCheckAll[iCad]->Enable(false);
		}
    }
    VerifyData();

}

void lmDlgCfgIdfyCadence::SetAnswerButton(int iButton, bool fEnable)
{
	//Enable/disable the check boxes for cadences in the group and the 'check all' button
	//It doesn't alter the check boxes content (tick mark)

	m_pChkAnswerButton[iButton]->Enable(fEnable);
	m_pChkAnswerButton[iButton]->SetValue(false);
	SetCadenceCheckBoxes(iButton, fEnable);
	if (iButton != lmBT_IMPERFECT) m_pBtnCheckAll[iButton]->Enable(fEnable);
}

void lmDlgCfgIdfyCadence::SetCadenceCheckBoxes(int iCad, bool fEnable, bool fChangeCheck,
											   bool fNewCheckValue)
{
	//Enable/disable the check boxes for cadences in a cadences group and, optionaly
	//if enable operation, a new check value (tick mark) is set

	int iStart;
	int iEnd;
	switch (iCad) {
		case lmBT_PERFECT:
			iStart = lm_eCadPerfect;
			iEnd = lm_eCadLastPerfect;
			break;

		case lmBT_PLAGAL:
			iStart = lm_eCadPlagal;
			iEnd = lm_eCadLastPlagal;
			break;

		case lmBT_HALF:
			iStart = lm_eCadHalf;
			iEnd = lm_eCadLastHalf;
			break;

		case lmBT_DECEPTIVE:
			iStart = lm_eCadDeceptive;
			iEnd = lm_eCadLastDeceptive;
			break;

		case lmBT_IMPERFECT:
			iStart = lm_eCadImperfect;
			iEnd = lm_eCadLastImperfect;
			break;
	}

    for (int i=iStart; i < iEnd; i++) {
        m_pChkCadence[i]->Enable(fEnable);
        if (fChangeCheck) m_pChkCadence[i]->SetValue(fNewCheckValue);
    }

}


