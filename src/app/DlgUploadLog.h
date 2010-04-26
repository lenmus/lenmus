//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_DLGUPLOADLOG_H__        //to avoid nested includes
#define __LM_DLGUPLOADLOG_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgUploadLog.h"
#endif

// headers
#include <wx/defs.h>
#include <wx/dialog.h>
#include <wx/socket.h>
#include <wx/event.h>


//-------------------------------------------------------------------------------------------
// lmForensicLog: helper class for managing/uploading the debug log
//-------------------------------------------------------------------------------------------

class lmForensicLog : public wxEvtHandler
{
public:
    lmForensicLog(wxString& sLogName, wxString& sCurlPath);

    void UploadLog(wxString sURL, wxString sInputField, wxString sAction, bool fHandlingCrash);

    inline wxString GetLogFilename() { return m_sLogName; }
    wxString GetDirectory();
    inline wxString GetReportName() const { return _T("LenMus"); }

    // socket event handler
    void OnSocketEvent(wxSocketEvent& event);

    bool DoUpload();

protected:
    bool OnServerReply(const wxArrayString& reply);
    void ConnectToServer();
    void OnConnectionLost();
    void OnConnectedToServer();
    void OnServerAnswer();

    wxString m_sUploadURL;      //URL to use with HTTP POST request
    wxString m_sInputField;     //name of the input field containing the file name in the form
    wxString m_sCurlCmd;        //the curl command line
    wxString m_sAction;         //the form action
    wxString m_sLogName;

    wxSocketClient      m_oSocket;
    wxString            m_sPostMsg;

    DECLARE_EVENT_TABLE()
};

#endif      //__LM_DLGUPLOADLOG_H__
