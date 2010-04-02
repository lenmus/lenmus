//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-1010 LenMus project
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

#ifndef __LM_TONALITYCONSTRAINS_H__        //to avoid nested includes
#define __LM_TONALITYCONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TonalityConstrains.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Generators.h"
#include "Constrains.h"


class lmTonalityConstrains : public lmExerciseOptions
{
public:
    lmTonalityConstrains(wxString sSection);
    ~lmTonalityConstrains() {}

    inline lmKeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }

    inline bool UseMajorMinorButtons() { return m_fUseMajorMinorButtons; }

    inline void UseMajorMinorButtons(bool fValue) { m_fUseMajorMinorButtons = fValue; }

    inline bool IsValidKey(lmEKeySignatures nKey) { 
        return m_oValidKeys.IsValid(nKey); 
    }

    inline void SetSection(wxString sSection) {
        m_sSection = sSection;
        LoadSettings();
    }

    void SaveSettings();

private:
    void LoadSettings();

    bool                m_fUseMajorMinorButtons;
    lmKeyConstrains     m_oValidKeys;           //allowed key signatures
};

#endif  // __LM_TONALITYCONSTRAINS_H__
