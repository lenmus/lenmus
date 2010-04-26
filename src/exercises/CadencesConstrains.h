//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_CADENCESCONSTRAINS_H__        //to avoid nested includes
#define __LM_CADENCESCONSTRAINS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "CadencesConstrains.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Generators.h"
#include "Constrains.h"

// Cadences
// --------
//
// a) Terminal (Conclusivas)
//
//  - Perfect authentic (Auténtica), chords in root position
//      * V -> I
//      * V7 -> I
//      * I(64) -> V  (6-4 cadencial. En Do: Sol-Do-Mi / Sol-Si-re)
//      * V con 5ª aum -> I
//      * V con 5ª dism -> I
//  - Plagal (Plagal):
//      * IV -> I
//      * iv -> I
//      * II -> I (= VdeV -> I)
//      * ii -> I (= ii, como napolitana)
//      * VI -> I
//
// b) Transient (Suspensivas)
//
//	- Imperfect authentic (Imperfecta):
//      * V -> I, uno o ambos acordes invertidos
//	- Deceptive or interrupted (Cadencia rota):
//      * V -> IV
//      * V -> iv
//      * V -> VI
//      * V -> vi
//      * V -> ii
//      * V -> III
//      * V -> VII
//	- Half, or open, or imperfect (Semicadencia):
//      * ii(6) -> V
//      * IV -> V
//      * I -> V
//      * IV6 -> V
//      * II -> V,
//      * IIdim(6) -> V (sexta napolitana = II dim en 1ª inversión.
//                     Ej: en Do: Fa-lab-reb -Sol-si-re)
//      * VdeV con 5ª dim y en 2ª inversión -> V (sexta aumentada)
//

//AWARE: any change in this enumeration requieres the appropriate change in
//      method lmIdfyCadencesCtrolParms::CadenceNameToType()

enum lmECadenceType
{
    // Terminal cadences
    lm_eCadTerminal = 0,    //Start of terminal cadences

    // Start of Perfect authentic cadences
    lm_eCadPerfect = lm_eCadTerminal,         
    lm_eCadPerfect_V_I = lm_eCadPerfect,
    lm_eCadPerfect_V7_I,
    lm_eCadPerfect_Va5_I,
    lm_eCadPerfect_Vd5_I,
    lm_eCadLastPerfect,

    // Start of Plagal cadences
	lm_eCadPlagal = lm_eCadLastPerfect,
    lm_eCadPlagal_IV_I = lm_eCadPlagal,
    lm_eCadPlagal_IVm_I,
    lm_eCadPlagal_IIc6_I,
    lm_eCadPlagal_IImc6_I,
    lm_eCadLastPlagal,

    // Imperfect authentic cadences
	lm_eCadImperfect = lm_eCadLastPlagal,
	lm_eCadImperfect_V_I = lm_eCadImperfect,
    lm_eCadLastImperfect,

    lm_eCadLastTerminal = lm_eCadLastImperfect,    // last terminal cadence

    // Transient cadences
    lm_eCadTransient = lm_eCadLastTerminal,

    // Deceptive cadences
    lm_eCadDeceptive = lm_eCadTransient,
    lm_eCadDeceptive_V_IV = lm_eCadDeceptive,
    lm_eCadDeceptive_V_IVm,
    lm_eCadDeceptive_V_VI,
    lm_eCadDeceptive_V_VIm,
    lm_eCadDeceptive_V_IIm,
    lm_eCadDeceptive_V_III,
    lm_eCadDeceptive_V_VII,
    lm_eCadLastDeceptive,

    // Half cadences
    lm_eCadHalf = lm_eCadLastDeceptive,
    lm_eCadHalf_IImc6_V = lm_eCadHalf,
    lm_eCadHalf_IV_V,
    lm_eCadHalf_I_V,
    lm_eCadHalf_Ic64_V,
    lm_eCadHalf_IV6_V,
    lm_eCadHalf_II_V,
    lm_eCadHalf_IIdimc6_V,
    lm_eCadHalf_VdeVdim5c64_V,
    lm_eCadLastHalf,

    lm_eCadLastTransient = lm_eCadLastHalf,

    //last element, to signal End Of Table
    lm_eCadMaxCadence = lm_eCadLastTransient,
};

enum lmECadenceButtons
{
    lm_eCadButtonTerminal = 0,
    lm_eCadButtonTransient,
    lm_eCadButtonPerfect,
	lm_eCadButtonPlagal,
    lm_eCadButtonImperfect,
    lm_eCadButtonDeceptive,
    lm_eCadButtonHalf,

    lm_eCadMaxButton
};


class lmCadencesConstrains : public lmExerciseOptions
{
public:
    lmCadencesConstrains(wxString sSection);
    ~lmCadencesConstrains() {}

    lmECadenceType GetRandomCadence();

    bool IsCadenceValid(lmECadenceType nType) { return m_fValidCadences[nType]; }
    void SetCadenceValid(lmECadenceType nType, bool fValid) { m_fValidCadences[nType] = fValid; }
    bool* GetValidCadences() { return m_fValidCadences; }

    bool IsValidButton(lmECadenceButtons nB) { return m_fValidButtons[nB]; }
    void SetValidButton(lmECadenceButtons nB, bool fValue) { m_fValidButtons[nB] = fValue; }
    bool* GetValidButtons() { return m_fValidButtons; }

    int GetKeyDisplayMode() { return m_nKeyDisplayMode; }
    void SetKeyDisplayMode(int nKeyDisplayMode) { m_nKeyDisplayMode = nKeyDisplayMode; }

    void SetSection(wxString sSection) {
                m_sSection = sSection;
                LoadSettings();
            }

    void SaveSettings();
    
    lmKeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }


private:
    void LoadSettings();

    bool                m_fValidButtons[lm_eCadMaxButton];
    bool                m_fValidCadences[lm_eCadMaxCadence];
    lmKeyConstrains     m_oValidKeys;           //allowed key signatures

    //params only for ear training exercises
    int                 m_nKeyDisplayMode;      // 0-play A4 note
                                                // 1-play tonic chord

};

#endif  // __LM_CADENCESCONSTRAINS_H__
