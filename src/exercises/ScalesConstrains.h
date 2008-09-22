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

#ifndef __LM_SCALESCONSTRAINS_H__        //to avoid nested includes
#define __LM_SCALESCONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScalesConstrains.cpp"
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

//    Major scales            Type
//    ---------------------   ----
//    Natural                 I  
//    Harmonic                II
//    Type III                III
//    Mixolydian              IV
//
//    Minor scales            Type
//    ---------------------   ----
//    Natural                   
//    Melodic
//    Dorian
//    Harmonic
//
//    Medievals modes         Mode    Gregorian modes
//    ---------------------   ----    --------------------------
//    Protus      auténtico   I       Dorico          Dorian (Re)
//                plagal      II      Hipodórico
//    Deuterus    auténtico   III     Frigio          Phrygian (Mi)
//                plagal      IV      Hipofrigio
//    Tritus      auténtico   V       Lidio           Lydian (Fa)
//                plagal      VI      Hipolidio
//    Tetrardus   auténtico   VII     Mixolidio       Mixolydian (Sol)
//                plagal      VIII    Hipomixolidio
//
//    Modes introduced in 1547:
//
//                auténtico   IX      Eolio           Aeolian (La = minor natural)
//                plagal      X       Hipoeolio
//                auténtico   XI      Jónico          Ionian (Do = major natural)
//                plagal      XII     Hipojónico
//
//    Later introduced modes (rarely used):
//
//                            XIII    Locrio          Locrian (Si)
//                            XIV     Hipolocrio
//
enum EScaleType
{
    // Major scales
    est_MajorNatural = 0,
    est_MajorTypeII,
    est_MajorTypeIII,
    est_MajorTypeIV,
    est_LastMajor = est_MajorTypeIV, 

    // Minor scales
    est_MinorNatural,
    est_MinorDorian,
    est_MinorHarmonic,
    est_MinorMelodic,
    est_LastMinor = est_MinorMelodic,

    // From here, scales without mode
    est_EndOfModalScales = est_LastMinor,

    // Gregorian modes
    est_GreekIonian,
    est_GreekDorian,
    est_GreekPhrygian,
    est_GreekLydian,
    est_GreekMixolydian,
    est_GreekAeolian,
    est_GreekLocrian,
    est_LastGreek = est_GreekLocrian,

    // Other scales
    est_PentatonicMinor,
    est_PentatonicMajor,
    est_Blues,
        //Start of non-tonal scales 
    est_StartNonTonal, 
    est_WholeTones = est_StartNonTonal,
    est_Chromatic,
    est_LastOther = est_Chromatic,

    //last element, to signal End Of Table
    est_Max
};

enum EScaleGroup
{
    esg_Major = 0,
    esg_Minor,
    esg_Gregorian,
    esg_Other,
    //last element, to signal End Of Table
    esg_Max
};


class lmScalesConstrains : public lmExerciseOptions
{
public:
    lmScalesConstrains(wxString sSection);
    ~lmScalesConstrains() {}

    EScaleType GetRandomScaleType();
    bool GetRandomPlayMode();

    bool IsScaleValid(EScaleType nType) { return m_fValidScales[nType]; }
    void SetScaleValid(EScaleType nType, bool fValid) { m_fValidScales[nType] = fValid; }
    bool* GetValidScales() { return m_fValidScales; }

    bool IsValidGroup(EScaleGroup nGroup);

    int GetPlayMode() { return m_nPlayMode; }
    void SetPlayMode(int nPlayMode) { m_nPlayMode = nPlayMode; }

    void SetDisplayKey(bool fValue) { m_fDisplayKey = fValue; }
    bool DisplayKey() { return m_fDisplayKey; }

    void SetSection(wxString sSection) {
                m_sSection = sSection;
                LoadSettings();
            }

    void SaveSettings();
    
    lmKeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }


private:
    void LoadSettings();

    bool                m_fValidScales[est_Max];
    lmKeyConstrains     m_oValidKeys;           //allowed key signatures
    bool                m_fDisplayKey;
    int                 m_nPlayMode;            // 0-ascending
                                                // 1-descending
                                                // 2-both

};

#endif  // __LM_SCALESCONSTRAINS_H__
