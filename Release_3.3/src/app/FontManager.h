// RCS-ID: $Id: FontManager.h,v 1.3 2006/02/23 19:16:31 cecilios Exp $
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
/*! @file FontManager.h
    @brief Header file for class lmFontManager
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __FONTMANAGER_H        //to avoid nested includes
#define __FONTMANAGER_H

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
    
#endif    // __FONTMANAGER_H
