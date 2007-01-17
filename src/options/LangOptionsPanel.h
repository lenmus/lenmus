//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __LANGOPTIONSPANEL_H__
#define __LANGOPTIONSPANEL_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "lmLangOptionsPanel.cpp"
#endif

#include "OptionsPanel.h"


class lmLangOptionsPanel: public lmOptionsPanel
{    
public:
    lmLangOptionsPanel(wxWindow* parent);
    ~lmLangOptionsPanel();
    bool Verify();
    void Apply();

private:
    // controls
    wxChoice*        m_pChoice;

    // other member variables
    int                m_nNumLangs;
    wxArrayString    m_cLangCodes;
    wxArrayString    m_cLangNames;
    wxString        m_sCurLang;
};

#endif    // __LANGOPTIONSPANEL_H__
