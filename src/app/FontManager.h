//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_FONTMANAGER_H        //to avoid nested includes
#define __LM_FONTMANAGER_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "FontManager.cpp"
#endif

// fonts will be maintained in a keyed list structure. Let's declare it
#include "wx/list.h"
WX_DECLARE_LIST(wxFont, FontsList);


class lmFontManager
{
public:
    lmFontManager();
    ~lmFontManager();

    wxFont* GetFont(int nPointSize,
                    wxString sFontName = _T("LeMus Notas"),
                    int nFamily = wxDEFAULT,    //wxDEFAULT, wxDECORATIVE, wxROMAN, wxSCRIPT, wxSWISS, wxMODERN
                    int nStyle = wxNORMAL,        //wxNORMAL, wxSLANT or wxITALIC
                    int nWeight = wxNORMAL,        //wxNORMAL, wxLIGHT or wxBOLD
                    bool fUnderline = false);

private:
    FontsList    m_cFonts;    // list of allocated fonts

};

#endif    // __LM_FONTMANAGER_H
