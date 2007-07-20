//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
//    for (any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "DlgCfgIdfyCadence.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <wx/button.h>

#include "wx/xrc/xmlres.h"

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;



#include "DlgCfgIdfyCadence.h"
#include "../ldp_parser/AuxString.h"
#include "../auxmusic/Conversion.h"



//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDlgCfgIdfyCadence, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), lmDlgCfgIdfyCadence::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), lmDlgCfgIdfyCadence::OnCancelClicked )

    // Radio button boxes
    EVT_RADIOBOX( XRCID( "radBoxShowKey" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_RADIOBOX( XRCID( "radBoxAnswerType" ), lmDlgCfgIdfyCadence::OnRadAnswerType )

    // answer buttons
    EVT_CHECKBOX( XRCID( "chkButtonPerfect" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkButtonPlagal" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkButtonImperfect" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkButtonDeceptive" ), lmDlgCfgIdfyCadence::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkButtonHalf" ), lmDlgCfgIdfyCadence::OnDataChanged )

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

    // Allowed answer buttons
    m_pChkButtonPerfect = XRCCTRL(*this, "chkButtonPerfect", wxCheckBox);
    m_pChkButtonPlagal = XRCCTRL(*this, "chkButtonPlagal", wxCheckBox);
    m_pChkButtonImperfect = XRCCTRL(*this, "chkButtonImperfect", wxCheckBox);
    m_pChkButtonDeceptive = XRCCTRL(*this, "chkButtonDeceptive", wxCheckBox);
    m_pChkButtonHalf = XRCCTRL(*this, "chkButtonHalf", wxCheckBox);

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

    //initialize check boxes for allowed cadences with current settings
    int i;
    for (i=0; i < lm_eCadMaxCadence; i++) {
        m_pChkCadence[i]->SetValue( m_pConstrains->IsCadenceValid((lmECadenceType)i) );
    }
    
    ////play mode
    //m_pBoxShowKey->SetSelection( m_pConstrains->GetPlayMode() );

    // allowed key signatures
    lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < earmFa+1; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignatures)i) );
    }

    // allowed answer buttons
    if (pConstrains->IsValidButton(lm_eCadButtonTerminal))
    {
        //Identify cadence group (terminal/transient)
        m_pBoxAnswerType->SetSelection(0);

        m_pChkButtonPerfect->Enable(false);
        m_pChkButtonPlagal->Enable(false);
        m_pChkButtonImperfect->Enable(false);
        m_pChkButtonDeceptive->Enable(false);
        m_pChkButtonHalf->Enable(false);

        m_pChkButtonPerfect->SetValue(false);
        m_pChkButtonPlagal->SetValue(false);
        m_pChkButtonImperfect->SetValue(false);
        m_pChkButtonDeceptive->SetValue(false);
        m_pChkButtonHalf->SetValue(false);
    }
    else
    {
        //Identify cadence type
        m_pBoxAnswerType->SetSelection(1);

        m_pChkButtonPerfect->Enable(true);
        m_pChkButtonPlagal->Enable(true);
        m_pChkButtonImperfect->Enable(true);
        m_pChkButtonDeceptive->Enable(true);
        m_pChkButtonHalf->Enable(true);

        m_pChkButtonPerfect->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonPerfect) );
        m_pChkButtonPlagal->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonPlagal) );
        m_pChkButtonImperfect->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonImperfect) );
        m_pChkButtonDeceptive->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonDeceptive) );
        m_pChkButtonHalf->SetValue( m_pConstrains->IsValidButton(lm_eCadButtonHalf) );
    }

    // As this dialog is shared by EarTraining and Theory.
    // Flag m_fTheoryMode controls whether to show/hide
    // specific controls used only in one of the exercises
    if (m_fTheoryMode) {
        //// This dialog is being used for Theory so, hide Show key radio buttons
        //m_pBoxShowKey->Show(false);
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
    for (i=0; i < earmFa+1; i++) {
        pKeyConstrains->SetValid((EKeySignatures)i, m_pChkKeySign[i]->GetValue());
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
        m_pConstrains->SetValidButton(lm_eCadButtonPerfect, m_pChkButtonPerfect->GetValue());
	    m_pConstrains->SetValidButton(lm_eCadButtonPlagal, m_pChkButtonPlagal->GetValue());
        m_pConstrains->SetValidButton(lm_eCadButtonImperfect, m_pChkButtonImperfect->GetValue());
        m_pConstrains->SetValidButton(lm_eCadButtonDeceptive, m_pChkButtonDeceptive->GetValue());
        m_pConstrains->SetValidButton(lm_eCadButtonHalf, m_pChkButtonHalf->GetValue());
    }

    ////save other options
    //
    //// save options depending on mode: theory/ear training
    //if (!m_fTheoryMode) {
    //    //ear training. Save play modes
    //    m_pConstrains->SetPlayMode(m_pBoxShowKey->GetSelection());
    //}

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

    //bool fAtLeastOne;
    //int i;

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
        if (m_pChkButtonPerfect->GetValue()) nButtons++;
        if (m_pChkButtonPlagal->GetValue()) nButtons++;
        if (m_pChkButtonImperfect->GetValue()) nButtons++;
        if (m_pChkButtonDeceptive->GetValue()) nButtons++;
        if (m_pChkButtonHalf->GetValue()) nButtons++;
        fError = (nButtons < 2);
        if (fError) {
            m_pLblButtonsError->Show(true);
            m_pBmpButtonsError->Show(true);
        }
        fLocalError |= fError;
    }
    

    //// check that at least one cadence is selected
    //fError = false;
    //fAtLeastOne = false;
    //for (i=0; i < lm_eCadMaxCadence; i++) {
    //    if (m_pChkCadence[i]->GetValue()) {
    //        fAtLeastOne = true;
    //        break;
    //    }
    //}
    //fError = !fAtLeastOne;
    //if (fError) {
    //    m_pLblAllowedCadencesError->Show(true);
    //    m_pBmpAllowedCadencesError->Show(true);
    //}
    //fLocalError |= fError;
    //
    //// check that at least one key signature has been choosen
    //fAtLeastOne = false;
    //for (i=0; i < earmFa+1; i++) {
    //    fAtLeastOne |= m_pChkKeySign[i]->GetValue();
    //}
    //fError = !fAtLeastOne;
    //if (fError) {
    //    m_pLblKeySignError->Show(true);
    //    m_pBmpKeySignError->Show(true);
    //}
    //fLocalError |= fError;


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
    if (m_pBoxAnswerType->GetSelection() == 0)
    {
        //Identify cadence group (terminal/transient). Disable individual cadences
        m_pChkButtonPerfect->Enable(false);
        m_pChkButtonPlagal->Enable(false);
        m_pChkButtonImperfect->Enable(false);
        m_pChkButtonDeceptive->Enable(false);
        m_pChkButtonHalf->Enable(false);
    }
    else
    {
        //Identify cadence type. Enable individual cadences
        m_pChkButtonPerfect->Enable(true);
        m_pChkButtonPlagal->Enable(true);
        m_pChkButtonImperfect->Enable(true);
        m_pChkButtonDeceptive->Enable(true);
        m_pChkButtonHalf->Enable(true);
    }

    VerifyData();
}

