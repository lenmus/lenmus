//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2021 LenMus project
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
#include "lenmus_welcome_window.h"
#include "lenmus_standard_header.h"

#include "lenmus_paths.h"
#include "lenmus_main_frame.h"
#include "lenmus_string.h"
#include "lenmus_dlg_levels.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
//#include <wx/hyperlink.h>
#include <wx/panel.h>
//#include <wx/docview.h>      //for wxFileHistory
#include <wx/artprov.h>


namespace lenmus
{

//IDs for controls
const int k_id_exercises = wxNewId();
const int k_id_book_1 = wxNewId();
const int k_id_book_2 = wxNewId();
const int k_id_book_3 = wxNewId();
const int k_id_study_guide = wxNewId();



wxBEGIN_EVENT_TABLE(WelcomeWindow, wxScrolledWindow)
    EVT_BUTTON(k_id_exercises, WelcomeWindow::on_button_exercises)
    EVT_BUTTON(k_id_book_1, WelcomeWindow::on_button_book_1)
    EVT_BUTTON(k_id_book_2, WelcomeWindow::on_button_book_2)
    EVT_BUTTON(k_id_book_3, WelcomeWindow::on_button_book_3)
    EVT_BUTTON(k_id_study_guide, WelcomeWindow::on_button_study_guide)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
WelcomeWindow::WelcomeWindow(ContentWindow* parent, ApplicationScope& appScope,
                             wxWindowID id)
    : wxScrolledWindow(parent, id, wxDefaultPosition, wxSize(1000, 700), wxHW_SCROLLBAR_AUTO | wxHSCROLL | wxVSCROLL)
    , CanvasInterface(parent)
    , m_appScope(appScope)
{
    SetVirtualSize(1000, 750);      //1000 x (770-20) px
    SetScrollRate(20, 20);          //20px steps

	create_controls();

	//set eBooks name
    Paths* pPaths = m_appScope.get_paths();
    if (pPaths->GetLanguageCode() == "es")
    {
	    m_txtLevel1->SetLabel("Lectura musical I");
        m_txtLevel2->SetLabel("Lectura musical II");
        m_txtLevel3->SetLabel(L"Teoría y armonía");
    }

    //load icons
    m_logoUCA->SetBitmap( wxArtProvider::GetIcon("logoUCA", wxART_OTHER) );

    m_logo->SetBitmap( wxArtProvider::GetIcon("logo50x67", wxART_OTHER) );
    m_bmpGuide->SetBitmap( wxArtProvider::GetIcon("welcome_find", wxART_OTHER) );
    m_bmpTrain->SetBitmap( wxArtProvider::GetIcon("welcome_practice", wxART_OTHER) );
//    m_bmpCollaborate->SetBitmap( wxArtProvider::GetIcon("welcome_colaborate", wxART_OTHER) );
    m_bmpBooks->SetBitmap( wxArtProvider::GetIcon("welcome_book", wxART_OTHER) );
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::create_controls()
{
    //content generated with wxFormBuilder (x_unversioned(StartPage.fbp). Modified to:
    // - add version number to title
    // - add loop to load recent files

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* sizerMain;
	sizerMain = new wxBoxSizer( wxVERTICAL );

	m_logo = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	sizerMain->Add( m_logo, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 30 );


	m_txtWelcome = new wxStaticText( this, wxID_ANY, _("Welcome to LenMus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtWelcome->Wrap( -1 );
	m_txtWelcome->SetFont( wxFont( 24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerMain->Add( m_txtWelcome, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxRIGHT|wxLEFT, 5 );

    wxString version = m_appScope.get_version_string();
    wxString title = wxString::Format(_("Version %s"), version.wx_str());
	m_txtVersion = new wxStaticText( this, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, 0 );
	m_txtVersion->Wrap( -1 );
	sizerMain->Add( m_txtVersion, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_txtWhatToDo = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_txtWhatToDo->Wrap( -1 );
	m_txtWhatToDo->SetFont( wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	m_txtWhatToDo->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT ) );

	sizerMain->Add( m_txtWhatToDo, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


//	sizerMain->Add( 0, 0, 1, wxEXPAND, 5 );

    //content area
	wxBoxSizer* sizerContent;
	sizerContent = new wxBoxSizer( wxHORIZONTAL );

    //margin at left
	sizerContent->Add( 0, 0, 1, wxEXPAND, 5 );
//	sizerContent->Add( 60, 0, 0, 0, 5 );

	//area for left content
	wxBoxSizer* sizerLeft;
	sizerLeft = new wxBoxSizer( wxVERTICAL );


	//Guide block
	wxBoxSizer* sizerGuideBox;
	sizerGuideBox = new wxBoxSizer( wxHORIZONTAL );

	m_bmpGuide = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerGuideBox->Add( m_bmpGuide, 0, wxALL, 5 );

	wxBoxSizer* sizerGuide;
	sizerGuide = new wxBoxSizer( wxVERTICAL );

	m_txtGuide = new wxStaticText( this, wxID_ANY, _("Get started"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtGuide->Wrap( -1 );
	m_txtGuide->SetFont( wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerGuide->Add( m_txtGuide, 0, wxALL|wxEXPAND, 5 );

	m_txtGuideText = new wxStaticText( this, wxID_ANY, _("Read about how to get the most out of LenMus."), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_txtGuideText->Wrap( 350 );
	m_txtGuideText->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );

	sizerGuide->Add( m_txtGuideText, 0, wxALL, 5 );

	m_btnGuide = new wxButton( this, k_id_study_guide, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGuide->Add( m_btnGuide, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	sizerGuideBox->Add( sizerGuide, 0, wxALL, 5 );
    sizerLeft->Add( sizerGuideBox, 0, 0, 5 );

	//End of Guide block


    sizerLeft->Add( 0, 60, 0, 0, 5 );       //space between this block and next block


	//Block: Practice
	wxBoxSizer* sizerTrainBox;
	sizerTrainBox = new wxBoxSizer( wxHORIZONTAL );

	m_bmpTrain = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerTrainBox->Add( m_bmpTrain, 0, wxALL, 5 );

	wxBoxSizer* sizerTrain;
	sizerTrain = new wxBoxSizer( wxVERTICAL );

	m_txtTrain = new wxStaticText( this, wxID_ANY, _("Train with LenMus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtTrain->Wrap( -1 );
	m_txtTrain->SetFont( wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerTrain->Add( m_txtTrain, 0, wxALL, 5 );

	m_txtTrainText = new wxStaticText( this, wxID_ANY, _("Develop your musical skills with progressive exercises that will help you assimilate the different contents. Set your own learning pace."), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_txtTrainText->Wrap( 350 );
	m_txtTrainText->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );

	sizerTrain->Add( m_txtTrainText, 0, wxALL, 5 );

	m_btnTrain = new wxButton( this, k_id_exercises, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerTrain->Add( m_btnTrain, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	sizerTrainBox->Add( sizerTrain, 0, wxALL, 5 );
	sizerLeft->Add( sizerTrainBox, 0, 0, 5 );


	//End of Practice block


	sizerContent->Add( sizerLeft, 0, 0, 5 );

	sizerContent->Add( 60, 0, 0, 0, 5 );

	//****** End of left block *****************


    //area for right content
	wxBoxSizer* sizerRight;
	sizerRight = new wxBoxSizer( wxVERTICAL );

	m_bmpBooks = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 128,-1 ), 0 );
	sizerRight->Add( m_bmpBooks, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_txtBooks = new wxStaticText( this, wxID_ANY, _("Study and practice"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtBooks->Wrap( -1 );
	m_txtBooks->SetFont( wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	sizerRight->Add( m_txtBooks, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_txtBooksText = new wxStaticText( this, wxID_ANY, _("Study with the interactive LenMus music theory books. Start with the first of these two:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_txtBooksText->Wrap( 340 );
	m_txtBooksText->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );

	sizerRight->Add( m_txtBooksText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxFlexGridSizer* sizerGridBooks;
	sizerGridBooks = new wxFlexGridSizer( 4, 2, 0, 0 );
	sizerGridBooks->SetFlexibleDirection( wxHORIZONTAL );
	sizerGridBooks->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	//------- book 1
	m_txtLevel1 = new wxStaticText( this, wxID_ANY, _("Music Reading I"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtLevel1->Wrap( -1 );
	m_txtLevel1->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	sizerGridBooks->Add( m_txtLevel1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_btnLevel1 = new wxButton( this, k_id_book_1, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGridBooks->Add( m_btnLevel1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	//------- book 2
	m_txtLevel2 = new wxStaticText( this, wxID_ANY, _("Music Reading II"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtLevel2->Wrap( -1 );
	m_txtLevel2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	sizerGridBooks->Add( m_txtLevel2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_btnLevel2 = new wxButton( this, k_id_book_2, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGridBooks->Add( m_btnLevel2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	//------- book 3
	m_txtLevel3 = new wxStaticText( this, wxID_ANY, _("Theory and Harmony"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtLevel3->Wrap( -1 );
	m_txtLevel3->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	sizerGridBooks->Add( m_txtLevel3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_btnLevel3 = new wxButton( this, k_id_book_3, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerGridBooks->Add( m_btnLevel3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );



	sizerRight->Add( sizerGridBooks, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	    // close content

	sizerContent->Add( sizerRight, 0, 0, 5 );

    //margin at right
	sizerContent->Add( 0, 0, 1, wxEXPAND, 5 );
	sizerMain->Add( sizerContent, 0, wxEXPAND, 5 );


	    //UCA block

	//Block: UCA Logo
    sizerRight->Add( 0, 40, 0, 0, 5 );       //space between this block and next block

	wxBoxSizer* sizerUCABox;
	sizerUCABox = new wxBoxSizer( wxHORIZONTAL );

	sizerUCABox->Add( 0, 0, 1, 0, 5 );  //spacer, to center

	m_txtUCA = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	//m_txtUCA->SetLabel( "\n" + _("Colabora:") );
	m_txtUCA->SetLabel( "\n" + _("Acknowledgments:") );
	m_txtUCA->Wrap( -1 );
	m_txtUCA->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	sizerUCABox->Add( m_txtUCA, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_logoUCA = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sizerUCABox->Add( m_logoUCA, 0, wxALIGN_CENTER_VERTICAL, 5 );

	sizerUCABox->Add( 0, 0, 1, 0, 5 );  //spacer, to center



	sizerMain->Add( sizerUCABox, 0, wxEXPAND, 5 );



	this->SetSizer( sizerMain );
	this->Layout();
}

//---------------------------------------------------------------------------------------
WelcomeWindow::~WelcomeWindow()
{
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_exercises(wxCommandEvent& UNUSED(event))
{
    LevelsDlg dlg(this, m_appScope);
    if (dlg.ShowModal() == wxID_OK)
        open_book("GeneralExercises.lmb", dlg.get_level());
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_book_1(wxCommandEvent& UNUSED(event))
{
    Paths* pPaths = m_appScope.get_paths();
    wxString lang = pPaths->GetLanguageCode();
    if (lang == "es")
        open_book("L1_MusicReading_v2.lmb");
    else
        open_book("L1_MusicReading.lmb");
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_book_2(wxCommandEvent& UNUSED(event))
{
    Paths* pPaths = m_appScope.get_paths();
    wxString lang = pPaths->GetLanguageCode();
    if (lang == "es")
        open_book("L2_MusicReading_v2.lmb");
    else
        open_book("L2_MusicReading.lmb");
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_book_3(wxCommandEvent& UNUSED(event))
{
    Paths* pPaths = m_appScope.get_paths();
    wxString lang = pPaths->GetLanguageCode();
    open_book("TheoryHarmony.lmb");
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::open_book(const wxString& filename, int level)
{
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetBooksPath();
    wxFileName oFile(sPath, filename, wxPATH_NATIVE);
    if (!oFile.FileExists())
    {
        //try to use the english version
        sPath = pPaths->GetLocaleRootPath();
        oFile.AssignDir(sPath);
        oFile.AppendDir("en");
        oFile.AppendDir("books");
        oFile.SetFullName(filename);
        if (!oFile.FileExists())
        {
            wxMessageBox(_("Sorry: File not found!"));
            LOMSE_LOG_WARN("File '%s' not found!",
                           oFile.GetFullPath().ToStdString().c_str() );
            return;
        }
    }

    wxCommandEvent myEvent(LM_EVT_OPEN_BOOK, k_id_open_book);
    myEvent.SetString( oFile.GetFullPath() );
    myEvent.SetInt(level);
    ::wxPostEvent(this, myEvent);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::on_button_study_guide(wxCommandEvent& UNUSED(event))
{
    wxString sDoc = "study-guide.htm";
    show_document(sDoc);
}

//---------------------------------------------------------------------------------------
void WelcomeWindow::show_document(const wxString& sDocName)
{
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetLocalePath();
    wxFileName oFile(sPath, sDocName, wxPATH_NATIVE);
	if (!oFile.FileExists())
	{
		//try to display the english version
		sPath = pPaths->GetLocaleRootPath();
		oFile.AssignDir(sPath);
		oFile.AppendDir("en");
		oFile.SetFullName(sDocName);
        if (!oFile.FileExists())
        {
            wxMessageBox(_("Sorry: File not found!"));
            wxLogMessage("[WelcomeWindow::show_document] File %s' not found!",
                         oFile.GetFullPath().wx_str() );
            return;
        }
	}
    ::wxLaunchDefaultBrowser( oFile.GetFullPath() );
}


}   //namespace lenmus
