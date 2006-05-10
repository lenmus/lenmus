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



#include "Updater.h"
#include "AboutDialog.h"       // to get this version string

//access to error's logger
#include "Logger.h"
extern lmLogger* g_pLogger;

//access to user preferences
#include "Preferences.h"


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



bool lmUpdater::CheckForUpdates(wxString sPlatform)
{
    if (!m_fCheckForUpdates) return false;

    m_sPlatform = sPlatform;

    //connect to URL

    //download UpdateData.txt

    //if error display error msge. Suggest posible firewall problem. return false;

    //load file into memory stream

    //wxXmlDocument load from memory stream
    wxXmlDocument oDoc;
    //if (!oDoc.Load(inputstream)) 
    wxString sFilename = _T("c:\\usr\\desarrollo_wx\\lenmus\\docs\\src\\UpdateData.txt");
    if (!oDoc.Load(sFilename) )
    {
        g_pLogger->ReportProblem(_("Error loading XML file ") + sFilename);
        return false;
    }

    //Verify type of document. Must be <UpdateData>
    wxXmlNode *pRoot = oDoc.GetRoot();
    if (pRoot->GetName() != _T("UpdateData")) {
        g_pLogger->ReportProblem(
            _("Error. <%s> files are not supported"), pRoot->GetName() );
        return false;
    }
    
    //analyze document and extract information for this platform
    ParseDocument(pRoot);

    m_fNeedsUpdate = (m_sVersion != _T("") && m_sVersion != LM_VERSION_STR);

    return m_fNeedsUpdate;

}

void lmUpdater::ParseDocument(wxXmlNode* pNode)
{
    //initialize results
    m_sVersion = _T("");
    m_sDescription = _T("");

    //start parsing. Loop to find <Platform> tag for this platform
    g_pLogger->LogTrace(_T("lmUpdater"),
        _T("[lmUpdater::ParseDocument] Starting the parser"));
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;
    
    while (pElement) {
        if (pElement->GetName() != _T("Platform")) {
            g_pLogger->LogError(
                _T("[lmUpdater::ParseDocument] Expected tag <Platform> but found <%s>"),
                pElement->GetName() );
            return;
        }
        else {
            //if this platform found analyze it and finish
            if (GetAttribute(pElement, _T("name"), _T("")) == m_sPlatform) {
                g_pLogger->LogTrace(_T("lmUpdater"),
                    _T("[lmUpdater::ParseDocument] Analyzing data for <Platform name='%s'>"),
                    m_sPlatform );

                //find first child
                pNode = GetFirstChild(pNode);
                pElement = pNode;
                  
                while (pElement) {
                    if (pElement->GetName() == _T("Version")) {
                        m_sVersion = GetText(pElement);
                    }
                    else if (pElement->GetName() == _T("Description")) {
                        m_sDescription = GetText(pElement);
                        // replace "{" and "}" by "<" and ">", respectively
                        m_sDescription.Replace(_T("{"), _T("<"));
                        m_sDescription.Replace(_T("}"), _T(">"));
                    }
                    else {
                        g_pLogger->LogError(
                            _T("[lmUpdater::ParseDocument] Expected tag <Version> or <Description> but found <%s>"),
                            pElement->GetName() );
                    }

                    // Find next sibling 
                    pNode = GetNextSibling(pNode);
                    pElement = pNode;
                }
                return;     //done
            }
            //it is not this platform. Continue with next <Platform> tag
        }

        // Find next next <Platform> tag
        pNode = GetNextSibling(pNode);
        pElement = pNode;
    
    }

}


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


