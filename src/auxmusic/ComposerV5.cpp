//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ComposerV5.h"
#endif

// For compilers that support precompilation, includes "wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/arrstr.h"      // to use wxArrayString


#include "../score/Score.h"
#include "../ldp_parser/LDPParser.h"
#include "ComposerV5.h"
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

    lmROOT_POSITION     = 0x00000008,
    lmFIRST_INVERSION   = 0x00000010,
    lmSECOND_INVERSION  = 0x00000020,
    lmTHIRD_INVERSION   = 0x00000040,

    lmSEVENTH_ADDED     = 0x00000100,

    // masks
    lmGRADE_MASK        = 0x00000007,   // to extract grade
};



lmComposer5::lmComposer5() : lmComposer()

{
    m_nNumNotes = 0;

}

lmComposer5::~lmComposer5()
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
lmScore* lmComposer5::GenerateScore(lmScoreConstrains* pConstrains)
{
    //Save parameters
    m_pConstrains = pConstrains;

    //Generate a random key, time signature and clef satisfying the constraints
    lmRandomGenerator oGenerator;
    m_nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    m_nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    m_nTimeSign = oGenerator.GenerateTimeSign(m_pConstrains->GetTimeSignConstrains());


    // prepare and initialize the score
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmScore* pScore = new lmScore();
    pScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );            //5mm
    pScore->AddInstrument(1, g_pMidi->DefaultVoiceChannel(),
   						  g_pMidi->DefaultVoiceInstr(), _T(""));
    lmVStaff *pVStaff = pScore->GetVStaff(1, 1);   //get first vstaff of instr.1
    pVStaff->AddClef( m_nClef );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature( m_nTimeSign );

    //
    // Content generation
    //
    ChooseRangeOfNotes();

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
        //! @todo error logging. Suppress message
        wxMessageBox(_("[lmComposer5::GenerateScore] No usable fragments!"));
        return pScore;
    }

    //chose ramdomly a fragment satisfying the constraints, and take the first segment
    pConstrains->ChooseRandomFragment();
    pSegment = pConstrains->GetNextSegment();
    //! @todo what if no fragment satisfies the constraints?

    int nSegmentLoopCounter = 0;
    while (nNumMeasures < nMeasuresToGenerate) {
        //If no measure is opened start a new measure
        if (!fMeasure) {
            sMeasure = wxString::Format(_T("(c %d "), nNumMeasures+1);
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
        while (pSegment && rOccupiedDuration < rMeasureDuration) {
            //check if segment fits in. A segment S will fit in the measure
            //only when (tr >= ts && tcb <= tab)

            rTimeRemaining =rMeasureDuration - rOccupiedDuration;
            rSegmentDuration = pSegment->GetSegmentDuration();
            rConsumedBeatTime = rOccupiedDuration;  //this line and next two ones compute tcb = tc % tb;
            while (rConsumedBeatTime > 0.0) rConsumedBeatTime -= rBeatDuration;
            if (rConsumedBeatTime < 0.0) rConsumedBeatTime += rBeatDuration;
            rSegmentAlignBeatTime = pSegment->GetTimeAlignBeat();
            fFits = (rTimeRemaining >= rSegmentDuration && rConsumedBeatTime <= rSegmentAlignBeatTime);

            g_pLogger->LogTrace(_T("lmComposer5"), _T("[GenerateScore] sMeasure=%s, pSegment=%s, tr=%.2f, ts=%.2f, tcb=%.2f, tab=%.2f, tc=%.2f, tb=%.2f, fits=%s"),
                    sMeasure.c_str(),
                    (pSegment->GetSource()).c_str(), rTimeRemaining, rSegmentDuration,
                    rConsumedBeatTime, rSegmentAlignBeatTime,
                    rOccupiedDuration, rBeatDuration,
                    (fFits ? _T("yes") : _T("no")) );

            //if segment fits add it to current measure
            if (fFits) {
                //it fits. Add it to current measure
                float rNoteTime = rSegmentAlignBeatTime - rConsumedBeatTime;
                if (rNoteTime > 0.0) {
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

            // increment measures counter
            nNumMeasures++;

            // Instantiate the notes by assigning note pitches and add
            // the measure to the score
            g_pLogger->LogTrace(_T("lmComposer5"),
                    _T("[GenerateScore] Adding measure = '%s')"), sMeasure.c_str());
            pNode = parserLDP.ParseText( InstantiateNotes(sMeasure) );
            parserLDP.AnalyzeMeasure(pNode, pVStaff);
        }

    }

    // In Music Reading, level 1, introduction lessons use only quarter notes. In those
    // exercises we should not use half notes in the last measure. So lets check if
    // only quarter notes are used in the composed piece of music.
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    bool fOnlyQuarterNotes = true;
    lmStaffObjIterator* pIter = pVStaff->CreateIterator(eTR_AsStored);
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_NoteRest) {
            pNR = (lmNoteRest*)pSO;
            if (!pNR->IsRest()) {
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
    g_pLogger->LogTrace(_T("lmComposer5"),
            _T("[GenerateScore] fOnlyQuarterNotes=%s)"),
            (fOnlyQuarterNotes ? _T("True") : _T("False")) );

    // add a final measure with a root pitch note lasting, at least, one beat
    sMeasure = CreateLastMeasure(++nNumMeasures, m_nTimeSign, fOnlyQuarterNotes);
    g_pLogger->LogTrace(_T("lmComposer5"),
            _T("[GenerateScore] Adding final measure = '%s')"), sMeasure.c_str());
    pNode = parserLDP.ParseText( InstantiateNotes(sMeasure, true) );
    parserLDP.AnalyzeMeasure(pNode, pVStaff);

    // done
    //pScore->Dump(_T("lenus_score_dump.txt"));
    return pScore;

}

void lmComposer5::ChooseRangeOfNotes()
{
    m_nNumNotes = 0;    // the next note to generate will be the first one
    m_fTied = false;    // no tie yet

    //set the minimum note and the range
    wxString sMinPitch = (m_pConstrains->GetClefConstrains())->GetLowerPitch(m_nClef);
    wxString sMaxPitch = (m_pConstrains->GetClefConstrains())->GetUpperPitch(m_nClef);
    EAccidentals nAccidentals;
    PitchNameToData(sMinPitch, &m_minPitch, &nAccidentals);
    PitchNameToData(sMaxPitch, &m_maxPitch, &nAccidentals);
}

wxString lmComposer5::GenerateNewNote(bool fRepeat, bool fRootPitch)
{

    lmPitch newPitch;
    lmRandomGenerator oGenerator;

    if (m_nNumNotes == 0 || fRootPitch) {
        // First note always the root note
        newPitch = RootNote(m_nKey);
        // ensure it is in range minPitch to maxPitch
        if (newPitch > m_maxPitch) {
            while (newPitch > m_maxPitch) newPitch -= 7;    // go down one octave
            if (newPitch < m_minPitch)  newPitch = oGenerator.RandomNumber(m_minPitch, m_maxPitch);
        }
        else if (newPitch < m_minPitch) {
            while (newPitch < m_minPitch) newPitch += 7;    // go up one octave
            if (newPitch > m_maxPitch)  newPitch = oGenerator.RandomNumber(m_minPitch, m_maxPitch);
        }
    }
    else if (fRepeat) {
        newPitch = m_lastPitch;
    }
    else {
        //int nStep = oGenerator.RandomNumber(1, m_pConstrains->GetMaxInterval());
        //bool fUp;
        //if (m_lastPitch < m_minPitch + 3)
        //    fUp = true;
        //else if (m_lastPitch > m_maxPitch - 3)
        //    fUp = false;
        //else
        //    fUp = oGenerator.FlipCoin();

        //newPitch = m_lastPitch + (fUp ? nStep : -nStep);
        //if (newPitch > m_maxPitch) newPitch = m_maxPitch;
        //else if (newPitch < m_minPitch) newPitch = m_minPitch;

        int nRange = m_pConstrains->GetMaxInterval();
        int lowLimit = wxMax(m_lastPitch - nRange, m_minPitch);
        int upperLimit = wxMin(m_lastPitch + nRange, m_maxPitch);
        newPitch = oGenerator.RandomNumber(lowLimit, upperLimit);
    }

    // save values
    m_nNumNotes++;
    m_lastPitch = newPitch;

    lmConverter oConverter;
    return oConverter.PitchToLDPName(newPitch);

}


lmPitch lmComposer5::RootNote(EKeySignatures nKey)
{
    // returns the pitch of root note (in octave 4) for the given key signature.
    // For example, for C major returns 29 (c4); for A sharp minor returns 34 (a4).

    switch(nKey) {
        case earmDo:
        case earmDom:
        case earmDosm:
        case earmDos:
        case earmDob:
            return lmC4PITCH;

        case earmRe:
        case earmReb:
        case earmResm:
        case earmRem:
            return lmC4PITCH+1;

        case earmMi:
        case earmMim:
        case earmMib:
        case earmMibm:
            return lmC4PITCH+2;

        case earmFa:
        case earmFasm:
        case earmFas:
        case earmFam:
            return lmC4PITCH+3;

        case earmSol:
        case earmSolsm:
        case earmSolm:
        case earmSolb:
            return lmC4PITCH+4;

        case earmLa:
        case earmLam:
        case earmLasm:
        case earmLab:
        case earmLabm:
            return lmC4PITCH+5;

        case earmSim:
        case earmSi:
        case earmSib:
        case earmSibm:
            return lmC4PITCH+6;

        default:
            return lmC4PITCH;
    }
}

wxString lmComposer5::InstantiateNotes(wxString sPattern, bool fRootPitch)
{
    // it is assumed that the pattern is normalized (lower case, no extra spaces).
    // if fRootPitch the firts note will be root pitch

    wxString sSource = sPattern;
    wxString sResult = _T("");
    bool fRepeat, fFirstNote=true;
    bool fRoot = fRootPitch;

    int i = sSource.Find(_T("*"));
    int j = sSource.Find(_T(" l"));
    while (i != -1) {
        sResult += sSource.Mid(0, i);
        fRepeat = (fFirstNote ? m_fTied : (j != -1 && j < i) );
        sResult += GenerateNewNote(fRepeat, fRoot);
        sSource = sSource.Mid(i + 1);
        i = sSource.Find(_T("*"));
        j = sSource.Find(_T(" l"));
        fFirstNote = false;
        fRoot = false;
    }
    sResult += sSource;
    m_fTied = (j != -1);

    return sResult;

}

void lmComposer5::AddSegment(wxString* pMeasure, lmSegmentEntry* pSegment, float rNoteTime)
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
    g_pLogger->LogTrace(_T("lmComposer5"), _T("[AddSegment] Adding segment %s, duration=%.2f"),
        (pSegment->GetSource()).c_str(), pSegment->GetSegmentDuration());
    *pMeasure += pSegment->GetSource();

}

wxString lmComposer5::CreateNoteRest(int nNoteRestDuration, bool fNote)
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
    g_pLogger->LogTrace(_T("lmComposer5"), _T("[CreateNoteRest] Needed duration= %d, added=%s"),
        nNoteRestDuration, sElement.c_str());
    return sElement;

}

wxString lmComposer5::CreateLastMeasure(int nNumMeasure, ETimeSignature nTimeSign,
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

