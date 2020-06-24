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

//lenmus headers
#include "lenmus_updater.h"

#include "lenmus_msg_box.h"
#include "lenmus_updater_dlg.h"
#include "lenmus_paths.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets headers
#include <wx/dialup.h>
#include <wx/wfstream.h>
#include <wx/protocol/http.h>
#include <wx/mstream.h>         //to use files in memory
#include <wx/url.h>
#include <wx/datetime.h>        //to get and save the date of last successful check
#include <wx/mimetype.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>

namespace lenmus
{

//=======================================================================================
// Updater implementation
//=======================================================================================
Updater::Updater(ApplicationScope& appScope)
    : m_appScope(appScope)
    , m_pParent(nullptr)
    , m_fCheckForUpdates(false)
    , m_sPlatform(wxEmptyString)
    , m_sVersion(wxEmptyString)
    , m_sPackage(wxEmptyString)
    , m_sDescription(wxEmptyString)
    , m_sUrl(wxEmptyString)
    , m_fNeedsUpdate(false)
{
}

//---------------------------------------------------------------------------------------
Updater::~Updater()
{
}

//---------------------------------------------------------------------------------------
void Updater::LoadUserPreferences()
{
    //load settings form user congiguration data or default values
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Read("/Internet/CheckForUpdates", &m_fCheckForUpdates, true );

}

//---------------------------------------------------------------------------------------
void Updater::SaveUserPreferences()
{
    //save settings in user congiguration data
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Write("/Internet/CheckForUpdates", m_fCheckForUpdates );

}

//---------------------------------------------------------------------------------------
//true if errors in conection or update information download
bool Updater::DoCheck(wxString sPlatform, bool fSilent)
{
    m_sPlatform = sPlatform;

    wxXmlDocument oDoc;

    //for developing and testing do not connect to internet (it implies firewall
    //rules modification each time I recompile) unless explicitly selected 'Release
    //behaviour' in Debug menu.
    if (m_appScope.is_release_behaviour())
    {
        //Release behaviour. Access to internet

        //verify if internet is available
        if (!CheckInternetConnection())
        {
            if (!fSilent)
            {
                wxString sEmpty = "";
                wxString sMsg = sEmpty +
                        _("You are not connected to internet!") + "\n\n" +
                        _("To check for updates LenMus needs internet connection.") + "\n" +
                        _("Please, connect to internet and then retry.");
                ErrorBox oEB(sMsg, "Close this error window");
                oEB.ShowModal();
            }
            return true;
        }

        //wxString sUrl = "http://localhost/sw/UpdateData.xml";
#if (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)
        wxString sUrl = "http://www.lenmus.org/sw/UpdateData.xml";
#else
        wxString sUrl = "http://www.lenmus.org/sw/UpdateData-linux.xml";
#endif

//-----------------------------------
     ////Old code using wxHTTP

     //   // ensure we can build a wxURL object from the given URL
     //   wxURL oURL(sUrl);
     //   wxASSERT( oURL.GetError() == wxURL_NOERR);
     //
     //   //Setup user-agent string to be identified not as a bot but as a browser
     //   wxProtocol& oProt = oURL.GetProtocol();
     //   wxHTTP* pHTTP = (wxHTTP*)&oProt;
     //   pHTTP->SetHeader("User-Agent", "LenMus Phonascus updater");
     //   oProt.SetTimeout(10);              // 90 sec

     //   //create the input stream by establishing http connection
     //   wxInputStream* pInput = oURL.GetInputStream();

//-----------------------------------
    //// New code using wxHttpEngine

    //    wxHTTPBuilder oHttp;
    //    oHttp.InitContentTypes(); // Initialise the content types on the page

    //    //get proxy options
    //    ProxySettings* pSettings = GetProxySettings();

    //    if (pSettings->fUseProxy) {
    //        oHttp.HttpProxy(pSettings->sProxyHostname, pSettings->nProxyPort );
    //        if (pSettings->fRequiresAuth)
    //            oHttp.HttpProxyAuth( pSettings->sProxyUsername, pSettings->sProxyPassword);
    //    }

    //    //Setup user-agent string to be identified not as a bot but as a browser
    //    oHttp.SetHeader("User-Agent", "LenMus Phonascus updater");
    //    oHttp.SetTimeout(10);              // 10 sec

    //    wxInputStream* pInput = oHttp.GetInputStream( sUrl );

//-----------------------------------
    // New code using wxHTTP

        // ensure we can build a wxURL object from the given URL
        wxURL oURL(sUrl);
        wxASSERT( oURL.GetError() == wxURL_NOERR);

        //Setup user-agent string to be identified not as a bot but as a browser
        wxProtocol& oProt = oURL.GetProtocol();
        wxHTTP* pHTTP = (wxHTTP*)&oProt;
        pHTTP->SetHeader("User-Agent", "LenMus_updater");
        oProt.SetTimeout(10);              // 10 secs.

        //get proxy options
        ProxySettings* pSettings = m_appScope.get_proxy_settings();

        // If a proxy is used, set it
        if (pSettings->fUseProxy)
        {
            pHTTP->SetProxyMode(true);
            // user:pass@host:port
            wxString sProxyUrl = "http://";
            if (pSettings->fRequiresAuth)
            {
                sProxyUrl += pSettings->sProxyUsername + ":" +
                             pSettings->sProxyPassword + "@";
            }
            sProxyUrl += pSettings->sProxyHostname  +
                wxString::Format( ":%d", pSettings->nProxyPort );
            oURL.SetProxy( sProxyUrl );
        }

        //create the input stream by establishing http connection
        wxInputStream* pInput = oURL.GetInputStream();


//-----------------------------------
        if (!pInput || !pInput->IsOk())
        {
            if (pInput)
                delete pInput;
            //wxLogMessage( oHttp.GetLastError() );
            wxString sEmpty = "";
            wxString sMsg = sEmpty + _("Error checking for updates") + "\n\n" +
_("A connection with the server could not be established. \
Check that you are connected to the internet and that no firewalls are blocking \
this program; then try again. If problems persist, the server \
may be down. Please, try again later.");
            ErrorBox oEB(sMsg, "Close this error window");
            oEB.ShowModal();
            return true;
        }

        //download updates data file and analyze it
        if (!oDoc.Load(*pInput))
        {
            LOMSE_LOG_ERROR("Problem: Error loading XML file ");
            return true;
        }

    }

    else
    {
        //Debug behaviour. Instead of accesing Internet use local files
        wxFileName oFN(m_appScope.get_paths()->GetSrcRootPath(), "TestUpdateData.xml");
        wxString sFilename = oFN.GetFullPath();
        if (!oDoc.Load(sFilename) )
        {
            LOMSE_LOG_ERROR("Problem: Error loading XML file ");
            return true;
        }
    }

    //Verify type of document. Must be <UpdateData>
    wxXmlNode *pRoot = oDoc.GetRoot();
    if (pRoot->GetName() != "UpdateData")
    {
        wxLogMessage("[Updater::DoCheck] Error. <%s> files are not supported", pRoot->GetName().wx_str() );
        return true;
    }

    //analyze document and extract information for this platform
    ParseDocument(pRoot);

    m_fNeedsUpdate = (m_sVersion != "" &&
                      m_sVersion != m_appScope.get_version_string());

    return false;       //no errors
}

//---------------------------------------------------------------------------------------
bool Updater::CheckInternetConnection()
{
#if (LENMUS_PLATFORM_MAC == 0)
    bool fConnected = false;    //assume not connected

    // check connection
    wxDialUpManager* pManager = wxDialUpManager::Create();
    if (pManager->IsOk()) {
        fConnected = pManager->IsOnline();
    }

    delete pManager;
    return fConnected;
#else
    return true;
#endif
}

//---------------------------------------------------------------------------------------
void Updater::ParseDocument(wxXmlNode* pNode)
{
    //initialize results
    m_sVersion = "";
    m_sDescription = "";
    m_sPackage = "No package!";

    //start parsing. Loop to find <platform> tag for this platform
//    wxLogMessage("[Updater::ParseDocument] Trace: Starting the parser");
    pNode = GetFirstChild(pNode);
    wxXmlNode* pElement = pNode;

    while (pElement) {
        if (pElement->GetName() != "platform")
        {
            LOMSE_LOG_ERROR("Error: Expected tag <platform> but found <%s>",
                            pElement->GetName().ToStdString().c_str() );
            return;
        }
        else {
            //if this platform found analyze it and finish
            if (GetAttribute(pElement, "name", "") == m_sPlatform)
            {
//                wxLogMessage("[Updater::ParseDocument] Trace: Analyzing data for <platform name='%s'>",
//                    m_sPlatform.wx_str() );

                //find first child
                pNode = GetFirstChild(pNode);
                pElement = pNode;

                while (pElement) {
                    if (pElement->GetName() == "version") {
                        m_sVersion = GetText(pElement);
                    }
                    else if (pElement->GetName() == "package") {
                        m_sPackage = GetText(pElement);
                    }
                    else if (pElement->GetName() == "description") {
                        m_sDescription = GetText(pElement);
                    }
                    else if (pElement->GetName() == "download_url") {
                        m_sUrl = GetText(pElement);
                    }
                    else
                    {
                        LOMSE_LOG_ERROR(
                            "Error: Expected tag <version> or <description> but found <%s>",
                            pElement->GetName().ToStdString().c_str() );
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

//---------------------------------------------------------------------------------------
void Updater::check_for_updates(wxFrame* pParent, bool fSilent)
{
    if (!m_fCheckForUpdates) return;

    //initializations
    m_pParent = pParent;
    m_fNeedsUpdate = false;
    ::wxBeginBusyCursor();

    //Inform user and ask permision to proceed
    if (!fSilent) {
        UpdaterDlgStart dlgStart(m_pParent);
        if (dlgStart.ShowModal() == wxID_CANCEL) {
            ::wxEndBusyCursor();
            return;  //updater canceled
        }
    }

    //conect to server and get information about updates
#if (LENMUS_PLATFORM_WIN32 == 1)
    if (DoCheck("Win32", fSilent)) {
#else
    if (DoCheck("Linux", fSilent)) {
#endif
        ::wxEndBusyCursor();
        return;  //Error. Not posible to do check
    }

    //inform about results and remove busy cursor
    if (!m_fNeedsUpdate) {
        //no updates available
        ::wxEndBusyCursor();
        if (!fSilent) wxMessageBox(_("No updates available."));
   }
    else {
        //update available. Create and show informative dialog
        UpdaterDlgInfo dlg(m_pParent, this);
        dlg.AddPackage(GetPackage(), GetDescription());
        ::wxEndBusyCursor();
        dlg.ShowModal();
    }

    //save the date of last successful check
    wxString sLastCheckDate = (wxDateTime::Now()).Format("%x");
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Write("/Options/CheckForUpdates/LastCheck", sLastCheckDate);

}

//---------------------------------------------------------------------------------------
bool Updater::DownloadFiles()
{
    ::wxLaunchDefaultBrowser(m_sUrl);
    return false;
}

//---------------------------------------------------------------------------------------
wxString Updater::GetText(wxXmlNode* pElement)
{
    //    Receives node of type ELEMENT and returns its text content
    wxASSERT(pElement->GetType() == wxXML_ELEMENT_NODE);

    wxXmlNode* pNode = pElement->GetChildren();
    wxString sName = pElement->GetName();
    wxString sValue = "";

    if (pNode->GetType() == wxXML_TEXT_NODE) {
        sValue = pNode->GetContent();
    }
    return sValue;
}

//---------------------------------------------------------------------------------------
wxString Updater::GetAttribute(wxXmlNode* pNode, wxString sName, wxString sDefault)
{
    wxXmlAttribute* pAttrib = pNode->GetAttributes();
    while(pAttrib) {
        if (pAttrib->GetName() == sName)
            return pAttrib->GetValue();
        pAttrib = pAttrib->GetNext();
    }

    return sDefault;
}

//---------------------------------------------------------------------------------------
wxXmlNode* Updater::GetNextSibling(wxXmlNode* pNode)
{
    // Return next sibling element or nullptr if no more
    pNode = pNode->GetNext();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;

}
//---------------------------------------------------------------------------------------
wxXmlNode* Updater::GetFirstChild(wxXmlNode* pNode)
{
    // Return first child element or nullptr if no more
    pNode = pNode->GetChildren();
    while (pNode && pNode->GetType() != wxXML_ELEMENT_NODE)
        pNode = pNode->GetNext();
    return pNode;
}


}   //namespace lenmus
