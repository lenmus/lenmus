//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

#include "lenmus_standard_header.h"
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "Harmony.h"
//#endif
//
//// for (compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "Harmony.h"
//#include "../score/VStaff.h"
//
//
//namespace lenmus
//{
//
//#include "../app/MainFrame.h"
//extern lmMainFrame* GetMainFrame();
//#include "../app/ScoreDoc.h"  // DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
//
//
////
//// Global functions
////
//
//// return: total number of errors
//int AnalyzeHarmonicProgression(lmScoreChord** pChordDescriptor, int nNCH, ChordInfoBox* pChordErrorBox)
//{
//    wxLogMessage("*** AnalyzeHarmonicProgression N:%d ", nNCH);
//    for (int i = 0; i<nNCH; i++)
//    {
//        wxLogMessage("   Chord %d to analyze: %s", i, pChordDescriptor[i]->ToString().wx_str());
//    }
//
//    int nNumChordError[lmMAX_NUM_CHORDS]; // number of errors in each chord
//
//    lmRuleList tRules(pChordDescriptor, nNCH);
//
//    wxString sStr;
//    sStr.clear();
//
//    int nNumErros = 0; // TODO: decide: total num of errors or num of chords with error
//    lmRule* pRule;
//    // TODO: create a method of the list to evaluate ALL the rules ?
//    for (int nR = lmCVR_FirstChordValidationRule; nR<lmCVR_LastChordValidationRule; nR++)
//    {
//        pRule = tRules.GetRule(nR);
//        if ( pRule == nullptr)
//        {
////todo: remove this message?            wxLogMessage(" Rule %d is nullptr !!!", nR);
//        }
//        else if (pRule->IsEnabled())
//        {
//            wxLogMessage("Evaluating rule %d, description: %s"
//                , pRule->GetRuleId(), pRule->GetDescription().wx_str());
//            nNumErros += pRule->Evaluate(sStr, &nNumChordError[0], pChordErrorBox);
//            wxLogMessage("   Total error count after rule %d: %d errors", pRule->GetRuleId(), nNumErros   );
//            if (nNumErros > 0)
//            {
//              // TODO: anything else here?
//            }
//        }
//    }
//
//    return nNumErros;
//}
//
////  Harmonic direction of an interval:
////   descending (-1) when the interval is negative
////   ascending (1) when positive
////    linear when 0
//int GetHarmonicDirection(int nInterval)
//{
//    if (nInterval > 0)
//        return 1;
//    else  if (nInterval < 0)
//        return -1;
//    else
//        return 0;
//}
//
//
////  Harmonic motion of 2 voices: calculated from the harmonic direction of each voice
////    parallel: both voices have the same direction
////    contrary: both voices have opposite direction
////    oblique: one direction is linear and the other is not
//int GetHarmonicMovementType( FPitch fVoice10, FPitch fVoice11, FPitch fVoice20, FPitch fVoice21)
//{
//    int nMovType = -10;
//
//    int nD1 = GetHarmonicDirection(fVoice11 - fVoice10);
//    int nD2 = GetHarmonicDirection(fVoice21 - fVoice20);
//
//    if (nD1 == nD2)
//    {
//        nMovType =  lm_eDirectMovement;
//    }
//    else if (nD1 == -nD2)
//    {
//        nMovType = lm_eContraryMovement;
//    }
//    else
//    {
//        assert ( (nD1 == 0 && nD2 != 0)  ||  (nD2 == 0 && nD1 != 0) );
//        nMovType = lm_eObliqueMovement;
//    }
//    return nMovType;
//}
//
//int GetIntervalNumberFromFPitchDistance(FPitch n2, FPitch n1) //@@@ todo remove!!!
//{
//    FIntval nDistance  = abs (n2 - n1);
//    int nIntervalNumber  = FIntval_GetNumber(nDistance);
//    wxLogMessage(_T("\t\t GetIntervalNumberFromFPitchDistance: %d-%d D:%d I:%d ")
//        , n2, n1, nDistance, nIntervalNumber);
//    return nIntervalNumber;
//}
//
//// todo: move this to "Pitch" file o  merge this with FPitch_ToAbsLDPName
//// This is just FPitch_ToAbsLDPName but WITHOUT OCTAVE
//static  wxString m_sNoteName[7] = {
//            "c",  "d", "e", "f", "g", "a", "b" };
//wxString NormalizedFPitch_ToAbsLDPName(FPitch fp)
//{
//    wxString sAnswer;
//    switch(FPitch_Accidentals(fp)) {
//        case -2: sAnswer ="--"; break;
//        case -1: sAnswer ="-"; break;
//        case 0:  sAnswer =""; break;
//        case 1:  sAnswer ="+"; break;
//        case 2:  sAnswer ="x"; break;
//        default:
//            return wxEmptyString;
//    }
//    sAnswer += m_sNoteName[FPitch_Step(fp)];
//    return sAnswer;
//}
//
//static wxString m_sNumeralsDegrees[7] =
//        {" I", " II", "III", " IV", "  V", " VI", "VII"};
//
//wxString GetChordDegreeString(StepType nStep )
//{
//    if (nStep < lmMIN_STEP ||  nStep > lmMAX_STEP)
//    {
//        wxLogMessage("GetDegreeString: Invalid step %d", nStep);
//        nStep = 0;
//    }
//
//    return m_sNumeralsDegrees[nStep];
//}
//
//static wxString m_sFourVoiceNames[4] =
//        {"Soprano", "Alto", "Tenor", "Bass"};
//// return the voice name. Range 1 (soprano) to 4 (bass)
//wxString Get4VoiceName(int nVoice )
//{
//    const int nBassVoice = 4; // AWARE: BASS IS VOICE 4!!!!
//    const int nSopranoVoice = 1;
//    static wxString sUnknownVoice;
//    if (nVoice < nSopranoVoice ||  nVoice > nBassVoice)
//    {
//        sUnknownVoice = wxString::Format(
//            "Unknown voice %d; min:%d, max:%d"
//                , nVoice, nSopranoVoice, nBassVoice);
//
//        wxLogMessage("Get4VoiceName: Invalid voice %d", nVoice);
//        return sUnknownVoice;
//    }
//    else
//        return m_sFourVoiceNames[nVoice-1];
//}
//
//
//
////  Get interval in FPitch from:
////   chord degree (root note step)
////   key signature
////   interval index (1=3rd, 2=5th, etc)
////  TODO: Used in harmony, but if it useful in general, move it to a better place such as Pitch file
//FIntval FPitchInterval(int nRootStep, EKeySignature nKey, int nInterval)
//{
//    // steps: 0 .. 6 (k_step_C .. k_step_B)
//    assert (nRootStep>=k_step_C && nRootStep <= k_step_B);
//
//    // aware: in harmony by default an interval has 2 steps, therefore step2 = step1 + 2*N
//    FPitch fpPitch = FPitchStepsInterval(nRootStep, (nRootStep+(nInterval*2))%(k_step_B+1), nKey);
//    return (FIntval) fpPitch;
//}
//
////-----------------------------------------------------------------------------------
//
//
// void SortChordNotes(int nNumNotes, ImoNote** pInpChordNotes)
//{
//    // todo remove? wxASSERT(nNumNotes < k_notes_in_chord);
//    // Classic Bubble sort
//    int nCount, fSwapDone;
//    ImoNote* auxNote;
//    do
//    {
//        fSwapDone = 0;
//        for (nCount = 0; nCount < nNumNotes - 1; nCount++)
//        {
//            wxASSERT(pInpChordNotes[nCount] != nullptr);
//            wxASSERT(pInpChordNotes[nCount+1] != nullptr);
//            if (pInpChordNotes[nCount]->GetFPitch() > pInpChordNotes[nCount+1]->GetFPitch() )
//            {
//	            auxNote = pInpChordNotes[nCount];
//	            pInpChordNotes[nCount] = pInpChordNotes[nCount + 1];
//	            pInpChordNotes[nCount + 1] = auxNote;
//	            fSwapDone = 1;
//            }
//        }
//    }while (fSwapDone);
//}
//
//void SortChordNotes(int nNumNotes, FPitch fInpChordNotes[])
//{
//    // todo remove? wxASSERT(nNumNotes < k_notes_in_chord);
//    wxASSERT(fInpChordNotes != nullptr);
//    // Classic Bubble sort
//    int nCount, fSwapDone;
//    FPitch auxNote;
//    do
//    {
//        fSwapDone = 0;
//        for (nCount = 0; nCount < nNumNotes - 1; nCount++)
//        {
//            if (fInpChordNotes[nCount] > fInpChordNotes[nCount+1] )
//            {
//	            auxNote = fInpChordNotes[nCount];
//	            fInpChordNotes[nCount] = fInpChordNotes[nCount + 1];
//	            fInpChordNotes[nCount + 1] = auxNote;
//	            fSwapDone = 1;
//            }
//        }
//    }while (fSwapDone);
//}
//
//#if 0
///*
// This method is not used anymore. lmChordInfo has evolved to Chord and ChordIntervals, which
//    use a diferent approach to extract the intervals.
//    But the process of obtaining the intervals from the notes is essential in chord processing,
//    therefore, we leave this method as an example of to do it.
// */
//void GetIntervalsFromNotes(int nNumNotes, ImoNote** pInpChordNotes, lmChordInfo* tOutChordInfo)
//{
//    wxASSERT(nNumNotes < k_notes_in_chord);
//    wxASSERT(pInpChordNotes != nullptr);
//    wxASSERT(tOutChordInfo != nullptr);
//    wxASSERT(pInpChordNotes[0] != nullptr);
//
//    FIntval fpIntv;
//    int nCurrentIntvIndex = 0;
//    int nExistingIntvIndex = 0;
//    for (int nCount = 1; nCount < nNumNotes; nCount++)
//    {
//        wxASSERT(pInpChordNotes[nCount] != nullptr);
//        fpIntv = (FIntval) (pInpChordNotes[nCount]->GetFPitch() - pInpChordNotes[0]->GetFPitch());
//
//        if (fpIntv >= k_interval_p8)
//        {
//              fpIntv = fpIntv % k_interval_p8;
//        }
//#if (LENMUS_DEBUG_BUILD == 1)
//        wxLogMessage("[GetIntervalsFromNotes note %d: %d  note 0: %d] INTERVAL: %d"
//            , nCount, pInpChordNotes[nCount]->GetFPitch(), pInpChordNotes[0]->GetFPitch(), fpIntv);
//#endif
//        // Update chord interval information
//        for (nExistingIntvIndex=0; nExistingIntvIndex<nCurrentIntvIndex; nExistingIntvIndex++)
//        {
//            if (tOutChordInfo->nIntervals[nExistingIntvIndex] == fpIntv)
//                break; // interval already exists
//        }
//        if (nExistingIntvIndex < nCurrentIntvIndex)
//        {
//            wxLogMessage(" Interval %d: IGNORED, already in %d"
//            , fpIntv, nExistingIntvIndex);
//        }
//        else
//        {
//            if (fpIntv == 0)
//            {
//                // Ignored 0 Interval
//            }
//            else
//            {
//                // Add interval
//                tOutChordInfo->nIntervals[nCurrentIntvIndex] = fpIntv;
//                nCurrentIntvIndex++;
//            }
//        }
//    }
//
//    tOutChordInfo->nNumNotes = nNumNotes;
//    tOutChordInfo->nNumIntervals = nCurrentIntvIndex;
//
//    // Sort Intervals
//    //  Classic bubble sort
//    int fSwapDone;
//    FIntval auxIntv;
//    do
//    {
//        fSwapDone = 0;
//        for (int nCount = 0; nCount < tOutChordInfo->nNumIntervals - 1; nCount++)
//        {
//            if (tOutChordInfo->nIntervals[nCount] > tOutChordInfo->nIntervals[nCount+1] )
//            {
//                auxIntv = tOutChordInfo->nIntervals[nCount];
//                tOutChordInfo->nIntervals[nCount] = tOutChordInfo->nIntervals[nCount + 1];
//                tOutChordInfo->nIntervals[nCount + 1] = auxIntv;
//                fSwapDone = 1;
//            }
//        }
//    }while (fSwapDone);
//
//
//    // Set the non-used intervals to  nullptr
//    for (int i=tOutChordInfo->nNumIntervals; i<k_intervals_in_chord; i++)
//    {
//      tOutChordInfo->nIntervals[i] = lmNULL_FIntval;
//    }
//
//}
//#endif
//
//
////
//// Message box to display the results if the chord analysis
////
//// Remember:
////      x: relative to object; positive: right
////      y: relative to top line; positive: down
//ChordInfoBox::ChordInfoBox(wxSize* pSize, lmFontInfo* pFontInfo
//                           , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement)
//{
//    Settings(pSize, pFontInfo, nBoxX, nBoxY, nLineX, nLineY, nBoxYIncrement);
//}
//void ChordInfoBox::Settings(wxSize* pSize, lmFontInfo* pFontInfo
//                            , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement)
//{
//    m_ntConstBoxXstart = nBoxX;
//    m_ntConstInitialBoxYStart = nBoxY;
//    m_ntConstLineXstart = nLineX;
//    m_ntConstLineYStart = nLineY;
//    m_ntConstBoxYIncrement = nBoxYIncrement;
//    m_pFontInfo = pFontInfo;
//    m_pSize = pSize;
//
//    assert(m_pFontInfo != nullptr);
//    assert(m_pSize != nullptr);
//
//    m_ntCurrentBoxYStart = m_ntConstInitialBoxYStart;
//}
//void ChordInfoBox::ResetPosition()
//{
//    m_ntCurrentBoxYStart = m_ntConstInitialBoxYStart;
//}
//void ChordInfoBox::SetYPosition(int nYpos)
//{
//    m_ntCurrentBoxYStart = nYpos;
//}
//void ChordInfoBox::DisplayChordInfo(lmScore* pScore, lmScoreChord* pChordDsct, wxColour colour, wxString &sText)
//{
//    if (pChordDsct == nullptr )
//    {
//        wxLogMessage(
//            " DisplayChordInfo ERROR: Chord descriptor is nullptr. Msg: %s"
//            , sText.wx_str());
//        return;  // todo: improvement: in this case, display a box but not attached to any note ?
//    }
//    if ( pChordDsct->get_num_notes() < 1)
//    {
//        wxLogMessage(" DisplayChordInfo ERROR: NO notes to attach the textbox");
//        return;  // todo: improvement: in this case, display a box but not attached to any note ?
//    }
//
//    int m_nNumChordNotes  = pChordDsct->get_num_notes();
//    if ( ! pChordDsct->HasLmNotes())
//    {
//        wxLogMessage(" DisplayChordInfo ERROR: NO score notes!");
//        return;
//    }
//    lmTextStyle* pStyle = pScore->GetStyleName(*m_pFontInfo);
//
//    // Display chord info in score with a line and text
//    assert(m_nNumChordNotes > 0);
//    assert(m_nNumChordNotes < 20);
//
///* todo remove    for (int i = 0; i<m_nNumChordNotes; i++)
//    {
//        assert(pChordDsct->GetNoteLmNote(i) != nullptr);
//        pChordDsct->GetNoteLmNote(i)->SetColour(colour);
//    } */
//
//
//    // Line end: the first note
//    lmStaffObj* cpSO = pChordDsct->GetNoteLmNote(m_nNumChordNotes-1);
//    lmTPoint lmTBoxPos(m_ntConstBoxXstart, m_ntCurrentBoxYStart);
//    lmTPoint lmTLinePos(m_ntConstLineXstart, m_ntConstLineYStart);
//    lmAuxObj* pTxtBox = cpSO->AttachTextBox(lmTBoxPos, lmTLinePos, sText, pStyle, *m_pSize, colour);
//
//    // here increment the static variables
//    m_ntCurrentBoxYStart += m_ntConstBoxYIncrement;
//}
//
//
//void DrawArrow(ImoNote* pNote1, ImoNote* pNote2, wxColour color)
//{
//    //get VStaff
//    lmVStaff* pVStaff = pNote1->GetVStaff();
//
//    //get note heads positions
//    lmURect uBounds1 = pNote1->GetNoteheadShape()->GetBounds();
//    lmURect uBounds2 = pNote2->GetNoteheadShape()->GetBounds();
//
//    //start point
//    lmUPoint uStart( uBounds1.GetWidth(), 0);
//    uStart.y = pNote1->GetShiftToNotehead();        //center of notehead
//
//    //end point
//    lmUPoint uEnd(uBounds2.GetRightTop() - uBounds1.GetRightTop());
//    uEnd.y += uStart.y;
//
//    //convert to tenths
//    lmTenths xtStart = pVStaff->LogicalToTenths(uStart.x) + 8.0;
//    lmTenths ytStart = pVStaff->LogicalToTenths(uStart.y);
//    lmTenths xtEnd = pVStaff->LogicalToTenths(uEnd.x) - 8.0;
//    lmTenths ytEnd = pVStaff->LogicalToTenths(uEnd.y);
//
//    //create arrow
//    pNote1->AttachLine(xtStart, ytStart, xtEnd, ytEnd, 2, lm_eLineCap_None,
//                       lm_eLineCap_Arrowhead, lm_eLine_Solid, color);
//   /* todo remove
//    pNote1->SetColour(color);
//    pNote2->SetColour(color); --*/
//}
//
//
///* AWARE:
//
//  Chord is the basic chord, defined by ROOT NOTE and intervals
//      aware: only the root note is REAL; the rest of notes, obtained with GetNote(i) (where i>0)
//        are just POSSIBLE notes
//
//    for certain operations we need to know all the real notes of the chord
//
//  lmFPitchChord is a Chord that can contain also notes in "FPitch".
//  The FPitch notes can be added after the construction, except the root note (bass voice) that is already in Chord
//    remember: the ROOT NOTE is REQUIRED in ANY chord;
//      it is defined in the construction and should never be modified afterwards
//
//
//    Possible constructors :
//      1 created with NO notes; then added with AddNoteFromLmNote or AddNoteLmFPitch
//	      GetNoteFpitch returns a 0 if the note has not been added
//      2 created with notes. NO POSSIBLE TO ADD NOTES.  [controlled with m_fCreatedWithNotes]
//    check the number of notes available with get_num_notes()
//	Notes can not be removed nor modified
//
//  lmScoreChord is a 'real score chord': it contains actual notes of the score (lmNotes)
//   it is a lmFPitchChord that can contain also notes in "lmNotes"
//
//   The lmNotes notes can be added after the construction
//
//    aware: the root note might NOT be present. The root note is alway present in FPitch, but it may not have
//            a corresponding ImoNote.
//
//    Possible constructors :
//      1 created with NO notes; then
//             added with AddNoteFromLmNote
//			    adds both FPitch and lmNotes
//             added ONLY FPitch with AddNoteLmFPitch or AddNoteFromInterval
//			   it only adds  FPitch !!!
//			      To add the missing ImoNote, use SetLmNote
//			         since the FPitch of this note must exist
//  					    it must be the same as the note->GetFPitch(). This is checked.
//	      GetNoteFpitch returns a 0 if the note has not been added
//      2 created with notes. NO POSSIBLE TO ADD NOTES.
//*/
////
//// class lmFPitchChord
////
//
//// Constructors
//
//// Create a chord from a list of ORDERED score notes in LmFPitch
//lmFPitchChord::lmFPitchChord(int nNumNotes, FPitch fNotes[], EKeySignature nKey)
//    : Chord(nNumNotes, fNotes, nKey)
//{
//    assert(nNumNotes < k_notes_in_chord);
//    for (int i = 0; i<nNumNotes; i++)
//    {
//        assert( IsValidChordNote(fNotes[i]) );
//        if (i == 0)
//        {
//            assert( (fNotes[0] % k_interval_p8) == GetNormalizedBass() );
//        }
//        m_fpChordNotes[i] = fNotes[i];
//    }
//    for (int i = nNumNotes; i<k_notes_in_chord; i++)
//    {
//        m_fpChordNotes[i] = 0;
//    }
//    m_nNumChordNotes = nNumNotes;
//
//    m_fCreatedWithNotes = true;
//}
//
//lmFPitchChord::lmFPitchChord(int nNumNotes, ImoNote** pNotes, EKeySignature nKey)
//    : Chord(nNumNotes, pNotes, nKey)
//{
//    assert(nNumNotes < k_notes_in_chord);
//    for (int i = 0; i<nNumNotes; i++)
//    {
//        assert( IsValidChordNote(pNotes[i]->GetFPitch()) );
//        if (i == 0)
//        {
//            assert( (pNotes[0]->GetFPitch() % k_interval_p8) == GetNormalizedBass() );
//            // actual bass note must be consistent with chord bass note
//        }
//        m_fpChordNotes[i] = pNotes[i]->GetFPitch();
//    }
//    for (int i = nNumNotes; i<k_notes_in_chord; i++)
//    {
//        m_fpChordNotes[i] = 0;
//    }
//    m_nNumChordNotes = nNumNotes;
//    m_fCreatedWithNotes = true;
//}
//
//// Creates a chord from "essential" information
//lmFPitchChord::lmFPitchChord(int nDegree, EKeySignature nKey, int nNumIntervals, int nNumInversions, int octave)
//    : Chord(nDegree, nKey, nNumIntervals, nNumInversions, octave)
//{
//    m_nNumChordNotes = 0;
//    m_fCreatedWithNotes = false;
//}
//
//// todo: consider to implement IsEqualTo in lmFPitchChord as
////         Chord::IsEqualTo + notes comparison
////        The question is:
////        Are two chords equal if both are of the same type but have different notes?
//
//int  lmFPitchChord::AddNoteLmFPitch(FPitch fNote)
//{
//    if (m_fCreatedWithNotes)
//    {
//        wxLogMessage(" lmFPitchChord::AddNoteLmFPitch ERROR, it was created with %d notes "
//            , m_nNumChordNotes );
//    }
//    else
//    {
//        assert(m_nNumChordNotes<k_notes_in_chord);
//        if ( this->IsValidChordNote(fNote) )
//        {
//            m_fpChordNotes[m_nNumChordNotes] = fNote;
//            m_nNumChordNotes++;
//            SortChordNotes(m_nNumChordNotes, m_fpChordNotes); // sort notes so that voice <=> index
//        }
//        else
//        {
//            wxLogMessage(" lmFPitchChord::AddNoteLmFPitch ERROR note %d [%s] does not belong to chord {%s}"
//                ,fNote,FPitch_ToAbsLDPName(fNote).wx_str() , this->ToString().wx_str()  );
//        }
//    }
//    return m_nNumChordNotes;
//}
//
//
//bool lmFPitchChord::IsBassDuplicated()
//{
//    // remember: the lowest note is the BASS note, not the root note
//    //           (bass == root only if NO INVERSIONS)
//
//    // Normalize wit "% k_interval_p8" to remove octave information
//    for (int i=1; i<m_nNumChordNotes; i++)
//    {
//        if ( (m_fpChordNotes[i] % k_interval_p8) == this->GetNormalizedBass() )
//            return true;
//    }
//    return false;
//}
//
//
//void lmFPitchChord::RemoveAllNotes()
//{
//    m_nNumChordNotes = 0;
//    for (int i = 0; i<k_notes_in_chord; i++)
//    {
//        m_fpChordNotes[i] = 0;
//    }
//}
//
//
//wxString lmFPitchChord::ToString()
//{
//    // extend the parent information
//    wxString sStr = this->Chord::ToString();
//    sStr += "; Notes:";
//    for (int nN = 0; nN<m_nNumChordNotes; nN++)
//    {
//        sStr += " ";
//        sStr += FPitch_ToAbsLDPName(m_fpChordNotes[nN]).wx_str();
//    }
//    return sStr;
//}
//
//
//
////
//// class lmScoreChord
////
//
//// Creates a chord from a list of ordered score notes
//lmScoreChord::lmScoreChord(int nNumNotes, ImoNote** pNotes, EKeySignature nKey)
//    : lmFPitchChord(nNumNotes, pNotes, nKey)
//{
//    //   Whenever a ImoNote is added, it should be checked that
//    //  - this note matches the corresponding in FPitch (m_fpChordNotes)
//    //  - the note is valid: it can be obtained from the bass note by adding an interval and +-octaves
//    m_nNumLmNotes = 0;
//    assert(nNumNotes < k_notes_in_chord);
//    for (int i = 0; i<nNumNotes; i++)
//    {
//        assert( pNotes[i]->GetFPitch() == m_fpChordNotes[i] );
//        if (i == 0)
//        {
//            // ENSURE THE FPitch of the note is the same as pNote-> GetFPitch()
//            assert( (pNotes[0]->GetFPitch() % k_interval_p8) == GetNormalizedBass() );
//            // actual bass note must be consistent with chord bass note
//        }
//        m_pChordNotes[i] = pNotes[i];
//        m_nNumLmNotes++;
//    }
//    for (int i = nNumNotes; i<k_notes_in_chord; i++)
//    {
//        m_pChordNotes[i] = 0;
//    }
//
//    tChordErrors.nErrList = 0;
//}
//
//lmScoreChord::lmScoreChord(int nDegree, EKeySignature nKey, int nNumIntervals, int nNumInversions, int octave)
//    : lmFPitchChord(nDegree, nKey, nNumIntervals, nNumInversions, octave)
//{
//    m_nNumLmNotes = 0;
//    for (int i = 0; i<k_notes_in_chord; i++)
//    {
//        m_pChordNotes[i] = 0;
//    }
//    tChordErrors.nErrList = 0;
//}
//
//
//// aware: this is only to associate the ImoNote to a note in FPitch that already exists
////   it is not to add a note!
//bool lmScoreChord::SetLmNote(ImoNote* pNote)
//{
//    assert(pNote);
//
//    for (int nIndex = 0; nIndex < k_notes_in_chord; nIndex++)
//    {
//        if (m_fpChordNotes[nIndex] == pNote->GetFPitch())
//        {
//            m_pChordNotes[nIndex] = pNote;
//            m_nNumLmNotes++;
//            wxLogMessage(" SetLmNote %d %d OK, total LmNotes:%d", nIndex, m_fpChordNotes[nIndex], m_nNumLmNotes);
//            return true;
//        }
//    }
//    wxLogMessage(" SetLmNote ERROR!! %d (%s) , not found in %d notes"
//        , pNote->GetFPitch(), FPitch_ToAbsLDPName(pNote->GetFPitch()).wx_str(), m_nNumChordNotes);
//    return false;
//
//}
//
//
//ImoNote* lmScoreChord::GetNoteLmNote(int nIndex)
//{
//    if (m_pChordNotes[nIndex] != 0)
//       return m_pChordNotes[nIndex];
//    else
//       return 0;
//}
//int lmScoreChord::GetNoteVoice(int nIndex)
//{
//    if (m_pChordNotes[nIndex] != 0)
//        return m_pChordNotes[nIndex]->GetVoice();
//    else
//    {
//        assert(nIndex<m_nNumChordNotes);
//        return m_nNumChordNotes-nIndex;
//    }
//}
//
//int lmScoreChord::GetNumLmNotes() // todo: not necessary: remove
//{
//    return m_nNumLmNotes;
//    /* other possibility:
//    int nCount = 0;
//    for (int i = 0; i<k_notes_in_chord; i++)
//    {
//        if (m_pChordNotes[i] != 0)
//            nCount++;
//
//    }
//    return nCount; -*/
//}
//
//void lmScoreChord::RemoveAllNotes()
//{
//    this->lmFPitchChord::RemoveAllNotes();
//    m_nNumLmNotes = 0;
//    for (int i = 0; i<k_notes_in_chord; i++)
//    {
//        m_pChordNotes[i] = 0;
//    }
//}
//
//// todo: remove this method in the final release: it just shows redundant note information
//wxString lmScoreChord::ToString()
//{
//    wxString sStr = this->lmFPitchChord::ToString();
//    /*- todo remove: the notes should be the same as in lmFPitchChord
//    sStr += wxString::Format("; %d lmNotes:", m_nNumLmNotes);
//    for (int nN = 0; nN<m_nNumLmNotes; nN++)
//    {
//        if (m_pChordNotes[nN] != 0 && m_fpChordNotes[nN] != 0)
//        {
//            sStr += " ";
//            sStr += m_pChordNotes[nN]->GetPrintName().wx_str();
//        }
//    } */
//    return sStr;
//}
//
//
//
//
//
////----------------------------------------------------------------------------------
//// class lmActiveNotes
////----------------------------------------------------------------------------------
//
//lmActiveNotes::lmActiveNotes()
//    : m_rCurrentTime(0.0)
//{
//}
//
//lmActiveNotes::~lmActiveNotes()
//{
//    std::list<lmActiveNoteInfo*>::iterator it;
//    it=m_ActiveNotesInfo.begin();
//    while( it != m_ActiveNotesInfo.end())
//    {
//         delete *it;
//         it = m_ActiveNotesInfo.erase(it);
//    }
//
//}
//
//void lmActiveNotes::SetTime(float rNewCurrentTime)
//{
//    m_rCurrentTime = rNewCurrentTime;
//    RecalculateActiveNotes();
//}
//
//void lmActiveNotes::ResetNotes()
//{
//    m_ActiveNotesInfo.clear();
//}
//
//int lmActiveNotes::GetNumActiveNotes()
//{
//    return (int)m_ActiveNotesInfo.size();
//}
//
//int lmActiveNotes::GetNotes(ImoNote** pNotes)
//{
//    assert(pNotes != nullptr);
//    std::list<lmActiveNoteInfo*>::iterator it;
//    int nCount = 0;
//    for(it=m_ActiveNotesInfo.begin(); it != m_ActiveNotesInfo.end(); ++it, nCount++)
//    {
//        pNotes[nCount] = (*it)->pNote;
//    }
//    return nCount;
//}
//
//void lmActiveNotes::AddNote(ImoNote* pNoteS, float rEndTimeS)
//{
//    lmActiveNoteInfo* plmActiveNoteInfo = LENMUS_NEW lmActiveNoteInfo(pNoteS, rEndTimeS);
//	m_ActiveNotesInfo.push_back( plmActiveNoteInfo );
//}
//
//void lmActiveNotes::RecalculateActiveNotes()
//{
//     std::list<lmActiveNoteInfo*>::iterator it;
//     it=m_ActiveNotesInfo.begin();
//     while(it != m_ActiveNotesInfo.end())
//     {
//         // AWARE: EQUAL time considered as finished  (TODO: CONFIRM by music expert)
//         if ( ! is_greater_time(  (*it)->rEndTime, m_rCurrentTime ) )
//         {
//             delete *it;
//             it = m_ActiveNotesInfo.erase(it);  // aware: "it = " needed to avoid crash in loop....
//         }
//         else
//             it++;
//     }
//}
//
//// TODO: method used for debug. Keep it?
//wxString lmActiveNotes::ToString()
//{
//    wxString sRetStr;
//    std::list<lmActiveNoteInfo*>::iterator it;
//    for(it=m_ActiveNotesInfo.begin(); it != m_ActiveNotesInfo.end(); ++it)
//    {
//        sRetStr += wxString::Format(" %s ", (*it)->pNote->GetPrintName().wx_str());
//    }
//    return sRetStr;
//}
//
//
////
//// class lmRuleList
////
//
//lmRuleList::lmRuleList(lmScoreChord** pChD, int nNumChords)
//{
//    CreateRules();
//    SetChordDescriptor(pChD, nNumChords);
//};
//
//// TODO: ADD MORE HARMONY RULES
////        To add a rule:
////        1) Create the class (recommended to use the macro LM_CREATE_CHORD_RULE)
////        2) Add an instance in AddRule
////        3) Implement the Evaluate method
////////////////////////////////////////////////////////////////////////
//
//// Todo: select the applicable rules somehow?  use IsEnabled?
//
////
//// Add rules
////
//
//LM_CREATE_CHORD_RULE(lmRuleNoParallelMotion, lmCVR_NoParallelMotion)
//LM_CREATE_CHORD_RULE(lmRuleNoResultingFifthOctaves, lmCVR_NoResultingFifthOctaves)
//LM_CREATE_CHORD_RULE(lmRuleNoVoicesCrossing, lmCVR_NoVoicesCrossing)
//LM_CREATE_CHORD_RULE(lmNoIntervalHigherThanOctave, lmCVR_NoIntervalHigherThanOctave)
//
//void lmRuleList::CreateRules()
//{
//    AddRule( LENMUS_NEW lmRuleNoParallelMotion(),
//        "No parallel motion of perfect octaves, perfect fifths, and unisons" );
//    AddRule( LENMUS_NEW lmRuleNoResultingFifthOctaves(),
//        "No resulting fifths and octaves" );
//    AddRule( LENMUS_NEW lmRuleNoVoicesCrossing(),
//        "Do not allow voices crossing. No duplicates (only for root position and bass duplicated)" );
//    AddRule( LENMUS_NEW lmNoIntervalHigherThanOctave(),
//        "Voices interval not greater than one octave (except bass-tenor)" );
//}
//
//
//
////
//// lmChordError
////
//// TODO: evaluate usability of this method of compressing each error in just a bit of information
//bool lmChordError::IncludesError(int nBrokenRule)
//{
//    if ( nBrokenRule < lmCVR_FirstChordValidationRule || nBrokenRule > lmCVR_LastChordValidationRule)
//        return false; // invalid rule
//    wxLogMessage("IncludesError %d ,  ErrList:%u ,  %u"
//		 , nBrokenRule,  nErrList,   (nErrList & (1 << nBrokenRule) ) != 0 );
//    return (nErrList & (1 << nBrokenRule) ) != 0;
//}
//void lmChordError::SetError(int nBrokenRule, bool fVal)
//{
//    assert ( nBrokenRule >= lmCVR_FirstChordValidationRule && nBrokenRule <= lmCVR_LastChordValidationRule);
//    nErrList |= ( (fVal? 1:0) << nBrokenRule );
//}
//
//
////
//// class Rule
////
//
//lmRule::lmRule(int nRuleID)
//{
//    m_fEnabled = true;
//    m_pChordDescriptor = nullptr;
//    m_sDetails = "nothing";
//    m_nRuleId = nRuleID;
//};
//
//
////
//// Definition of rules of harmony
////
//
//// return: number of errors
////
//// RULE: Parallel motion of voices is forbiden for the following intervals:
////   unison/octave
////   5th
//int lmRuleNoParallelMotion::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[], ChordInfoBox* pBox )
//{
//    sResultDetails = "Rule: No parallel motion ";
//    if ( m_pChordDescriptor == nullptr)
//    {
//        wxLogMessage(" lmRuleNoParallelMotion: m_pChordDescriptor nullptr ");
//        return false;
//    }
//    int nDifColour = this->GetRuleId() * 10;   // each rule has a slightly different color
//    int nTransp = 128;
//    wxColour colour( 200, 20+nDifColour, 20+nDifColour, nTransp);
//    int nErrCount = 0;
//    int nNumNotes;
//    int nVoiceInterval[k_notes_in_chord];
//    sResultDetails = "";
//    // Analyze all chords
//    for (int nC=1; nC<m_nNumChords; nC++)
//    {
//            wxLogMessage("Check chord %d ", (nC)+1);
//
//        pNumFailuresInChord[nC] = 0;
//
//        // num notes: min of both chords
//        nNumNotes = (m_pChordDescriptor[nC]->get_num_notes() < m_pChordDescriptor[nC-1]->get_num_notes()?
//                     m_pChordDescriptor[nC]->get_num_notes():  m_pChordDescriptor[nC-1]->get_num_notes());
//        for (int nN=0; nN<nNumNotes; nN++)
//        {
//            // get the interval between voices
//            nVoiceInterval[nN] =  ( m_pChordDescriptor[nC]->GetNoteFpitch(nN)
//                - m_pChordDescriptor[nC-1]->GetNoteFpitch(nN) ) % k_interval_p8 ;
//
//            // check if it is parallel with previous chord
//            for (int i=0; i<nN; i++)
//            {
//                if ( nVoiceInterval[i] == nVoiceInterval[nN])
//                {
//                     FIntval nInterval = abs(
//                         m_pChordDescriptor[nC]->GetNoteFpitch(nN)
//                         - m_pChordDescriptor[nC]->GetNoteFpitch(i) );
//                     int nIntervalNumber = FIntval_GetNumber(nInterval);
//
//                     wxLogMessage(" >>> Check parallel motion in chord %d, notes:%d %d, INTERVAL:%d(%s) {%d}"
//		               ,nC, i,  nN,  nIntervalNumber
//                       , FIntval_GetIntvCode(nInterval).wx_str()
//                       , nInterval);
//
//                    // check if parallel interval is unison/octave or 5th
//                    if ( nIntervalNumber == 1 || nIntervalNumber == 5 )
//                    {
//                        wxString sType =  FIntval_GetName(nInterval);
//                        pNumFailuresInChord[nC] = pNumFailuresInChord[nC]  +1;
//
//                        int nFullVoiceInterval = abs ( m_pChordDescriptor[nC]->GetNoteFpitch(i)
//                              - m_pChordDescriptor[nC-1]->GetNoteFpitch(i) );
//
////TODO: accumulate messages?                        sResultDetails += wxString::Format(
//                        sResultDetails = wxString::Format(
//                            "Parallel motion of %s, chords: %d, %d; v%d %s-->%s, v%d %s-->%s, Interval: %s"
//                            ,sType.wx_str(),  (nC-1)+1, (nC)+1
//                            , m_pChordDescriptor[nC]->GetNoteVoice(i)
//                            , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(i)).wx_str()
//                            , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(i)).wx_str()
//                            , m_pChordDescriptor[nC]->GetNoteVoice(nN)
//                            , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(nN)).wx_str()
//                            , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).wx_str()
//                            , FIntval_GetIntvCode(nInterval).wx_str()
//                            );
//
//                        wxLogMessage( sResultDetails );
//
//
//                        if (pBox && m_pChordDescriptor[nC]->HasLmNotes())
//                        {
//                            pBox->DisplayChordInfo(
//                                GetMainFrame()->GetActiveDoc()->GetScore()
//                                , m_pChordDescriptor[nC], colour, sResultDetails);
//
//
//                            // display failing notes in red   TODO: this could be improved...
//                            m_pChordDescriptor[nC]->GetNoteLmNote(nN)->SetColour(*wxCYAN);
//                            m_pChordDescriptor[nC]->GetNoteLmNote(i)->SetColour(*wxBLUE);
//                            m_pChordDescriptor[nC-1]->GetNoteLmNote(nN)->SetColour(*wxCYAN);
//                            m_pChordDescriptor[nC-1]->GetNoteLmNote(i)->SetColour(*wxBLUE);
//
//                            DrawArrow(
//                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(nN),
//                                 m_pChordDescriptor[nC]->GetNoteLmNote(nN),
//                                 wxColour(*wxRED) );
//                            DrawArrow(
//                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(i),
//                                 m_pChordDescriptor[nC]->GetNoteLmNote(i),
//                                 wxColour(*wxRED) );
//                        }
//
//
//                        m_pChordDescriptor[nC]->tChordErrors.SetError( this->GetRuleId(), true);
//                        nErrCount++;
//                    }
//                }
//
//            }
//        }
//    }
//
//    wxLogMessage(" Rule %d final error count %d", this->GetRuleId(), nErrCount);
//    return nErrCount;
//}
//
//// RULE:  Forbidden to arrive to a fifth or octave interval by means of a direct movement
////      unless: voice interval is 2nd and voice is not bass
//int lmRuleNoResultingFifthOctaves::Evaluate(wxString& sResultDetails
//                                            , int pNumFailuresInChord[], ChordInfoBox* pBox)
//{
//    wxString sMovTypes[] =
//        {"Direct", "Oblique", "Contrary"};
//
//    sResultDetails = "Rule: No resulting fifth/octaves ";
//    if ( m_pChordDescriptor == nullptr)
//    {
//        wxLogMessage(" lmRuleNoResultingFifthOctaves: m_pChordDescriptor nullptr ");
//        return 0;
//    }
//
//    int nDifColour = this->GetRuleId() * 10;   // each rule has a slightly different color
//    int nTransp = 128;
//    wxColour colour( 200, 20+nDifColour, 20+nDifColour, nTransp);
//    int nErrCount = 0;
//    int nNumNotes;
//    int nVoiceMovementType;
//    // Analyze all chords
//    for (int nC=1; nC<m_nNumChords; nC++)
//    {
//        wxLogMessage("Check chords %d TO %d", nC-1, nC);
//
//        pNumFailuresInChord[nC] = 0;
//
//        // num notes: min of both chords
//        nNumNotes = (m_pChordDescriptor[nC]->get_num_notes() < m_pChordDescriptor[nC-1]->get_num_notes()?
//                     m_pChordDescriptor[nC]->get_num_notes():  m_pChordDescriptor[nC-1]->get_num_notes());
//
//        // for all the notes in the chord...
//        for (int nN=0; nN<nNumNotes; nN++)
//        {
//            // check type of movement with any previous note
//            for (int i=0; i<nN; i++)
//            {
//                nVoiceMovementType = GetHarmonicMovementType(
//                  m_pChordDescriptor[nC-1]->GetNoteFpitch(nN), m_pChordDescriptor[nC]->GetNoteFpitch(nN),
//                  m_pChordDescriptor[nC-1]->GetNoteFpitch(i), m_pChordDescriptor[nC]->GetNoteFpitch(i));
//
//
//                FIntval nInterval = abs(
//                         m_pChordDescriptor[nC]->GetNoteFpitch(nN)
//                         - m_pChordDescriptor[nC]->GetNoteFpitch(i) );
//                int nIntervalNumber = FIntval_GetNumber(nInterval);
//
//                wxLogMessage(" Notes: %s-->%s %s-->%s Movement type:%s  INTERVAL:%d (%s)"
//                        , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(nN)).wx_str()
//                        , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).wx_str()
//                        , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(i)).wx_str()
//                        , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(i)).wx_str()
//                        , sMovTypes[nVoiceMovementType].wx_str()
//                        , nIntervalNumber, FIntval_GetIntvCode(nInterval).wx_str());
//
//                if ( nVoiceMovementType == lm_eDirectMovement && ( nIntervalNumber == 1 || nIntervalNumber == 5 )  )
//                {
//                    // Incorrect, unless: voice interval is 2th and voice is > 0 (not BASS)
//                     FIntval nVoiceInterval = abs(
//                         m_pChordDescriptor[nC]->GetNoteFpitch(nN)
//                         - m_pChordDescriptor[nC]->GetNoteFpitch(i) ) ;
//                     int nVoiceIntervalNumber = FIntval_GetNumber(nVoiceInterval);
//
//                     if (  nVoiceIntervalNumber == 2 && nN > 0 )
//                     {
//                        wxLogMessage(" Exception!, voice not BASS and voice interval is 2th!  ");
//                     }
//                     else
//                     {
//                        wxString sType;
//                        if (nInterval > 80) // current limitation in FIntval_GetName
//                           sType = "higher than 2 octaves";
//                        else
//                           sType =  FIntval_GetName(nInterval);
//
//                        sResultDetails = wxString::Format(
//               "Direct movement resulting %s. Chords:%d,%d. Voices:%d %s-->%s and %d %s-->%s. Interval: %s"
//               , sType.wx_str(), (nC-1)+1, (nC)+1
//               , m_pChordDescriptor[nC]->GetNoteVoice(nN)
//               , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(nN)).wx_str()
//               , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).wx_str()
//               , m_pChordDescriptor[nC]->GetNoteVoice(i)
//               , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(i)).wx_str()
//               , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(i)).wx_str()
//               , FIntval_GetIntvCode(nInterval).wx_str());
//
//                        if (pBox && m_pChordDescriptor[nC-1]->HasLmNotes() && m_pChordDescriptor[nC]->HasLmNotes())
//                        {
//                            DrawArrow(
//                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(nN),
//                                 m_pChordDescriptor[nC]->GetNoteLmNote(nN),
//                                 wxColour(*wxCYAN) );
//                             DrawArrow(
//                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(i),
//                                 m_pChordDescriptor[nC]->GetNoteLmNote(i),
//                                 wxColour(*wxCYAN) );
//                        }
//
//                        wxLogMessage( sResultDetails );
//
//
//                        if (pBox && m_pChordDescriptor[nC]->HasLmNotes() )
//                        {
//                           pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
//                              , m_pChordDescriptor[nC], colour, sResultDetails);
//
//                            // display failing notes in red  (TODO: improve error display?)
//                            m_pChordDescriptor[nC]->GetNoteLmNote(nN)->SetColour(*wxRED);
//                            m_pChordDescriptor[nC]->GetNoteLmNote(i)->SetColour(*wxRED);
//                        }
//
//
//                        m_pChordDescriptor[nC]->tChordErrors.SetError( this->GetRuleId(), true);
//                        nErrCount++;
//                    }
//                }
//
//            }
//        }
//    }
//
//    wxLogMessage(" Rule %d final error count %d", this->GetRuleId(), nErrCount);
//    return nErrCount;
//}
//
//
//
//// 4-parts harmony RULE: do not allow voices crossing when the chord in root position (no inversions) AND
////        the bass note is duplicated
//int lmRuleNoVoicesCrossing::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[]
//                                     , ChordInfoBox* pBox)
//{
//    sResultDetails = "Rule: No voices crossing:";
//    if ( m_pChordDescriptor == nullptr)
//    {
//        wxLogMessage(" lmRuleNoVoicesCrossing: m_pChordDescriptor nullptr ");
//        return 0;
//    }
//    int nDifColour = this->GetRuleId() * 10;   // each rule has a slightly different color
//    int nTransp = 128;
//    wxColour colour( 200, 20+nDifColour, 20+nDifColour, nTransp);
//    int nErrCount = 0;
//    int nNumNotes1 = 0;
//    int nNumNotes2 = 0;
//
//    // Analyze each chord against the previous one
//    for (int nC=1; nC<m_nNumChords; nC++)
//    {
//        pNumFailuresInChord[nC] = 0;
//
//        // Apply rule only if:
//        //  chord in root position (no inversions)
//        //  root note is duplicated
//        if ( m_pChordDescriptor[nC]->GetInversion() != 0 )
//        {
//            wxLogMessage(" Rule not applicable: not root position: %d inversions", m_pChordDescriptor[nC]->GetInversion());
//            continue;
//        }
//        if (  m_pChordDescriptor[nC]->GetInversion() == 0 && ! m_pChordDescriptor[nC]->IsBassDuplicated() )
//        {
//            wxLogMessage(" Rule not applicable: root position but bass note not duplicated");
//            continue;
//        }
//
//
//        // Check that the voices of the notes in both chords are the same
//        //   (note that only the voices present in both chords must be checked).
//        // Report an error for each mismatch
//        nNumNotes1 = m_pChordDescriptor[nC-1]->get_num_notes(); // num notes in previous chord
//        nNumNotes2 = m_pChordDescriptor[nC]->get_num_notes(); // num notes in previous chord
//        wxLogMessage(  "  Chords %d to %d, checking voice crossing", nC-1, nC);
//        for (int nN1=0; nN1 < nNumNotes1; nN1++)
//        {
//            for (int nN2=0; nN2 < nNumNotes2; nN2++)
//            {
//                if ( nN1 != nN2
//                    && m_pChordDescriptor[nC-1]->GetNoteVoice(nN1) == m_pChordDescriptor[nC]->GetNoteVoice(nN2)  )
//                {
//                    sResultDetails = wxString::Format(
//                        " Voice %d crossing: chord %d note:%d, chord %d note:%d "
//                        , m_pChordDescriptor[nC-1]->GetNoteVoice(nN1)
//                        , nC,   nN1
//                        , nC+1, nN2);
//
//                    wxLogMessage( sResultDetails );
//
//                    if (pBox)
//                    {
//                         pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
//                            , m_pChordDescriptor[nC], colour, sResultDetails);
//
//                        if (m_pChordDescriptor[nC-1]->HasLmNotes() && m_pChordDescriptor[nC]->HasLmNotes())
//                        {
//                             // display failing notes in red
//                             m_pChordDescriptor[nC-1]->GetNoteLmNote(nN1)->SetColour(*wxRED);
//                             m_pChordDescriptor[nC]->GetNoteLmNote(nN2)->SetColour(*wxRED);
//                             DrawArrow(
//                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(nN1),
//                                 m_pChordDescriptor[nC]->GetNoteLmNote(nN2),
//                                 colour );
//                        }
//                     }
//                     m_pChordDescriptor[nC]->tChordErrors.SetError( this->GetRuleId(), true);
//                     nErrCount++;
//                }
//            }
//        }
//    }
//
//    wxLogMessage(" Rule %d final error count %d", this->GetRuleId(), nErrCount);
//    return nErrCount;
//}
//
//
//// RULE: forbidden intervals higher than octave when the chord is in root position (no inversions) AND
////         the bass is duplicated
//int lmNoIntervalHigherThanOctave::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[]
//                                           , ChordInfoBox* pBox)
//{
//    sResultDetails = " Rule: no interval higher than octave";
//    if ( m_pChordDescriptor == nullptr)
//    {
//        wxLogMessage(" lmNoIntervalHigherThanOctave:  m_pChordDescriptor nullptr ");
//        return 0;
//    }
//    int nDifColour = this->GetRuleId() * 10;
//    int nTransp = 128;
//    wxColour colour( 200, 20+nDifColour, 20+nDifColour, nTransp);
//    int nErrCount = 0;
//    int nNumNotes;
//    int nInterval;
//
//    // Analyze all chords
//    for (int nC=0; nC<m_nNumChords; nC++)
//    {
//        wxLogMessage("Check chord %d ", nC);
//
//        pNumFailuresInChord[nC] = 0;
//
//        // Apply rule only if:
//        //  chord in root position (o inversions)
//        //  root note is duplicated
//        if ( m_pChordDescriptor[nC]->GetInversion() != 0 )
//        {
//            wxLogMessage(" Rule not applicable: not root position: %d inversions", m_pChordDescriptor[nC]->GetInversion());
//            continue;
//        }
//        if (  m_pChordDescriptor[nC]->GetInversion() == 0 && ! m_pChordDescriptor[nC]->IsBassDuplicated() )
//        {
//            wxLogMessage(" Rule not applicable: root position but bass note is not duplicated");
//            continue;
//        }
//
//        nNumNotes = m_pChordDescriptor[nC]->get_num_notes() ;
//
//        // TODO: confirm: only applicable to 4 voices
//        if ( nNumNotes !=  4 )
//        {
//            wxLogMessage(" Rule not applicable: not 4 notes (%d)", nNumNotes);
//            continue;
//        }
//        // for all the voices in the chord...
//        for (int nN=1; nN<4; nN++)
//        {
//            FIntval nLimit;
//            if (nN == 1)
//                nLimit = k_interval_p8*2; // up to 2 octaves allowed for bass-tenor
//            else
//                nLimit = k_interval_p8; // only ine octave allowed for the rest
//
//            nInterval = m_pChordDescriptor[nC]->GetNoteFpitch(nN)
//                            - m_pChordDescriptor[nC]->GetNoteFpitch(nN-1);
//
//            wxLogMessage("  Notes %d - %d: interval: %d ", nN, nN-1, nInterval);
//
//            if (  nInterval > nLimit )
//            {
//                sResultDetails = wxString::Format(
//                "Chord %d: Interval %s higher than octave between voices %d (%s) and %d (%s)"
//                , (nC)+1
//                , FIntval_GetIntvCode(nInterval).wx_str()
//                , m_pChordDescriptor[nC]->GetNoteVoice(nN)
//                , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).wx_str()
//                , m_pChordDescriptor[nC]->GetNoteVoice(nN-1)
//                , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN-1)).wx_str()
//                );
//
//                wxLogMessage( sResultDetails );
//
//                if (pBox &&  m_pChordDescriptor[nC]->HasLmNotes())
//                {
//                    pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
//                      , m_pChordDescriptor[nC], colour, sResultDetails);
//
//                    m_pChordDescriptor[nC]->GetNoteLmNote(nN)->SetColour(*wxRED);
//                    m_pChordDescriptor[nC]->GetNoteLmNote(nN-1)->SetColour(*wxRED);
//
//                    DrawArrow(
//                         m_pChordDescriptor[nC]->GetNoteLmNote(nN-1),
//                         m_pChordDescriptor[nC]->GetNoteLmNote(nN),
//                         wxColour(*wxBLUE) );
//                }
//
//                 m_pChordDescriptor[nC]->tChordErrors.SetError( this->GetRuleId(), true);
//                 nErrCount++;
//                 wxLogMessage(" Rule %d partial error count %d", this->GetRuleId(), nErrCount);
//            }
//        }
//    }
//
//    wxLogMessage(" Rule %d final error count %d", this->GetRuleId(), nErrCount);
//    return nErrCount;
//}
//
//
//
////
//// lmRuleList
////
//
//lmRuleList::~lmRuleList()
//{
//    // Iterate over the map and delete lmRule
//    std::map<int, lmRule*>::iterator it;
//    for(it = m_Rules.begin(); it != m_Rules.end(); ++it)
//    {
//        delete it->second;
//    }
//    m_Rules.clear();
//}
//
//
//bool lmRuleList::AddRule(lmRule* pNewRule, const wxString& sDescription )
//{
//    int nRuleId = pNewRule->GetRuleId();
//    pNewRule->SetDescription(sDescription);
//
//    std::map<int, lmRule*>::iterator it = m_Rules.find(nRuleId);
//    if(it != m_Rules.end())
//    {
//        wxLogMessage(" AddRule: Rule %d already stored !", nRuleId);
//        return false;
//    }
//    if ( nRuleId >= lmCVR_FirstChordValidationRule && nRuleId <= lmCVR_LastChordValidationRule)
//    {
//        m_Rules.insert(std::pair<int, lmRule*>(nRuleId, pNewRule));
//    }
//    else
//    {
//        wxLogMessage(" AddRule: INVALID rule id: %d", nRuleId );
//    }
//    return true;
//}
//
//bool lmRuleList::DeleteRule(int nRuleId)
//{
//    std::map<int, lmRule*>::iterator it = m_Rules.find(nRuleId);
//    if(it == m_Rules.end())
//    {
//        wxLogMessage(" DeleteRule: Rule %d not stored !", nRuleId);
//        return false;
//    }
//    m_Rules.erase(it);
//    return true;
//}
//lmRule* lmRuleList::GetRule(int nRuleId)
//{
//    std::map<int, lmRule*>::iterator it = m_Rules.find(nRuleId);
//    if(it == m_Rules.end())
//        return nullptr;
//    else
//        return it->second;
//}
//void lmRuleList::SetChordDescriptor(lmScoreChord** pChD, int nNumChords)
//{
//    // Iterate over the map and set Chord Descriptor to each item
//    //  Note: Use a const_iterator if we are not going to change the values
//    //     for(mapType::const_iterator it = data.begin(); it != data.end(); ++it)
//    std::map<int, lmRule*>::iterator it;
//    for(it = m_Rules.begin(); it != m_Rules.end(); ++it)
//    {
//        it->second->SetChordDescriptor( pChD, nNumChords );
//    }
//}
//
//
//}   //namespace lenmus
