//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

//#ifndef __LENMUS_CHORDCONSTRAINS_H__        //to avoid nested includes
//#define __LENMUS_CHORDCONSTRAINS_H__
//
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "ChordConstrains.cpp"
//#endif
//
//// For compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif
//
//#include "lenmus_generators.h"
//#include "lenmus_constrains.h"
//
//enum lmEChordType
//{
//    // Triads
//    ect_MajorTriad = 0,     // 3M,5p perfect major
//    ect_FirstTriad = ect_MajorTriad,
//    ect_MinorTriad,         // 3m,5p perfect minor
//    ect_AugTriad,           // 3M,5A 5th augmented
//    ect_DimTriad,           // 3m,5d 5th diminished
//    ect_Suspended_4th,      // p4,p5
//    ect_Suspended_2nd,      // m4,p5
//    ect_LastTriad = ect_Suspended_2nd,
//
//    // Seventh chords
//    ect_MajorSeventh,
//    ect_FirstSeventh = ect_MajorSeventh,
//    ect_DominantSeventh,
//    ect_MinorSeventh,
//    ect_DimSeventh,
//    ect_HalfDimSeventh,
//    ect_AugMajorSeventh,
//    ect_AugSeventh,
//    ect_MinorMajorSeventh,
//    ect_LastSeventh = ect_MinorMajorSeventh,
//
//    // Sixth chords
//    ect_MajorSixth,
//    ect_FirstSixth = ect_MajorSixth,
//    ect_MinorSixth,
//    ect_AugSixth,
//    ect_LastSixth = ect_AugSixth,
//
//    ect_MaxInExercises,    //<-- AWARE: Last value for exercises
//
//    //Ninths
//    ect_DominantNinth = ect_MaxInExercises,          //dominant-seventh + major ninth
//    ect_FirstNinth = ect_DominantNinth,
//    ect_MajorNinth,             //major-seventh + major ninth
//    ect_MinorNinth,             //minor-seventh + major ninth
//    ect_LastNinth = ect_MinorNinth,
//
//    //11ths
//    ect_Dominant_11th,          //dominantNinth + perfect 11th
//    ect_First_11th = ect_Dominant_11th,
//    ect_Major_11th,             //majorNinth + perfect 11th
//    ect_Minor_11th,             //minorNinth + perfect 11th
//    ect_Last_11th = ect_Minor_11th,
//
//    //13ths
//    ect_Dominant_13th,          //dominant_11th + major 13th
//    ect_First_13th = ect_Dominant_13th,
//    ect_Major_13th,             //major_11th + major 13th
//    ect_Minor_13th,             //minor_11th + major 13th
//    ect_Last_13th = ect_Minor_13th,
//
//    //Other
//    //ect_PowerChord,             //perfect fifth, (octave)
//    //ect_FirstOther = ect_PowerChord,
//    ect_TristanChord,           //augmented fourth, augmented sixth, augmented second
//    //ect_LastOther = ect_TristanChord,
//
//    //last element, to signal End Of Table
//    ect_Max
//};
//
//enum EChordGroup
//{
//    ecg_Triads = 0,
//    ecg_Sevenths,
//    ecg_Sixths,
//    //last element, to signal End Of Table
//    ecg_Max
//};
//
//
//class lmChordConstrains : public ExerciseOptions
//{
//public:
//    lmChordConstrains(wxString sSection);
//    ~lmChordConstrains() {}
//
//    lmEChordType GetRandomChordType();
//    int GetRandomMode();
//
//    bool AreInversionsAllowed() { return m_fAllowInversions; }
//    void SetInversionsAllowed(bool fValue) { m_fAllowInversions = fValue; }
//
//    bool IsChordValid(lmEChordType nType) { return m_fValidChords[nType]; }
//    void SetChordValid(lmEChordType nType, bool fValid) { m_fValidChords[nType] = fValid; }
//    bool* GetValidChords() { return m_fValidChords; }
//
//    bool IsValidGroup(EChordGroup nGroup);
//
//    bool IsModeAllowed(int nMode) { return m_fAllowedModes[nMode]; }
//    void SetModeAllowed(int nMode, bool fValue) {
//            m_fAllowedModes[nMode] = fValue;
//        }
//
//    void SetDisplayKey(bool fValue) { m_fDisplayKey = fValue; }
//    bool DisplayKey() { return m_fDisplayKey; }
//
//    void SaveSettings();
//
//    KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }
//
//
//private:
//    void LoadSettings();
//
//    bool                m_fAllowInversions;
//    bool                m_fValidChords[ect_Max];
//    KeyConstrains     m_oValidKeys;           //allowed key signatures
//    bool                m_fDisplayKey;
//    bool                m_fAllowedModes[3];     // 0-harmonic
//                                                // 1-melodic ascending
//                                                // 2-melodic descending
//    wxString            m_sLowerRoot;    //valid range for root notes
//    wxString            m_sUpperRoot;
//
//};
//
//#endif  // __LENMUS_CHORDCONSTRAINS_H__
