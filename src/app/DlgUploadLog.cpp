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
//    for (any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DlgUploadLog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/ffile.h>
#include <wx/mimetype.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>

#ifdef __WXMSW__
    #include "wx/evtloop.h"     // for SetCriticalWindow()
#endif // __WXMSW__


#include "DlgUploadLog.h"
#include "DlgDebug.h"

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


//for controls ID
enum
{
    lmID_VIEW_LOG = 2600,
    lmID_UPLOAD,
    lmID_CANCEL,
    lmID_SOCKET,
};

#define lmUSE_SOCKETS   0   //for uploading the log either use curl or sockets

//----------------------------------------------------------------------------------
// Helper class lmDebugReportDialog: the dialog to inform user about the upload
//----------------------------------------------------------------------------------

class lmDebugReportDialog : public wxDialog
{
public:
    lmDebugReportDialog(lmForensicLog* pDbgReport, bool fHandlingCrash);

    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();

private:
    void CreateDialog();
    void OnView(wxCommandEvent& event);
    void OnUpload(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);


    // small helper: add wxEXPAND and wxALL flags
    static wxSizerFlags SizerFlags(int proportion)
    {
        return wxSizerFlags(proportion).Expand().Border();
    }


    lmForensicLog*  m_pDbgReport;
    wxString        m_sLogFilename;

	wxStaticText* m_pLblTitle;
	wxStaticText* m_pLblExplainFile;
	wxTextCtrl* m_pTxtFileName;
	wxButton* m_pBtView;
	wxStaticText* m_pLblMoreInfo;
	wxTextCtrl* m_pTxtMoreInfo;
	wxStaticText* m_pLblNeedHelp;
	wxStaticText* m_pLblWhy;
	wxStaticText* m_pLblThanks;
	
	wxButton* m_pBtUpload;
	wxButton* m_pBtCancel;

    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------------
// lmDebugReportDialog implementation
//----------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDebugReportDialog, wxDialog)
    EVT_BUTTON(lmID_VIEW_LOG, lmDebugReportDialog::OnView)
    EVT_BUTTON(lmID_UPLOAD, lmDebugReportDialog::OnUpload)
    EVT_BUTTON(lmID_CANCEL, lmDebugReportDialog::OnCancel)
END_EVENT_TABLE()


lmDebugReportDialog::lmDebugReportDialog(lmForensicLog* pDbgReport, bool fHandlingCrash)
    : wxDialog(NULL, wxID_ANY, _("Abnormal program termination"),
               wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
    , m_pDbgReport(pDbgReport)
{
    m_sLogFilename = m_pDbgReport->GetLogFilename();
    CreateDialog();

    if (!fHandlingCrash)
    {
        //change some texts
        this->SetTitle(_("Debug log file from a previous crash"));
	    m_pLblTitle->SetLabel(_("A debug log from previous crash has been detected!"));
    }
}

void lmDebugReportDialog::CreateDialog()
{
	this->SetSizeHints( wxSize( -1,-1 ), wxSize( -1,-1 ) );
	
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* pPreviewSizer;
	pPreviewSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Debug report preview") ), wxVERTICAL );
	
	m_pLblTitle = new wxStaticText( this, wxID_ANY, _("An abnormal program termination has been detected!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblTitle->Wrap( -1 );
	m_pLblTitle->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Tahoma") ) );
	
	pPreviewSizer->Add( m_pLblTitle, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_pLblExplainFile = new wxStaticText( this, wxID_ANY, _("The program has created a text file with information about the score being edited and the action performed. This debug file will be sent to LenMus to help us to debug the program. The file to send does not contain any identification data of any nature (personal, equipment, internet address, etc). It only has the score being edited an the actions being performed. The file is at:"), wxDefaultPosition, wxSize( 550,60 ), 0 );
	m_pLblExplainFile->Wrap( -1 );
	pPreviewSizer->Add( m_pLblExplainFile, 0, wxEXPAND|wxALL, 5 );
	
	m_pTxtFileName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	pPreviewSizer->Add( m_pTxtFileName, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_pBtView = new wxButton( this, lmID_VIEW_LOG, _("View content"), wxDefaultPosition, wxDefaultSize, 0 );
	pPreviewSizer->Add( m_pBtView, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	pMainSizer->Add( pPreviewSizer, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* p_MoreInfoSizer;
	p_MoreInfoSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Additional info") ), wxVERTICAL );
	
	m_pLblMoreInfo = new wxStaticText( this, wxID_ANY, _("If you have any additional information pertaining to this bug report, please enter it here and it will be attached:"), wxDefaultPosition, wxSize( 400,-1 ), 0 );
	m_pLblMoreInfo->Wrap( -1 );
	p_MoreInfoSizer->Add( m_pLblMoreInfo, 1, wxALL|wxEXPAND, 5 );
	
	m_pTxtMoreInfo = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,60 ), wxTE_MULTILINE );
	p_MoreInfoSizer->Add( m_pTxtMoreInfo, 0, wxALL|wxEXPAND, 5 );
	
	pMainSizer->Add( p_MoreInfoSizer, 0, wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	wxBoxSizer* pFinalTextSizer;
	pFinalTextSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pLblNeedHelp = new wxStaticText( this, wxID_ANY, _("To debug the program I need your help!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLblNeedHelp->Wrap( -1 );
	m_pLblNeedHelp->SetFont( wxFont( 9, 74, 90, 92, false, wxT("Tahoma") ) );
	
	pFinalTextSizer->Add( m_pLblNeedHelp, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_pLblWhy = new wxStaticText( this, wxID_ANY, _("Take into account that most of my time is devoted to programming, not to using the program. Therefore you probably will catch more errors than me! Anyway, if you wish to suppress this debug report completely, please choose the \"Cancel\" button."), wxDefaultPosition, wxSize( 550,45 ), 0 );
	m_pLblWhy->Wrap( -1 );
	pFinalTextSizer->Add( m_pLblWhy, 0, wxALL|wxEXPAND, 5 );
	
	m_pLblThanks = new wxStaticText( this, wxID_ANY, _("Thank you for your collaboration. I'm sorry for the inconvenience."), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	m_pLblThanks->Wrap( -1 );
	pFinalTextSizer->Add( m_pLblThanks, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	pMainSizer->Add( pFinalTextSizer, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* pButtonsSizer;
	pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_pBtUpload = new wxButton( this, lmID_UPLOAD, _("Send file"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtUpload, 0, wxALL, 5 );
	
	m_pBtCancel = new wxButton( this, lmID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtCancel, 0, wxALL, 5 );
	
	
	pButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	pMainSizer->Add( pButtonsSizer, 0, wxEXPAND|wxALIGN_RIGHT|wxTOP|wxBOTTOM, 5 );
	
	this->SetSizer( pMainSizer );
	this->Layout();
	pMainSizer->Fit( this );
	
	this->Centre( wxBOTH );
}

bool lmDebugReportDialog::TransferDataToWindow()
{
    m_pTxtFileName->SetLabel( m_sLogFilename );

    return true;
}

bool lmDebugReportDialog::TransferDataFromWindow()
{
    // if the user entered any notes, add them to the report

    const wxString sNotes = m_pTxtMoreInfo->GetValue();
    if ( !sNotes.empty() )
    {
        g_pLogger->LogForensic(_T("User notes follow:"));
        g_pLogger->LogForensic(sNotes);
    }

    return true;
}

void lmDebugReportDialog::OnView(wxCommandEvent& event)
{
    wxString sFileContent;
    wxFFile file(m_sLogFilename);
    if ( file.IsOpened() && file.ReadAll(&sFileContent) )
    {
        lmDlgDebug dlg(this, _T("Forensic log preview"), sFileContent, false);     //false: no 'Save' button
        dlg.ShowModal();
    }
}

void lmDebugReportDialog::OnUpload(wxCommandEvent& event)
{
    ::wxBeginBusyCursor();
    m_pDbgReport->DoUpload();
    ::wxEndBusyCursor();
    g_pLogger->DeleteForensicTarget();
    EndModal(wxID_OK);
}

void lmDebugReportDialog::OnCancel(wxCommandEvent& event)
{
    g_pLogger->DeleteForensicTarget();
    EndModal(wxID_CANCEL);
}


//-------------------------------------------------------------------------------------------
// lmForensicLog: helper class for uploading/emailing the debug log
//-------------------------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmForensicLog, wxEvtHandler)
    EVT_SOCKET(lmID_SOCKET, lmForensicLog::OnSocketEvent)
END_EVENT_TABLE()

lmForensicLog::lmForensicLog(wxString& sLogname, wxString& sCurlPath)
    : wxEvtHandler()
    , m_sLogName(sLogname)
    , m_sCurlCmd(sCurlPath)
{
#if lmUSE_SOCKETS
    // Setup the socket event handler and subscribe to most events
    m_oSocket.SetEventHandler(*this, lmID_SOCKET);
    m_oSocket.SetNotify(wxSOCKET_CONNECTION_FLAG |
                        wxSOCKET_INPUT_FLAG |
                        wxSOCKET_LOST_FLAG);
    m_oSocket.Notify(true);
#endif
}

void lmForensicLog::UploadLog(wxString sURL, wxString sInputField, wxString sAction,
                              bool fHandlingCrash)
{
    m_sUploadURL = sURL;
    m_sInputField = sInputField;
    m_sCurlCmd += _T("\\curl -s ");
    m_sAction = sAction;

    if ( m_sUploadURL.Last() != _T('/') )
        m_sUploadURL += _T('/');

    m_sUploadURL += sAction;

    //Show dilaog to inform user and ask for permission
    lmDebugReportDialog dlg(this, fHandlingCrash);
#ifdef __WXMSW__
    if (fHandlingCrash)
    {
        //if we are handling a crash (wxApp::OnFatalException method),
        //before entering the event loop (from ShowModal()), block the event
        //handling for all other windows as this could result in more crashes
        wxEventLoop::SetCriticalWindow(&dlg);
    }
#endif

    dlg.ShowModal();
}

bool lmForensicLog::OnServerReply(const wxArrayString& reply)
{
    // this is called with the contents of the server response.
    //TODO: parse the XML document in OnServerReply() instead of just dumping it
    if ( reply.IsEmpty() )
    {
        wxLogError(_T("Didn't receive the expected server reply."));
        return false;
    }

    wxString s(_T("Server replied:\n"));

    const size_t count = reply.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        s << _T('\t') << reply[n] << _T('\n');
    }

    wxMessageBox(s.c_str());

    return true;
}

bool lmForensicLog::DoUpload()
{
    //returns true if upload done, false otherwise

#if defined(_LM_DEBUG_)
    //in debug version, wait 3 secs to simulate upload delay time and
    //to have time to evaluate progress bar/hourglass cursor
    ::wxMilliSleep(3000);       //3 secs
#endif

    //create the zip file
    wxFileName oFN(m_sLogName);
    wxFileName oZF(oFN.GetPath(), _T("LenMus"), _T("zip"));
    wxFFileOutputStream oOS(oZF.GetFullPath(), _T("wb"));
    wxZipOutputStream oZOS(oOS, 9);

    //create the forensic log entry in the zip file
    wxZipEntry* pZE = new wxZipEntry(oFN.GetFullName());
    if ( !oZOS.PutNextEntry(pZE) )
        return false;

    //add forensic log content to the zip file
    wxFFileInputStream oIS(m_sLogName);
    if ( !oIS.IsOk() || !oZOS.Write(oIS).IsOk() )
        return false;

    //close zip file
    if ( !oZOS.Close() )
        return false;

    //Execute the curl command to upload the file
#if lmUSE_SOCKETS
    ConnectToServer();
#else
    wxArrayString output, errors;
    wxString sCmd = wxString::Format(_T("%s -F %s=@\"%s\" %s"),
                                    m_sCurlCmd.c_str(), m_sInputField.c_str(),
                                    oZF.GetFullPath(),
                                    m_sUploadURL.c_str() );
    int rc = wxExecute(sCmd, output, errors);
    if ( rc == -1 )
    {
        wxLogMessage(_T("[lmForensicLog::DoUpload] Failed to execute curl, Is curl missing?. Cmd='%s'"),
                     sCmd.c_str() );
    }
    else if ( rc != 0 )
    {
        wxLogMessage(_T("[lmForensicLog::DoUpload] Failed to upload the debug report (error code %d)."), rc);
        for (int i = 0; i < (int)errors.GetCount(); ++i )
        {
            wxLogWarning(_T("[lmForensicLog::DoUpload] Error %d: %s"), 
                            i, errors[i].c_str());
        }
    }
    else // rc == 0
    {
        if ( OnServerReply(output) )
            return true;
    }
#endif

    return false;
}

wxString lmForensicLog::GetDirectory()
{
    wxFileName oFN(m_sLogName);
    return oFN.GetPath();
}

void lmForensicLog::ConnectToServer()
{
    //Connect to the server in non-blocking mode. This implies that we
    //must wait for a socket event (either connecte or connection lost)
    //to continue
    wxIPV4address addr;
    addr.Hostname(_T("localhost"));
    addr.Service(80);
    m_oSocket.Connect(addr, false);         //false = non-blocking
    m_oSocket.WaitOnConnect(10);            //wait 10 secs

    if (m_oSocket.IsConnected())
        wxMessageBox(_T("Succeeded ! Connection established\n"));
    else
    {
        m_oSocket.Close();
        wxMessageBox(_T("Failed ! Unable to connect\n"));
    }
}

void lmForensicLog::OnSocketEvent(wxSocketEvent& event)
{
    wxString s = _T("OnSocketEvent: ");

    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            OnServerAnswer();
            s.Append(_T("wxSOCKET_INPUT\n"));
            break;
        case wxSOCKET_LOST:
            OnConnectionLost();
            s.Append(_T("wxSOCKET_LOST\n"));
            break;
        case wxSOCKET_CONNECTION:
            OnConnectedToServer();
            s.Append(_T("wxSOCKET_CONNECTION\n"));
            break;
        default:
            s.Append(_T("Unexpected event !\n"));
    }
    wxLogMessage(s);
}

void lmForensicLog::OnConnectionLost()
{
    wxMessageBox(_T("Server connection lost"));
}

void lmForensicLog::OnConnectedToServer()
{
    //Now connected. Send POST message

    wxFileName oFN(m_sLogName);
    wxFileName oZF(oFN.GetPath(), _T("LenMus"), _T("zip"));
    wxString m_sPostMsg = wxString::Format(_T("%s=%s"),
                                           m_sInputField.c_str(),
                                           oZF.GetFullPath().c_str() );
    //wxString sPost = wxString::Format(_T("POST %s HTTP/1.1\r\n"), m_sUploadURL.c_str());
    wxString sPost = _T("POST /sw/forensic.php HTTP/1.1\r\n");
    wxCharBuffer post = sPost.mb_str(wxConvUTF8);
    m_oSocket.Write(post, strlen(post));
    sPost = _T("Host: localhost\r\n");
    m_oSocket.Write(sPost.mb_str(wxConvUTF8), 128);
    sPost = _T("User-Agent: LenMus/8\r\n");
    m_oSocket.Write(sPost.mb_str(wxConvUTF8), 128);
    sPost = _T("Content-Type: application/x-www-form-urlencoded\r\n");
    m_oSocket.Write(sPost.mb_str(wxConvUTF8), 128);

    m_sPostMsg = _T("file=LenMus.zip"),

    sPost = _T("Content-Length: ");
    sPost += wxString::Format(_T("%d\r\n"), m_sPostMsg.Length());
    m_oSocket.Write(sPost.mb_str(wxConvUTF8), 128);

    post = sPost.mb_str(wxConvUTF8);
    m_oSocket.Write(post, strlen(post));

    sPost = _T("Connection: Close\r\n");
    m_oSocket.Write(sPost.mb_str(wxConvUTF8), 128);
}

void lmForensicLog::OnServerAnswer()
{
    //received something from server.

    int nLengh = 10000;
    char* pBuffer = (char*)malloc(nLengh);
    m_oSocket.Read(pBuffer, nLengh);
    nLengh = (int)m_oSocket.GetLastIOSize();
    *(pBuffer+nLengh) = 0;
    wxString sAnswer = wxString(pBuffer, wxConvUTF8);
    wxMessageBox(wxString(pBuffer, wxConvUTF8));
}

