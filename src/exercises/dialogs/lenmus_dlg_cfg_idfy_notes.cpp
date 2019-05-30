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
#include "lenmus_dlg_cfg_idfy_notes.h"
#include "lenmus_utilities.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/xrc/xmlres.h>

//lomse
#include <lomse_score_utilities.h>
using namespace lomse;


namespace lenmus
{


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

enum
{
	lmID_PANEL_NOTES = 2600,
	lmID_NOTES_FROM_KEY,
	lmID_KEY_SIGNATURE,
	lmID_NOTES_FROM_SELECTED,
	lmID_CHK_NOTE,
	lmID_PANEL_CLEF,
	lmID_CLEF,
	lmID_PANEL_OTHER,
	lmID_PLAY,
    lmID_RAD_OCTAVES,
	lmID_ACCEPT,
	lmID_CANCEL,
};


wxBEGIN_EVENT_TABLE(DlgCfgIdfyNotes, wxDialog)
    EVT_BUTTON(lmID_ACCEPT, DlgCfgIdfyNotes::OnAcceptClicked )
    EVT_BUTTON(lmID_CANCEL, DlgCfgIdfyNotes::OnCancelClicked )
    EVT_CHOICE(lmID_KEY_SIGNATURE, DlgCfgIdfyNotes::OnDataChanged )
    EVT_RADIOBUTTON(lmID_NOTES_FROM_KEY, DlgCfgIdfyNotes::OnRadioFromKeySignature )
    EVT_RADIOBUTTON(lmID_NOTES_FROM_SELECTED, DlgCfgIdfyNotes::OnRadioSelectedNotes )
    EVT_CHECKBOX(lmID_CHK_NOTE, DlgCfgIdfyNotes::OnDataChanged )
    EVT_RADIOBOX(lmID_CLEF, DlgCfgIdfyNotes::OnDataChanged )
    EVT_RADIOBOX(lmID_PLAY, DlgCfgIdfyNotes::OnDataChanged )
    EVT_RADIOBOX(lmID_RAD_OCTAVES, DlgCfgIdfyNotes::OnDataChanged )
wxEND_EVENT_TABLE()


DlgCfgIdfyNotes::DlgCfgIdfyNotes(wxWindow* parent,
                                           NotesConstrains* pConstrains)
    : wxDialog(parent, wxID_ANY, _("Exercise options"),
               wxDefaultPosition, wxDefaultSize,
               wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX )
    , m_pConstrains(pConstrains)
{
    // create the dialog controls
    create_controls();

    //copy pointers to notes check boxes to an array, to simplify coding
    m_pChkNote[0] = m_pChkC;
    m_pChkNote[1] = m_pChkCSharp;
    m_pChkNote[2] = m_pChkD;
    m_pChkNote[3] = m_pChkDSharp;
    m_pChkNote[4] = m_pChkE;
    m_pChkNote[5] = m_pChkF;
    m_pChkNote[6] = m_pChkFSharp;
    m_pChkNote[7] = m_pChkG;
    m_pChkNote[8] = m_pChkGSharp;
    m_pChkNote[9] = m_pChkA;
    m_pChkNote[10] = m_pChkASharp;
    m_pChkNote[11] = m_pChkB;

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));
    m_pBmpNotesError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pLblNotesError->Show(false);
    m_pBmpNotesError->Show(false);

        //
        // initialize all controls with current constraints data
        //

    //key signatures combo
    m_pCboKeySignature->Clear();
    for (int i = k_min_key; i <= k_max_key; i++)
    {
        EKeySignature nKey = static_cast<EKeySignature>(i);
        m_pCboKeySignature->Append( get_key_signature_name(nKey) );
    }
    m_pCboKeySignature->SetStringSelection( get_key_signature_name(pConstrains->GetKeySignature()) );

    //currently selected notes
    for (int i=0; i < 12; i++)
        m_pChkNote[i]->SetValue( pConstrains->IsValidNote(i) );

    //notes selection radio buttons
    EnableDisableNotesSelection(!pConstrains->SelectNotesFromKeySignature());

    //selected clef
    switch (pConstrains->GetClef())
    {
        case k_clef_G2:   m_pRadClefs->SetSelection(0);   break;
        case k_clef_F4:   m_pRadClefs->SetSelection(1);   break;
        case k_clef_F3:   m_pRadClefs->SetSelection(2);   break;
        case k_clef_C1:   m_pRadClefs->SetSelection(3);   break;
        case k_clef_C2:   m_pRadClefs->SetSelection(4);   break;
        case k_clef_C3:   m_pRadClefs->SetSelection(5);   break;
        case k_clef_C4:   m_pRadClefs->SetSelection(6);   break;
        default:
            break;
    }

    //octaves
    m_pRadOctaves->SetSelection( m_pConstrains->GetOctaves() - 1 );
}

DlgCfgIdfyNotes::~DlgCfgIdfyNotes()
{
}

void DlgCfgIdfyNotes::create_controls()
{
    //Copy as generated by wxFormBuilder. No changes.

	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

	wxBoxSizer* m_pMainSizer;
	m_pMainSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pNoteBook = LENMUS_NEW wxNotebook( this, lmID_PANEL_NOTES, wxDefaultPosition, wxDefaultSize, wxNB_TOP );
	m_pPanelNotes = LENMUS_NEW wxPanel( m_pNoteBook, lmID_PANEL_NOTES, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* m_pNotesSizer;
	m_pNotesSizer = LENMUS_NEW wxStaticBoxSizer( LENMUS_NEW wxStaticBox( m_pPanelNotes, wxID_ANY, _("Notes to practise") ), wxVERTICAL );

	wxBoxSizer* m_pKeySizer;
	m_pKeySizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pRadFromKeySignature = LENMUS_NEW wxRadioButton( m_pPanelNotes, lmID_NOTES_FROM_KEY, _("From this key signature:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pKeySizer->Add( m_pRadFromKeySignature, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* m_pKeyCboSizer;
	m_pKeyCboSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );


	m_pKeyCboSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	wxArrayString m_pCboKeySignatureChoices;
	m_pCboKeySignature = LENMUS_NEW wxChoice( m_pPanelNotes, lmID_KEY_SIGNATURE, wxDefaultPosition, wxDefaultSize, m_pCboKeySignatureChoices, 0 );
	m_pCboKeySignature->SetSelection( 0 );
	m_pKeyCboSizer->Add( m_pCboKeySignature, 4, wxALL, 5 );

	m_pKeySizer->Add( m_pKeyCboSizer, 1, wxEXPAND, 5 );

	m_pNotesSizer->Add( m_pKeySizer, 0, wxEXPAND, 5 );

	wxBoxSizer* m_pSelectedNotesSizer;
	m_pSelectedNotesSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pRadSelectedNotes = LENMUS_NEW wxRadioButton( m_pPanelNotes, lmID_NOTES_FROM_SELECTED, _("The following notes:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pSelectedNotesSizer->Add( m_pRadSelectedNotes, 0, wxALL, 5 );

	wxBoxSizer* m_pColumnsSizer;
	m_pColumnsSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );


	m_pColumnsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* m_pColumn1Sizer;
	m_pColumn1Sizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pChkC = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("B b / C"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn1Sizer->Add( m_pChkC, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkCSharp = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("C # / D b"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn1Sizer->Add( m_pChkCSharp, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkD = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("D"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn1Sizer->Add( m_pChkD, 0, wxEXPAND|wxALL, 5 );

	m_pChkDSharp = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("D # / E b"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn1Sizer->Add( m_pChkDSharp, 0, wxEXPAND|wxALL, 5 );

	m_pChkE = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("E / F b"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn1Sizer->Add( m_pChkE, 0, wxEXPAND|wxALL, 5 );

	m_pChkF = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("E # / F"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn1Sizer->Add( m_pChkF, 0, wxEXPAND|wxALL, 5 );

	m_pColumnsSizer->Add( m_pColumn1Sizer, 2, wxEXPAND, 5 );

	wxBoxSizer* m_pColumn2Sizer;
	m_pColumn2Sizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pChkFSharp = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("F # / G b"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn2Sizer->Add( m_pChkFSharp, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkG = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("G"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn2Sizer->Add( m_pChkG, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkGSharp = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("G # / A b"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn2Sizer->Add( m_pChkGSharp, 0, wxEXPAND|wxALL, 5 );

	m_pChkA = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("A"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn2Sizer->Add( m_pChkA, 0, wxEXPAND|wxALL, 5 );

	m_pChkASharp = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("A # / B b"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn2Sizer->Add( m_pChkASharp, 0, wxEXPAND|wxALL, 5 );

	m_pChkB = LENMUS_NEW wxCheckBox( m_pPanelNotes, lmID_CHK_NOTE, _("B / C b"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumn2Sizer->Add( m_pChkB, 0, wxEXPAND|wxALL, 5 );

	m_pColumnsSizer->Add( m_pColumn2Sizer, 2, wxEXPAND, 5 );

	m_pSelectedNotesSizer->Add( m_pColumnsSizer, 1, wxEXPAND|wxTOP, 5 );

	wxBoxSizer* m_pNotesErrorSizer;
	m_pNotesErrorSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBmpNotesError = LENMUS_NEW wxStaticBitmap( m_pPanelNotes, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_pNotesErrorSizer->Add( m_pBmpNotesError, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	m_pLblNotesError = LENMUS_NEW wxStaticText( m_pPanelNotes, wxID_ANY, _("You must choose one at least! "), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblNotesError->Wrap( -1 );
	m_pLblNotesError->SetBackgroundColour( wxColour( 255, 215, 215 ) );

	m_pNotesErrorSizer->Add( m_pLblNotesError, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_pSpaceNotes = LENMUS_NEW wxStaticText( m_pPanelNotes, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pSpaceNotes->Wrap( -1 );
	m_pNotesErrorSizer->Add( m_pSpaceNotes, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pSelectedNotesSizer->Add( m_pNotesErrorSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_pNotesSizer->Add( m_pSelectedNotesSizer, 0, wxEXPAND, 5 );

	m_pPanelNotes->SetSizer( m_pNotesSizer );
	m_pPanelNotes->Layout();
	m_pNotesSizer->Fit( m_pPanelNotes );
	m_pNoteBook->AddPage( m_pPanelNotes, _("Notes"), true );
	m_pPanelClef = LENMUS_NEW wxPanel( m_pNoteBook, lmID_PANEL_CLEF, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_pClefsSizer;
	m_pClefsSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	wxString m_pRadClefsChoices[] = { _("G clef (violin)"), _("F clef in 4th line (bass)"), _("F clef in 3rd line"), _("C clef on 1st line (soprano)"), _("C clef on 2nd line (mezzo soprano)"), _("C clef on 3rd line (contralto)"), _("C clef on 4th line (tenor)") };
	int m_pRadClefsNChoices = sizeof( m_pRadClefsChoices ) / sizeof( wxString );
	m_pRadClefs = LENMUS_NEW wxRadioBox( m_pPanelClef, lmID_CLEF, _("Clef to use"), wxDefaultPosition, wxDefaultSize, m_pRadClefsNChoices, m_pRadClefsChoices, 1, wxRA_SPECIFY_COLS );
	m_pRadClefs->SetSelection( 0 );
	m_pClefsSizer->Add( m_pRadClefs, 1, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_pPanelClef->SetSizer( m_pClefsSizer );
	m_pPanelClef->Layout();
	m_pClefsSizer->Fit( m_pPanelClef );
	m_pNoteBook->AddPage( m_pPanelClef, _("Clef"), false );
	m_pPanelOther = LENMUS_NEW wxPanel( m_pNoteBook, lmID_PANEL_OTHER, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_pOtherOptionsSizer;
	m_pOtherOptionsSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	wxString m_pRadOctavesChoices[] = { _("One octave"), _("Two octaves") };
	int m_pRadOctavesNChoices = sizeof( m_pRadOctavesChoices ) / sizeof( wxString );
	m_pRadOctaves = LENMUS_NEW wxRadioBox( m_pPanelOther, lmID_RAD_OCTAVES, _("How many octaves"), wxDefaultPosition, wxDefaultSize, m_pRadOctavesNChoices, m_pRadOctavesChoices, 1, wxRA_SPECIFY_COLS );
	m_pRadOctaves->SetSelection( 1 );
	m_pOtherOptionsSizer->Add( m_pRadOctaves, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );

	m_pPanelOther->SetSizer( m_pOtherOptionsSizer );
	m_pPanelOther->Layout();
	m_pOtherOptionsSizer->Fit( m_pPanelOther );
	m_pNoteBook->AddPage( m_pPanelOther, _("Other"), false );

	m_pMainSizer->Add( m_pNoteBook, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* m_pButtonsSizer;
	m_pButtonsSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBtnAccept = LENMUS_NEW wxButton( this, lmID_ACCEPT, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBtnAccept->SetDefault();
	m_pButtonsSizer->Add( m_pBtnAccept, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_pBtnCancel = LENMUS_NEW wxButton( this, lmID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBtnCancel->SetDefault();
	m_pButtonsSizer->Add( m_pBtnCancel, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_pMainSizer->Add( m_pButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	this->SetSizer( m_pMainSizer );
	this->Layout();
	m_pMainSizer->Fit( this );

	this->Centre( wxBOTH );
}

void DlgCfgIdfyNotes::OnDataChanged(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void DlgCfgIdfyNotes::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.

    // save notes selection mode and related data
    m_pConstrains->SetSelectNotesFromKeySignature( m_pRadFromKeySignature->GetValue() );
    if ( m_pRadFromKeySignature->GetValue() )
        m_pConstrains->SetKeySignature( static_cast<EKeySignature>(m_pCboKeySignature->GetSelection()) );

    //currently selected notes
    for (int i=0; i < 12; i++)
        m_pConstrains->SetValidNote(i, m_pChkNote[i]->GetValue() );

    //selected clef
    switch (m_pRadClefs->GetSelection())
    {
        case 0:   m_pConstrains->SetClef(k_clef_G2);   break;
        case 1:   m_pConstrains->SetClef(k_clef_F4);   break;
        case 2:   m_pConstrains->SetClef(k_clef_F3);   break;
        case 3:   m_pConstrains->SetClef(k_clef_C1);   break;
        case 4:   m_pConstrains->SetClef(k_clef_C2);   break;
        case 5:   m_pConstrains->SetClef(k_clef_C3);   break;
        case 6:   m_pConstrains->SetClef(k_clef_C4);   break;
    }

   // octaves
    m_pConstrains->SetOctaves( m_pRadOctaves->GetSelection() + 1 );

    //terminate the dialog
    EndModal(wxID_OK);
}

bool DlgCfgIdfyNotes::VerifyData()
{
    // Returns a boolean to enable or not a tab change. That is: returns true if there are
    // local errors (errors affecting only to the data in a tab) so not to enable a tab
    // change. If there are no tab local errors then returns false (althought it there might
    // be global errors -- coherence between data in different tabs --).
    //
    // Anyway, global errors are also checked. If there are no global neither local
    // errors the Accept button is enabled. Otherwise it is disabled.

    //assume no errors
    bool fError = false;
    bool fLocalError = false;
    bool fGlobalError = false;

    m_pLblNotesError->Show(false);
    m_pBmpNotesError->Show(false);

    // check that at least one note has been choosen
    if (m_pRadSelectedNotes->GetValue())
    {
        bool fAtLeastOneNote = false;
        for (int i=0; i < 12; i++)
            fAtLeastOneNote |= m_pChkNote[i]->GetValue();

        fError = !fAtLeastOneNote;
        if (fError) {
            m_pLblNotesError->Show(true);
            m_pBmpNotesError->Show(true);
        }
    }
    fLocalError |= fError;

        //
        // Check for global errors
        //

    fGlobalError = fLocalError;

    // set valid notes if key signature selected
    if ( m_pRadFromKeySignature->GetValue() )
    {
        EKeySignature nKey = static_cast<EKeySignature>(m_pCboKeySignature->GetSelection());
        int nAcc[7];
        KeyUtilities::get_accidentals_for_key(nKey, nAcc);
        m_pChkC->SetValue(nAcc[0] == 0 || nAcc[6] == 1);
        m_pChkCSharp->SetValue(nAcc[0] == 1 || nAcc[1] == -1);
        m_pChkD->SetValue(nAcc[1] == 0);
        m_pChkDSharp->SetValue(nAcc[1] == 1 || nAcc[2] == -1);
        m_pChkE->SetValue(nAcc[2] == 0 || nAcc[3] == -1);
        m_pChkF->SetValue(nAcc[3] == 0 || nAcc[2] == 1);
        m_pChkFSharp->SetValue(nAcc[3] == 1 || nAcc[4] == -1);
        m_pChkG->SetValue(nAcc[4]==0);
        m_pChkGSharp->SetValue(nAcc[4] == 1 || nAcc[5] == -1);
        m_pChkA->SetValue(nAcc[5] == 0);
        m_pChkASharp->SetValue(nAcc[5] == 1 || nAcc[6] == -1);
        m_pChkB->SetValue(nAcc[6] == 0 || nAcc[0] == -1);
    }

    //enable / disable accept button
    m_pBtnAccept->Enable(!fGlobalError);

    return fGlobalError;
}

void DlgCfgIdfyNotes::OnRadioFromKeySignature(wxCommandEvent& WXUNUSED(event))
{
    EnableDisableNotesSelection(false);
}

void DlgCfgIdfyNotes::OnRadioSelectedNotes(wxCommandEvent& WXUNUSED(event))
{
    EnableDisableNotesSelection(true);
}

void DlgCfgIdfyNotes::EnableDisableNotesSelection(bool fEnable)
{
    m_pRadFromKeySignature->SetValue(!fEnable);
    m_pRadSelectedNotes->SetValue(fEnable);

    //enable/disable notes checkboxes
    for (int i=0; i < 12; i++)
    {
        m_pChkNote[i]->Enable(fEnable);
    }

    //enable/disable cbo for key signature
    m_pCboKeySignature->Enable(!fEnable);

    VerifyData();
}


}   // namespace lenmus
