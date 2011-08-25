//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "DlgCfgIdfyTonality.h"
//#endif
//
//// for (compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif
//
//#include <wx/dialog.h>
//#include <wx/button.h>
//
//#include <wx/xrc/xmlres.h>
//
//// access to paths
//#include "../../globals/Paths.h"
//extern lmPaths* g_pPaths;
//
//
//
//#include "DlgCfgIdfyTonality.h"
//#include "../../ldp_parser/AuxString.h"
//#include "../../auxmusic/Conversion.h"
//
////-----------------------------------------------------------------------------
//// Event table: connect the events to the handler functions to process them
////-----------------------------------------------------------------------------
//
//enum
//{
//	lmID_NOTEBOOK = 3000,
//	lmID_CHECK_KEY,
//	lmID_BUTTON_ALL_MAJOR,
//	lmID_BUTTON_ALL_MINOR,
//	lmID_RAD_ANSWER_BUTTONS,
//};
//
//
//BEGIN_EVENT_TABLE(DlgCfgIdfyTonality, wxDialog)
//    EVT_BUTTON(wxID_OK, DlgCfgIdfyTonality::OnAcceptClicked )
//    EVT_BUTTON(wxID_CANCEL, DlgCfgIdfyTonality::OnCancelClicked )
//    EVT_BUTTON(lmID_BUTTON_ALL_MAJOR, DlgCfgIdfyTonality::OnCheckAllMajor )
//    EVT_BUTTON(lmID_BUTTON_ALL_MINOR, DlgCfgIdfyTonality::OnCheckAllMinor )
//    EVT_RADIOBOX(lmID_RAD_ANSWER_BUTTONS, DlgCfgIdfyTonality::OnDataChanged )
//    EVT_CHECKBOX(lmID_CHECK_KEY, DlgCfgIdfyTonality::OnDataChanged )
//END_EVENT_TABLE()
//
//
//DlgCfgIdfyTonality::DlgCfgIdfyTonality(wxWindow* parent,
//                                           TonalityConstrains* pConstrains)
//    : wxDialog(parent, wxID_ANY, _("Identify tonality settings"),
//               wxDefaultPosition, wxDefaultSize,
//               wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX )
//    , m_pConstrains(pConstrains)
//{
//    // create the dialog controls
//    CreateControls();
//
//    // Copy key signature pointers to use an array
//    m_pChkKeySign[k_key_C] = m_pChkKeyC;
//    m_pChkKeySign[earmSol] = m_pChkKeyG;
//    m_pChkKeySign[earmRe] = m_pChkKeyD;
//    m_pChkKeySign[earmLa] = m_pChkKeyA;
//    m_pChkKeySign[earmMi] = m_pChkKeyE;
//    m_pChkKeySign[earmSi] = m_pChkKeyB;
//    m_pChkKeySign[earmFas] = m_pChkKeyFSharp;
//    m_pChkKeySign[earmDos] = m_pChkKeyCSharp;
//    m_pChkKeySign[earmDob] = m_pChkKeyCFlat;
//    m_pChkKeySign[earmSolb] = m_pChkKeyGFlat;
//    m_pChkKeySign[earmReb] = m_pChkKeyDFlat;
//    m_pChkKeySign[earmLab] = m_pChkKeyAFlat;
//    m_pChkKeySign[earmMib] = m_pChkKeyEFlat;
//    m_pChkKeySign[earmSib] = m_pChkKeyBFlat;
//    m_pChkKeySign[k_key_F] = m_pChkKeyF;
//        // Minor keys
//    m_pChkKeySign[earmLam] = m_pChkKeyAMinor;
//    m_pChkKeySign[earmMim] = m_pChkKeyEMinor;
//    m_pChkKeySign[earmSim] = m_pChkKeyBMinor;
//    m_pChkKeySign[earmFasm] = m_pChkKeyFSharpMinor;
//    m_pChkKeySign[earmDosm] = m_pChkKeyCSharpMinor;
//    m_pChkKeySign[earmSolsm] = m_pChkKeyGSharpMinor;
//    m_pChkKeySign[earmResm] = m_pChkKeyDSharpMinor;
//    m_pChkKeySign[earmLasm] = m_pChkKeyASharpMinor;
//    m_pChkKeySign[earmLabm] = m_pChkKeyAFlatMinor;
//    m_pChkKeySign[earmMibm] = m_pChkKeyEFlatMinor;
//    m_pChkKeySign[earmSibm] = m_pChkKeyBFlatMinor;
//    m_pChkKeySign[earmFam] = m_pChkKeyFMinor;
//    m_pChkKeySign[earmDom] = m_pChkKeyCMinor;
//    m_pChkKeySign[earmSolm] = m_pChkKeyGMinor;
//    m_pChkKeySign[earmRem] = m_pChkKeyDMinor;
//
//    //set error icons
//    wxBitmap bmpError =
//         wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, wxSize(16,16));
//    m_pBmpKeySignError->SetBitmap(bmpError);
//    m_pBmpGlobalError->SetBitmap(bmpError);
//
//    //hide all error messages and their associated icons
//    m_pLblKeySignError->Show(false);
//    m_pBmpKeySignError->Show(false);
//    m_pLblGlobalError->Show(false);
//    m_pBmpGlobalError->Show(false);
//
//        //
//        // initialize all controls with current constraints data
//        //
//
//    // allowed key signatures
//    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
//    for (int i=0; i <= k_max_key; i++)
//        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignature)i) );
//
//    // allowed answer buttons and cadence groups
//    m_pRadAnswerType->SetSelection( (pConstrains->UseMajorMinorButtons() ? 0 : 1) );
//}
//
//DlgCfgIdfyTonality::~DlgCfgIdfyTonality()
//{
//}
//
//void DlgCfgIdfyTonality::CreateControls()
//{
//    //Copy as generated by wxFormBuilder. No changes.
//
//	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
//	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
//
//	wxBoxSizer* m_pMainSizer;
//	m_pMainSizer = new wxBoxSizer( wxVERTICAL );
//
//	m_pNoteBook = new wxNotebook( this, lmID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_DEFAULT|wxNB_TOP );
//	m_pPanelKeySignatures = new wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
//	wxBoxSizer* m_pPanelKSSizer;
//	m_pPanelKSSizer = new wxBoxSizer( wxVERTICAL );
//
//	wxStaticBoxSizer* m_pKeySignaturesSizer;
//	m_pKeySignaturesSizer = new wxStaticBoxSizer( new wxStaticBox( m_pPanelKeySignatures, wxID_ANY, _("Key signatures") ), wxVERTICAL );
//
//	wxBoxSizer* m_pKSColumnsSizer;
//	m_pKSColumnsSizer = new wxBoxSizer( wxHORIZONTAL );
//
//	wxBoxSizer* m_pColumnSizer1;
//	m_pColumnSizer1 = new wxBoxSizer( wxVERTICAL );
//
//	m_pChkKeyC = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("C Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer1->Add( m_pChkKeyC, 0, wxALIGN_LEFT|wxALL, 5 );
//
//	m_pChkKeyG = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("G Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer1->Add( m_pChkKeyG, 0, wxALIGN_LEFT|wxALL, 5 );
//
//	m_pChkKeyD = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("D Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer1->Add( m_pChkKeyD, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyA = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("A Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer1->Add( m_pChkKeyA, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyE = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("E Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer1->Add( m_pChkKeyE, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyB = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("B Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer1->Add( m_pChkKeyB, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyFSharp = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("F# Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer1->Add( m_pChkKeyFSharp, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyCSharp = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("C# Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer1->Add( m_pChkKeyCSharp, 0, wxEXPAND|wxALL, 5 );
//
//	m_pKSColumnsSizer->Add( m_pColumnSizer1, 1, wxALIGN_TOP|wxALL, 5 );
//
//	wxBoxSizer* m_pColumnSizer2;
//	m_pColumnSizer2 = new wxBoxSizer( wxVERTICAL );
//
//	m_pChkKeyCFlat = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Cb Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer2->Add( m_pChkKeyCFlat, 0, wxALIGN_LEFT|wxALL, 5 );
//
//	m_pChkKeyGFlat = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Gb Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer2->Add( m_pChkKeyGFlat, 0, wxALIGN_LEFT|wxALL, 5 );
//
//	m_pChkKeyDFlat = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Db Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer2->Add( m_pChkKeyDFlat, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyAFlat = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Ab Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer2->Add( m_pChkKeyAFlat, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyEFlat = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Eb Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer2->Add( m_pChkKeyEFlat, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyBFlat = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Bb Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer2->Add( m_pChkKeyBFlat, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyF = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("F Major"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer2->Add( m_pChkKeyF, 0, wxEXPAND|wxALL, 5 );
//
//	m_pKSColumnsSizer->Add( m_pColumnSizer2, 1, wxALIGN_TOP|wxALL, 5 );
//
//	wxBoxSizer* m_pColumnSizer3;
//	m_pColumnSizer3 = new wxBoxSizer( wxVERTICAL );
//
//	m_pChkKeyAMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("A minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer3->Add( m_pChkKeyAMinor, 0, wxALIGN_LEFT|wxALL, 5 );
//
//	m_pChkKeyEMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("E minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer3->Add( m_pChkKeyEMinor, 0, wxALIGN_LEFT|wxALL, 5 );
//
//	m_pChkKeyBMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("B minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer3->Add( m_pChkKeyBMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyFSharpMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("F# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer3->Add( m_pChkKeyFSharpMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyCSharpMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("C# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer3->Add( m_pChkKeyCSharpMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyGSharpMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("G# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer3->Add( m_pChkKeyGSharpMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyDSharpMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("D# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer3->Add( m_pChkKeyDSharpMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyASharpMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("A# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer3->Add( m_pChkKeyASharpMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pKSColumnsSizer->Add( m_pColumnSizer3, 1, wxALIGN_TOP|wxALL, 5 );
//
//	wxBoxSizer* m_pColumnSizer4;
//	m_pColumnSizer4 = new wxBoxSizer( wxVERTICAL );
//
//	m_pChkKeyAFlatMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Ab minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer4->Add( m_pChkKeyAFlatMinor, 0, wxALIGN_LEFT|wxALL, 5 );
//
//	m_pChkKeyEFlatMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Eb minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer4->Add( m_pChkKeyEFlatMinor, 0, wxALIGN_LEFT|wxALL, 5 );
//
//	m_pChkKeyBFlatMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("Bb minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer4->Add( m_pChkKeyBFlatMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyFMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("F minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer4->Add( m_pChkKeyFMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyCMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("C minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer4->Add( m_pChkKeyCMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyGMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("G minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer4->Add( m_pChkKeyGMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pChkKeyDMinor = new wxCheckBox( m_pPanelKeySignatures, lmID_CHECK_KEY, _("D minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
//
//	m_pColumnSizer4->Add( m_pChkKeyDMinor, 0, wxEXPAND|wxALL, 5 );
//
//	m_pKSColumnsSizer->Add( m_pColumnSizer4, 1, wxALIGN_TOP|wxALL, 5 );
//
//	m_pKeySignaturesSizer->Add( m_pKSColumnsSizer, 0, wxEXPAND|wxALL, 5 );
//
//	wxBoxSizer* m_pSelectButtonsSizer;
//	m_pSelectButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
//
//	m_pBtCheckAllMajor = new wxButton( m_pPanelKeySignatures, lmID_BUTTON_ALL_MAJOR, _("check/uncheck all major"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pBtCheckAllMajor->SetDefault();
//	m_pSelectButtonsSizer->Add( m_pBtCheckAllMajor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
//
//	m_pBtCheckAllMinor = new wxButton( m_pPanelKeySignatures, lmID_BUTTON_ALL_MINOR, _("check/uncheck all minor"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pBtCheckAllMinor->SetDefault();
//	m_pSelectButtonsSizer->Add( m_pBtCheckAllMinor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
//
//	m_pKeySignaturesSizer->Add( m_pSelectButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
//
//	wxBoxSizer* m_pKSErrorSizer;
//	m_pKSErrorSizer = new wxBoxSizer( wxHORIZONTAL );
//
//	m_pBmpKeySignError = new wxStaticBitmap( m_pPanelKeySignatures, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
//	m_pKSErrorSizer->Add( m_pBmpKeySignError, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );
//
//	m_pKSErrorSpaces = new wxStaticText( m_pPanelKeySignatures, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
//	m_pKSErrorSpaces->Wrap( -1 );
//	m_pKSErrorSizer->Add( m_pKSErrorSpaces, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5 );
//
//	m_pLblKeySignError = new wxStaticText( m_pPanelKeySignatures, wxID_ANY, _("You must choose at least one key signature! "), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pLblKeySignError->Wrap( -1 );
//	m_pLblKeySignError->SetBackgroundColour( wxColour( 255, 215, 215 ) );
//
//	m_pKSErrorSizer->Add( m_pLblKeySignError, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5 );
//
//	m_pKeySignaturesSizer->Add( m_pKSErrorSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
//
//	m_pPanelKSSizer->Add( m_pKeySignaturesSizer, 0, wxEXPAND|wxALL, 5 );
//
//	m_pPanelKeySignatures->SetSizer( m_pPanelKSSizer );
//	m_pPanelKeySignatures->Layout();
//	m_pPanelKSSizer->Fit( m_pPanelKeySignatures );
//	m_pNoteBook->AddPage( m_pPanelKeySignatures, _("Key signatures"), true );
//	m_pPanelOther = new wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
//	wxBoxSizer* m_pOtherSizer;
//	m_pOtherSizer = new wxBoxSizer( wxVERTICAL );
//
//	wxString m_pRadAnswerTypeChoices[] = { _("Just identify type: major or minor"), _("Name the key signature") };
//	int m_pRadAnswerTypeNChoices = sizeof( m_pRadAnswerTypeChoices ) / sizeof( wxString );
//	m_pRadAnswerType = new wxRadioBox( m_pPanelOther, lmID_RAD_ANSWER_BUTTONS, _("Type of desired answer"), wxDefaultPosition, wxDefaultSize, m_pRadAnswerTypeNChoices, m_pRadAnswerTypeChoices, 2, wxRA_SPECIFY_ROWS );
//	m_pRadAnswerType->SetSelection( 1 );
//	m_pOtherSizer->Add( m_pRadAnswerType, 0, wxEXPAND|wxALL, 5 );
//
//	m_pPanelOther->SetSizer( m_pOtherSizer );
//	m_pPanelOther->Layout();
//	m_pOtherSizer->Fit( m_pPanelOther );
//	m_pNoteBook->AddPage( m_pPanelOther, _("Answer buttons"), false );
//
//	m_pMainSizer->Add( m_pNoteBook, 0, wxEXPAND|wxALL, 5 );
//
//	wxBoxSizer* m_pErrorsSizer;
//	m_pErrorsSizer = new wxBoxSizer( wxHORIZONTAL );
//
//	m_pBmpGlobalError = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
//	m_pErrorsSizer->Add( m_pBmpGlobalError, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );
//
//	m_pSpace1 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
//	m_pSpace1->Wrap( -1 );
//	m_pErrorsSizer->Add( m_pSpace1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5 );
//
//	m_pLblGlobalError = new wxStaticText( this, wxID_ANY, _("You must choose at least one major and one minor in tab 'Key signatures'! "), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pLblGlobalError->Wrap( -1 );
//	m_pLblGlobalError->SetBackgroundColour( wxColour( 255, 215, 215 ) );
//
//	m_pErrorsSizer->Add( m_pLblGlobalError, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5 );
//
//	m_pMainSizer->Add( m_pErrorsSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
//
//	wxBoxSizer* m_pButtonsSizer;
//	m_pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
//
//	m_pBtAccept = new wxButton( this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pBtAccept->SetDefault();
//	m_pButtonsSizer->Add( m_pBtAccept, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
//
//	m_pBtCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_pBtCancel->SetDefault();
//	m_pButtonsSizer->Add( m_pBtCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
//
//	m_pMainSizer->Add( m_pButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
//
//	this->SetSizer( m_pMainSizer );
//	this->Layout();
//	m_pMainSizer->Fit( this );
//}
//
//void DlgCfgIdfyTonality::OnDataChanged(wxCommandEvent& WXUNUSED(event))
//{
//    VerifyData();
//}
//
//void DlgCfgIdfyTonality::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
//{
//    // Accept button will be enabled only if all data have been validated and is Ok. So
//    // when accept button is clicked we can proceed to save data.
//
//    // save selected key signatures
//    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
//    for (int i=0; i <= k_max_key; i++) {
//        pKeyConstrains->SetValid((EKeySignature)i, m_pChkKeySign[i]->GetValue());
//    }
//
//    // save answer type
//	m_pConstrains->UseMajorMinorButtons( m_pRadAnswerType->GetSelection() == 0 );
//
//    //terminate the dialog
//    EndModal(wxID_OK);
//}
//
//bool DlgCfgIdfyTonality::VerifyData()
//{
//    // Returns a boolean to enable or not a tab change. That is: returns true if there are
//    // local errors (errors affecting only to the data in a tab) so not to enable a tab
//    // change. If there are no tab local errors then returns false (althought it there might
//    // be global errors -- coherence between data in different tabs --).
//    //
//    // Anyway, global errors al also checked. If there are no global neither local
//    // errors the Accept button is enabled. Otherwise it is disabled.
//
//    //assume no errors
//    bool fError = false;
//    bool fLocalError = false;
//    bool fGlobalError = false;
//
//    m_pLblKeySignError->Show(false);
//    m_pBmpKeySignError->Show(false);
//    m_pLblGlobalError->Show(false);
//    m_pBmpGlobalError->Show(false);
//
//    // check that at least one key signature has been choosen
//    bool fAtLeastOneMajor = false;
//    for (int i=lmMIN_MAJOR_KEY; i <= lmMAX_MAJOR_KEY; i++) {
//        fAtLeastOneMajor |= m_pChkKeySign[i]->GetValue();
//    }
//    bool fAtLeastOneMinor = false;
//    for (int i=lmMIN_MINOR_KEY; i <= lmMAX_MINOR_KEY; i++) {
//        fAtLeastOneMinor |= m_pChkKeySign[i]->GetValue();
//    }
//    fError = !(fAtLeastOneMajor || fAtLeastOneMinor);
//    if (fError) {
//        m_pLblKeySignError->Show(true);
//        m_pBmpKeySignError->Show(true);
//    }
//    fLocalError |= fError;
//
//        //
//        // Check for global errors
//        //
//
//    fGlobalError = fLocalError;
//
//    //if use only major/minor answer buttons check that at least one major and
//    //one minor are selected
//	if (!fGlobalError && m_pRadAnswerType->GetSelection() == 0)
//	{
//        fGlobalError = !(fAtLeastOneMajor && fAtLeastOneMinor);
//        if (fGlobalError) {
//            m_pLblGlobalError->Show(true);
//            m_pBmpGlobalError->Show(true);
//        }
//    }
//
//    //enable / disable accept button
//    m_pBtAccept->Enable(!fGlobalError);
//
//    return fGlobalError;
//}
//
//void DlgCfgIdfyTonality::OnCheckAllMajor(wxCommandEvent& WXUNUSED(event))
//{
//    bool fCheck = !m_pChkKeySign[k_key_C]->GetValue();
//    for (int i=0; i <= k_key_F; i++) {
//        m_pChkKeySign[i]->SetValue(fCheck);
//    }
//    VerifyData();
//}
//
//void DlgCfgIdfyTonality::OnCheckAllMinor(wxCommandEvent& WXUNUSED(event))
//{
//    bool fCheck = !m_pChkKeySign[earmLam]->GetValue();
//    for (int i=earmLam; i <= k_max_key; i++) {
//        m_pChkKeySign[i]->SetValue(fCheck);
//    }
//    VerifyData();
//}
//
