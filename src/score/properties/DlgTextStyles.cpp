//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DlgTextStyles.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "DlgTextStyles.h"
#include "../../app/ArtProvider.h"


//--------------------------------------------------------------------------------------
/// Implementation of lmDlgTextStyles
//--------------------------------------------------------------------------------------

//const int lmEDIT_CUT = wxNewId();
//const int lmEDIT_COPY = wxNewId();
//const int lmEDIT_PASTE = wxNewId();
//const int lmEDIT_BOLD = wxNewId();
//const int lmEDIT_ITALIC = wxNewId();
//const int lmEDIT_UNDERLINED = wxNewId();
//const int lmEDIT_FONT_NAME = wxNewId();
//const int lmEDIT_FONT_SIZE = wxNewId();


//BEGIN_EVENT_TABLE(lmDlgTextStyles, wxDialog)
//    EVT_MENU(lmEDIT_BOLD, lmDlgTextStyles::OnBold)
//    EVT_MENU(lmEDIT_ITALIC, lmDlgTextStyles::OnItalic)
//    EVT_MENU(lmEDIT_UNDERLINED, lmDlgTextStyles::OnUnderline)
//
//    EVT_BUTTON(wxID_OK, lmDlgTextStyles::OnAccept)
//    EVT_BUTTON(wxID_CANCEL, lmDlgTextStyles::OnCancel)
//END_EVENT_TABLE()
//
enum {
    lmSTYLE = 0,
    lmFONTNAME,
    lmFONTSIZE,
    lmBOLD,
    lmITALIC,
    lmCOLOR,
};

lmDlgTextStyles::lmDlgTextStyles( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    CreateControls();

    //set column sizes
    m_pGrid->SetColSize(lmSTYLE, 220);
    m_pGrid->SetColSize(lmFONTNAME, 150);
    //m_pGrid->SetColSize(lmFONTSIZE, 60);
    //m_pGrid->SetColSize(lmBOLD, 60);
    //m_pGrid->SetColSize(lmITALIC, 60);
    //m_pGrid->SetColSize(lmCOLOR, 60);
    m_pGrid->AutoSizeColumn(lmFONTSIZE);
    m_pGrid->AutoSizeColumn(lmBOLD);
    m_pGrid->AutoSizeColumn(lmITALIC);
    m_pGrid->AutoSizeColumn(lmCOLOR);

    //set all cells as read-only, and center some columns
    for (int iRow=0; iRow < m_pGrid->GetNumberRows(); iRow++)
    {
        for (int iCol=0; iCol < m_pGrid->GetNumberCols(); iCol++)
        {
            m_pGrid->SetReadOnly(iRow, iCol);
        }
        m_pGrid->SetCellAlignment(wxALIGN_CENTRE, iRow, lmBOLD);
        m_pGrid->SetCellAlignment(wxALIGN_CENTRE, iRow, lmITALIC);
    }

    //set format bool for columns Bold and Italic
    m_pGrid->SetColFormatBool(lmBOLD);
    m_pGrid->SetColFormatBool(lmITALIC);

    //set font for styles
    int w,h = 0;
    wxClientDC dc(this);
    wxFont font = m_pGrid->GetFont();
    for (int iRow=0; iRow < m_pGrid->GetNumberRows(); iRow++)
    {
        int nPoints = 10+2*iRow;
        font.SetPointSize(nPoints);
        dc.SetFont(font);
        dc.GetTextExtent(_T("This is a sample text qjgltTQYI"),&w,&h);
        m_pGrid->SetCellFont(iRow, lmSTYLE, font);
        m_pGrid->SetCellTextColour(iRow, lmSTYLE, *wxRED);
        m_pGrid->SetCellValue(iRow, lmFONTSIZE, wxString::Format(_T("%d pt (%d px)"), nPoints, h));
        m_pGrid->SetRowSize(iRow, h + 8);
    }
    m_pGrid->ForceRefresh();

    //load values
    for (int iRow=0; iRow < m_pGrid->GetNumberRows(); iRow++)
    {
        m_pGrid->SetCellValue(iRow, lmSTYLE, _T("wxGrid is good" ));
        m_pGrid->SetCellValue(iRow, lmFONTNAME, _T("Times New Roman"));
        m_pGrid->SetCellValue(iRow, lmBOLD, (iRow % 2 ? _T("1") : _T("0")));
        m_pGrid->SetCellValue(iRow, lmITALIC, (iRow % 2 ? _T("0") : _T("1")));
    }

    //set colors
    for (int iRow=0; iRow < m_pGrid->GetNumberRows(); iRow++)
    {
        m_pGrid->SetCellBackgroundColour(iRow, lmCOLOR, *wxRED);
    }

}

void lmDlgTextStyles::CreateControls()
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Defined text styles"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_pGrid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_pGrid->CreateGrid( 5, 6 );
	m_pGrid->EnableEditing( false );
	m_pGrid->EnableGridLines( true );
	m_pGrid->EnableDragGridSize( false );
	m_pGrid->SetMargins( 0, 0 );
	
	// Columns
	m_pGrid->EnableDragColMove( false );
	m_pGrid->EnableDragColSize( true );
	m_pGrid->SetColLabelSize( 30 );
	m_pGrid->SetColLabelValue( 0, wxT("Text style") );
	m_pGrid->SetColLabelValue( 1, wxT("Font name") );
	m_pGrid->SetColLabelValue( 2, wxT("Font size (pt)") );
	m_pGrid->SetColLabelValue( 3, wxT("Bold") );
	m_pGrid->SetColLabelValue( 4, wxT("Italic") );
	m_pGrid->SetColLabelValue( 5, wxT("colour") );
	m_pGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_pGrid->EnableDragRowSize( true );
	m_pGrid->SetRowLabelSize( 30 );
	m_pGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_pGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer2->Add( m_pGrid, 1, wxALL|wxEXPAND, 5 );
	
	bSizer11->Add( bSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_radioBox1Choices[] = { wxT("Show all defined text styles"), wxT("Show not used text styles"), wxT("Show styles using fonts not available") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox1->SetSelection( 2 );
	bSizer10->Add( m_radioBox1, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_button11 = new wxButton( this, wxID_ANY, wxT("Add new"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_button11, 0, wxALL|wxEXPAND, 5 );
	
	m_button21 = new wxButton( this, wxID_ANY, wxT("Remove selected"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_button21, 0, wxALL|wxEXPAND, 5 );
	
	m_button211 = new wxButton( this, wxID_ANY, wxT("Add default set"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_button211, 0, wxALL|wxEXPAND, 5 );
	
	m_button6 = new wxButton( this, wxID_ANY, wxT("Edit selected"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_button6, 0, wxALL|wxEXPAND, 5 );
	
	bSizer10->Add( gSizer1, 1, 0, 5 );
	
	bSizer11->Add( bSizer10, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer11, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button3 = new wxButton( this, wxID_OK, wxT("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button3, 0, wxALL, 5 );
	
	
	bSizer12->Add( 0, 0, 2, wxEXPAND, 5 );
	
	m_button4 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button4, 0, wxALL, 5 );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer12, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

lmDlgTextStyles::~lmDlgTextStyles()
{
}

