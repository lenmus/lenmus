//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#ifndef __LENMUS_CADENCE_H__        //to avoid nested includes
#define __LENMUS_CADENCE_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_interval.h"
#include "lenmus_conversion.h"

////wxWidgets
//#include <wx/wxprec.h>
//#include <wx/wx.h>

//lomse
#include <lomse_pitch.h>
using namespace lomse;

////other
//#include "vector"


namespace lenmus
{

//forward declarations
class Chord;

//---------------------------------------------------------------------------------------
// Auxiliary, to store notes data
struct NoteBits
{
    int nStep;              // 'c'=0, 'd'=1, 'e'=2, 'f'=3, 'g'=4, 'a'=5, 'b'=6
    int nOctave;            // 0..9
    int nAccidentals;       // '--'=-1, '-'=-1, ''=0, '+'=+1, '++'=+2
    int nStepSemitones;     // 'c'=0, 'd'=2, 'e'=4, 'f'=5, 'g'=7, 'a'=9, 'b'=11
};

//---------------------------------------------------------------------------------------
// enum to assign code to each cadence
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
//      method IdfyCadencesCtrolParams::CadenceNameToType()

enum ECadenceType
{
    // Terminal cadences
    k_cadence_terminal = 0,    //Start of terminal cadences

    // Start of Perfect authentic cadences
    k_cadence_perfect = k_cadence_terminal,
    k_cadence_perfect_V_I = k_cadence_perfect,
    k_cadence_perfect_V7_I,
    k_cadence_perfect_Va5_I,
    k_cadence_perfect_Vd5_I,
    k_cadence_last_perfect,

    // Start of Plagal cadences
	k_cadence_plagal = k_cadence_last_perfect,
    k_cadence_plagal_IV_I = k_cadence_plagal,
    k_cadence_plagal_IVm_I,
    k_cadence_plagal_IIc6_I,
    k_cadence_plagal_IImc6_I,
    k_cadence_last_plagal,

    // Imperfect authentic cadences
	k_cadence_imperfect = k_cadence_last_plagal,
	k_cadence_imperfect_V_I = k_cadence_imperfect,
    k_cadence_last_imperfect,

    k_cadence_last_terminal = k_cadence_last_imperfect,    // last terminal cadence

    // Transient cadences
    k_cadence_transient = k_cadence_last_terminal,

    // Deceptive cadences
    k_cadence_deceptive = k_cadence_transient,
    k_cadence_deceptive_V_IV = k_cadence_deceptive,
    k_cadence_deceptive_V_IVm,
    k_cadence_deceptive_V_VI,
    k_cadence_deceptive_V_VIm,
    k_cadence_deceptive_V_IIm,
    k_cadence_deceptive_V_III,
    k_cadence_deceptive_V_VII,
    k_cadence_last_deceptive,

    // Half cadences
    k_cadence_half = k_cadence_last_deceptive,
    k_cadence_half_IImc6_V = k_cadence_half,
    k_cadence_half_IV_V,
    k_cadence_half_I_V,
    k_cadence_half_Ic64_V,
    k_cadence_half_IV6_V,
    k_cadence_half_II_V,
    k_cadence_half_IIdimc6_V,
    k_cadence_half_VdeVdim5c64_V,
    k_cadence_last_half,

    k_cadence_last_transient = k_cadence_last_half,

    //last element, to signal End Of Table
    k_cadence_max = k_cadence_last_transient,
};

//---------------------------------------------------------------------------------------
// auxiliary object to comfortably manage chords

// For selecting the less bad chrd it is necessary to know all errors and defects that a chord
// has. To this end , each defect will be stored as a bit mark
#define lmChordError    long

// Values for defects must be assigned by severity: higher numbers for higher severity
enum EBadChordReason
{
    lm_eChordValid = 0,
    lm_eNotDoubledThird     = 0x0001,   //Cuando el bajo enlaza el V grado con el VI (cadencia rota), en el acorde de VI grado se duplica la tercera.
    lm_eNotContraryMotion   = 0x0002,   //bass moves by step and not all other voices moves in opposite direction to bass
    lm_eGreaterThanSixth    = 0x0004,   //No es conveniente exceder el intervalo de sexta, exceptuando la octava justa
    lm_eChromaticAlter      = 0x0008,   //Chromatic alteration not resolved the same direction than the alteration
    lm_eVoiceOverlap        = 0x0010,   //voice overlap
    lm_eVoiceCrossing       = 0x0020,   //notes not in ascending sequence or duplicated
    lm_eGreaterThanOctave   = 0x0040,   //notes interval greater than one octave (other than bass-tenor)
    lm_eSeventhResolution   = 0x0080,   //the seventh of a chord should always resolve down by second.
    lm_eLeadingResolution   = 0x0100,   //Scale degree seven (the leading tone) doesn't resolve to tonic
    lm_eLeadingToneDoubled  = 0x0200,   //the leading tone is doubled
    lm_eFifthDoubled        = 0x0400,   //the fifth is doubled
    lm_eResultantFifthOctves = 0x0800,  //3. No hacer 5ªs ni 8ªs resultantes, excepto:
                                            //> a) la soprano se ha movido por segundas
                                            //> b) (para 5ªs) uno de los sonidos ya estaba
    lm_eFifthOctaveMotion   = 0x1000,   //parallel motion of perfect octaves or perfect fifths
    lm_eFifthMissing        = 0x2000,   // Acorde completo. Contiene todas las notas (en todo caso, elidir la 5ª)
    lm_eNotAllNotes         = 0x4000,   //not all chord steps in the chord
    lm_eChordDiscarded      = 0x8000,   //Chord discarded: not possible to generate a valid next chord

    lm_eMaxSeverity         = 0x8888,
};

class lmHChord
{
public:

    lmHChord()
    {
        nNote[0] = nNote[1] = nNote[2] = nNote[3] = NO_DPITCH;
        nAcc[0] = nAcc[1] = nAcc[2] = nAcc[3] = 0;
        nReason=lm_eChordValid;
        nNumNotes=0;
    };

    wxString GetPrintName(int iNote) {
        wxString sAnswer;
        switch(nAcc[iNote])
        {
            case -2: sAnswer =_T("--"); break;
            case -1: sAnswer =_T("-"); break;
            case 0:  sAnswer =_T(" "); break;
            case 1:  sAnswer =_T("+"); break;
            case 2:  sAnswer =_T("++"); break;
            default:
                sAnswer = _T("");
        }
        sAnswer += to_wx_string( nNote[iNote].get_english_note_name() );
        return sAnswer;
    }


    DiatonicPitch   nNote[4];   //diatonic pitch so 1:1 mapping to staff lines/spaces
    int             nAcc[4];    //accidentals: -2 ... +2
    EBadChordReason nReason;    //used when filtering out invalid chords
    lmChordError    nSeverity;  //broken rules
    int             nImpact;    //0-internal voices, 1-external voices
    int             nNumNotes;  //normally 4
};

//---------------------------------------------------------------------------------------
typedef struct lmChordAuxDataStruct {
	int nSteps[4];	// the steps of the chord
    int nStep5;     // the fifth of the chord
	int nNumSteps;	// how many steps this chord has (3 or 4)
} lmChordAuxData;

//declare global functions defined in this module
extern wxString CadenceTypeToName(ECadenceType nType);
extern wxString GetChordErrorDescription(lmChordError nError);

//---------------------------------------------------------------------------------------
//A cadence is a sequence of up to 2 chords
//Change this for more chords in a cadence
#define k_chords_in_cadence  2

class Cadence
{
public:
    Cadence();
    ~Cadence();

    bool Create(ECadenceType nCadenceType, EKeySignature nKey, bool fUseGrandStaff);
    bool IsCreated() { return m_fCreated; }

    inline ECadenceType GetCadenceType() { return m_nType; }
    int GetNumChords() { return m_nNumChords; }
    Chord* GetChord(int iC);
	wxString GetName();
    string get_rel_ldp_name(int iChord, int iNote);
    Chord* GetTonicChord();

    //global functions
    static ECadenceType CadenceNameToType(wxString sCadence);

private:
    wxString SelectChord(wxString sFunction, EKeySignature nKey, int* pInversion);
    FPitch GetRootNote(wxString sFunct, EKeySignature nKey, EClef nClef,
                       bool fUseGrandStaff);

    int GenerateFirstChord(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
						   Chord* pChord, int nInversion);
    int GenerateNextChord(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
						  Chord* pChord, int nInversion, int iPrevHChord);
    int FilterChords(std::vector<lmHChord>& aChords, int nNumChords, lmChordAuxData& tChordData,
                     lmHChord* pPrevChord, bool fExhaustive=false);
    void SelectLessBad(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
					   int iHChord);
    int GenerateSopranoNote(NoteBits oChordNotes[4], int iBass, int nNumNotes);


	// Debug methods
	void Debug_DumpAllChords(std::vector<lmHChord>& aChords);
	void Debug_DumpChord(lmHChord& oChord, int iChord=0);



    //member variables

    bool            m_fCreated;
    ECadenceType    m_nType;
	int				m_nImperfectCad;
    EKeySignature   m_nKey;
    Chord*          m_pTonicChord;
    Chord*          m_pChords[k_chords_in_cadence];
    int             m_nInversions[k_chords_in_cadence];
    int             m_nNumChords;       //num of chords in this cadence
    lmHChord        m_Chord[k_chords_in_cadence];

};


}   //namespace lenmus

#endif  // __LENMUS_CADENCE_H__

