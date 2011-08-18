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

#ifndef __LENMUS_LOGGER_H__        //to avoid nested includes
#define __LENMUS_LOGGER_H__

#include "lenmus_config.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/wfstream.h>


namespace lenmus
{

/*

Error reporting methods
-----------------------
All of them inform the user about the problem by using a message box.

ReportFatalError
        Program can't continue: inform user with a message box and abort immediately.
        All additional information for debugging must be previously logged
        with LogTrace.

ReportError
        The error affects only to certain functionality and so, the program could
        continue without it. User is informed by a message box about it. And user is
        asked whether to continue or to abort program.
        All additional information for debugging must be previously logged
        with LogTrace.

ReportBug
        errors in the normal flow of the program: non-expected values for
        variables, execution flow reaching an "imposible to reach point", etc. These
        errors are normally associated with wxASSERT sentences.
        These errors are logged for debugging. The user just receives a generic
        error message and the program is aborted.
        All additional information for debugging must be previously logged
        with LogTrace.

ReportProblem
        Inform the user about a local error that affects only to the action being
        performed. The program can continue without any risk. User is informed.


Data logging methods
--------------------
These just writes data in a file for future analysis. User is not informed about the
logged message and program continues normally.

LogDataError
        a error in data that the program must parse (i.e. a source score,
        html code, etc.). All this errors are logged in a text file and
        the user is informed when the parsing process finishes.

LogDataMessage
        An alias for LogDataError to improve program legibility when logging additional
        information for a DataError to be reported or to give more information about a
        DataError log (i.e. date, title, ...)

ShowDataErrors
        A dialog with current content of DataError log. After showing it the log is deleted.

LogTrace
        Messages recorded for debugging purpose. Disabled in release mode.
    eDebug,      // never shown to the user, disabled in release mode
    eTrace,      // trace messages are also only enabled in debug mode

*/

// Aliases to improve program legibility
#define LogDataMessage    LogDataError



class Logger
{
public:
    Logger();
    ~Logger();

    void SetDataErrorTarget(wxString& sPath);
    void SetTraceTarget(wxString& sPath);
    void SetForensicTarget(wxString& sLogPath, wxString& sScorePath);
    void DeleteForensicTarget();
    bool IsValidForensicTarget(wxString& sPath);

    void ReportProblem(const wxChar* szFormat, ...);
    void ReportBug(const wxChar* szFormat, ...);
    void FlushDataErrorLog();
    void FlushForensicLog();
    void ShowDataErrors(wxString sTitle = _("Errors"));
    void LogDataError(const wxChar* szFormat, ...);
    void LogFatalError(const wxChar* szFormat, ...);
    void LogError(const wxChar* szFormat, ...);
    void LogWarning(const wxChar* szFormat, ...);
    void LogMessage(const wxChar* szFormat, ...);
    void LogForensic(const wxChar* szFormat, ...);
    void LogScore(wxString sScore);
    void LogVerbose(const wxChar* szFormat, ...);
    void LogSysError(const wxChar* szFormat, ...);

#if (LENMUS_DEBUG == 1)
    void LogDebug(const wxChar* szFormat, ...);
    void LogTrace(const wxString& mask, const wxChar* szFormat, ...);
    void DefineTraceMask(wxString mask);
    void AddTraceMask(const wxString& mask);
    void RemoveTraceMask(const wxString& mask);
    void ClearTraceMasks();
    bool IsAllowedTraceMask(const wxChar* mask);
    void PopulateWithDefinedTraceMasks(wxControlWithItems* pCtrol, int nSelected=0);

#else
    inline void LogDebug(const wxChar* szFormat, ...) {}
    inline void LogTrace(const wxString& mask, const wxChar* szFormat, ...) {}
    inline void DefineTraceMask(wxString mask) {}
    inline void AddTraceMask(const wxString& mask) {}
    inline void RemoveTraceMask(wxString& mask) {}
    inline void ClearTraceMasks() {}
    inline bool IsAllowedTraceMask(const wxChar *mask) { return false; }
    inline void PopulateWithDefinedTraceMasks(wxControlWithItems* pCtrol, int nSelected=0) {}
#endif

private:
    wxFile*         m_pTrace;
    wxFile*         m_pDataError;
    wxFile*         m_pForensic;
    wxArrayString   m_aMasks;
    wxString        m_sDataErrorPath;
    wxString        m_sForensicPath;
    wxString        m_sScorePath;
};


}   //namespace lenmus

#endif    // __LENMUS_LOGGER_H__
