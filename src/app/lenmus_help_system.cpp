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
#include "lenmus_help_system.h"

#include "lenmus_string.h"
#include "lenmus_paths.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/artprov.h>
#include <wx/gdicmn.h>
#include <wx/wxsqlite3.h>               //to access wxSQLite3 DB

//lomse
#include <lomse_doorway.h>
#include <lomse_injectors.h>
using namespace lomse;

//other
#include <wxMidi.h>         //to get wxMidi version


//wxWidgets
#include <wx/dialog.h>

//forward declarations
class wxButton;
class wxWindow;
class wxFileName;

namespace lenmus
{

//=======================================================================================
// Helper class, to give feedback about opening help system
//=======================================================================================

const long k_id_timer = wxNewId();

//---------------------------------------------------------------------------------------
class HelpDlg : public wxDialog
{
protected:
    wxStaticText* m_pMsg;
    wxGauge* m_pGauge;
    wxTimer m_timer;

public:
	HelpDlg(wxWindow* pParent = nullptr);
	~HelpDlg();

    void create_controls();

    // event handlers
    void on_timer_event(wxTimerEvent& WXUNUSED(event));
    void on_activate(wxActivateEvent& event);

protected:
    wxDECLARE_EVENT_TABLE();
};

//=======================================================================================
// HelpDlg implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(HelpDlg, wxDialog)
    EVT_TIMER       (k_id_timer, HelpDlg::on_timer_event)
    EVT_ACTIVATE    (HelpDlg::on_activate)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
HelpDlg::HelpDlg(wxWindow* pParent)
    : wxDialog(pParent, wxID_ANY, _("Help system"),
               wxDefaultPosition, wxSize(400,150),
               wxDEFAULT_DIALOG_STYLE)
    , m_timer(this, k_id_timer)
{
    create_controls();
    m_pGauge->SetRange(100);
    m_pGauge->SetValue(0);
    CentreOnScreen();
    m_timer.Start(200, wxTIMER_CONTINUOUS);     //200ms
}

//---------------------------------------------------------------------------------------
HelpDlg::~HelpDlg()
{
    m_timer.Stop();
}

//---------------------------------------------------------------------------------------
void HelpDlg::create_controls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	pMainSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pMsg = new wxStaticText( this, wxID_ANY, _("Opening Internet browser. Please wait ..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_pMsg->Wrap( -1 );
	pMainSizer->Add( m_pMsg, 0, wxALL|wxEXPAND, 5 );

	m_pGauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	pMainSizer->Add( m_pGauge, 0, wxALL|wxEXPAND, 5 );


	pMainSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
void HelpDlg::on_timer_event(wxTimerEvent& WXUNUSED(event))
{
    int value = m_pGauge->GetValue();
    m_pGauge->SetValue( value == 100 ? 0 : value + 2 );
}

//---------------------------------------------------------------------------------------
void HelpDlg::on_activate(wxActivateEvent& event)
{
    if (!event.GetActive())
        EndModal(wxID_CANCEL);
}


//=======================================================================================
// HelpSystem implementation
//=======================================================================================

HelpSystem::HelpSystem(wxWindow* pParent, ApplicationScope& appScope)
    : m_appScope(appScope)
    , m_pParent(pParent)
    , m_pHelp(nullptr)
{
}

//---------------------------------------------------------------------------------------
bool HelpSystem::initialize()
{
    //returns true if success

    if (!m_pHelp)
    {
        m_pHelp = LENMUS_NEW wxExtHelpController(m_pParent);
        wxString sPath = m_appScope.get_paths()->GetHelpPath();
        if (m_pHelp->Initialize(sPath) )
            return true;

        //error: path not found
        //if language is not English, try to fall-back on English documentation
        wxConfigBase* pPrefs = m_appScope.get_preferences();
        wxString lang = pPrefs->Read("/Locale/Language", "");
        if (lang != "en")
        {
            wxFileName oFile(m_appScope.get_paths()->GetLocaleRootPath(), wxPATH_NATIVE);
            oFile.AppendDir("en");
            oFile.AppendDir("help");
            if (m_pHelp->Initialize(oFile.GetFullPath()) )
                return true;
        }
    }

    ::wxMessageBox(_("Help documentation not found: Re-install LenMus?"));
    delete m_pHelp;
    m_pHelp = nullptr;
    return false;
}

//---------------------------------------------------------------------------------------
bool HelpSystem::display_section(int nSect)
{
    //AWARE: this method uses section ids for displaying particular topics.
    //Section ids are specified in the wxhelp.map file

    //returns true if success

    if (m_pHelp)
    {
        if (!m_pHelp->DisplaySection(nSect))
        {
            wxMessageBox("Failed to open external browser or section id not found");
            return false;
        }
        else
        {
            HelpDlg dlg(m_pParent);
            dlg.ShowModal();
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool HelpSystem::display_theme(const wxString& theme)
{
    //TODO: Is this method needed?

    if (m_pHelp)
    {
        if (!m_pHelp->DisplaySection(theme))
        {
            wxMessageBox("Failed to open external browser or theme not found");
            return false;
        }
        else
        {
            HelpDlg dlg(m_pParent);
            dlg.ShowModal();
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool HelpSystem::search_for(const wxString& key)
{
    //returns true if success

    if (m_pHelp)
    {
        if (m_pHelp->KeywordSearch(key))
        {
            HelpDlg dlg(m_pParent);
            dlg.ShowModal();
        }
        return true;
    }
    return false;
}


}   //namespace lenmus
