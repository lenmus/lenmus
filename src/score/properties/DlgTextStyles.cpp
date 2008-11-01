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
#else
    #include <wx/string.h>
    #include <wx/stattext.h>
    #include <wx/gdicmn.h>
    #include <wx/font.h>
    #include <wx/colour.h>
    #include <wx/settings.h>
    #include <wx/grid.h>
    #include <wx/sizer.h>
    #include <wx/radiobox.h>
    #include <wx/button.h>
    #include <wx/dialog.h>
#endif

#include <wx/fontdlg.h>



#include "DlgTextStyles.h"
#include "../Score.h"
#include "../../app/ArtProvider.h"


//--------------------------------------------------------------------------------------
/// Implementation of lmDlgTextStyles
//--------------------------------------------------------------------------------------


#define lmID_SHOW 2100
#define lmID_ADD_NEW 2101
#define lmID_REMOVE 2102
#define lmID_ADD_DEFAULT 2103
#define lmID_EDIT 2104


BEGIN_EVENT_TABLE(lmDlgTextStyles, wxDialog)
    EVT_BUTTON(wxID_OK, lmDlgTextStyles::OnAccept)
    EVT_BUTTON(wxID_CANCEL, lmDlgTextStyles::OnCancel)

    EVT_BUTTON(lmID_ADD_NEW, lmDlgTextStyles::OnAddNew)
    EVT_BUTTON(lmID_ADD_DEFAULT, lmDlgTextStyles::OnAddDefault)
    EVT_BUTTON(lmID_REMOVE, lmDlgTextStyles::OnRemove)
    //EVT_BUTTON(lmID_EDIT, lmDlgTextStyles::OnEdit)

    EVT_GRID_CELL_LEFT_DCLICK(lmDlgTextStyles::OnEdit)

END_EVENT_TABLE()


enum {
    lmSTYLE = 0,
    lmFONTNAME,
    lmFONTSIZE,
    lmBOLD,
    lmITALIC,
    lmCOLOR,
};

lmDlgTextStyles::lmDlgTextStyles(wxWindow* parent, lmScore* pScore)
    : wxDialog(parent, wxID_ANY, _("Styles defined in current score"),
               wxDefaultPosition, wxSize( 630,400 ), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
    m_pScore = pScore;
    CreateControls();

    //set column sizes
    m_pGrid->SetColSize(lmSTYLE, 220);
    m_pGrid->SetColSize(lmFONTNAME, 150);
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

    //load data
    LoadStyles();
}

void lmDlgTextStyles::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* pMainCtrolsSizer;
	pMainCtrolsSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* pGridSizer;
	pGridSizer = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Defined text styles"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	pGridSizer->Add( m_staticText1, 0, wxALL, 5 );

	m_pGrid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	m_pGrid->CreateGrid( 8, 6 );
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
	pGridSizer->Add( m_pGrid, 0, wxALL|wxEXPAND, 5 );

	pMainCtrolsSizer->Add( pGridSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pActionsSizer;
	pActionsSizer = new wxBoxSizer( wxHORIZONTAL );

	wxString m_pRadShowChoices[] = { wxT("Show all defined text styles"), wxT("Show not used text styles"), wxT("Show styles using fonts not available") };
	int m_pRadShowNChoices = sizeof( m_pRadShowChoices ) / sizeof( wxString );
	m_pRadShow = new wxRadioBox( this, lmID_SHOW, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_pRadShowNChoices, m_pRadShowChoices, 1, wxRA_SPECIFY_COLS );
	m_pRadShow->SetSelection( 2 );
	pActionsSizer->Add( m_pRadShow, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	wxGridSizer* m_pGrid;
	m_pGrid = new wxGridSizer( 2, 2, 0, 0 );

	m_pBtAddNew = new wxButton( this, lmID_ADD_NEW, wxT("Add new"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pGrid->Add( m_pBtAddNew, 0, wxALL|wxEXPAND, 5 );

	m_pBtRemove = new wxButton( this, lmID_REMOVE, wxT("Remove selected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pGrid->Add( m_pBtRemove, 0, wxALL|wxEXPAND, 5 );

	m_pBtAddDefault = new wxButton( this, lmID_ADD_DEFAULT, wxT("Add default set"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pGrid->Add( m_pBtAddDefault, 0, wxALL|wxEXPAND, 5 );

	m_pBtEdit = new wxButton( this, lmID_EDIT, wxT("Edit selected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pGrid->Add( m_pBtEdit, 0, wxALL|wxEXPAND, 5 );

	pActionsSizer->Add( m_pGrid, 1, 0, 5 );

	pMainCtrolsSizer->Add( pActionsSizer, 0, wxEXPAND, 5 );

	pMainSizer->Add( pMainCtrolsSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* pButtonsSizer;
	pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pBtAccept = new wxButton( this, wxID_OK, wxT("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtAccept, 0, wxALL, 5 );


	pButtonsSizer->Add( 0, 0, 2, wxEXPAND, 5 );

	m_pBtCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtCancel, 0, wxALL, 5 );


	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	pMainSizer->Add( pButtonsSizer, 0, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

lmDlgTextStyles::~lmDlgTextStyles()
{
}

void lmDlgTextStyles::LoadStyles()
{
    int width = 0, height = 0;
    wxClientDC dc(this);
    wxFont font = m_pGrid->GetFont();

    lmTextStyle* pStyle = m_pScore->GetFirstStyle();
    int iRow = 0;
    while (pStyle)
    {
        //set font
        int nPoints = pStyle->tFont.nFontSize;
        font.SetPointSize(nPoints);
        font.SetWeight(pStyle->tFont.nFontWeight);
        font.SetStyle(pStyle->tFont.nFontStyle);
        font.SetFaceName(pStyle->tFont.sFontName);
        dc.SetFont(font);

        dc.GetTextExtent(_T("This is a sample text qjgltTQYI"), &width, &height);
        m_pGrid->SetCellFont(iRow, lmSTYLE, font);
        m_pGrid->SetCellTextColour(iRow, lmSTYLE, pStyle->nColor);
        m_pGrid->SetCellValue(iRow, lmFONTSIZE, wxString::Format(_T("%d pt"), nPoints));
        m_pGrid->SetRowSize(iRow, height + 8);

        //set other columns
        m_pGrid->SetCellValue(iRow, lmSTYLE, pStyle->sName);
        m_pGrid->SetCellValue(iRow, lmFONTNAME, pStyle->tFont.sFontName);
        m_pGrid->SetCellValue(iRow, lmBOLD,
                    (pStyle->tFont.nFontWeight == wxFONTWEIGHT_BOLD ? _T("1") : _T("0")));
        m_pGrid->SetCellValue(iRow, lmITALIC,
                    (pStyle->tFont.nFontStyle == wxFONTSTYLE_ITALIC ? _T("1") : _T("0")));
        m_pGrid->SetCellBackgroundColour(iRow, lmCOLOR, pStyle->nColor);

        //get next style
        pStyle = m_pScore->GetNextStyle();
        iRow++;
   }
    m_pGrid->ForceRefresh();
}

void lmDlgTextStyles::OnAccept(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_OK);
}

void lmDlgTextStyles::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndDialog(wxID_CANCEL);
}

void lmDlgTextStyles::OnAddNew(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_("Not yet implemented"));
}

void lmDlgTextStyles::OnAddDefault(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_("Not yet implemented"));
}

void lmDlgTextStyles::OnRemove(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_("Not yet implemented"));
}

void lmDlgTextStyles::OnEdit(wxGridEvent& event)
{
    int iCol = event.GetCol();
    int iRow = event.GetRow();

    wxFont font = m_pGrid->GetCellFont(iRow, lmSTYLE);
    wxColor color = m_pGrid->GetCellTextColour(iRow, lmSTYLE);

    wxFontData data;
    data.SetInitialFont(font);
    data.SetColour(color);

    wxFontDialog dialog(this, data);
    if (dialog.ShowModal() != wxID_OK)
        return;

    //get new font/color data
    wxFontData newData = dialog.GetFontData();
    wxFont newFont = newData.GetChosenFont();

    int width = 0, height = 0;
    wxClientDC dc(this);

    lmTextStyle* pStyle = m_pScore->GetStyleInfo( m_pGrid->GetCellValue(iRow, lmSTYLE) );
    if (pStyle)
    {
        //save new font data
        pStyle->tFont.nFontSize = newFont.GetPointSize();
        pStyle->tFont.nFontStyle = newFont.GetStyle();
        pStyle->tFont.nFontWeight = (wxFontWeight)newFont.GetWeight();
        pStyle->tFont.sFontName = newFont.GetFaceName();
        pStyle->nColor = newData.GetColour();

        //update the grid
        dc.SetFont(newFont);

        dc.GetTextExtent(_T("This is a sample text qjgltTQYI"), &width, &height);
        int nPoints = pStyle->tFont.nFontSize;
        m_pGrid->SetCellFont(iRow, lmSTYLE, newFont);
        m_pGrid->SetCellTextColour(iRow, lmSTYLE, pStyle->nColor);
        m_pGrid->SetCellValue(iRow, lmFONTSIZE, wxString::Format(_T("%d pt"), nPoints));
        m_pGrid->SetRowSize(iRow, height + 8);

        //set other columns
        m_pGrid->SetCellValue(iRow, lmSTYLE, pStyle->sName);
        m_pGrid->SetCellValue(iRow, lmFONTNAME, pStyle->tFont.sFontName);
        m_pGrid->SetCellValue(iRow, lmBOLD,
                    (pStyle->tFont.nFontWeight == wxFONTWEIGHT_BOLD ? _T("1") : _T("0")));
        m_pGrid->SetCellValue(iRow, lmITALIC,
                    (pStyle->tFont.nFontStyle == wxFONTSTYLE_ITALIC ? _T("1") : _T("0")));
        m_pGrid->SetCellBackgroundColour(iRow, lmCOLOR, pStyle->nColor);
   }
    m_pGrid->ForceRefresh();
}


