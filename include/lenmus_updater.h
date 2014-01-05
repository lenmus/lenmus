//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_UPDATER_H__        //to avoid nested includes
#define __LENMUS_UPDATER_H__

#include "lenmus_injectors.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/xml/xml.h>          // to use wxXmlDocument

namespace lenmus
{

//---------------------------------------------------------------------------------------
class Updater
{
protected:
    ApplicationScope& m_appScope;
    wxFrame*    m_pParent;
    bool        m_fCheckForUpdates;     //user allows this checking
    wxString    m_sPlatform;
    wxString    m_sVersion;
    wxString    m_sPackage;
    wxString    m_sDescription;
    wxString    m_sUrl;                 //url for download
    bool        m_fNeedsUpdate;

public:
    Updater(ApplicationScope& appScope);
    ~Updater();

    void check_for_updates(wxFrame* pParent, bool fSilent);
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

};


}   // namespace lenmus

#endif    // __LENMUS_UPDATER_H__

