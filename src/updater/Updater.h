//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_UPDATER_H__        //to avoid nested includes
#define __LM_UPDATER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Updater.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xml/xml.h"          // to use wxXmlDocument

// values for flag fSilent (method CheckForUpdates)
#define lmNOT_SILENT    false
#define lmSILENT        true


class lmUpdater
{
public:
    lmUpdater();
    ~lmUpdater();

    //actions
    void CheckForUpdates(wxFrame* pParent, bool fSilent);
    bool DownloadFiles();
    const wxString& GetVersion() { return m_sVersion; }
    const wxString& GetPackage() { return m_sPackage; }
    const wxString& GetDescription() { return m_sDescription; }
    const wxString& GetUrl() { return m_sUrl; }

private:
    //default values and user preferences
    void LoadUserPreferences();
    void SaveUserPreferences();

    bool DoCheck(wxString sPlatform, bool fSilent);     //true if errors in conection or update information download

    bool CheckInternetConnection();     //true if internet connection operative

    void ParseDocument(wxXmlNode* pNode);
    wxString GetText(wxXmlNode* pElement);
    wxString GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault);
    wxXmlNode* GetFirstChild(wxXmlNode* pNode);
    wxXmlNode* GetNextSibling(wxXmlNode* pNode);




        //
        // member variables
        //

    wxFrame*    m_pParent;
    bool        m_fCheckForUpdates;     //user allows this checking
    wxString    m_sPlatform;
    wxString    m_sVersion;
    wxString    m_sPackage;
    wxString    m_sDescription;
    wxString    m_sUrl;                 //url for download
    bool        m_fNeedsUpdate;

};

//global function
bool LaunchDefaultBrowser(const wxString& url);



#endif    // __LM_UPDATER_H__

