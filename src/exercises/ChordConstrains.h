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

#ifndef __CHORDCONSTRAINS_H__        //to avoid nested includes
#define __CHORDCONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ChordConstrains.cpp"
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

enum EChordType
{
    // Triads
    ect_MajorTriad = 0,
    ect_MinorTriad,
    ect_AugTriad,
    ect_DimTriad,
    ect_Suspended_4th,
    ect_Suspended_2nd,
    ect_LastTriad = ect_Suspended_2nd,

    // Seventh chords
    ect_MajorSeventh,
    ect_DominantSeventh,
    ect_MinorSeventh,
    ect_DimSeventh,
    ect_HalfDimSeventh,
    ect_AugMajorSeventh,
    ect_AugSeventh,
    ect_MinorMajorSeventh,
    ect_LastSeventh = ect_MinorMajorSeventh,

    // Sixth chords
    ect_MajorSixth,
    ect_MinorSixth,
    ect_AugSixth,
    ect_LastSixth = ect_AugSixth,

    //last element, to signal End Of Table
    ect_Max
};

enum EChordGroup
{
    ecg_Triads = 0,
    ecg_Sevenths,
    ecg_Sixths,
    //last element, to signal End Of Table
    ecg_Max
};


class lmChordConstrains : public lmExerciseOptions
{
public:
    lmChordConstrains(wxString sSection);
    ~lmChordConstrains() {}

    EChordType GetRandomChordType();
    int GetRandomMode();

    bool AreInversionsAllowed() { return m_fAllowInversions; }
    void SetInversionsAllowed(bool fValue) { m_fAllowInversions = fValue; }

    bool IsChordValid(EChordType nType) { return m_fValidChords[nType]; }
    void SetChordValid(EChordType nType, bool fValid) { m_fValidChords[nType] = fValid; }
    bool* GetValidChords() { return m_fValidChords; }

    bool IsValidGroup(EChordGroup nGroup);

    bool IsModeAllowed(int nMode) { return m_fAllowedModes[nMode]; }
    void SetModeAllowed(int nMode, bool fValue) {
            m_fAllowedModes[nMode] = fValue;
        }

    void SetDisplayKey(bool fValue) { m_fDisplayKey = fValue; }
    bool DisplayKey() { return m_fDisplayKey; }

    void SaveSettings();

    lmKeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }


private:
    void LoadSettings();

    bool                m_fAllowInversions;
    bool                m_fValidChords[ect_Max];
    lmKeyConstrains     m_oValidKeys;           //allowed key signatures
    bool                m_fDisplayKey;
    bool                m_fAllowedModes[3];     // 0-harmonic
                                                // 1-melodic ascending
                                                // 2-melodic descending
    wxString            m_sLowerRoot;    //valid range for root notes
    wxString            m_sUpperRoot;

};

#endif  // __CHORDCONSTRAINS_H__
