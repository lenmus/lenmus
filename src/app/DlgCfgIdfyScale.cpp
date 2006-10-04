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
/*! @file DlgCfgIdfyScale.cpp
    @brief Implementation file for class lmDlgCfgIdfyScale
    @ingroup app_gui
*/

//for GCC
#ifdef __GNUG__
    #pragma implementation "DlgCfgIdfyScale.h"
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



#include "DlgCfgIdfyScale.h"
#include "../ldp_parser/AuxString.h"
#include "../auxmusic/Conversion.h"



//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDlgCfgIdfyScale, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), lmDlgCfgIdfyScale::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), lmDlgCfgIdfyScale::OnCancelClicked )

    // Type of interval check boxes
    EVT_RADIOBOX( XRCID( "radBoxPlayModes" ), lmDlgCfgIdfyScale::OnDataChanged )

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), lmDlgCfgIdfyScale::OnDataChanged )

    // Allowed chords check boxes
    EVT_CHECKBOX( XRCID( "chkScaleMajorNatural" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleMajorTypeII" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleMajorTypeIII" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleMajorTypeIV" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleMinorNatural" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleMinorDorian" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleMinorHarmonic" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleMinorMelodic" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleGreekIonian" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleGreekDorian" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleGreekPhrygian" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleGreekLydian" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleGreekMixolydian" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleGreekAeolian" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleGreekLocrian" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScalePentatonicMinor" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScalePentatonicMajor" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleBlues" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleHeptatonic" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleWholeTones" ), lmDlgCfgIdfyScale::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkScaleChromatic" ), lmDlgCfgIdfyScale::OnDataChanged )

END_EVENT_TABLE()



lmDlgCfgIdfyScale::lmDlgCfgIdfyScale(wxWindow* parent,
                           lmScalesConstrains* pConstrains,
                           bool fTheoryMode)
{
    // save received data
    m_pConstrains = pConstrains;
    m_fTheoryMode = fTheoryMode;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("DlgCfgIdfyScale"));

        //
        //get pointers to all controls
        //

    // error messages and bitmaps
    m_pBmpKeySignError = XRCCTRL(*this, "bmpKeySignError", wxStaticBitmap);
    m_pLblKeySignError = XRCCTRL(*this, "lblKeySignError", wxStaticText);
    m_pBmpAllowedScalesError = XRCCTRL(*this, "bmpAllowedScalesError", wxStaticBitmap);
    m_pLblAllowedScalesError = XRCCTRL(*this, "lblAllowedScalesError", wxStaticText);

    // allowed scales checkboxes
    // correspondence with EScaleType
    m_pChkScale[est_MajorNatural] = XRCCTRL(*this, "chkScaleMajorNatural", wxCheckBox);
    m_pChkScale[est_MajorTypeII] = XRCCTRL(*this, "chkScaleMajorTypeII", wxCheckBox);
    m_pChkScale[est_MajorTypeIII] = XRCCTRL(*this, "chkScaleMajorTypeIII", wxCheckBox);
    m_pChkScale[est_MajorTypeIV] = XRCCTRL(*this, "chkScaleMajorTypeIV", wxCheckBox);
    m_pChkScale[est_MinorNatural] = XRCCTRL(*this, "chkScaleMinorNatural", wxCheckBox);
    m_pChkScale[est_MinorDorian] = XRCCTRL(*this, "chkScaleMinorDorian", wxCheckBox);
    m_pChkScale[est_MinorHarmonic] = XRCCTRL(*this, "chkScaleMinorHarmonic", wxCheckBox);
    m_pChkScale[est_MinorMelodic] = XRCCTRL(*this, "chkScaleMinorMelodic", wxCheckBox);
    m_pChkScale[est_GreekIonian] = XRCCTRL(*this, "chkScaleGreekIonian", wxCheckBox);
    m_pChkScale[est_GreekDorian] = XRCCTRL(*this, "chkScaleGreekDorian", wxCheckBox);
    m_pChkScale[est_GreekPhrygian] = XRCCTRL(*this, "chkScaleGreekPhrygian", wxCheckBox);
    m_pChkScale[est_GreekLydian] = XRCCTRL(*this, "chkScaleGreekLydian", wxCheckBox);
    m_pChkScale[est_GreekMixolydian] = XRCCTRL(*this, "chkScaleGreekMixolydian", wxCheckBox);
    m_pChkScale[est_GreekAeolian] = XRCCTRL(*this, "chkScaleGreekAeolian", wxCheckBox);
    m_pChkScale[est_GreekLocrian] = XRCCTRL(*this, "chkScaleGreekLocrian", wxCheckBox);
    m_pChkScale[est_PentatonicMinor] = XRCCTRL(*this, "chkScalePentatonicMinor", wxCheckBox);
    m_pChkScale[est_PentatonicMajor] = XRCCTRL(*this, "chkScalePentatonicMajor", wxCheckBox);
    m_pChkScale[est_Blues] = XRCCTRL(*this, "chkScaleBlues", wxCheckBox);
    m_pChkScale[est_WholeTones] = XRCCTRL(*this, "chkScaleWholeTones", wxCheckBox);
    m_pChkScale[est_Chromatic] = XRCCTRL(*this, "chkScaleChromatic", wxCheckBox);

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

    //other controls
    m_pChkDisplayKey = XRCCTRL(*this, "chkDisplayKey", wxCheckBox);
    m_pBoxPlayModes = XRCCTRL(*this, "radBoxPlayModes", wxRadioBox);

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, wxSize(16,16));
    m_pBmpKeySignError->SetBitmap(bmpError);
    m_pBmpAllowedScalesError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblAllowedScalesError->Show(false);
    m_pBmpAllowedScalesError->Show(false);

        //
        // initialize all controls with current constrains data
        //

    //initialize check boxes for allowed scales with current settings
    int i;
    for (i=0; i < est_Max; i++) {
        m_pChkScale[i]->SetValue( m_pConstrains->IsScaleValid((EScaleType)i) );
    }
    
    //play mode
    m_pBoxPlayModes->SetSelection( m_pConstrains->GetPlayMode() );

    // allowed key signatures
    lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < earmFa+1; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignatures)i) );
    }

    // other
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

lmDlgCfgIdfyScale::~lmDlgCfgIdfyScale()
{
}

void lmDlgCfgIdfyScale::OnDataChanged(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCfgIdfyScale::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.

    //save allowed scales
    int i;
    for (i=0; i < est_Max; i++) {
        m_pConstrains->SetScaleValid((EScaleType)i, m_pChkScale[i]->GetValue());
    }
    
    // save selected key signatures
    lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < earmFa+1; i++) {
        pKeyConstrains->SetValid((EKeySignatures)i, m_pChkKeySign[i]->GetValue());
    }

    //save other options
    m_pConstrains->SetDisplayKey( m_pChkDisplayKey->GetValue() );
    
    // save options depending on mode: theory/ear training
    if (!m_fTheoryMode) {
        //ear training. Save play modes
        m_pConstrains->SetPlayMode(m_pBoxPlayModes->GetSelection());
    }

    //terminate the dialog 
    EndModal(wxID_OK);      
}

bool lmDlgCfgIdfyScale::VerifyData()
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
    m_pLblAllowedScalesError->Show(false);
    m_pBmpAllowedScalesError->Show(false);
    
    // check that at least one scale is selected
    fError = false;
    fAtLeastOne = false;
    for (i=0; i < est_Max; i++) {
        if (m_pChkScale[i]->GetValue()) {
            fAtLeastOne = true;
            break;
        }
    }
    fError = !fAtLeastOne;
    if (fError) {
        m_pLblAllowedScalesError->Show(true);
        m_pBmpAllowedScalesError->Show(true);
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

     
        //
        // Check for global errors
        //
    
    fGlobalError = false;   //no global checkings in this dlg

  
    //enable / disable accept button
    wxButton* pButtonAccept = XRCCTRL(*this, "buttonAccept", wxButton);
    pButtonAccept->Enable(!fLocalError && !fGlobalError);

    return fLocalError;
    
}
