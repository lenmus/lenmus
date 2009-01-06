//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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
#pragma implementation "ComposerV6.h"
#endif

// For compilers that support precompilation, includes "wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/arrstr.h"      // to use wxArrayString
#include "vector"

#include "../score/Score.h"
#include "../score/VStaff.h"
#include "../ldp_parser/LDPParser.h"
#include "ComposerV6.h"
#include "Conversion.h"
#include "../ldp_parser/AuxString.h"
#include "../exercises/Generators.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"



enum lmEHarmonicFunction
{
    lmTONIC             = 0x00000001,   // I
    lmSUPERTONIC        = 0x00000002,   // ii
    lmMEDIANT           = 0x00000003,   // iii
    lmSUBDOMINANT       = 0x00000004,   // IV
    lmDOMINANT          = 0x00000005,   // V
    lmSUBMEDIANT        = 0x00000006,   // vi
    lmLEADING           = 0x00000007,   // vii
    lmSUBTONIC          = 0x00000007,   // vii

    // alias
    lm_I                = 0x00000001,
    lm_II               = 0x00000002,
    lm_III              = 0x00000003,
    lm_IV               = 0x00000004,
    lm_V                = 0x00000005,
    lm_VI               = 0x00000006,
    lm_VII              = 0x00000007,


    lmROOT_POSITION     = 0x00000008,
    lmFIRST_INVERSION   = 0x00000010,
    lmSECOND_INVERSION  = 0x00000020,
    lmTHIRD_INVERSION   = 0x00000040,

    lmSEVENTH_ADDED     = 0x00000100,

    // masks
    lmGRADE_MASK        = 0x00000007,   // to extract grade
};

// table of typical harmonic progressions, to base compositions on them
static lmEHarmonicFunction m_aProgression[][8] =
{
    {lm_I,  lm_V,   lm_I,   lm_IV,  lm_II,  lm_III, lm_IV,  lm_I },
    {lm_I,  lm_II,  lm_V,   lm_I,   lm_III, lm_IV,  lm_V,   lm_I },

};

lmComposer6::lmComposer6() : lmComposer()

{

}

lmComposer6::~lmComposer6()
{
    //
}


/*! Synthesises a score

    Algorithm 1 to generate N measures
    ----------------------------------

        1. Determine how may beats we have to generate:
                BeatsToGenerate = Beats_per_measure x N
                NumBeats = 0
        2. while (NumBeats < BeatsToGenerate) {
            2.1 Randomly choose a pattern satisfying the constraints (lesson, level, time
                signature, etc.).
            2.2 Instantiate the choosen pattern by assingning note pitches
            2.3 NumBeats = NumBeats + num. beats in choosen pattern
        }
        3. Divide generated beats string into measures (just assign to each measure the
           next n beats -- where n=Beats_per_measure -- )

        Problems:
            - difficulties for using patterns that are not full measures
            - difficulties for aligning patterns to barlines at specific points
            - strong limitation: requires patterns divided into beats. It would be
                impossible, for example, include a dotted quarter note in a binary
                measure.



    Algorithm 2 to generate N measures
    ----------------------------------
        Rationale:
            Instead of generating beats, let's organize the main loop around generating
            measures.
            Lets use patterns that contains musical phrases instead of full measures.
            Patterns will contain, when necessary, baline alignment information.
            Before adding a pattern to the current measure, lets align the pattern by
            inserting rests, if required.


        while (NumMeasures < MeasuresToGenerate) {
            If (no measure is opened) start a measure
            If (no beats in pattern) {
                Randomly choose a pattern satisfying the constraints (lesson,
                    level, time signature, etc.).
                Instantiate the choosen pattern by assingning note pitches.
                Align pattern to measure by adding any required rest to the measure
            }
            While (there are beats in the choosen pattern and measure not full) {
                add beats.
            }
            If measure full {
                Close measure
                NumMeasures++
            }
        }


    Algorithm 3 to generate N measures (8/Feb/06)
    ---------------------------------------------

        Rationale:
            Algorithm 2 generates scores with mis-aligments and irregular measures.
            The problem is due to lack of information about alignment. So in this
            improved algorith we will also use patterns containing musical phrases
            instead of full measures.
            These patterns will be called 'fragments'. Fragments are divided into
            'segments' (a segment is a group of elements - notes and rests - that must
            go together and occupies one or more full beats).
            Fragments will contain baline alignment information.

            The algorithm is essentialy the same, but the method to add beats is
            improved by taking into account alignment and duration information. Also
            we must take into account that a segment may contain more than a beat.

            Another improvement is generating the final measure not by using fragments
            but by generation a note lasting one ore more beats. And its pitch is the root
            pitch of the used key signature.

            Notes.
            - It is assumed that all beats are equal. Therefore, current algorith
                works only for regular time signature rhythms. It will
                not work, for example, with 7/8.



        while (NumMeasures < MeasuresToGenerate - 1 ) {
            If (no measure is opened) start a measure
            If (no beats in pattern) {
                Randomly choose a pattern satisfying the constraints (lesson,
                    level, time signature, etc.).
                Instantiate the choosen pattern by assingning note pitches.
            }
            While (there are beats in the choosen pattern and measure not full) {
                add beats.
            }
            If measure full {
                Close measure
                NumMeasures++
            }
        }
        Add final measure


*/
lmScore* lmComposer6::GenerateScore(lmScoreConstrains* pConstrains)
{
    //Save parameters
    m_pConstrains = pConstrains;

    //Generate a random key, time signature and clef satisfying the constraints
    lmRandomGenerator oGenerator;
    m_nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    m_nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    m_nTimeSign = oGenerator.GenerateTimeSign(m_pConstrains->GetTimeSignConstrains());


    // prepare and initialize the score
    lmLDPParser parserLDP(_T("es"), _T("utf-8"));
    lmLDPNode* pNode;
    lmScore* pScore = new lmScore();
    lmInstrument* pInstr = pScore->AddInstrument(g_pMidi->DefaultVoiceChannel(),
   						  g_pMidi->DefaultVoiceInstr(), _T(""));
    lmVStaff *pVStaff = pInstr->GetVStaff();
    pScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( m_nClef );
    pVStaff->AddKeySignature( m_nKey );
#if 0   //useful for debugging and to generate scores with a chosen rhythm line to write documentation
    pVStaff->AddTimeSignature( emtr24 );
    pNode = parserLDP.ParseText(_T("(c 1 (s c)(n * c)(n * c g+)(n * c l g-)(Barra Simple))"));
    parserLDP.AnalyzeMeasure(pNode, pVStaff);
    pNode = parserLDP.ParseText(_T("(c 2 (n * c g+)(n * s)(n * s g-)(n * c g+)(n * c g-)(Barra Simple))"));
    parserLDP.AnalyzeMeasure(pNode, pVStaff);
    pNode = parserLDP.ParseText(_T("(c 3 (n * n)(s c)(n * c)(Barra Simple))"));
    parserLDP.AnalyzeMeasure(pNode, pVStaff);
    pNode = parserLDP.ParseText(_T("(c 4 (n * s g+)(n * s)(n * s)(n * s g-)(n * c g+)(n * c l g-)(Barra Simple))"));
    parserLDP.AnalyzeMeasure(pNode, pVStaff);
    pNode = parserLDP.ParseText(_T("(c 5 (n * c g+)(n * s)(n * s g-)(n * s g+)(n * s)(n * s)(n * s g-)(Barra Simple))"));
    parserLDP.AnalyzeMeasure(pNode, pVStaff);
    pNode = parserLDP.ParseText(_T("(c 6 (n * s g+)(n * s)(n * s)(n * s g-)(n * c g+)(n * c l g-)(Barra Simple))"));
    parserLDP.AnalyzeMeasure(pNode, pVStaff);
    pNode = parserLDP.ParseText(_T("(c 7 (n * n)(s c)(n * c)(Barra Simple))"));
    parserLDP.AnalyzeMeasure(pNode, pVStaff);
    pNode = parserLDP.ParseText(_T("(c 8 (n * b)(Barra Final))"));
    parserLDP.AnalyzeMeasure(pNode, pVStaff);

#else
    pVStaff->AddTimeSignature( m_nTimeSign );

    //
    // Content generation
    //

    // Determine how may measures we have to generate:
    #define NUM_MEASURES   8        //num of measures to generate
    int nMeasuresToGenerate = NUM_MEASURES - 1;
    int nNumMeasures = 0;
    //int nMeasureDuration = GetNumBeatsFromTimeSignType(m_nTimeSign);
    float rMeasureDuration = GetMeasureDuration(m_nTimeSign);       //tm
    float rBeatDuration = GetBeatDuration(m_nTimeSign);             //tb
    float rTimeRemaining;           //tr
    float rSegmentDuration;         //ts
    float rConsumedBeatTime;        //tcb
    float rSegmentAlignBeatTime;    //tab


    // Loop to generate the required measures
    wxString sMeasure;              //source code of current measure
    bool fFits;                     //current segment fits in current measure
    float rOccupiedDuration;        //consumed time in current measure (tc)
    bool fMeasure = false;          //there is a measure started
    lmSegmentEntry* pSegment;       //segment to add to measure

    //select all usable fragments for current time signature
    //int nBeatType= GetBeatTypeFromTimeSignType(m_nTimeSign);
    if (m_pConstrains->SelectFragments(m_nTimeSign) == 0) {
        //TODO: error logging. Suppress message
        wxMessageBox(_("[lmComposer6::GenerateScore] No usable fragments!"));
        return pScore;
    }

    //chose ramdomly a fragment satisfying the constraints, and take the first segment
    pConstrains->ChooseRandomFragment();
    pSegment = pConstrains->GetNextSegment();
    //TODO: what if no fragment satisfies the constraints?

    int nSegmentLoopCounter = 0;
    while (nNumMeasures < nMeasuresToGenerate) {
        //If no measure is opened start a new measure
        if (!fMeasure) {
            sMeasure = wxString::Format(_T("(c %d "), nNumMeasures+1);
            //NEW sMeasure = _T("(musicData ");
            rOccupiedDuration = 0.0;
            fMeasure = true;
        }

        //If there are no more segments in current fragment, choose a new fragment
        if (!pSegment) {
            //Randomly choose a new fragment satisfying the constraints
            pConstrains->ChooseRandomFragment();
            pSegment = pConstrains->GetNextSegment();
            wxASSERT(pSegment);     //there must exits a fragment satisfying
                                    //the constraints. Otherwise this would have been
                                    //detected before entering the main while loop
        }

        //While (there are segments in the current fragment and the measure is not full) {
        while (pSegment && rOccupiedDuration < rMeasureDuration)
        {
            //check if segment fits in. A segment S will fit in the measure
            //only when (tr >= ts && tcb <= tab)

            rTimeRemaining =rMeasureDuration - rOccupiedDuration;
            rSegmentDuration = pSegment->GetSegmentDuration();
            rConsumedBeatTime = rOccupiedDuration;  //this line and next two ones compute tcb = tc % tb;
            while (IsHigherTime(rConsumedBeatTime, 0.0f))
                rConsumedBeatTime -= rBeatDuration;
            if (rConsumedBeatTime < 0.0)
                rConsumedBeatTime += rBeatDuration;
            rSegmentAlignBeatTime = pSegment->GetTimeAlignBeat();
            fFits = (!IsLowerTime(rTimeRemaining, rSegmentDuration) 
                     && !IsHigherTime(rConsumedBeatTime, rSegmentAlignBeatTime));

            g_pLogger->LogTrace(_T("lmComposer6"), _T("[GenerateScore] sMeasure=%s, pSegment=%s, tr=%.2f, ts=%.2f, tcb=%.2f, tab=%.2f, tc=%.2f, tb=%.2f, fits=%s"),
                    sMeasure.c_str(),
                    (pSegment->GetSource()).c_str(), rTimeRemaining, rSegmentDuration,
                    rConsumedBeatTime, rSegmentAlignBeatTime,
                    rOccupiedDuration, rBeatDuration,
                    (fFits ? _T("yes") : _T("no")) );

            //if segment fits add it to current measure
            if (fFits) {
                //it fits. Add it to current measure
                float rNoteTime = rSegmentAlignBeatTime - rConsumedBeatTime;
                if (IsHigherTime(rNoteTime, 0.0f)) {
                    if (rConsumedBeatTime > 0)
                        sMeasure += CreateNote((int)rNoteTime);
                    else
                        sMeasure += CreateRest((int)rNoteTime);
                }

                //add segment
                sMeasure += pSegment->GetSource();

                //update tr
                rOccupiedDuration += rSegmentDuration + rNoteTime;

                //get next segment
                pSegment = pConstrains->GetNextSegment();
                nSegmentLoopCounter = 0;
            }
            else {
                //does not fit.
                if (nSegmentLoopCounter++ > 100) {
                    //let's assume that no segment fits. Fill the measure with a note
                    sMeasure += CreateNote((int)rTimeRemaining);
                    rOccupiedDuration += rTimeRemaining;
                    nSegmentLoopCounter = 0;
                }
                else {
                    // Ignore segment and take a new one
                    pSegment = pConstrains->GetNextSegment();
                }
            }
        }

        // if measure is full, close it and increment measures count
       if (rOccupiedDuration >= rMeasureDuration) {

            // close current measure
            fMeasure = false;   // no measure opened
            sMeasure += _T("(Barra Simple))");
            //NEW sMeasure += _T("(barline simple))");

            // increment measures counter
            nNumMeasures++;

            // Instantiate the notes by assigning note pitches and add
            // the measure to the score
            g_pLogger->LogTrace(_T("lmComposer6"),
                    _T("[GenerateScore] Adding measure = '%s')"), sMeasure.c_str());
            pNode = parserLDP.ParseText(sMeasure);
            parserLDP.AnalyzeMeasure(pNode, pVStaff);
            //NEW parserLDP.AnalyzeMusicData(pNode, pVStaff);
        }

    }

    // In Music Reading, level 1, introduction lessons use only quarter notes. In those
    // exercises we should not use half notes in the last measure. So lets check if
    // only quarter notes are used in the composed piece of music.
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    bool fOnlyQuarterNotes = true;
    lmSOIterator* pIter = pVStaff->CreateIterator();
    while(!pIter->EndOfCollection()) {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_NoteRest) {
            pNR = (lmNoteRest*)pSO;
            if (pNR->IsNote()) {
                //OK. Note fount. Take duration
                pNote = (lmNote*)pSO;
                float rDuration = pNote->GetDuration();
                fOnlyQuarterNotes &= (rDuration == (float)eQuarterDuration);
                if (!fOnlyQuarterNotes) break;
            }
        }
        pIter->MoveNext();
    }
    delete pIter;
    g_pLogger->LogTrace(_T("lmComposer6"),
            _T("[GenerateScore] fOnlyQuarterNotes=%s)"),
            (fOnlyQuarterNotes ? _T("True") : _T("False")) );

    // add a final measure with a root pitch note lasting, at least, one beat
    sMeasure = CreateLastMeasure(++nNumMeasures, m_nTimeSign, fOnlyQuarterNotes);
    g_pLogger->LogTrace(_T("lmComposer6"),
            _T("[GenerateScore] Adding final measure = '%s')"), sMeasure.c_str());
    pNode = parserLDP.ParseText(sMeasure);
    parserLDP.AnalyzeMeasure(pNode, pVStaff);
    //NEW parserLDP.AnalyzeMusicData(pNode, pVStaff);
#endif

    // Score is built but pitches are not yet defined.
    // Proceed to instatiate pitches according to key signature
    GetNotesRange();
#if 0   //useful to generate only the rhymth line, to write documenation
    InstantiateWithNote(pScore, lmAPitch(_T("a4")) );
#else
    InstantiateNotes(pScore, m_nKey);
#endif

    // done
    //pScore->Dump(_T("lenus_score_dump.txt"));
    return pScore;

}

void lmComposer6::GetNotesRange()
{
    //get the minimum and maximum notes
    wxString sMinPitch = (m_pConstrains->GetClefConstrains())->GetLowerPitch(m_nClef);
    wxString sMaxPitch = (m_pConstrains->GetClefConstrains())->GetUpperPitch(m_nClef);
    lmConverter oConv;
    m_nMinPitch = lmAPitch(sMinPitch);
    m_nMaxPitch = lmAPitch(sMaxPitch);

}



int lmComposer6::GetRootStep(const lmEKeySignatures nKey) const
{
    // returns the step of root note for the given key signature.
    // For example, for C major returns lmSTEP_C (0); for A sharp minor returns lmSTEP_A (6).

    switch(nKey) {
        case earmDo:
        case earmDom:
        case earmDosm:
        case earmDos:
        case earmDob:
            return lmSTEP_C;

        case earmRe:
        case earmReb:
        case earmResm:
        case earmRem:
            return lmSTEP_D;

        case earmMi:
        case earmMim:
        case earmMib:
        case earmMibm:
            return lmSTEP_E;

        case earmFa:
        case earmFasm:
        case earmFas:
        case earmFam:
            return lmSTEP_F;

        case earmSol:
        case earmSolsm:
        case earmSolm:
        case earmSolb:
            return lmSTEP_G;

        case earmLa:
        case earmLam:
        case earmLasm:
        case earmLab:
        case earmLabm:
            return lmSTEP_A;

        case earmSim:
        case earmSi:
        case earmSib:
        case earmSibm:
            return lmSTEP_B;

        default:
            wxASSERT(false);
            return lmSTEP_C;
    }
}

void lmComposer6::AddSegment(wxString* pMeasure, lmSegmentEntry* pSegment, float rNoteTime)
{
    //This method adds the segment to the measure.
    //Precondition: it must be checked that segment fits in measure
    //The procedure is:
    //    1. Add note N(tab-tcb).
    //    2. Add segment S
    //    3. Update remaining time:  tr = tr - ts - (tab - tcb)
    //            [time update is not done here, but in calling method]

    //step 1
    int nNoteTime = (int)rNoteTime;
    if (nNoteTime > 0) *pMeasure += CreateNote(nNoteTime);

    // step 2
    g_pLogger->LogTrace(_T("lmComposer6"), _T("[AddSegment] Adding segment %s, duration=%.2f"),
        (pSegment->GetSource()).c_str(), pSegment->GetSegmentDuration());
    *pMeasure += pSegment->GetSource();

}

wxString lmComposer6::CreateNoteRest(int nNoteRestDuration, bool fNote)
{
    //Returns a string with one or more LDP elements containing notes o rests up to a total
    //duration nNoteDuration. They will be notes if fNote==true; otherwise they will be rests.
    //For example, for nNoteDuration=64 it will return "(n * n)"

    wxString sElement = _T("");
    int nDuration;
    int nTimeNeeded = nNoteRestDuration;

    while (nTimeNeeded > 0) {
        sElement += (fNote ? _T("(n * ") : _T("(s ") );
        if (nTimeNeeded >= eWholeDottedDuration) {
            sElement += _T("r.)");
            nDuration = eWholeDottedDuration;
        }
        else if (nTimeNeeded >= eWholeDuration) {
            sElement += _T("r)");
            nDuration = eWholeDuration;
        }
        else if (nTimeNeeded >= eHalfDottedDuration) {
            sElement += _T("b.)");
            nDuration = eHalfDottedDuration;
        }
        else if (nTimeNeeded >= eHalfDuration) {
            sElement += _T("b)");
            nDuration = eHalfDuration;
        }
        else if (nTimeNeeded >= eQuarterDottedDuration) {
            sElement += _T("n.)");
            nDuration = eQuarterDottedDuration;
        }
        else if (nTimeNeeded >= eQuarterDuration) {
            sElement += _T("n)");
            nDuration = eQuarterDuration;
        }
        else if (nTimeNeeded >= eEighthDottedDuration) {
            sElement += _T("c.)");
            nDuration = eEighthDottedDuration;
        }
        else if (nTimeNeeded >= eEighthDuration) {
            sElement += _T("c)");
            nDuration = eEighthDuration;
        }
        else if (nTimeNeeded >= e16hDottedDuration) {
            sElement += _T("s.)");
            nDuration = e16hDottedDuration;
        }
        else if (nTimeNeeded >= e16thDuration) {
            sElement += _T("s)");
            nDuration = e16thDuration;
        }
        else if (nTimeNeeded >= e32thDottedDuration) {
            sElement += _T("f.)");
            nDuration = e32thDottedDuration;
        }
        else if (nTimeNeeded >= e32thDuration) {
            sElement += _T("f)");
            nDuration = e32thDuration;
        }
        else if (nTimeNeeded >= e64thDottedDuration) {
            sElement += _T("m.)");
            nDuration = e64thDottedDuration;
        }
        else if (nTimeNeeded >= e64thDuration) {
            sElement += _T("m)");
            nDuration = e64thDuration;
        }
        else if (nTimeNeeded >= e128thDottedDuration) {
            sElement += _T("g.)");
            nDuration = e128thDottedDuration;
        }
        else if (nTimeNeeded >= e128thDuration) {
            sElement += _T("g)");
            nDuration = e128thDuration;
        }
        else {
            sElement += _T("p)");
            nDuration = e256thDuration;
        }

        nTimeNeeded -= nDuration;

    }
    g_pLogger->LogTrace(_T("lmComposer6"), _T("[CreateNoteRest] Needed duration= %d, added=%s"),
        nNoteRestDuration, sElement.c_str());
    return sElement;

}

wxString lmComposer6::CreateLastMeasure(int nNumMeasure, lmETimeSignature nTimeSign,
                                        bool fOnlyQuarterNotes)
{
    // Returns a final meaure. This final measure has only a note, long enough, and
    // a final bar

    wxString sMeasure = wxString::Format(_T("(c %d "), nNumMeasure);
    float rMeasureDuration = GetMeasureDuration(nTimeSign);
    float rBeatDuration = GetBeatDuration(nTimeSign);
    float rNoteDuration = rBeatDuration;
    if (!fOnlyQuarterNotes && rMeasureDuration / rBeatDuration >= 2.0) {
        //flip coin to randomly add a one-beat note or a two-beats note
        lmRandomGenerator oGenerator;
        if (oGenerator.FlipCoin()) rNoteDuration += rBeatDuration;
    }

    sMeasure += CreateNote((int)rNoteDuration);
    rNoteDuration = rMeasureDuration - rNoteDuration;
    if (rNoteDuration > 0.0) sMeasure += CreateRest((int)rNoteDuration);
    sMeasure += _T("(Barra Final))");
    return sMeasure;
}

//----------------------------------------------------------------------------------
// Methods to deal with tonality
//----------------------------------------------------------------------------------

bool lmComposer6::InstantiateNotes(lmScore* pScore, lmEKeySignatures nKey)
{
    // Returns true if error


    // Choose a chord progression, based on key signature: nChords[]
    int nNumMeasures = pScore->GetNumMeasures();
    std::vector<long> nChords(nNumMeasures);
    GetRandomHarmony(nNumMeasures, nChords);

    // Lets compute the notes in the natural scale of the key signature to use
    // This will be used later in various places
    lmAPitch aScale[7];             // the notes in the scale
    GenerateScale(nKey, aScale);

    // In a later step we are goin to choose and compute a contour curve.
    // The contour curve will have as many points as on-chord notes in the music line.
    // So first we have to compute the number of on-chord notes. The following code is a
    // loop to count on-chord notes in first staff of first instrument
    // and to locate last note. This is necessary to assign it the root pitch (later)
    lmNote* pLastNote = (lmNote*)NULL;
    lmInstrument* pInstr = pScore->GetFirstInstrument();
    lmVStaff* pVStaff = pInstr->GetVStaff();
    lmSOIterator* pIter = pVStaff->CreateIterator();
    int nNumPoints = 0;
    while(!pIter->EndOfCollection())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        EStaffObjType nType = pSO->GetClass();
        if (nType == eSFOT_NoteRest) {
            if ( ((lmNoteRest*)pSO)->IsNote() )
            {
                // It is a note. Get its chord position
                if( ((lmNote*)pSO)->GetChordPosition() != lmNON_CHORD_NOTE)
                {
                    // on beat note
                    nNumPoints++;
                }
                pLastNote = (lmNote*)pSO;
            }
        }

        pIter->MoveNext();
    }
    delete pIter;

    // If number of points is small (i.e < 8) forget about this. Instatiate notes
    // with random pitch and finish. This bypasess the only problem found, when a
    //score has rests in all beat positions (L2_musicReading, Lesson 17, exercise 1)
    if (nNumPoints < 8)
    {
        InstantiateNotesRandom(pScore);
        return false;       // no error
    }


    // Now we are going to choose at random a contour curve and compute its points
    // The curve will always start and finish in the root note. Its amplitude will
    // be adjusted to satisfy the constrains (min and max pitch)
    std::vector<lmDPitch> aContour(nNumPoints);
    GenerateContour(nNumPoints, aContour);

    // allocate a vector for valid notes in chord (all notes in valid notes range)
    int iC = 0;                                         //index to current chord (ic)
    lmDPitch dnMinPitch = m_nMinPitch.ToDPitch();
    lmDPitch dnMaxPitch = m_nMaxPitch.ToDPitch();
    g_pLogger->LogTrace(_T("lmComposer6::InstantiateNotes"), _T("min pitch %d (%s), max pitch %d (%s)"),
        dnMinPitch, DPitch_ToLDPName(dnMinPitch).c_str(),
        dnMaxPitch, DPitch_ToLDPName(dnMaxPitch).c_str() );
    std::vector<lmAPitch> aOnChordPitch;
    aOnChordPitch.reserve((dnMaxPitch - dnMinPitch)/2);    // Reserve space. Upper limit estimation
    lmAPitch nRootNote = GenerateInChordList(nKey, nChords[iC], aOnChordPitch);

    // Loop to process notes/rests in first staff of first instrument
    lmNote* pOnChord1 = (lmNote*)NULL;      //Pair of on-chord notes. First one
    lmNote* pOnChord2 = (lmNote*)NULL;      //Pair of on-chord notes. Second one
    lmNote* pNonChord[20];                  //non-chod notes between the two on-chord notes
    int nCount = 0;                         //number of non-chord notes between the two on-chord notes

    lmNote* pNotePrev = (lmNote*)NULL;
    lmNote* pNoteCur;
    int iPt = 0;
    lmAPitch apPitchNew;
    wxString sDbg = _T("");
    pIter = pVStaff->CreateIterator();
    while(!pIter->EndOfCollection())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_NoteRest)
        {
            // 1. It is a note or a rest
            if ( ((lmNoteRest*)pSO)->IsNote() )
            {
                // It is a note. Get its chord position
                pNoteCur = (lmNote*)pSO;
                if (pNoteCur->GetChordPosition() != lmNON_CHORD_NOTE)
                {
                    // on beat note. Pitch must be on chord.
                    // Assign a pitch from nChords[iC].
                    for(int k=0; k < (int)aOnChordPitch.size(); k++)
                        g_pLogger->LogTrace(_T("lmComposer6::InstantiateNotes"), _T("OnChord %d = %s"), k, aOnChordPitch[k].LDPName().c_str() );
                    apPitchNew = NearestNoteOnChord(aContour[iPt++], pNotePrev, pNoteCur,
                                            aOnChordPitch);
                    for(int k=0; k < (int)aOnChordPitch.size(); k++)
                        g_pLogger->LogTrace(_T("lmComposer6::InstantiateNotes"), _T("OnChord %d = %s"), k, aOnChordPitch[k].LDPName().c_str() );
                    wxString sNoteName = apPitchNew.LDPName();
                    g_pLogger->LogTrace(_T("lmComposer6::InstantiateNotes"), _T("on-chord note %d. Assigned pitch = %d (%s), chord=%d"),
                        iPt, apPitchNew.ToDPitch(), sNoteName.c_str(),
                        nChords[iC] & lmGRADE_MASK);
                    pNoteCur->ChangePitch(apPitchNew, lmCHANGE_TIED);

                    // assing pitch to non-chord notes between previous on-chord one
                    // and this one
                    sDbg += _T(",c");
                    if (nCount != 0) {
                        pOnChord2 = pNoteCur;
                        AssignNonChordNotes(nCount, pOnChord1, pOnChord2, pNonChord, aScale);
                    }

                    // Prepare data for next pair processing
                    nCount = 0;
                    pOnChord1 = pNoteCur;

                }

                else
                {
                    // non-chord note. Save it to be processed
                    if (!pNoteCur->IsPitchDefined()) {
                        pNonChord[nCount++] = pNoteCur;
                        sDbg += _T(",nc");
                    }
                    else
                        sDbg += _T(",(nc)");
                }
                pNotePrev = pNoteCur;
            }
        }

        else if (pSO->GetClass() == eSFOT_Barline)
        {
            // End of measure:
            // choose the next chord in progression
            iC++;
            nRootNote = GenerateInChordList(nKey, nChords[iC % 8], aOnChordPitch);
        }

        // get next note/rest
        pIter->MoveNext();

    }
    delete pIter;

    //// All chord notes are assigned. Now we are going to traverse the score again
    //// to classify non-chord notes and to assign pitch to them
    //AssignNCT(pScore, aScale);
    g_pLogger->LogTrace(_T("lmComposer6::InstantiateNotes"), sDbg);

    return false;       // no error

}

void lmComposer6::InstantiateNotesRandom(lmScore* pScore)
{
    // Loop to process notes/rests in first staff of first instrument
    lmInstrument* pInstr = pScore->GetFirstInstrument();
    lmVStaff* pVStaff = pInstr->GetVStaff();
    lmSOIterator* pIter = pVStaff->CreateIterator();
    while(!pIter->EndOfCollection())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsNote())
        {
            // Assign it a random pitch
            lmAPitch apPitchNew = RandomPitch();
            ((lmNote*)pSO)->ChangePitch(apPitchNew, lmCHANGE_TIED);
        }

        // get next note/rest
        pIter->MoveNext();
    }

    delete pIter;
}

lmAPitch lmComposer6::RandomPitch()
{
    int nMinPitch = (int)m_nMinPitch.ToDPitch();
    int nMaxPitch = (int)m_nMaxPitch.ToDPitch();
    static int nLastPitch = 0;
    
    if (nLastPitch == 0)
        nLastPitch = (nMinPitch + nMaxPitch) / 2;

    lmRandomGenerator oGenerator;
    int nRange = m_pConstrains->GetMaxInterval();
    int nLowLimit = wxMax(nLastPitch - nRange, nMinPitch);
    int nUpperLimit = wxMin(nLastPitch + nRange, nMaxPitch);
    int nNewPitch;
    if (nUpperLimit - nLowLimit < 2)
        nNewPitch = nLowLimit;
    else
        nNewPitch = oGenerator.RandomNumber(nLowLimit, nUpperLimit);

    // save value
    nLastPitch = nNewPitch;

    return lmAPitch((lmDPitch)nNewPitch, 0);
}

void lmComposer6::GetRandomHarmony(int nFunctions, std::vector<long>& aFunction)
{
    //Fills array 'pFunction' with an ordered set of harmonic functions to
    //build a melody. i.e.: I,V,I,IV,II,III,IV,I

    int nNumProgs = sizeof(m_aProgression) / (8 * sizeof(long));
    int iP = lmRandomGenerator::RandomNumber(0, nNumProgs-1);
    for(int i=0; i < 8; i++) {
        aFunction[i] = m_aProgression[iP][i];
    }

}

void lmComposer6::FunctionToChordNotes(lmEKeySignatures nKey, long nFunction,
                                       lmAPitch aNotes[4])
{
    //Given a key signature and an harmonic function returns the notes to build the
    //chord (four notes per chord). The first chord note is always in octave 4
    //i.e.:
    //C Major, II --> d4, f4, a4
    //D Major, I  --> d4, +f4, a4

    int nAcc[7];
    ComputeAccidentals(nKey, nAcc);
    int nStep = GetRootStep(nKey);
    lmAPitch aScale[15];
    int nOctave = lmOCTAVE_4;
    for (int iN=0; iN < 15; iN++) {
        aScale[iN].Set(nStep, nOctave, nAcc[nStep]);
        if(++nStep == 7) {
            nStep = 0;
            nOctave++;
        }
    }


    // Compute the triad
    long iF = (nFunction & lmGRADE_MASK) - 1L;
    aNotes[0].Set( aScale[iF] );
    aNotes[1].Set( aScale[iF+2] );
    aNotes[2].Set( aScale[iF+4] );

    g_pLogger->LogTrace(_T("lmComposer6::FunctionToChordNotes"), _T("Function %d, Key=%d, note0 %d (%s), note1 %d (%s), note2 %d (%s)."),
        iF, nKey,
        aNotes[0].ToDPitch(), aNotes[0].LDPName().c_str(),
        aNotes[1].ToDPitch(), aNotes[1].LDPName().c_str(),
        aNotes[2].ToDPitch(), aNotes[2].LDPName().c_str() );


}

lmAPitch lmComposer6::MoveByStep(bool fUpStep, lmAPitch nPitch, lmAPitch aScale[7])
{
    // Generates a new note by moving up/down one step in the scale
    // The new pitch must be on the key signature natural scale

    // extract pitch components
    int nStep = nPitch.Step();
    int nOctave = nPitch.Octave();

    // find current note in scale
    int i;
    for (i=0; i < 7; i++) {
        if (aScale[i].Step() == nStep) break;
    }
    wxASSERT(i < 7);

    if (fUpStep) {
        // increment note
        if (++i == 7) i = 0;
        if (nStep == lmSTEP_B) nOctave++;
    }
    else {
        // decrement note
        if (--i == -1) i = 6;
        if (nStep == lmSTEP_C) nOctave--;
    }

    nStep = aScale[i].Step();
    int nAcc = aScale[i].Accidentals();
    return lmAPitch(nStep, nOctave, nAcc);

}

lmAPitch lmComposer6::MoveByChromaticStep(bool fUpStep, lmAPitch nPitch)
{
    // Generates a new note by moving up/down one chromatic step in the scale

    // extract pitch accidentals
    int nAcc = nPitch.Accidentals();

    if (fUpStep) {
        nAcc++;
    }
    else {
        nAcc--;
    }
    return lmAPitch(nPitch.ToDPitch(), nAcc);

}

void lmComposer6::GenerateScale(lmEKeySignatures nKey, lmAPitch aNotes[7])
{
    int nAcc[7];
    ComputeAccidentals(nKey, nAcc);
    int nStep = GetRootStep(nKey);
    for (int iN=0; iN < 7; iN++) {
        aNotes[iN].Set(nStep, lmOCTAVE_4, nAcc[nStep]);
        if(++nStep == 7) nStep = 0;
    }
}

lmAPitch lmComposer6::GenerateInChordList(lmEKeySignatures nKey, long nChord,
                                             std::vector<lmAPitch>& aValidPitch)
{
    // Returns the root note in octave 4
    // Generates a list with all allowed notes in the chord, satisfying the
    // constraints for notes range. For instance:
    // D Major chord: d4, +f4, a4
    // notes range: a3 to a5
    // returns: a3, d4, +f4, a4, d5, +f5, a5

    // allocate an array for notes in chord (basic chord, octave 4)
    lmAPitch aNotes[4];                          // notes in current chord
    FunctionToChordNotes(nKey, nChord, aNotes);

    // extract valid steps, to simplify
    int nValidStep[4];
    for (int i=0; i < 4; i++) {
        if (aNotes[i].ToDPitch() == lmNO_NOTE)
            nValidStep[i] = -1;        //you can assign any non valid value for a step
        else
            nValidStep[i] = aNotes[i].Step();
    }

    // empty valid pitches array
    aValidPitch.clear();

    // scan notes range and select those in chord
    lmDPitch dnMinPitch = m_nMinPitch.ToDPitch();
    lmDPitch dnMaxPitch = m_nMaxPitch.ToDPitch();
    for (int i=dnMinPitch; i <= dnMaxPitch; i++) {
        int nStep = DPitch_Step(i);
        for (int j=0; j < 4; j++) {
            if (nStep == nValidStep[j]) {
                // Note in chord. Add it to the list
                lmAPitch nPitch(nStep, DPitch_Octave(i), aNotes[j].Accidentals());
                aValidPitch.push_back(nPitch);
            }
        }
    }

    return aNotes[0];

}

void lmComposer6::GenerateContour(int nNumPoints, std::vector<lmDPitch>& aContour)
{
    // In this method we choose at random a contour curve and compute its points
    // The curve will always start and finish in the root note. Its amplitude will
    // be adjusted to satisfy the constrains (min and max pitch), and will be
    // a value between one and two octaves, depending on the valid notes range and
    // the type of contour.
    // In case the valid notes range is lower than one octave, arch like curves will
    // be forced

    // First, we will determine the root note
    int nRootStep = GetRootStep(m_nKey);

    // Now lets do some computations to determine a suitable octave
    lmDPitch dnMinPitch = m_nMinPitch.ToDPitch();
    lmDPitch dnMaxPitch = m_nMaxPitch.ToDPitch();
    int nAmplitude = dnMaxPitch - dnMinPitch + 1;
    g_pLogger->LogTrace(_T("lmComposer6::GenerateContour"), _T("minPitch %d  (%s), max pitch %d (%s), amplitude %d"),
        dnMinPitch, DPitch_ToLDPName(dnMinPitch).c_str(),
        dnMaxPitch, DPitch_ToLDPName(dnMaxPitch).c_str(),
        nAmplitude );


        // determine minimum root pitch
    int nOctave = DPitch_Octave(dnMinPitch);
    if (DPitch_Step(dnMinPitch) > nRootStep) nOctave++;
    lmDPitch dnMinRoot = DPitch(nRootStep, nOctave);

        // determine maximum root pitch
    lmDPitch dnMaxRoot = dnMinRoot;
    while (dnMaxRoot+7 <= dnMaxPitch)
        dnMaxRoot+=7;

        // if range greater than two octaves reduce it and reposition
    if (dnMaxRoot-dnMinRoot > 14) {
        int nRange = (dnMaxRoot-dnMinRoot) / 7;
        int nShift = lmRandomGenerator::RandomNumber(0, nRange-2);
        dnMinRoot += 7*nShift;
        dnMaxRoot = dnMinRoot + 14;
    }



    g_pLogger->LogTrace(_T("lmComposer6::GenerateContour"), _T("min root %d  (%s), max root %d (%s)"),
        dnMinRoot, DPitch_ToLDPName(dnMinRoot).c_str(),
        dnMaxRoot, DPitch_ToLDPName(dnMaxRoot).c_str() );



    // Choose a contour curve
    enum {
        lmCONTOUR_TRIANGLE = 0,
        lmCONTOUR_TRIANGLE_RAMP,
        lmCONTOUR_RAMP_TRIANGLE,
        lmSTART_RESTRICTED_CONTOURS,
        lmCONTOUR_ZIG_ZAG = lmSTART_RESTRICTED_CONTOURS,
        lmCONTOUR_RAMP,
        lmMAX_CONTOUR,
        lmCONTOUR_ARCH,             //Bad results. Top is very flat so the sensation of
                                    //raeching a peak is very poor
    };



    // Choose a contour curve. If range is not at least an octave, do not allow
    // ramp curves
    int nCurve;
    bool fUp;
    if (dnMaxRoot == dnMinRoot)
    {
        // Pitch range is not at least an octave. Only some curves allowed.
        nCurve = lmRandomGenerator::RandomNumber(0, lmSTART_RESTRICTED_CONTOURS-1);
        // Lets force curve direction to better use the avalable notes range
        if (dnMaxPitch < dnMinRoot)
        {
            // allowed notes does not include root note. Use all notes range.
            // Curve direction doesn't matter
            dnMinRoot = dnMinPitch;
            dnMaxRoot = dnMaxPitch;
            fUp = lmRandomGenerator::FlipCoin();
        }
        else if (dnMinRoot-dnMinPitch < dnMaxPitch-dnMinRoot)
        {
            // Use upper part of range. Curve going up
            fUp = true;
            dnMaxRoot = dnMaxPitch;
        }
        else
        {
            // use lower part of range. Curve going down.
            fUp = false;
            dnMinRoot = dnMinPitch;
        }
    }
    else
    {
        // range is grater than one octave. Any curve and direction allowed
        nCurve = lmRandomGenerator::RandomNumber(0, lmMAX_CONTOUR-1);
        fUp = lmRandomGenerator::FlipCoin();
    }

    g_pLogger->LogTrace(_T("lmComposer6::GenerateContour"), _T("type=%d, nNumPoints=%d, up=%s"),
        nCurve, nNumPoints, (fUp ? _T("Yes") : _T("No")) );

    // prepare curve parameters and compute the curve points
    lmDPitch dnLowPitch, dnHighPitch, dnStartRamp, dnEndRamp;
    switch (nCurve)
    {
        case lmCONTOUR_ARCH:
            //----------------------------------------------------------------------------
            // Arch. An arch will be defined by the amplitude, the center beat, and
            //the direction
            //
            if (fUp) {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
            }
            else {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
            }
            ComputeArch(fUp, 0, nNumPoints, dnLowPitch, dnHighPitch, aContour);
            break;

        case lmCONTOUR_TRIANGLE:
            //----------------------------------------------------------------------------
            // Triangle. Defined the amplitude, the center beat, and the direction
            if (fUp) {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
            }
            else {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
            }
            ComputeTriangle(fUp, 0, nNumPoints, dnLowPitch, dnHighPitch, aContour);
            break;

        case lmCONTOUR_RAMP:
            //----------------------------------------------------------------------------
            // Ramp.
            // Amplitude will move two octaves from root to root note unless not possible
            if (fUp)
                ComputeRamp(0, nNumPoints, dnMinRoot, dnMaxRoot, aContour);
            else
                ComputeRamp(0, nNumPoints, dnMaxRoot, dnMinRoot, aContour);
            break;

        case lmCONTOUR_TRIANGLE_RAMP:
            //----------------------------------------------------------------------------
            // Triangle+Ramp
            // Triangle occupies two thirds and ramp one third
        {
            // Triangle: set up amplitude and last point
            int nPoints = 2* nNumPoints / 3;
            if (fUp) {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
                dnStartRamp = dnHighPitch;
                dnEndRamp = dnLowPitch;
            }
            else {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
                dnStartRamp = dnLowPitch;
                dnEndRamp = dnHighPitch;
            }
            ComputeTriangle(fUp, 0, nPoints, dnLowPitch, dnHighPitch, aContour);
            ComputeRamp(nPoints, nNumPoints, dnStartRamp, dnEndRamp, aContour);
            break;
        }

        case lmCONTOUR_ZIG_ZAG:
            //----------------------------------------------------------------------------
            // Triangle+Ramp
            // Triangle occupies two thirds and ramp one third
        {
            // Triangle: set up amplitude and last point
            int nPoints = 2* nNumPoints / 3;
            if (fUp) {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
                dnStartRamp = dnLowPitch;
                dnEndRamp = dnMaxRoot;
            }
            else {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
                dnStartRamp = dnHighPitch;
                dnEndRamp = dnMinRoot;
            }
            ComputeTriangle(fUp, 0, nPoints, dnLowPitch, dnHighPitch, aContour);
            ComputeRamp(nPoints, nNumPoints, dnStartRamp, dnEndRamp, aContour);
            break;
        }

        case lmCONTOUR_RAMP_TRIANGLE:
            //----------------------------------------------------------------------------
            // Ramp+Triangle
        {
            // Ramp: num points
            int nPoints = nNumPoints / 3;

            // Triangle: set up amplitude and last point
            if (fUp) {
                dnLowPitch = dnMinRoot;
                nAmplitude = dnMaxPitch - dnLowPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnHighPitch = dnLowPitch + nAmplitude;
                dnStartRamp = dnLowPitch;
                dnEndRamp = dnMaxRoot;
            }
            else {
                dnHighPitch = dnMaxRoot;
                nAmplitude = dnHighPitch - dnMinPitch;
                if (nAmplitude > 14)
                    nAmplitude = 14;
                dnLowPitch = dnHighPitch - nAmplitude;
                dnStartRamp = dnHighPitch;
                dnEndRamp = dnMinRoot;
            }

            ComputeRamp(0, nPoints, dnStartRamp, dnEndRamp, aContour);
            ComputeTriangle(fUp, nPoints, nNumPoints, dnLowPitch, dnHighPitch, aContour);
            break;
        }
    }

    for (int i=0; i < nNumPoints; i++)
        g_pLogger->LogTrace(_T("lmComposer6::GenerateContour"), _T("point[%d] = %d"), i, aContour[i]);

}

void lmComposer6::ComputeTriangle(bool fUp, int iStart, int nPoints, lmDPitch dnLowPitch,
                                  lmDPitch dnHighPitch, std::vector<lmDPitch>& aPoints)
{
    // Triangle. Defined the amplitude, the center beat, and the direction
    // (up/down). Also by start pitch, top/bottom pitch, and end pitch

    // first ramp
    float rNumPoints = (float)((nPoints-iStart)/2);
    float rStep = (float)(dnHighPitch - dnLowPitch) / rNumPoints;
    if (!fUp) rStep = -rStep;
    float yValue = (float)(fUp ? dnLowPitch : dnHighPitch);
    g_pLogger->LogTrace(_T("lmComposer6"), _T("[ComputeTriangle] fUp=%s, iStart=%d, nPoints=%d, dnLowPitch=%d, dnHighPitch=%d, rStep=%.5f"),
        (fUp ? _T("Yes") : _T("No")), iStart, nPoints, dnLowPitch, dnHighPitch, rStep);
    int i = iStart;
    int nCenter = (nPoints+iStart)/2;
    for (; i < nCenter; i++) {
        aPoints[i] = (int)floor(yValue+0.5);
        yValue += rStep;
    }
    yValue -= rStep;

    // second ramp
    rStep = (float)(dnHighPitch - dnLowPitch) / ((float)(nPoints-iStart) - rNumPoints);
    if (fUp) rStep = -rStep;
    g_pLogger->LogTrace(_T("lmComposer6"), _T("[ComputeTriangle] fUp=%s, iStart=%d, nPoints=%d, dnLowPitch=%d, dnHighPitch=%d, rStep=%.5f"),
        (fUp ? _T("Yes") : _T("No")), i, nPoints, dnLowPitch, dnHighPitch, rStep);
    for (; i < nPoints; i++) {
        aPoints[i] = (int)floor(yValue+0.5);
        yValue += rStep;
    }

    //force last point to be a root note
    aPoints[nPoints-1] = (fUp ? dnLowPitch : dnHighPitch);

}

void lmComposer6::ComputeRamp(int iStart, int nPoints, lmDPitch dnStartPitch,
                              lmDPitch dnEndPitch, std::vector<lmDPitch>& aPoints)
{
    // Ramp
    float rNumPoints = (float)(nPoints-iStart);
    float rStep = (float)(dnEndPitch - dnStartPitch) / rNumPoints;
    float yValue = (float)dnStartPitch;
    g_pLogger->LogTrace(_T("lmComposer6"), _T("[ComputeRamp] iStart=%d, nPoints=%d, dnStartPitch=%d, dnEndPitch=%d, rStep=%.5f"),
        iStart, nPoints, dnStartPitch, dnEndPitch, rStep);
    for (int i=iStart; i < nPoints; i++) {
        aPoints[i] = (int)floor(yValue+0.5);
        yValue += rStep;
    }
    //force last point to be a root note
    aPoints[nPoints-1] = dnEndPitch;

}

void lmComposer6::ComputeArch(bool fUp, int iStart, int nPoints, lmDPitch dnLowPitch,
                              lmDPitch dnHighPitch, std::vector<lmDPitch>& aPoints)
{
    // Arch. An arch will be defined the amplitude, the center beat, and the direction
    // (up/down). Also by start pitch, top/bottom pitch, and end pitch
    // I will use a second degree polinimio. I approximate it by using the Lagrange method.
    // The resulting polinom is
    //      P(x) = RootPitch + ((4 * maxPitch * x * (numPoints - x))/numPoints**2)
    //           = a3 + ((a1 * x * (numPoints - x))/ a2)
    //      a1 = 4 * maxPitch
    //      a2 = numPoints**2
    //      a3 = RootPitch
    //      a4 = numPoints

    g_pLogger->LogTrace(_T("lmComposer6"), _T("[ComputeArch] fUp=%s, iStart=%d, nPoints=%d, dnLowPitch=%d, dnHighPitch=%d"),
        (fUp ? _T("Yes") : _T("No")), iStart, nPoints, dnLowPitch, dnHighPitch);
    float a1 = 4.0 * (float)(dnHighPitch-dnLowPitch);
    float a2 = (float)(nPoints * nPoints);
    float a3 = (float)dnLowPitch;
    float a4 = (float)nPoints;
    float x = 0.0;
    for (int i=iStart; i < iStart+nPoints; i++, x+=1.0) {
        float y = a3 + ((a1 * x * (a4 - x)) / a2);
        aPoints[i] = (int)floor(y + 0.5);
    }
    //force last point to be a root note
    aPoints[iStart+nPoints-1] = (fUp ? dnLowPitch : dnHighPitch);

}


lmAPitch lmComposer6::NearestNoteOnChord(lmDPitch nPoint, lmNote* pNotePrev,
                                            lmNote* pNoteCur,
                                            std::vector<lmAPitch>& aOnChordPitch)
{
    g_pLogger->LogTrace(_T("lmComposer6::NearestNoteOnChord"), _T("nPoint=%d"), nPoint );

    // if note is tied to previous one, return previous note pitch
    if (pNotePrev && pNotePrev->IsTiedToNext() && pNotePrev->IsPitchDefined()) {
        g_pLogger->LogTrace(_T("lmComposer6::NearestNoteOnChord"), _T("Previous note = %s"), (pNotePrev->GetAPitch()).LDPName().c_str());
        return pNotePrev->GetAPitch();
    }

    for (int i=0; i < (int)aOnChordPitch.size(); i++) {
        lmDPitch dnCur = aOnChordPitch[i].ToDPitch();
        if (nPoint == dnCur)
            return aOnChordPitch[i];
        else if (nPoint < dnCur) {
            // The nearest one is this one or the previous one
            // If no previous one, return this one
            if (i == 0)
                return aOnChordPitch[i];
            // there is a 'previous one'. So lets compute differences
            lmDPitch dnPrev = aOnChordPitch[i-1].ToDPitch();
            if (nPoint - dnPrev < dnCur - nPoint)
                return aOnChordPitch[i-1];
            else
                return aOnChordPitch[i];
        }
    }

    //requested note is out of range. Return maximum allowed one
    //return aOnChordPitch[aOnChordPitch.size()-1];
    return aOnChordPitch[aOnChordPitch.size()-1];
}

void lmComposer6::InstantiateWithNote(lmScore* pScore, lmAPitch anPitch)
{
    // This method is used only to generate images for documentation.
    // The idea is to instantiate the score with the same pitch for all
    // notes, to create scores with the rhymth pattern before instatiation algorithm

    // Loop to instantiate notes
    lmInstrument* pInstr = pScore->GetFirstInstrument();
    lmVStaff* pVStaff = pInstr->GetVStaff();
    lmSOIterator* pIter = pVStaff->CreateIterator();
    while(!pIter->EndOfCollection())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        EStaffObjType nType = pSO->GetClass();
        if (nType == eSFOT_NoteRest) {
            if ( ((lmNoteRest*)pSO)->IsNote() )
            {
                // It is a note. Instantiate it
                ((lmNote*)pSO)->ChangePitch(anPitch, lmCHANGE_TIED);
            }
        }

        pIter->MoveNext();
    }
    delete pIter;

}


void lmComposer6::AssignNonChordNotes(int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
                                      lmNote* pNonChord[], lmAPitch aScale[7])
{
    // Receives the two on-chord notes and the non-chord notes between them, and assign
    // the pitch to all notes
    // The first on-chord note can be NULL (first anacruxis measure)
    // The number of non-chord notes received is in 'nNumNotes'


    //case: no non-chord notes. Nothing to do
    if (nNumNotes == 0) {
        g_pLogger->LogTrace(_T("lmComposer6::AssignNonChordNotes"), _T("[AssignNonChordNotes] No non-chord notes. Nothing to do."));
        return;
    }

    //case: anacruxis measure
    if (!pOnChord1) {
        g_pLogger->LogTrace(_T("lmComposer6::AssignNonChordNotes"), _T("[AssignNonChordNotes] Anacruxis measure"));
        //we are going to assing an ascending sequence, by step, to finish in the root
        //note (the first on chord note)
        if (nNumNotes == 1) {
            //assign root pitch
            pNonChord[0]->ChangePitch(pOnChord2->GetAPitch(), lmCHANGE_TIED);
        }
        else {
            // ascending sequence of steps
            lmAPitch nPitch = pOnChord2->GetAPitch();
            for(int i=nNumNotes-1; i >= 0; i--) {
                if (!pNonChord[i]->IsPitchDefined()) {
                    nPitch = MoveByStep(lmDOWN, nPitch, aScale);
                    pNonChord[i]->ChangePitch(nPitch, lmCHANGE_TIED);
                }
            }
        }
        return;
    }

    // Compute inteval formed by on-chord notes
    lmAPitch ap1 = pOnChord1->GetAPitch();
    lmAPitch ap2 = pOnChord2->GetAPitch();
    //lmFPitch fp1 = FPitch(ap1);
    //lmFPitch fp2 = FPitch(ap2);
    int nDIntval = ap2.ToDPitch() - ap1.ToDPitch();
    //int nFIntval = abs(fp2 - fp1);
    int nAbsIntval = abs(nDIntval) + 1;

    //Choose non-chord notes type depending on interval formed by on-chord notes
    if (nAbsIntval == 1)
    {
        //unison
        //If one or two notes use neighboring notes. If more notes
        //choose between neighboring notes or on-chord arpege
        if (nNumNotes < 3)
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        return;
    }

    else if (nAbsIntval == 2)
    {
        //second
        //If one note there are several possibilities (anticipation / suspension /
        //retardation / appogiatura) but I will just use a on-chord tone (a third apart)
        if (nNumNotes == 1)
            ThirdFifthNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        return;
    }

    else if (nAbsIntval == 3)
    {
        //third
        //If one note use a passing note, else we could use two passing notes by
        //chromatic step, but chromatic accidentals could not be appropriate for
        //lower music reading levels; therefore I will use a neighboring tone
        if (nNumNotes == 1)
            PassingNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        return;
    }

    else if (nAbsIntval == 4)
    {
        //fourth
        //interval 1:   Third / Appoggiatura
        //interval 2:   Two passing tones (by step) / Appoggiatura
        if (nNumNotes == 1)
            ThirdFifthNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else if (nNumNotes == 1)
            PassingNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        return;
    }

    else if (nAbsIntval == 5)
    {
        //fifth
        //interval 1:   Third / Appoggiatura
        //interval 2:   Third+Fifth / Double appoggiatura
        //interval 3:   three passing tones (by step)
        if (nNumNotes < 3)
            ThirdFifthNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else if (nNumNotes == 3)
            PassingNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        return;
    }

    else if (nAbsIntval == 6)
    {
        //sixth
        //interval 1:   Appoggiatura
        //interval 2:   Double appoggiatura
        //interval 4:   four passing tones (by step)
        if (nNumNotes < 3)
            ThirdFifthNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else if (nNumNotes == 4)
            PassingNotes((nDIntval > 0), nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        else
            NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        return;
    }

    else {
        wxLogMessage(_T("[lmComposer6::AssignNonChordNotes] Program error: ")
            _T("case not defined: Intval %d, num.notes %d"),
            nAbsIntval, nNumNotes );
        NeightboringNotes(nNumNotes, pOnChord1, pOnChord2, pNonChord, aScale);
        return;
    }

}

void lmComposer6::NeightboringNotes(int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
                                    lmNote* pNonChord[], lmAPitch aScale[7])
{
    // Receives the two on-chord notes and the non-chord notes between them, and assign
    // the pitch to all notes. The number of non-chord notes received is in 'nNumNotes'

    //wxASSERT(nNumNotes > 0 && nNumNotes < 4);
    //// 1, 2 or 3 neightboring notes

    bool fUpStep = lmRandomGenerator::FlipCoin();
    lmAPitch ap = pOnChord1->GetAPitch();
    //wxASSERT(ap.ToDPitch() != lmNO_NOTE);
    lmAPitch nFirstPitch = MoveByStep(fUpStep, ap, aScale);
    pNonChord[0]->ChangePitch(nFirstPitch, lmCHANGE_TIED);
    if (nNumNotes == 1) return;
    pNonChord[1]->ChangePitch(MoveByStep(!fUpStep, ap, aScale), lmCHANGE_TIED);
    if (nNumNotes == 2) return;
    pNonChord[2]->ChangePitch(nFirstPitch, lmCHANGE_TIED);
}

void lmComposer6::PassingNotes(bool fUp, int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
                               lmNote* pNonChord[], lmAPitch aScale[7])
{
    // Receives the two on-chord notes and the non-chord notes between them, and assign
    // the pitch to all notes. The number of non-chord notes received is in 'nNumNotes'

    wxASSERT(nNumNotes > 0);

    // passing note
    lmAPitch apNewPitch = pOnChord1->GetAPitch();
    pNonChord[0]->ChangePitch(MoveByStep(fUp, apNewPitch, aScale), lmCHANGE_TIED);

    // two passing notes
    for (int i=1; i < nNumNotes; i++) {
        apNewPitch = MoveByStep(fUp, apNewPitch, aScale);
        pNonChord[1]->ChangePitch(apNewPitch, lmCHANGE_TIED);
    }

}

void lmComposer6::ThirdFifthNotes(bool fUp, int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
                               lmNote* pNonChord[], lmAPitch aScale[7])
{
    // Receives the two on-chord notes and the non-chord notes between them, and assign
    // the pitch to all notes. The number of non-chord notes received is in 'nNumNotes'

    wxASSERT(nNumNotes == 1 || nNumNotes == 2);

    // third
    lmAPitch apPitch = MoveByStep(fUp, pOnChord1->GetAPitch(), aScale);  //second
    apPitch = MoveByStep(fUp, apPitch, aScale);     //third
    pNonChord[0]->ChangePitch(apPitch, lmCHANGE_TIED);
    if (nNumNotes == 1) return;
    // fifth
    apPitch = MoveByStep(fUp, apPitch, aScale);     //fourth
    apPitch = MoveByStep(fUp, apPitch, aScale);     //fifth
    pNonChord[1]->ChangePitch(apPitch, lmCHANGE_TIED);

}
