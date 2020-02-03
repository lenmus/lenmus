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

//lenmus
#include "lenmus_cadence.h"
#include "lenmus_standard_header.h"

#include "lenmus_chord.h"
#include "lenmus_generators.h"
#include "lenmus_utilities.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_score_utilities.h>
using namespace lomse;

//set to 1 for tracing cadence generation
#define TRACE_CADENCE  2        //0=no trace, 1=main trace, 2=also details
#if (LENMUS_COMPILER_MSVC == 1)     //VC++ 2003 does not support variadic macros
    #if (TRACE_CADENCE > 0)
        #define TraceCadence    wxLogMessage
    #else
        #define TraceCadence    __noop
    #endif
#else
    #if (TRACE_CADENCE > 0)
        #define TraceCadence    LOMSE_LOG_INFO
    #else
        inline void DoNothing() {}
        #define TraceCadence(a, args ...)           DoNothing()
    #endif
#endif



namespace lenmus
{

//---------------------------------------------------------------------------------------
// Conversion table: harmonic function to chord intervals.

// an entry for the table to convert harmonic function to chord intervals
struct FunctionData
{
    wxString    sFunction;      // harmonic function
    wxString    sChordMajor;    // nil if not usable
    wxString    sChordMinor1;   // alternative 1 or nil if not usable in minor mode
    wxString    sChordMinor2;   // alternative 2 or nil if no alternative
};


// the table.
// Inversions are not necessary except in minor modes. In these cases the inversion
// is notated without the slash (e.g: IVm6 instead of IVm/6)
// AWARE: The maximum number of notes in a chord is defined in 'Chord.h', constant
//        k_notes_in_chord. Currently its value is 7. Change this value if you need more
//        notes.
//
static const FunctionData m_aFunctionToIntervals[] =
{
    //                          minor key   minor key
    //Function  Major key       option 1    option 2
    //--------- -------------   ----------  --------------
    {"I",       "M3,p5",        "m3,p5",    "nil" },
    {"II",      "m3,p5",        "m3,d5",    "m3,p5" },
    {"IIm",     "m3,p5",        "nil",      "nil" },
    {"III",     "m3,p5",        "M3,p5",    "M3,a5" },
    {"IV",      "M3,p5",        "m3,p5",    "M3,p5" },
    {"IVm",     "m3,p5",        "nil",      "nil" },
    {"IVm6",    "nil",          "M3,M6",    "nil" },    // IVm/6 used in Phrygian half cadence
    {"V",       "M3,p5",        "M3,p5",    "m3,p5" },
    {"V7",      "M3,p5,m7",     "M3,p5,m7", "nil" },
    {"Va5",     "M3,a5",        "nil",      "nil" },
    {"Vd5",     "M3,d5",        "M3,d5",    "m3,d5" },
    {"VI",      "m3,p5",        "M3,p5",    "#,m3,d5" },
    {"VIm",     "m3,d5",        "nil",      "nil" },
    {"VII",     "m3,d5",        "#,m3,d5",  "M3,p5" },
    {"IIb6",    "m3,m6",        "m3,m6",    "nil" },    // IIb/6 used in neapolitan sixth
    {"IId",     "m3,d5",        "nil",      "nil" },    // IId used in augmented sixth
//    {"IId/6", ????????},
};


//---------------------------------------------------------------------------------------
static wxString m_sCadenceName[k_cadence_max+1];


//---------------------------------------------------------------------------------------
// AWARE: Array indexes are in correspondence with enum ECadenceType
// AWARE: Change constant k_chords_in_cadence to increment number of chords
//
static const wxString aFunction[k_cadence_max][k_chords_in_cadence] =
{
    // Perfect authentic cadences
    { "V",      "I" },      //Perfect_V_I
    { "V7",     "I" },      //Perfect_V7_I
    { "Va5",    "I" },      //Perfect_Va5_I
    { "Vd5",    "I" },      //Perfect_Vd5_I
    // Plagal cadences
    { "IV",     "I" },      //Plagal_IV_I
    { "IVm",    "I" },      //Plagal_IVm_I
    { "II/6",   "I" },      //Plagal_IIc6_I
    { "IIm/6",  "I" },      //Plagal_IImc6_I
    // Imperfect authentic cadences
    // AWARE: This entry is not used. Inversions are encoded in aImperfect[]
    { "V",      "I" },      //Imperfect_V_I
    // Deceptive cadences
    { "V",      "IV" },     //Deceptive_V_IV
    { "V",      "IVm" },    //Deceptive_V_IVm
    { "V",      "VI" },     //Deceptive_V_VI
    { "V",      "VIm" },    //Deceptive_V_VIm
    { "V",      "IIm" },    //Deceptive_V_IIm
    { "V",      "III" },    //Deceptive_V_III
    { "V",      "VII" },    //Deceptive_V_VII
    // Half cadences
    { "IIm/6",  "V" },      //Half_IImc6_V
    { "IV",     "V" },      //Half_IV_V
    { "I",      "V" },      //Half_I_V
    { "I/64",   "V" },      //Half_Ic64_V
    { "IVm6",   "V" },      //Half_IV6_V (Phrygian)
    { "II",     "V" },      //Half_II_V
    { "IId/6",  "V" },      //Half_IIdimc6_V (Neapolitan sixth)
    { "IIb6",   "V" },      //Half_VdeVdim5c64_V (augmented sixth)
};

//---------------------------------------------------------------------------------------
// Chords for imperfect cadence
static const wxString aImperfect[4][k_chords_in_cadence] =
{
    { "V/6",    "I" },
    { "V/64",   "I" },
    { "V/64",   "I/6" },
    { "V",      "I/64" },
};

#define k_exhaustive  true      //for FilterChords() method



//---------------------------------------------------------------------------------------
wxString dbg_get_function_in_conversion_table(int i)
{
    int iMax = sizeof(m_aFunctionToIntervals)/sizeof(FunctionData);
    if (i < iMax)
        return m_aFunctionToIntervals[i].sFunction;
    else
        return wxEmptyString;
}

//---------------------------------------------------------------------------------------
wxString dbg_get_function_for_cadence(int iF, ECadenceType nCadenceType)
{
    return aFunction[nCadenceType][iF];
}

//---------------------------------------------------------------------------------------
wxString dbg_get_function_for_imperfect(int iF, int iInv)
{
    return aImperfect[iInv][iF];
}

//---------------------------------------------------------------------------------------
bool dbg_function_is_defined(wxString& sFunc)
{
    int iF, iMax = sizeof(m_aFunctionToIntervals)/sizeof(FunctionData);
    for (iF=0; iF < iMax; iF++)
    {
        if (m_aFunctionToIntervals[iF].sFunction == sFunc)
            return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------
// Implementation of Cadence class
//---------------------------------------------------------------------------------------
Cadence::Cadence()
    : m_nNumChords(0)
    , m_pTonicChord(nullptr)
    , m_nImperfectCad(0)
{
}

//---------------------------------------------------------------------------------------
Cadence::~Cadence()
{
    if (m_pTonicChord)
        delete m_pTonicChord;
}

//---------------------------------------------------------------------------------------
bool Cadence::create(ECadenceType nCadenceType, EKeySignature nKey)
{
    //Parameters received:
    // - the required cadence type
    // - the key signature
    // return true if cadence created

    TraceCadence("Starting generation of a chord progression =========================");

    //save parameters
    m_nType = nCadenceType;
    m_nKey = nKey;

	//get all data for the cadence
    get_cadence_data(nCadenceType);

    //generate the cadence              0 = chords 0 & 1
    bool fSuccess = generate_two_chords(0, m_sFunction[0], m_sFunction[1], m_nKey,
                                        nullptr);

    #if (TRACE_CADENCE > 0)
    if (fSuccess)
    {
        TraceCadence("============> Selected as first chord: %s, %s, %s, %s",
                     m_Chords[0].get_print_name(0).c_str(),
                     m_Chords[0].get_print_name(1).c_str(),
                     m_Chords[0].get_print_name(2).c_str(),
                     m_Chords[0].get_print_name(3).c_str() );
        TraceCadence("============> Selected as second chord: %s, %s, %s, %s",
                     m_Chords[1].get_print_name(0).c_str(),
                     m_Chords[1].get_print_name(1).c_str(),
                     m_Chords[1].get_print_name(2).c_str(),
                     m_Chords[1].get_print_name(3).c_str() );
    }
    #endif

    return fSuccess;
}

//---------------------------------------------------------------------------------------
bool Cadence::generate_two_chords(int iC, const wxString& sFunct1,
                                  const wxString& sFunc2, EKeySignature nKey,
                                  CadenceChord* pPrevChord)
{
    //iC is the index for storing in the cadence the first generated chord.
    //The second one will be stored in index iC+1
    int iC0 = iC;
    int iC1 = ++iC;

    //generate the basic chords with the note steps
    Chord* pBasicChord[2];
    pBasicChord[0] = get_basic_chord_for(sFunct1, nKey);
    pBasicChord[1] = get_basic_chord_for(sFunc2, nKey);

	//First cadence chord: try all possible soprano notes
    NoteSet SopranoSet0;    //all possible notes for the soprano voice
    NoteSet AltoSet0;       //all possible notes for the alto voice
    NoteSet TenorSet0;      //all possible notes for the tenor voice

	FPitch B0 = pBasicChord[0]->get_note(0);
    generate_soprano_set(iC0, pBasicChord[0], &SopranoSet0);
	int nS0 = SopranoSet0.numNotes;
	for (int i1=0; i1 < nS0; ++i1)
	{
        ChordSet chordSet0;
		FPitch S0 = SopranoSet0.pitch[i1];
        int nSet0 = generate_all_chords_for(iC0, B0, S0, &AltoSet0, &TenorSet0,
                                            pBasicChord[0], pPrevChord, &chordSet0);

		//Second cadence chord generation
        NoteSet SopranoSet1;    //all possible notes for the soprano voice
        NoteSet AltoSet1;       //all possible notes for the alto voice
        NoteSet TenorSet1;      //all possible notes for the tenor voice

        FPitch B1 = pBasicChord[1]->get_note(0);
        generate_soprano_set(iC1, pBasicChord[1], &SopranoSet1);
		//loop to try for first chord all valid chords in chordSet0
		for (int j1=0; j1 < nSet0; ++j1)
		{
            TraceCadence("Loop2 start. Chord 1: Try valid chord %d as chord 0", j1);
			CadenceChord chord0 = chordSet0[j1];

			//Second chord: try all possible soprano notes
            int nS1 = SopranoSet1.numNotes;
			for (int i2=0; i2 < nS1; ++i2)
			{
                ChordSet chordSet1;
                FPitch S1 = SopranoSet1.pitch[i2];
                int nSet1 = generate_all_chords_for(iC1, B1, S1, &AltoSet1, &TenorSet1,
                                                    pBasicChord[1], &chord0, &chordSet1);
				if (nSet1 > 0)
                {
                    m_Chords[iC0] = chord0;
                    m_Chords[iC1] = (nSet1 == 1 ? chordSet1[0] :
                        chordSet1[RandomGenerator::random_number(0,nSet1-1)] );

                    delete pBasicChord[0];
                    delete pBasicChord[1];

                    return true;    //cadence created
                }
			}
			//if arrives here, no valid chord2 for chord1.
			//Choose another chord for chord1 (next loop iteration)
		}
		//if arrives here, not possible to generate a valid chord progression with soprano note S1.
		//Chose another note for S1 (next loop iteration)
	}
	//if arrives here, not possible to generate a valid chord progression. What to do?
    LOMSE_LOG_ERROR("Not possible to generate a valid chord progression. "
                    "Cadence type = %d, key=%d", m_nType, m_nKey);

    delete pBasicChord[0];
    delete pBasicChord[1];

    return false;   //cadence not created
}

//---------------------------------------------------------------------------------------
int Cadence::generate_all_chords_for(int iChord, FPitch BNote, FPitch SNote,
                                     NoteSet* pAltoSet, NoteSet* pTenorSet,
                                     Chord* pBasicChord, CadenceChord* pPrevChord,
                                     ChordSet* pValidChords)
{
    TraceCadence("Chord %d: Bass %s, trying %s for soprano", iChord,
                 BNote.to_abs_ldp_name().c_str(), SNote.to_abs_ldp_name().c_str());

    generate_alto_set(iChord, SNote, pBasicChord, pAltoSet);
    generate_tenor_set(iChord, pBasicChord, pTenorSet);

    combine_notes_and_filter_chords(pBasicChord, BNote, SNote, *pAltoSet,
                                  *pTenorSet, pPrevChord, pValidChords);

    int nSet = int(pValidChords->size());
    #if (TRACE_CADENCE > 0)
        TraceCadence("Generated %d valid chords", nSet);
        for (int i=0; i < nSet; i++)
        {
            TraceCadence("Valid chord %d : %s, %s, %s, %s",
                         i,
                         pValidChords->at(i).get_print_name(0).c_str(),
                         pValidChords->at(i).get_print_name(1).c_str(),
                         pValidChords->at(i).get_print_name(2).c_str(),
                         pValidChords->at(i).get_print_name(3).c_str() );
        }
    #endif

    return nSet;
}

//---------------------------------------------------------------------------------------
void Cadence::get_cadence_data(ECadenceType nCadenceType)
{
	//from cadence type gets the harmonic functions, and the number of chords
	//in the cadence.Also generates, at random, the inversion number, just in case
	//it would be necessary.
	//
	//Results are stored in member variables m_nNumChords and m_function[]
    //
	//If any error found, it is logged but the offending function will be replaced by
	//'I' to avoid propagating errors.

	//select an imperfect authentic cadence
    m_nImperfectCad = RandomGenerator::random_number(0, 3);

    //find harmonic functions for this cadence type
    for (int iC=0; iC < k_chords_in_cadence; iC++)
    {
        wxString sFunct;
        if (nCadenceType == k_cadence_imperfect_V_I)
            sFunct = aImperfect[m_nImperfectCad][iC];
        else
            sFunct = aFunction[nCadenceType][iC];

        // if no function, exit loop. No more chords in cadence
        if (sFunct == "")
        {
            if (iC > 1)
                break;      //no error

            //table maintenace error
            LOMSE_LOG_ERROR("Table maintenace error. No harmonic function!. "
                            "nCadenceType=%d, iC=%d", nCadenceType, iC);
            sFunct = "I";   //use I
        }
        m_sFunction[iC] = sFunct;

        m_nNumChords++;
    }
}

//---------------------------------------------------------------------------------------
Chord* Cadence::get_basic_chord_for(const wxString& sFunct, EKeySignature nKey)
{
    //get the chord intervals
    int nInversion;
    wxString sIntervals = get_chord_intervals(sFunct, nKey, &nInversion);
    TraceCadence("sFunct='%s', chord intervals='%s'",
                 to_std_string(sFunct).c_str(), to_std_string(sIntervals).c_str() );
    if (sIntervals == "")
    {
        //error: no chord for function. Use function 'I' to avoid propagating errors
        LOMSE_LOG_ERROR("No chord found for function '%s', nKey=%d",
                        to_std_string(sFunct).c_str(), nKey);
        sIntervals = get_chord_intervals("I", nKey, &nInversion);
    }


    //Get root note for this key signature
    FPitch fpRootNote = get_root_note(sFunct, nKey);
    TraceCadence("sFunc='%s', nKey=%d, sRootNote='%s'",
                 to_std_string(sFunct).c_str(), nKey,
                 fpRootNote.to_abs_ldp_name().c_str());

    //Prepare the chord
    return LENMUS_NEW Chord(fpRootNote, sIntervals, nKey);
}

//---------------------------------------------------------------------------------------
FPitch Cadence::get_root_note(const wxString& sFunct, EKeySignature nKey)
{
    //Get root note for this key signature
    int step = KeyUtilities::get_step_for_root_note(nKey);    //0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si

    // add function grade
    size_t nSize = sFunct.length();
    if (nSize > 2)
    {
        if (sFunct.compare(0, 3, "VII") == 0)
            step += 6;
        else if (sFunct.compare(0, 3, "III") == 0)
            step += 2;
        else if (sFunct.compare(0, 2, "VI") == 0)
            step += 5;
        else if (sFunct.compare(0, 2, "IV") == 0)
            step += 3;
        else if (sFunct.compare(0, 2, "II") == 0)
            step += 1;
        else if (sFunct.compare(0, 1, "V") == 0)
            step += 4;
    }
    else if (nSize > 1)
    {
        if (sFunct.compare(0, 2, "VI") == 0)
            step += 5;
        else if (sFunct.compare(0, 2, "IV") == 0)
            step += 3;
        else if (sFunct.compare(0, 2, "II") == 0)
            step += 1;
        else if (sFunct.compare(0, 1, "V") == 0)
            step += 4;
    }
    else if (sFunct.compare(0, 1, "V") == 0)
        step += 4;

    step = step % 7;

    //Get accidentals for this note
    int nAccidentals[7];
    KeyUtilities::get_accidentals_for_key(nKey, nAccidentals);
    int acc = nAccidentals[step];

    //The bass voice range, according to most theorists, is e2-d4
    //Therefore, the valid octaves are: C,D - 3,4 and  E,F,G,A,B = 2,3
    //But this method will only consider notes: g2,a2,b2,c3,d3,e3,f3 better
    //suited for grand staff.
    int octave = (step >= k_step_G ? 2 : 3);

    return FPitch(step, octave, acc);
}

//---------------------------------------------------------------------------------------
wxString Cadence::get_chord_intervals(wxString sFunction, EKeySignature nKey, int* pInversion)
{
    // returns the intervals that form the chord, or empty string if errors
    // and updates variable pointed by pInversion to place the number of the
    // applicable inversion

    //Strip out inversions
    wxString sFunc;
    int iSlash = sFunction.find('/');
    if (iSlash !=  wxNOT_FOUND)
    {
        sFunc = sFunction.substr(0, iSlash);
        wxString sInv = sFunction.substr(iSlash+1);
        if (sInv=="6")
            *pInversion = 1;
        else if (sInv=="64")
            *pInversion = 2;
        else
        {
            LOMSE_LOG_ERROR("Conversion table maintenance error. Unknown inversion "
                            "code '%s'", to_std_string(sInv).c_str());
            *pInversion = 0;
        }
    }
    else
    {
        sFunc = sFunction;
        *pInversion = 0;
    }

    // look for function
    int iF, iMax = sizeof(m_aFunctionToIntervals)/sizeof(FunctionData);
    for (iF=0; iF < iMax; iF++)
    {
        if (m_aFunctionToIntervals[iF].sFunction == sFunc) break;
    }
    if (iF == iMax)
    {
        // table maintenance error
        LOMSE_LOG_ERROR("Conversion table maintenance error. Function '%s' not found. "
                        "Key=%d", to_std_string(sFunction).c_str(), nKey);
        return "";      // not valid chord
    }

    if (KeyUtilities::is_major_key(nKey))
    {
        // major key: return chord for major key
        wxString sChord = m_aFunctionToIntervals[iF].sChordMajor;
        if (sChord == "nil")
        {
            LOMSE_LOG_ERROR("Conversion table maintenance error. Not valid function '"
                            "%s' for major key=%d (%s)",
                            to_std_string(sFunction).c_str(), nKey,
                            to_std_string(get_key_signature_name(nKey)).c_str() );
            return "";        // not valid chord
        }
        if (sChord == "?")
        {
            LOMSE_LOG_ERROR("Conversion table maintenance error. Undefined chord. "
                            "Function '%s', Key=%d (%s)",
                            to_std_string(sFunction).c_str(), nKey,
                            to_std_string(get_key_signature_name(nKey)).c_str() );
            return "";        // not valid chord
        }
        else
            return sChord;
    }
    else
    {
        // minor key: return one of the chords for minor key
        wxString sChord1 = m_aFunctionToIntervals[iF].sChordMinor1;
        wxString sChord2 = m_aFunctionToIntervals[iF].sChordMinor2;
        if (sChord1 == "nil")
        {
            LOMSE_LOG_ERROR("Conversion table maintenance error. Not valid function '"
                            "%s' for minor key=%d (%s)",
                            to_std_string(sFunction).c_str(), nKey,
                            to_std_string(get_key_signature_name(nKey)).c_str() );
            return "";        // not valid chord
        }
        else if (sChord2 == "nil")
            return sChord1;
        else
        {
            RandomGenerator oRnd;
            if (oRnd.flip_coin())
                return sChord1;
            else
                return sChord2;
        }
    }
}

//---------------------------------------------------------------------------------------
int Cadence::generate_soprano_set(int iC, Chord* pBasicChord, NoteSet* pNoteSet)
{
    // Generate the set of possible notes for Soprano voice, ordered at random.
    // Returns the number of generated notes

    // Range allowed for soprano, both notes inclusive: d4-a5
    DiatonicPitch nMinSopranoPitch(k_step_D, 4);
    DiatonicPitch nMaxSopranoPitch(k_step_A, 5);

    FPitch fpRootNote = pBasicChord->GetNormalizedRoot();
    int stepB = fpRootNote.step();

    int nNumNotes = pBasicChord->get_num_notes();

    int iN = 0;
    #if (TRACE_CADENCE == 2)
        stringstream ssS;
    #endif
    for (int i=0; i < nNumNotes; i++)
    {
        int step = pBasicChord->get_step(i);
        if (step != stepB)
        {
            DiatonicPitch dp(step, 4);
            int acc = pBasicChord->get_num_accidentals(i);
            while (dp < nMaxSopranoPitch)
            {
                if (dp >= nMinSopranoPitch)
                {
                    FPitch fp(dp, acc);
                    pNoteSet->pitch[iN] = fp;
                    ++iN;
                    #if (TRACE_CADENCE == 2)
                        ssS << fp.to_abs_ldp_name() << " ";
                    #endif
                }
                dp += 7;
            }
        }
    }
    pNoteSet->numNotes = iN;
    #if (TRACE_CADENCE == 2)
        TraceCadence("Soprano set (%d) = %s", iN, ssS.str().c_str());
    #endif

    shuffle_set(pNoteSet, iN);

    #if (TRACE_CADENCE > 0)
        stringstream ssS2;
        for (int i=0; i < iN; ++i)
            ssS2 << pNoteSet->pitch[i].to_abs_ldp_name() << " ";
        TraceCadence("Chord %d: shuffled soprano set = %s", iC, ssS2.str().c_str());
    #endif

    return iN;
}

//---------------------------------------------------------------------------------------
int Cadence::generate_alto_set(int WXUNUSED(iC), FPitch fpSoprano, Chord* pBasicChord,
                               NoteSet* pNoteSet)
{
    // Generate the set of possible notes for Alto voice, ordered at random.
    // Returns the number of generated notes

    // Range allowed for alto voice, both notes inclusive: g3-d5
    DiatonicPitch dpMinAlto(k_step_G, 3);
    DiatonicPitch dpMaxAlto(k_step_D, 5);

    DiatonicPitch dpSoprano = fpSoprano.to_diatonic_pitch();
    int nNumNotes = pBasicChord->get_num_notes();

    int iN = 0;
    #if (TRACE_CADENCE == 2)
        stringstream ss;
    #endif
    for (int i=0; i < nNumNotes; i++)
    {
        int step = pBasicChord->get_step(i);
        int acc = pBasicChord->get_num_accidentals(i);
        DiatonicPitch dpAlto(step, 3);
        while (dpAlto < dpSoprano && dpAlto < dpMaxAlto)
        {
            if (dpAlto >= dpMinAlto)
            {
                FPitch fp(dpAlto, acc);
                pNoteSet->pitch[iN++] = fp;
                #if (TRACE_CADENCE == 2)
                    ss << fp.to_abs_ldp_name() << " ";
                #endif
            }
            dpAlto += 7;
        }
    }
    pNoteSet->numNotes = iN;
    #if (TRACE_CADENCE == 2)
        TraceCadence("Alto set (%d) = %s", iN, ss.str().c_str());
    #endif

    return iN;
}

//---------------------------------------------------------------------------------------
int Cadence::generate_tenor_set(int WXUNUSED(iC), Chord* pBasicChord, NoteSet* pNoteSet)
{
    //all chord notes are possible. Bass <= Tenor <= Alto

    // Range allowed for tenor voice, both notes inclusive: g3-d5
    DiatonicPitch dpMinTenor(k_step_D, 3);
    DiatonicPitch dpMaxTenor(k_step_G, 4);

    int nNumNotes = pBasicChord->get_num_notes();

    int iN = 0;
    #if (TRACE_CADENCE == 2)
        stringstream ss;
    #endif
    for (int i=0; i < nNumNotes; i++)
    {
        int step = pBasicChord->get_step(i);
        int acc = pBasicChord->get_num_accidentals(i);
        DiatonicPitch dpTenor(step, 3);
        while (dpTenor < dpMaxTenor)
        {
            if (dpTenor >= dpMinTenor)
            {
                FPitch fp(dpTenor, acc);
                pNoteSet->pitch[iN++] = fp;
                #if (TRACE_CADENCE == 2)
                    ss << fp.to_abs_ldp_name() << " ";
                #endif
            }
            dpTenor += 7;
        }
    }
    pNoteSet->numNotes = iN;
    #if (TRACE_CADENCE == 2)
        TraceCadence("Tenor set (%d) = %s", iN, ss.str().c_str());
    #endif
    return iN;
}

//---------------------------------------------------------------------------------------
void Cadence::shuffle_set(NoteSet* pNoteSet, int numNotes)
{
    int* idx = new int[numNotes];
    RandomGenerator::shuffle(numNotes, idx);
    #if (TRACE_CADENCE == 2)
        for (int i=0; i < numNotes; ++i)
            TraceCadence("idx = %d", *(idx+i));
    #endif

    FPitch* fp = new FPitch[numNotes];
    for (int i=0; i < numNotes; ++i)
        *(fp+i) = pNoteSet->pitch[i];

    for (int i=0; i < numNotes; ++i)
        pNoteSet->pitch[i] = *(fp + *(idx+i));

    delete[] idx;
    delete[] fp;
}

//---------------------------------------------------------------------------------------
int Cadence::combine_notes_and_filter_chords(Chord* pBasicChord, FPitch B1, FPitch S1,
                                            NoteSet& AltoSet,
                                            NoteSet& TenorSet,
                                            CadenceChord* pPrevChord,
                                            ChordSet* pValidChords)
{
    //Combine notes to generate the set of possible chords
    //Filter possible chords and generate the set of valid chords.
    //If prevChord == nullptr rules for linking chords are not apply, only rules for
    //well formed chords.
    //
    //Return the set of valid chords, shuffled at random

    int numChords = AltoSet.numNotes * TenorSet.numNotes;
    ChordSet allChords;
    allChords.resize(numChords);
    int iC = 0;
    for (int iA=0; iA < AltoSet.numNotes; iA++)
    {
        for (int iT=0; iT < TenorSet.numNotes; iT++)
        {
            // bass
            allChords[iC].nNote[0] = B1.to_diatonic_pitch();
            allChords[iC].nAcc[0] = B1.num_accidentals();
            // soprano
            allChords[iC].nNote[3] = S1.to_diatonic_pitch();
            allChords[iC].nAcc[3] = S1.num_accidentals();
            // alto
            allChords[iC].nNote[2] = AltoSet.pitch[iA].to_diatonic_pitch();
            allChords[iC].nAcc[2] = AltoSet.pitch[iA].num_accidentals();
            // tenor
            allChords[iC].nNote[1] = TenorSet.pitch[iT].to_diatonic_pitch();
            allChords[iC].nAcc[1] = TenorSet.pitch[iT].num_accidentals();
            //
            allChords[iC].nReason = k_chord_error_0_none;
            allChords[iC].nSeverity = k_chord_error_0_none;
            allChords[iC].nNumNotes = 4;
            iC++;
        }
    }

#if (TRACE_CADENCE == 2)
    TraceCadence("Num Chords = %d", numChords);
    for (int i=0; i < numChords; i++)
    {
        TraceCadence("Chord %d : %s, %s, %s, %s",
                     i,
                     allChords[i].get_print_name(0).c_str(),
                     allChords[i].get_print_name(1).c_str(),
                     allChords[i].get_print_name(2).c_str(),
                     allChords[i].get_print_name(3).c_str() );
    }
#endif

    pValidChords->clear();
    int nValidChords = filter_chords(allChords, pValidChords, pBasicChord, numChords,
                                     pPrevChord);
    #if (TRACE_CADENCE > 0)
        TraceCadence("%d chords generated. Valid Chords = %d", numChords, nValidChords);
        for (int i=0; i < numChords; i++)
            dump_chord(allChords[i], i);
    #endif

    return nValidChords;
}

//---------------------------------------------------------------------------------------
int Cadence::filter_chords(ChordSet& allChords, ChordSet* pValidChords,
                            Chord* pBasicChord, int numChords,
                            CadenceChord* pPrevChord, bool fExhaustive)
{
    // Parameters:
    //  - if fExhaustive == true, check each chord against all validation rules. This
    //                            option is useful to know all the rules that a chord
    //                            do not satisfy.
    //                   == false, discard a chord as soon as a rule fails. This option
    //                             saves time when it is not required to know which
    //                             rules are not satisfied.


    //prepare information
    int nPrevAlter[4] = {0,0,0,0};      //chromatic alterations (accidentals not in key signature)
    get_chromatic_alterations(pPrevChord, m_nKey, &nPrevAlter[0]);
    int nStepLeading = KeyUtilities::get_step_for_leading_note(m_nKey);
    int iLeading = find_leading_tone_in_previous_chord(pPrevChord, nStepLeading);

    //check the chords
    pValidChords->clear();
    int numValid = 0;
    for (int i=0; i < numChords; i++)
    {
        check_chord(&allChords[i], pBasicChord, pPrevChord, m_nKey, &nPrevAlter[0],
                    nStepLeading, iLeading, fExhaustive);

        if (allChords[i].nReason == k_chord_error_0_none)
        {
            ++numValid;
            pValidChords->push_back(allChords[i]);
        }
    }

    return numValid;
}

//---------------------------------------------------------------------------------------
int Cadence::find_leading_tone_in_previous_chord(CadenceChord* pPrevChord,
                                                 int nStepLeading)
{
    //return index to first chord (from bass to soprano) that is the leading note in
    //the chord or -1 if none.

    //In a well formed chord the leading tone is never doubled. Therefore, it is not
    //necessary to look for more than one instance. If there were more, the chord will
    //be rejected in validation rules.

    if (pPrevChord)
    {
        //Check if leading tone is present in previous chord
        for (int iN=0; iN < 4; iN++)
        {
            if (pPrevChord->nNote[iN].step() == nStepLeading)
            {
                return iN;
                break;
            }
        }
    }
    return -1;
}

//---------------------------------------------------------------------------------------
void Cadence::get_chromatic_alterations(CadenceChord* pChord, EKeySignature nKey,
                                        int* pAlter)
{
    //locate chromatic alterations (accidentals not in key signature).

    if (pChord)
    {
        int nKeyAccidentals[7];
        KeyUtilities::get_accidentals_for_key(nKey, nKeyAccidentals);
        for (int iN=0; iN < 4; ++iN, ++pAlter)
        {
            int step = pChord->nNote[iN].step();
            *pAlter = pChord->nAcc[iN] - nKeyAccidentals[step];
            #if (TRACE_CADENCE == 2)
                TraceCadence("Chromatic alteration for note %d = %d", iN, *pAlter);
            #endif
        }
    }
}

//---------------------------------------------------------------------------------------
bool Cadence::check_chord(CadenceChord* pChord, Chord* pBasicChord,
                          CadenceChord* pPrevChord, EKeySignature WXUNUSED(nKey),
                          int nPrevAlter[4], int nStepLeading, int iLeading,
                          bool fExhaustive)
{
    // Parameters:
    // - pChord - ptr to the CadenceChord to verify
    // - pBasicChord - ptr to basic chord that corresponds to pChord
    // - pPrevChord - ptr to preceding CadenceChord or nullptr if none
    // - nKey - key signature
    // - nPrevAlter[4] - chromatic alterations in pChord
    // - nStepLeading - the step (0..6) for the leading note in current key signature
    // - iLeading - index (0..3) to the first leading note in pChord, or -1 if not present.
    // - if fExhaustive == true, check the chord against all validation rules. This
    //                           option is useful to know all the rules that a chord
    //                           do not satisfy.
    //                  == false, terminate checking as soon as a rule fails. This
    //                            option saves time when it is not required to know
    //                            which rules are not satisfied.
    //
    // Returns true if the chord satisfies all validation rules


    //determine motion between chords
    FIntval motionIntv[4];
    int motionSteps[4];
    if (pPrevChord)
    {
        for (int iN=0; iN < 4; iN++)
        {
            motionIntv[iN] = FPitch(pChord->nNote[iN], pChord->nAcc[iN])
                         - FPitch(pPrevChord->nNote[iN], pPrevChord->nAcc[iN]);

            motionSteps[iN] = pChord->nNote[iN] - pPrevChord->nNote[iN];
        }
    }

    //find fifths, octaves and unisons in prev chord
    list< pair<int,int> > couples;
    if (pPrevChord)
    {
        for (int i=0; i < 4; i++)
        {
            for (int j=i+1; j < 4; j++)
            {
                FIntval intval = FPitch(pPrevChord->nNote[j], pPrevChord->nAcc[j])
                               - FPitch(pPrevChord->nNote[i], pPrevChord->nAcc[i]);
                if (intval == k_interval_p1)
                    couples.push_back( make_pair(i, j) );   //unison
                else if (intval % k_interval_p5 == 0)
                    couples.push_back( make_pair(i, j) );   //fifth
                else if (intval % k_interval_p8 == 0)
                    couples.push_back( make_pair(i, j) );   //octave
            }
        }
    }


    //Apply validation rules

    //Rule 1. The chord is complete. All steps are in the chord
    if (fExhaustive || pChord->nReason == k_chord_error_0_none)
        rule_1_all_steps_in_chord(pChord, pBasicChord);

    //Rule 2. k_chord_error_2_fifth_missing - Acorde completo. Contiene todas las
    //        notas (en todo caso, elidir la 5ª)
        //TODO

    //Rule 3. No parallel motion of perfect octaves, perfect fifths, and unisons
    if (pPrevChord && (fExhaustive || pChord->nReason == k_chord_error_0_none))
        rule_3_no_parallel_motion_fifths_octaves(pChord, couples, motionIntv);

    //4. No resultant fifths or octaves, except when soprano moved by second or (for
    //   fifths) when one of the pitches was in previous chord.
//    if (pPrevChord && (fExhaustive || pChord->nReason == k_chord_error_0_none))
//        rule_4_resultant_fifth_octave(pChord, pPrevChord, motionSteps);

    //Rule 5. The fifth is not doubled
    //Rule 6. The leading tone is never doubled
    if (fExhaustive || pChord->nReason == k_chord_error_0_none)
        rule_5_6_fifth_leading_not_doubled(pChord, pBasicChord, nStepLeading);

    //7. Scale degree seven (the leading tone) should resolve to tonic.
    if (pPrevChord && iLeading >=0 && (fExhaustive || pChord->nReason == k_chord_error_0_none))
        rule_7_leading_resolution(pChord, motionSteps[iLeading], iLeading);

    //8. The seventh of a chord should always resolve down by second.
//    if (pPrevChord && iLeading >=0 && (fExhaustive || pChord->nReason == k_chord_error_0_none))
//        rule_8_seventh_resolution(pChord, motionSteps[iLeading], iLeading);

    //9. voices interval not greater than one octave (except bass-tenor)
    if (fExhaustive || pChord->nReason == k_chord_error_0_none)
        rule_9_intervals_not_greater_than_octave(pChord);

    //10. notes in ascending sequence (do not to allow voices crossing). No duplicates
    if (fExhaustive || pChord->nReason == k_chord_error_0_none)
        rule_10_no_voice_crossing(pChord);

    //11. No voice overlap. No voice moves above or below a pitch previously sounded
    //    by another voice.
    if (pPrevChord && (fExhaustive || pChord->nReason == k_chord_error_0_none))
        rule_11_no_voice_overlap(pChord, pPrevChord);

    //12. Resolve chromatic accidentals [by step] in the same direction than the
    //    key signature accidental.
    if (pPrevChord && (fExhaustive || pChord->nReason == k_chord_error_0_none))
        rule_12_chromatic_accidentals(pChord, nPrevAlter, pPrevChord);

    //13. k_chord_error_13_greater_than_sixth. No es conveniente exceder el intervalo de
    //    sexta, exceptuando la octava justa
        //TODO

    //14. If bass moves by step all other voices must move in opposite direction to bass
    if (pPrevChord && (fExhaustive || pChord->nReason == k_chord_error_0_none))
        rule_14_not_contrary_motion(pChord, motionSteps);

    //15. k_chord_error_15_doubled_third. Cuando el bajo enlaza el V grado con el VI
    //    (cadencia rota), en el acorde de VI grado se duplica la tercera.
        //TODO



    return (pChord->nReason == k_chord_error_0_none);
}

//---------------------------------------------------------------------------------------
void Cadence::rule_1_all_steps_in_chord(CadenceChord* pChord, Chord* pBasicChord)
{
    //1. The chord is complete. All steps are in the chord

    bool fFound[4] = { false, false, false, false };
    for (int iN=0; iN < 4; iN++)
    {
        int nStep = pChord->nNote[iN].step();
        for(int j=0; j < pBasicChord->get_num_notes(); j++)
        {
            if (nStep == pBasicChord->get_step(j))
            {
                fFound[j] = true;
                break;
            }
        }
    }
    bool fValid = true;
    for(int j=0; j < pBasicChord->get_num_notes(); j++)
    {
        fValid &= fFound[j];
    }

    if (!fValid)
        pChord->add_error(k_chord_error_1_not_all_notes, 0);
}

//---------------------------------------------------------------------------------------
void Cadence::rule_3_no_parallel_motion_fifths_octaves(CadenceChord* pChord,
                                                       list< pair<int,int> >& couples,
                                                       FIntval motionIntv[4])
{
    //3. No parallel motion of perfect octaves, perfect fifths, and unisons

    if (couples.size() > 0)
    {
        list< pair<int,int> >::iterator it;
        for (it=couples.begin(); it != couples.end(); ++it)
        {
            if (motionIntv[it->first] == motionIntv[it->second])
            {
                pChord->add_error(k_chord_error_3_fifth_octave_motion, 0);
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void Cadence::rule_4_resultant_fifth_octave(CadenceChord* pChord,
                                            CadenceChord* pPrevChord,
                                            int motionSteps[4])
{
    //4. No resultant fifths or octaves, except when soprano moved by second or (for
    //   fifths) when one of the pitches was in previous chord.

    //find fifths and octaves in current chord
    for (int i=0; i < 4; i++)
    {
        FPitch fp1(pChord->nNote[i], pChord->nAcc[i]);

        for (int j=i+1; j < 4; j++)
        {
            FPitch fp2(pChord->nNote[j], pChord->nAcc[j]);
            FIntval intval = (fp2 > fp1 ? fp2-fp1 : fp1-fp2);

            if (intval % k_interval_p5 == 0 || intval % k_interval_p8 == 0)
            {
                if (motionSteps[4] == 2)
                {
                    //valid: the soprano moved by second
                    continue;
                }
                else if (intval % k_interval_p5 == 0 &&
                         (fp2 == FPitch(pPrevChord->nNote[j], pPrevChord->nAcc[j]) ||
                          fp1 == FPitch(pPrevChord->nNote[i], pPrevChord->nAcc[i])) )
                {
                    //fifth valid: one of the notes was in prev chord
                    continue;
                }
                else
                {
                    pChord->add_error(k_chord_error_4_resultant_fifth_octave,
                                      (j==0 || j==3 ? 1 : 0) );
                    return;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void Cadence::rule_5_6_fifth_leading_not_doubled(CadenceChord* pChord,
                                                 Chord* pBasicChord,
                                                 int nStepLeading)
{
    //Rule 5. The fifth is not doubled
    //Rule 6. The leading tone is never doubled

    int nFifths = 0;
    int nLeadingTones = 0;
    for (int iN=0; iN < 4; iN++)
    {
        int nStep = pChord->nNote[iN].step();
        if (nStep == pBasicChord->get_step(2))
            nFifths++;
        else if (nStep == nStepLeading)
            nLeadingTones++;
    }
    if (nFifths > 1)
        pChord->add_error(k_chord_error_5_fifth_doubled, 0);
    if (nLeadingTones > 1)
        pChord->add_error(k_chord_error_6_leading_doubled, 0);
}

//---------------------------------------------------------------------------------------
void Cadence::rule_7_leading_resolution(CadenceChord* pChord, int leadingMotion,
                                        int iLeading)
{
    //7. Scale degree seven (the leading tone) should resolve to tonic.

    // leading tone should move up by step
    if (leadingMotion != 1)
        pChord->add_error(k_chord_error_7_leading_resolution,
                          (iLeading==0 || iLeading==3 ? 1 : 0));
}

//---------------------------------------------------------------------------------------
void Cadence::rule_8_seventh_resolution(CadenceChord* pChord, int leadingMotion,
                                        int iLeading)
{
    //8. The seventh of a chord should always resolve down by second.

    // The seventh should move down by step
    if (leadingMotion != -1)
        pChord->add_error(k_chord_error_8_seventh_resolution,
                          (iLeading==0 || iLeading==3 ? 1 : 0));
}

//---------------------------------------------------------------------------------------
void Cadence::rule_9_intervals_not_greater_than_octave(CadenceChord* pChord)
{
    //9. notes interval not greater than one octave (except bass-tenor)

    if ((int)pChord->nNote[2] - (int)pChord->nNote[1] > 7 ||
        (int)pChord->nNote[3] - (int)pChord->nNote[2] > 7 )
    {
        pChord->add_error(k_chord_error_9_greater_than_octave,
                          ((int)pChord->nNote[3] - (int)pChord->nNote[2] > 7 ? 1 : 0) );
    }
}

//---------------------------------------------------------------------------------------
void Cadence::rule_10_no_voice_crossing(CadenceChord* pChord)
{
    //10. notes in ascending sequence (do not allow voices crossing)

    if (pChord->nNote[1] < pChord->nNote[0]
        || pChord->nNote[2] < pChord->nNote[1]
        || pChord->nNote[3] < pChord->nNote[2] )
    {
        pChord->nReason = k_chord_error_10_notes_not_ascending;
        pChord->nSeverity |= k_chord_error_10_notes_not_ascending;
        if (pChord->nNote[1] <= pChord->nNote[0] ||
                pChord->nNote[3] <= pChord->nNote[2] )
        {
            pChord->nImpact = 1;
        }
        else
            pChord->nImpact = 0;
    }
}

//---------------------------------------------------------------------------------------
void Cadence::rule_11_no_voice_overlap(CadenceChord* pChord, CadenceChord* pPrevChord)
{
    //11. No voice overlap. No voice moves above or below a pitch previously sounded
    //    by another voice.

    bool fUpMotion;
    for (int iN=0; iN < 4; iN++)
    {
        if (pChord->nNote[iN] != pPrevChord->nNote[iN])
        {
            fUpMotion = (pChord->nNote[iN] > pPrevChord->nNote[iN]);
            if (iN < 3 && fUpMotion && pChord->nNote[iN] > pPrevChord->nNote[iN+1])
            {
                pChord->add_error(k_chord_error_11_voice_overlap,
                                  (iN==0 || iN+1==3 ? 1 : 0) );
                break;
            }
            else if (iN > 0 && !fUpMotion && pChord->nNote[iN] < pPrevChord->nNote[iN-1])
            {
                pChord->add_error(k_chord_error_11_voice_overlap,
                                  (iN==3 || iN-1==0 ? 1 : 0) );
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void Cadence::rule_12_chromatic_accidentals(CadenceChord* pChord, int nPrevAlter[4],
                                            CadenceChord* pPrevChord)
{
    //12. Resolve chromatic accidentals [by step] in the same direction than
    //    the accidentals in the key signature

    for (int iN=0; iN < 4; iN++)
    {
        if (pChord->nNote[iN] != pPrevChord->nNote[iN] && nPrevAlter[iN] != 0)
        {
            if (((pChord->nNote[iN] > pPrevChord->nNote[iN]) && nPrevAlter[iN] < 0) ||
                    ((pChord->nNote[iN] < pPrevChord->nNote[iN]) && nPrevAlter[iN] > 0) )
            {
                pChord->add_error(k_chord_error_12_chromatic_acc,
                                  (iN==0 || iN==3 ? 1 : 0) );
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void Cadence::rule_14_not_contrary_motion(CadenceChord* pChord, int motionSteps[4])
{
    //14. If bass moves by step all other voices must move in opposite direction to bass

    if (motionSteps[0] == 1)
    {
        // bass motion: up
        for (int iN=1; iN < 4; iN++)
        {
            if (motionSteps[iN] > 0)
            {
                pChord->add_error(k_chord_error_14_not_contrary_motion,
                                  (iN==0 || iN==3 ? 1 : 0) );
                break;
            }
        }
    }
    else if (motionSteps[0] == -1)
    {
        // bass motion: down
        for (int iN=1; iN < 4; iN++)
        {
            if (motionSteps[iN] < 0)
            {
                pChord->add_error(k_chord_error_14_not_contrary_motion,
                                  (iN==0 || iN==3 ? 1 : 0) );
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
#if (TRACE_CADENCE > 0)
void Cadence::dump_chord(CadenceChord& oChord, int iChord, bool fAllErrors)
{
    if (oChord.nReason == k_chord_error_0_none)
    {
        TraceCadence("Chord %d: Valid ==========> %s, %s, %s, %s",
                     iChord,
                     oChord.get_print_name(0).c_str(),
                     oChord.get_print_name(1).c_str(),
                     oChord.get_print_name(2).c_str(),
                     oChord.get_print_name(3).c_str()
                    );
    }
    else
    {
        ChordError nError = oChord.nSeverity;
        if (fAllErrors)
        {
            TraceCadence("Chord %d: Invalid: %s, %s, %s, %s - Impact %d, severity %ld. Errors:\n%s",
                         iChord,
                         oChord.get_print_name(0).c_str(),
                         oChord.get_print_name(1).c_str(),
                         oChord.get_print_name(2).c_str(),
                         oChord.get_print_name(3).c_str(),
                         oChord.nImpact, nError,
                         get_all_errors_reason(nError).c_str() );
        }
        else
        {
            TraceCadence("Chord %d: Invalid: %s, %s, %s, %s - %s (severity %ld)",
                         iChord,
                         oChord.get_print_name(0).c_str(),
                         oChord.get_print_name(1).c_str(),
                         oChord.get_print_name(2).c_str(),
                         oChord.get_print_name(3).c_str(),
                         get_error_description(nError).c_str(),
                         nError );
        }
    }
}
#else
    void Cadence::dump_chord(CadenceChord& oChord, int iChord) {}
#endif

//---------------------------------------------------------------------------------------
string Cadence::get_all_errors_reason(ChordError nError)
{
    stringstream ss;
    if (nError & k_chord_error_15_doubled_third)
        ss << get_error_description(k_chord_error_15_doubled_third) << endl;
    if (nError & k_chord_error_14_not_contrary_motion)
        ss << get_error_description(k_chord_error_14_not_contrary_motion) << endl;
    if (nError & k_chord_error_13_greater_than_sixth)
        ss << get_error_description(k_chord_error_13_greater_than_sixth) << endl;
    if (nError & k_chord_error_12_chromatic_acc)
        ss << get_error_description(k_chord_error_12_chromatic_acc) << endl;
    if (nError & k_chord_error_11_voice_overlap)
        ss << get_error_description(k_chord_error_11_voice_overlap) << endl;
    if (nError & k_chord_error_10_notes_not_ascending)
        ss << get_error_description(k_chord_error_10_notes_not_ascending) << endl;
    if (nError & k_chord_error_9_greater_than_octave)
        ss << get_error_description(k_chord_error_9_greater_than_octave) << endl;
    if (nError & k_chord_error_8_seventh_resolution)
        ss << get_error_description(k_chord_error_8_seventh_resolution) << endl;
    if (nError & k_chord_error_7_leading_resolution)
        ss << get_error_description(k_chord_error_7_leading_resolution) << endl;
    if (nError & k_chord_error_6_leading_doubled)
        ss << get_error_description(k_chord_error_6_leading_doubled) << endl;
    if (nError & k_chord_error_5_fifth_doubled)
        ss << get_error_description(k_chord_error_5_fifth_doubled) << endl;
    if (nError & k_chord_error_4_resultant_fifth_octave)
        ss << get_error_description(k_chord_error_4_resultant_fifth_octave) << endl;
    if (nError & k_chord_error_3_fifth_octave_motion)
        ss << get_error_description(k_chord_error_3_fifth_octave_motion) << endl;
    if (nError & k_chord_error_2_fifth_missing)
        ss << get_error_description(k_chord_error_2_fifth_missing) << endl;
    if (nError & k_chord_error_1_not_all_notes)
        ss << get_error_description(k_chord_error_1_not_all_notes) << endl;

    return ss.str();
}

//---------------------------------------------------------------------------------------
wxString Cadence::get_name()
{
    //get the functions
    int iC;
    wxString sName = type_to_name(m_nType) + " : ";
    for (iC=0; iC < k_chords_in_cadence; iC++)
    {
        wxString sFunct;
        // get the function
        if (m_nType == k_cadence_imperfect_V_I)
            sFunct = aImperfect[m_nImperfectCad][iC];
        else
            sFunct = aFunction[m_nType][iC];
        if (sFunct == "")
            break;
        if (iC != 0)
            sName += " -> ";
        sName += sFunct;
    }
    return sName;

}

//---------------------------------------------------------------------------------------
string Cadence::get_rel_ldp_name(int iChord, int iNote)
{
    int step = m_Chords[iChord].nNote[iNote].step();
    int octave = m_Chords[iChord].nNote[iNote].octave();
    int acc = m_Chords[iChord].nAcc[iNote];

    FPitch fp(step, octave, acc);
    return fp.to_rel_ldp_name(m_nKey);
}

//---------------------------------------------------------------------------------------
Chord* Cadence::get_tonic_chord()
{
    if (!m_pTonicChord)
    {
        //Create tonic chord

        //Get root note for this key signature and clef
        FPitch fpRootNote = get_root_note("I", m_nKey);
        wxString sIntervals = "";
        if (KeyUtilities::is_major_key(m_nKey))
            sIntervals = "M3,p5";
        else
            sIntervals = "m3,p5";

        //create the chord
        m_pTonicChord = LENMUS_NEW Chord(fpRootNote, sIntervals, m_nKey);
    }
    return m_pTonicChord;
}

//---------------------------------------------------------------------------------------
ECadenceType Cadence::name_to_type(wxString sCadence)
{
    //AWARE: bijective to ECadenceType
    static const wxString sNames[] =
    {
        // Perfect authentic:
        "V_I_Perfect", "V7_I", "Va5_I", "Vd5_I",
        // Plagal:
        "IV_I", "IVm_I", "IIc6_I", "IImc6_I",
        // Imperfect authentic:
        "V_I_Imperfect",
        // Deceptive:
        "V_IV", "V_IVm", "V_VI", "V_VIm", "V_IIm",
        "V_III", "V_VII",
        // Half cadences:
        "IImc6_V", "IV_V", "I_V", "Ic64_V", "IV6_V",
        "II_V", "IIdimc6_V", "VdeVdim5c64_V",
    };

    for (int i=0; i < k_cadence_max; i++)
    {
        if (sCadence == sNames[i])
            return (ECadenceType)i;
    }

    return (ECadenceType)-1;
}

//---------------------------------------------------------------------------------------
string Cadence::get_error_description(ChordError nError)
{
    static string m_error[k_chord_error_max];
    static wxString m_language = "??";

    //language dependent strings. Can not be statically initialized because
    //then they do not get translated
    if (m_language != ApplicationScope::get_language())
    {
        m_error[k_chord_error_0_none] =
            _("Chord is valid");
        m_error[k_chord_error_1_not_all_notes] =
            _("Not all chord steps in the chord");
        m_error[k_chord_error_2_fifth_missing] =
            _("Acorde completo. Contiene todas las notas (en todo caso, elidir la 5ª)");
        m_error[k_chord_error_3_fifth_octave_motion] =
            _("Parallel motion of perfect octaves, perfect fifths or unisons");
        m_error[k_chord_error_4_resultant_fifth_octave] =
            _("Resultant fifths or octaves in no valid cases");
        m_error[k_chord_error_5_fifth_doubled] =
            _("The fifth is doubled");
        m_error[k_chord_error_6_leading_doubled] =
            _("The leading tone is doubled");
        m_error[k_chord_error_7_leading_resolution] =
            _("Scale degree seven (the leading tone) doesn't resolve to tonic");
        m_error[k_chord_error_8_seventh_resolution] =
            _("The seventh does not resolve down by second");
        m_error[k_chord_error_9_greater_than_octave] =
            _("Notes interval greater than one octave (other than bass-tenor)");
        m_error[k_chord_error_10_notes_not_ascending] =
            _("Notes not in ascending sequence or duplicated");
        m_error[k_chord_error_11_voice_overlap] =
            _("Voice overlap");
        m_error[k_chord_error_12_chromatic_acc] =
            _("Chromatic accidental not resolved in the same direction than "
              "key accidental");
        m_error[k_chord_error_13_greater_than_sixth] =
            _("No es conveniente exceder el intervalo de sexta, exceptuando la "
              "octava justa");
        m_error[k_chord_error_14_not_contrary_motion] =
            _("Bass moves by step and not all other voices moves in the opposite "
              "direction");
        m_error[k_chord_error_15_doubled_third] =
            _("Cuando el bajo enlaza el V grado con el VI (cadencia rota), en "
              "el acorde de VI grado se duplica la tercera");

        m_language = ApplicationScope::get_language();
    }

    if (nError < k_chord_error_0_none || nError >= k_chord_error_max)
    {
        LOMSE_LOG_ERROR("Invalid chord error %ld", nError);
        return "Program error: Invalid error value";
    }
    return m_error[nError];
}

//----------------------------------------------------------------------------------------
wxString Cadence::type_to_name(ECadenceType nType)
{
    static wxString m_language = "??";

    wxASSERT(nType <= k_cadence_max);

    //language dependent strings. Can not be statically initialized because
    //then they do not get translated
    if (m_language != ApplicationScope::get_language())
    {
        // Terminal cadences
        // Perfect authentic cadences
        m_sCadenceName[k_cadence_perfect_V_I] = _("Perfect");
        m_sCadenceName[k_cadence_perfect_V7_I] = _("Perfect");
        m_sCadenceName[k_cadence_perfect_Va5_I] = _("Perfect");
        m_sCadenceName[k_cadence_perfect_Vd5_I] = _("Perfect");
        // Plagal cadences
        m_sCadenceName[k_cadence_plagal_IV_I] = _("Plagal");
        m_sCadenceName[k_cadence_plagal_IVm_I] = _("Plagal");
        m_sCadenceName[k_cadence_plagal_IIc6_I] = _("Plagal");
        m_sCadenceName[k_cadence_plagal_IImc6_I] = _("Plagal");

        // Transient cadences
        // Imperfect authentic cadences
        m_sCadenceName[k_cadence_imperfect_V_I] = _("Imperfect authentic");
        // Deceptive cadences
        m_sCadenceName[k_cadence_deceptive_V_IV] = _("Deceptive");
        m_sCadenceName[k_cadence_deceptive_V_IVm] = _("Deceptive");
        m_sCadenceName[k_cadence_deceptive_V_VI] = _("Deceptive");
        m_sCadenceName[k_cadence_deceptive_V_VIm] = _("Deceptive");
        m_sCadenceName[k_cadence_deceptive_V_IIm] = _("Deceptive");
        m_sCadenceName[k_cadence_deceptive_V_III] = _("Deceptive");
        m_sCadenceName[k_cadence_deceptive_V_VII] = _("Deceptive");
        // Half cadences
        m_sCadenceName[k_cadence_half_IImc6_V] = _("Half cadence");
        m_sCadenceName[k_cadence_half_IV_V] = _("Half cadence");
        m_sCadenceName[k_cadence_half_I_V] = _("Half cadence");
        m_sCadenceName[k_cadence_half_Ic64_V] = _("Half cadence");
        m_sCadenceName[k_cadence_half_IV6_V] = _("Half cadence");
        m_sCadenceName[k_cadence_half_II_V] = _("Half cadence");
        m_sCadenceName[k_cadence_half_IIdimc6_V] = _("Half cadence");
        m_sCadenceName[k_cadence_half_VdeVdim5c64_V] = _("Half cadence");
        m_sCadenceName[k_cadence_last_half] = _("Half cadence");

        m_language = ApplicationScope::get_language();
    }

    return m_sCadenceName[nType];
}


}   //namespace lenmus
