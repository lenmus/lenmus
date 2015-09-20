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

#ifndef __LENMUS_INTERNET_OPT_PANEL_H__
#define __LENMUS_INTERNET_OPT_PANEL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"

//wxWidgets
#include <wx/splitter.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
class InternetOptPanel : public OptionsPanel
{
public:
    InternetOptPanel(wxWindow* parent, ApplicationScope& appScope);
    ~InternetOptPanel();
    bool Verify();
    void Apply();

    //event handlers
    void OnChkUseProxyClicked(wxCommandEvent& event);
    void OnChkProxyAuthenticationClicked(wxCommandEvent& event);
    void OnDataChanged(wxCommandEvent& WXUNUSED(event)) { Verify(); }


private:
    void create_controls();
    void EnableProxySettings(bool fEnabled);
    void EnableProxyAuthentication(bool fEnabled);

    // controls
    wxPanel*        m_pTitlePanel;
    wxStaticText*   m_pLblTitle;
    wxStaticBitmap* m_pTitleIcon;
    wxStaticText*   m_pTxtCheckFreq;
    wxComboBox*     m_pCboCheckFreq;
    wxStaticText*   m_pTxtLastCheck;
    wxStaticText*   m_pTxtLastCheckDate;
    wxCheckBox*     m_pChkUseProxy;
    wxStaticText*   m_pLblHostname;
    wxTextCtrl*     m_pTxtHostname;
    wxStaticText*   m_pLblPortNumber;
    wxTextCtrl*     m_pTxtPortNumber;
    wxStaticBitmap* m_pBmpServerSettingsError;
    wxStaticText*   m_pLblSpacer1;
    wxStaticText*   m_pLblServerSettingsError;
    wxCheckBox*     m_pChkProxyAuthentication;
    wxStaticText*   m_pLblUsername;
    wxTextCtrl*     m_pTxtUsername;
    wxStaticText*   m_pLblPassword;
    wxTextCtrl*     m_pTxtPassword;
    wxStaticBitmap* m_pBmpAuthenticationError;
    wxStaticText*   m_pSpacer2;
    wxStaticText*   m_pLblAuthenticationError;

    wxStaticBoxSizer* m_pBoxProxySettings;
	wxStaticBoxSizer* m_pBoxServerSettings;
	wxStaticBoxSizer* m_pBoxProxyAuthentication;


    // other member variables
    bool                m_fCheckForUpdates;
    wxSplitterWindow*   m_pParent;

    //to detect that it is the first time we display an error message
    bool    m_fFirstTimeServerSettingsError;
    bool    m_fFirstTimeAuthenticationError;

    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif    // __LENMUS_INTERNET_OPT_PANEL_H__
