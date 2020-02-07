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
#include "lenmus_shortcuts_opt_panel.h"
#include "lenmus_standard_header.h"

#include "lenmus_actions.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/config.h>


namespace lenmus
{


//---------------------------------------------------------------------------------------
// HwxGrid: helper class to force wxGrid to occupy all available with
//      Based on code posted in wxWidgets forum, under the usual wx license terms
//      by Yuri Borsky, brahmbeus@gmail.com
//
/*
 *	HwxGrid is wxGrid that would fit to parent width by resizing its columns
 *		according to their stretch factors, assigned by user;
 *
 *	Stretch Factor may be:
 *			0: 		for AutoSize columns, where size is set by wxGrid to fit content
 *			negative:       for fixed size columns where size is abs(stretch factor).
 *			positive:	the more stretch factor the more space column would get on resize
 *
 *	Example: to get grid with one autosized column, one fixed(50px) column and two more stretchable
 		columns we SetColStretch(0,0);
			   SetColStretch(1,-50);
			   SetColStretch(2,1);
			   SetColStretch(3,1);
 *
 *	(c) Yuri Borsky		brahmbeus@gmail.com
 *
 *	Usual wx license terms applies
 */

#include <wx/grid.h>

#define HwxGrid_MAXF	(3)      //max number of columns

class HwxGrid: public wxGrid
{
protected:
	int sf[HwxGrid_MAXF];       //stretch factor for each column

public:
	HwxGrid(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS,
            const wxString& name = wxGridNameStr )
        : wxGrid(parent, id, pos, size, style, name)
   	{
		//init. all columns with strecth factor = 1
		for(unsigned i=0; i < HwxGrid_MAXF; ++i)
            sf[i]=1;

		Connect( wxEVT_SIZE, wxSizeEventHandler( HwxGrid::OnSizeEvt) );
	}
	virtual ~HwxGrid() {}

	void SetColStretch(unsigned i, int factor) { if( i < HwxGrid_MAXF ) sf[i]=factor; }
	int  GetColStretch(unsigned i) const { return (i < HwxGrid_MAXF) ? sf[i] : 1; }
	void ReLayout() { StretchIt(); }

private:

	void OnSizeEvt(wxSizeEvent& ev)
	{
		if (!StretchIt())
            ev.Skip();
	}

	int StretchIt()
	{
		int new_width = GetClientSize().GetWidth() - GetRowLabelSize();
        new_width -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X); // extra space to prevent horizontal scroll bar

		int fixedWidth=0, numStretches=0, numStretched=0;
		for( int i=0; i<GetNumberCols(); ++i )
		{
			if (sf[i] == 0)         //autosized (fit content)
                fixedWidth += GetColSize(i);
			else if (sf[i] < 0)     //fixed size
			{
				//AutoSizeColumn(i, false);
				fixedWidth += 120;  //GetColSize(i);
			}
			else    //auto-stretch
            {
				numStretches += sf[i];
				numStretched += 1;
			}
		}

		//now either we have space for normal layout or resort to wxGrid default behaviour
		if(numStretched && ((fixedWidth + numStretched*10) < new_width) )
		{
			int stretchSpace = (new_width - fixedWidth) / numStretches;
			BeginBatch();
			for (int i=0; i < GetNumberCols(); ++i)
            {
				if (sf[i] > 0)
					SetColSize(i,stretchSpace*sf[i]);
            }
			EndBatch();
			return 1;
		}
		return 0;
	}

};






//---------------------------------------------------------------------------------------
// IDs for controls
const long ID_BUTTON_RESET_ALL = wxNewId();
const long ID_BUTTON_CLEAR = wxNewId();
const long ID_BUTTON_DEFINE = wxNewId();
const long ID_CATEGORY = wxNewId();


//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(ShortcutsOptPanel, wxPanel)
	EVT_BUTTON( ID_BUTTON_RESET_ALL, ShortcutsOptPanel::on_button_reset_all )
	EVT_BUTTON( ID_BUTTON_CLEAR, ShortcutsOptPanel::on_button_clear )
	EVT_BUTTON( ID_BUTTON_DEFINE, ShortcutsOptPanel::on_button_define )
	EVT_CHOICE( ID_CATEGORY, ShortcutsOptPanel::on_category_selected )
wxEND_EVENT_TABLE()



//---------------------------------------------------------------------------------------
ShortcutsOptPanel::ShortcutsOptPanel(wxWindow* parent, ApplicationScope& appScope)
    : OptionsPanel(parent, appScope)
{
    create_controls();

    //load title icon
    //TODO: Change icon
    m_pTitleIcon->SetBitmap( wxArtProvider::GetIcon("opt_shortcuts", wxART_TOOLBAR, wxSize(24,24)) );

//    //set error icons
//    wxBitmap bmpError =
//         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));
//    m_pBmpServerSettingsError->SetBitmap(bmpError);
//    m_pBmpAuthenticationError->SetBitmap(bmpError);
//
//    //hide all error messages and their associated icons
//    m_pLblServerSettingsError->Show(false);
//    m_pBmpServerSettingsError->Show(false);
//    m_pLblAuthenticationError->Show(false);
//    m_pBmpAuthenticationError->Show(false);

    load_categories();
    load_current_settings();
}

//---------------------------------------------------------------------------------------
ShortcutsOptPanel::~ShortcutsOptPanel()
{
//      releaseKeyboard();
}

//---------------------------------------------------------------------------------------
void ShortcutsOptPanel::create_controls()
{
    //content generated with wxFormBuilder
    //Manual changes:
    // - changed wxGrid by HwxGrid
	// - Force Vertical scrollbar and hide Horizontal scrollbar:
	//      added style wxVSCROLL|wxALWAYS_SHOW_SB to wxGrid creation
	//      added: m_pGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	// - added lines for setting column strecth factors and size
    //	    m_pGrid->SetColStretch(0, 1);       //auto-stretch, to get all remainder space
    //	    m_pGrid->SetColStretch(1, -120);    //fixed, 120px
    // - number of initial rows in m_pGrid changed to k_cmd_max

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pTitlePanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	m_pTitlePanel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	wxBoxSizer* pTitleSizer;
	pTitleSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pLblTitle = new wxStaticText( m_pTitlePanel, wxID_ANY, _("Keyboard shortcuts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblTitle->Wrap( -1 );
	pTitleSizer->Add( m_pLblTitle, 0, wxALL, 5 );


	pTitleSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pTitleIcon = new wxStaticBitmap( m_pTitlePanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pTitleSizer->Add( m_pTitleIcon, 0, wxALL, 5 );

	m_pTitlePanel->SetSizer( pTitleSizer );
	m_pTitlePanel->Layout();
	pTitleSizer->Fit( m_pTitlePanel );
	pMainSizer->Add( m_pTitlePanel, 0, wxEXPAND|wxBOTTOM, 5 );

	wxBoxSizer* pSizerCategory;
	pSizerCategory = new wxBoxSizer( wxHORIZONTAL );

	pCategoryLabel = new wxStaticText( this, wxID_ANY, _("Category:"), wxDefaultPosition, wxDefaultSize, 0 );
	pCategoryLabel->Wrap( -1 );
	pSizerCategory->Add( pCategoryLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_pCategoryChoiceChoices;
	m_pCategoryChoice = new wxChoice( this, ID_CATEGORY, wxDefaultPosition, wxDefaultSize, m_pCategoryChoiceChoices, 0 );
	m_pCategoryChoice->SetSelection( 0 );
	pSizerCategory->Add( m_pCategoryChoice, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	pMainSizer->Add( pSizerCategory, 0, wxEXPAND, 5 );

	m_pGrid = new HwxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxVSCROLL ); //|wxALWAYS_SHOW_SB );

	// Grid
	m_pGrid->CreateGrid( k_cmd_max, 2 );
	m_pGrid->EnableEditing( false );
	m_pGrid->EnableGridLines( true );
	m_pGrid->EnableDragGridSize( false );
	m_pGrid->SetMargins( 0, 0 );
//#if wxVERSION_NUMBER >= 3.0 (how to encode 3.0?)
//	m_pGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
//#endif

	// Columns
	m_pGrid->SetColSize( 0, 100 );
	m_pGrid->SetColSize( 1, 120 );
	m_pGrid->SetColStretch(0, 1);       //auto-stretch, to get all remainder space
	m_pGrid->SetColStretch(1, -120);    //fixed, 120px
	m_pGrid->EnableDragColMove( false );
	m_pGrid->EnableDragColSize( false );
	m_pGrid->SetColLabelSize( 30 );
	m_pGrid->SetColLabelValue( 0, _("Action") );
	m_pGrid->SetColLabelValue( 1, _("Shortcut") );
	m_pGrid->SetColLabelAlignment( wxALIGN_LEFT, wxALIGN_CENTRE );
//#if wxVERSION_NUMBER >= 3.0 (how to encode 3.0?)
//	m_pGrid->SetSortingColumn(0);
//#endif

	// Rows
	m_pGrid->AutoSizeRows();
	m_pGrid->EnableDragRowSize( false );
	m_pGrid->SetRowLabelSize( 0 );
	//m_pGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

	// Label Appearance

	// Cell Defaults
	m_pGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	pMainSizer->Add( m_pGrid, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* pSizerButtons;
	pSizerButtons = new wxBoxSizer( wxHORIZONTAL );

	m_pResetAll = new wxButton( this, ID_BUTTON_RESET_ALL, _("Reset all to default"), wxDefaultPosition, wxDefaultSize, 0 );
	pSizerButtons->Add( m_pResetAll, 0, wxALL, 5 );

	m_pClear = new wxButton( this, ID_BUTTON_CLEAR, _("Clear selected"), wxDefaultPosition, wxDefaultSize, 0 );
	pSizerButtons->Add( m_pClear, 0, wxALL, 5 );

	m_pDefine = new wxButton( this, ID_BUTTON_DEFINE, _("Change selected"), wxDefaultPosition, wxDefaultSize, 0 );
	pSizerButtons->Add( m_pDefine, 0, wxALL, 5 );

	pMainSizer->Add( pSizerButtons, 0, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
void ShortcutsOptPanel::load_categories()
{
    KeyTranslator* table = m_appScope.get_key_translator();
    for (int i=0; i < table->get_max_category(); ++i)
    {
        m_pCategoryChoice->Append( table->get_category_name(i) );
    }
	m_pCategoryChoice->SetSelection( 0 );
}

//---------------------------------------------------------------------------------------
void ShortcutsOptPanel::load_current_settings()
{
    m_pGrid->ClearGrid();

    int iCategory = m_pCategoryChoice->GetCurrentSelection();
    KeyTranslator* table = m_appScope.get_key_translator();
    int iRow = 0;
    for (int i=0; i < table->get_num_actions(); ++i)
    {
        AppAction* a = table->get_action(i);
        if (a && a->category() == iCategory)
        {
            m_pGrid->SetCellValue(iRow, 0, a->description());
            //TODO: save ptr. to AppAction
            if (a->has_shortcut())
                m_pGrid->SetCellValue(iRow, 1, a->shortcut_as_string());
            ++iRow;
        }
    }
}

//---------------------------------------------------------------------------------------
bool ShortcutsOptPanel::Verify()
{
    //assume no errors
    //bool fError = false;
    bool fLocalError = false;
    //bool fRelayout = false;

//    m_pLblServerSettingsError->Show(false);
//    m_pBmpServerSettingsError->Show(false);
//    m_pLblAuthenticationError->Show(false);
//    m_pBmpAuthenticationError->Show(false);
//
//
//    if (fRelayout) {
//        // Force a relayout. This code is to by pass a bad behaviour.
//        // When we change the visibility status (hide/show) of error messages
//        // they are not properly drawn in their right positions. So we have to force a
//        // relayout
//        GetSizer()->CalcMin();
//        GetSizer()->Layout();
//        GetSizer()->Fit(this);
//        GetSizer()->SetSizeHints(this);
//        //Now the panel is properly drawn but its width changes and the splitter windows
//        //doesn't get aware. It is necessary to force a redraw of the splitter window
//        m_pParent->SetSashPosition(m_pParent->GetSashPosition(), true);
//        //OK. Now it works.
//    }

    return fLocalError;
}

//---------------------------------------------------------------------------------------
void ShortcutsOptPanel::Apply()
{
    KeyTranslator* table = m_appScope.get_key_translator();
    table->save_user_preferences();
}

//---------------------------------------------------------------------------------------
void ShortcutsOptPanel::on_button_reset_all(wxCommandEvent& WXUNUSED(event))
{
    //TODO
//    KeyTranslator* table = m_appScope.get_key_translator();
//    table->load_defaults();
//    load_current_settings();
//
//    Refresh();
}

//---------------------------------------------------------------------------------------
void ShortcutsOptPanel::on_category_selected(wxCommandEvent& WXUNUSED(event))
{
    load_current_settings();
}

//---------------------------------------------------------------------------------------
void ShortcutsOptPanel::on_button_clear(wxCommandEvent& WXUNUSED(event))
{
//    KeyTranslator* table = m_appScope.get_key_translator();
//    wxGridCellCoordsArray cells = m_pGrid->GetSelectedCells();
//    int iRow = 0;
//    for (int i=0; i < k_cmd_max; ++i)
//    {
//        AppAction* a = table->find(i);
//        if (a)
//        {
//            m_pGrid->SetCellValue(iRow, 0, a->description());
//            if (a->has_shortcut())
//                m_pGrid->SetCellValue(iRow, 1, a->shortcut_as_string());
//            ++iRow;
//        }
//    }
}

//---------------------------------------------------------------------------------------
void ShortcutsOptPanel::on_button_define(wxCommandEvent& WXUNUSED(event))
{

}


}   //namespace lenmus
