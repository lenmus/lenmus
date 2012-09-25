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
#pragma implementation "ebook_processor.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/txtstrm.h"
#include "wx/zipstrm.h"
#include "wx/textfile.h"
#include "wx/arrstr.h"
#include <wx/dir.h>

#include "help_processor.h"
#include "wx/xml2.h"            // include libxml2 wrapper definitions
#include "wx/dtd.h"				// include libxml2 wrapper definitions
#include "Paths.h"


ltHelpProcessor::ltHelpProcessor(int nDbgOptions, wxTextCtrl* pUserLog)
    : m_pLog(pUserLog)
{
}

ltHelpProcessor::~ltHelpProcessor()
{
}

bool ltHelpProcessor::GenerateHelpFile(wxString sSrcPath, wxString sLangCode,
                                   wxString sCharCode, int nOptions)
{
    // returns true if success
    // PO and lang.cpp are created in sDestName (langtool\locale\)
    // help.xxx files are created in sTempFolder (lenmus\locale\xx\books\)
    // help.htb is created in sDestName (lenmus\locale\xx\)

    //temp folder
    wxString sTempFolder = g_pPaths->GetTempPath();

    //destination folder
    wxFileName oFDest( g_pPaths->GetBooksRootPath() );
    oFDest.AppendDir(sLangCode);
    wxString sDestFolder = oFDest.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    //output zip file
    oFDest.SetName(_T("help"));
    oFDest.SetExt(_T("htb"));
    m_pZipOutFile = new wxFFileOutputStream( oFDest.GetFullPath() );
    m_pZipFile = new wxZipOutputStream(*m_pZipOutFile);

    //generate help file
    bool fSuccess = PackHelpFile(sSrcPath, sTempFolder, sDestFolder);

    //delete temp files

    return fSuccess;
}

bool ltHelpProcessor::CreateHelpPoFile(wxString sFilename, wxString& sCharSet,
                                    wxString& sLangName, wxString& sLangCode,
                                    wxString& sFolder)
{
    // returns true if success

    //wxFile oFile(sFilename, wxFile::write);
    //if (!m_pLangFile->IsOpened())
    //{
    //    wxLogMessage(_T("Error: File %s can not be created"), sFilename);
    //    m_pLangFile = (wxFile*)NULL;
    //    return false;        //error
    //}

    ////Generate Po header
    //wxString sNil = _T("");
    //wxString sHeader = sNil +
    //    _T("msgid \"\"\n")
    //    _T("msgstr \"\"\n")
    //    _T("\"Project-Id-Version: LenMus 3.4\\n\"\n")
    //    _T("\"POT-Creation-Date: \\n\"\n")
    //    _T("\"PO-Revision-Date: 2006-08-25 12:19+0100\\n\"\n")
    //    _T("\"Last-Translator: \\n\"\n")
    //    _T("\"Language-Team:  <cecilios@gmail.com>\\n\"\n")
    //    _T("\"MIME-Version: 1.0\\n\"\n")
    //    _T("\"Content-Type: text/plain; charset=utf-8\\n\"\n")
    //    _T("\"Content-Transfer-Encoding: 8bit\\n\"\n")
    //    _T("\"X-Poedit-Language: ") + sLangName + _T("\\n\"\n")
    //    _T("\"X-Poedit-SourceCharset: utf-8\\n\"\n")
    //    _T("\"X-Poedit-Basepath: c:\\usr\\desarrollo_wx\\lenmus\\langtool\\locale\\src\\n\"\n")
    //    _T("\"X-Poedit-SearchPath-0: ") + sFolder + _T("\\n\"\n\n");


    //oFile.Write(sHeader);
    //oFile.Close();
    return true;
}

bool ltHelpProcessor::PackHelpFile(wxString& sSrcFolder, wxString& sTempFolder,
                                   wxString& sDestFolder)
{
    //return true if success
    //- copy all files in sTempFolder, zip them as help.htb and places this
    //  resulting file in sDestFolder.
    //- Doesn't delete temp files

    bool fSuccess = true;

    // copy index files
    wxFileName oFN1(sSrcFolder, _T("help.hhc"), wxPATH_NATIVE);
    fSuccess &= CopyFileToHelp( oFN1.GetFullPath() );
    wxFileName oFN2(sSrcFolder, _T("help.hhk"), wxPATH_NATIVE);
    fSuccess &= CopyFileToHelp( oFN2.GetFullPath() );
    wxFileName oFN3(sSrcFolder, _T("help.hhp"), wxPATH_NATIVE);
    fSuccess &= CopyFileToHelp( oFN3.GetFullPath() );


    //copy htm files
    fSuccess &= CopyAllFilesToHelp(sSrcFolder, _T(""), _T("*.htm"));

    //copy folder 'img'
    wxFileName oFNImg(sSrcFolder);
    oFNImg.AppendDir(_T("img"));
    wxString sImgFolder = oFNImg.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    fSuccess &= CopyAllFilesToHelp(sImgFolder, _T("img\\"), _T("*.*"));
    //wxDir oDirImg(sImgFolder);
    //if ( !oDirImg.IsOpened() )
    //{
    //    wxLogMessage(_T("Error: Folder %s can not be opened"), sImgFolder);
    //    return false;
    //}
    //bool fFound = oDirImg.GetFirst(&sFilename, _T("*.*"), wxDIR_FILES);
    //while(fFound)
    //{
    //    wxFileName oFN(sImgFolder, sFilename, wxPATH_NATIVE);
    //    fSuccess &= CopyFileToHelp( oFN.GetFullPath() );
    //    fFound = oDirImg.GetNext(&sFilename);
    //}

    //close zip file
    m_pZipFile->Close();

    //print final message
    if (fSuccess)
        LogMsg( _T("Help file created successfuly") );
    else
        LogMsg( _T("Errors while creating help file") );
    LogMsg( _T("===========================================\n") );


    return fSuccess;
}

bool ltHelpProcessor::CopyFileToHelp(const wxString& sFilename, const wxString& sZipFolder)
{
    //return true if success
    //sZipFolder is a string with the folder prefix to add to filename when creating
    //the entry in the zip file. For example, if zip file should contain a folder
    //in which to add the files sZipFolder should contain "the-folder"

    wxFFileInputStream inFile( sFilename, _T("rb") );
    if (!inFile.IsOk()) {
        wxLogMessage(_T("*** Error: File %s can not be included in help"), sFilename.c_str());
        return false;
    }
    wxFileName oFN(sFilename);
    wxString sEntryName = sZipFolder + oFN.GetFullName();
    m_pZipFile->PutNextEntry( sEntryName );
    m_pZipFile->Write( inFile );
    m_pZipFile->CloseEntry();
    return true;
}

bool ltHelpProcessor::CopyAllFilesToHelp(const wxString& sSrcFolder,
                                         const wxString& sZipFolder,
                                         const wxString& sFilter)
{
    //return true if success

    bool fSuccess = true;
    wxDir oDir(sSrcFolder);
    if ( !oDir.IsOpened() )
    {
        wxLogMessage(_T("*** Error: Folder %s can not be opened"), sSrcFolder.c_str());
        return false;
    }
    wxString sFilename;
    bool fFound = oDir.GetFirst(&sFilename, sFilter, wxDIR_FILES);
    while(fFound)
    {
        wxFileName oFN(sSrcFolder, sFilename, wxPATH_NATIVE);
        fSuccess &= CopyFileToHelp(oFN.GetFullPath(), sZipFolder);
        fFound = oDir.GetNext(&sFilename);
    }

    return fSuccess;
}

void ltHelpProcessor::LogMsg(const wxChar* szFormat, ...)
{
    if (!m_pLog) return;

    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr) + _T("\n");
    m_pLog->AppendText(sMsg);
    va_end(argptr);
}

