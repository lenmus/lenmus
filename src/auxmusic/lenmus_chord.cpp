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

#include "lenmus_chord.h"
#include "lenmus_standard_header.h"
#include "lenmus_string.h"

//lomse
#include <lomse_logger.h>
#include <lomse_interval.h>
using namespace lomse;

namespace lenmus
{

//---------------------------------------------------------------------------------------
// Chords data table
// A master table with basic information about chords
//---------------------------------------------------------------------------------------

//an entry of the master table
struct ChordData
{
    int       nNumNotes;
    FIntval   nIntervals[k_intervals_in_chord];   //from root note
};

#define lmNIL   lmNULL_FIntval

//The Master Table.
// AWARE: Array indexes are in correspondence with enum EChordType
// - intervals are from root note

static ChordData tChordData[ect_Max] =
{
    //Triads:
    { 3, { k_interval_M3, k_interval_p5 }},             //MT        - MajorTriad
    { 3, { k_interval_m3, k_interval_p5 }},             //mT        - MinorTriad
    { 3, { k_interval_M3, k_interval_a5 }},             //aT        - AugTriad
    { 3, { k_interval_m3, k_interval_d5 }},             //dT        - DimTriad
    //Suspended:
    { 3, { k_interval_p4, k_interval_p5 }},             //I,IV,V    - Suspended_4th
    { 3, { k_interval_M2, k_interval_p5 }},             //I,II,V    - Suspended_2nd
    //Sevenths:
    { 4, { k_interval_M3, k_interval_p5, k_interval_M7 }},      //MT + M7   - MajorSeventh
    { 4, { k_interval_M3, k_interval_p5, k_interval_m7 }},      //MT + m7   - DominantSeventh
    { 4, { k_interval_m3, k_interval_p5, k_interval_m7 }},      //mT + m7   - MinorSeventh
    { 4, { k_interval_m3, k_interval_d5, k_interval_d7 }},      //dT + d7   - DimSeventh
    { 4, { k_interval_m3, k_interval_d5, k_interval_m7 }},      //dT + m7   - HalfDimSeventh
    { 4, { k_interval_M3, k_interval_a5, k_interval_M7 }},      //aT + M7   - AugMajorSeventh
    { 4, { k_interval_M3, k_interval_a5, k_interval_m7 }},      //aT + m7   - AugSeventh
    { 4, { k_interval_m3, k_interval_p5, k_interval_M7 }},      //mT + M7   - MinorMajorSeventh
    //Sixths:
    { 4, { k_interval_M3, k_interval_p5, k_interval_M6 }},      //MT + M6   - MajorSixth
    { 4, { k_interval_m3, k_interval_p5, k_interval_M6 }},      //mT + M6   - MinorSixth
    { 4, { k_interval_M3, k_interval_a4, k_interval_a6 }},      //          - AugSixth
    //Ninths:
    { 5, { k_interval_M3, k_interval_p5, k_interval_m7, k_interval_M9 }},   // - DominantNinth  = dominant-seventh + major ninth
    { 5, { k_interval_M3, k_interval_p5, k_interval_M7, k_interval_M9 }},   // - MajorNinth     = major-seventh + major ninth
    { 5, { k_interval_m3, k_interval_p5, k_interval_m7, k_interval_M9 }},   // - MinorNinth     = minor-seventh + major ninth
    //11ths:
    { 6, { k_interval_M3, k_interval_p5, k_interval_m7, k_interval_M9, k_interval_p11 }},   // - Dominant_11th    = dominantNinth + perfect 11th
    { 6, { k_interval_M3, k_interval_p5, k_interval_M7, k_interval_M9, k_interval_p11 }},   // - Major_11th       = majorNinth + perfect 11th
    { 6, { k_interval_m3, k_interval_p5, k_interval_m7, k_interval_M9, k_interval_p11 }},   // - Minor_11th       = minorNinth + perfect 11th
    //13ths:
    { 7, { k_interval_M3, k_interval_p5, k_interval_m7, k_interval_M9, k_interval_p11, k_interval_M13 }}, // - Dominant_13th    = dominant_11th + major 13th
    { 7, { k_interval_M3, k_interval_p5, k_interval_M7, k_interval_M9, k_interval_p11, k_interval_M13 }}, // - Major_13th       = major_11th + major 13th
    { 7, { k_interval_m3, k_interval_p5, k_interval_m7, k_interval_M9, k_interval_p11, k_interval_M13 }}, // - Minor_13th       = minor_11th + major 13th
    //Other:
    //{ 2, { k_interval_p5 }},                    // - PowerChord     = perfect fifth, (octave)
    { 4, { k_interval_a2, k_interval_a4, k_interval_a6 }},      // - TristanChord   = augmented fourth, augmented sixth, augmented second
};

//Special chords table.
//These chords are normally built as specified in this table.
//
//static ChordData tSpecialChords[] =
//{
//    //Functional sixths:
//  //{ 3, { lm_m3, lm_p4 }},             // - NeapolitanSixth
//  //{ 3, { lm_a4, lm_m6 }},             // - ItalianSixth
//  //{ 4, { lm_M2, lm_a4, lm_m6 }},      // - FrenchSixth
//  //{ 4, { lm_m3, lm_a4, lm_m6 }},      // - GermanSixth
//}




//=======================================================================================
// ChordsDB implementation
// A singleton containing a data base with elaborated chords data and methods to get info.
//=======================================================================================

//an entry of the DB table
class ChordDBEntry
{
public:
    ChordDBEntry(EChordType type, int inversion, ChordIntervals* pChordIntv,
                   wxString fingerPrint)
        : nType(type)
        , nInversion(inversion)
        , oIntervals(*pChordIntv)
        , sFingerPrint(fingerPrint)
    {}

    void DumpChordsDBEntry()
    {
        wxLogMessage("%s - %d, %d, Int:'%s', Fingerprint='%s'",
                        Chord::type_to_name(nType).wx_str(),
                        nInversion,
                        oIntervals.GetNumIntervals(),
                        oIntervals.DumpIntervals().wx_str(),
                        sFingerPrint.wx_str() );
    }


    //accessors
    inline ChordIntervals* GetChordIntervals() { return &oIntervals; }
    inline int GetNumIntervals() { return oIntervals.GetNumIntervals(); }

    //class memeber variables. public access
    EChordType      nType;
    int             nInversion;
    ChordIntervals  oIntervals;   //from root note
    wxString        sFingerPrint;
};


//the only instance of ChordsDB class
ChordsDB* ChordsDB::m_pInstance = nullptr;

//---------------------------------------------------------------------------------------
ChordsDB::ChordsDB()
{
    BuildDatabase();
    #if (LENMUS_DEBUG_BUILD == 1)
        DumpChords();
    #endif
}

//---------------------------------------------------------------------------------------
ChordsDB::~ChordsDB()
{
    //delete the table

    std::vector<ChordDBEntry*>::iterator it;
    for (it=m_ChordsDB.begin(); it != m_ChordsDB.end(); ++it)
    {
        delete *it;
    }
    m_ChordsDB.clear();
}

//---------------------------------------------------------------------------------------
ChordsDB* ChordsDB::GetInstance()
{
    if (!m_pInstance)
        m_pInstance = LENMUS_NEW ChordsDB();
    return m_pInstance;
}

//---------------------------------------------------------------------------------------
void ChordsDB::DeleteInstance()
{
    delete m_pInstance;
    m_pInstance = (ChordsDB*)nullptr;
}

//---------------------------------------------------------------------------------------
void ChordsDB::BuildDatabase()
{
    for (int nType = 0; nType < ect_Max; nType++)
    {
        //get entry from master table
        int nNumNotes = tChordData[nType].nNumNotes;
        ChordIntervals oCI(nNumNotes-1, &tChordData[nType].nIntervals[0]);

        //create root position entry
        int nInversion = 0;
        ChordIntervals oNCI = oCI;
        oNCI.Normalize();

        ChordDBEntry* pEntry =
            LENMUS_NEW ChordDBEntry((EChordType)nType, nInversion, &oCI, oNCI.DumpIntervals());
        m_ChordsDB.push_back(pEntry);

        //create all inversions entries
        for (nInversion = 1; nInversion <= nNumNotes-1; nInversion++)
        {
            oCI.DoInversion();
            ChordIntervals oNCI = oCI;
            oNCI.Normalize();

            ChordDBEntry* pEntry =
                LENMUS_NEW ChordDBEntry((EChordType)nType, nInversion, &oCI, oNCI.DumpIntervals());
            m_ChordsDB.push_back(pEntry);
        }
    }

    //To speed up comparisons, sort chords database by number of intervals, type and inversion
    //Bubble sort
    bool fSwapDone = true;
    while (fSwapDone)
    {
        fSwapDone = false;
        for (int i = 0; i < (int)m_ChordsDB.size() - 1; i++)
        {
            if (m_ChordsDB[i]->GetNumIntervals() > m_ChordsDB[i+1]->GetNumIntervals()
                || (m_ChordsDB[i]->GetNumIntervals() == m_ChordsDB[i+1]->GetNumIntervals()
                    && m_ChordsDB[i]->nType > m_ChordsDB[i+1]->nType )
                || (m_ChordsDB[i]->GetNumIntervals() == m_ChordsDB[i+1]->GetNumIntervals()
                    && m_ChordsDB[i]->nType == m_ChordsDB[i+1]->nType
                    && m_ChordsDB[i]->nInversion > m_ChordsDB[i+1]->nInversion )
                )
            {
                std::swap(m_ChordsDB[i], m_ChordsDB[i+1]);
	            fSwapDone = true;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void ChordsDB::DumpChords()
{
//    wxLogMessage(_T("\n"));
//    wxLogMessage("Dump of chords DB table (%d entries)", m_ChordsDB.size());
//    wxLogMessage("===================================================================");
//    std::vector<ChordDBEntry*>::iterator it;
//    for (it=m_ChordsDB.begin(); it != m_ChordsDB.end(); ++it)
//    {
//        (*it)->DumpChordsDBEntry();
//    }
//
//    //Find duplicated fingerprints
//    wxLogMessage(_T("\n"));
//    wxLogMessage("Looking for duplicated fingerprints in chords DB table");
//    wxLogMessage("===================================================================");
//    int nDuplicates = 0;
//    std::vector<ChordDBEntry*>::iterator itStart, itCur;
//    for (itStart = m_ChordsDB.begin(); itStart != m_ChordsDB.end(); ++itStart)
//    {
//        itCur = itStart;
//        for(++itCur; itCur != m_ChordsDB.end(); ++itCur)
//        {
//            if ((*itCur)->sFingerPrint == (*itStart)->sFingerPrint)
//            {
//                wxLogMessage("ERROR: -------------------------- Duplicated fingerprints:");
//                (*itStart)->DumpChordsDBEntry();
//                (*itCur)->DumpChordsDBEntry();
//                ++nDuplicates;
//            }
//        }
//    }
//    wxLogMessage(_T("\n"));
//    if (nDuplicates == 0)
//        wxLogMessage("==> OK. No duplicated fingerprints found");
//    else
//        wxLogMessage("==> ERROR. %d duplicated fingerprints found.", nDuplicates);
//    wxLogMessage(_T("\n"));
//
}

//---------------------------------------------------------------------------------------
ChordDBEntry* ChordsDB::Find(ChordIntervals* pChordIntv)
{
    //TODO: As table is ordered, optimize search. Use binary search.

    ChordIntervals oCI = *pChordIntv;
    oCI.Normalize();
    wxString sFingerprint = oCI.DumpIntervals();
    std::vector<ChordDBEntry*>::iterator it;
    for (it = m_ChordsDB.begin(); it != m_ChordsDB.end(); ++it)
    {
        if ((*it)->sFingerPrint == sFingerprint)
            return *it;
    }

    #if (LENMUS_DEBUG_BUILD == 1)
    wxString sIntvals = "No match found. Intervals: ";
    sIntvals += pChordIntv->DumpIntervals();
    sIntvals += " fingerprint=";
    sIntvals += sFingerprint;
    LOMSE_LOG_ERROR( to_std_string(sIntvals) );
    #endif

    return (ChordDBEntry*)nullptr;
}


//=======================================================================================
// Chord class implementation
//=======================================================================================
Chord::Chord(FPitch fpRootNote, EChordType nChordType, int nInversion,
             EKeySignature nKey)
    : ChordIntervals(nChordType, nInversion)
    , m_nType(nChordType)
    , m_nKey(nKey)
    , m_nInversion(nInversion)
    , m_fpRootNote(fpRootNote)
{
}

//---------------------------------------------------------------------------------------
Chord::Chord(FPitch fpRootNote, wxString sIntervals, EKeySignature nKey)
    : ChordIntervals(sIntervals)
    , m_nType(ect_undefined)
    , m_nKey(nKey)
    , m_nInversion(0)
    , m_fpRootNote(fpRootNote)
{
    // If first interval is "#" or "b" it refers to the root note
    int iEnd = sIntervals.find(',', 0);
    wxString sIntval = sIntervals.substr(0, iEnd);
    if (sIntval == "#" || sIntval == "b")
    {
        int step = m_fpRootNote.step();
        int octave = m_fpRootNote.octave();
        int acc = m_fpRootNote.num_accidentals();
        if (sIntval == "#")
            m_fpRootNote = FPitch(step, octave, acc+1);
        else if (sIntval == "b")
            m_fpRootNote = FPitch(step, octave, acc-1);
    }

    //determine chord type and inversion type
    ComputeTypeAndInversion();
}

//TODO 5.0
////---------------------------------------------------------------------------------------
//Chord::Chord(FPitch fpRootNote, lmFiguredBass* pFigBass, EKeySignature nKey)
//    : ChordIntervals(0, (FIntval*)nullptr)
//    , m_nKey(nKey)
//    , m_nInversion(0)
//    , m_nType(ect_undefined)
//    , m_fpRootNote(fpRootNote)
//{
//    //Creates a chord from the root note, the figured bass, and the key signature.
//
//    // Create root note and get its components
//    int nRootStep = m_fpRootNote.step();
//    int nRootOctave = m_fpRootNote.octave();
//    int nRootAcc = m_fpRootNote.num_accidentals();
//    m_nNumIntv = 0;
//    //wxLogMessage("Root note=%d", m_fpRootNote);
//
//    //get accidentals for desired key signature
//    int nAccidentals[7];
//    ::lmComputeAccidentals(nKey, nAccidentals);
            //#include <lomse_score_utilities.h>
            //get_accidentals_for_key(nKey, nAccidentals);
//
//    //build the intervals from the root note
//    //TODO: Review for intervals equal or greater than one octave
//    int nOctave = nRootOctave;
//    int nStep = nRootStep;
//    int nFirstIntval = 0;
//    for (int i=2; i <= lmFB_MAX_INTV; i++)
//    {
//        //determine step and octave
//        if (++nStep == 7)
//        {
//            nStep = 0;
//            nOctave++;
//        }
//
//        //decide accidentals
//        FPitch fpNote;
//        int nAcc = nAccidentals[nStep];     //accidentals from key signature
//
//        //compute pitch and add note to chord
//        if (pFigBass->IntervalSounds(i))
//        {
//            lmEIntervalQuality nIntvQuality = pFigBass->GetQuality(i);
//            switch(nIntvQuality)
//            {
//                case lm_eIM_NotPresent:
//                {   //the interval is as implied by key signature
//                    break;
//                }
//                case lm_eIM_AsImplied:
//                {   //the interval is as implied by key signature
//                    break;
//                }
//                case lm_eIM_RaiseHalf:
//                {   //the interval is raised by half step, relative to the key signature
//                    nAcc++;
//                    break;
//                }
//                case lm_eIM_LowerHalf:
//                {   //the interval is lowered by half step, relative to the key signature
//                    nAcc--;
//                    break;
//                }
//                case lm_eIM_Natural:
//                {   //the interval is natural, regardless of the key signature
//                    nAcc = 0;
//                    break;
//                }
//                case lm_eIM_Diminished:
//                {   //the interval is diminished, regardless of the key signature
//                    FIntval fi = FIntval_FromType(i, eti_Diminished);
//                    fpNote = m_fpRootNote + fi;
//                }
//                default:
//                    wxASSERT(false);
//            }
//
//            //build pitch. It is already built for case lm_eIM_Diminished
//            if (nIntvQuality != lm_eIM_Diminished)
//                fpNote = FPitch(nStep, nOctave, nAcc);
//
//            //add this interval to the chord
//            if (m_nNumIntv < k_intervals_in_chord)
//            {
//                m_nIntervals[m_nNumIntv++] = fpNote - m_fpRootNote;
//                //wxLogMessage("Added note=%d", fpNote);
//            }
//            else
//                wxLogMessage("[Chord::Chord] Number of notes in a chord exceeded!");
//
//            //determine the first present interval greater than second
//            if (nFirstIntval == 0 && i >= 3)
//                nFirstIntval = i;
//        }
//    }
//
//        //here all chord note are created. Compute chord additional info
//
//    //determine chord type and inversion type
//    ComputeTypeAndInversion();
//}

//---------------------------------------------------------------------------------------
Chord::Chord(int numNotes, string notes[], EKeySignature nKey)
    : ChordIntervals(numNotes, notes)
    , m_nType(ect_undefined)
    , m_nKey(nKey)
    , m_nInversion(0)
{
    //Creates a chord from a list of notes in LDP source code

    //get root note
    m_fpRootNote = FPitch(notes[0]);

    //determine chord type and inversion type
    ComputeTypeAndInversion();
}

////---------------------------------------------------------------------------------------
//// Contructor to create a chord from the essential chord information
//// Arguments:
////   chord degree = step of root note. Values k_step_C .. k_step_B
////                    (todo: consider to make an enum type for steps)
////   key signature
////   number of intervals ( = number of notes -1)
////   number of inversions
////   octave
////        TODO: aware: the octave is NOT required; only the chord degree (step of root note) is necessary!!!
////              consider to make octave an OPTIONAL argument
//Chord::Chord(int nStep, EKeySignature nKey, int nIntervals, int nInversion, int octave)
//    : ChordIntervals(nStep, nKey, nIntervals, nInversion)
//    , m_nKey(nKey)
//    , m_nInversion(nInversion)
//    , m_nType(ect_undefined)
//{
//    m_fpRootNote = FPitchK(nStep, octave, nKey); // only to debug
//
//    // aware: for calculating the root note when there are inversions...
//
//    int nnStep = nStep+(2*nInversion); // aware: can be in a higher octave
//    int nIncreaseOctave = nnStep / (k_step_B+1); // aware: after B (6): higher octave
//    nnStep = nnStep % (k_step_B+1); // 0 .. 6
//
//    m_fpRootNote = FPitchK(nnStep, octave+nIncreaseOctave, nKey);
//
//    ComputeTypeAndInversion();
//}
//
////---------------------------------------------------------------------------------------
//// Creates a chord from a list of score notes
//Chord::Chord(int nNumNotes, ImoNote** pNotes, EKeySignature nKey)
//    : ChordIntervals(nNumNotes, pNotes)
//    , m_nKey(nKey)
//    , m_fpRootNote ( pNotes[0]->GetFPitch() )
//    , m_nInversion(0)
//    , m_nType(ect_undefined)
//{
//    ComputeTypeAndInversion();
//}
//
////---------------------------------------------------------------------------------------
//// Creates a chord from a list of score notes
//Chord::Chord(int nNumNotes, FPitch fNotes[], EKeySignature nKey)
//    : ChordIntervals(nNumNotes, fNotes, 0)
//    , m_nKey(nKey)
//    , m_nInversion(0)
//    , m_fpRootNote ( fNotes[0] )
//    , m_nType(ect_undefined)
//{
//    ComputeTypeAndInversion();
//}

//---------------------------------------------------------------------------------------
Chord::~Chord()
{
}

//---------------------------------------------------------------------------------------
EChordType Chord::get_chord_type()
{
    if (m_nType != ect_undefined)
        return m_nType;

    //determine chord type and inversion type
    ComputeTypeAndInversion();
    return m_nType;
}

//---------------------------------------------------------------------------------------
int Chord::GetInversion()
{
    if (m_nType != ect_undefined)
        return m_nInversion;

    //determine chord type and inversion type
    ComputeTypeAndInversion();
    return m_nInversion;
}

//---------------------------------------------------------------------------------------
FIntval Chord::GetInterval(int i)
{
    //return the chord interval #i (i = 1 .. m_nNumNotes-1)

// Carlos sep 09: warning: ChordIntervals::GetInterval starts from 0

// Carlos sep09: todo: confirm this change    wxASSERT(i > 0 && i < m_nNumIntv+1);
//       Interval 0 should be allowed. It should be interpreted as "root note duplicated in chord".
//       Then, for interval 0, just return "unison"
    wxASSERT(i >= 0 && i < m_nNumIntv+1);
    if (i == 0)
        return k_interval_p1; // unison
    else
        return m_nIntervals[i-1];
}

//---------------------------------------------------------------------------------------
FPitch Chord::get_note(int i)
{
    //i = 0..nNumNotes-1
    if (i >= get_num_notes())
    {
        wxLogMessage(" Chord error, requesting note %d, max: %d", i, get_num_notes());
        return 0;  //TODO: error protection added by Carlos. Improve it?
    }

    if (i == 0)
        return m_fpRootNote;
    else
        return m_fpRootNote + GetInterval(i);
}

//---------------------------------------------------------------------------------------
MidiPitch Chord::get_midi_note(int i)
{
    wxASSERT(i < get_num_notes());
    return get_note(i).to_midi_pitch();
}

//---------------------------------------------------------------------------------------
string Chord::GetPattern(int i)
{
    // Returns Relative LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < get_num_notes());
    return get_note(i).to_rel_ldp_name(m_nKey);
}

//---------------------------------------------------------------------------------------
string Chord::note_steps_to_string()
{
    static string steps("cdefgab");

    stringstream ss;
    for (int i=0; i < get_num_notes(); ++i)
    {
        if (i > 0)
            ss << ",";
        ss << steps.at(get_note(i).step());
    }
    return ss.str();
}

//---------------------------------------------------------------------------------------
wxString Chord::get_name_and_inversion()
{
    EChordType nType = get_chord_type();

    wxString sName;

    if ( nType != ect_invalid )
    {
        sName = Chord::type_to_name( nType );
        sName += ", ";
        if (m_nInversion == 0)
            sName += _("root position");
        else if (m_nInversion == 1)
            sName += _("1st inversion");
        else if (m_nInversion == 2)
            sName += _("2nd inversion");
        else if (m_nInversion == 3)
            sName += _("3rd inversion");
        else if (m_nInversion == 4)
            sName += _("4th inversion");
        else if (m_nInversion == 5)
            sName += _("5th inversion");
        else
            sName += wxString::Format("%d inversion", m_nInversion);
    }
    else
        sName = _("unknown chord");

    return sName;
}

//---------------------------------------------------------------------------------------
// A note is valid in a chord if it can be derived from de root note plus any of the intervals, i.e:
//    note + any interval + N octaves
int Chord::IsValidChordNote(FPitch fNote)
{
    FPitch fpNormalizedRoot = this->GetNormalizedBass();

    for (int nI=0; nI <= m_nNumIntv; nI++) // note that valid intervals are: 0 .. m_nNumIntv
    {
        FPitch fpNormalizedNoteDistance = (  fNote - GetInterval(nI)) % k_interval_p8;
        if ( fpNormalizedRoot == fpNormalizedNoteDistance)
        {
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------
// key independent root note, calculated from bass and inversions
FPitch Chord::GetNormalizedRoot()
{
    if (m_nType == ect_invalid)
        return 0;
    int nNumInversions = GetInversion();
    int nNumNotes = get_num_notes();
    int nIntervalToApplyToTheBass = (nNumNotes - nNumInversions) % nNumNotes;
    FIntval fpIntv = GetInterval(nIntervalToApplyToTheBass);
    FPitch fpBass = GetNormalizedBass();
    FPitch fpRootNote = (fpBass + fpIntv) %  k_interval_p8;
    return fpRootNote;
}

//---------------------------------------------------------------------------------------
StepType Chord::GetChordDegree() // Chord degree == root step
{
    if (m_nType == ect_invalid)
        return 0;
    return GetNormalizedRoot().step();
}

//---------------------------------------------------------------------------------------
void Chord::ComputeTypeAndInversion()
{
    //look for the entry in in the Chords DB that matches this chord intervals.

    ChordDBEntry* pEntry = ChordsDB::GetInstance()->Find((ChordIntervals*)this);
    if (pEntry)
    {
        m_nType = pEntry->nType;
        m_nInversion = pEntry->nInversion;
    }
    else
    {
        m_nType = ect_invalid;    //no match found!
        m_nInversion = 0;
    }
}

////TODO 5.0
////---------------------------------------------------------------------------------------
//wxString Chord::ToString()
//{
//    wxString sRetStr;
//    if ( ! this->IsStandardChord() )
//        sRetStr = _("Not recognized");
//    else
//    {
//        int nNumNotes = get_num_notes();
//        // Note that the number of notes and the number of inversions is already in the description from get_name_and_inversion
//        sRetStr = wxString::Format(" %s", get_name_and_inversion().wx_str());
//
//        sRetStr += wxString::Format(", Bass:%s"
//                , NormalizedFPitch_ToAbsLDPName(this->GetNormalizedBass()).wx_str());
//
//        if ( m_nInversion > 0)
//        {
//            // aware: if no inversions then root == bass
//            sRetStr += wxString::Format(", Root:%s"
//                , NormalizedFPitch_ToAbsLDPName(this->GetNormalizedRoot()).wx_str());
//        }
//
//        if (m_nElision > 0)
//          sRetStr += wxString::Format(", %d elisions", m_nElision);
//
//        sRetStr += wxString::Format(",");
//        sRetStr += this->ChordIntervals::ToString().wx_str();
//
//        sRetStr += " Pattern:";
//
//        for (int n=0; n<=m_nNumIntv; n++)
//        {
//            sRetStr += " ";
//            sRetStr += GetPattern(n);
//        }
//    }
//    return sRetStr;
//}

//---------------------------------------------------------------------------------------
// TODO: confirm this
// Remember: chord fundamental information is
//   bass note, octave independent
//   intervals
bool Chord::IsEqualTo(Chord* tOther)
{
    if ( ! this->ChordIntervals::IsEqualTo((ChordIntervals*)tOther))
        return false;

    if (this->GetNormalizedBass() != tOther->GetNormalizedBass() )
        return false;

    return true;
}

//---------------------------------------------------------------------------------------
wxString Chord::type_to_name(EChordType nType)
{
    static wxString name[ect_Max];
    static bool fNameInitialized = false;

    if (nType >= ect_Max)
        return _("Not identified");

    //AWARE: language dependent strings. Can not be statically initiallized because
    //       then they do not get translated

    if (!fNameInitialized)
    {
        // Triads
        //name[ect_MajorTriad] = wxCONTEXT("Chord name", "Major triad");
        name[ect_MajorTriad] = _("Major triad");
        name[ect_MinorTriad] = _("Minor triad");
        name[ect_AugTriad] = _("Augmented triad");
        name[ect_DimTriad] = _("Diminished triad");
        name[ect_Suspended_4th] = _("Suspended triad (4th)");
        name[ect_Suspended_2nd] = _("Suspended triad (2nd)");

        // Seventh chords
        name[ect_MajorSeventh] = _("Major 7th");
        name[ect_DominantSeventh] = _("Dominant 7th");
        name[ect_MinorSeventh] = _("Minor 7th");
        name[ect_DimSeventh] = _("Diminished 7th");
        name[ect_HalfDimSeventh] = _("Half diminished 7th");
        name[ect_AugMajorSeventh] = _("Augmented major 7th");
        name[ect_AugSeventh] = _("Augmented 7th");
        name[ect_MinorMajorSeventh] = _("Minor major 7th");

        // Sixth chords
        name[ect_MajorSixth] = _("Major 6th");
        name[ect_MinorSixth] = _("Minor 6th");
        name[ect_AugSixth] = _("Augmented 6th");

        //Ninths
        name[ect_DominantNinth] = _("Dominant ninth");
        name[ect_MajorNinth] = _("Major ninth");
        name[ect_MinorNinth] = _("Minor ninth");

        //11ths
        name[ect_Dominant_11th] = _("Dominant 11th");
        name[ect_Major_11th] = _("Major 11th");
        name[ect_Minor_11th] = _("Minor 11th");

        //13ths
        name[ect_Dominant_13th] = _("Dominant 13th");
        name[ect_Major_13th] = _("Major 13th");
        name[ect_Minor_13th] = _("Minor 13th");

        //Other
        //name[ect_PowerChord] = _("Power chord");
        name[ect_TristanChord] = _("Tristan chord");

        fNameInitialized = true;
    }

    return name[nType];
}

//---------------------------------------------------------------------------------------
int Chord::num_notes(EChordType nChordType)
{
    wxASSERT(nChordType > ect_undefined && nChordType < ect_Max);
    return tChordData[nChordType].nNumNotes;
}

//---------------------------------------------------------------------------------------
EChordType Chord::short_name_to_type(wxString sName)
{
    // returns -1 if error
    //
    // m-minor, M-major, a-augmented, d-diminished, s-suspended
    // T-triad, dom-dominant, hd-half diminished
    //
    // triads: mT, MT, aT, dT, s4, s2
    // sevenths: m7, M7, a7, d7, mM7, aM7 dom7, hd7
    // sixths: m6, M6, a6

            // Triads
    if      (sName == "MT") return ect_MajorTriad;
    else if (sName == "mT") return ect_MinorTriad;
    else if (sName == "aT") return ect_AugTriad;
    else if (sName == "dT") return ect_DimTriad;
    else if (sName == "s4") return ect_Suspended_4th;
    else if (sName == "s2") return ect_Suspended_2nd;

        // Seventh chords
    else if (sName == "M7") return ect_MajorSeventh;
    else if (sName == "dom7") return ect_DominantSeventh;
    else if (sName == "m7") return ect_MinorSeventh;
    else if (sName == "d7") return ect_DimSeventh;
    else if (sName == "hd7") return ect_HalfDimSeventh;
    else if (sName == "aM7") return ect_AugMajorSeventh;
    else if (sName == "a7") return ect_AugSeventh;
    else if (sName == "mM7") return ect_MinorMajorSeventh;

        // Sixth chords
    else if (sName == "M6") return ect_MajorSixth;
    else if (sName == "m6") return ect_MinorSixth;
    else if (sName == "a6") return ect_AugSixth;

    return (EChordType)-1;  //error
}

//=======================================================================================
// ChordIntervals implementation: A list of intervals
//=======================================================================================
ChordIntervals::ChordIntervals(EChordType nType, int nInversion)
{
    m_nNumIntv = tChordData[nType].nNumNotes - 1;
    for (int i=0; i < m_nNumIntv; i++)
        m_nIntervals[i] = tChordData[nType].nIntervals[i];

    //apply inversions
    for (int i=0; i < nInversion; i++)
        DoInversion();
}

//---------------------------------------------------------------------------------------
ChordIntervals::ChordIntervals(int nNumIntv, FIntval* pFI)
    : m_nNumIntv(nNumIntv)
{
    for (int i=0; i < m_nNumIntv; i++)
        m_nIntervals[i] = *(pFI+i);
}

//---------------------------------------------------------------------------------------
ChordIntervals::ChordIntervals(wxString sIntervals)
{
    // extract intervals
    m_nNumIntv = 0;

    int nSize = (int)sIntervals.length();
    int iStart = 0;
    int iEnd = sIntervals.find(',', iStart);
    while (iEnd != wxNOT_FOUND && m_nNumIntv < k_intervals_in_chord)
    {
        wxString sIntval = sIntervals.substr(iStart, iEnd-iStart);
        // If first interval is "#" or "b" it refers to the root note
        // Ignore it
        if (iStart==0 && (sIntval == "#" || sIntval == "b"))
            ;   //ignore the interval
        else
        {
            // convert interval name to value
            m_nIntervals[m_nNumIntv++] = FIntval(to_std_string(sIntval));
        }

        // advance pointers
        iStart = iEnd + 1;
        if (iStart >= nSize) break;
        iEnd = sIntervals.find(',', iStart);
    }

    //add last interval
    if (m_nNumIntv < k_intervals_in_chord)
    {
        wxString sIntval = sIntervals.substr(iStart);
        m_nIntervals[m_nNumIntv++] = FIntval( to_std_string(sIntval) );
    }
}

////TODO 5.0
////---------------------------------------------------------------------------------------
//ChordIntervals::ChordIntervals(int nNumNotes, ImoNote** pNotes)
//{
//    // AWARE: NOTES MUST BE ALREADY ORDERED INCREMENTALLY
//
//    //Creates the intervals from a list of score notes
//    if (nNumNotes > 0)
//        m_nNumIntv = nNumNotes - 1;
//    else
//    {
//        m_nNumIntv = 0;
//        return;
//    }
//
//    if ( pNotes[0] == nullptr)
//    {
//        wxLogMessage(" ChordIntervals ERROR: note %d is nullptr", 0);
//        return;
//    }
//
//
//    //get intervals
//    for (int i=0; i < m_nNumIntv; i++)
//    {
//        if ( pNotes[i+1] == nullptr)
//        {
//            wxLogMessage(" ChordIntervals ERROR: note %d is nullptr", i+1);
//            return;
//        }
//
//        m_nIntervals[i] = (pNotes[i+1]->GetFPitch() - pNotes[0]->GetFPitch()) % k_interval_p8;
//    }
//
//    this->Normalize(); // normalize to 1 octave range, remove duplicated and sort.
//}
//
////---------------------------------------------------------------------------------------
//ChordIntervals::ChordIntervals(int nNumNotes, FPitch fNotes[], int nUseless)
//{
//    //Creates the intervals from a list of FPitch notes
//    if (nNumNotes > 0)
//        m_nNumIntv = nNumNotes - 1;
//    else
//    {
//        m_nNumIntv = 0;
//        return;
//    }
//
//    //get intervals
//    for (int i=0; i < m_nNumIntv; i++)
//    {
//        m_nIntervals[i] = fNotes[i+1] - fNotes[0];
//    }
//
//    this->Normalize(); // normalize to 1 octave range, remove duplicated and sort.
//}

//---------------------------------------------------------------------------------------
ChordIntervals::ChordIntervals(int numNotes, string notes[])
{
    //Creates the intervals from a list of notes in LDP source code
    if (numNotes > 0)
        m_nNumIntv = numNotes - 1;
    else
    {
        m_nNumIntv = 0;
        return;
    }
    FPitch fpRootNote(notes[0]);

    //get intervals
    for (int i=0; i < m_nNumIntv; i++)
    {
        m_nIntervals[i] = FPitch(notes[i+1]) - fpRootNote;
    }
}

////---------------------------------------------------------------------------------------
//ChordIntervals::ChordIntervals(int nRootStep, EKeySignature nKey, int nNumIntervals, int nInversion)
//{
//    m_nNumIntv = nNumIntervals;
//
//    //get intervals
//    for (int i=0; i < m_nNumIntv; i++)
//    {
//        m_nIntervals[i] = FPitchInterval(nRootStep, nKey, i+1);
//    }
//
//    FIntval nI;
//    for (int i=0; i < m_nNumIntv; i++)
//    {
//        nI = m_nIntervals[i];
//    }
//
//    //apply inversions
//    for (int i=0; i < nInversion; i++)
//        DoInversion();
//
//    for (int i=0; i < m_nNumIntv; i++)
//    {
//        nI = m_nIntervals[i];
//    }
//}

//---------------------------------------------------------------------------------------
ChordIntervals::~ChordIntervals()
{
}

//---------------------------------------------------------------------------------------
void ChordIntervals::DoInversion()
{
    //Do first inversion of list of intervals.
    //Note that second inversion can be obtained by invoking this method
    //two times. The third inversion, by invoking it three times, etc.

    FIntval newIntvals[k_intervals_in_chord];
    for (int i=0; i < m_nNumIntv-1; i++)
    {
        newIntvals[i] = m_nIntervals[i+1] - m_nIntervals[0];
        if ((int)newIntvals[i] < 0)
            newIntvals[i] += k_interval_p8;
    }

    newIntvals[m_nNumIntv-1] = k_interval_p8 - m_nIntervals[0];

    //transfer results
    for (int i=0; i < m_nNumIntv; i++)
        m_nIntervals[i] = newIntvals[i];

}

//---------------------------------------------------------------------------------------
void ChordIntervals::SortIntervals()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

    //sort intervals, from shortest to largest. Bubble method

    bool fSwapDone = true;
    while (fSwapDone)
    {
        fSwapDone = false;
        for (int i = 0; i < m_nNumIntv - 1; i++)
        {
            if (m_nIntervals[i] > m_nIntervals[i+1])
            {
	            FIntval fiAux = m_nIntervals[i];
	            m_nIntervals[i] = m_nIntervals[i+1];
	            m_nIntervals[i+1] = fiAux;
	            fSwapDone = true;
            }
        }
    }
#pragma GCC diagnostic pop
}

//---------------------------------------------------------------------------------------
void ChordIntervals::Normalize()
{
    //reduce any interval grater than the octave and unisons, and remove duplicated.
    //sort intervals

    if (m_nNumIntv < 1) // if no interval to normailze return
        return; // aware: no only to save time; the while below fails if 0 intervals

    //reduce any interval greater than the octave
    for (int i=0; i < m_nNumIntv; i++)
    {
        while (m_nIntervals[i] >= k_interval_p8)
            m_nIntervals[i] -= k_interval_p8;
    }

    SortIntervals();

    //remove duplicated and unisons
    int iCur = 1;
    int iLast = 0;
    while (iCur < m_nNumIntv)
    {
        if (m_nIntervals[iLast] != m_nIntervals[iCur])
        {
            if (m_nIntervals[iLast] != k_interval_p1)
                ++iLast;
            m_nIntervals[iLast] = m_nIntervals[iCur];
        }

        iCur++;
    }
    m_nNumIntv = iLast + 1;
}

//---------------------------------------------------------------------------------------
wxString ChordIntervals::DumpIntervals()
{
    wxString sIntvals = "";
    if (m_nNumIntv < 1)
        return sIntvals;
    for (int i=0; i < m_nNumIntv-1; i++)
    {
        sIntvals += wxString::Format("(int=%d)", int(m_nIntervals[i]));
        sIntvals += m_nIntervals[i].get_code();
        sIntvals += ",";
    }
    sIntvals += m_nIntervals[m_nNumIntv-1].get_code();
    return sIntvals;
}

//---------------------------------------------------------------------------------------
string ChordIntervals::intervals_to_string()
{
    stringstream ss;
    for (int i=0; i < m_nNumIntv; i++)
    {
        if (i > 0)
            ss << ",";
        ss << to_std_string( m_nIntervals[i].get_code() );
    }
    return ss.str();
}

//---------------------------------------------------------------------------------------
wxString ChordIntervals::ToString()
{
    wxString sIntvals = " Intervals:";
    for (int i=0; i < m_nNumIntv; i++)
    {
        sIntvals += wxString::Format("%s(%d) "
                        , to_wx_string(m_nIntervals[i].get_code()).wx_str()
                        , int(m_nIntervals[i])
                    );
    }
    return sIntvals;
}

// TODO: confirm this
//---------------------------------------------------------------------------------------
bool ChordIntervals::IsEqualTo(ChordIntervals* tOther)
{
    if ( this->GetNumIntervals() != tOther->GetNumIntervals() )
        return false;

    for (int i=0; i < m_nNumIntv; i++)
    {
        if (this->GetInterval(i) != tOther->GetInterval(i))
            return false;
    }
    return true;
}



////TODO 5.0
//#if (LENMUS_DEBUG_BUILD == 1)
////---------------------------------------------------------------------------------------
//// Debug global functions
////---------------------------------------------------------------------------------------
//
//bool lmChordUnitTests()
//{
//    //returns true if all tests passed correctly
//
//    bool fTestOK = true;
//    bool fOK;
//
//    //Chord contructor from lmFiguredBass
//    fOK = lmChordFromFiguredBassUnitTest();
//    fTestOK &= fOK;
//
//    //TODO: Add other tests
//
//    return fTestOK;
//}
//
////---------------------------------------------------------------------------------------
//bool lmChordFromFiguredBassUnitTest()
//{
//    //Unit test for Chord contructor from lmFiguredBass
//    //returns true if all tests passed correctly
//
//    typedef struct lmTestDataStruct
//    {
//        wxString            sFigBass;   //figured bass string
//        EKeySignature    nKey;       //key signature
//        wxString            sRootNote;  //root note
//        EChordType        nChordType; //test result, to validate test
//        int                 nInversion; //test result, to validate test
//        wxString            sIntervals; //test result, to validate test
//    }
//    lmTestData;
//
//    static lmTestData tTestData[] =
//    {
//        //Minor scale
//        //fig
//        //bass         key      root note  chord type        inversion     Intvals
//        { "#",     k_key_a, "a3",  ect_MajorTriad,      0, "M3,p5" },
//        { "b",     k_key_a, "a3",  ect_Max,             0, "d3,p5" },
//        { "=",     k_key_a, "a3",  ect_MinorTriad,      0, "m3,p5" },
//        { "2",     k_key_a, "a3",  ect_HalfDimSeventh,  3, "M2,p4,m6" },
//        { "#2",    k_key_a, "a3",  ect_Max,             0, "a2,p4,m6" },
//        { "b2",    k_key_a, "a3",  ect_MajorSeventh,    3, "m2,p4,m6" },
//        { "=2",    k_key_a, "a3",  ect_HalfDimSeventh,  3, "M2,p4,m6" },
//        { "2+",    k_key_a, "a3",  ect_Max,             0, "a2,p4,m6" },
//        { "2 3",   k_key_a, "a3",  ect_Max,             0, "M2,m3" },
//        { "3",     k_key_a, "a3",  ect_MinorTriad,      0, "m3,p5" },
//        { "4",     k_key_a, "a3",  ect_Suspended_4th,   0, "p4,p5" },
//        { "4 2",   k_key_a, "a3",  ect_HalfDimSeventh,  3, "M2,p4,m6" },
//        { "4+ 2",  k_key_a, "a3",  ect_AugSixth,        1, "M2,a4,m6" },
//        { "4 3",   k_key_a, "a3",  ect_MinorSeventh,    2, "m3,p4,m6" },
//        { "5",     k_key_a, "a3",  ect_MinorTriad,      0, "m3,p5" },
//        { "5 #",   k_key_a, "a3",  ect_MajorTriad,      0, "M3,p5" },
//        { "5 b",   k_key_a, "a3",  ect_Max,             0, "d3,p5" },
//        { "5+",    k_key_a, "a3",  ect_Max,             0, "m3,a5" },
//        { "5/",    k_key_a, "a3",  ect_Max,             0, "m3,a5" },
//        { "5 3",   k_key_a, "a3",  ect_MinorTriad,      0, "m3,p5" },
//        { "5 4",   k_key_a, "a3",  ect_Suspended_4th,   0, "p4,p5" },
//        { "6",     k_key_a, "a3",  ect_MajorTriad,      1, "m3,m6" },
//        { "6 #",   k_key_a, "a3",  ect_AugTriad,        1, "M3,m6" },
//        { "6 b",   k_key_a, "a3",  ect_Max,             0, "d3,m6" },
//        { "6\\",   k_key_a, "a3",  ect_Max,             0, "m3,d6" },
//        { "6 3",   k_key_a, "a3",  ect_MajorTriad,      1, "m3,m6" },
//        { "6 #3",  k_key_a, "a3",  ect_AugTriad,        1, "M3,m6" },
//        { "6 b3",  k_key_a, "a3",  ect_Max,             0, "d3,m6" },
//        { "6 4",   k_key_a, "a3",  ect_MinorTriad,      2, "p4,m6" },
//        { "6 4 2", k_key_a, "a3",  ect_HalfDimSeventh,  3, "M2,p4,m6" },
//        { "6 4 3", k_key_a, "a3",  ect_MinorSeventh,    2, "m3,p4,m6" },
//        { "6 5",   k_key_a, "a3",  ect_MajorSeventh,    1, "m3,p5,m6" },
//        { "6 5 3", k_key_a, "a3",  ect_MajorSeventh,    1, "m3,p5,m6" },
//        { "7",     k_key_a, "a3",  ect_MinorSeventh,    0, "m3,p5,m7" },
//        { "7 4 2", k_key_a, "a3",  ect_Max,             0, "M2,p4,m7" },
//        { "8",     k_key_a, "a3",  ect_Max,             0, "p8" },
//        { "9",     k_key_a, "a3",  ect_Max,             0, "m3,p5,M9" },
//        { "10",    k_key_a, "a3",  ect_Max,             0, "m3,p5,m10" },
//
//        // Mayor scale
//        { "#",     k_key_C, "c4", ect_Max,              0, "a3,p5" },
//        { "b",     k_key_C, "c4", ect_MinorTriad,       0, "m3,p5" },
//        { "=",     k_key_C, "c4", ect_MajorTriad,       0, "M3,p5" },
//        { "2",     k_key_C, "c4", ect_MinorSeventh,     3, "M2,p4,M6" },
//        { "#2",    k_key_C, "c4", ect_Max,              0, "a2,p4,M6" },
//        { "b2",    k_key_C, "c4", ect_AugMajorSeventh,  3, "m2,p4,M6" },
//        { "=2",    k_key_C, "c4", ect_MinorSeventh,     3, "M2,p4,M6" },
//        { "2+",    k_key_C, "c4", ect_Max,              0, "a2,p4,M6" },
//        { "2 3",   k_key_C, "c4", ect_Max,              0, "M2,M3" },
//        { "3",     k_key_C, "c4", ect_MajorTriad,       0, "M3,p5" },
//        { "4",     k_key_C, "c4", ect_Suspended_4th,    0, "p4,p5" },
//        { "4 2",   k_key_C, "c4", ect_MinorSeventh,     3, "M2,p4,M6" },
//        { "4+ 2",  k_key_C, "c4", ect_DominantSeventh,  3, "M2,a4,M6" },
//        { "4 3",   k_key_C, "c4", ect_MajorSeventh,     2, "M3,p4,M6" },
//        { "5",     k_key_C, "c4", ect_MajorTriad,       0, "M3,p5" },
//        { "5 #",   k_key_C, "c4", ect_Max,              0, "a3,p5" },
//        { "5 b",   k_key_C, "c4", ect_MinorTriad,       0, "m3,p5" },
//        { "5+",    k_key_C, "c4", ect_AugTriad,         0, "M3,a5" },
//        { "5/",    k_key_C, "c4", ect_AugTriad,         0, "M3,a5" },
//        { "5 3",   k_key_C, "c4", ect_MajorTriad,       0, "M3,p5" },
//        { "5 4",   k_key_C, "c4", ect_Suspended_4th,    0, "p4,p5" },
//        { "6",     k_key_C, "c4", ect_MinorTriad,       1, "M3,M6" },
//        { "6 #",   k_key_C, "c4", ect_Max,              0, "a3,M6" },
//        { "6 b",   k_key_C, "c4", ect_DimTriad,         1, "m3,M6" },
//        { "6\\",   k_key_C, "c4", ect_AugTriad,         1, "M3,m6" },
//        { "6 3",   k_key_C, "c4", ect_MinorTriad,       1, "M3,M6" },
//        { "6 #3",  k_key_C, "c4", ect_Max,              0, "a3,M6" },
//        { "6 b3",  k_key_C, "c4", ect_DimTriad,         1, "m3,M6" },
//        { "6 4",   k_key_C, "c4", ect_MajorTriad,       2, "p4,M6" },
//        { "6 4 2", k_key_C, "c4", ect_MinorSeventh,     3, "M2,p4,M6" },
//        { "6 4 3", k_key_C, "c4", ect_MajorSeventh,     2, "M3,p4,M6" },
//        { "6 5",   k_key_C, "c4", ect_MinorSeventh,     1, "M3,p5,M6" },
//        { "6 5 3", k_key_C, "c4", ect_MinorSeventh,     1, "M3,p5,M6" },
//        { "7",     k_key_C, "c4", ect_MajorSeventh,     0, "M3,p5,M7" },
//        { "7 4 2", k_key_C, "c4", ect_Max,              0, "M2,p4,M7" },
//        { "8",     k_key_C, "c4", ect_Max,              0, "p8" },
//        { "9",     k_key_C, "c4", ect_Max,              0, "M3,p5,M9" },
//        { "10",    k_key_C, "c4", ect_Max,              0, "M3,p5,M10" },
//    };
//
//    int nNumTestCases = sizeof(tTestData) / sizeof(lmTestData);
//
//    //TODO: Check that following cases are only possible in major scales
//    //    ect_AugMajorSeventh
//    //    ect_DominantSeventh
//    //    ect_DimTriad
//
//
//    wxLogMessage("UnitTests: Chord contructor from lmFiguredBass");
//    wxLogMessage("====================================================");
//
//    lmLDPParser parserLDP;
//    bool fTestOK = true;
//    for (int i=0; i < nNumTestCases; i++)
//    {
//        ImoScore* pScore = new_score();
//        lmInstrument* pInstr = pScore->AddInstrument(0,0, "");
//        lmVStaff* pVStaff = pInstr->GetVStaff();
//        pVStaff->AddClef( k_clef_G2 );
//        pVStaff->AddKeySignature( tTestData[i].nKey );
//        pVStaff->AddTimeSignature(4 ,4, k_no_visible );
//        wxString sPattern = "(figuredBass \"" + tTestData[i].sFigBass + "\")";
//        lmLDPNode* pNode = parserLDP.ParseText( sPattern );
//        lmFiguredBass* pFB = parserLDP.AnalyzeFiguredBass(pNode, pVStaff);
//
//        Chord oChord(tTestData[i].sRootNote, pFB, tTestData[i].nKey);
//        bool fOK = (oChord.DumpIntervals() == tTestData[i].sIntervals)
//                   && (tTestData[i].nChordType != ect_Max);
//        fTestOK &= fOK;
//        if (!fOK)
//        {
//            wxString sMsg = wxString::Format("figured bass ='%s', chord type=%d (%s), inversion=%d, Intvals: ",
//                tTestData[i].sFigBass.wx_str(),
//                oChord.get_chord_type(),
//                Chord::type_to_name(oChord.get_chord_type()).wx_str(),
//                oChord.GetInversion() );
//            sMsg += oChord.DumpIntervals();
//            wxLogMessage(sMsg);
//        }
//        delete pScore;
//    }
//
//    return fTestOK;
//}
//
//
//#endif      //Debug global methods


}   //namespace lenmus
