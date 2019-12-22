//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

//lomse
#include <lomse_pitch.h>
using namespace lomse;

//other
#include "vector"
#include "list"
using namespace std;


namespace lenmus
{

//forward declarations
class Chord;

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

//AWARE: any change in this enumeration requires the appropriate change in
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
// To know all errors and defects that a chord has it is necessary that errors can be
// stored in a compact way, as a bit mark
typedef long ChordError;

//values must be ordered by severity: higher numbers for higher severity
enum EBadChordReason
{
    k_chord_error_0_none = 0,
    k_chord_error_15_doubled_third          = 0x0001,
    k_chord_error_14_not_contrary_motion    = 0x0002,
    k_chord_error_13_greater_than_sixth     = 0x0004,
    k_chord_error_12_chromatic_acc          = 0x0008,
    k_chord_error_11_voice_overlap          = 0x0010,
    k_chord_error_10_notes_not_ascending    = 0x0020,
    k_chord_error_9_greater_than_octave     = 0x0040,
    k_chord_error_8_seventh_resolution      = 0x0080,
    k_chord_error_7_leading_resolution      = 0x0100,
    k_chord_error_6_leading_doubled         = 0x0200,
    k_chord_error_5_fifth_doubled           = 0x0400,
    k_chord_error_4_resultant_fifth_octave  = 0x0800,
    k_chord_error_3_fifth_octave_motion     = 0x1000,
    k_chord_error_2_fifth_missing           = 0x2000,
    k_chord_error_1_not_all_notes           = 0x4000,

    k_chord_error_max = 0xFFFF
};

//---------------------------------------------------------------------------------------
//All the data for a chord in a cadence
class CadenceChord
{
public:
    DiatonicPitch   nNote[4];   //diatonic pitch
    int             nAcc[4];    //accidentals: -2 ... +2
    EBadChordReason nReason;    //used when filtering out invalid chords
    ChordError      nSeverity;  //broken rules
    int             nImpact;    //0-internal voices, 1-external voices
    int             nNumNotes;  //how many notes in this chord, normally 4

public:
    CadenceChord()
    {
        nNote[0] = nNote[1] = nNote[2] = nNote[3] = NO_DPITCH;
        nAcc[0] = nAcc[1] = nAcc[2] = nAcc[3] = 0;
        nReason = k_chord_error_0_none;
        nNumNotes = 0;
    };

    //constructor for unit tests
    CadenceChord(const string& note1, const string& note2, const string& note3,
                 const string& note4)
    {
        FPitch fp1(note1);
        nNote[0] = fp1.to_diatonic_pitch();
        nAcc[0] = fp1.accidentals();
        FPitch fp2(note2);
        nNote[1] = fp2.to_diatonic_pitch();
        nAcc[1] = fp2.accidentals();
        FPitch fp3(note3);
        nNote[2] = fp3.to_diatonic_pitch();
        nAcc[2] = fp3.accidentals();
        FPitch fp4(note4);
        nNote[3] = fp4.to_diatonic_pitch();
        nAcc[3] = fp4.accidentals();

        nReason = k_chord_error_0_none;
        nNumNotes = 4;
    };

    string get_print_name(int iNote)
    {
        string sAnswer;
        switch(nAcc[iNote])
        {
            case -2: sAnswer ="--"; break;
            case -1: sAnswer ="-"; break;
            case 0:  sAnswer =" "; break;
            case 1:  sAnswer ="+"; break;
            case 2:  sAnswer ="++"; break;
            default:
                sAnswer = "";
        }
        sAnswer += nNote[iNote].get_english_note_name();
        return sAnswer;
    }

    void add_error(EBadChordReason errorId, int impact)
    {
        nReason = errorId;
        nSeverity |= errorId;
        nImpact = impact;
    }

};

#define k_max_notes_in_chord 12      // max notes in a set for a voice: 3 octaves * 4 notes
struct NoteSet
{
    FPitch pitch[k_max_notes_in_chord];
    int numNotes;
};

typedef  vector<CadenceChord>      ChordSet;

//---------------------------------------------------------------------------------------
//A cadence is a sequence of up to 2 chords
//Change this for more chords in a cadence
const int k_chords_in_cadence = 2;

class Cadence
{
protected:
    ECadenceType    m_nType;                            //cadence type
    EKeySignature   m_nKey;                             //key signature
    wxString        m_sFunction[k_chords_in_cadence];   //harmonic funcion
    CadenceChord    m_Chords[k_chords_in_cadence];      //cadence full chords
    int             m_nNumChords;                       //num of chords in this cadence

    //auxiliary data used while building the cadence chords
    Chord*  m_pTonicChord;
	int     m_nImperfectCad;    //when imperfect authentic cadence, its type (0..3)
    int     m_nInversions[k_chords_in_cadence];     //inversion required to build each cadence chord

public:
    Cadence();
    virtual ~Cadence();

    bool create(ECadenceType nCadenceType, EKeySignature nKey);

    inline ECadenceType get_cadence_type() { return m_nType; }
    inline int get_num_chords() { return m_nNumChords; }
	wxString get_name();
    string get_rel_ldp_name(int iChord, int iNote);
    Chord* get_tonic_chord();
    inline wxString& get_function(int iChord) { return m_sFunction[iChord]; }

    //global functions
    static ECadenceType name_to_type(wxString sCadence);
    static wxString type_to_name(ECadenceType nType);
    static FPitch get_root_note(const wxString& sFunct, EKeySignature nKey);
    static Chord* get_basic_chord_for(const wxString& sFunct, EKeySignature nKey);
    static string get_all_errors_reason(ChordError nError);
    static string get_error_description(ChordError nError);
    static int find_leading_tone_in_previous_chord(CadenceChord* pPrevChord,
                                                   int nStepLeading);
    static void get_chromatic_alterations(CadenceChord* pChord, EKeySignature nKey,
                                          int* pAlter);
    static bool check_chord(CadenceChord* pChord, Chord* pBasicChord,
                            CadenceChord* pPrevChord, EKeySignature nKey,
                            int nPrevAlter[4], int nStepLeading, int iLeading,
                            bool fExhaustive);

protected:

    void get_cadence_data(ECadenceType nCadenceType);
    bool generate_two_chords(int iC, const wxString& sFunct1, const wxString& sFunc2,
                             EKeySignature nKey, CadenceChord* pPrevChord);
    int generate_soprano_set(int iC, Chord* pBasicChord, NoteSet* pNoteSet);
    int generate_all_chords_for(int iChord, FPitch BNote, FPitch SNote,
                                NoteSet* pAltoSet, NoteSet* pTenorSet,
                                Chord* pBasicChord, CadenceChord* pPrevChord,
                                ChordSet* pValidChords);
	int generate_alto_set(int iC, FPitch fpSoprano, Chord* pBasicChord,
                          NoteSet* pNoteSet);
    int generate_tenor_set(int iC, Chord* pBasicChord, NoteSet* pNoteSet);
    int combine_notes_and_filter_chords(Chord* basicChord, FPitch B1, FPitch S1,
                                      NoteSet& AltoSet, NoteSet& TenorSet,
                                      CadenceChord* pPrevChord, ChordSet* pValidChords);

    void shuffle_set(NoteSet* pNoteSet, int numNotes);
    int filter_chords(ChordSet& allChords, ChordSet* pValidChords, Chord* pBasicChord,
                      int numChords, CadenceChord* pPrevChord, bool fExhaustive=false);
    static wxString get_chord_intervals(wxString sFunction, EKeySignature nKey,
                                        int* pInversion);

    //validation rules
    static void rule_1_all_steps_in_chord(CadenceChord* pChord, Chord* pBasicChord);
    static void rule_3_no_parallel_motion_fifths_octaves(CadenceChord* pChord,
                                           list< pair<int,int> >& couples,
                                           FIntval motionIntv[4]);
    static void rule_4_resultant_fifth_octave(CadenceChord* pChord,
                                              CadenceChord* pPrevChord,
                                              int motionSteps[4]);
    static void rule_5_6_fifth_leading_not_doubled(CadenceChord* pChord,
                                                   Chord* pBasicChord,
                                                   int nStepLeading);
    static void rule_7_leading_resolution(CadenceChord* pChord, int leadingMotion,
                                          int iLeading);
    static void rule_8_seventh_resolution(CadenceChord* pChord, int leadingMotion,
                                          int iLeading);
    static void rule_9_intervals_not_greater_than_octave(CadenceChord* pChord);
    static void rule_10_no_voice_crossing(CadenceChord* pChord);
    static void rule_11_no_voice_overlap(CadenceChord* pChord, CadenceChord* pPrevChord);
    static void rule_12_chromatic_accidentals(CadenceChord* pChord, int nPrevAlter[4],
                                              CadenceChord* pPrevChord);
    static void rule_14_not_contrary_motion(CadenceChord* pChord, int motionSteps[4]);


    //debug
    void dump_chord(CadenceChord& oChord, int iChord, bool fAllErrors=false);

};

//global methods for unit tests, for accessing and validating internal tables
extern wxString dbg_get_function_for_cadence(int iF, ECadenceType nCadenceType);
extern wxString dbg_get_function_for_imperfect(int iF, int iInv);
extern wxString dbg_get_function_in_conversion_table(int i);
extern bool dbg_function_is_defined(wxString& sFunct);


}   //namespace lenmus

#endif  // __LENMUS_CADENCE_H__

