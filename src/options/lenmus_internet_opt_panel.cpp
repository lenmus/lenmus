//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#include "lenmus_internet_opt_panel.h"

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
// IDs for controls
const long ID_CHK_USE_PROXY = wxNewId();
const long ID_CHK_PROXY_AUTHENTICATION = wxNewId();
const long ID_TXT_HOSTNAME = wxNewId();
const long ID_TXT_PORT_NUMBER = wxNewId();
const long ID_TXT_USERNAME = wxNewId();
const long ID_TXT_PASSWORD = wxNewId();

//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(InternetOptPanel, wxPanel)
    EVT_CHECKBOX (ID_CHK_USE_PROXY, InternetOptPanel::OnChkUseProxyClicked )
    EVT_CHECKBOX (ID_CHK_PROXY_AUTHENTICATION, InternetOptPanel::OnChkProxyAuthenticationClicked )
    EVT_TEXT (ID_TXT_HOSTNAME, InternetOptPanel::OnDataChanged )
    EVT_TEXT (ID_TXT_PORT_NUMBER, InternetOptPanel::OnDataChanged )
    EVT_TEXT (ID_TXT_USERNAME, InternetOptPanel::OnDataChanged )
    EVT_TEXT (ID_TXT_PASSWORD, InternetOptPanel::OnDataChanged )
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
InternetOptPanel::InternetOptPanel(wxWindow* parent, ApplicationScope& appScope)
    : OptionsPanel(parent, appScope)
    , m_pParent( static_cast<wxSplitterWindow*>(parent) )
    , m_fFirstTimeServerSettingsError(true)
    , m_fFirstTimeAuthenticationError(true)
{
    create_controls();

    //load icon
    m_pTitleIcon->SetBitmap( wxArtProvider::GetIcon("opt_internet", wxART_TOOLBAR, wxSize(24,24)) );

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap("msg_error", wxART_TOOLBAR, wxSize(16,16));
    m_pBmpServerSettingsError->SetBitmap(bmpError);
    m_pBmpAuthenticationError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pLblServerSettingsError->Show(false);
    m_pBmpServerSettingsError->Show(false);
    m_pLblAuthenticationError->Show(false);
    m_pBmpAuthenticationError->Show(false);

    // populate combo box
    m_pCboCheckFreq->Append( _("Never") );
    m_pCboCheckFreq->Append( _("Daily") );
    m_pCboCheckFreq->Append( _("Weekly") );
    m_pCboCheckFreq->Append( _("Monthly") );

        //Select current settings

    // web update frequency
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    wxString sCheckFreq = pPrefs->Read("/Options/CheckForUpdates/Frequency",
                                       "Weekly" );
    if (sCheckFreq == "Never")
        m_pCboCheckFreq->SetSelection(0);
    else if (sCheckFreq == "Daily")
        m_pCboCheckFreq->SetSelection(1);
    else if (sCheckFreq == "Weekly")
        m_pCboCheckFreq->SetSelection(2);
    else if (sCheckFreq == "Monthly")
        m_pCboCheckFreq->SetSelection(3);
    else {
        m_pCboCheckFreq->SetSelection(2);       // assume weekly
        LOMSE_LOG_WARN(
            "Invalid value in ini file. Key '/Options/CheckForUpdates/Frequency', value='%s'",
            sCheckFreq.ToStdString().c_str() );
    }

    // display web update last check date
    wxString sLastCheckDate = pPrefs->Read("/Options/CheckForUpdates/LastCheck",
                                           "");
    if (sLastCheckDate == "") {
        sLastCheckDate = _("Never");
    }
    m_pTxtLastCheckDate->SetLabel(sLastCheckDate);

    //proxy settings
    bool fUseProxy;
    pPrefs->Read("/Internet/UseProxy", &fUseProxy, false);
    m_pChkUseProxy->SetValue(fUseProxy);
    EnableProxySettings(fUseProxy);

    m_pTxtHostname->SetValue( pPrefs->Read("/Internet/Hostname", "") );

    long nPort = 0;
    wxString sPort = pPrefs->Read("/Internet/PortNumber", "");
    if (sPort.IsNumber())
        sPort.ToLong(&nPort);
    m_pTxtPortNumber->SetValue( wxString::Format("%d", int(nPort)) );

    bool fAuthentication;
    pPrefs->Read("/Internet/ProxyAuthentication", &fAuthentication, false);
    m_pChkProxyAuthentication->SetValue( fAuthentication );

    m_pTxtUsername->SetValue( pPrefs->Read("/Internet/Username", "") );

    m_pTxtPassword->SetValue( pPrefs->Read("/Internet/Password", "") );
}

//---------------------------------------------------------------------------------------
InternetOptPanel::~InternetOptPanel()
{
}

//---------------------------------------------------------------------------------------
void InternetOptPanel::create_controls()
{
    //content generated with wxFormBuilder
    //Manual changes:
    // - move m_pBoxProxySettings definition to header
    // - move m_pBoxServerSettings definition to header
    // - move m_pBoxProxyAuthentication definition to header

	wxBoxSizer* pMainSizer;
	pMainSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pTitlePanel = LENMUS_NEW wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* pTitleSizer;
	pTitleSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pLblTitle = LENMUS_NEW wxStaticText( m_pTitlePanel, wxID_ANY, _("Internet connection options"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_pLblTitle->Wrap( -1 );
	m_pLblTitle->SetFont( wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pTitleSizer->Add( m_pLblTitle, 0, wxALIGN_TOP|wxALL, 5 );


	pTitleSizer->Add( 5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTitleIcon = LENMUS_NEW wxStaticBitmap( m_pTitlePanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pTitleSizer->Add( m_pTitleIcon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTitlePanel->SetSizer( pTitleSizer );
	m_pTitlePanel->Layout();
	pTitleSizer->Fit( m_pTitlePanel );
	pMainSizer->Add( m_pTitlePanel, 0, wxEXPAND|wxBOTTOM, 5 );

	wxBoxSizer* pContentSizer;
	pContentSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* pBoxUpdates;
	pBoxUpdates = LENMUS_NEW wxStaticBoxSizer( LENMUS_NEW wxStaticBox( this, wxID_ANY, _("Software updates") ), wxVERTICAL );

	wxBoxSizer* pSizerCheckFreq;
	pSizerCheckFreq = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );


	pSizerCheckFreq->Add( 5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTxtCheckFreq = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Check for updates"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtCheckFreq->Wrap( -1 );
	pSizerCheckFreq->Add( m_pTxtCheckFreq, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pCboCheckFreq = LENMUS_NEW wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_DROPDOWN );
	pSizerCheckFreq->Add( m_pCboCheckFreq, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	pBoxUpdates->Add( pSizerCheckFreq, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5 );

	wxBoxSizer* pSizerLastCheck;
	pSizerLastCheck = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );


	pSizerLastCheck->Add( 5, 5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

	m_pTxtLastCheck = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Last successful check:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtLastCheck->Wrap( -1 );
	pSizerLastCheck->Add( m_pTxtLastCheck, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

	m_pTxtLastCheckDate = LENMUS_NEW wxStaticText( this, wxID_ANY, "dd/mm/yyyy", wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtLastCheckDate->Wrap( -1 );
	pSizerLastCheck->Add( m_pTxtLastCheckDate, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

	pBoxUpdates->Add( pSizerLastCheck, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

	pContentSizer->Add( pBoxUpdates, 0, wxEXPAND|wxALL, 5 );

	m_pChkUseProxy = LENMUS_NEW wxCheckBox( this, ID_CHK_USE_PROXY, _("Use a proxy server to connect to Internet"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	pContentSizer->Add( m_pChkUseProxy, 0, wxEXPAND|wxALL, 5 );

	m_pBoxProxySettings = LENMUS_NEW wxStaticBoxSizer( LENMUS_NEW wxStaticBox( this, wxID_ANY, _("Proxy settings") ), wxVERTICAL );

	m_pBoxServerSettings = LENMUS_NEW wxStaticBoxSizer( LENMUS_NEW wxStaticBox( this, wxID_ANY, _("Server settings") ), wxVERTICAL );

	wxBoxSizer* pSizerProxy;
	pSizerProxy = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	wxBoxSizer* pSizerServerSettings;
	pSizerServerSettings = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pLblHostname = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Host:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblHostname->Wrap( -1 );
	pSizerServerSettings->Add( m_pLblHostname, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTxtHostname = LENMUS_NEW wxTextCtrl( this, ID_TXT_HOSTNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pSizerServerSettings->Add( m_pTxtHostname, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	pSizerServerSettings->Add( 5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pLblPortNumber = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Port number:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblPortNumber->Wrap( -1 );
	pSizerServerSettings->Add( m_pLblPortNumber, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTxtPortNumber = LENMUS_NEW wxTextCtrl( this, ID_TXT_PORT_NUMBER, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	pSizerServerSettings->Add( m_pTxtPortNumber, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	pSizerProxy->Add( pSizerServerSettings, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );

	wxBoxSizer* pSizerServerSettingsError;
	pSizerServerSettingsError = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBmpServerSettingsError = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pSizerServerSettingsError->Add( m_pBmpServerSettingsError, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_pLblSpacer1 = LENMUS_NEW wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblSpacer1->Wrap( -1 );
	pSizerServerSettingsError->Add( m_pLblSpacer1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );

	m_pLblServerSettingsError = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Please enter data for both, host name and port number"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblServerSettingsError->Wrap( -1 );
	m_pLblServerSettingsError->SetBackgroundColour( wxColour( 255, 215, 215 ) );

	pSizerServerSettingsError->Add( m_pLblServerSettingsError, 0, wxALIGN_CENTER_VERTICAL, 5 );

	pSizerProxy->Add( pSizerServerSettingsError, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_pBoxServerSettings->Add( pSizerProxy, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

	m_pBoxProxySettings->Add( m_pBoxServerSettings, 0, wxEXPAND|wxALL, 5 );

	m_pChkProxyAuthentication = LENMUS_NEW wxCheckBox( this, ID_CHK_PROXY_AUTHENTICATION, _("Proxy server requires authentication"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	m_pBoxProxySettings->Add( m_pChkProxyAuthentication, 0, wxEXPAND|wxALL, 5 );

	m_pBoxProxyAuthentication = LENMUS_NEW wxStaticBoxSizer( LENMUS_NEW wxStaticBox( this, wxID_ANY, _("Authentication") ), wxVERTICAL );

	wxBoxSizer* pSizerUsername;
	pSizerUsername = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pLblUsername = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblUsername->Wrap( -1 );
	pSizerUsername->Add( m_pLblUsername, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTxtUsername = LENMUS_NEW wxTextCtrl( this, ID_TXT_USERNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pSizerUsername->Add( m_pTxtUsername, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	pSizerUsername->Add( 5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pLblPassword = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblPassword->Wrap( -1 );
	pSizerUsername->Add( m_pLblPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTxtPassword = LENMUS_NEW wxTextCtrl( this, ID_TXT_PASSWORD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	pSizerUsername->Add( m_pTxtPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pBoxProxyAuthentication->Add( pSizerUsername, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT, 5 );

	wxBoxSizer* pSizerErrorAuthentication;
	pSizerErrorAuthentication = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pBmpAuthenticationError = LENMUS_NEW wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pSizerErrorAuthentication->Add( m_pBmpAuthenticationError, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_pSpacer2 = LENMUS_NEW wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pSpacer2->Wrap( -1 );
	pSizerErrorAuthentication->Add( m_pSpacer2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );

	m_pLblAuthenticationError = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Please enter data for both, user name and password"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblAuthenticationError->Wrap( -1 );
	m_pLblAuthenticationError->SetBackgroundColour( wxColour( 255, 215, 215 ) );

	pSizerErrorAuthentication->Add( m_pLblAuthenticationError, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_pBoxProxyAuthentication->Add( pSizerErrorAuthentication, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_pBoxProxySettings->Add( m_pBoxProxyAuthentication, 0, wxEXPAND|wxALL, 5 );

	pContentSizer->Add( m_pBoxProxySettings, 0, wxEXPAND|wxALL, 5 );

	pMainSizer->Add( pContentSizer, 0, wxEXPAND|wxALL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
bool InternetOptPanel::Verify()
{
    //assume no errors
    bool fError = false;
    bool fLocalError = false;
    bool fRelayout = false;

    m_pLblServerSettingsError->Show(false);
    m_pBmpServerSettingsError->Show(false);
    m_pLblAuthenticationError->Show(false);
    m_pBmpAuthenticationError->Show(false);

    //proxy settings
    bool fUseProxy = m_pChkUseProxy->IsChecked();
    if (fUseProxy)
    {
        // verify host name & port number
        if (m_pTxtHostname->GetValue() == "" ||
            m_pTxtPortNumber->GetValue() == "0" ||
            m_pTxtPortNumber->GetValue() == "" ||
            !(m_pTxtPortNumber->GetValue()).IsNumber() )
        {
            m_pLblServerSettingsError->Show(true);
            m_pBmpServerSettingsError->Show(true);
            fError = true;
            if (m_fFirstTimeServerSettingsError) {
                fRelayout = true;
                m_fFirstTimeServerSettingsError = false;
            }
        }
        fLocalError |= fError;

        // proxy authentication
        bool fAuthentication = m_pChkProxyAuthentication->IsChecked();
        if (fAuthentication)
        {
            // verify username & password
            if (m_pTxtUsername->GetValue() == "" || m_pTxtPassword->GetValue() == "") {
                m_pLblAuthenticationError->Show(true);
                m_pBmpAuthenticationError->Show(true);
                fError = true;
                if (m_fFirstTimeAuthenticationError) {
                    fRelayout = true;
                    m_fFirstTimeAuthenticationError = false;
                }
            }
            fLocalError |= fError;
        }
    }

    if (fRelayout) {
        // Force a relayout. This code is to by pass a bad behaviour.
        // When we change the visibility status (hide/show) of error messages
        // they are not properly drawn in their right positions. So we have to force a
        // relayout
        GetSizer()->CalcMin();
        GetSizer()->Layout();
        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
        //Now the panel is properly drawn but its width changes and the splitter windows
        //doesn't get aware. It is necessary to force a redraw of the splitter window
        m_pParent->SetSashPosition(m_pParent->GetSashPosition(), true);
        //OK. Now it works.
    }

    return fLocalError;
}

//---------------------------------------------------------------------------------------
void InternetOptPanel::Apply()
{
    // Web updates options
    // AWARE: here it is not safe to use translation macro _() , as language could
    // have been changed and displayed strings in m_pCboCheckFreq are in a different
    // laguage. SO DO NOT USE m_pCboCheckFreq->GetValue()
    int nCheckFreq = m_pCboCheckFreq->GetSelection();
    wxString sValue;
    //prefs. value is always stored in English
    if (nCheckFreq == 0)    //Never
        sValue = "Never";
    else if (nCheckFreq == 1)   //Daily
        sValue = "Daily";
    else if (nCheckFreq == 2)   //Weekly
        sValue = "Weekly";
    else if (nCheckFreq == 3)   //Monthly
        sValue = "Monthly";
    else {
        sValue = "Weekly";      //assume weekly
        LOMSE_LOG_ERROR("Invalid selection in CboCheckFreq (%d)", nCheckFreq);
    }
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Write("/Options/CheckForUpdates/Frequency", sValue);

    // proxy settings
    bool fUseProxy = m_pChkUseProxy->IsChecked();
    pPrefs->Write("/Internet/UseProxy", fUseProxy);

    if (fUseProxy)
    {
        pPrefs->Write("/Internet/Hostname", m_pTxtHostname->GetValue());
        pPrefs->Write("/Internet/PortNumber", m_pTxtPortNumber->GetValue());
        bool fAuthentication = m_pChkProxyAuthentication->IsChecked();
        pPrefs->Write("/Internet/ProxyAuthentication", fAuthentication);
        if (fAuthentication)
        {
            pPrefs->Write("/Internet/Username", m_pTxtUsername->GetValue());
            pPrefs->Write("/Internet/Password", m_pTxtPassword->GetValue());
        }
        else
        {
            pPrefs->Write("/Internet/Username", "");
            pPrefs->Write("/Internet/Password", "");
        }
    }
    else
    {
        pPrefs->Write("/Internet/Hostname", "");
        pPrefs->Write("/Internet/PortNumber", "");
        pPrefs->Write("/Internet/ProxyAuthentication", false);
        pPrefs->Write("/Internet/Username", "");
        pPrefs->Write("/Internet/Password", "");
    }

}

//---------------------------------------------------------------------------------------
void InternetOptPanel::OnChkUseProxyClicked(wxCommandEvent& event)
{
    EnableProxySettings(  event.IsChecked() );
    Verify();
}

//---------------------------------------------------------------------------------------
void InternetOptPanel::OnChkProxyAuthenticationClicked(wxCommandEvent& event)
{
    EnableProxyAuthentication(  event.IsChecked() );
    Verify();
}

//---------------------------------------------------------------------------------------
void InternetOptPanel::EnableProxySettings(bool fEnabled)
{
    m_pBoxProxySettings->GetStaticBox()->Enable(fEnabled);

    m_pBoxServerSettings->GetStaticBox()->Enable(fEnabled);
    m_pLblHostname->Enable(fEnabled);
    m_pTxtHostname->Enable(fEnabled);
    m_pLblPortNumber->Enable(fEnabled);
    m_pTxtPortNumber->Enable(fEnabled);

    m_pChkProxyAuthentication->Enable(fEnabled);

    EnableProxyAuthentication(fEnabled && m_pChkProxyAuthentication->IsChecked());

}

//---------------------------------------------------------------------------------------
void InternetOptPanel::EnableProxyAuthentication(bool fEnabled)
{
    m_pBoxProxyAuthentication->GetStaticBox()->Enable(fEnabled);
    m_pLblUsername->Enable(fEnabled);
    m_pTxtUsername->Enable(fEnabled);
    m_pLblPassword->Enable(fEnabled);
    m_pTxtPassword->Enable(fEnabled);

}


}   //namespace lenmus
