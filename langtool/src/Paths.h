//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
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
#ifndef __LM_PATHS_H__
#define __LM_PATHS_H__

class wxFileName;

class lmPaths
{
public:
    lmPaths(wxString sBinPath);
    ~lmPaths();

    //Access to paths
    wxString GetLocalePath() { return m_sLocale; }
    wxString GetBooksRootPath() { return m_sBooksRoot; }
    wxString GetLayoutPath() { return m_sLayout; }
    wxString GetLenMusLocalePath() { return m_sLenMusLocale; }


private:
    void Init();

    // LangTool paths
    wxFileName  m_root;             // the root (instalation path) for LangTool
    wxString    m_sLocale;          // eMusicBooks translation files ( .mo catalogs)
    wxString    m_sLayout;          // layout files: i.e. images

    // LenMus paths
    wxString    m_sLenMus;          // the root (instalation path) for LenMus
    wxString    m_sLenMusLocale;    // locale root, for translating installation file
    wxString    m_sBooksRoot;       // root path for books (without the language subfolder)

};

extern lmPaths* g_pPaths;

#endif    // __LM_PATHS_H__
