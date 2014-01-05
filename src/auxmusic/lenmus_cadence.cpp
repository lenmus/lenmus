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

//lenmus
#include "lenmus_cadence.h"
#include "lenmus_standard_header.h"

#include "lenmus_chord.h"
#include "lenmus_generators.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_score_utilities.h>
using namespace lomse;

//set to 1 for tracing cadence generation
#define TRACE_CADENCE  0
#if (LENMUS_COMPILER_MSVC == 1)     //VC++ 2003 does not support variadic macros
    #if (TRACE_CADENCE == 1)
        #define TraceCadence    wxLogMessage
    #else
        #define TraceCadence    __noop
    #endif
#else
    #if (TRACE_CADENCE == 1)
        #define TraceCadence(format, args ...)      wxLogMessage(format, args)
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
// is notated without the slash (IVm6,
// AWARE: The maximum number of notes in a chord is defined in 'Chord.h', constant
//        k_notes_in_chord. Currently its value is 6. Change this value if you need more
//        notes.

static const FunctionData m_aFunctionData[] = {
    //                                    minor key      minor key
    //Function        Major key           option 1       option 2
    //-----------    ------------------  -------------   --------------
    {_T("I"),       _T("M3,p5"),        _T("m3,p5"),    _T("nil") },
    {_T("II"),      _T("m3,p5"),        _T("m3,d5"),    _T("m3,p5") },
    {_T("IIm"),     _T("m3,p5"),        _T("nil"),      _T("nil") },
    {_T("III"),     _T("m3,p5"),        _T("M3,p5"),    _T("M3,a5") },
    {_T("IV"),      _T("M3,p5"),        _T("m3,p5"),    _T("M3,p5") },
    {_T("IVm"),     _T("m3,p5"),        _T("nil"),      _T("nil") },
    {_T("IVm6"),    _T("nil"),          _T("M3,M6"),    _T("nil") },    // IVm/6 used in Phrygian half cadence
    {_T("V"),       _T("M3,p5"),        _T("M3,p5"),    _T("m3,p5") },
    {_T("V7"),      _T("M3,p5,m7"),     _T("M3,p5,m7"), _T("nil") },
    {_T("Va5"),     _T("M3,a5"),        _T("nil"),      _T("nil") },
    {_T("Vd5"),     _T("M3,d5"),        _T("M3,d5"),    _T("m3,d5") },
    {_T("VI"),      _T("m3,p5"),        _T("M3,p5"),    _T("#,m3,d5") },
    {_T("VII"),     _T("m3,d5"),        _T("#,m3,d5"),  _T("M3,p5") },
    {_T("IIb6"),    _T("m3,m6"),        _T("m3,m6"),    _T("nil") },    // IIb/6 used in neapolitan sixth
	{_T("IId"),     _T("m3,d5"),		_T("nil"),      _T("nil") },	// IId used in augmented sixth
};


//---------------------------------------------------------------------------------------
static wxString m_sCadenceName[k_cadence_max+1];
static bool m_fStringsInitialized = false;


//---------------------------------------------------------------------------------------
// AWARE: Array indexes are in correspondence with enum ECadenceType
// AWARE: Change constant k_chords_in_cadence to increment number of chords
static const wxString aFunction[k_cadence_max][k_chords_in_cadence] = {
    // Perfect authentic cadences
    { _T("V"),      _T("I") },      //Perfect_V_I
    { _T("V7"),     _T("I") },      //Perfect_V7_I
    { _T("Va5"),    _T("I") },      //Perfect_Va5_I
    { _T("Vd5"),    _T("I") },      //Perfect_Vd5_I
    // Plagal cadences
    { _T("IV"),     _T("I") },      //Plagal_IV_I
    { _T("IVm"),    _T("I") },      //Plagal_IVm_I
    { _T("II/6"),   _T("I") },      //Plagal_IIc6_I
    { _T("IIm/6"),  _T("I") },      //Plagal_IImc6_I
    // Imperfect authentic cadences
    // AWARE: This entry is not used. Inversions are encoded in aImperfect[]
	{ _T("V"),      _T("I") },      //Imperfect_V_I
    // Deceptive cadences
    { _T("V"),      _T("IV") },     //Deceptive_V_IV
    { _T("V"),      _T("IVm") },    //Deceptive_V_IVm
    { _T("V"),      _T("VI") },     //Deceptive_V_VI
    { _T("V"),      _T("VIm") },    //Deceptive_V_VIm
    { _T("V"),      _T("IIm") },    //Deceptive_V_IIm
    { _T("V"),      _T("III") },    //Deceptive_V_III
    { _T("V"),      _T("VII") },    //Deceptive_V_VII
    // Half cadences
    { _T("IIm/6"),  _T("V") },      //Half_IImc6_V
    { _T("IV"),     _T("V") },      //Half_IV_V
    { _T("I"),      _T("V") },      //Half_I_V
    { _T("I/64"),   _T("V") },      //Half_Ic64_V
    { _T("IVm6"),   _T("V") },      //Half_IV6_V (Phrygian)
    { _T("II"),     _T("V") },      //Half_II_V
    { _T("IId/6"),  _T("V") },      //Half_IIdimc6_V (Neapolitan sixth)
    { _T("IIb6"),   _T("V") },      //Half_VdeVdim5c64_V (augmented sixth)
};

//---------------------------------------------------------------------------------------
// Chords for imperfect cadence
static const wxString aImperfect[4][k_chords_in_cadence] = {
    { _T("V/6"),    _T("I") },
    { _T("V/64"),   _T("I") },
    { _T("V/64"),   _T("I/6") },
    { _T("V"),      _T("I/64") },
};

#define lmEXHAUSTIVE  true      //for FilterChords() method


//---------------------------------------------------------------------------------------
// Implementation of Cadence class
//---------------------------------------------------------------------------------------
Cadence::Cadence()
    : m_fCreated(false)
    , m_pTonicChord(NULL)
    , m_nNumChords(0)
{
}

//---------------------------------------------------------------------------------------
bool Cadence::Create(ECadenceType nCadenceType, EKeySignature nKey, bool fUseGrandStaff)
{
    // return true if cadence created
    // if fUseGrandStaff is true chords will have 4 notes, with root note in 2/3 octaves

    //save parameters
    m_nType = nCadenceType;
    m_nKey = nKey;

    //get chords
    wxString sFunct;
	m_nImperfectCad = RandomGenerator::random_number(0, 3);	//to select an imperfect authentic cadence
    int iC;
    for (iC=0; iC < k_chords_in_cadence; iC++)
    {
        // get the function
        if (nCadenceType == k_cadence_imperfect_V_I)
            sFunct = aImperfect[m_nImperfectCad][iC];
        else
            sFunct = aFunction[nCadenceType][iC];

        // if no function, exit loop. No more chords in cadence
        if (sFunct == _T(""))
        {
            if (iC > 1) break;      //no error
            //table maintenace error
                //debug
            TraceCadence(_T("[Cadence::Create] No harmonic function!. nCadenceType=%d, iC=%d"),
                         nCadenceType, iC);
            return false;
        }

        //get the chord intervals
        wxString sIntervals = SelectChord(sFunct, nKey, &m_nInversions[iC]);
        //debug
		TraceCadence(_T("[Cadence::Create] sFunct='%s', chord intervals='%s'"),
                sFunct.c_str(), sIntervals.c_str() );
        if (sIntervals == _T("")) {
            //error: no chord for choosen function
            //debug
            TraceCadence(_T("[Cadence::Create] No chord found for nCadenceType=%d, nKey=%d, iC=%d"),
                    nCadenceType, nKey, iC);
            return false;
        }

        //Get root note for this key signature and clef
        FPitch fpRootNote = GetRootNote(sFunct, nKey, k_clef_G2, fUseGrandStaff);
            //debug
        TraceCadence(_T("[Cadence"),
                _T("[Cadence::Create] sFunc='%s', nKey=%d, sRootNote='%s'"),
				sFunct.c_str(), nKey, to_wx_string(fpRootNote.to_abs_ldp_name()).c_str());

       //Prepare the chord
        m_pChords[iC] = LENMUS_NEW Chord(fpRootNote, sIntervals, nKey);
        m_nNumChords++;
    }

    // generate first chord
    std::vector<lmHChord> aFirstChord;
	lmChordAuxData tFirstChordData;
    int nValidFirst = GenerateFirstChord(aFirstChord, tFirstChordData, m_pChords[0], m_nInversions[0]);
    if (nValidFirst == 0) {
        // Select the less bad chord
        SelectLessBad(aFirstChord, tFirstChordData, 0);
    }
    else {
        // choose at random one of the valid chords
        int iSel = RandomGenerator::random_number(1, nValidFirst);
        int nValid = 0;
        int iC;
        for (iC=0; iC < (int)aFirstChord.size(); iC++) {
            if (aFirstChord[iC].nReason == lm_eChordValid) {
                if (iSel == ++nValid) break;
            }
        }
        // chord iC is the selected one.
        m_Chord[0] = aFirstChord[iC];
            //debug
        TraceCadence(_T("[Cadence::Create] Selected : %s, %s, %s, %s"),
                    aFirstChord[iC].GetPrintName(0).c_str(),
                    aFirstChord[iC].GetPrintName(1).c_str(),
                    aFirstChord[iC].GetPrintName(2).c_str(),
                    aFirstChord[iC].GetPrintName(3).c_str() );
    }

    // Select the second chord
    std::vector<lmHChord> aNextChord;
	lmChordAuxData tNextChordData;
    int nValidNext = GenerateNextChord(aNextChord, tNextChordData, m_pChords[1], m_nInversions[1], 0);
    if (nValidNext == 0) {
        //Select the less bad chord
        SelectLessBad(aNextChord, tNextChordData, 1);
    }

    m_fCreated = true;
    return true;
}

//---------------------------------------------------------------------------------------
Cadence::~Cadence()
{
    if (m_pTonicChord)
        delete m_pTonicChord;

    for (int i=0; i < m_nNumChords; i++)
    {
        if (m_pChords[i])
            delete m_pChords[i];
    }
}

//---------------------------------------------------------------------------------------
Chord* Cadence::GetChord(int iC)
{
    // returns a pointer to chord iC (0..n-1).
    // if iC is out of range returns NULL pointer

    if (iC >=0 && iC < m_nNumChords)
        return m_pChords[iC];
    else
        return (Chord*) NULL;
}

//---------------------------------------------------------------------------------------
wxString Cadence::SelectChord(wxString sFunction, EKeySignature nKey, int* pInversion)
{
    // returns the intervals that form the chord, or empty string if errors
    // and updates variable pointed by pInversion to place the number of the
    // applicable inversion

    //Strip out inversions
    wxString sFunc;
    int iSlash = sFunction.find(_T('/'));
    if (iSlash != (int)wxStringBase::npos) {
        sFunc = sFunction.substr(0, iSlash);
        wxString sInv = sFunction.substr(iSlash+1);
        if (sInv==_T("6"))
            *pInversion = 1;
        else if (sInv==_T("64"))
            *pInversion = 2;
            //debug
        else {
            TraceCadence(_T("[Cadence::SelectChord] Conversion table maintenance error. Unknown inversion code '%s'"), sInv.c_str());
            *pInversion = 0;
        }
    }
    else {
        sFunc = sFunction;
        *pInversion = 0;
    }

    // look for function
    int iF, iMax = sizeof(m_aFunctionData)/sizeof(FunctionData);
    for (iF=0; iF < iMax; iF++) {
        if (m_aFunctionData[iF].sFunction == sFunc) break;
    }
    if (iF == iMax) {
        // table maintenance error
            //debug
        TraceCadence(_T("[Cadence::SelectChord] Conversion table maintenance error. Function '%s' not found. Key=%d"),
            sFunction.c_str(), nKey);
        return _T("");      // not valid chord
    }

    if (is_major_key(nKey))
    {
        // major key: return chord for major key
        wxString sChord = m_aFunctionData[iF].sChordMajor;
        if (sChord == _T("nil"))
            return _T("");        // not valid chord
        if (sChord == _T("?")) {
            //debug
            TraceCadence(_T("[Cadence::SelectChord] Conversion table maintenance error. Undefined chord. Function '%s', Key=%d"),
                sFunction.c_str(), nKey);
            return _T("");        // not valid chord
        }
        else
            return sChord;
    }
    else
    {
        // minor key: return one of the chords for minor key
        wxString sChord1 = m_aFunctionData[iF].sChordMinor1;
        wxString sChord2 = m_aFunctionData[iF].sChordMinor2;
        if (sChord1 == _T("nil"))
            return _T("");        // not valid chord
        else if (sChord2 == _T("nil"))
            return sChord1;
        else {
            RandomGenerator oRnd;
            if (oRnd.flip_coin())
                return sChord1;
            else
                return sChord2;
       }
    }
}

//---------------------------------------------------------------------------------------
FPitch Cadence::GetRootNote(wxString sFunct, EKeySignature nKey, EClef nClef,
                              bool fUseGrandStaff)
{
    //Get root note for this key signature
    int step = get_step_for_root_note(nKey);    //index (0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si) to root note

    // add function grade
    size_t nSize = sFunct.length();
    if (nSize > 2)
    {
        if (sFunct.compare(0, 3, _T("VII")) == 0)
            step += 6;
        else if (sFunct.compare(0, 3, _T("III")) == 0)
            step += 2;
        else if (sFunct.compare(0, 2, _T("VI")) == 0)
            step += 5;
        else if (sFunct.compare(0, 2, _T("IV")) == 0)
            step += 3;
        else if (sFunct.compare(0, 2, _T("II")) == 0)
            step += 1;
        else if (sFunct.compare(0, 1, _T("V")) == 0)
            step += 4;
    }
    else if (nSize > 1)
    {
        if (sFunct.compare(0, 2, _T("VI")) == 0)
            step += 5;
        else if (sFunct.compare(0, 2, _T("IV")) == 0)
            step += 3;
        else if (sFunct.compare(0, 2, _T("II")) == 0)
            step += 1;
        else if (sFunct.compare(0, 1, _T("V")) == 0)
            step += 4;
    }
    else if (sFunct.compare(0, 1, _T("V")) == 0)
        step += 4;

    step = step % 7;

    //Get accidentals for this note
    int nAccidentals[7];
    get_accidentals_for_key(nKey, nAccidentals);
    int acc = nAccidentals[step];

    //octave depends on clef and use of grand-staff
    // TODO: Take more clefs into account
    int octave;
    if (fUseGrandStaff && step >= k_step_G)
        octave = (step >= k_step_G ? 2 : 3);
    else if (nClef == k_clef_G2)
        octave = 4;
    else
        wxASSERT(false);

    return FPitch(step, octave, acc);
}

//---------------------------------------------------------------------------------------
wxString Cadence::GetName()
{
    //get the functions
    int iC;
	wxString sName = CadenceTypeToName(m_nType) + _T(" : ");
    for (iC=0; iC < k_chords_in_cadence; iC++)
    {
		wxString sFunct;
        // get the function
        if (m_nType == k_cadence_imperfect_V_I)
            sFunct = aImperfect[m_nImperfectCad][iC];
        else
            sFunct = aFunction[m_nType][iC];
        if (sFunct == _T("")) break;
		if (iC != 0) sName += _T(" -> ");
		sName += sFunct;
	}
	return sName;

}

//---------------------------------------------------------------------------------------
int Cadence::GenerateFirstChord(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
								  Chord* pChord, int nInversion)
{
    // Generates all possible chords for the first chord for a cadence.
    // Returns the number of valid chords found.
    // Generated chords are placed in vector aChords. They are filtered and marked

    #define lmMAX_NOTES 12      // max notes in a set for a voice: 3 octaves * 4 notes

    // Ranges allowed by most theorists for each voice. Both notes inclusive
    // Soprano: d4-g5,  Alto: g3-d5,  Tenor: d3-g4,  Bass: e2-d4
	// 	        e4-a5	      a3-d5			 f3-g4		   e2-c4
    DiatonicPitch nMinAltoPitch(k_step_G, 3);
    DiatonicPitch nMaxAltoPitch(k_step_D, 5);
    DiatonicPitch nMinTenorPitch(k_step_D, 3);
    DiatonicPitch nMaxTenorPitch(k_step_G, 4);


    // 1. Get chord note-steps for the required harmonic function.
    int nNumNotes = pChord->get_num_notes();   // notes in chord
    NoteBits oChordNotes[4];
    for (int i=0; i < nNumNotes; i++) {
        oChordNotes[i].nAccidentals = pChord->get_num_accidentals(i);
        oChordNotes[i].nStep = tChordData.nSteps[i] = pChord->get_step(i);
        oChordNotes[i].nOctave = 0;
    }
    tChordData.nStep5 = tChordData.nSteps[2];     // the fifth of the chord
	tChordData.nNumSteps = nNumNotes;

#if (TRACE_CADENCE == 1)
    //START DBG -------------------------------------------------------------
		wxLogMessage(_T("[Cadence::GenerateFirstChord] Base chord. Num notes = %d"), nNumNotes);
		wxString sNotes = _T("cdefgabc");
		for (int i=0; i < nNumNotes; i++)
        {
			wxLogMessage(_T("[Cadence::GenerateFirstChord] note %s"),
					sNotes.substr(oChordNotes[i].nStep, 1).c_str() );
		}
    //END DBG ---------------------------------------------------------------
#endif


    // 2. Generates the note for the bass voice, placed in the lower part of the
    // Bass staff (notes: g2,a2,b2,c3,d3,e3,f3).
    lmHChord oChord;
    int iB = nInversion;
    if (oChordNotes[iB].nStep > 3)        // if note step is g, a or b
        oChordNotes[iB].nOctave = 2;      //  place note in octave 2
    else                            // else
        oChordNotes[iB].nOctave = 3;      //  place note in octave 3
    oChord.nNote[0] = DiatonicPitch(oChordNotes[iB].nStep, oChordNotes[iB].nOctave);
    oChord.nAcc[0] = oChordNotes[iB].nAccidentals;
            //debug
    TraceCadence(_T("[Cadence::GenerateFirstChord] Bass = %s"),
                    oChord.GetPrintName(0).c_str() );

    // 3. Generate note for soprano voice
    //    At random, any of the chord notes (except the bass one)
    int iS = GenerateSopranoNote(oChordNotes, iB, nNumNotes);
    oChord.nNote[3] = DiatonicPitch(oChordNotes[iS].nStep, oChordNotes[iS].nOctave);
    oChord.nAcc[3] = oChordNotes[iS].nAccidentals;
            //debug
    TraceCadence(_T("[Cadence::GenerateFirstChord] Soprano = %s"),
                    oChord.GetPrintName(3).c_str() );

    // 4. Generate the set of possible notes for Alto voice.
    //    All chord notes possible
    int nAltoSet[lmMAX_NOTES];
    int nAltoSetAcc[lmMAX_NOTES];
    int iAS = 0;
    int iA;
    for (iA=0; iA < nNumNotes; iA++) {
        DiatonicPitch nAltoPitch(oChordNotes[iA].nStep, 3);
        DiatonicPitch nSopranoPitch = oChord.nNote[3];
        while (nAltoPitch < nSopranoPitch && nAltoPitch < nMaxAltoPitch)
        {
            if (nAltoPitch >= nMinAltoPitch)
            {
                nAltoSetAcc[iAS] = oChordNotes[iA].nAccidentals;
                nAltoSet[iAS++] = nAltoPitch;
            //debug
                TraceCadence(_T("[Cadence::GenerateFirstChord] Alto = %s"),
                            to_wx_string(nAltoPitch.get_english_note_name()).c_str() );
            }
            nAltoPitch += 7;
        }
    }

    // 5. Generate the set of possible notes for Tenor voice.
    //    All chord notes possible
    int nTenorSet[lmMAX_NOTES];
    int nTenorSetAcc[lmMAX_NOTES];
    int iTS = 0;
    int iT;
    for (iT=0; iT < nNumNotes; iT++) {
        DiatonicPitch nTenorPitch = DiatonicPitch(oChordNotes[iT].nStep, 3);
        while (nTenorPitch < nMaxTenorPitch) {
            if (nTenorPitch >= nMinTenorPitch) {
                nTenorSetAcc[iTS] = oChordNotes[iT].nAccidentals;
                nTenorSet[iTS++] = nTenorPitch;
            //debug
                TraceCadence(_T("[Cadence::GenerateFirstChord] Tenor = %s"),
                            to_wx_string(nTenorPitch.get_english_note_name()).c_str() );
            }
            nTenorPitch += 7;
        }
    }

    // Generate the set of possible chords
    int nNumChords = iAS * iTS;
    aChords.resize(nNumChords);
    int iC = 0;
    for (int iA=0; iA < iAS; iA++) {
        for (int iT=0; iT < iTS; iT++) {
            // bass
            aChords[iC].nNote[0] = oChord.nNote[0];
            aChords[iC].nAcc[0] = oChord.nAcc[0];
            // soprano
            aChords[iC].nNote[3] = oChord.nNote[3];
            aChords[iC].nAcc[3] = oChord.nAcc[3];
            // alto
            aChords[iC].nNote[2] = nAltoSet[iA];
            aChords[iC].nAcc[2] = nAltoSetAcc[iA];
            // tenor
            aChords[iC].nNote[1] = nTenorSet[iT];
            aChords[iC].nAcc[1] = nTenorSetAcc[iT];
            //
            aChords[iC].nReason = lm_eChordValid;
            aChords[iC].nSeverity = lm_eChordValid;
            aChords[iC].nNumNotes = 4;
            iC++;
        }
    }
    aChords.resize(nNumChords);

#if (TRACE_CADENCE == 1)
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[Cadence::GenerateFirstChord] Num Chords = %d"), nNumChords);
    for (int i=0; i < nNumChords; i++)
    {
        wxLogMessage(_T("[Cadence::GenerateFirstChord] Chord %d : %s, %s, %s, %s"),
                i,
                aChords[i].GetPrintName(0).c_str(),
                aChords[i].GetPrintName(1).c_str(),
                aChords[i].GetPrintName(2).c_str(),
                aChords[i].GetPrintName(3).c_str() );
    }
    //END DBG ---------------------------------------------------------------
#endif

    int nValidChords = FilterChords(aChords, nNumChords, tChordData, (lmHChord*)NULL);
#if (TRACE_CADENCE == 1)
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[Cadence::GenerateFirstChord] Valid Chords = %d"), nValidChords);
    for (int i=0; i < nNumChords; i++) {
        if (aChords[i].nReason == lm_eChordValid)
        {
            wxLogMessage(_T("[Cadence::GenerateFirstChord] Valid chord %d : %s, %s, %s, %s"),
                i,
                aChords[i].GetPrintName(0).c_str(),
                aChords[i].GetPrintName(1).c_str(),
                aChords[i].GetPrintName(2).c_str(),
                aChords[i].GetPrintName(3).c_str() );
        }
    }
    //END DBG ---------------------------------------------------------------
#endif

    return nValidChords;

}

//---------------------------------------------------------------------------------------
int Cadence::GenerateNextChord(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
							   Chord* pChord, int nInversion, int iPrevHChord)
{
    // Generates the next chord for a cadence.
    // returns the number of valid chords generated

    int iThisChord = iPrevHChord + 1;

    // 1. Get chord notes (no octave) for the required harmonic function
    int nNumNotes = pChord->get_num_notes();   // notes in chord
    NoteBits oChordNotes[4];
    for (int i=0; i < nNumNotes; i++)
    {
        oChordNotes[i].nAccidentals = pChord->get_num_accidentals(i);
        oChordNotes[i].nStep = tChordData.nSteps[i] = pChord->get_step(i);
        oChordNotes[i].nOctave = 0;
    }
    tChordData.nStep5 = tChordData.nSteps[2];     // the fifth of the chord
	tChordData.nNumSteps = nNumNotes;

#if (TRACE_CADENCE == 1)
    //START DBG -------------------------------------------------------------
    TraceCadence(_T("[Cadence::GenerateNextChord] Base chord. Num notes = %d"), nNumNotes);
    wxString sNotes = _T("cdefgabc");
    for (int i=0; i < nNumNotes; i++)
    {
        TraceCadence(_T("[Cadence::GenerateNextChord] note %s"),
                sNotes.substr(oChordNotes[i].nStep, 1).c_str() );
    }
    //END DBG ---------------------------------------------------------------
#endif


    // 2. Generates the note for the bass voice, placed in the lower part of the
    // Bass staff (notes: g2,a2,b2,c3,d3,e3,f3).
    int iB = nInversion;
    if (oChordNotes[iB].nStep > 3)        // if note step is g, a or b
        oChordNotes[iB].nOctave = 2;      //  place note in octave 2
    else                            // else
        oChordNotes[iB].nOctave = 3;      //  place note in octave 3
    DiatonicPitch nBassPitch(oChordNotes[iB].nStep, oChordNotes[iB].nOctave);
    m_Chord[iThisChord].nNote[0] = nBassPitch;
    m_Chord[iThisChord].nAcc[0] = oChordNotes[iB].nAccidentals;
            //debug
    TraceCadence(_T("[Cadence::GenerateNextChord] Bass = %s"),
                    to_wx_string(m_Chord[iThisChord].nNote[0].get_english_note_name()).c_str() );

    // generate the set of possible notes for each chord note
    DiatonicPitch nSetNotes[20];
    int nSetAcc[20];
    int nSet = 0;       //points to first not used entry = number of entries
    DiatonicPitch nMaxPitch = DiatonicPitch(k_step_C, 6);
    int iN;             // point to note in process
    for (iN=0; iN < nNumNotes; iN++)
    {
        DiatonicPitch nDiatonicPitch(oChordNotes[iN].nStep, 2);
        if (nDiatonicPitch < nBassPitch)
            nDiatonicPitch += 7;
        while (nDiatonicPitch <= nMaxPitch)
        {
            nSetNotes[nSet] = nDiatonicPitch;
            nSetAcc[nSet++] = oChordNotes[iN].nAccidentals;
                //debug
            TraceCadence(_T("[Cadence::GenerateNextChord] Added to set = %s (%d)"),
                         to_wx_string(nDiatonicPitch.get_english_note_name()).c_str(), int(nDiatonicPitch) );
            nDiatonicPitch += 7;
        }
    }

    // sort notes set, ascending. Bubble method
    for (int i = 0; i < nSet; i++)
    {
        bool fSwap = false;
        int j = nSet -1;      // last element
        while(j != i)
        {
            int k = j-1;
            if (nSetNotes[j] < nSetNotes[k])
            {
                fSwap = true;
                int nNote = nSetNotes[j];
                int nAcc = nSetAcc[j];
                nSetNotes[j] = nSetNotes[k];
                nSetAcc[j] = nSetAcc[k];
                nSetNotes[k] = nNote;
                nSetAcc[k] = nAcc;
            }
            j = k;
        }
        if (!fSwap) break;
    }
#if (TRACE_CADENCE == 1)
    //START DBG -------------------------------------------------------------
    for (int i=0; i < nSet; i++)
    {
        TraceCadence(_T("[Cadence::GenerateNextChord] ordered set: note %d = %s"),
                i, to_wx_string(nSetNotes[i].get_english_note_name()).c_str() );
    }
    //END DBG ---------------------------------------------------------------
#endif

    //for each previous chord note (except bass) find the nearest ones to move to
    //and for the sets of eligible notes.  To simplify, all arrays include the bass
    //voice (array index 0) but it is not used.
    DiatonicPitch nElegibleNote[4][5];       //four voices, five possible alternatives for each voice
    int nElegibleAcc[4][5];
    int nE[4];                          //number of found alternatives for each voice.

    for (iN = 1; iN < 4; iN++)
    {
        DiatonicPitch nNote = m_Chord[iPrevHChord].nNote[iN];
        int nAcc = m_Chord[iPrevHChord].nAcc[iN];
        nE[iN] = 0;     // no notes yet
        // find this note in set
        for(int i=0; i < nSet; i++)
        {
            if (nNote == nSetNotes[i])
            {
                // note in chord. keep it.
                nElegibleNote[iN][nE[iN]] = nNote;
                nElegibleAcc[iN][nE[iN]++] = nAcc;

                // and add also the next and previous ones (two up and two down)
                if (i > 0) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i-1];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i-1];
                }
                if (i > 1) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i-2];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i-2];
                }
                if (i+1 < nSet) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i+1];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i+1];
                }
                if (i+2 < nSet) {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i+2];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i+2];
                }
                break;
            }
            else if (nNote < nSetNotes[i])
            {
                // note not in chord. Take the two nearest ones up and down
                if (i > 0)
                {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i-1];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i-1];
                }
                if (i > 1)
                {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i-2];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i-2];
                }
                nElegibleNote[iN][nE[iN]] = nSetNotes[i];
                nElegibleAcc[iN][nE[iN]++] = nSetAcc[i];
                if (i+1 < nSet)
                {
                    nElegibleNote[iN][nE[iN]] = nSetNotes[i+1];
                    nElegibleAcc[iN][nE[iN]++] = nSetAcc[i+1];
                }
                break;
            }
        }
    }

#if (TRACE_CADENCE == 1)
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[Cadence::GenerateNextChord] Elegible set S: Num.notes=%d, notes: %d, %d, %d, %d, %d"),
        nE[3],
        int(nElegibleNote[3][0]),
        int(nElegibleNote[3][1]),
        int(nElegibleNote[3][2]),
        int(nElegibleNote[3][3]),
        int(nElegibleNote[3][4])
    );
    wxLogMessage(_T("[Cadence::GenerateNextChord] Elegible set S: Num.notes=%d, %s, %s, %s, %s, %s"),
        nE[3],
        to_wx_string(nElegibleNote[3][0].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[3][1].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[3][2].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[3][3].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[3][4].get_english_note_name()).c_str() );
    wxLogMessage(_T("[Cadence::GenerateNextChord] Elegible set A: Num.notes=%d, notes: %d, %d, %d, %d, %d"),
        nE[2],
        int(nElegibleNote[2][0]),
        int(nElegibleNote[2][1]),
        int(nElegibleNote[2][2]),
        int(nElegibleNote[2][3]),
        int(nElegibleNote[2][4])
    );
    wxLogMessage(_T("[Cadence::GenerateNextChord] Elegible set A: Num.notes=%d, %s, %s, %s, %s, %s"),
        nE[2],
        to_wx_string(nElegibleNote[2][0].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[2][1].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[2][2].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[2][3].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[2][4].get_english_note_name()).c_str() );
    wxLogMessage(_T("[Cadence::GenerateNextChord] Elegible set T: Num.notes=%d, notes: %d, %d, %d, %d, %d"),
        nE[1],
        int(nElegibleNote[1][0]),
        int(nElegibleNote[1][1]),
        int(nElegibleNote[1][2]),
        int(nElegibleNote[1][3]),
        int(nElegibleNote[1][4])
    );
    wxLogMessage(_T("[Cadence::GenerateNextChord] Elegible set T: Num.notes=%d, %s, %s, %s, %s, %s"),
        nE[1],
        to_wx_string(nElegibleNote[1][0].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[1][1].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[1][2].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[1][3].get_english_note_name()).c_str(),
        to_wx_string(nElegibleNote[1][4].get_english_note_name()).c_str() );
    //END DBG ---------------------------------------------------------------
#endif

    // create the set of possible chords
    int nNumChords = nE[1] * nE[2] * nE[3];
    aChords.resize(nNumChords);
    int iC = 0;
    for (int iS=0; iS < nE[3]; iS++) {
        for (int iA=0; iA < nE[2]; iA++) {
            for (int iT=0; iT < nE[1]; iT++) {
                // bass
                aChords[iC].nNote[0] = m_Chord[iThisChord].nNote[0];
                aChords[iC].nAcc[0] = m_Chord[iThisChord].nAcc[0];
                // tenor
                aChords[iC].nNote[1] = nElegibleNote[1][iT];
                aChords[iC].nAcc[1] = nElegibleAcc[1][iT];

                if (aChords[iC].nNote[1] > aChords[iC].nNote[0]) {
                    // alto
                    aChords[iC].nNote[2] = nElegibleNote[2][iA];
                    aChords[iC].nAcc[2] = nElegibleAcc[2][iA];

                    if (aChords[iC].nNote[2] > aChords[iC].nNote[1]) {
                        // soprano
                        aChords[iC].nNote[3] = nElegibleNote[3][iS];
                        aChords[iC].nAcc[3] = nElegibleAcc[3][iS];

                        if (aChords[iC].nNote[3] > aChords[iC].nNote[2]) {
                            aChords[iC].nReason = lm_eChordValid;
                            aChords[iC].nSeverity = lm_eChordValid;
                            aChords[iC].nNumNotes = 4;
                            iC++;
                        }
                    }
                }
            }
        }
    }
    nNumChords = iC;
    aChords.resize(nNumChords);

#if (TRACE_CADENCE == 1)
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[Cadence::GenerateNextChord] Num possible chords = %d"), nNumChords);
    for (int i=0; i < nNumChords; i++)
    {
        wxLogMessage(_T("[Cadence::GenerateNextChord] Possible chord %d : %s, %s, %s, %s"),
                i,
                to_wx_string(aChords[i].nNote[0].get_english_note_name()).c_str(),
                to_wx_string(aChords[i].nNote[1].get_english_note_name()).c_str(),
                to_wx_string(aChords[i].nNote[2].get_english_note_name()).c_str(),
                to_wx_string(aChords[i].nNote[3].get_english_note_name()).c_str() );
    }
    //END DBG ---------------------------------------------------------------
#endif

    //Filter invalid chords
    int nValidChords = FilterChords(aChords, nNumChords, tChordData, &m_Chord[iPrevHChord]);

    //        //debug
    //TraceCadence(_T("[Cadence::GenerateNextChord] Valid Chords = %d"), nValidChords);
	Debug_DumpAllChords(aChords);

    if (nValidChords < 1) return nValidChords;
    // choose one at random
    RandomGenerator oRnd;
    int iSel = oRnd.random_number(1, nValidChords);
    int nValid = 0;
    for (iC=0; iC < nNumChords; iC++) {
        if (aChords[iC].nReason == lm_eChordValid) {
            if (iSel == ++nValid) break;
        }
    }
    // chord iC is the selected one.
    m_Chord[iThisChord] = aChords[iC];

#if (TRACE_CADENCE == 1)
    //START DBG -------------------------------------------------------------
    wxLogMessage(_T("[Cadence::GenerateNextChord] Selected : %s, %s, %s, %s"),
        to_wx_string(aChords[iC].nNote[0].get_english_note_name()).c_str(),
        to_wx_string(aChords[iC].nNote[1].get_english_note_name()).c_str(),
        to_wx_string(aChords[iC].nNote[2].get_english_note_name()).c_str(),
        to_wx_string(aChords[iC].nNote[3].get_english_note_name()).c_str() );
    //END DBG ---------------------------------------------------------------
#endif

    return nValidChords;
}

//---------------------------------------------------------------------------------------
int Cadence::GenerateSopranoNote(NoteBits oChordNotes[4], int iBass,
                                          int nNumNotes)
{
    // Generates a note for the Soprano voice, located in the upper part of the
    // upper staff (notes: g4,a4,b4,c5,d5,e5,f5).

    // Algorithm: choose at random one of the chord notes (except the bass one),
    // and place it on the allowed notes range (notes: g4,a4,b4,c5,d5,e5,f5)


    // 1. choose at random one of the chord notes (except the bass one)
    RandomGenerator oRnd;
    int nWatchDog = 0;
    int iS = iBass;
    while (iS == iBass) {
        iS = oRnd.random_number(0, nNumNotes-1);
        if (nWatchDog++ == 1000) {
            wxLogMessage(_T("Program error: Loop detected in Cadence::GenerateSopranoNote"));
            return iBass;   // at least, this one exists!
        }
    }

    // 2. place note on the allowed notes range (notes: g4,a4,b4,c5,d5,e5,f5)
    if (oChordNotes[iS].nStep > 3)        // if note step is g, a or b
        oChordNotes[iS].nOctave = 4;      //  place note in octave 4
    else                            // else
        oChordNotes[iS].nOctave = 5;      //  place note in octave 5

    return iS;

}

//---------------------------------------------------------------------------------------
int Cadence::FilterChords(std::vector<lmHChord>& aChords, int nNumChords,
                            lmChordAuxData& tChordData, lmHChord* pPrevChord,
                            bool fExhaustive)
{
    // Parameters:
    //  - if fExhaustive == true it will check each chord against all validation rules
    //
    // Validations:
    // 1. The chord is complete (has all note steps)
    // *2. lm_eFifthMissing - Acorde completo. Contiene todas las notas (en todo caso, elidir la 5ª)
    // 3. No parallel motion of perfect octaves, perfect fifths, and unisons
    // *4. lm_eResultantFifthOctves - No hacer 5ªs ni 8ªs resultantes, excepto:
                                            //> a) la soprano se ha movido por segundas
                                            //> b) (para 5ªs) uno de los sonidos ya estaba
    // 5. The fifth is not doubled
	// 6. The leading tone is never doubled
    // 7. Scale degree seven (the leading tone) should resolve to tonic.
    // *8.lm_eSeventhResolution - the seventh of a chord should always resolve down by second.
    // 9. voices interval not greater than one octave (except bass-tenor)
    // 10. Do not allow voices crossing. No duplicates
    // 11. Voice overlap: when a voice moves above or below a pitch previously sounded by another voice.
    // 12. Resolve chromatic alterations by step in the same direction than the alteration.
    // *13.lm_eGreaterThanSixth - No es conveniente exceder el intervalo de sexta, exceptuando la octava justa
    // 14. If bass moves by step all other voices moves in opposite direction to bass
    // *15.lm_eNotDoubledThird - Cuando el bajo enlaza el V grado con el VI (cadencia rota), en el acorde de VI grado se duplica la tercera.




    //locate leading tone in previous chord
    int iLeading = -1;      // index to leading note in previous chord or -1 if none
    int nRoot = get_step_for_root_note(m_nKey);    //index (0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si) to root note
    int nStepLeading = (nRoot == 0 ? 6 : nRoot-1);      //scale degree seven
    if (pPrevChord)
    {
        //Check if leading tone is present in previous chord
        for (int iN=0; iN < 4; iN++) {
            if (pPrevChord->nNote[iN].step() == nStepLeading)
            {
                iLeading = iN;
                break;
            }
        }
    }

    //locate chromatic alterations (accidentals not in key signature)
    int nPrevAlter[4] = {0,0,0,0};
    if (pPrevChord) {
        int nKeyAccidentals[7];
        get_accidentals_for_key(m_nKey, nKeyAccidentals);
        for (int iN=0; iN < 4; iN++) {
            int nStep = pPrevChord->nNote[iN].step();
            nPrevAlter[iN] = pPrevChord->nAcc[iN] - nKeyAccidentals[nStep];
            //debug
            TraceCadence(_T("[Cadence::FilterChords] nPrevAlter[%d] = %d"), iN, nPrevAlter[iN] );
        }
    }



    for (int i=0; i < nNumChords; i++)
    {
        //1. The chord is complete. All steps are in the chord
        if (fExhaustive || aChords[i].nReason == lm_eChordValid) {
            bool fFound[4] = { false, false, false, false };
            for (int iN=0; iN < 4; iN++) {
				int nStep = aChords[i].nNote[iN].step();
                for(int j=0; j < tChordData.nNumSteps; j++) {
                    if (nStep == tChordData.nSteps[j]) {
                        fFound[j] = true;
                        break;
                    }
                }
            }
            bool fValid = true;
            for(int j=0; j < tChordData.nNumSteps; j++) {
                fValid &= fFound[j];
            }
			if (!fValid) {
                aChords[i].nReason = lm_eNotAllNotes;
                aChords[i].nSeverity |= lm_eNotAllNotes;
                aChords[i].nImpact = 0;
			}
        }

        //2. lm_eFifthMissing        = 0x2000,   // Acorde completo. Contiene todas las notas (en todo caso, elidir la 5ª)

        //3. No parallel motion of perfect octaves or perfect fifths
        int aMotion[4];
        if (pPrevChord && (fExhaustive || aChords[i].nReason == lm_eChordValid)) {
            int nUpFifth = 0;
            int nUpOctave = 0;
            int nDownFifth = 0;
            int nDownOctave = 0;
            for (int iN=0; iN < 4; iN++) {
                int nMotion = aChords[i].nNote[iN] - pPrevChord->nNote[iN];
                if (nMotion != 0) {
                    if (nMotion > 0)
                        nMotion++;
                    else
                        nMotion--;
                }
                if (nMotion == 5)
                    nUpFifth++;
                else if (nMotion == 8)
                    nUpOctave++;
                else if (nMotion == -5)
                    nDownFifth++;
                else if (nMotion == -8)
                    nDownOctave++;

                aMotion[iN] = nMotion;
            }
			if (nUpFifth > 1 || nUpOctave > 1 || nDownFifth > 1 || nDownOctave > 1) {
                aChords[i].nReason = lm_eFifthOctaveMotion;
                aChords[i].nSeverity |= lm_eFifthOctaveMotion;
                aChords[i].nImpact = 0;
			}
        }

        //4. lm_eResultantFifthOctves = 0x0800,  //3. No hacer 5ªs ni 8ªs resultantes, excepto:
                                            //> a) la soprano se ha movido por segundas
                                            //> b) (para 5ªs) uno de los sonidos ya estaba

        //5. The fifth is not doubled
        //6. The leading tone is never doubled
        if (fExhaustive || aChords[i].nReason == lm_eChordValid)
        {
            int nFifths = 0;
            int nLeadingTones = 0;
            for (int iN=0; iN < 4; iN++)
            {
                int nStep = aChords[i].nNote[iN].step();
                if (nStep == tChordData.nStep5)
                    nFifths++;
                else if (nStep == nStepLeading)
                    nLeadingTones++;
            }
            if (nFifths > 1)
            {
                aChords[i].nReason = lm_eFifthDoubled;
                aChords[i].nSeverity |= lm_eFifthDoubled;
                aChords[i].nImpact = 0;
            }
            if (nLeadingTones > 1)
            {
                aChords[i].nReason = lm_eLeadingToneDoubled;
                        aChords[i].nSeverity |= lm_eLeadingToneDoubled;
                        aChords[i].nImpact = 0;
            }
        }

        //7. Scale degree seven (the leading tone) should resolve to tonic.
        if (pPrevChord && iLeading >=0 && (fExhaustive || aChords[i].nReason == lm_eChordValid)) {
            // leading tone should move up by step
            if (aMotion[iLeading] != 2) {
                aChords[i].nReason = lm_eLeadingResolution;
                aChords[i].nSeverity |= lm_eLeadingResolution;
                aChords[i].nImpact = (iLeading==0 || iLeading==3 ? 1 : 0);
            }
        }

        //8. lm_eSeventhResolution   the seventh of a chord should always resolve down by second.

        //9. notes interval not greater than one octave (except bass-tenor)
        if ((fExhaustive || aChords[i].nReason == lm_eChordValid) &&
            ((int)aChords[i].nNote[2] - (int)aChords[i].nNote[1] > 7 ||
             (int)aChords[i].nNote[3] - (int)aChords[i].nNote[2] > 7 ))
        {
            aChords[i].nReason = lm_eGreaterThanOctave;
            aChords[i].nSeverity |= lm_eGreaterThanOctave;
            aChords[i].nImpact =
                ((int)aChords[i].nNote[3] - (int)aChords[i].nNote[2] > 7 ? 1 : 0);
        }

        //10. notes in ascending sequence (not to allow voices crossing). No duplicates
        if ((fExhaustive || aChords[i].nReason == lm_eChordValid) &&
            (aChords[i].nNote[1] <= aChords[i].nNote[0] ||
             aChords[i].nNote[2] <= aChords[i].nNote[1] ||
             aChords[i].nNote[3] <= aChords[i].nNote[2] ))
        {
            aChords[i].nReason = lm_eVoiceCrossing;
            aChords[i].nSeverity |= lm_eVoiceCrossing;
            if (aChords[i].nNote[1] <= aChords[i].nNote[0] ||
                aChords[i].nNote[3] <= aChords[i].nNote[2] )
            {
                aChords[i].nImpact = 1;
            }
            else
                aChords[i].nImpact = 0;
        }

        //11. No voice overlap
        if (pPrevChord && (fExhaustive || aChords[i].nReason == lm_eChordValid)) {
            bool fUpMotion;
            for (int iN=0; iN < 4; iN++) {
                if (aChords[i].nNote[iN] != pPrevChord->nNote[iN]) {
                    fUpMotion = (aChords[i].nNote[iN] > pPrevChord->nNote[iN]);
                    if (iN < 3 && fUpMotion && aChords[i].nNote[iN] > pPrevChord->nNote[iN+1]) {
                        aChords[i].nReason = lm_eVoiceOverlap;
                        aChords[i].nSeverity |= lm_eVoiceOverlap;
                        aChords[i].nImpact = (iN==0 || iN+1==3 ? 1 : 0);
                        break;
                    }
                    if (iN > 0 && !fUpMotion && aChords[i].nNote[iN] < pPrevChord->nNote[iN-1]) {
                        aChords[i].nReason = lm_eVoiceOverlap;
                        aChords[i].nSeverity |= lm_eVoiceOverlap;
                        aChords[i].nImpact = (iN==3 || iN-1==0 ? 1 : 0);
                        break;
                    }
                }
            }
        }

        //12. Resolve chromatic alterations [by step] in the same direction than the alteration.
        if (pPrevChord && (fExhaustive || aChords[i].nReason == lm_eChordValid)) {
            for (int iN=0; iN < 4; iN++) {
                if (aChords[i].nNote[iN] != pPrevChord->nNote[iN] && nPrevAlter[iN] != 0) {
                    if (((aChords[i].nNote[iN] > pPrevChord->nNote[iN]) && nPrevAlter[iN] < 0) ||
                        ((aChords[i].nNote[iN] < pPrevChord->nNote[iN]) && nPrevAlter[iN] > 0) )
                    {
                        aChords[i].nReason = lm_eChromaticAlter;
                        aChords[i].nSeverity |= lm_eChromaticAlter;
                        aChords[i].nImpact = (iN==0 || iN==3 ? 1 : 0);
                        break;
                    }
                }
            }
        }

        //13. lm_eGreaterThanSixth    = 0x0004,   //No es conveniente exceder el intervalo de sexta, exceptuando la octava justa

        //14. If bass moves by step all other voices moves in opposite direction to bass
        if (pPrevChord && (fExhaustive || aChords[i].nReason == lm_eChordValid)) {
            if (aMotion[0] == 1) {
                // bass motion: up
                for (int iN=1; iN < 4; iN++) {
                    if (aMotion[iN] > 0) {
                        aChords[i].nReason = lm_eNotContraryMotion;
                        aChords[i].nSeverity |= lm_eNotContraryMotion;
                        aChords[i].nImpact = (iN==0 || iN==3 ? 1 : 0);
                        break;
                    }
                }
            }
            if (aMotion[0] == -1) {
                // bass motion: down
                for (int iN=1; iN < 4; iN++) {
                    if (aMotion[iN] < 0) {
                        aChords[i].nReason = lm_eNotContraryMotion;
                        aChords[i].nSeverity |= lm_eNotContraryMotion;
                        aChords[i].nImpact = (iN==0 || iN==3 ? 1 : 0);
                        break;
                    }
                }
            }
        }

        //15. lm_eNotDoubledThird     = 0x0001,   //Cuando el bajo enlaza el V grado con el VI (cadencia rota), en el acorde de VI grado se duplica la tercera.


    }

    // count valid chords
    int nValid = 0;
    for (int i=0; i < nNumChords; i++)
        if (aChords[i].nReason == lm_eChordValid) nValid++;

    return nValid;
}

//---------------------------------------------------------------------------------------
void Cadence::SelectLessBad(std::vector<lmHChord>& aChords, lmChordAuxData& tChordData,
							  int iHChord)
{
    // None of the chords in vector aChords is valid. This method selects the less bad
    // chord and places its data in element iHChord of global variable m_Chord

    // Apply all checking rules
	lmHChord* pPrevChord = (iHChord == 0 ? (lmHChord*)NULL : &m_Chord[iHChord-1] );
    FilterChords(aChords, (int)aChords.size(), tChordData, pPrevChord, lmEXHAUSTIVE);

    //find minimum impact chords
    int aImpact[4] = {0,0,0,0};      //AWARE: dimension must be grater than number of impact values
    int nMinImpact = 999;
    for (int i=0; i < (int)aChords.size(); i++) {
        int iP = aChords[i].nImpact;
        aImpact[iP]++;
        nMinImpact = wxMin(iP, nMinImpact);
    }
	Debug_DumpAllChords(aChords);

    //find minimum severity chords with the minimun impact
    int iSel = 0;
    lmChordError nMinSeverity = lm_eMaxSeverity;
    for (int i=0; i < (int)aChords.size(); i++) {
        if (aChords[i].nImpact == nMinImpact) {
            if (aChords[i].nSeverity < nMinSeverity) {
                nMinSeverity = aChords[i].nSeverity;
                iSel = i;
            }
        }
    }

    // chord iSel is the selected one. Move its data to m_Chord
    m_Chord[iHChord] = aChords[iSel];
            //debug
    TraceCadence(_T("[Cadence::SelectLessBad] Less bad chord selected: iSel=%d"), iSel);
    Debug_DumpChord(aChords[iSel], iSel);

}

//---------------------------------------------------------------------------------------
string Cadence::get_rel_ldp_name(int iChord, int iNote)
{
    int step = m_Chord[iChord].nNote[iNote].step();
    int octave = m_Chord[iChord].nNote[iNote].octave();
    int acc = m_Chord[iChord].nAcc[iNote];

    FPitch fp(step, octave, acc);
    return fp.to_rel_ldp_name(m_nKey);
}

//---------------------------------------------------------------------------------------
void Cadence::Debug_DumpAllChords(std::vector<lmHChord>& aChords)
{
    for (int i=0; i < (int)aChords.size(); i++) {
        Debug_DumpChord(aChords[i], i);
    }
}

//---------------------------------------------------------------------------------------
#if (TRACE_CADENCE == 1)
void Cadence::Debug_DumpChord(lmHChord& oChord, int iChord)
{
    if (oChord.nReason == lm_eChordValid)
    {
		wxLogMessage(_T("[Cadence::Debug_DumpChord] Valid chord %d : %s, %s, %s, %s"),
            iChord,
            oChord.nNote[0].get_english_note_name().c_str(),
            oChord.nNote[1].get_english_note_name().c_str(),

            oChord.nNote[2].get_english_note_name().c_str(),
            oChord.nNote[3].get_english_note_name().c_str() );
    }
    else
    {
		lmChordError nError = oChord.nSeverity;
        wxLogMessage(_T("[Cadence::Debug_DumpChord] Invalid chord %d : %s, %s, %s, %s - Impact %d, severity %d"),
            iChord,
            oChord.nNote[0].get_english_note_name().c_str(),
            oChord.nNote[1].get_english_note_name().c_str(),
            oChord.nNote[2].get_english_note_name().c_str(),
            oChord.nNote[3].get_english_note_name().c_str(),
            oChord.nImpact, nError );

		if (nError & lm_eNotDoubledThird)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eNotDoubledThird).c_str() );
		if (nError & lm_eNotContraryMotion)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eNotContraryMotion).c_str() );
		if (nError & lm_eGreaterThanSixth)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eGreaterThanSixth).c_str() );
		if (nError & lm_eChromaticAlter)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eChromaticAlter).c_str() );
		if (nError & lm_eVoiceOverlap)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eVoiceOverlap).c_str() );
		if (nError & lm_eVoiceCrossing)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eVoiceCrossing).c_str() );
		if (nError & lm_eGreaterThanOctave)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eGreaterThanOctave).c_str() );
		if (nError & lm_eSeventhResolution)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eSeventhResolution).c_str() );
		if (nError & lm_eLeadingResolution)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eLeadingResolution).c_str() );
		if (nError & lm_eLeadingToneDoubled)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eLeadingToneDoubled).c_str() );
		if (nError & lm_eFifthDoubled)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eFifthDoubled).c_str() );
		if (nError & lm_eResultantFifthOctves)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eResultantFifthOctves).c_str() );
		if (nError & lm_eFifthOctaveMotion)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eFifthOctaveMotion).c_str() );
		if (nError & lm_eFifthMissing)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eFifthMissing).c_str() );
		if (nError & lm_eNotAllNotes)
			wxLogMessage(_T("                                      %s"), GetChordErrorDescription(lm_eNotAllNotes).c_str() );
    }
}
#else
void Cadence::Debug_DumpChord(lmHChord& oChord, int iChord) {}
#endif

//---------------------------------------------------------------------------------------
Chord* Cadence::GetTonicChord()
{
    if (!m_pTonicChord)
    {
        //Create tonic chord

        //Get root note for this key signature and clef
        FPitch fpRootNote = GetRootNote(_T("I"), m_nKey, k_clef_G2, false);  //false = don't use Grand Staff
        wxString sIntervals = _T("");
        if (is_major_key(m_nKey))
            sIntervals = _T("M3,p5");
        else
            sIntervals = _T("m3,p5");

        //create the chord
        m_pTonicChord = LENMUS_NEW Chord(fpRootNote, sIntervals, m_nKey);
    }
    return m_pTonicChord;
}

//---------------------------------------------------------------------------------------
ECadenceType Cadence::CadenceNameToType(wxString sCadence)
{
    //AWARE: biyective to ECadenceType
    static const wxString sNames[] =
    {
        // Perfect authentic:
        _T("V_I_Perfect"), _T("V7_I"), _T("Va5_I"), _T("Vd5_I"),
        // Plagal:
        _T("IV_I"), _T("IVm_I"), _T("IIc6_I"), _T("IImc6_I"),
        // Imperfect authentic:
	    _T("V_I_Imperfect"),
        // Deceptive:
        _T("V_IV"), _T("V_IVm"), _T("V_VI"), _T("V_VIm"), _T("V_IIm"),
        _T("V_III"), _T("V_VII"),
        // Half cadences:
        _T("IImc6_V"), _T("IV_V"), _T("I_V"), _T("Ic64_V"), _T("IV6_V"),
        _T("II_V"), _T("IIdimc6_V"), _T("VdeVdim5c64_V"),
    };

    for (int i=0; i < k_cadence_max; i++)
    {
        if (sCadence == sNames[i])
            return (ECadenceType)i;
    }

    return (ECadenceType)-1;
}



//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

wxString CadenceTypeToName(ECadenceType nType)
{
    wxASSERT(nType <= k_cadence_max);

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized)
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

        m_fStringsInitialized = true;
    }

    return m_sCadenceName[nType];

}

//---------------------------------------------------------------------------------------
wxString GetChordErrorDescription(lmChordError nError)
{
    switch (nError) {
        case lm_eChordValid:
            return _T("Chord is valid");
        case lm_eChordDiscarded:
            return _T("Chord discarded: not possible to generate a valid next chord");
        case lm_eVoiceCrossing:
            return _T("Notes not in ascending sequence or duplicated");
        case lm_eGreaterThanOctave:
            return _T("Notes interval greater than one octave (other than bass-tenor)");
        case lm_eNotAllNotes:
            return _T("Not all chord steps in the chord");
        case lm_eFifthDoubled:
            return _T("The fifth is doubled");
        case lm_eLeadingToneDoubled:
            return _T("The leading tone is doubled");
        case lm_eFifthOctaveMotion:
            return _T("Parallel motion of perfect octaves or perfect fifths");
        case lm_eVoiceOverlap:
            return _T("Voice overlap");
        case lm_eNotContraryMotion:
            return _T("Bass moves by step and not all other voices moves in opposite direction to bass");
        case lm_eLeadingResolution:
            return _T("Scale degree seven (the leading tone) doesn't resolve to tonic.");
        case lm_eChromaticAlter:
            return _T("Chromatic alteration not resolved the same direction than the alteration.");

        default:
            return _T("Error: Invalid value");
    }
}


}   //namespace lenmus
