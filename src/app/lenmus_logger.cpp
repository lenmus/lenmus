//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#include "lenmus_logger.h"
#include "lenmus_standard_header.h"

#include "lenmus_dlg_debug.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/textfile.h>        //to use wxTextFile


namespace lenmus
{

//---------------------------------------------------------------------------------------
// Logger class deals with all loging and error reporting issues
//
//    There are two log files:
//
//    Trace.log    is for logging all trace messages for debugging, error reporting, etc.
//                that is, all kind of information developers would like to see if the
//                user report a problem.
//    Parse.log    is for logging information to be shown to the user related to source
//                music scores parsing errors/unsupported tags: XML import, LDP, etc.
//
//
//    Logging methods:
//
//    LogFatalError    which is like LogError, but also terminates the program with
//                    the exit code 3 (using abort() standard function). Unlike for all
//                    the other logging functions, this function can't be overridden by
//                    a log target.
//    LogError        is the function to use for error messages, i.e. the messages that must
//                    be shown to the user. The default processing is to pop up a message
//                    box to inform the user about it.
//    LogWarning        for warnings - they are also normally shown to the user, but don't
//                    interrupt the program work.
//    LogMessage        is for all normal, informational messages. They also appear in a
//                    message box by default (but it can be changed, see below).
//    LogVerbose        is for verbose output. Normally, it is suppressed, but might be
//                    activated if the user wishes to know more details about the program
//                    progress.
//    LogSysError        is mostly used by wxWidgets itself, but might be handy for logging
//                    errors after system call (API function) failure. It logs the specified
//                    message text as well as the last system error code (errno or
//                    ::GetLastError() depending on the platform) and the corresponding
//                    error message. The second form of this function takes the error
//                    code explicitly as the first argument.
//    LogDebug        is the right function for debug output. It only does anything at all
//                    in the debug mode (when the preprocessor symbol _LM_DEBUG_ is defined)
//                    and expands to nothing in release mode (otherwise). Tip: under Windows,
//                    you must either run the program under debugger or use a 3rd party
//                    program such as DbgView to actually see the debug output.
//    LogTrace        as LogDebug only does something in debug build. The reason for making
//                    it a separate function from it is that usually there are a lot of
//                    trace messages, so it might make sense to separate them from
//                    other debug messages which would be flooded in them. Moreover, the
//                    second version of this function takes a trace mask as the first
//                    argument which allows to further restrict the amount of messages
//                    generated.
//

Logger::Logger()
    : m_pTrace((wxFile*)NULL)
    , m_pDataError((wxFile*)NULL)
    , m_pForensic((wxFile*)NULL)
    , m_sForensicPath(wxEmptyString)
    , m_sScorePath(wxEmptyString)
{
    // For now use wxLog facilities and send messages to Stderr
    #if (LENMUS_PLATFORM_UNIX == 1)
        wxLog *logger=LENMUS_NEW wxLogStderr();
        delete wxLog::SetActiveTarget(logger);
    #elif (LENMUS_PLATFORM_WIN32 == 1)
        wxLog *logger=LENMUS_NEW wxLogStderr();
        delete wxLog::SetActiveTarget(logger);
    #else
        // do nothing. Use default output for log messages (message box)
    #endif


    //Activate trace for errors
    AddTraceMask(_T("Errors"));

}

Logger::~Logger()
{
    if (m_pTrace) {
        m_pTrace->Close();
        delete m_pTrace;
    }

    if (m_pDataError) {
        m_pDataError->Close();
        delete m_pDataError;
    }

    if (m_pForensic) {
        m_pForensic->Close();
        delete m_pForensic;
    }

}

bool Logger::IsValidForensicTarget(wxString& sPath)
{
    //returns true if forensic file already exists and contains a score

    //file must exist and must contain a score
    wxString sFileContent;
    wxFFile file(sPath);
    if ( file.IsOpened() && file.ReadAll(&sFileContent) )
        return sFileContent.Contains(_T("(score"));

    return false;
}

void Logger::LogScore(wxString sScore)
{
    LogForensic(sScore);

    //save the score, to be recovered later
    wxFile oFile(m_sScorePath, wxFile::write);
    if (!oFile.IsOpened())
    {
        wxLogMessage(_T("[Logger::LogScore] Error while saving score. Path '%s'"),
            m_sScorePath.c_str());
        return;
    }
    oFile.Write(sScore);
    oFile.Close();
}

void Logger::SetForensicTarget(wxString& sLogPath, wxString& sScorePath)
{
    m_sScorePath = sScorePath;

    //prepare data error log file
    m_sForensicPath = sLogPath;
    m_pForensic = LENMUS_NEW wxFile(m_sForensicPath, wxFile::write);
    if (!m_pForensic->IsOpened())
    {
        wxLogMessage(_T("[Logger::SetForensicTarget] Error while openning forensic log!"));
        delete m_pForensic;
        m_pForensic = (wxFile*)NULL;
        return;
    }

    //log time stamp
    m_pForensic->Write( (wxDateTime::Now()).Format(_T("%Y/%m/%d %H:%M:%S")) + _T("\n") );
}

void Logger::SetDataErrorTarget(wxString& sPath)
{
    //prepare data error log file
    m_sDataErrorPath = sPath;
    m_pDataError = LENMUS_NEW wxFile(m_sDataErrorPath, wxFile::write);
    if (!m_pDataError->IsOpened())
    {
        //TODO
        delete m_pDataError;
        m_pDataError = (wxFile*)NULL;
        return;
    }
}

void Logger::SetTraceTarget(wxString& sPath)
{
}

void Logger::ReportProblem(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr);
    if (m_pTrace) {
        sMsg += _T("\n");
        m_pTrace->Write(sMsg);
    }
    else {
        wxLogTrace(_T("Errors"), sMsg, _T(""));
    }
    va_end(argptr);

    ::wxMessageBox(sMsg, _("Error message"));

}

void Logger::ReportBug(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr) + _T("\n");
    if (m_pDataError) {
        m_pDataError->Write(sMsg);
    }
    else {
        wxLogMessage(sMsg);
    }
    va_end(argptr);

}

void Logger::FlushDataErrorLog()
{
    // close current file
    if (m_pDataError) {
        m_pDataError->Close();
        delete m_pDataError;
        m_pDataError = (wxFile*) NULL;
    }

    //open a LENMUS_NEW one
    SetDataErrorTarget(m_sDataErrorPath);

}

void Logger::FlushForensicLog()
{
    // close current file
    if (m_pForensic)
    {
        m_pForensic->Close();
        delete m_pForensic;
        m_pForensic = (wxFile*) NULL;
    }

    //open a LENMUS_NEW one
    SetForensicTarget(m_sForensicPath, m_sScorePath);
}

void Logger::DeleteForensicTarget()
{
    //close current file
    if (m_pForensic)
    {
        m_pForensic->Close();
        delete m_pForensic;
        m_pForensic = (wxFile*) NULL;
    }

    //delete the file
    if (m_sForensicPath != wxEmptyString)
        ::wxRemoveFile(m_sForensicPath);

    //delete any previous score
    if (m_sScorePath != wxEmptyString)
        ::wxRemoveFile(m_sScorePath);
}

void Logger::ShowDataErrors(wxString sTitle)
{
    if (!m_pDataError) return;

    wxTextFile oFile(m_sDataErrorPath);
    oFile.Open();
    wxString sContent = oFile.GetFirstLine();
    sContent += _T("\n");
    while (!oFile.Eof()) {
        sContent += oFile.GetNextLine();
        sContent += _T("\n");
    }
    oFile.Close();
    DlgDebug dlg( wxApp().GetTopWindow(), _T("Errors"), sContent);
    dlg.SetTitle(sTitle);
    dlg.ShowModal();
}

void Logger::LogDataError(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr);
    if (m_pDataError) {
        sMsg += _T("\n");
        m_pDataError->Write(sMsg);
    }
    else {
        wxLogMessage(sMsg);
    }
    va_end(argptr);

}

void Logger::LogFatalError(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogFatalError(szFormat, argptr);
    va_end(argptr);
}

void Logger::LogError(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogError(szFormat, argptr);
    va_end(argptr);
}

void Logger::LogWarning(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogWarning(szFormat, argptr);
    va_end(argptr);
}

void Logger::LogMessage(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogMessage(szFormat, argptr);
    va_end(argptr);
}

void Logger::LogForensic(const wxChar* szFormat, ...)
{
    //received data
    va_list argptr;
    va_start(argptr, szFormat);
    m_pForensic->Write( wxString::FormatV(szFormat, argptr) );
    m_pForensic->Write( _T("\n") );
    va_end(argptr);
}

void Logger::LogVerbose(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogVerbose(szFormat, argptr);
    va_end(argptr);
}

void Logger::LogSysError(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogSysError(szFormat, argptr);
    va_end(argptr);
}

// the following methods are only for the debug version. For the release one they
// expand to nothing
#if (LENMUS_DEBUG == 1)

void Logger::LogDebug(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogDebug(szFormat, argptr);
    va_end(argptr);
}

void Logger::LogTrace(const wxString& mask, const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogTrace(mask, szFormat, argptr);
    va_end(argptr);
}

void Logger::DefineTraceMask(wxString mask)
{
    m_aMasks.Add( mask );
}

void Logger::PopulateWithDefinedTraceMasks(wxControlWithItems* pCtrol, int nSelected)
{
	// Populate a wxControlWithItems (wxListBox, wxCheckListBox, wxChoice, wxComboBox, ...)
	// with the list of defined trace masks.
    // Set 'nSelected' trace mask as the selected one (default: the first one)

	pCtrol->Clear();
	int iSel = 0;

    //populate control
	for (int i=0; i < (int)m_aMasks.GetCount(); i++ ) {
        pCtrol->Append( m_aMasks[i] );
		if (i == nSelected) iSel = i;
    }
    pCtrol->SetStringSelection( pCtrol->GetString(iSel) );

}


void Logger::AddTraceMask(const wxString& mask) { wxLog::AddTraceMask(mask); }
void Logger::RemoveTraceMask(const wxString& mask) { wxLog::RemoveTraceMask(mask); }
void Logger::ClearTraceMasks() { wxLog::ClearTraceMasks(); }

bool Logger::IsAllowedTraceMask(const wxChar *mask)
{
    return wxLog::IsAllowedTraceMask(mask);
}



#endif        // definitions for LENMUS_DEBUG mode


}   //namespace lenmus
