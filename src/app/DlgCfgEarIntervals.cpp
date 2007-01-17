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

//for GCC
#ifdef __GNUG__
    #pragma implementation "DlgCfgEarIntervals.h"
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



#include "DlgCfgEarIntervals.h"
#include "../ldp_parser/AuxString.h"
#include "../auxmusic/Conversion.h"



//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDlgCfgEarIntervals, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), lmDlgCfgEarIntervals::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), lmDlgCfgEarIntervals::OnCancelClicked )
    EVT_TEXT( XRCID( "cboFromNote" ), lmDlgCfgEarIntervals::OnCboFromNote )
    EVT_TEXT( XRCID( "cboToNote" ), lmDlgCfgEarIntervals::OnCboToNote )
    EVT_NOTEBOOK_PAGE_CHANGING( XRCID( "noteBook" ), lmDlgCfgEarIntervals::OnPageChanging ) 

    // Type of interval check boxes
    EVT_CHECKBOX( XRCID( "chkIntvalTypeHarmonic" ), lmDlgCfgEarIntervals::OnChkIntvalTypeClicked )
    EVT_CHECKBOX( XRCID( "chkIntvalTypeMelodicAsc" ), lmDlgCfgEarIntervals::OnChkIntvalTypeClicked )
    EVT_CHECKBOX( XRCID( "chkIntvalTypeMelodicDesc" ), lmDlgCfgEarIntervals::OnChkIntvalTypeClicked )

    // Key signature check boxes
    EVT_CHECKBOX( XRCID( "chkKeyC" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyG" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyD" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyA" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyE" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyB" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyFSharp" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyCSharp" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyCFlat" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyGFlat" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyDFlat" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyAFlat" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyEFlat" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyBFlat" ), lmDlgCfgEarIntervals::OnChkKeyClicked )
    EVT_CHECKBOX( XRCID( "chkKeyF" ), lmDlgCfgEarIntervals::OnChkKeyClicked )

    // Accidentals type: radio buttons
    EVT_RADIOBOX( XRCID( "radAccidentals" ), lmDlgCfgEarIntervals::OnRadAccidentalsClicked )

    // Alloved intervals check boxes
    EVT_CHECKBOX( XRCID( "chkIntval1" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval2min" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval2maj" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval3min" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval3maj" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval4" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval4aug" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval5" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval6min" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval6maj" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval7min" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval7maj" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval8" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval9min" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval9maj" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval10min" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval10maj" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval11" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval11aug" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval12" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval13min" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval13maj" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval14min" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval14maj" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )
    EVT_CHECKBOX( XRCID( "chkIntval2oct" ), lmDlgCfgEarIntervals::OnChkIntvalClicked )

    END_EVENT_TABLE()



lmDlgCfgEarIntervals::lmDlgCfgEarIntervals(wxWindow * parent,
                           lmEarIntervalsConstrains* pConstrains,
                           bool fEnableFirstEqual)
{
    // save received data
    m_pConstrains = pConstrains;
    m_fEnableFirstEqual = fEnableFirstEqual;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("DlgCfgEarIntervals"));

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

    // notes range
    m_pCboFromNote = XRCCTRL(*this, "cboFromNote", wxComboBox);
    m_pCboToNote = XRCCTRL(*this, "cboToNote", wxComboBox);

    // allowed types of intervals
    m_pChkIntvalType[0] = XRCCTRL(*this, "chkIntvalTypeHarmonic", wxCheckBox);
    m_pChkIntvalType[1] = XRCCTRL(*this, "chkIntvalTypeMelodicAsc", wxCheckBox);
    m_pChkIntvalType[2] = XRCCTRL(*this, "chkIntvalTypeMelodicDesc", wxCheckBox);

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, wxSize(16,16));
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
        // initialize all controls with current constrains data
        //

    //initialize check boxes for allowed intervals with current settings
    int i;
    for (i=0; i < lmNUM_INTVALS; i++) {
        m_pChkIntval[i]->SetValue( m_pConstrains->IsIntervalAllowed(i) );
    }
    
    // populate combos for minimum and maximun notes
    LoadCboBoxWithNoteNames(m_pCboFromNote, m_pConstrains->MinNote());
    LoadCboBoxWithNoteNames(m_pCboToNote, m_pConstrains->MaxNote());

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
    lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (i=0; i < earmFa+1; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignatures)i) );
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

lmDlgCfgEarIntervals::~lmDlgCfgEarIntervals()
{
}

/*! Accept button will be enabled only if all data habe been validated and is Ok. So
    when accept button is clicked we can proceed to save data.
*/
void lmDlgCfgEarIntervals::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    //save allowed intervals
    int i;
    for (i=0; i < lmNUM_INTVALS; i++) {
        m_pConstrains->SetIntervalAllowed(i, m_pChkIntval[i]->GetValue());
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
        m_pConstrains->SetTypeAllowed(i, m_pChkIntvalType[i]->GetValue());
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
    if (m_fEnableFirstEqual) {
        m_pConstrains->SetFirstNoteEqual( m_pChkStartSameNote->GetValue() );
    }

    //terminate the dialog 
    EndModal(wxID_OK);      
}

void lmDlgCfgEarIntervals::OnCboFromNote(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCfgEarIntervals::OnCboToNote(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

/*! Returns a boolean to enable or not a tab change. That is: returns true if there are
    local errors (errors affecting only to the data in a tab) so not to enable a tab
    change. If there are no tab local errors then returns false (althought it there might
    be global errors -- coherence between data in different tabs --).
    
    Anyway, global errors al also checked. If there are no global neither local 
    errors the Accept button is enabled. Otherwise it is disabled.

*/
bool lmDlgCfgEarIntervals::VerifyData()
{
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
    
    //verify that notes range is valid
    fError = false;
    wxString sFromPitch = m_pCboFromNote->GetValue();
    wxString sToPitch = m_pCboToNote->GetValue();
    lmPitch nToPitch, nFromPitch;
    EAccidentals nAccidentals;
    PitchNameToData(sFromPitch, &nFromPitch, &nAccidentals);
    PitchNameToData(sToPitch, &nToPitch, &nAccidentals);
    if (nFromPitch > nToPitch) {
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
    
    //check that at least one interval type is choosen
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
    // key signature has been choosen
    fAtLeastOne = true;     // assume no error
    wxRadioBox* pAccidentals = XRCCTRL(*this, "radAccidentals", wxRadioBox);
    if (pAccidentals->GetSelection() == 0) {
        fAtLeastOne = false;
        for (i=0; i < earmFa+1; i++) {
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
    //AWARE: nFromPitch and nToPitch where computed when checking the notes range
    int ntMidiMin = lmConverter::PitchToMidiPitch(nFromPitch);
    int ntMidiMax = lmConverter::PitchToMidiPitch(nToPitch);
    int nRange = wxMin(ntMidiMax - ntMidiMin, lmNUM_INTVALS);
    for (i=0; i <= nRange; i++) {
        if (m_pChkIntval[i]->GetValue()) {
            fError = false;
            break;
        }
    }
    
    if (fError) {
        m_pLblGeneralError->SetLabel(
_("It is nor possible to generate any interval. \
Selected notes range interval (tab 'Other settings') is lower than \
minimum allowed interval (tab 'Intervals')"));
    }
    else {
        //check that it is possible to generate the maximun allowed interval
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

/*! This event handler receives control when the selected tab is about to be changed.
    We must proceed to verify current tab data and veto the change if there are
    errors.
*/
void lmDlgCfgEarIntervals::OnPageChanging(wxNotebookEvent& event)
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

void lmDlgCfgEarIntervals::OnChkKeyClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCfgEarIntervals::OnChkIntvalTypeClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCfgEarIntervals::EnableKeySignCheckBoxes(bool fEnable)
{
    int i;
    for (i=0; i < earmFa+1; i++) {
        m_pChkKeySign[i]->Enable(fEnable);
    }
}

void lmDlgCfgEarIntervals::OnRadAccidentalsClicked(wxCommandEvent& WXUNUSED(event))
{
    wxRadioBox* pAccidentals = XRCCTRL(*this, "radAccidentals", wxRadioBox);
    bool fEnable = (pAccidentals->GetSelection() == 0);
    EnableKeySignCheckBoxes(fEnable);

    VerifyData();
}

void lmDlgCfgEarIntervals::OnChkIntvalClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

