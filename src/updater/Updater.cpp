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
/*! @file Updater.cpp
    @brief Implementation file for class lmUpdater
    @ingroup updates_management
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Updater.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/dialup.h>


#include "Updater.h"
#include "../app/AboutDialog.h"       // to get this version string
#include "../app/ErrorDlg.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to user preferences
#include "../app/Preferences.h"

#include "UpdaterDlg.h"


//-------------------------------------------------------------------------------------------
// lmUpdater implementation
//-------------------------------------------------------------------------------------------

lmUpdater::lmUpdater()
{
}

lmUpdater::~lmUpdater()
{
}

void lmUpdater::LoadUserPreferences()
{
    //load settings form user congiguration data or default values
    g_pPrefs->Read(_T("/Internet/CheckForUpdates"), &m_fCheckForUpdates, true );

}

void lmUpdater::SaveUserPreferences()
{
    //save settings in user congiguration data
    g_pPrefs->Write(_T("/Internet/CheckForUpdates"), m_fCheckForUpdates );   

}


//true if errors in conection or update information download
bool lmUpdater::DoCheck(wxString sPlatform, bool fSilent)
{
    m_sPlatform = sPlatform;

    if (!fSilent) {
        //open dlg to show connection status
    }

    //verify if internet is available
    if (!CheckInternetConnection()) {
        lmErrorDlg dlg(m_pParent, _("Error"), _("You are not connected to internet!\n\n \
To check for updates LenMus needs internet connection. \n \
Please, connect to internet and then retry."));
        dlg.ShowModal();
        return true;
    }

    ////create download thread and download UpdateData.txt
    //m_dThread = new lmDownloadThread(this);
    //if (m_dThread->Create() != wxTHREAD_NO_ERROR ||
    //    m_dThread->Run() != wxTHREAD_NO_ERROR) {
    //    wxMessageBox(wxT("Low resources; cannot run the WebUpdate dialog...\n")
    //        wxT("Close some applications and then retry."),
    //        wxT("Error"), wxOK | wxICON_ERROR);
    //    wxLogAdvMsg(wxT("wxWebUpdateDlg::InitThreads - cannot run the download thread !"));
    //    return true;
    //}


    //if error display message. Suggest posible firewall problem and terminate
    if (false) {
        lmErrorDlg dlg(m_pParent, _("Error"), _("Connection with the server could \
not be established. \
Check that you are connected to the internet and that no firewalls are blocking \
this program; then try again. If problems continue, the server \
may be down. Please, try again later."));
        dlg.ShowModal();
        return true;
    }

    //load file into memory stream

    //wxXmlDocument load from memory stream
    wxXmlDocument oDoc;
    //if (!oDoc.Load(inputstream)) 
    wxString sFilename = _T("c:\\usr\\desarrollo_wx\\lenmus\\docs\\src\\UpdateData.txt");
    if (!oDoc.Load(sFilename) )
    {
        g_pLogger->ReportProblem(_("Error loading XML file ") + sFilename);
        return true;
    }

    //Verify type of document. Must be <UpdateData>
    wxXmlNode *pRoot = oDoc.GetRoot();
    if (pRoot->GetName() != _T("UpdateData")) {
        g_pLogger->ReportProblem(
            _("Error. <%s> files are not supported"), pRoot->GetName() );
        return true;
    }
    
    //analyze document and extract information for this platform
    ParseDocument(pRoot);

    m_fNeedsUpdate = (m_sVersion != _T("") && m_sVersion != LM_VERSION_STR);

    return false;       //no errors

}

bool lmUpdater::CheckInternetConnection()
{
    bool fConnected = false;    //assume not connected

    // check connection
    wxDialUpManager* pManager = wxDialUpManager::Create();
    if (pManager->IsOk()) {
        fConnected = pManager->IsOnline();
    }

    delete pManager;
    return fConnected;
}


void lmUpdater::ParseDocument(wxXmlNode* pNode)
{
    //initialize results
    m_sVersion = _T("");
    m_sDescription = _T("");

    //start parsing. Loop to find <platform> tag for this platform
    g_pLogger->LogTrace(_T("lmUpdater"),
        _T("[lmUpdater::ParseDocument] Starting the parser"));
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    
    while (pElement) {
        if (pElement->GetName() != _T("platform")) {
            g_pLogger->LogError(
                _T("[lmUpdater::ParseDocument] Expected tag <platform> but found <%s>"),
                pElement->GetName() );
            return;
        }
        else {
            //if this platform found analyze it and finish
            if (GetAttribute(pElement, _T("name"), _T("")) == m_sPlatform) {
                g_pLogger->LogTrace(_T("lmUpdater"),
                    _T("[lmUpdater::ParseDocument] Analyzing data for <platform name='%s'>"),
                    m_sPlatform );

                //find first child
                pNode = GetFirstChild(pNode);
                pElement = pNode;
                  
                while (pElement) {
                    if (pElement->GetName() == _T("version")) {
                        m_sVersion = GetText(pElement);
                    }
                    else if (pElement->GetName() == _T("description")) {
                        m_sDescription = GetText(pElement);
                        // replace "{" and "}" by "<" and ">", respectively
                        m_sDescription.Replace(_T("{"), _T("<"));
                        m_sDescription.Replace(_T("}"), _T(">"));
                    }
                    else if (pElement->GetName() == _T("download_url")) {
                        m_sUrl = GetText(pElement);
                    }
                    else {
                        g_pLogger->LogError(
                            _T("[lmUpdater::ParseDocument] Expected tag <version> or <description> but found <%s>"),
                            pElement->GetName() );
                    }

                    // Find next sibling 
                    pNode = GetNextSibling(pNode);
                    pElement = pNode;
                }
                return;     //done
            }
            //it is not this platform. Continue with next <platform> tag
        }

        // Find next next <platform> tag
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    
    }

}


void lmUpdater::CheckForUpdates(wxFrame* pParent, bool fSilent)
{
    if (!m_fCheckForUpdates) return;

    //initializations
    m_pParent = pParent;
    m_fNeedsUpdate = false;
    
    //conect to server and get information about updates
    if (DoCheck(_T("Win32"), fSilent)) {
        //Error. Not posible to do check
        return;
    }

    //inform about results
    if (!m_fNeedsUpdate) {
        //no updates available
        if (!fSilent) wxMessageBox(_T("No updates available."));
    }
    else {
        //update available. Create and show dialog
        lmUpdaterDlg dlg(m_pParent, this);
        dlg.ShowModal();
    }

}

//--------------------------------------------------------------------------------------
// Copied from XMLParser.cpp
// If you need to fix something here check if the fix is also applicable there
//--------------------------------------------------------------------------------------

// Copied from XMLParser.cpp
// If you need to fix something here check if the fix is also applicable there
wxString lmUpdater::GetText(wxXmlNode* pElement)
{
    //    Receives node of type ELEMENT and returns its text content
    wxASSERT(pElement->GetType() == wxXML_ELEMENT_NODE);

    wxXmlNode* pNode = pElement->GetChildren();
    wxString sName = pElement->GetName();
    wxString sValue = _T("");
    
    if (pNode->GetType() == wxXML_TEXT_NODE) {
        sValue = pNode->GetContent();
    }
    return sValue;
}

// Copied from XMLParser.cpp
// If you need to fix something here check if the fix is also applicable there
wxString lmUpdater::GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault)
{
    wxXmlProperty* pAttrib = pNode->GetProperties();
    while(pAttrib) {
        if (pAttrib->GetName() == sName)
            return pAttrib->GetValue();
        pAttrib = pAttrib->GetNext();
    }

    return sDefault;
}

wxXmlNode* lmUpdater::GetNextSibling(wxXmlNode* pNode)
{
    // Return next sibling element or NULL if no more
    pNode = pNode->GetNext();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;

}
wxXmlNode* lmUpdater::GetFirstChild(wxXmlNode* pNode)
{
    // Return first child element or NULL if no more
    pNode = pNode->GetChildren();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;
}

