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
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "Paths.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"

#include "Paths.h"

// the only object
lmPaths* g_pPaths = (lmPaths*) NULL;

//-------------------------------------------------------------------------------------------
// lmPaths implementation
//-------------------------------------------------------------------------------------------

lmPaths::lmPaths(wxString sBinPath)
{
    //Receives the full path to the langtool executable folder (/bin) and
    //extracts the root path
    if (sBinPath == _T(""))
        m_root.AssignCwd();
    else
        m_root.Assign(sBinPath, _T(""), wxPATH_NATIVE);
    m_root.Normalize();
    //wxMessageBox(m_root.GetFullPath());
    m_root.RemoveLastDir();
    Init();

}

lmPaths::~lmPaths()
{
}


void lmPaths::Init()
{
    wxFileName path;

    // Paths in LangTool
    path = m_root;
    path.AppendDir(_T("locale"));
    m_sLocale = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = m_root;
    path.AppendDir(_T("layout"));
    m_sLayout = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    // Paths in LenMus
    wxFileName oLenMusPath = m_root;    // 'lenmus/bin/'
	oLenMusPath.RemoveLastDir();		// 'lenmus/'
    m_sLenMus = oLenMusPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

	oLenMusPath.AppendDir(_T("books"));		// 'lenmus/books'
    m_sBooksRoot = oLenMusPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

	oLenMusPath.RemoveLastDir();
	oLenMusPath.AppendDir(_T("locale"));	// 'lenmus/locale'
    m_sLenMusLocale = oLenMusPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

}

