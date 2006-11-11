// RCS-ID: $Id: Logger.h,v 1.4 2006/02/23 19:16:31 cecilios Exp $
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
/*! @file Logger.h
    @brief Header file for class lmLogger
    @ingroup app_gui
*/
#ifndef __LOGGER_H__        //to avoid nested includes
#define __LOGGER_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/wfstream.h"

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



class lmLogger
{
public:
    lmLogger();
    ~lmLogger();

    void SetDataErrorTarget(wxString sPath);
    void SetTraceTarget(wxString sPath);

    void ReportProblem(const wxChar* szFormat, ...);
    void ReportBug(const wxChar* szFormat, ...);
    void FlushDataErrorLog();
    void ShowDataErrors(wxString sTitle = _("Errors"));
    void LogDataError(const wxChar* szFormat, ...);

    void LogFatalError(const wxChar* szFormat, ...);
    void LogError(const wxChar* szFormat, ...);
    void LogWarning(const wxChar* szFormat, ...);
    void LogMessage(const wxChar* szFormat, ...);
    void LogVerbose(const wxChar* szFormat, ...);
    void LogSysError(const wxChar* szFormat, ...);
#ifdef _DEBUG
    void LogDebug(const wxChar* szFormat, ...);
    void LogTrace(const wxString& mask, const wxChar* szFormat, ...);
    void DefineTraceMask(wxString mask);
    void AddTraceMask(const wxString& mask);
    void RemoveTraceMask(const wxString& mask);
    void ClearTraceMasks();
    bool IsAllowedTraceMask(const wxChar* mask);
    wxString GetDefinedTraceMasks();

#else
    inline void LogDebug(const wxChar* szFormat, ...) {}
    inline void LogTrace(const wxString& mask, const wxChar* szFormat, ...) {}
    inline void DefineTraceMask(wxString mask) {}
    inline void AddTraceMask(const wxString& mask) {}
    inline void RemoveTraceMask(wxString& mask) {}
    inline void ClearTraceMasks() {}
    inline bool IsAllowedTraceMask(const wxChar *mask) { return false; }
    inline wxString GetDefinedTraceMasks() {}

#endif


private:
    wxFile*        m_pTrace;
    wxFile*        m_pDataError;

    wxString    m_sDataErrorPath;
};

#endif    // __LOGGER_H__
