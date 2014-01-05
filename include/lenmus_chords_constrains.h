//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_CHORD_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_CHORD_CONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
//#include "lenmus_generators.h"
#include "lenmus_chord.h"
#include "lenmus_constrains.h"

////wxWidgets
//#include <wx/wxprec.h>
//#include <wx/wx.h>


namespace lenmus
{


//---------------------------------------------------------------------------------------
// To classify chords into groups, for selection, dialogs, etc.
enum EChordGroup
{
    ecg_Triads = 0,
    ecg_Sevenths,
    ecg_Sixths,
    //last element, to signal End Of Table
    ecg_Max
};

//---------------------------------------------------------------------------------------
class ChordConstrains : public ExerciseOptions
{
public:
    ChordConstrains(wxString sSection, ApplicationScope& appScope);
    ~ChordConstrains() {}

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

    void save_settings();

    KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }


private:
    void load_settings();

    bool    m_fAllowInversions;
    bool    m_fValidChords[ect_Max];
    bool    m_fDisplayKey;
    bool    m_fAllowedModes[3];     // 0-harmonic
                                    // 1-melodic ascending
                                    // 2-melodic descending
    KeyConstrains   m_oValidKeys;       //allowed key signatures
    wxString        m_sLowerRoot;       //valid range for root notes
    wxString        m_sUpperRoot;

};


}   //namespace lenmus

#endif  // __LENMUS_CHORD_CONSTRAINS_H__
