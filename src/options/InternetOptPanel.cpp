//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
#pragma implementation "InternetOptPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xrc/xmlres.h"

#include "InternetOptPanel.h"

//access to preferences object
#include "wx/config.h"
extern wxConfigBase* g_pPrefs;

//access to user preferences
#include "../app/Preferences.h"



BEGIN_EVENT_TABLE(lmInternetOptPanel, wxPanel)
    EVT_CHECKBOX( XRCID( "chkUseProxy" ), lmInternetOptPanel::OnChkUseProxyClicked )
    EVT_CHECKBOX( XRCID( "chkProxyAuthentication" ), lmInternetOptPanel::OnChkProxyAuthenticationClicked )
    EVT_TEXT( XRCID( "txtHostname" ), lmInternetOptPanel::OnDataChanged )
    EVT_TEXT( XRCID( "txtPortNumber" ), lmInternetOptPanel::OnDataChanged )
    EVT_TEXT( XRCID( "txtUsername" ), lmInternetOptPanel::OnDataChanged )
    EVT_TEXT( XRCID( "txtPassword" ), lmInternetOptPanel::OnDataChanged )
END_EVENT_TABLE()


lmInternetOptPanel::lmInternetOptPanel(wxWindow* parent)
{
    // initializations
    m_pParent = (wxSplitterWindow*)parent;
    m_fFirstTimeServerSettingsError = true;
    m_fFirstTimeAuthenticationError = true;

    // create the panel
    wxXmlResource::Get()->LoadPanel(this, parent, _T("InternetOptPanel"));

    //load icon
    wxStaticBitmap* pBmpIcon = XRCCTRL(*this, "bmpIconTitle", wxStaticBitmap);
    pBmpIcon->SetBitmap( wxArtProvider::GetIcon(_T("opt_internet"), wxART_TOOLBAR, wxSize(24,24)) );

    //store pointers to controls
    m_pCboCheckFreq = XRCCTRL(*this, "cboCheckFreq", wxComboBox);
    m_pTxtLastCheck = XRCCTRL(*this, "txtLastCheckDate", wxStaticText);

    m_pChkUseProxy = XRCCTRL(*this, "chkUseProxy", wxCheckBox);
    m_pBoxProxySettings = XRCCTRL(*this, "boxProxySettings", wxStaticBox);
    m_pBoxServerSettings = XRCCTRL(*this, "boxServerSettings", wxStaticBox);
    m_pTxtHostname = XRCCTRL(*this, "txtHostname", wxTextCtrl);
    m_TxtPortNumber = XRCCTRL(*this, "txtPortNumber", wxTextCtrl);
    m_pChkProxyAuthentication = XRCCTRL(*this, "chkProxyAuthentication", wxCheckBox);
    m_pBoxProxyAuthentication = XRCCTRL(*this, "boxProxyAuthentication", wxStaticBox);
    m_pTxtUsername = XRCCTRL(*this, "txtUsername", wxTextCtrl);
    m_pTxtPassword = XRCCTRL(*this, "txtPassword", wxTextCtrl);

    m_pLblUsername = XRCCTRL(*this, "lblUsername", wxStaticText);
    m_pLblPassword = XRCCTRL(*this, "lblPassword", wxStaticText);
    m_pLblHostname = XRCCTRL(*this, "lblHostname", wxStaticText);
    m_pLblPortNumber = XRCCTRL(*this, "lblPortNumber", wxStaticText);

    // error messages and bitmaps
    m_pLblServerSettingsError = XRCCTRL(*this, "lblServerSettingsError", wxStaticText);
    m_pBmpServerSettingsError = XRCCTRL(*this, "bmpServerSettingsError", wxStaticBitmap);
    m_pLblAuthenticationError = XRCCTRL(*this, "lblAuthenticationError", wxStaticText);
    m_pBmpAuthenticationError = XRCCTRL(*this, "bmpAuthenticationError", wxStaticBitmap);

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, wxSize(16,16));
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
    wxString sCheckFreq = g_pPrefs->Read(_T("/Options/CheckForUpdates/Frequency"), _T("Weekly") );
    if (sCheckFreq == _T("Never"))
        m_pCboCheckFreq->SetSelection(0);
    else if (sCheckFreq == _T("Daily"))
        m_pCboCheckFreq->SetSelection(1);
    else if (sCheckFreq == _T("Weekly"))
        m_pCboCheckFreq->SetSelection(2);
    else if (sCheckFreq == _T("Monthly"))
        m_pCboCheckFreq->SetSelection(3);
    else {
        m_pCboCheckFreq->SetSelection(2);       // assume weekly
        wxLogMessage(_T("[lmInternetOptPanel] Invalid value in ini file. Key '/Options/CheckForUpdates/Frequency', value='%s'"),
            sCheckFreq.c_str() );
    }

    // display web update last check date
    wxString sLastCheckDate = g_pPrefs->Read(_T("/Options/CheckForUpdates/LastCheck"), _T(""));
    if (sLastCheckDate == _T("")) {
        sLastCheckDate = _("Never");
    }
    m_pTxtLastCheck->SetLabel(sLastCheckDate);

    //proxy settings
    lmProxySettings* pSettings = GetProxySettings();
    m_pChkUseProxy->SetValue( pSettings->fUseProxy );
    EnableProxySettings( pSettings->fUseProxy );
    m_pTxtHostname->SetValue( pSettings->sProxyHostname );
    m_TxtPortNumber->SetValue( wxString::Format(_T("%d"), pSettings->nProxyPort) );
    m_pChkProxyAuthentication->SetValue( pSettings->fRequiresAuth );
    m_pTxtUsername->SetValue( pSettings->sProxyUsername );
    m_pTxtPassword->SetValue( pSettings->sProxyPassword );

}

lmInternetOptPanel::~lmInternetOptPanel()
{
}

bool lmInternetOptPanel::Verify()
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
        if (m_pTxtHostname->GetValue() == _T("") ||
            m_TxtPortNumber->GetValue() == _T("0") ||
            m_TxtPortNumber->GetValue() == _T("") ||
            !(m_TxtPortNumber->GetValue()).IsNumber() )
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
            if (m_pTxtUsername->GetValue() == _T("") || m_pTxtPassword->GetValue() == _T("")) {
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

void lmInternetOptPanel::Apply()
{
    // Web updates options
    // AWARE: here it is not safe to use translation macro _() , as language could
    // have been changed and displayed strings in m_pCboCheckFreq are in a different
    // laguage. SO DO NOT USE m_pCboCheckFreq->GetValue()
    int nCheckFreq = m_pCboCheckFreq->GetSelection();
    wxString sValue;
    //prefs. value is always stored in English
    if (nCheckFreq == 0)    //Never
        sValue = _T("Never");
    else if (nCheckFreq == 1)   //Daily
        sValue = _T("Daily");
    else if (nCheckFreq == 2)   //Weekly
        sValue = _T("Weekly");
    else if (nCheckFreq == 3)   //Monthly
        sValue = _T("Monthly");
    else {
        sValue = _T("Weekly");      //assume weekly
        wxLogMessage(_T("[lmInternetOptPanel::Apply()] Invalid selection in CboCheckFreq (%d)"),
            nCheckFreq );
    }
    g_pPrefs->Write(_T("/Options/CheckForUpdates/Frequency"), sValue);

    // proxy settings
    bool fUseProxy = m_pChkUseProxy->IsChecked();
    g_pPrefs->Write(_T("/Internet/UseProxy"), fUseProxy);

    if (fUseProxy) {
        g_pPrefs->Write(_T("/Internet/Hostname"), m_pTxtHostname->GetValue());
        g_pPrefs->Write(_T("/Internet/PortNumber"), m_TxtPortNumber->GetValue());
        bool fAuthentication = m_pChkProxyAuthentication->IsChecked();
        g_pPrefs->Write(_T("/Internet/ProxyAuthentication"), fAuthentication);
        if (fAuthentication) {
            g_pPrefs->Write(_T("/Internet/Username"), m_pTxtUsername->GetValue());
            g_pPrefs->Write(_T("/Internet/Password"), m_pTxtPassword->GetValue());
        }
        else {
            g_pPrefs->Write(_T("/Internet/Username"), _T(""));
            g_pPrefs->Write(_T("/Internet/Password"), _T(""));
        }
    }
    else {
        g_pPrefs->Write(_T("/Internet/Hostname"), _T(""));
        g_pPrefs->Write(_T("/Internet/PortNumber"), _T(""));
        g_pPrefs->Write(_T("/Internet/ProxyAuthentication"), false);
        g_pPrefs->Write(_T("/Internet/Username"), _T(""));
        g_pPrefs->Write(_T("/Internet/Password"), _T(""));
    }

}

void lmInternetOptPanel::OnChkUseProxyClicked(wxCommandEvent& event)
{
    EnableProxySettings(  event.IsChecked() );
    Verify();
}

void lmInternetOptPanel::OnChkProxyAuthenticationClicked(wxCommandEvent& event)
{
    EnableProxyAuthentication(  event.IsChecked() );
    Verify();
}

void lmInternetOptPanel::EnableProxySettings(bool fEnabled)
{
    m_pBoxProxySettings->Enable(fEnabled);

    m_pBoxServerSettings->Enable(fEnabled);
    m_pLblHostname->Enable(fEnabled);
    m_pTxtHostname->Enable(fEnabled);
    m_pLblPortNumber->Enable(fEnabled);
    m_TxtPortNumber->Enable(fEnabled);

    m_pChkProxyAuthentication->Enable(fEnabled);

    EnableProxyAuthentication(fEnabled && m_pChkProxyAuthentication->IsChecked());

}

void lmInternetOptPanel::EnableProxyAuthentication(bool fEnabled)
{
    m_pBoxProxyAuthentication->Enable(fEnabled);
    m_pLblUsername->Enable(fEnabled);
    m_pTxtUsername->Enable(fEnabled);
    m_pLblPassword->Enable(fEnabled);
    m_pTxtPassword->Enable(fEnabled);

}
