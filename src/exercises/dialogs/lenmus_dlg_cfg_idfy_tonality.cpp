//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
#include "lenmus_dlg_cfg_idfy_tonality.h"
#include "lenmus_constrains.h"
#include "lenmus_tonality_constrains.h"
#include "lenmus_utilities.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/xrc/xmlres.h>
#include <wx/checkbox.h>

namespace lenmus
{

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

enum
{
	lmID_NOTEBOOK = 3000,
	lmID_CHECK_KEY,
	lmID_BUTTON_ALL_MAJOR,
	lmID_BUTTON_ALL_MINOR,
	lmID_RAD_ANSWER_BUTTONS,
};


wxBEGIN_EVENT_TABLE(DlgCfgIdfyTonality, wxDialog)
    EVT_BUTTON(wxID_OK, DlgCfgIdfyTonality::OnAcceptClicked )
    EVT_BUTTON(wxID_CANCEL, DlgCfgIdfyTonality::OnCancelClicked )
    EVT_BUTTON(lmID_BUTTON_ALL_MAJOR, DlgCfgIdfyTonality::OnCheckAllMajor )
    EVT_BUTTON(lmID_BUTTON_ALL_MINOR, DlgCfgIdfyTonality::OnCheckAllMinor )
    EVT_RADIOBOX(lmID_RAD_ANSWER_BUTTONS, DlgCfgIdfyTonality::OnDataChanged )
    EVT_CHECKBOX(lmID_CHECK_KEY, DlgCfgIdfyTonality::OnDataChanged )
wxEND_EVENT_TABLE()


DlgCfgIdfyTonality::DlgCfgIdfyTonality(wxWindow* parent,
                                       TonalityConstrains* pConstrains)
    : wxDialog(parent, wxID_ANY, _("Identify tonality settings"),
               wxDefaultPosition, wxDefaultSize,
               wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX )
    , m_pConstrains(pConstrains)
{
    // create the dialog controls
    create_controls();

    // Copy key signature pointers to use an array
    m_pChkKeySign[k_key_C] = m_pChkKeyC;
    m_pChkKeySign[k_key_G] = m_pChkKeyG;
    m_pChkKeySign[k_key_D] = m_pChkKeyD;
    m_pChkKeySign[k_key_A] = m_pChkKeyA;
    m_pChkKeySign[k_key_E] = m_pChkKeyE;
    m_pChkKeySign[k_key_B] = m_pChkKeyB;
    m_pChkKeySign[k_key_Fs] = m_pChkKeyFSharp;
    m_pChkKeySign[k_key_Cs] = m_pChkKeyCSharp;
    m_pChkKeySign[k_key_Cf] = m_pChkKeyCFlat;
    m_pChkKeySign[k_key_Gf] = m_pChkKeyGFlat;
    m_pChkKeySign[k_key_Df] = m_pChkKeyDFlat;
    m_pChkKeySign[k_key_Af] = m_pChkKeyAFlat;
    m_pChkKeySign[k_key_Ef] = m_pChkKeyEFlat;
    m_pChkKeySign[k_key_Bf] = m_pChkKeyBFlat;
    m_pChkKeySign[k_key_F] = m_pChkKeyF;
        // Minor keys
    m_pChkKeySign[k_key_a] = m_pChkKeyAMinor;
    m_pChkKeySign[k_key_e] = m_pChkKeyEMinor;
    m_pChkKeySign[k_key_b] = m_pChkKeyBMinor;
    m_pChkKeySign[k_key_fs] = m_pChkKeyFSharpMinor;
    m_pChkKeySign[k_key_cs] = m_pChkKeyCSharpMinor;
    m_pChkKeySign[k_key_gs] = m_pChkKeyGSharpMinor;
    m_pChkKeySign[k_key_ds] = m_pChkKeyDSharpMinor;
    m_pChkKeySign[k_key_as] = m_pChkKeyASharpMinor;
    m_pChkKeySign[k_key_af] = m_pChkKeyAFlatMinor;
    m_pChkKeySign[k_key_ef] = m_pChkKeyEFlatMinor;
    m_pChkKeySign[k_key_bf] = m_pChkKeyBFlatMinor;
    m_pChkKeySign[k_key_f] = m_pChkKeyFMinor;
    m_pChkKeySign[k_key_c] = m_pChkKeyCMinor;
    m_pChkKeySign[k_key_g] = m_pChkKeyGMinor;
    m_pChkKeySign[k_key_d] = m_pChkKeyDMinor;

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));
    m_pBmpKeySignError->SetBitmap(bmpError);
    m_pBmpGlobalError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblGlobalError->Show(false);
    m_pBmpGlobalError->Show(false);

        //
        // initialize all controls with current constraints data
        //

    // allowed key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (int i=0; i <= k_max_key; i++)
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignature)i) );

    // allowed answer buttons and cadence groups
    m_pRadAnswerType->SetSelection( (pConstrains->UseMajorMinorButtons() ? 0 : 1) );
}

DlgCfgIdfyTonality::~DlgCfgIdfyTonality()
{
}

void DlgCfgIdfyTonality::create_controls()
{
    //Copy as generated by wxFormBuilder. No changes.

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

	wxBoxSizer* m_pMainSizer;
	m_pMainSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pNoteBook = LENMUS_NEW wxNotebook( this, lmID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_DEFAULT|wxNB_TOP );
	m_pPanelKeySignatures = LENMUS_NEW wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_pPanelKSSizer;
	m_pPanelKSSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* m_pKeySignaturesSizer;
	m_pKeySignaturesSizer = LENMUS_NEW wxStaticBoxSizer( LENMUS_NEW wxStaticBox( m_pPanelKeySignatures, wxID_ANY, _("Key signatures") ), wxVERTICAL );

	wxBoxSizer* m_pKSColumnsSizer;
	m_pKSColumnsSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* m_pColumnSizer1;
	m_pColumnSizer1 = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pChkKeyC = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("C Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer1->Add( m_pChkKeyC, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeyG = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("G Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer1->Add( m_pChkKeyG, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeyD = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("D Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer1->Add( m_pChkKeyD, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyA = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("A Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer1->Add( m_pChkKeyA, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyE = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("E Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer1->Add( m_pChkKeyE, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyB = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("B Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer1->Add( m_pChkKeyB, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyFSharp = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("F# Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer1->Add( m_pChkKeyFSharp, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyCSharp = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("C# Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer1->Add( m_pChkKeyCSharp, 0, wxEXPAND|wxALL, 5 );

	m_pKSColumnsSizer->Add( m_pColumnSizer1, 1, wxALIGN_TOP|wxALL, 5 );

	wxBoxSizer* m_pColumnSizer2;
	m_pColumnSizer2 = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pChkKeyCFlat = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Cb Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer2->Add( m_pChkKeyCFlat, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeyGFlat = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Gb Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer2->Add( m_pChkKeyGFlat, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeyDFlat = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Db Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer2->Add( m_pChkKeyDFlat, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyAFlat = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Ab Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer2->Add( m_pChkKeyAFlat, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyEFlat = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Eb Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer2->Add( m_pChkKeyEFlat, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyBFlat = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Bb Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer2->Add( m_pChkKeyBFlat, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyF = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("F Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer2->Add( m_pChkKeyF, 0, wxEXPAND|wxALL, 5 );

	m_pKSColumnsSizer->Add( m_pColumnSizer2, 1, wxALIGN_TOP|wxALL, 5 );

	wxBoxSizer* m_pColumnSizer3;
	m_pColumnSizer3 = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pChkKeyAMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("A minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer3->Add( m_pChkKeyAMinor, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeyEMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("E minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer3->Add( m_pChkKeyEMinor, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeyBMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("B minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer3->Add( m_pChkKeyBMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyFSharpMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("F# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer3->Add( m_pChkKeyFSharpMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyCSharpMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("C# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer3->Add( m_pChkKeyCSharpMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyGSharpMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("G# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer3->Add( m_pChkKeyGSharpMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyDSharpMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("D# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer3->Add( m_pChkKeyDSharpMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyASharpMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("A# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer3->Add( m_pChkKeyASharpMinor, 0, wxEXPAND|wxALL, 5 );

	m_pKSColumnsSizer->Add( m_pColumnSizer3, 1, wxALIGN_TOP|wxALL, 5 );

	wxBoxSizer* m_pColumnSizer4;
	m_pColumnSizer4 = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pChkKeyAFlatMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Ab minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer4->Add( m_pChkKeyAFlatMinor, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeyEFlatMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Eb minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer4->Add( m_pChkKeyEFlatMinor, 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeyBFlatMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Bb minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer4->Add( m_pChkKeyBFlatMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyFMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("F minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer4->Add( m_pChkKeyFMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyCMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("C minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer4->Add( m_pChkKeyCMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyGMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("G minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer4->Add( m_pChkKeyGMinor, 0, wxEXPAND|wxALL, 5 );

	m_pChkKeyDMinor = LENMUS_NEW wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("D minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pColumnSizer4->Add( m_pChkKeyDMinor, 0, wxEXPAND|wxALL, 5 );

	m_pKSColumnsSizer->Add( m_pColumnSizer4, 1, wxALIGN_TOP|wxALL, 5 );

	m_pKeySignaturesSizer->Add( m_pKSColumnsSizer, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* m_pSelectButtonsSizer;
	m_pSelectButtonsSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBtCheckAllMajor = LENMUS_NEW wxButton( m_pPanelKeySignatures, lmID_BUTTON_ALL_MAJOR, _("check/uncheck all major"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBtCheckAllMajor->SetDefault();
	m_pSelectButtonsSizer->Add( m_pBtCheckAllMajor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pBtCheckAllMinor = LENMUS_NEW wxButton( m_pPanelKeySignatures, lmID_BUTTON_ALL_MINOR, _("check/uncheck all minor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBtCheckAllMinor->SetDefault();
	m_pSelectButtonsSizer->Add( m_pBtCheckAllMinor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pKeySignaturesSizer->Add( m_pSelectButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	wxBoxSizer* m_pKSErrorSizer;
	m_pKSErrorSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBmpKeySignError = LENMUS_NEW wxStaticBitmap( m_pPanelKeySignatures, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_pKSErrorSizer->Add( m_pBmpKeySignError, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	m_pKSErrorSpaces = LENMUS_NEW wxStaticText( m_pPanelKeySignatures, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pKSErrorSpaces->Wrap( -1 );
	m_pKSErrorSizer->Add( m_pKSErrorSpaces, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pLblKeySignError = LENMUS_NEW wxStaticText( m_pPanelKeySignatures, wxID_ANY, _("You must choose at least one key signature! "), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblKeySignError->Wrap( -1 );
	m_pLblKeySignError->SetBackgroundColour( wxColour( 255, 215, 215 ) );

	m_pKSErrorSizer->Add( m_pLblKeySignError, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_pKeySignaturesSizer->Add( m_pKSErrorSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_pPanelKSSizer->Add( m_pKeySignaturesSizer, 0, wxEXPAND|wxALL, 5 );

	m_pPanelKeySignatures->SetSizer( m_pPanelKSSizer );
	m_pPanelKeySignatures->Layout();
	m_pPanelKSSizer->Fit( m_pPanelKeySignatures );
	m_pNoteBook->AddPage( m_pPanelKeySignatures, _("Key signatures"), true );
	m_pPanelOther = LENMUS_NEW wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_pOtherSizer;
	m_pOtherSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	wxString m_pRadAnswerTypeChoices[] = { _("Just identify type: major or minor"), _("Name the key signature") };
	int m_pRadAnswerTypeNChoices = sizeof( m_pRadAnswerTypeChoices ) / sizeof( wxString );
	m_pRadAnswerType = LENMUS_NEW wxRadioBox( m_pPanelOther, lmID_RAD_ANSWER_BUTTONS, _("Type of desired answer"), wxDefaultPosition, wxDefaultSize, m_pRadAnswerTypeNChoices, m_pRadAnswerTypeChoices, 2, wxRA_SPECIFY_ROWS );
	m_pRadAnswerType->SetSelection( 1 );
	m_pOtherSizer->Add( m_pRadAnswerType, 0, wxEXPAND|wxALL, 5 );

	m_pPanelOther->SetSizer( m_pOtherSizer );
	m_pPanelOther->Layout();
	m_pOtherSizer->Fit( m_pPanelOther );
	m_pNoteBook->AddPage( m_pPanelOther, _("Answer buttons"), false );

	m_pMainSizer->Add( m_pNoteBook, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* m_pErrorsSizer;
	m_pErrorsSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBmpGlobalError = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_pErrorsSizer->Add( m_pBmpGlobalError, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	m_pSpace1 = LENMUS_NEW wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pSpace1->Wrap( -1 );
	m_pErrorsSizer->Add( m_pSpace1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pLblGlobalError = LENMUS_NEW wxStaticText( this, wxID_ANY, _("You must choose at least one major and one minor in tab 'Key signatures'! "), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblGlobalError->Wrap( -1 );
	m_pLblGlobalError->SetBackgroundColour( wxColour( 255, 215, 215 ) );

	m_pErrorsSizer->Add( m_pLblGlobalError, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_pMainSizer->Add( m_pErrorsSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* m_pButtonsSizer;
	m_pButtonsSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBtAccept = LENMUS_NEW wxButton( this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBtAccept->SetDefault();
	m_pButtonsSizer->Add( m_pBtAccept, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pBtCancel = LENMUS_NEW wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBtCancel->SetDefault();
	m_pButtonsSizer->Add( m_pBtCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pMainSizer->Add( m_pButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	this->SetSizer( m_pMainSizer );
	this->Layout();
	m_pMainSizer->Fit( this );
}

void DlgCfgIdfyTonality::OnDataChanged(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void DlgCfgIdfyTonality::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.

    // save selected key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (int i=0; i <= k_max_key; i++) {
        pKeyConstrains->SetValid((EKeySignature)i, m_pChkKeySign[i]->GetValue());
    }

    // save answer type
	m_pConstrains->UseMajorMinorButtons( m_pRadAnswerType->GetSelection() == 0 );

    //terminate the dialog
    EndModal(wxID_OK);
}

bool DlgCfgIdfyTonality::VerifyData()
{
    // Returns a boolean to enable or not a tab change. That is: returns true if there are
    // local errors (errors affecting only to the data in a tab) so not to enable a tab
    // change. If there are no tab local errors then returns false (althought it there might
    // be global errors -- coherence between data in different tabs --).
    //
    // Anyway, global errors al also checked. If there are no global neither local
    // errors the Accept button is enabled. Otherwise it is disabled.

    //assume no errors
    bool fError = false;
    bool fLocalError = false;
    bool fGlobalError = false;

    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblGlobalError->Show(false);
    m_pBmpGlobalError->Show(false);

    // check that at least one key signature has been choosen
    bool fAtLeastOneMajor = false;
    for (int i=k_min_major_key; i <= k_min_major_key; i++) {
        fAtLeastOneMajor |= m_pChkKeySign[i]->GetValue();
    }
    bool fAtLeastOneMinor = false;
    for (int i=k_min_minor_key; i <= k_min_minor_key; i++) {
        fAtLeastOneMinor |= m_pChkKeySign[i]->GetValue();
    }
    fError = !(fAtLeastOneMajor || fAtLeastOneMinor);
    if (fError) {
        m_pLblKeySignError->Show(true);
        m_pBmpKeySignError->Show(true);
    }
    fLocalError |= fError;

        //
        // Check for global errors
        //

    fGlobalError = fLocalError;

    //if use only major/minor answer buttons check that at least one major and
    //one minor are selected
	if (!fGlobalError && m_pRadAnswerType->GetSelection() == 0)
	{
        fGlobalError = !(fAtLeastOneMajor && fAtLeastOneMinor);
        if (fGlobalError) {
            m_pLblGlobalError->Show(true);
            m_pBmpGlobalError->Show(true);
        }
    }

    //enable / disable accept button
    m_pBtAccept->Enable(!fGlobalError);

    return fGlobalError;
}

void DlgCfgIdfyTonality::OnCheckAllMajor(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkKeySign[k_key_C]->GetValue();
    for (int i=0; i <= k_key_F; i++) {
        m_pChkKeySign[i]->SetValue(fCheck);
    }
    VerifyData();
}

void DlgCfgIdfyTonality::OnCheckAllMinor(wxCommandEvent& WXUNUSED(event))
{
    bool fCheck = !m_pChkKeySign[k_key_a]->GetValue();
    for (int i=k_key_a; i <= k_max_key; i++) {
        m_pChkKeySign[i]->SetValue(fCheck);
    }
    VerifyData();
}


}  //namespace lenmus
