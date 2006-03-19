// RCS-ID: $Id: Paths.h,v 1.5 2006/02/23 19:20:28 cecilios Exp $
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
/*! @file Paths.h
    @brief Header file for class lmPaths
    @ingroup configuration
*/
#ifndef __SBPATHS_H__
#define __SBPATHS_H__

#include "wx/config.h"
#include "wx/filename.h"

class lmPaths
{
public:
    lmPaths(wxString sBinPath);
    ~lmPaths();

    void LoadUserPreferences();
    void SaveUserPreferences();

    //Access to paths
    wxString GetRootPath() { return m_root.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR); }
    wxString GetBinPath() { return m_sBin; }
    wxString GetXrcPath() { return m_sXrc; }
    wxString GetDataPath() { return m_sData; }
    wxString GetTempPath() { return m_sTemp; }
    wxString GetImagePath() { return m_sImages; }
    wxString GetLocaleRootPath() { return m_sLocaleRoot; }

    // paths that depend on the language name
    wxString GetLocalePath() { return m_sLocale; }
    wxString GetHelpPath() { return m_sHelp; }
    wxString GetBooksPath() { return m_sBooks; }

    void SetLocalePathNames(wxString sLangCode);
    void SetLanguageCode(wxString sLangCode) { m_sLangCode = sLangCode; }

private:

        //
        // member variables
        //

    //root (instalation path) for LenMus
    wxFileName    m_root;

    //path names
    wxString    m_sLocaleRoot;  //root path for locale (it is m_sLocale without the language subfolder)
    wxString    m_sLocale;      //path containing program translation files ( .mo catalogs)
    wxString    m_sLangCode;    //language code (2 or 5 chars: i.e. "en", "en_US")
    wxString    m_sScores;      //path for scores
    wxString    m_sTemp;        //path for temporary files (help books preprocessed, ...)
    wxString    m_sBooks;       //path for books
    wxString    m_sHelp;        //path for help files
    wxString    m_sBin;         //path for binaries
    wxString    m_sData;        //path for .ini database files
    wxString    m_sXrc;         //path for .xrc and .xrs resource files
    wxString    m_sImages;      //path for resource images
    
};

#endif    // __SBPATHS_H__