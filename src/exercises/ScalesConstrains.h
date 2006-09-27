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
/*! @file ScalesConstrains.h
    @brief Header file for Scales exercises
    @ingroup generators
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __SCALESCONSTRAINS_H__        //to avoid nested includes
#define __SCALESCONSTRAINS_H__

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
#include "EarExerciseCtrol.h"

//    Major scales            Type
//    ---------------------   ----
//    Natural                 I  
//    Harmonic                II
//    Type III                III
//    Mixolydian              IV
//
//    Minor scales            Type
//    ---------------------   ----
//    Melodic
//    Dorian
//    Harmonic
//
//    Medievals modes         Mode    Greek modes
//    ---------------------   ----    --------------------------
//    Protus      auténtico   I       Dorico          dorian
//                plagal      II      Hipodórico
//    Deuterus    auténtico   III     Frigio          phrygian
//                plagal      IV      Hipofrigio
//    Tritus      auténtico   V       Lidio           lydian
//                plagal      VI      Hipolidio
//    Tetrardus   auténtico   VII     Mixolidio       mixolydian
//                plagal      VIII    Hipomixolidio
//
//    Modes introduced in 1547:
//
//                auténtico   IX      Eolio           aeolian
//                plagal      X       Hipoeolio
//                auténtico   XI      Jónico          ionian
//                plagal      XII     Hipojónico
//
//    Later introduced modes (rarely used):
//
//                            XIII    Locrio          locrian
//                            XIV     Hipolocrio
//
enum EScaleType
{
    // Major scales
    est_MajorTriad = 0,
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

    // Greek modes
    est_GreekDorian,
    est_GreekPhrygian,
    est_GreekLydian,
    est_GreekMixolydian,
    est_GreekAeolian,
    est_GreekIonian,
    est_GreekLocrian,
    est_LastSixth = est_GreekLocrian,

    //last element, to signal End Of Table
    est_Max
};

enum EScaleGroup
{
    esg_Major = 0,
    esg_Minor,
    esg_Greek,
    esg_Other,
    //last element, to signal End Of Table
    esg_Max
};


class lmScalesConstrains : public lmEarConstrains
{
public:
    lmScalesConstrains(wxString sSection);
    ~lmScalesConstrains() {}

    EScaleType GetRandomChordType();
    int GetRandomMode();

    bool AreInversionsAllowed() { return m_fAllowInversions; }
    void SetInversionsAllowed(bool fValue) { m_fAllowInversions = fValue; }

    bool IsScaleValid(EScaleType nType) { return m_fValidScales[nType]; }
    void SetScaleValid(EScaleType nType, bool fValid) { m_fValidScales[nType] = fValid; }
    bool* GetValidScales() { return m_fValidScales; }

    bool IsValidGroup(EScaleGroup nGroup);

    bool IsModeAllowed(int nMode) { return m_fAllowedModes[nMode]; }
    void SetModeAllowed(int nMode, bool fValue) {
            m_fAllowedModes[nMode] = fValue;
        }

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

    bool                m_fAllowInversions;
    bool                m_fValidScales[est_Max];
    lmKeyConstrains     m_oValidKeys;           //allowed key signatures
    bool                m_fDisplayKey;
    bool                m_fAllowedModes[3];     // 0-harmonic
                                                // 1-melodic ascending
                                                // 2-melodic descending
    wxString            m_sLowerRoot;    //valid range for root notes
    wxString            m_sUpperRoot;



};

#endif  // __SCALESCONSTRAINS_H__
