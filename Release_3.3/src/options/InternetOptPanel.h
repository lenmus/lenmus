//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file InternetOptPanel.h
    @brief Header file for class lmInternetOptPanel
    @ingroup options_management
*/
#ifndef __INTERNETOPTPANEL_H__
#define __INTERNETOPTPANEL_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "InternetOptPanel.cpp"
#endif

#include "OptionsPanel.h"
#include "wx/splitter.h"


class lmInternetOptPanel: public lmOptionsPanel
{    
public:
    lmInternetOptPanel(wxWindow* parent);
    ~lmInternetOptPanel();
    bool Verify();
    void Apply();

    //event handlers
    void OnChkUseProxyClicked(wxCommandEvent& event);
    void OnChkProxyAuthenticationClicked(wxCommandEvent& event);
    void OnDataChanged(wxCommandEvent& WXUNUSED(event)) { Verify(); }


private:
    void EnableProxySettings(bool fEnabled);
    void EnableProxyAuthentication(bool fEnabled);

    
    // controls
        // Web updates
    wxStaticText*   m_pTxtLastCheck;
    wxComboBox*     m_pCboCheckFreq;
        // Proxy settings
    wxCheckBox*     m_pChkUseProxy;
    wxStaticBox*    m_pBoxProxySettings;
    wxStaticBox*    m_pBoxServerSettings;
    wxTextCtrl*     m_pTxtHostname;
    wxTextCtrl*     m_TxtPortNumber;
    wxCheckBox*     m_pChkProxyAuthentication;
    wxStaticBox*    m_pBoxProxyAuthentication;
    wxTextCtrl*     m_pTxtUsername;
    wxTextCtrl*     m_pTxtPassword;
    wxStaticText*   m_pLblUsername;
    wxStaticText*   m_pLblPassword;
    wxStaticText*   m_pLblHostname;
    wxStaticText*   m_pLblPortNumber;

    //error labels and bitmaps
    wxStaticText*   m_pLblServerSettingsError;
    wxStaticBitmap* m_pBmpServerSettingsError;
    wxStaticText*   m_pLblAuthenticationError;
    wxStaticBitmap* m_pBmpAuthenticationError;

    //to detect that it is the first time we display an error message
    bool            m_fFirstTimeServerSettingsError;   
    bool            m_fFirstTimeAuthenticationError;   


    // other member variables
    bool                m_fCheckForUpdates;
    wxSplitterWindow*   m_pParent;

    DECLARE_EVENT_TABLE()

};

#endif    // __INTERNETOPTPANEL_H__
