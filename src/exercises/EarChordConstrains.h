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
/*! @file EarChordConstrains.h
    @brief Header file for EarChord exercises
    @ingroup generators
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __EARCHORDCONSTRAINS_H__        //to avoid nested includes
#define __EARCHORDCONSTRAINS_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../auxmusic/ChordManager.h"
#include "Generators.h"
#include "Constrains.h"


class lmEarChordConstrains
{
public:
    lmEarChordConstrains(wxString sSection);
    ~lmEarChordConstrains() {}

    EChordType GetRandomChordType();

    bool InversionsAllowed() { return m_fAllowInversions; }

    void SaveSettings();


private:
    void LoadSettings();

    wxString            m_sSection;             //to save settings
    bool                m_fAllowInversions;
    lmChordConstrains   m_oChordTypes;  
    bool                m_fAllowedModes[3];     // 0-harmonic
                                                // 1-melodic ascending
                                                // 2-melodic descending
    wxString            m_sLowerRoot;    //valid range for root notes
    wxString            m_sUpperRoot;

};

#endif  // __EARCHORDCONSTRAINS_H__

