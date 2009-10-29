//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Chord.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <algorithm>

#include "Chord.h"
#include "Conversion.h"
#include "../ldp_parser/AuxString.h"
#include "../exercises/Generators.h"
#include "../score/KeySignature.h"
#include "../score/FiguredBass.h"
#include "Harmony.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

#ifdef __WXDEBUG__          //for UnitTests
#include "../ldp_parser/LDPParser.h"
#include "../score/Score.h"
#include "../score/VStaff.h"
#endif

static wxString m_sChordName[ect_Max];
static bool m_fStringsInitialized = false;

#define lmEMPTY_CHORD_TYPE    ((lmEChordType)-1)

//----------------------------------------------------------------------------------
// Chords data table
// A master table with basic information about chords
//----------------------------------------------------------------------------------

//an entry of the master table
typedef struct lmChordDataStruct
{
    int         nNumNotes;
    lmFIntval   nIntervals[lmINTERVALS_IN_CHORD];   //from root note
}
lmChordData;

#define lmNIL   lmNULL_FIntval

//The Master Table.
// AWARE: Array indexes are in correspondence with enum lmEChordType
// - intervals are from root note

static lmChordData tChordData[ect_Max] =
{
    //Triads:
    { 3, { lm_M3, lm_p5 }},             //MT        - MajorTriad
    { 3, { lm_m3, lm_p5 }},             //mT        - MinorTriad
    { 3, { lm_M3, lm_a5 }},             //aT        - AugTriad
    { 3, { lm_m3, lm_d5 }},             //dT        - DimTriad
    //Suspended:
    { 3, { lm_p4, lm_p5 }},             //I,IV,V    - Suspended_4th
    { 3, { lm_M2, lm_p5 }},             //I,II,V    - Suspended_2nd
    //Sevenths:
    { 4, { lm_M3, lm_p5, lm_M7 }},      //MT + M7   - MajorSeventh
    { 4, { lm_M3, lm_p5, lm_m7 }},      //MT + m7   - DominantSeventh
    { 4, { lm_m3, lm_p5, lm_m7 }},      //mT + m7   - MinorSeventh
    { 4, { lm_m3, lm_d5, lm_d7 }},      //dT + d7   - DimSeventh
    { 4, { lm_m3, lm_d5, lm_m7 }},      //dT + m7   - HalfDimSeventh
    { 4, { lm_M3, lm_a5, lm_M7 }},      //aT + M7   - AugMajorSeventh
    { 4, { lm_M3, lm_a5, lm_m7 }},      //aT + m7   - AugSeventh
    { 4, { lm_m3, lm_p5, lm_M7 }},      //mT + M7   - MinorMajorSeventh
    //Sixths:
    { 4, { lm_M3, lm_p5, lm_M6 }},      //MT + M6   - MajorSixth
    { 4, { lm_m3, lm_p5, lm_M6 }},      //mT + M6   - MinorSixth
    { 4, { lm_M3, lm_a4, lm_a6 }},      //          - AugSixth
    //Ninths:
    { 5, { lm_M3, lm_p5, lm_m7, lm_M9 }},   // - DominantNinth  = dominant-seventh + major ninth
    { 5, { lm_M3, lm_p5, lm_M7, lm_M9 }},   // - MajorNinth     = major-seventh + major ninth
    { 5, { lm_m3, lm_p5, lm_m7, lm_M9 }},   // - MinorNinth     = minor-seventh + major ninth
    //11ths:
    { 6, { lm_M3, lm_p5, lm_m7, lm_M9, lm_p11 }},   // - Dominant_11th    = dominantNinth + perfect 11th
    { 6, { lm_M3, lm_p5, lm_M7, lm_M9, lm_p11 }},   // - Major_11th       = majorNinth + perfect 11th
    { 6, { lm_m3, lm_p5, lm_m7, lm_M9, lm_p11 }},   // - Minor_11th       = minorNinth + perfect 11th
    //13ths:
    { 7, { lm_M3, lm_p5, lm_m7, lm_M9, lm_p11, lm_M13 }}, // - Dominant_13th    = dominant_11th + major 13th
    { 7, { lm_M3, lm_p5, lm_M7, lm_M9, lm_p11, lm_M13 }}, // - Major_13th       = major_11th + major 13th
    { 7, { lm_m3, lm_p5, lm_m7, lm_M9, lm_p11, lm_M13 }}, // - Minor_13th       = minor_11th + major 13th
    //Other:
    //{ 2, { lm_p5 }},                    // - PowerChord     = perfect fifth, (octave)
    { 4, { lm_a2, lm_a4, lm_a6 }},      // - TristanChord   = augmented fourth, augmented sixth, augmented second
};

//Special chords table.
//These chords are normally built as specified in this table.
//
//static lmChordData tSpecialChords[] =
//{
//    //Functional sixths:
//  //{ 3, { lm_m3, lm_p4 }},             // - NeapolitanSixth
//  //{ 3, { lm_a4, lm_m6 }},             // - ItalianSixth
//  //{ 4, { lm_M2, lm_a4, lm_m6 }},      // - FrenchSixth
//  //{ 4, { lm_m3, lm_a4, lm_m6 }},      // - GermanSixth
//}





//-------------------------------------------------------------------------------------------
// lmChordsDB implementation
// A singleton containing a data base with elaborated chords data and methods to get info.
//-------------------------------------------------------------------------------------------

//an entry of the DB table
class lmChordDBEntry
{
public:
    lmChordDBEntry(lmEChordType type, int inversion, lmChordIntervals* pChordIntv,
                   wxString fingerPrint)
        : nType(type)
        , nInversion(inversion)
        , oIntervals(*pChordIntv)
        , sFingerPrint(fingerPrint)
    {}

    void DumpChordsDBEntry()
    {
        wxLogMessage(_T("%s - %d, %d, Int:'%s', Fingerprint='%s'"),
                        lmChordTypeToName(nType),
                        nInversion,
                        oIntervals.GetNumIntervals(),
                        oIntervals.DumpIntervals().c_str(),
                        sFingerPrint.c_str() );
    }


    //accessors
    inline lmChordIntervals* GetChordIntervals() { return &oIntervals; }
    inline int GetNumIntervals() { return oIntervals.GetNumIntervals(); }

    //class memeber variables. public access
    lmEChordType        nType;
    int                 nInversion;
    lmChordIntervals    oIntervals;   //from root note
    wxString            sFingerPrint;
};


//the only instance of lmChordsDB class
lmChordsDB* lmChordsDB::m_pInstance = (lmChordsDB*)NULL;

lmChordsDB::lmChordsDB()
{
    BuildDatabase();
    #ifdef __WXDEBUG__
        DumpChords();
    #endif
}

lmChordsDB::~lmChordsDB()
{
    //delete the table

    std::vector<lmChordDBEntry*>::iterator it;
    for (it=m_ChordsDB.begin(); it != m_ChordsDB.end(); ++it)
    {
        delete *it;
    }
    m_ChordsDB.clear();
}

lmChordsDB* lmChordsDB::GetInstance()
{
    if (!m_pInstance)
        m_pInstance = new lmChordsDB();
    return m_pInstance;
}

void lmChordsDB::DeleteInstance()
{
    if (m_pInstance)
        delete m_pInstance;
    m_pInstance = (lmChordsDB*)NULL;
}

void lmChordsDB::BuildDatabase()
{
    for (int nType = 0; nType < ect_Max; nType++)
    {
        //get entry from master table
        int nNumNotes = tChordData[nType].nNumNotes;
        lmChordIntervals oCI(nNumNotes-1, &tChordData[nType].nIntervals[0]);

        //create root position entry
        int nInversion = 0;
        lmChordIntervals oNCI = oCI;
        oNCI.Normalize();

        lmChordDBEntry* pEntry =
            new lmChordDBEntry((lmEChordType)nType, nInversion, &oCI, oNCI.DumpIntervals());
        m_ChordsDB.push_back(pEntry);

        //create all inversions entries
        for (nInversion = 1; nInversion <= nNumNotes-1; nInversion++)
        {
            oCI.DoInversion();
            lmChordIntervals oNCI = oCI;
            oNCI.Normalize();

            lmChordDBEntry* pEntry =
                new lmChordDBEntry((lmEChordType)nType, nInversion, &oCI, oNCI.DumpIntervals());
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

void lmChordsDB::DumpChords()
{
    wxLogMessage(_T("\n"));
    wxLogMessage(_T("Dump of chords DB table (%d entries)"), m_ChordsDB.size());
    wxLogMessage(_T("==================================================================="));
    std::vector<lmChordDBEntry*>::iterator it;
    for (it=m_ChordsDB.begin(); it != m_ChordsDB.end(); ++it)
    {
        (*it)->DumpChordsDBEntry();
    }

    //Find duplicated fingerprints
    wxLogMessage(_T("\n"));
    wxLogMessage(_T("Looking for duplicated fingerprints in chords DB table"));
    wxLogMessage(_T("==================================================================="));
    int nDuplicates = 0;
    std::vector<lmChordDBEntry*>::iterator itStart, itCur;
    for (itStart = m_ChordsDB.begin(); itStart != m_ChordsDB.end(); ++itStart)
    {
        itCur = itStart;
        for(++itCur; itCur != m_ChordsDB.end(); ++itCur)
        {
            if ((*itCur)->sFingerPrint == (*itStart)->sFingerPrint)
            {
                wxLogMessage(_T("ERROR: -------------------------- Duplicated fingerprints:"));
                (*itStart)->DumpChordsDBEntry();
                (*itCur)->DumpChordsDBEntry();
                ++nDuplicates;
            }
        }
    }
    wxLogMessage(_T("\n"));
    if (nDuplicates == 0)
        wxLogMessage(_T("==> OK. No duplicated fingerprints found"));
    else
        wxLogMessage(_T("==> ERROR. %d duplicated fingerprints found."), nDuplicates);
    wxLogMessage(_T("\n"));

}

lmChordDBEntry* lmChordsDB::Find(lmChordIntervals* pChordIntv)
{
    //TODO: As table is ordered, optimize search. Use binary search.

    lmChordIntervals oCI = *pChordIntv;
    oCI.Normalize();
    wxString sFingerprint = oCI.DumpIntervals();
    std::vector<lmChordDBEntry*>::iterator it;
    for (it = m_ChordsDB.begin(); it != m_ChordsDB.end(); ++it)
    {
        if ((*it)->sFingerPrint == sFingerprint)
            return *it;
    }

    #ifdef __WXDEBUG__
    wxString sIntvals = _T("[lmChordsDB::Find] No match found. Intervals: ");
    sIntvals += pChordIntv->DumpIntervals();
    sIntvals += _T(" fingerprint=");
    sIntvals += sFingerprint;
    wxLogMessage(sIntvals);
    #endif

    return (lmChordDBEntry*)NULL;
}


//-------------------------------------------------------------------------------------
// lmChord class implementation
//-------------------------------------------------------------------------------------

lmChord::lmChord(wxString sRootNote, lmEChordType nChordType, int nInversion,
                 lmEKeySignatures nKey)
    : lmChordIntervals(nChordType, nInversion)
    , m_nType(nChordType)
    , m_nKey(nKey)
    , m_nInversion(nInversion)
    , m_fpRootNote( FPitch(sRootNote) )
{
    //creates a chord from its type, the root note, the desired inversion, and the key signature.
    //Parameter 'nInversion' values: 0 (root position), 1 (1st inversion), 2 (2nd inversion),
    //and so on
}

lmChord::lmChord(wxString sRootNote, lmFiguredBass* pFigBass, lmEKeySignatures nKey)
    : lmChordIntervals(0, (lmFIntval*)NULL)
    , m_nKey(nKey)
    , m_nInversion(0)
    , m_nType(lmEMPTY_CHORD_TYPE)
{
    //Creates a chord from the root note, the figured bass, and the key signature.

    // Create root note and get its components
    m_fpRootNote = FPitch(sRootNote);
    int nRootStep = FPitch_Step( m_fpRootNote );
    int nRootOctave = FPitch_Octave( m_fpRootNote );
    int nRootAcc = FPitch_Accidentals( m_fpRootNote );
    m_nNumIntv = 0;
    //wxLogMessage(_T("Root note=%d"), m_fpRootNote);

    //get accidentals for desired key signature
    int nAccidentals[7];
    ::lmComputeAccidentals(nKey, nAccidentals);

    //build the intervals from the root note
    //TODO: Review for intervals equal or greater than one octave
    int nOctave = nRootOctave;
    int nStep = nRootStep;
    int nFirstIntval = 0;
    for (int i=2; i <= lmFB_MAX_INTV; i++)
    {
        //determine step and octave
        if (++nStep == 7)
        {
            nStep = 0;
            nOctave++;
        }

        //decide accidentals
        lmFPitch fpNote;
        int nAcc = nAccidentals[nStep];     //accidentals from key signature

        //compute pitch and add note to chord
        if (pFigBass->IntervalSounds(i))
        {
            lmEIntervalQuality nIntvQuality = pFigBass->GetQuality(i);
            switch(nIntvQuality)
            {
                case lm_eIM_NotPresent:
                {   //the interval is as implied by key signature
                    break;
                }
                case lm_eIM_AsImplied:
                {   //the interval is as implied by key signature
                    break;
                }
                case lm_eIM_RaiseHalf:
                {   //the interval is raised by half step, relative to the key signature
                    nAcc++;
                    break;
                }
                case lm_eIM_LowerHalf:
                {   //the interval is lowered by half step, relative to the key signature
                    nAcc--;
                    break;
                }
                case lm_eIM_Natural:
                {   //the interval is natural, regardless of the key signature
                    nAcc = 0;
                    break;
                }
                case lm_eIM_Diminished:
                {   //the interval is diminished, regardless of the key signature
                    lmFIntval fi = FIntval_FromType(i, eti_Diminished);
                    fpNote = m_fpRootNote + fi;
                }
                default:
                    wxASSERT(false);
            }

            //build pitch. It is already built for case lm_eIM_Diminished
            if (nIntvQuality != lm_eIM_Diminished)
                fpNote = FPitch(nStep, nOctave, nAcc);

            //add this interval to the chord
            if (m_nNumIntv < lmINTERVALS_IN_CHORD)
            {
                m_nIntervals[m_nNumIntv++] = fpNote - m_fpRootNote;
                //wxLogMessage(_T("Added note=%d"), fpNote);
            }
            else
                wxLogMessage(_T("[lmChord::lmChord] Number of notes in a chord exceeded!"));

            //determine the first present interval greater than second
            if (nFirstIntval == 0 && i >= 3)
                nFirstIntval = i;
        }
    }

        //here all chord note are created. Compute chord additional info

    //determine chord type and inversion type
    ComputeTypeAndInversion();
}

lmChord::lmChord(int nNumNotes, wxString* pNotes, lmEKeySignatures nKey)
    : lmChordIntervals(nNumNotes, pNotes)
    , m_nKey(nKey)
    , m_nInversion(0)
    , m_nType(lmEMPTY_CHORD_TYPE)
{
    //Creates a chord from a list of notes in LDP source code

    //get root note
    m_fpRootNote = ::lmLDPDataToFPitch( *pNotes );

    //determine chord type and inversion type
    ComputeTypeAndInversion();
}

lmChord::lmChord(wxString sRootNote, wxString sIntervals, lmEKeySignatures nKey)
    : lmChordIntervals(sIntervals)
    , m_nKey(nKey)
    , m_nInversion(0)
    , m_nType(lmEMPTY_CHORD_TYPE)
{
    // prepare root note
    m_fpRootNote = FPitch(sRootNote);

    // If first interval is "#" or "b" it refers to the root note
    int iEnd = sIntervals.find(_T(','), 0);
    wxString sIntval = sIntervals.substr(0, iEnd);
    if (sIntval == _T("#") || sIntval == _T("b"))
    {
        // modify root pitch
        if (sIntval == _T("#"))
            m_fpRootNote++;
        else
            m_fpRootNote--;
    }

    //determine chord type and inversion type
    ComputeTypeAndInversion();
}

// Contructor to create a chord from the essential chord information
// Arguments:
//   chord degree = step of root note. Values lmSTEP_C .. lmSTEP_B
//                    (todo: consider to make an enum type for steps)
//   key signature
//   number of intervals ( = number of notes -1)
//   number of inversions
//   octave
//        TODO: aware: the octave is NOT required; only the chord degree (step of root note) is necessary!!!
//              consider to make octave an OPTIONAL argument
lmChord::lmChord(int nStep, lmEKeySignatures nKey, int nIntervals, int nInversion, int octave)
    : lmChordIntervals(nStep, nKey, nIntervals, nInversion)
    , m_nKey(nKey)
    , m_nInversion(nInversion)
    , m_nType(lmEMPTY_CHORD_TYPE)
{
    m_fpRootNote = FPitchK(nStep, octave, nKey); // only to debug

    // aware: for calculating the root note when there are inversions...

    int nnStep = nStep+(2*nInversion); // aware: can be in a higher octave
    int nIncreaseOctave = nnStep / (lmSTEP_B+1); // aware: after B (6): higher octave
    nnStep = nnStep % (lmSTEP_B+1); // 0 .. 6

    m_fpRootNote = FPitchK(nnStep, octave+nIncreaseOctave, nKey);

    ComputeTypeAndInversion();
}

// Creates a chord from a list of score notes
lmChord::lmChord(int nNumNotes, lmNote** pNotes, lmEKeySignatures nKey)
    : lmChordIntervals(nNumNotes, pNotes)
    , m_nKey(nKey)
    , m_fpRootNote ( pNotes[0]->GetFPitch() )
    , m_nInversion(0)
    , m_nType(lmEMPTY_CHORD_TYPE)
{
    ComputeTypeAndInversion();
}

// Creates a chord from a list of score notes
lmChord::lmChord(int nNumNotes, lmFPitch fNotes[], lmEKeySignatures nKey)
    : lmChordIntervals(nNumNotes, fNotes)
    , m_nKey(nKey)
    , m_nInversion(0)
    , m_fpRootNote ( fNotes[0] )
    , m_nType(lmEMPTY_CHORD_TYPE)
{
    ComputeTypeAndInversion();
}


lmChord::~lmChord()
{
}

lmEChordType lmChord::GetChordType()
{
    if (m_nType != lmEMPTY_CHORD_TYPE)
        return m_nType;

    //determine chord type and inversion type
    ComputeTypeAndInversion();
    return m_nType;
}

int lmChord::GetInversion()
{
    if (m_nType != lmEMPTY_CHORD_TYPE)
        return m_nInversion;

    //determine chord type and inversion type
    ComputeTypeAndInversion();
    return m_nInversion;
}

lmFIntval lmChord::GetInterval(int i)
{
    //return the chord interval #i (i = 1 .. m_nNumNotes-1)

// Carlos sep 09: warning: lmChordIntervals::GetInterval starts from 0

// Carlos sep09: todo: confirm this change    wxASSERT(i > 0 && i < m_nNumIntv+1);
//       Interval 0 should be allowed. It should be interpreted as "root note duplicated in chord".
//       Then, for interval 0, just return "unison"
    wxASSERT(i >= 0 && i < m_nNumIntv+1);
    if (i == 0)
        return lm_p1; // unison
    else
        return m_nIntervals[i-1];
}

lmFPitch lmChord::GetNote(int i)
{
    //i = 0..nNumNotes-1
    if (i >= GetNumNotes())
    {
        wxLogMessage(_T(" Chord error, requesting note %d, max: %d"), i, GetNumNotes());
        return 0;  //TODO: error protection added by Carlos. Improve it?
    }

    return m_fpRootNote + (i==0 ? 0 : GetInterval(i));
}

lmMPitch lmChord::GetMidiNote(int i)
{
    wxASSERT(i < GetNumNotes());
    return FPitch_ToMPitch(GetNote(i));
}

wxString lmChord::GetPattern(int i)
{
    // Returns Relative LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < GetNumNotes());
    return FPitch_ToRelLDPName(GetNote(i), m_nKey);
}

wxString lmChord::GetNameFull()
{
    lmEChordType nType = GetChordType();

    wxString sName;

    if ( nType != lmINVALID_CHORD_TYPE )
    {
        sName = lmChordTypeToName( nType );
        sName += _T(", ");
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
            sName += wxString::Format(_T("%d inversion"), m_nInversion);
    }
    else
        sName = _("unknown chord");

    return sName;
}

// A note is valid in a chord if it can be derived from de root note plus any of the intervals, i.e:
//    note + any interval + N octaves
int lmChord::IsValidChordNote(lmFPitch fNote)
{
    lmFPitch fpNormalizedRoot = this->GetNormalizedBass();

    for (int nI=0; nI <= m_nNumIntv; nI++) // note that valid intervals are: 0 .. m_nNumIntv
    {
        lmFPitch fpNormalizedNoteDistance = (  fNote - GetInterval(nI)) % lm_p8;
        if ( fpNormalizedRoot == fpNormalizedNoteDistance)
        {
            return true;
        }
    }
    return false;
}


// key independent root note, calculated from bass and inversions
lmFPitch lmChord::GetNormalizedRoot()
{
    if (m_nType == lmINVALID_CHORD_TYPE)
        return 0;
    int nNumInversions = GetInversion();
    int nNumNotes = GetNumNotes();
    int nIntervalToApplyToTheBass = (nNumNotes - nNumInversions) % nNumNotes;
    lmFPitch fpIntv = GetInterval(nIntervalToApplyToTheBass);
    lmFPitch fpBass = GetNormalizedBass();
    lmFPitch fpRootNote = (fpBass + fpIntv) %  lm_p8;
    return fpRootNote;
}

lmStepType lmChord::GetChordDegree() // Chord degree == root step
{
    if (m_nType == lmINVALID_CHORD_TYPE)
        return 0;
    return FPitch_Step(GetNormalizedRoot());
}

void lmChord::ComputeTypeAndInversion()
{
    //look for the entry in in the Chords DB that matches this chord intervals.

    lmChordDBEntry* pEntry = lmChordsDB::GetInstance()->Find((lmChordIntervals*)this);
    if (pEntry)
    {
        m_nType = pEntry->nType;
        m_nInversion = pEntry->nInversion;
    }
    else
    {
        m_nType = lmINVALID_CHORD_TYPE;    //no match found!
        m_nInversion = 0;
    }
}

wxString lmChord::ToString()
{
    wxString sRetStr;
    if ( ! this->IsStandardChord() )
        sRetStr = _("Not recognized");
    else
    {
        int nNumNotes = GetNumNotes();
        // Note that the number of notes and the number of inversions is already in the description from GetNameFull
        sRetStr = wxString::Format(_T(" %s"), GetNameFull().c_str());

        sRetStr += wxString::Format(_(", Bass:%s")
            , NormalizedFPitch_ToAbsLDPName(this->GetNormalizedBass()).c_str());

        sRetStr += wxString::Format(_(", Root:%s")
            , NormalizedFPitch_ToAbsLDPName(this->GetNormalizedRoot()).c_str());

        if (m_nElision > 0)
          sRetStr += wxString::Format(_(", %d elisions"), m_nElision);

        sRetStr += wxString::Format(_T(","));
        sRetStr += this->lmChordIntervals::ToString().c_str();

        sRetStr += _(" Pattern:");

        for (int n=0; n<=m_nNumIntv; n++)
        {
            sRetStr += _T(" ");
            sRetStr += GetPattern(n);
        }
    }
    return sRetStr;
}

// TODO: confirm this
// Remember: chord fundamental information is
//   bass note, octave independent
//   intervals
bool lmChord::IsEqualTo(lmChord* tOther)
{
    if ( ! this->lmChordIntervals::IsEqualTo((lmChordIntervals*)tOther))
        return false;

    if (this->GetNormalizedBass() != tOther->GetNormalizedBass() )
        return false;

    return true;
}


#if 0
void UnitTests()
{
    //lmConverter::NoteToBits and lmConverter::NoteBitsToName
    wxLogMessage(_T("[lmChord::UnitTests] Test of lmConverter::NoteToBits() method:"));

    wxString sNote[8] = { _T("a4"), _T("+a5"), _T("--b2"), _T("-a4"),
        _T("+e4"), _T("++f6"), _T("b1"), _T("xc4") };
    lmNoteBits tNote;
    for(int i=0; i < 8; i++) {
        if (lmConverter::NoteToBits(sNote[i], &tNote))
            wxLogMessage(_T("Unexpected error in lmConverter::NoteToBits()"));
        else {
            wxLogMessage(_T("Note: '%s'. Bits: Step=%d, Octave=%d, Accidentals=%d, StepSemitones=%d --> '%s'"),
                sNote[i].c_str(), tNote.nStep, tNote.nOctave, tNote.nAccidentals, tNote.nStepSemitones,
                lmConverter::NoteBitsToName(tNote, m_nKey).c_str() );
        }
    }

    //ComputeInterval(): interval computation
    wxString sIntv[8] = { _T("M3"), _T("m3"), _T("p8"), _T("p5"),
        _T("a5"), _T("d7"), _T("M6"), _T("M2") };
    for(int i=0; i < 8; i++) {
        for (int j=0; j < 8; j++) {
            wxString sNewNote = ComputeInterval(sNote[i], sIntv[j], true, m_nKey);
            wxLogMessage(_T("Note='%s' + Intv='%s' --> '%s'"),
                         sNote[i].c_str(), sIntv[j].c_str(), sNewNote.c_str() );
            wxString sStartNote = ComputeInterval(sNewNote, sIntv[j], false, m_nKey);
            wxLogMessage(_T("Note='%s' - Intv='%s' --> '%s'"),
                         sNewNote.c_str(), sIntv[j].c_str(), sStartNote.c_str() );
        }
    }

    //IntervalCodeToBits and IntervalBitsToCode
    wxLogMessage(_T("[lmChord::UnitTests] Test of IntervalCodeToBits() method:"));
    lmIntvBits tIntv;
    for(int i=0; i < 8; i++) {
        if (IntervalCodeToBits(sIntv[i], &tIntv))
            wxLogMessage(_T("Unexpected error in IntervalCodeToBits()"));
        else {
            wxLogMessage(_T("Intv: '%s'. Bits: num=%d, Semitones=%d --> '%s'"),
                sIntv[i].c_str(), tIntv.nNum,tIntv.nSemitones,
                IntervalBitsToCode(tIntv).c_str() );
        }
    }

    ////SubstractIntervals
    //wxLogMessage(_T("[lmChord::UnitTests] Test of SubstractIntervals() method:"));
    //wxString sIntv1[8] = { _T("p5"), _T("p5"), _T("M7"), _T("M6"), _T("m6"), _T("M7"), _T("M6"), _T("p4") };
    //wxString sIntv2[8] = { _T("M3"), _T("m3"), _T("p5"), _T("p5"), _T("a5"), _T("M3"), _T("m3"), _T("M2") };
    //for(i=0; i < 8; i++) {
    //    wxLogMessage(_T("Intv1='%s', intv2='%s' --> dif='%s'"),
    //        sIntv1[i], sIntv2[i], SubstractIntervals(sIntv1[i], sIntv2[i]) );
    //}

    ////AddIntervals
    //wxLogMessage(_T("[lmChord::UnitTests] Test of AddIntervals() method:"));
    //wxString sIntv1[8] = { _T("p5"), _T("p5"), _T("M6"), _T("M3"), _T("M3"), _T("M6"), _T("d4"), _T("p8") };
    //wxString sIntv2[8] = { _T("M3"), _T("m3"), _T("m2"), _T("m3"), _T("M3"), _T("M3"), _T("m7"), _T("p8") };
    //for(i=0; i < 8; i++) {
    //    wxLogMessage(_T("Intv1='%s', intv2='%s' --> sum='%s'"),
    //        sIntv1[i].c_str(), sIntv2[i].c_str(), AddIntervals(sIntv1[i], sIntv2[i]).c_str() );
    //}

}

#endif



//----------------------------------------------------------------------------------------
// lmChordIntervals implementation: A list of intervals
//----------------------------------------------------------------------------------------

lmChordIntervals::lmChordIntervals(int nNumIntv, lmFIntval* pFI)
    : m_nNumIntv(nNumIntv)
{
    for (int i=0; i < m_nNumIntv; i++)
        m_nIntervals[i] = *(pFI+i);
}

lmChordIntervals::lmChordIntervals(lmEChordType nType, int nInversion)
{
    m_nNumIntv = tChordData[nType].nNumNotes - 1;
    for (int i=0; i < m_nNumIntv; i++)
        m_nIntervals[i] = tChordData[nType].nIntervals[i];

    //apply inversions
    for (int i=0; i < nInversion; i++)
        DoInversion();
}

lmChordIntervals::lmChordIntervals(wxString sIntervals)
{
    // extract intervals
    m_nNumIntv = 0;

    int nSize = (int)sIntervals.length();
    int iStart = 0;
    int iEnd = sIntervals.find(_T(','), iStart);
    while (iEnd != (int)wxStringBase::npos && m_nNumIntv < lmINTERVALS_IN_CHORD)
    {
        wxString sIntval = sIntervals.substr(iStart, iEnd-iStart);
        // If first interval is "#" or "b" it refers to the root note
        // Ignore it
        if (iStart==0 && (sIntval == _T("#") || sIntval == _T("b")))
            ;   //ignore the interval
        else
        {
            // convert interval name to value
            m_nIntervals[m_nNumIntv++] = FIntval(sIntval);
        }

        // advance pointers
        iStart = iEnd + 1;
        if (iStart >= nSize) break;
        iEnd = sIntervals.find(_T(','), iStart);
    }

    //add last interval
    if (m_nNumIntv < lmINTERVALS_IN_CHORD)
    {
        wxString sIntval = sIntervals.substr(iStart);
        m_nIntervals[m_nNumIntv++] = FIntval(sIntval);
    }
}

lmChordIntervals::lmChordIntervals(int nNumNotes, lmNote** pNotes)
{
    // AWARE: NOTES MUST BE ALREADY ORDERED INCREMENTALLY

    //Creates the intervals from a list of score notes
    if (nNumNotes > 0)
        m_nNumIntv = nNumNotes - 1;
    else
    {
        m_nNumIntv = 0;
        return;
    }

    if ( pNotes[0] == NULL)
    {
        wxLogMessage(_T(" lmChordIntervals ERROR: note %d is NULL"), 0);
        return;
    }


    //get intervals
    for (int i=0; i < m_nNumIntv; i++)
    {
        if ( pNotes[i+1] == NULL)
        {
            wxLogMessage(_T(" lmChordIntervals ERROR: note %d is NULL"), i+1);
            return;
        }

        m_nIntervals[i] = (pNotes[i+1]->GetFPitch() - pNotes[0]->GetFPitch()) % lm_p8;
    }

    this->Normalize(); // normalize to 1 octave range, remove duplicated and sort.
}

lmChordIntervals::lmChordIntervals(int nNumNotes, lmFPitch fNotes[], int nUseless)
{
    //Creates the intervals from a list of lmFPitch notes
    if (nNumNotes > 0)
        m_nNumIntv = nNumNotes - 1;
    else
    {
        m_nNumIntv = 0;
        return;
    }

    //get intervals
    for (int i=0; i < m_nNumIntv; i++)
    {
        m_nIntervals[i] = fNotes[i+1] - fNotes[0];
    }

    this->Normalize(); // normalize to 1 octave range, remove duplicated and sort.
}

lmChordIntervals::lmChordIntervals(int nNumNotes, wxString* pNotes)
{
    //Creates the intervals from a list of notes in LDP source code
    if (nNumNotes > 0)
        m_nNumIntv = nNumNotes - 1;
    else
    {
        m_nNumIntv = 0;
        return;
    }
    lmFPitch fpRootNote = ::lmLDPDataToFPitch( *pNotes );

    //get intervals
    for (int i=0; i < m_nNumIntv; i++)
    {
        m_nIntervals[i] = ::lmLDPDataToFPitch( *(pNotes+i+1) ) - fpRootNote;
    }
}

lmChordIntervals::lmChordIntervals(int nRootStep, lmEKeySignatures nKey, int nNumIntervals, int nInversion)
{
    m_nNumIntv = nNumIntervals;

    //get intervals
    for (int i=0; i < m_nNumIntv; i++)
    {
        m_nIntervals[i] = FPitchInterval(nRootStep, nKey, i+1);
    }

    lmFIntval nI;
    for (int i=0; i < m_nNumIntv; i++)
    {
        nI = m_nIntervals[i];
    }

    //apply inversions
    for (int i=0; i < nInversion; i++)
        DoInversion();

    for (int i=0; i < m_nNumIntv; i++)
    {
        nI = m_nIntervals[i];
    }
}


lmChordIntervals::~lmChordIntervals()
{
}

void lmChordIntervals::DoInversion()
{
    //Do first inversion of list of intervals.
    //Note that second inversion can be obtained by invoking this method
    //two times. The third inversion, by invoking it three times, etc.

    lmFIntval nNewIntervals[lmINTERVALS_IN_CHORD];
    for (int i=0; i < m_nNumIntv-1; i++)
    {
        nNewIntervals[i] = m_nIntervals[i+1] - m_nIntervals[0];
        if (nNewIntervals[i] < 0)
            nNewIntervals[i] += lm_p8;
    }

    nNewIntervals[m_nNumIntv-1] = lm_p8 - m_nIntervals[0];

    //transfer results
    for (int i=0; i < m_nNumIntv; i++)
        m_nIntervals[i] = nNewIntervals[i];

}

void lmChordIntervals::SortIntervals()
{
    //sort intervals, from shortest to largest. Bubble method

    bool fSwapDone = true;
    while (fSwapDone)
    {
        fSwapDone = false;
        for (int i = 0; i < m_nNumIntv - 1; i++)
        {
            if (m_nIntervals[i] > m_nIntervals[i+1])
            {
	            lmFIntval fiAux = m_nIntervals[i];
	            m_nIntervals[i] = m_nIntervals[i+1];
	            m_nIntervals[i+1] = fiAux;
	            fSwapDone = true;
            }
        }
    }
}

void lmChordIntervals::Normalize()
{
    //reduce any interval grater than the octave and unisons, and remove duplicated.
    //sort intervals

    if (m_nNumIntv < 1) // if no interval to normailze return
        return; // aware: no only to save time; the while below fails if 0 intervals

    //reduce any interval greater than the octave
    for (int i=0; i < m_nNumIntv; i++)
    {
        if (m_nIntervals[i] >= lm_p8)
            m_nIntervals[i] %= lm_p8;
    }

    SortIntervals();

    //remove duplicated and unisons
    int iCur = 1;
    int iLast = 0;
    while (iCur < m_nNumIntv)
    {
        if (m_nIntervals[iLast] != m_nIntervals[iCur])
        {
            if (m_nIntervals[iLast] != lm_p1)
                ++iLast;
            m_nIntervals[iLast] = m_nIntervals[iCur];
        }

        iCur++;
    }
    m_nNumIntv = iLast + 1;
}

wxString lmChordIntervals::DumpIntervals()
{
    wxString sIntvals = _T("");
    if (m_nNumIntv < 1)
        return sIntvals;
    for (int i=0; i < m_nNumIntv-1; i++)
    {
        sIntvals += FIntval_GetIntvCode( m_nIntervals[i] );
        sIntvals += _T(",");
    }
    sIntvals += FIntval_GetIntvCode( m_nIntervals[m_nNumIntv-1] );
    return sIntvals;
}

wxString lmChordIntervals::ToString()
{
    wxString sIntvals = _T(" Intervals:");
    for (int i=0; i < m_nNumIntv; i++)
    {
        sIntvals += wxString::Format(_("%s(%d) ")
            , FIntval_GetIntvCode( m_nIntervals[i] ).c_str(), m_nIntervals[i]);
    }
    return sIntvals;
}

// TODO: confirm this
bool lmChordIntervals::IsEqualTo(lmChordIntervals* tOther)
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


//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

wxString lmChordTypeToName(lmEChordType nType)
{
    if (nType >= ect_Max)
        return _("Not identified");

    //AWARE: language dependent strings. Can not be statically initiallized because
    //       then they do not get translated

    if (!m_fStringsInitialized)
    {
        // Triads
        m_sChordName[ect_MajorTriad] = _("Major triad");
        m_sChordName[ect_MinorTriad] = _("Minor triad");
        m_sChordName[ect_AugTriad] = _("Augmented triad");
        m_sChordName[ect_DimTriad] = _("Diminished triad");
        m_sChordName[ect_Suspended_4th] = _("Suspended triad (4th)");
        m_sChordName[ect_Suspended_2nd] = _("Suspended triad (2nd)");

        // Seventh chords
        m_sChordName[ect_MajorSeventh] = _("Major 7th");
        m_sChordName[ect_DominantSeventh] = _("Dominant 7th");
        m_sChordName[ect_MinorSeventh] = _("Minor 7th");
        m_sChordName[ect_DimSeventh] = _("Diminished 7th");
        m_sChordName[ect_HalfDimSeventh] = _("Half diminished 7th");
        m_sChordName[ect_AugMajorSeventh] = _("Augmented major 7th");
        m_sChordName[ect_AugSeventh] = _("Augmented 7th");
        m_sChordName[ect_MinorMajorSeventh] = _("Minor major 7th");

        // Sixth chords
        m_sChordName[ect_MajorSixth] = _("Major 6th");
        m_sChordName[ect_MinorSixth] = _("Minor 6th");
        m_sChordName[ect_AugSixth] = _("Augmented 6th");

        //Ninths
        m_sChordName[ect_DominantNinth] = _("Dominant ninth");
        m_sChordName[ect_MajorNinth] = _("Major ninth");
        m_sChordName[ect_MinorNinth] = _("Minor ninth");

        //11ths
        m_sChordName[ect_Dominant_11th] = _("Dominant 11th");
        m_sChordName[ect_Major_11th] = _("Major 11th");
        m_sChordName[ect_Minor_11th] = _("Minor 11th");

        //13ths
        m_sChordName[ect_Dominant_13th] = _("Dominant 13th");
        m_sChordName[ect_Major_13th] = _("Major 13th");
        m_sChordName[ect_Minor_13th] = _("Minor 13th");

        //Other
        //m_sChordName[ect_PowerChord] = _("Power chord");
        m_sChordName[ect_TristanChord] = _("Tristan chord");

        m_fStringsInitialized = true;
    }

    return m_sChordName[nType];
}

int lmNumNotesInChord(lmEChordType nChordType)
{
    wxASSERT(nChordType < ect_Max);
    return tChordData[nChordType].nNumNotes;
}

lmEChordType lmChordShortNameToType(wxString sName)
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
    if      (sName == _T("MT")) return ect_MajorTriad;
    else if (sName == _T("mT")) return ect_MinorTriad;
    else if (sName == _T("aT")) return ect_AugTriad;
    else if (sName == _T("dT")) return ect_DimTriad;
    else if (sName == _T("s4")) return ect_Suspended_4th;
    else if (sName == _T("s2")) return ect_Suspended_2nd;

        // Seventh chords
    else if (sName == _T("M7")) return ect_MajorSeventh;
    else if (sName == _T("dom7")) return ect_DominantSeventh;
    else if (sName == _T("m7")) return ect_MinorSeventh;
    else if (sName == _T("d7")) return ect_DimSeventh;
    else if (sName == _T("hd7")) return ect_HalfDimSeventh;
    else if (sName == _T("aM7")) return ect_AugMajorSeventh;
    else if (sName == _T("a7")) return ect_AugSeventh;
    else if (sName == _T("mM7")) return ect_MinorMajorSeventh;

        // Sixth chords
    else if (sName == _T("M6")) return ect_MajorSixth;
    else if (sName == _T("m6")) return ect_MinorSixth;
    else if (sName == _T("a6")) return ect_AugSixth;

    return (lmEChordType)-1;  //error
}


#ifdef __WXDEBUG__
//--------------------------------------------------------------------------------
// Debug global functions
//--------------------------------------------------------------------------------

bool lmChordUnitTests()
{
    //returns true if all tests passed correctly

    bool fTestOK = true;
    bool fOK;

    //lmChord contructor from lmFiguredBass
    fOK = lmChordFromFiguredBassUnitTest();
    fTestOK &= fOK;

    //TODO: Add other tests

    return fTestOK;
}

bool lmChordFromFiguredBassUnitTest()
{
    //Unit test for lmChord contructor from lmFiguredBass
    //returns true if all tests passed correctly

    typedef struct lmTestDataStruct
    {
        wxString            sFigBass;   //figured bass string
        lmEKeySignatures    nKey;       //key signature
        wxString            sRootNote;  //root note
        lmEChordType        nChordType; //test result, to validate test
        int                 nInversion; //test result, to validate test
        wxString            sIntervals; //test result, to validate test
    }
    lmTestData;

    static lmTestData tTestData[] =
    {
        //Minor scale
        //fig
        //bass       key      root note  chord type        inversion     Intvals
        { _T("#"),   earmLam, _T("a3"),  ect_MajorTriad,      0, _T("M3,p5") },
        { _T("b"),   earmLam, _T("a3"),  ect_Max,             0, _T("d3,p5") },
        { _T("="),   earmLam, _T("a3"),  ect_MinorTriad,      0, _T("m3,p5") },
        { _T("2"),   earmLam, _T("a3"),  ect_HalfDimSeventh,  3, _T("M2,p4,m6") },
        { _T("#2"),  earmLam, _T("a3"),  ect_Max,             0, _T("a2,p4,m6") },
        { _T("b2"),  earmLam, _T("a3"),  ect_MajorSeventh,    3, _T("m2,p4,m6") },
        { _T("=2"),  earmLam, _T("a3"),  ect_HalfDimSeventh,  3, _T("M2,p4,m6") },
        { _T("2+"),  earmLam, _T("a3"),  ect_Max,             0, _T("a2,p4,m6") },
        { _T("2 3"), earmLam, _T("a3"),  ect_Max,             0, _T("M2,m3") },
        { _T("3"),   earmLam, _T("a3"),  ect_MinorTriad,      0, _T("m3,p5") },
        { _T("4"),   earmLam, _T("a3"),  ect_Suspended_4th,   0, _T("p4,p5") },
        { _T("4 2"), earmLam, _T("a3"),  ect_HalfDimSeventh,  3, _T("M2,p4,m6") },
        { _T("4+ 2"), earmLam, _T("a3"), ect_AugSixth,        1, _T("M2,a4,m6") },
        { _T("4 3"), earmLam, _T("a3"),  ect_MinorSeventh,    2, _T("m3,p4,m6") },
        { _T("5"),   earmLam, _T("a3"),  ect_MinorTriad,      0, _T("m3,p5") },
        { _T("5 #"), earmLam, _T("a3"),  ect_MajorTriad,      0, _T("M3,p5") },
        { _T("5 b"), earmLam, _T("a3"),  ect_Max,             0, _T("d3,p5") },
        { _T("5+"),  earmLam, _T("a3"),  ect_Max,             0, _T("m3,a5") },
        { _T("5/"),  earmLam, _T("a3"),  ect_Max,             0, _T("m3,a5") },
        { _T("5 3"), earmLam, _T("a3"),  ect_MinorTriad,      0, _T("m3,p5") },
        { _T("5 4"), earmLam, _T("a3"),  ect_Suspended_4th,   0, _T("p4,p5") },
        { _T("6"),   earmLam, _T("a3"),  ect_MajorTriad,      1, _T("m3,m6") },
        { _T("6 #"), earmLam, _T("a3"),  ect_AugTriad,        1, _T("M3,m6") },
        { _T("6 b"), earmLam, _T("a3"),  ect_Max,             0, _T("d3,m6") },
        { _T("6\\"),  earmLam, _T("a3"),  ect_Max,             0, _T("m3,d6") },
        { _T("6 3"), earmLam, _T("a3"),  ect_MajorTriad,      1, _T("m3,m6") },
        { _T("6 #3"), earmLam, _T("a3"), ect_AugTriad,        1, _T("M3,m6") },
        { _T("6 b3"), earmLam, _T("a3"), ect_Max,             0, _T("d3,m6") },
        { _T("6 4"), earmLam, _T("a3"),  ect_MinorTriad,      2, _T("p4,m6") },
        { _T("6 4 2"), earmLam, _T("a3"),ect_HalfDimSeventh,  3, _T("M2,p4,m6") },
        { _T("6 4 3"), earmLam, _T("a3"),ect_MinorSeventh,    2, _T("m3,p4,m6") },
        { _T("6 5"), earmLam, _T("a3"),  ect_MajorSeventh,    1, _T("m3,p5,m6") },
        { _T("6 5 3"), earmLam, _T("a3"),ect_MajorSeventh,    1, _T("m3,p5,m6") },
        { _T("7"),   earmLam, _T("a3"),  ect_MinorSeventh,    0, _T("m3,p5,m7") },
        { _T("7 4 2"), earmLam, _T("a3"), ect_Max,            0, _T("M2,p4,m7") },
        { _T("8"),   earmLam, _T("a3"),  ect_Max,             0, _T("p8") },
        { _T("9"),   earmLam, _T("a3"),  ect_Max,             0, _T("m3,p5,M9") },
        { _T("10"),  earmLam, _T("a3"),  ect_Max,             0, _T("m3,p5,m10") },

        // Mayor scale
        { _T("#"),   earmDo, _T("c4"), ect_Max,              0, _T("a3,p5") },
        { _T("b"),   earmDo, _T("c4"), ect_MinorTriad,       0, _T("m3,p5") },
        { _T("="),   earmDo, _T("c4"), ect_MajorTriad,       0, _T("M3,p5") },
        { _T("2"),   earmDo, _T("c4"), ect_MinorSeventh,     3, _T("M2,p4,M6") },
        { _T("#2"),  earmDo, _T("c4"), ect_Max,              0, _T("a2,p4,M6") },
        { _T("b2"),  earmDo, _T("c4"), ect_AugMajorSeventh,  3, _T("m2,p4,M6") },
        { _T("=2"),  earmDo, _T("c4"), ect_MinorSeventh,     3, _T("M2,p4,M6") },
        { _T("2+"),  earmDo, _T("c4"), ect_Max,              0, _T("a2,p4,M6") },
        { _T("2 3"), earmDo, _T("c4"), ect_Max,              0, _T("M2,M3") },
        { _T("3"),   earmDo, _T("c4"), ect_MajorTriad,       0, _T("M3,p5") },
        { _T("4"),   earmDo, _T("c4"), ect_Suspended_4th,    0, _T("p4,p5") },
        { _T("4 2"), earmDo, _T("c4"), ect_MinorSeventh,     3, _T("M2,p4,M6") },
        { _T("4+ 2"), earmDo, _T("c4"), ect_DominantSeventh, 3, _T("M2,a4,M6") },
        { _T("4 3"), earmDo, _T("c4"), ect_MajorSeventh,     2, _T("M3,p4,M6") },
        { _T("5"),   earmDo, _T("c4"), ect_MajorTriad,       0, _T("M3,p5") },
        { _T("5 #"), earmDo, _T("c4"), ect_Max,              0, _T("a3,p5") },
        { _T("5 b"), earmDo, _T("c4"), ect_MinorTriad,       0, _T("m3,p5") },
        { _T("5+"),  earmDo, _T("c4"), ect_AugTriad,         0, _T("M3,a5") },
        { _T("5/"),  earmDo, _T("c4"), ect_AugTriad,         0, _T("M3,a5") },
        { _T("5 3"), earmDo, _T("c4"), ect_MajorTriad,       0, _T("M3,p5") },
        { _T("5 4"), earmDo, _T("c4"), ect_Suspended_4th,    0, _T("p4,p5") },
        { _T("6"),   earmDo, _T("c4"), ect_MinorTriad,       1, _T("M3,M6") },
        { _T("6 #"), earmDo, _T("c4"), ect_Max,              0, _T("a3,M6") },
        { _T("6 b"), earmDo, _T("c4"), ect_DimTriad,         1, _T("m3,M6") },
        { _T("6\\"),  earmDo, _T("c4"), ect_AugTriad,         1, _T("M3,m6") },
        { _T("6 3"), earmDo, _T("c4"), ect_MinorTriad,       1, _T("M3,M6") },
        { _T("6 #3"), earmDo, _T("c4"), ect_Max,             0, _T("a3,M6") },
        { _T("6 b3"), earmDo, _T("c4"), ect_DimTriad,        1, _T("m3,M6") },
        { _T("6 4"), earmDo, _T("c4"), ect_MajorTriad,       2, _T("p4,M6") },
        { _T("6 4 2"), earmDo, _T("c4"), ect_MinorSeventh,   3, _T("M2,p4,M6") },
        { _T("6 4 3"), earmDo, _T("c4"), ect_MajorSeventh,   2, _T("M3,p4,M6") },
        { _T("6 5"), earmDo, _T("c4"), ect_MinorSeventh,     1, _T("M3,p5,M6") },
        { _T("6 5 3"), earmDo, _T("c4"), ect_MinorSeventh,   1, _T("M3,p5,M6") },
        { _T("7"),   earmDo, _T("c4"), ect_MajorSeventh,     0, _T("M3,p5,M7") },
        { _T("7 4 2"), earmDo, _T("c4"), ect_Max,            0, _T("M2,p4,M7") },
        { _T("8"),   earmDo, _T("c4"), ect_Max,              0, _T("p8") },
        { _T("9"),   earmDo, _T("c4"), ect_Max,              0, _T("M3,p5,M9") },
        { _T("10"),  earmDo, _T("c4"), ect_Max,              0, _T("M3,p5,M10") },
    };

    int nNumTestCases = sizeof(tTestData) / sizeof(lmTestData);

    //TODO: Check that following cases are only possible in major scales
    //    ect_AugMajorSeventh
    //    ect_DominantSeventh
    //    ect_DimTriad


    wxLogMessage(_T("UnitTests: Chord contructor from lmFiguredBass"));
    wxLogMessage(_T("===================================================="));

    lmLDPParser parserLDP(_T("en"), _T("utf-8"));
    bool fTestOK = true;
    for (int i=0; i < nNumTestCases; i++)
    {
        lmScore* pScore = new lmScore();
        lmInstrument* pInstr = pScore->AddInstrument(0,0, _T(""));
        lmVStaff* pVStaff = pInstr->GetVStaff();
        pVStaff->AddClef( lmE_Sol );
        pVStaff->AddKeySignature( tTestData[i].nKey );
        pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
        wxString sPattern = _T("(figuredBass \"") + tTestData[i].sFigBass + _T("\")");
        lmLDPNode* pNode = parserLDP.ParseText( sPattern );
        lmFiguredBass* pFB = parserLDP.AnalyzeFiguredBass(pNode, pVStaff);

        lmChord oChord(tTestData[i].sRootNote, pFB, tTestData[i].nKey);
        bool fOK = (oChord.DumpIntervals() == tTestData[i].sIntervals)
                   && (tTestData[i].nChordType != ect_Max);
        fTestOK &= fOK;
        if (!fOK)
        {
            wxString sMsg = wxString::Format(_T("figured bass ='%s', chord type=%d (%s), inversion=%d, Intvals: "),
                tTestData[i].sFigBass.c_str(),
                oChord.GetChordType(),
                lmChordTypeToName(oChord.GetChordType()).c_str(),
                oChord.GetInversion() );
            sMsg += oChord.DumpIntervals();
            wxLogMessage(sMsg);
        }
        delete pScore;
    }

    return fTestOK;
}


#endif      //Debug global methods

