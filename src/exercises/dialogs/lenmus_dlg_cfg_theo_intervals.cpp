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

//lenmus
#include "lenmus_dlg_cfg_theo_intervals.h"

#include "lenmus_art_provider.h"
#include "lenmus_constrains.h"

//lomse
#include <lomse_internal_model.h>

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/string.h>
#include <wx/radiobox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/dialog.h>

////I have to include these here. Otherwise GCC complains
//#include <wx/artprov.h>
//#include <wx/notebook.h>
//
//
//// access to paths
//#include "../../globals/Paths.h"
//extern Paths* g_pPaths;
//
//
//
//#include "DlgCfgTheoIntervals.h"
//#include "../../ldp_parser/AuxString.h"
//#include "../../auxmusic/Conversion.h"
//
//

namespace lenmus
{
//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
enum
{
	lmID_CLEF = 2100,
	lmID_KEY,
	lmID_OTHER,
	lmID_LEVEL,
	lmID_LEDGER_ABOVE,
	lmID_LEDGER_BELOW,
	lmID_INTVAL_TYPE,
	lmID_ACCEPT,
	lmID_CANCEL,
};


BEGIN_EVENT_TABLE(DlgCfgTheoIntervals, wxDialog)
    EVT_BUTTON(lmID_ACCEPT, DlgCfgTheoIntervals::OnAcceptClicked )
    EVT_BUTTON(lmID_CANCEL, DlgCfgTheoIntervals::OnCancelClicked )
    EVT_RADIOBOX(lmID_LEVEL, DlgCfgTheoIntervals::OnControlClicked )
    EVT_RADIOBOX(lmID_OTHER, DlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX(lmID_CLEF, DlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX(lmID_KEY, DlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX(lmID_INTVAL_TYPE, DlgCfgTheoIntervals::OnControlClicked )
    EVT_COMBOBOX(lmID_LEDGER_ABOVE, DlgCfgTheoIntervals::OnControlClicked )
    EVT_COMBOBOX(lmID_LEDGER_BELOW, DlgCfgTheoIntervals::OnControlClicked )

END_EVENT_TABLE()



DlgCfgTheoIntervals::DlgCfgTheoIntervals(wxWindow * parent,
                                             TheoIntervalsConstrains* pConstrains)
    : wxDialog(parent, wxID_ANY, _("Options: Intervals - Theory exercises"),
               wxDefaultPosition, wxSize( -1,-1 ),
               wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX )
{
    // save received data
    m_pConstrains = pConstrains;

    // create the dialog controls
    CreateControls();

    //set error icons
    wxBitmap bmpError = wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR,
                                                 wxSize(16,16));
    m_pBmpClefError->SetBitmap(bmpError);
    m_pBmpKeySignError->SetBitmap(bmpError);
    m_pBmpIntvalTypeError->SetBitmap(bmpError);

        //
        // initialize all controls with current constraints data
        //

    // selected clefs
    for (int i=0; i < 7; i++) {
        m_pChkClef[i]->SetValue( m_pConstrains->IsValidClef((lmEClefType)((int)lmMIN_CLEF+i) ));
    }

    // selected key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (int i=0; i < k_key_F+1; i++) {
        m_pChkKeySign[i]->SetValue( pKeyConstrains->IsValid((EKeySignature)i) );
    }

    //allowed interval types
    for (int i=0; i < 3; i++) {
        m_pChkIntvalType[i]->SetValue( m_pConstrains->IsTypeAllowed(i) );
    }

    //ledger lines
    m_pCboAboveLines->Clear();
    m_pCboBelowLines->Clear();
    for (int i=0; i < 5; i++) {
        m_pCboAboveLines->Append( wxString::Format(_T("%d"), i) );
        m_pCboBelowLines->Append( wxString::Format(_T("%d"), i) );
    }
    m_pCboAboveLines->SetSelection( m_pConstrains->GetLedgerLinesAbove() );
    m_pCboBelowLines->SetSelection( m_pConstrains->GetLedgerLinesBelow() );

    // problem level
    m_pRadLevel->SetSelection( (int)m_pConstrains->GetProblemLevel() );


    //Hide controls not applicable for current exercise mode.
    //This dialog is shared by identify intval. and build intval. exercises.
    if (m_pConstrains->GetProblemType() == TheoIntervalsConstrains::k_build_interval)
    {
        //build interval mode
        m_pBoxIntvalTypes->Show(false);
    }
    else
    {
        //indentify interval mode
        m_pBoxIntvalTypes->Show(true);
    }

    //hide all error messages and their associated icons
    m_pLblClefError->Show(false);
    m_pBmpClefError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblKeySignError->Show(false);
    m_pLblIntvalTypeError->Show(false);
    m_pBmpIntvalTypeError->Show(false);

    //center dialog on screen
    CentreOnScreen();
}

DlgCfgTheoIntervals::~DlgCfgTheoIntervals()
{
}

void DlgCfgTheoIntervals::CreateControls()
{
    // ATTENTION: Following code has been generated with wxFormBuider.
    // Later modifications:
    // m_pBoxIntvalTypes definition moved to header (line 412 appox.)

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	noteBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP );
	m_pPanelClefs = new wxPanel( noteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer53;
	bSizer53 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* wxID_ANY3;
	wxID_ANY3 = new wxStaticBoxSizer( new wxStaticBox( m_pPanelClefs, wxID_ANY, _("Clefs to use") ), wxVERTICAL );

	m_pChkClef[lmE_Sol] = new wxCheckBox( m_pPanelClefs, lmID_CLEF, _("G clef (violin)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	wxID_ANY3->Add( m_pChkClef[lmE_Sol], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkClef[lmE_Fa4] = new wxCheckBox( m_pPanelClefs, lmID_CLEF, _("F clef on 4th line (bass)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	wxID_ANY3->Add( m_pChkClef[lmE_Fa4], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkClef[lmE_Fa3] = new wxCheckBox( m_pPanelClefs, lmID_CLEF, _("F clef on 3rd line"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	wxID_ANY3->Add( m_pChkClef[lmE_Fa3], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkClef[lmE_Do1] = new wxCheckBox( m_pPanelClefs, lmID_CLEF, _("C clef on 1st line (soprano)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	wxID_ANY3->Add( m_pChkClef[lmE_Do1], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkClef[lmE_Do2] = new wxCheckBox( m_pPanelClefs, lmID_CLEF, _("C clef on 2nd line (mezzo soprano)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	wxID_ANY3->Add( m_pChkClef[lmE_Do2], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkClef[lmE_Do3] = new wxCheckBox( m_pPanelClefs, lmID_CLEF, _("C clef on 3rd line (contralto)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	wxID_ANY3->Add( m_pChkClef[lmE_Do3], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkClef[lmE_Do4] = new wxCheckBox( m_pPanelClefs, lmID_CLEF, _("C clef on 4th line (tenor)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	wxID_ANY3->Add( m_pChkClef[lmE_Do4], 0, wxALIGN_LEFT|wxALL, 5 );

	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );

	m_pBmpClefError = new wxStaticBitmap( m_pPanelClefs, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer36->Add( m_pBmpClefError, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	m_pLblClefError = new wxStaticText( m_pPanelClefs, wxID_ANY, _("You must choose one at least! "), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblClefError->Wrap( -1 );
	m_pLblClefError->SetBackgroundColour( wxColour( 255, 215, 215 ) );

	bSizer36->Add( m_pLblClefError, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5 );

	m_pSpace5 = new wxStaticText( m_pPanelClefs, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pSpace5->Wrap( -1 );
	bSizer36->Add( m_pSpace5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5 );

	wxID_ANY3->Add( bSizer36, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	bSizer53->Add( wxID_ANY3, 1, wxEXPAND|wxALL, 5 );

	m_pPanelClefs->SetSizer( bSizer53 );
	m_pPanelClefs->Layout();
	bSizer53->Fit( m_pPanelClefs );
	noteBook->AddPage( m_pPanelClefs, _("Clefs"), false );
	m_pPanelKeys = new wxPanel( noteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer55;
	bSizer55 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* wxID_ANY1;
	wxID_ANY1 = new wxStaticBoxSizer( new wxStaticBox( m_pPanelKeys, wxID_ANY, _("Key signatures") ), wxVERTICAL );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	m_pChkKeySign[k_key_C] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("C Major / A minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer7->Add( m_pChkKeySign[k_key_C], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeySign[k_key_G] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("G Major / E minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer7->Add( m_pChkKeySign[k_key_G], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeySign[k_key_D] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("D Major / B minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer7->Add( m_pChkKeySign[k_key_D], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_A] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("A Major / F# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer7->Add( m_pChkKeySign[k_key_A], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_E] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("E Major / C# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer7->Add( m_pChkKeySign[k_key_E], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_B] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("B Major / G# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer7->Add( m_pChkKeySign[k_key_B], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_Fs] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("F# Major / D# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer7->Add( m_pChkKeySign[k_key_Fs], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_Cs] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("C# Major / A# minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer7->Add( m_pChkKeySign[k_key_Cs], 0, wxEXPAND|wxALL, 5 );

	bSizer6->Add( bSizer7, 1, wxALIGN_TOP|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_pChkKeySign[k_key_Cf] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("Cb Major / Ab minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer8->Add( m_pChkKeySign[k_key_Cf], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeySign[k_key_Gf] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("Gb Major / Eb minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer8->Add( m_pChkKeySign[k_key_Gf], 0, wxALIGN_LEFT|wxALL, 5 );

	m_pChkKeySign[k_key_Df] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("Db Major / Bb minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer8->Add( m_pChkKeySign[k_key_Df], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_Af] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("Ab Major / F minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer8->Add( m_pChkKeySign[k_key_Af], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_Ef] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("Eb Major / C minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer8->Add( m_pChkKeySign[k_key_Ef], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_Bf] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("Bb Major / G minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer8->Add( m_pChkKeySign[k_key_Bf], 0, wxEXPAND|wxALL, 5 );

	m_pChkKeySign[k_key_F] = new wxCheckBox( m_pPanelKeys, lmID_KEY, _("F Major / D minor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	bSizer8->Add( m_pChkKeySign[k_key_F], 0, wxEXPAND|wxALL, 5 );

	bSizer6->Add( bSizer8, 1, wxALIGN_TOP|wxALL, 5 );

	wxID_ANY1->Add( bSizer6, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_pBmpKeySignError = new wxStaticBitmap( m_pPanelKeys, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_pBmpKeySignError, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	wxID_STATIC1 = new wxStaticText( m_pPanelKeys, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	wxID_STATIC1->Wrap( -1 );
	bSizer9->Add( wxID_STATIC1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5 );

	m_pLblKeySignError = new wxStaticText( m_pPanelKeys, wxID_ANY, _("You must choose one at least! "), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblKeySignError->Wrap( -1 );
	m_pLblKeySignError->SetBackgroundColour( wxColour( 255, 215, 215 ) );

	bSizer9->Add( m_pLblKeySignError, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5 );

	wxID_ANY1->Add( bSizer9, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	bSizer55->Add( wxID_ANY1, 1, wxEXPAND|wxALL, 5 );

	m_pPanelKeys->SetSizer( bSizer55 );
	m_pPanelKeys->Layout();
	bSizer55->Fit( m_pPanelKeys );
	noteBook->AddPage( m_pPanelKeys, _("Key signatures"), false );
	m_pPanelOther = new wxPanel( noteBook, lmID_OTHER, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );

	wxString m_pRadLevelChoices[] = { _("Just name the interval number"), _("Perfect, major and minor intervals"), _("Also augmented and diminished"), _("Also double augmented / diminished") };
	int m_pRadLevelNChoices = sizeof( m_pRadLevelChoices ) / sizeof( wxString );
	m_pRadLevel = new wxRadioBox( m_pPanelOther, lmID_LEVEL, _("Difficulty"), wxDefaultPosition, wxDefaultSize, m_pRadLevelNChoices, m_pRadLevelChoices, 1, wxRA_SPECIFY_COLS );
	m_pRadLevel->SetSelection( 0 );
	bSizer19->Add( m_pRadLevel, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( m_pPanelOther, wxID_ANY, _("Maximum number of leger lines") ), wxVERTICAL );

	wxBoxSizer* bSizer121;
	bSizer121 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxHORIZONTAL );

	wxID_STATIC2 = new wxStaticText( m_pPanelOther, wxID_ANY, _("Above"), wxDefaultPosition, wxDefaultSize, 0 );
	wxID_STATIC2->Wrap( -1 );
	bSizer131->Add( wxID_STATIC2, 0, wxALIGN_LEFT|wxADJUST_MINSIZE|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxArrayString m_pCboAboveLinesChoices;
	m_pCboAboveLines = new wxChoice( m_pPanelOther, lmID_LEDGER_ABOVE, wxDefaultPosition, wxSize( 60,-1 ), m_pCboAboveLinesChoices, 0 );
	m_pCboAboveLines->SetSelection( 0 );
	bSizer131->Add( m_pCboAboveLines, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	bSizer121->Add( bSizer131, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );

	wxBoxSizer* bSizer141;
	bSizer141 = new wxBoxSizer( wxHORIZONTAL );

	wxID_STATIC3 = new wxStaticText( m_pPanelOther, wxID_ANY, _("Below"), wxDefaultPosition, wxDefaultSize, 0 );
	wxID_STATIC3->Wrap( -1 );
	bSizer141->Add( wxID_STATIC3, 0, wxALIGN_LEFT|wxADJUST_MINSIZE|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxArrayString m_pCboBelowLinesChoices;
	m_pCboBelowLines = new wxChoice( m_pPanelOther, lmID_LEDGER_BELOW, wxDefaultPosition, wxSize( 60,-1 ), m_pCboBelowLinesChoices, 0 );
	m_pCboBelowLines->SetSelection( 0 );
	bSizer141->Add( m_pCboBelowLines, 0, wxALL, 5 );

	bSizer121->Add( bSizer141, 0, wxALIGN_CENTER_VERTICAL, 5 );

	sbSizer8->Add( bSizer121, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

	bSizer19->Add( sbSizer8, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_pBoxIntvalTypes = new wxStaticBoxSizer( new wxStaticBox( m_pPanelOther, wxID_ANY, _("Intervals' types") ), wxVERTICAL );

	m_pChkIntvalType[0] = new wxCheckBox( m_pPanelOther, lmID_INTVAL_TYPE, _("Harmonic (simultaneous notes)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	m_pBoxIntvalTypes->Add( m_pChkIntvalType[0], 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_pChkIntvalType[1] = new wxCheckBox( m_pPanelOther, lmID_INTVAL_TYPE, _("Melodic (ascending)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	m_pChkIntvalType[1]->SetValue(true);

	m_pBoxIntvalTypes->Add( m_pChkIntvalType[1], 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_pChkIntvalType[2] = new wxCheckBox( m_pPanelOther, lmID_INTVAL_TYPE, _("Melodic (descending)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	m_pChkIntvalType[2]->SetValue(true);

	m_pBoxIntvalTypes->Add( m_pChkIntvalType[2], 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bSizer161;
	bSizer161 = new wxBoxSizer( wxHORIZONTAL );

	m_pBmpIntvalTypeError = new wxStaticBitmap( m_pPanelOther, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer161->Add( m_pBmpIntvalTypeError, 0, wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

	wxID_STATIC4 = new wxStaticText( m_pPanelOther, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	wxID_STATIC4->Wrap( -1 );
	bSizer161->Add( wxID_STATIC4, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE|wxTOP|wxRIGHT|wxLEFT, 5 );

	 m_pLblIntvalTypeError = new wxStaticText( m_pPanelOther, wxID_ANY, _("You must choose one at least! "), wxDefaultPosition, wxDefaultSize, 0 );
	 m_pLblIntvalTypeError->Wrap( -1 );
	m_pLblIntvalTypeError->SetBackgroundColour( wxColour( 255, 215, 215 ) );

	bSizer161->Add(  m_pLblIntvalTypeError, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5 );

	m_pBoxIntvalTypes->Add( bSizer161, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	bSizer19->Add( m_pBoxIntvalTypes, 1, wxEXPAND|wxALL, 5 );

	m_pPanelOther->SetSizer( bSizer19 );
	m_pPanelOther->Layout();
	bSizer19->Fit( m_pPanelOther );
	noteBook->AddPage( m_pPanelOther, _("Other"), true );

	bSizer1->Add( noteBook, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	m_pBtnAccept = new wxButton( this, lmID_ACCEPT, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBtnAccept->SetDefault();
	bSizer17->Add( m_pBtnAccept, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_pBtnCancel = new wxButton( this, lmID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pBtnCancel->SetDefault();
	bSizer17->Add( m_pBtnCancel, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	bSizer1->Add( bSizer17, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	this->Centre( wxBOTH );
}

void DlgCfgTheoIntervals::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    //Accept button will be enabled only if all data habe been validated and is Ok.
    //So, when accept button is clicked we can proceed to save data.

    // save allowed clefs
    for (int i=0; i < 7; i++) {
        m_pConstrains->SetClef((lmEClefType)((int)lmMIN_CLEF+i), m_pChkClef[i]->GetValue());
    }

    // save selected key signatures
    KeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
    for (int i=0; i < k_key_F+1; i++) {
        pKeyConstrains->SetValid((EKeySignature)i, m_pChkKeySign[i]->GetValue());
    }

    // save intervals' type
    for (int i=0; i < 3; i++) {
        m_pConstrains->SetTypeAllowed(i, m_pChkIntvalType[i]->GetValue());
    }

    // ledger lines
    m_pConstrains->SetLedgerLinesAbove( m_pCboAboveLines->GetSelection() );
    m_pConstrains->SetLedgerLinesBelow( m_pCboBelowLines->GetSelection() );

    // problem level
    m_pConstrains->SetProblemLevel( m_pRadLevel->GetSelection() );

    //terminate the dialog
    EndModal(wxID_OK);
}

bool DlgCfgTheoIntervals::VerifyData()
{
    //Returns true if there are errors. If there are no
    //errors the Accept button is enabled. Otherwise it is disabled.

    int i;

    //assume no errors
    bool fError = false;
    bool fGlobalError = false;
    m_pLblClefError->Show(false);
    m_pBmpClefError->Show(false);
    m_pLblKeySignError->Show(false);
    m_pBmpKeySignError->Show(false);
    m_pLblIntvalTypeError->Show(false);
    m_pBmpIntvalTypeError->Show(false);

    // check that at least one clef is allowed
    bool fAtLeastOne = false;
    for (i=0; i < 7; i++) {
        if (m_pChkClef[i]->GetValue()) {
            fAtLeastOne = true;
            break;
        }
    }
    fError = !fAtLeastOne;
    if (fError) {
        m_pLblClefError->Show(true);
        m_pBmpClefError->Show(true);
    }
    fGlobalError |= fError;

    // verify that at least one key signature has been choosen
    fAtLeastOne = false;
    for (i=0; i < k_key_F+1; i++) {
        fAtLeastOne |= m_pChkKeySign[i]->GetValue();
    }

    fError = !fAtLeastOne;
    if (fError) {
        m_pLblKeySignError->Show(true);
        m_pBmpKeySignError->Show(true);
    }
    fGlobalError |= fError;

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
    fGlobalError |= fError;

    //enable / disable accept button
    m_pBtnAccept->Enable(!fGlobalError);

    return fGlobalError;

}

void DlgCfgTheoIntervals::OnControlClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}


}   //namespace lenmus
