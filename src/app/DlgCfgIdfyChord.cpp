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
//    for (any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file DlgCfgIdfyChord.cpp
    @brief Implementation file for class lmDlgCfgIdfyChord
    @ingroup app_gui
*/

//for GCC
#ifdef __GNUG__
    #pragma implementation "DlgCfgIdfyChord.h"
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



#include "DlgCfgIdfyChord.h"
#include "../ldp_parser/AuxString.h"
#include "../auxmusic/Conversion.h"



//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDlgCfgIdfyChord, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), lmDlgCfgIdfyChord::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), lmDlgCfgIdfyChord::OnCancelClicked )
    EVT_NOTEBOOK_PAGE_CHANGING( XRCID( "noteBook" ), lmDlgCfgIdfyChord::OnPageChanging ) 

    // Type of interval check boxes
    EVT_CHECKBOX( XRCID( "chkModeHarmonic" ), lmDlgCfgIdfyChord::OnChkModeClicked )
    EVT_CHECKBOX( XRCID( "chkModeMelodicAsc" ), lmDlgCfgIdfyChord::OnChkModeClicked )
    EVT_CHECKBOX( XRCID( "chkModeMelodicDesc" ), lmDlgCfgIdfyChord::OnChkModeClicked )

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), lmDlgCfgIdfyChord::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), lmDlgCfgIdfyChord::OnChkKeyClicked )

    // Allowed chords check boxes
    EVT_CHECKBOX( XRCID( "chkChordMajorTriad" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordMinorTriad" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordAugTriad" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordDimTriad" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordSus4th" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordSus2nd" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordMajor7" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordDominant7" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordMinor7" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordDim7" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordHalfDim7" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordAugMajor7" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordAug7" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordMinorMajor7" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordMajor6" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordMinor6" ), lmDlgCfgIdfyChord::OnChkChordClicked )
    EVT_CHECKBOX( XRCID( "chkChordAug6" ), lmDlgCfgIdfyChord::OnChkChordClicked )

END_EVENT_TABLE()



lmDlgCfgIdfyChord::lmDlgCfgIdfyChord(wxWindow * parent,
                           lmChordConstrains* pConstrains,
                           bool fTheoryMode)
{
    // save received data
    m_pConstrains = pConstrains;
    m_fTheoryMode = fTheoryMode;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("DlgCfgIdfyChord"));

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

    // allowed intervals checkboxes
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

    // allowed types of intervals
    m_pChkPlayMode[0] = XRCCTRL(*this, "chkModeHarmonic", wxCheckBox);
    m_pChkPlayMode[1] = XRCCTRL(*this, "chkModeMelodicAsc", wxCheckBox);
    m_pChkPlayMode[2] = XRCCTRL(*this, "chkModeMelodicDesc", wxCheckBox);

    //other controls
    m_pChkAllowInversions = XRCCTRL(*this, "chkAllowInversions", wxCheckBox);
    m_pChkDisplayKey = XRCCTRL(*this, "chkDisplayKey", wxCheckBox);

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, wxSize(16,16));
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
        // initialize all controls with current constrains data
        //

    //initialize check boxes for allowed chords with current settings
    int i;
    for (i=0; i < ect_Max; i++) {
        m_pChkChord[i]->SetValue( m_pConstrains->IsValid((EChordType)i) );
    }
    
    //play modes
    for (i=0; i < 3; i++) {
        m_pChkPlayMode[i]->SetValue( m_pConstrains->IsModeAllowed(i) );
    }

    //// selected key signatures
    //lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    //for (i=0; i < earmFa+1; i++) {
    //    m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignatures)i) );
    //}

    // other
    m_pChkAllowInversions->SetValue( m_pConstrains->InversionsAllowed() );
    m_pChkDisplayKey->SetValue( m_pConstrains->DisplayKey() );

    //// As this dialog is shared by EarTraining and Theory. 
    //// Fflag m_fTheoryMode controls whether to show/hide
    //// specific controls used only in one of the exercises
    //if (m_fTheoryMode) {
    //    // This dialog is being used for Theory
    //    m_pChkAllowInversions->SetValue( m_pConstrains->IsTheoryMode() );
    //}
    //else {
    //    // This dialog is being used by Ear Training
    //    // So, hide play modes
    //    m_pChkAllowInversions->Show(false);
    //}


    
    //center dialog on screen
    CentreOnScreen();

}

lmDlgCfgIdfyChord::~lmDlgCfgIdfyChord()
{
}

void lmDlgCfgIdfyChord::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.
/*
    //save allowed intervals
    int i;
    for (i=0; i < ect_Max; i++) {
        m_pConstrains->SetIntervalAllowed(i, m_pChkChord[i]->GetValue());
    }
    
    //save notes range
    wxString sPitch = m_pCboFromNote->GetValue();
    lmPitch nPitch;
    EAccidentals nAccidentals;
    PitchNameToData(sPitch, &nPitch, &nAccidentals);
    m_pConstrains->SetMinNote(nPitch);

    sPitch = m_pCboToNote->GetValue();
    PitchNameToData(sPitch, &nPitch, &nAccidentals);
    m_pConstrains->SetMaxNote(nPitch);

    // save intervals' type
    for (i=0; i < 3; i++) {
        m_pConstrains->SetTypeAllowed(i, m_pChkPlayMode[i]->GetValue());
    }
    
    // save accidentals option and selected key signatures
    wxRadioBox* pAccidentals = XRCCTRL(*this, "radAccidentals", wxRadioBox);
    bool fOnlyNatural = (pAccidentals->GetSelection() == 0);
    m_pConstrains->SetOnlyNatural( fOnlyNatural );
    if (fOnlyNatural) {
        // store selected key signatures
        lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
        for (i=0; i < earmFa+1; i++) {
            pKeyConstrains->SetValid((EKeySignatures)i, m_pChkKeySign[i]->GetValue());
        }
    }
    
    // If this dialog is being used by EarCompareIntvCtrol, save first note equal value
    if (m_fTheoryMode) {
        m_pConstrains->SetFirstNoteEqual( m_pChkAllowInversions->GetValue() );
    }
*/
    //terminate the dialog 
    EndModal(wxID_OK);      
}

bool lmDlgCfgIdfyChord::VerifyData()
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
    for (i=0; i < ect_Max; i++) {
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
    
    //check that at least one play mode is choosen
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
     
  
    //enable / disable accept button
    wxButton* pButtonAccept = XRCCTRL(*this, "buttonAccept", wxButton);
    pButtonAccept->Enable(!fLocalError && !fGlobalError);

    return fLocalError;
    
}

/*! This event handler receives control when the selected tab is about to be changed.
    We must proceed to verify current tab data and veto the change if there are
    errors.
*/
void lmDlgCfgIdfyChord::OnPageChanging(wxNotebookEvent& event)
{
    //Do nothing when the notebook is being displayed, at dialog construction
    if ( event.GetOldSelection() == wxNOT_FOUND) return;

    //If execution reaches this point it is a real tab change. Verify data
    bool fError = VerifyData();
    if (fError) {
        event.Veto();
        wxMessageBox( _T("Correct errors before leaving this page."));
    }                

}

void lmDlgCfgIdfyChord::OnChkKeyClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCfgIdfyChord::OnChkModeClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCfgIdfyChord::EnableKeySignCheckBoxes(bool fEnable)
{
    int i;
    for (i=0; i < earmFa+1; i++) {
        m_pChkKeySign[i]->Enable(fEnable);
    }
}

void lmDlgCfgIdfyChord::OnChkChordClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

