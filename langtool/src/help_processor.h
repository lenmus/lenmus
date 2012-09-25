//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
//    Copyright (c) 2002-2009 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program. If not, see <http://www.gnu.org/licenses/>. 
//
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __LM_HELP_PROCESSOR_H__        //to avoid nested includes
#define __LM_HELP_PROCESSOR_H__

#include "wx/wfstream.h"

class ltHelpProcessor
{
public:
    ltHelpProcessor(int nDbgOptions=0, wxTextCtrl* pUserLog=(wxTextCtrl*)NULL);
    ~ltHelpProcessor();

    bool GenerateHelpFile(wxString sSrcPath, wxString sLangCode, wxString sCharCode,
                     int nOptions=0);
    bool CreateHelpPoFile(wxString sFilename, wxString& sCharSet, wxString& sLangName,
                      wxString& sLangCode, wxString& sFolder);

private:
    bool CopyFileToHelp(const wxString& sFilename, const wxString& sZipFolder=_T(""));
    bool CopyAllFilesToHelp(const wxString& sSrcFolder, const wxString& sZipFolder,
                            const wxString& sFilter);
    bool PackHelpFile(wxString& sSrcFolder, wxString& sTempFolder, wxString& sDestFolder);
    void LogMsg(const wxChar* szFormat, ...);

    wxZipOutputStream*      m_pZipFile;
    wxFFileOutputStream*    m_pZipOutFile;

    //debugging and trace
    wxTextCtrl*     m_pLog;
};


#endif    // __LM_HELP_PROCESSOR_H__
