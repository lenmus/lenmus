//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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
#include "../../globals/Paths.h"
extern lmPaths* g_pPaths;



#include "DlgCfgIdfyChord.h"
#include "../../ldp_parser/AuxString.h"
#include "../../auxmusic/Conversion.h"



//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDlgCfgIdfyChord, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), lmDlgCfgIdfyChord::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), lmDlgCfgIdfyChord::OnCancelClicked )

    // Type of interval check boxes
    EVT_CHECKBOX( XRCID( "chkModeHarmonic" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkModeMelodicAsc" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkModeMelodicDesc" ), lmDlgCfgIdfyChord::OnDataChanged )

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), lmDlgCfgIdfyChord::OnDataChanged )

    // Allowed chords check boxes
    EVT_CHECKBOX( XRCID( "chkChordMajorTriad" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMinorTriad" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordAugTriad" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordDimTriad" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordSus4th" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordSus2nd" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMajor7" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordDominant7" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMinor7" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordDim7" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordHalfDim7" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordAugMajor7" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordAug7" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMinorMajor7" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMajor6" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordMinor6" ), lmDlgCfgIdfyChord::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkChordAug6" ), lmDlgCfgIdfyChord::OnDataChanged )

END_EVENT_TABLE()



lmDlgCfgIdfyChord::lmDlgCfgIdfyChord(wxWindow* parent,
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
    m_pBoxPlayModes = XRCCTRL(*this, "boxPlayModes", wxStaticBox);

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
        // initialize all controls with current constraints data
        //

    //initialize check boxes for allowed chords with current settings
    int i;
    for (i=0; i < ect_LastInExercises; i++) {
        m_pChkChord[i]->SetValue( m_pConstrains->IsChordValid((lmEChordType)i) );
    }
    
    //play modes
    for (i=0; i < 3; i++) {
        m_pChkPlayMode[i]->SetValue( m_pConstrains->IsModeAllowed(i) );
    }

    // allowed key signatures
    lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < earmFa+1; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((lmEKeySignatures)i) );
    }

    // other
    m_pChkAllowInversions->SetValue( m_pConstrains->AreInversionsAllowed() );
    m_pChkDisplayKey->SetValue( m_pConstrains->DisplayKey() );

    // As this dialog is shared by EarTraining and Theory. 
    // Flag m_fTheoryMode controls whether to show/hide
    // specific controls used only in one of the exercises
    if (m_fTheoryMode) {
        // This dialog is being used for Theory so, hide play modes
        m_pBoxPlayModes->Show(false);
    }


    //center dialog on screen
    CentreOnScreen();

}

lmDlgCfgIdfyChord::~lmDlgCfgIdfyChord()
{
}

void lmDlgCfgIdfyChord::OnDataChanged(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCfgIdfyChord::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.

    //save allowed chords
    int i;
    for (i=0; i < ect_LastInExercises; i++) {
        m_pConstrains->SetChordValid((lmEChordType)i, m_pChkChord[i]->GetValue());
    }
    
    // save selected key signatures
    lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < earmFa+1; i++) {
        pKeyConstrains->SetValid((lmEKeySignatures)i, m_pChkKeySign[i]->GetValue());
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
    for (i=0; i < ect_LastInExercises; i++) {
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
    for (i=0; i < earmFa+1; i++) {
        fAtLeastOne |= m_pChkKeySign[i]->GetValue();
    }
    fError = !fAtLeastOne;
    if (fError) {
        m_pLblKeySignError->Show(true);
        m_pBmpKeySignError->Show(true);
    }
    fLocalError |= fError;

    //check that at least one play mode is choosen
    if (m_fTheoryMode) {
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
