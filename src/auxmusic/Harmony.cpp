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
#pragma implementation "Harmony.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Harmony.h"
#include "../score/VStaff.h"

#include "../app/MainFrame.h" 
extern lmMainFrame* GetMainFrame();
#include "../app/ScoreDoc.h"  // DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()


//
// Global functions
//

// return: total number of errors
int AnalyzeHarmonicProgression(lmScoreChord** pChordDescriptor, int nNCH, ChordInfoBox* pChordErrorBox)
{
    wxLogMessage(_T("*** AnalyzeHarmonicProgression N:%d "), nNCH);
    for (int i = 0; i<nNCH; i++)
    {
        wxLogMessage(_T("   Chord %d to analyze: %s"), i, pChordDescriptor[i]->ToString().c_str());
    }

    int nNumChordError[lmMAX_NUM_CHORDS]; // number of errors in each chord

    lmRuleList tRules(pChordDescriptor, nNCH);

    wxString sStr;
    sStr.clear();

    int nNumErros = 0; // TODO: decide: total num of errors or num of chords with error
    lmRule* pRule;
    // TODO: create a method of the list to evaluate ALL the rules ?
    for (int nR = lmCVR_FirstChordValidationRule; nR<lmCVR_LastChordValidationRule; nR++)
    {
        pRule = tRules.GetRule(nR);
        if ( pRule == NULL)
        {
//todo: remove this message?            wxLogMessage(_T(" Rule %d is NULL !!!"), nR);
        }
        else if (pRule->IsEnabled())
        {
            wxLogMessage(_T("Evaluating rule %d, description: %s")
                , pRule->GetRuleId(), pRule->GetDescription().c_str());
            nNumErros += pRule->Evaluate(sStr, &nNumChordError[0], pChordErrorBox);
            wxLogMessage(_T("   Total error count after rule %d: %d errors"), pRule->GetRuleId(), nNumErros   );
            if (nNumErros > 0)
            {
              // TODO: anything else here?
            }
        }
    }

    return nNumErros;
}

// return
//  -1: negative, 0, 1: positive
int GetHarmonicDirection(int nInterval)
{
    if (nInterval > 0)
        return 1;
    else  if (nInterval < 0)
        return -1;
    else
        return 0;
}


int GetHarmonicMovementType( lmFPitch fVoice10, lmFPitch fVoice11, lmFPitch fVoice20, lmFPitch fVoice21)
{
    int nMovType = -10;

    int nD1 = GetHarmonicDirection(fVoice11 - fVoice10);
    int nD2 = GetHarmonicDirection(fVoice21 - fVoice20);

    if (nD1 == nD2)
    {
        nMovType =  lm_eDirectMovement;
    }
    else if (nD1 == -nD2)
    {
        nMovType = lm_eContraryMovement;
    }
    else
    {
        assert ( (nD1 == 0 && nD2 != 0)  ||  (nD2 == 0 && nD1 != 0) );
        nMovType = lm_eObliqueMovement;
    }
    return nMovType;
}

int GetIntervalNumberFromFPitchDistance(lmFPitch n2, lmFPitch n1) //@@@ todo remove!!!
{
    lmFIntval nDistance  = abs (n2 - n1);
    int nIntervalNumber  = FIntval_GetNumber(nDistance);
    wxLogMessage(_T("\t\t GetIntervalNumberFromFPitchDistance: %d-%d D:%d I:%d ")
        , n2, n1, nDistance, nIntervalNumber);
    return nIntervalNumber;
}

// todo: move this to "Pitch" file o  merge this with FPitch_ToAbsLDPName
// This is just FPitch_ToAbsLDPName but WITHOUT OCTAVE
static  wxString m_sNoteName[7] = {
            _T("c"),  _T("d"), _T("e"), _T("f"), _T("g"), _T("a"), _T("b") };
wxString NormalizedFPitch_ToAbsLDPName(lmFPitch fp)
{
    wxString sAnswer;
    switch(FPitch_Accidentals(fp)) {
        case -2: sAnswer =_T("--"); break;
        case -1: sAnswer =_T("-"); break;
        case 0:  sAnswer =_T(""); break;
        case 1:  sAnswer =_T("+"); break;
        case 2:  sAnswer =_T("x"); break;
        default:
            return wxEmptyString;
    }
    sAnswer += m_sNoteName[FPitch_Step(fp)];
    return sAnswer;
}

static wxString m_sNumeralsDegrees[7] =
        {_T(" I"), _T(" II"), _T("III"), _T(" IV"), _T("  V"), _T(" VI"), _T("VII")};

wxString GetChordDegreeString(lmStepType nStep )
{
    if (nStep < lmMIN_STEP ||  nStep > lmMAX_STEP)
    {
        wxLogMessage(_T("GetDegreeString: Invalid step %d"), nStep);
        nStep = 0;
    }
 
    return m_sNumeralsDegrees[nStep];
}




//  Get interval in FPitch from:
//   chord degree (root note step)
//   key signature
//   interval index (1=3rd, 2=5th, etc)
//  TODO: Used in harmony, but if it useful in general, move it to a better place such as Pitch file
lmFIntval FPitchInterval(int nRootStep, lmEKeySignatures nKey, int nInterval)
{
    // steps: 0 .. 6 (lmSTEP_C .. lmSTEP_B)
    assert (nRootStep>=lmSTEP_C && nRootStep <= lmSTEP_B);

    // aware: in harmony by default an interval has 2 steps, therefore step2 = step1 + 2*N
    lmFPitch fpPitch = FPitchStepsInterval(nRootStep, (nRootStep+(nInterval*2))%(lmSTEP_B+1), nKey);
    return (lmFIntval) fpPitch;
}

//-----------------------------------------------------------------------------------


 void SortChordNotes(int nNumNotes, lmNote** pInpChordNotes)
{
    wxASSERT(nNumNotes < lmNOTES_IN_CHORD);
    // Classic Bubble sort
    int nCount, fSwapDone;
    lmNote* auxNote;
    do
    {
        fSwapDone = 0;
        for (nCount = 0; nCount < nNumNotes - 1; nCount++)
        {
            wxASSERT(pInpChordNotes[nCount] != NULL);
            wxASSERT(pInpChordNotes[nCount+1] != NULL);
            if (pInpChordNotes[nCount]->GetFPitch() > pInpChordNotes[nCount+1]->GetFPitch() )
            {
	            auxNote = pInpChordNotes[nCount];
	            pInpChordNotes[nCount] = pInpChordNotes[nCount + 1];
	            pInpChordNotes[nCount + 1] = auxNote;
	            fSwapDone = 1;
            }
        }
    }while (fSwapDone);
}

void SortChordNotes(int nNumNotes, lmFPitch fInpChordNotes[])
{
    wxASSERT(nNumNotes < lmNOTES_IN_CHORD);
    wxASSERT(fInpChordNotes != NULL);
    // Classic Bubble sort
    int nCount, fSwapDone;
    lmFPitch auxNote;
    do
    {
        fSwapDone = 0;
        for (nCount = 0; nCount < nNumNotes - 1; nCount++)
        {
            if (fInpChordNotes[nCount] > fInpChordNotes[nCount+1] )
            {
	            auxNote = fInpChordNotes[nCount];
	            fInpChordNotes[nCount] = fInpChordNotes[nCount + 1];
	            fInpChordNotes[nCount + 1] = auxNote;
	            fSwapDone = 1;
            }
        }
    }while (fSwapDone);
}

#if 0
/* 
 This method is not used anymore. lmChordInfo has evolved to lmChord and lmChordIntervals, which
    use a diferent approach to extract the intervals.
    But the process of obtaining the intervals from the notes is essential in chord processing,
    therefore, we leave this method as an example of to do it.
 */
void GetIntervalsFromNotes(int nNumNotes, lmNote** pInpChordNotes, lmChordInfo* tOutChordInfo)
{
    wxASSERT(nNumNotes < lmNOTES_IN_CHORD);
    wxASSERT(pInpChordNotes != NULL);
    wxASSERT(tOutChordInfo != NULL);
    wxASSERT(pInpChordNotes[0] != NULL);

    lmFIntval fpIntv;
    int nCurrentIntvIndex = 0;
    int nExistingIntvIndex = 0;
    for (int nCount = 1; nCount < nNumNotes; nCount++)
    {
        wxASSERT(pInpChordNotes[nCount] != NULL);
        fpIntv = (lmFIntval) (pInpChordNotes[nCount]->GetFPitch() - pInpChordNotes[0]->GetFPitch());

        if (fpIntv >= lm_p8)
        {
              fpIntv = fpIntv % lm_p8;
        }
#ifdef __WXDEBUG__
        wxLogMessage(_T("[GetIntervalsFromNotes note %d: %d  note 0: %d] INTERVAL: %d")
            , nCount, pInpChordNotes[nCount]->GetFPitch(), pInpChordNotes[0]->GetFPitch(), fpIntv);
#endif
        // Update chord interval information
        for (nExistingIntvIndex=0; nExistingIntvIndex<nCurrentIntvIndex; nExistingIntvIndex++)
        {
            if (tOutChordInfo->nIntervals[nExistingIntvIndex] == fpIntv)
                break; // interval already exists
        }
        if (nExistingIntvIndex < nCurrentIntvIndex)
        {
            wxLogMessage(_T(" Interval %d: IGNORED, already in %d")
            , fpIntv, nExistingIntvIndex);
        }
        else
        {
            if (fpIntv == 0)
            {
                // Ignored 0 Interval
            }
            else
            {
                // Add interval
                tOutChordInfo->nIntervals[nCurrentIntvIndex] = fpIntv;
                nCurrentIntvIndex++;
            }
        }
    }

    tOutChordInfo->nNumNotes = nNumNotes;
    tOutChordInfo->nNumIntervals = nCurrentIntvIndex;

    // Sort Intervals
    //  Classic bubble sort
    int fSwapDone;
    lmFIntval auxIntv;
    do
    {
        fSwapDone = 0;
        for (int nCount = 0; nCount < tOutChordInfo->nNumIntervals - 1; nCount++)
        {
            if (tOutChordInfo->nIntervals[nCount] > tOutChordInfo->nIntervals[nCount+1] )
            {
                auxIntv = tOutChordInfo->nIntervals[nCount];
                tOutChordInfo->nIntervals[nCount] = tOutChordInfo->nIntervals[nCount + 1];
                tOutChordInfo->nIntervals[nCount + 1] = auxIntv;
                fSwapDone = 1;
            }
        }
    }while (fSwapDone);


    // Set the non-used intervals to  NULL
    for (int i=tOutChordInfo->nNumIntervals; i<lmINTERVALS_IN_CHORD; i++)
    {
      tOutChordInfo->nIntervals[i] = lmNULL_FIntval;
    }

} 
#endif


//
// Message box to display the results if the chord analysis
//
// Remember:
//      x: relative to object; positive: right
//      y: relative to top line; positive: down
ChordInfoBox::ChordInfoBox(wxSize* pSize, lmFontInfo* pFontInfo
                           , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement)
{
    Settings(pSize, pFontInfo, nBoxX, nBoxY, nLineX, nLineY, nBoxYIncrement);
}
void ChordInfoBox::Settings(wxSize* pSize, lmFontInfo* pFontInfo
                            , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement)
{
    m_ntConstBoxXstart = nBoxX;
    m_ntConstInitialBoxYStart = nBoxY;
    m_ntConstLineXstart = nLineX;
    m_ntConstLineYStart = nLineY;
    m_ntConstBoxYIncrement = nBoxYIncrement;
    m_pFontInfo = pFontInfo;
    m_pSize = pSize;

    assert(m_pFontInfo != NULL);
    assert(m_pSize != NULL);

    m_ntCurrentBoxYStart = m_ntConstInitialBoxYStart;
}
void ChordInfoBox::ResetPosition()
{
    m_ntCurrentBoxYStart = m_ntConstInitialBoxYStart;
}
void ChordInfoBox::SetYPosition(int nYpos)
{
    m_ntCurrentBoxYStart = nYpos;
}
void ChordInfoBox::DisplayChordInfo(lmScore* pScore, lmScoreChord* pChordDsct, wxColour colour, wxString &sText)
{
    if (pChordDsct == NULL )
    {
        wxLogMessage(
            _T(" DisplayChordInfo ERROR: Chord descriptor is NULL. Msg: %s")
            , sText.c_str());
        return;  // todo: improvement: in this case, display a box but not attached to any note ?
    }
    if ( pChordDsct->GetNumNotes() < 1)
    {
        wxLogMessage(_T(" DisplayChordInfo ERROR: NO notes to attach the textbox"));
        return;  // todo: improvement: in this case, display a box but not attached to any note ?
    }

    int m_nNumChordNotes  = pChordDsct->GetNumNotes();
    if ( ! pChordDsct->HasLmNotes())
    {
        wxLogMessage(_T(" DisplayChordInfo ERROR: NO score notes!"));
        return;
    }
    lmTextStyle* pStyle = pScore->GetStyleName(*m_pFontInfo);

    // Display chord info in score with a line and text
    assert(m_nNumChordNotes > 0);
    assert(m_nNumChordNotes < 20);

    for (int i = 0; i<m_nNumChordNotes; i++)
    {
        assert(pChordDsct->GetNoteLmNote(i) != NULL);
        pChordDsct->GetNoteLmNote(i)->SetColour(colour);
    }


    // Line end: the first note
    lmStaffObj* cpSO = pChordDsct->GetNoteLmNote(m_nNumChordNotes-1);
    lmTPoint lmTBoxPos(m_ntConstBoxXstart, m_ntCurrentBoxYStart);
    lmTPoint lmTLinePos(m_ntConstLineXstart, m_ntConstLineYStart);
    lmAuxObj* pTxtBox = cpSO->AttachTextBox(lmTBoxPos, lmTLinePos, sText, pStyle, *m_pSize, colour);

    // here increment the static variables
    m_ntCurrentBoxYStart += m_ntConstBoxYIncrement;
}


void DrawArrow(lmNote* pNote1, lmNote* pNote2, wxColour color)
{
    //get VStaff
    lmVStaff* pVStaff = pNote1->GetVStaff();

    //get note heads positions
    lmURect uBounds1 = pNote1->GetNoteheadShape()->GetBounds();
    lmURect uBounds2 = pNote2->GetNoteheadShape()->GetBounds();

    //start point
    lmUPoint uStart( uBounds1.GetWidth(), 0);
    uStart.y = pNote1->GetShiftToNotehead();        //center of notehead

    //end point
    lmUPoint uEnd(uBounds2.GetRightTop() - uBounds1.GetRightTop());
    uEnd.y += uStart.y;

    //convert to tenths
    lmTenths xtStart = pVStaff->LogicalToTenths(uStart.x) + 8.0;
    lmTenths ytStart = pVStaff->LogicalToTenths(uStart.y);
    lmTenths xtEnd = pVStaff->LogicalToTenths(uEnd.x) - 8.0;
    lmTenths ytEnd = pVStaff->LogicalToTenths(uEnd.y);

    //create arrow
    pNote1->AttachLine(xtStart, ytStart, xtEnd, ytEnd, 2, lm_eLineCap_None,
                       lm_eLineCap_Arrowhead, lm_eLine_Solid, color);
    pNote1->SetColour(color);
    pNote2->SetColour(color);
}


/* AWARE:

  lmChord is the basic chord, defined by ROOT NOTE and intervals
      aware: only the root note is REAL; the rest of notes, obtained with GetNote(i) (where i>0)
        are just POSSIBLE notes

    for certain operations we need to know all the real notes of the chord

  lmFPitchChord is a lmChord that can contain also notes in "lmFPitch".
  The lmFPitch notes can be added after the construction, except the root note (bass voice) that is already in lmChord
    remember: the ROOT NOTE is REQUIRED in ANY chord;
      it is defined in the construction and should never be modified afterwards


    Possible constructors :
      1 created with NO notes; then added with AddNoteFromLmNote or AddNoteLmFPitch
	      GetNoteFpitch returns a 0 if the note has not been added
      2 created with notes. NO POSSIBLE TO ADD NOTES.  [controlled with m_fCreatedWithNotes]
    check the number of notes available with GetNumNotes()
	Notes can not be removed nor modified

  lmScoreChord is a 'real score chord': it contains actual notes of the score (lmNotes)
   it is a lmFPitchChord that can contain also notes in "lmNotes"

   The lmNotes notes can be added after the construction

    aware: the root note might NOT be present. The root note is alway present in lmFPitch, but it may not have
            a corresponding lmNote.

    Possible constructors :
      1 created with NO notes; then
             added with AddNoteFromLmNote
			    adds both lmFPitch and lmNotes
             added ONLY lmFPitch with AddNoteLmFPitch or AddNoteFromInterval
			   it only adds  lmFPitch !!!
			      To add the missing lmNote, use SetLmNote
			         since the lmFPitch of this note must exist
  					    it must be the same as the note->GetFPitch(). This is checked.
	      GetNoteFpitch returns a 0 if the note has not been added
      2 created with notes. NO POSSIBLE TO ADD NOTES.
*/
//
// class lmFPitchChord
//

// Constructors

// Create a chord from a list of ORDERED score notes in LmFPitch
lmFPitchChord::lmFPitchChord(int nNumNotes, lmFPitch fNotes[], lmEKeySignatures nKey)
    : lmChord(nNumNotes, fNotes, nKey)
{
    assert(nNumNotes<lmNOTES_IN_CHORD);
    for (int i = 0; i<nNumNotes; i++)
    {
        assert( IsValidChordNote(fNotes[i]) );
        if (i == 0)
        {
            assert( (fNotes[0] % lm_p8) == GetNormalizedBass() );
        }
        m_fpChordNotes[i] = fNotes[i];
    }
    for (int i = nNumNotes; i<lmNOTES_IN_CHORD; i++)
    {
        m_fpChordNotes[i] = 0;
    }
    m_nNumChordNotes = nNumNotes;

    m_fCreatedWithNotes = true;
}

lmFPitchChord::lmFPitchChord(int nNumNotes, lmNote** pNotes, lmEKeySignatures nKey)
    : lmChord(nNumNotes, pNotes, nKey)
{
    assert(nNumNotes<lmNOTES_IN_CHORD);
    for (int i = 0; i<nNumNotes; i++)
    {
        assert( IsValidChordNote(pNotes[i]->GetFPitch()) );
        if (i == 0)
        {
            assert( (pNotes[0]->GetFPitch() % lm_p8) == GetNormalizedBass() );
            // actual bass note must be consistent with chord bass note
        }
        m_fpChordNotes[i] = pNotes[i]->GetFPitch();
    }
    for (int i = nNumNotes; i<lmNOTES_IN_CHORD; i++)
    {
        m_fpChordNotes[i] = 0;
    }
    m_nNumChordNotes = nNumNotes;
    m_fCreatedWithNotes = true;
}

// Creates a chord from "essential" information
lmFPitchChord::lmFPitchChord(int nDegree, lmEKeySignatures nKey, int nNumIntervals, int nNumInversions, int octave)
    : lmChord(nDegree, nKey, nNumIntervals, nNumInversions, octave)
{
    m_nNumChordNotes = 0;
    m_fCreatedWithNotes = false;
}


int  lmFPitchChord::AddNoteLmFPitch(lmFPitch fNote)
{
    if (m_fCreatedWithNotes)
    {
        wxLogMessage(_T(" lmFPitchChord::AddNoteLmFPitch ERROR, it was created with %d notes ")
            , m_nNumChordNotes );
    }
    else
    {
        assert(m_nNumChordNotes<lmNOTES_IN_CHORD);
        if ( this->IsValidChordNote(fNote) )
        {
            m_fpChordNotes[m_nNumChordNotes] = fNote;
            m_nNumChordNotes++;
            SortChordNotes(m_nNumChordNotes, m_fpChordNotes); // sort notes so that voice <=> index
        }
        else
        {
            wxLogMessage(_T(" lmFPitchChord::AddNoteLmFPitch ERROR note %d [%s] does not belong to chord {%s}")
                ,fNote,FPitch_ToAbsLDPName(fNote).c_str() , this->ToString().c_str()  );
        }
    }
    return m_nNumChordNotes;
}


bool lmFPitchChord::IsBassDuplicated()
{
    // remember: the lowest note is the BASS note, not the root note
    //           (bass == root only if NO INVERSIONS)

    // Normalize wit "% lm_p8" to remove octave information
    for (int i=1; i<m_nNumChordNotes; i++)
    {
        if ( (m_fpChordNotes[i] % lm_p8) == this->GetNormalizedBass() )
            return true;
    }
    return false;
}


void lmFPitchChord::RemoveAllNotes()
{
    m_nNumChordNotes = 0;
    for (int i = 0; i<lmNOTES_IN_CHORD; i++)
    {
        m_fpChordNotes[i] = 0;
    }
}


wxString lmFPitchChord::ToString()
{
    // extend the parent information
    wxString sStr = this->lmChord::ToString();
    sStr += _T("; Notes:");
    for (int nN = 0; nN<m_nNumChordNotes; nN++)
    {
        sStr += _T(" ");
        sStr += FPitch_ToAbsLDPName(m_fpChordNotes[nN]).c_str();
    }
    return sStr;
}



//
// class lmScoreChord
//

// Creates a chord from a list of ordered score notes
lmScoreChord::lmScoreChord(int nNumNotes, lmNote** pNotes, lmEKeySignatures nKey)
    : lmFPitchChord(nNumNotes, pNotes, nKey)
{
    //   Whenever a lmNote is added, it should be checked that
    //  - this note matches the corresponding in lmFPitch (m_fpChordNotes)
    //  - the note is valid: it can be obtained from the bass note by adding an interval and +-octaves
    m_nNumLmNotes = 0;
    assert(nNumNotes<lmNOTES_IN_CHORD);
    for (int i = 0; i<nNumNotes; i++)
    {
        assert( pNotes[i]->GetFPitch() == m_fpChordNotes[i] );
        if (i == 0)
        {
            // ENSURE THE lmFPitch of the note is the same as pNote-> GetFPitch()
            assert( (pNotes[0]->GetFPitch() % lm_p8) == GetNormalizedBass() );
            // actual bass note must be consistent with chord bass note
        }
        m_pChordNotes[i] = pNotes[i];
        m_nNumLmNotes++;
    }
    for (int i = nNumNotes; i<lmNOTES_IN_CHORD; i++)
    {
        m_pChordNotes[i] = 0;
    }

    tChordErrors.nErrList = 0;
}

lmScoreChord::lmScoreChord(int nDegree, lmEKeySignatures nKey, int nNumIntervals, int nNumInversions, int octave)
    : lmFPitchChord(nDegree, nKey, nNumIntervals, nNumInversions, octave)
{
    m_nNumLmNotes = 0;
    for (int i = 0; i<lmNOTES_IN_CHORD; i++)
    {
        m_pChordNotes[i] = 0;
    }
    tChordErrors.nErrList = 0;
}


// aware: this is only to associate the lmNote to a note in lmFPitch that already exists
//   it is not to add a note!
bool lmScoreChord::SetLmNote(lmNote* pNote)
{
    assert(pNote);

    for (int nIndex = 0; nIndex < lmNOTES_IN_CHORD; nIndex++)
    {
        if (m_fpChordNotes[nIndex] == pNote->GetFPitch())
        {
            m_pChordNotes[nIndex] = pNote;
            m_nNumLmNotes++;
            wxLogMessage(_T(" SetLmNote %d %d OK, total LmNotes:%d"), nIndex, m_fpChordNotes[nIndex], m_nNumLmNotes);
            return true;
        }
    }
    wxLogMessage(_T(" SetLmNote ERROR!! %d (%s) , not found in %d notes")
        , pNote->GetFPitch(), FPitch_ToAbsLDPName(pNote->GetFPitch()).c_str(), m_nNumChordNotes);
    return false;

}


lmNote* lmScoreChord::GetNoteLmNote(int nIndex)
{
    if (m_pChordNotes[nIndex] != 0)
       return m_pChordNotes[nIndex];
    else
       return 0;
}
int lmScoreChord::GetNoteVoice(int nIndex)
{
    if (m_pChordNotes[nIndex] != 0)
        return m_pChordNotes[nIndex]->GetVoice();
    else
    {
        assert(nIndex<m_nNumChordNotes);
        return m_nNumChordNotes-nIndex;
    }
}

int lmScoreChord::GetNumLmNotes() // todo: not necessary: remove
{
    return m_nNumLmNotes;
    /* other possibility:
    int nCount = 0;
    for (int i = 0; i<lmNOTES_IN_CHORD; i++)
    {
        if (m_pChordNotes[i] != 0)
            nCount++;

    }
    return nCount; -*/
}

void lmScoreChord::RemoveAllNotes()
{
    this->lmFPitchChord::RemoveAllNotes();
    m_nNumLmNotes = 0;
    for (int i = 0; i<lmNOTES_IN_CHORD; i++)
    {
        m_pChordNotes[i] = 0;
    }
}

// todo: remove this method in the final release: it just shows redundant note information
wxString lmScoreChord::ToString()
{
    wxString sStr = this->lmFPitchChord::ToString();
    /*- todo remove: the notes should be the same as in lmFPitchChord
    sStr += wxString::Format(_T("; %d lmNotes:"), m_nNumLmNotes);
    for (int nN = 0; nN<m_nNumLmNotes; nN++)
    {
        if (m_pChordNotes[nN] != 0 && m_fpChordNotes[nN] != 0)
        {
            sStr += _T(" ");
            sStr += m_pChordNotes[nN]->GetPrintName().c_str();
        }
    } */
    return sStr;
}





//----------------------------------------------------------------------------------
// class lmActiveNotes
//----------------------------------------------------------------------------------

lmActiveNotes::lmActiveNotes()
    : m_rCurrentTime(0.0f)
{
}

lmActiveNotes::~lmActiveNotes()
{
    std::list<lmActiveNoteInfo*>::iterator it;
    it=m_ActiveNotesInfo.begin();
    while( it != m_ActiveNotesInfo.end())
    {
         delete *it;
         it = m_ActiveNotesInfo.erase(it);
    }

}

void lmActiveNotes::SetTime(float rNewCurrentTime)
{
    m_rCurrentTime = rNewCurrentTime;
    RecalculateActiveNotes();
}

void lmActiveNotes::ResetNotes()
{
    m_ActiveNotesInfo.clear();
}

int lmActiveNotes::GetNumActiveNotes()
{
    return (int)m_ActiveNotesInfo.size();
}

int lmActiveNotes::GetNotes(lmNote** pNotes)
{
    assert(pNotes != NULL);
    std::list<lmActiveNoteInfo*>::iterator it;
    int nCount = 0;
    for(it=m_ActiveNotesInfo.begin(); it != m_ActiveNotesInfo.end(); ++it, nCount++)
    {
        pNotes[nCount] = (*it)->pNote;
    }
    return nCount;
}

void lmActiveNotes::AddNote(lmNote* pNoteS, float rEndTimeS)
{
    lmActiveNoteInfo* plmActiveNoteInfo = new lmActiveNoteInfo(pNoteS, rEndTimeS);
	m_ActiveNotesInfo.push_back( plmActiveNoteInfo );
}

void lmActiveNotes::RecalculateActiveNotes()
{
     std::list<lmActiveNoteInfo*>::iterator it;
     it=m_ActiveNotesInfo.begin();
     while(it != m_ActiveNotesInfo.end())
     {
         // AWARE: EQUAL time considered as finished  (TODO: CONFIRM by music expert)
         if ( ! IsHigherTime(  (*it)->rEndTime, m_rCurrentTime ) )
         {
             delete *it;
             it = m_ActiveNotesInfo.erase(it);  // aware: "it = " needed to avoid crash in loop....
         }
         else
             it++;
     }
}

// TODO: method used for debug. Keep it?
wxString lmActiveNotes::ToString()
{
    wxString sRetStr;
    std::list<lmActiveNoteInfo*>::iterator it;
    for(it=m_ActiveNotesInfo.begin(); it != m_ActiveNotesInfo.end(); ++it)
    {
        sRetStr += wxString::Format(_T(" %s "), (*it)->pNote->GetPrintName().c_str());
    }
    return sRetStr;
}


//
// class lmRuleList
//

lmRuleList::lmRuleList(lmScoreChord** pChD, int nNumChords)
{
    CreateRules();
    SetChordDescriptor(pChD, nNumChords);
};

// TODO: ADD MORE HARMONY RULES
//        To add a rule:
//        1) Create the class (recommended to use the macro LM_CREATE_CHORD_RULE)
//        2) Add an instance in AddRule
//        3) Implement the Evaluate method
//////////////////////////////////////////////////////////////////////

// Todo: select the applicable rules somehow?  use IsEnabled?

//
// Add rules
//

LM_CREATE_CHORD_RULE(lmRuleNoParallelMotion, lmCVR_NoParallelMotion)
LM_CREATE_CHORD_RULE(lmRuleNoResultingFifthOctaves, lmCVR_NoResultingFifthOctaves)
LM_CREATE_CHORD_RULE(lmRuleNoVoicesCrossing, lmCVR_NoVoicesCrossing)
LM_CREATE_CHORD_RULE(lmNoIntervalHigherThanOctave, lmCVR_NoIntervalHigherThanOctave)

void lmRuleList::CreateRules()
{
    AddRule( new lmRuleNoParallelMotion(),
        _T("No parallel motion of perfect octaves, perfect fifths, and unisons") );
    AddRule( new lmRuleNoResultingFifthOctaves(),
        _T("No resulting fifths and octaves") );
    AddRule( new lmRuleNoVoicesCrossing(),
        _T("Do not allow voices crossing. No duplicates (only for root position and root duplicated)") );
    AddRule( new lmNoIntervalHigherThanOctave(),
        _T("Voices interval not greater than one octave (except bass-tenor)") );
}



//
// lmChordError
//
// TODO: evaluate usability of this method of compressing each error in just a bit of information
bool lmChordError::IncludesError(int nBrokenRule)
{
    if ( nBrokenRule < lmCVR_FirstChordValidationRule || nBrokenRule > lmCVR_LastChordValidationRule)
        return false; // invalid rule
    wxLogMessage(_T("IncludesError %d ,  ErrList:%u ,  %u")
		 , nBrokenRule,  nErrList,   (nErrList & (1 << nBrokenRule) ) != 0 );
    return (nErrList & (1 << nBrokenRule) ) != 0;
}
void lmChordError::SetError(int nBrokenRule, bool fVal)
{
    assert ( nBrokenRule >= lmCVR_FirstChordValidationRule && nBrokenRule <= lmCVR_LastChordValidationRule);
    nErrList |= ( (fVal? 1:0) << nBrokenRule );
}


//
// class Rule
//

lmRule::lmRule(int nRuleID)
{
    m_fEnabled = true;
    m_pChordDescriptor = NULL;
    m_sDetails = _T("nothing");
    m_nRuleId = nRuleID;
};


//
// Definition of rules of harmony
//

// return number of errors
int lmRuleNoParallelMotion::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[], ChordInfoBox* pBox )
{
    sResultDetails = _T("Rule: No parallel motion ");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmRuleNoParallelMotion: m_pChordDescriptor NULL "));
        return false;
    }
    wxColour colour( 200, 50, 50 );
    int nErrCount = 0;
    int nNumNotes;
    int nVoiceInterval[lmNOTES_IN_CHORD];
    sResultDetails = _T("");
    // Analyze all chords
    for (int nC=1; nC<m_nNumChords; nC++)
    {
            wxLogMessage(_T("Check chord %d "), (nC)+1);

        pNumFailuresInChord[nC] = 0;

        // num notes: min of both chords
        nNumNotes = (m_pChordDescriptor[nC]->GetNumNotes() < m_pChordDescriptor[nC-1]->GetNumNotes()?
                     m_pChordDescriptor[nC]->GetNumNotes():  m_pChordDescriptor[nC-1]->GetNumNotes());
        for (int nN=0; nN<nNumNotes; nN++)
        {
            nVoiceInterval[nN] =  ( m_pChordDescriptor[nC]->GetNoteFpitch(nN)
                - m_pChordDescriptor[nC-1]->GetNoteFpitch(nN) ) % lm_p8 ;

            // check if it is parallel with any previous note
            for (int i=0; i<nN; i++)
            {
                if ( nVoiceInterval[i] == nVoiceInterval[nN])
                {
                     lmFIntval nInterval = abs(
                         m_pChordDescriptor[nC]->GetNoteFpitch(nN)
                         - m_pChordDescriptor[nC]->GetNoteFpitch(i) );
                     int nIntervalNumber = FIntval_GetNumber(nInterval);

                     wxLogMessage(_T(" >>> Check parallel motion in chord %d, notes:%d %d, INTERVAL:%d(%s) {%d}")
		               ,nC, i,  nN,  nIntervalNumber
                       , FIntval_GetIntvCode(nInterval).c_str()
                       , nInterval);

                    if ( nIntervalNumber == 1 || nIntervalNumber == 5 )
                    {
                        wxString sType =  FIntval_GetName(nInterval);
                        pNumFailuresInChord[nC] = pNumFailuresInChord[nC]  +1;

                        int nFullVoiceInterval = abs ( m_pChordDescriptor[nC]->GetNoteFpitch(i)
                              - m_pChordDescriptor[nC-1]->GetNoteFpitch(i) );

//TODO: accumulate messages?                        sResultDetails += wxString::Format(
                        sResultDetails = wxString::Format(
                            _T("Parallel motion of %s, chords: %d, %d; v%d %s-->%s, v%d %s-->%s, Interval: %s")
                            ,sType.c_str(),  (nC-1)+1, (nC)+1
                            , m_pChordDescriptor[nC]->GetNoteVoice(i)
                            , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(i)).c_str()
                            , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(i)).c_str()
                            , m_pChordDescriptor[nC]->GetNoteVoice(nN)
                            , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(nN)).c_str()
                            , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).c_str()
                            , FIntval_GetIntvCode(nInterval).c_str()
                            );

                        wxLogMessage( sResultDetails );


                        if (pBox && m_pChordDescriptor[nC]->HasLmNotes())
                        {
                            pBox->DisplayChordInfo(
                                GetMainFrame()->GetActiveDoc()->GetScore()
                                , m_pChordDescriptor[nC], colour, sResultDetails);


                            // display failing notes in red   TODO: this could be improved...
                            m_pChordDescriptor[nC]->GetNoteLmNote(nN)->SetColour(*wxCYAN);
                            m_pChordDescriptor[nC]->GetNoteLmNote(i)->SetColour(*wxBLUE);
                            m_pChordDescriptor[nC-1]->GetNoteLmNote(nN)->SetColour(*wxCYAN);
                            m_pChordDescriptor[nC-1]->GetNoteLmNote(i)->SetColour(*wxBLUE);

                            DrawArrow(
                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(nN),
                                 m_pChordDescriptor[nC]->GetNoteLmNote(nN),
                                 wxColour(*wxRED) );
                            DrawArrow(
                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(i),
                                 m_pChordDescriptor[nC]->GetNoteLmNote(i),
                                 wxColour(*wxRED) );
                        }


                        m_pChordDescriptor[nC]->tChordErrors.SetError( this->GetRuleId(), true);
                        nErrCount++;
                    }
                }

            }
        }
    }

    wxLogMessage(_T(" Rule %d final error count %d"), this->GetRuleId(), nErrCount);
    return nErrCount;
}

// return number of errors
int lmRuleNoResultingFifthOctaves::Evaluate(wxString& sResultDetails
                                            , int pNumFailuresInChord[], ChordInfoBox* pBox)
{
    wxString sMovTypes[] =
        {_T("Direct"), _T("Oblique"), _T("Contrary")};

    // Forbidden to arrive to a fifth or octave by means of a direct movement ( both: same delta sign)
    // exceptions:
    //  - voice is soprano (TODO: consider: tenor, contralto??) and distance is 2th
    //  - TODO: consider: fifth and one sound existed??
    sResultDetails = _T("Rule: No resulting fifth/octaves ");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmRuleNoResultingFifthOctaves: m_pChordDescriptor NULL "));
        return 0;
    }

    int nDifColour = this->GetRuleId() * 2;   //todo: pensar forma de cambiar algo el color en cada regla?
    int nTransp = 128; // todo: ¿usar transparencia?
    wxColour colour( 200, 20+nDifColour, 20+nDifColour, nTransp);
    int nErrCount = 0;
    int nNumNotes;
    int nVoiceMovementType;
    // Analyze all chords
    for (int nC=1; nC<m_nNumChords; nC++)
    {
        wxLogMessage(_T("Check chords %d TO %d"), nC-1, nC);

        pNumFailuresInChord[nC] = 0;

        // num notes: min of both chords
        nNumNotes = (m_pChordDescriptor[nC]->GetNumNotes() < m_pChordDescriptor[nC-1]->GetNumNotes()?
                     m_pChordDescriptor[nC]->GetNumNotes():  m_pChordDescriptor[nC-1]->GetNumNotes());

        // for all the notes in the chord...
        for (int nN=0; nN<nNumNotes; nN++)
        {
            // check type of movement with any previous note
            for (int i=0; i<nN; i++)
            {
                nVoiceMovementType = GetHarmonicMovementType(
                  m_pChordDescriptor[nC-1]->GetNoteFpitch(nN), m_pChordDescriptor[nC]->GetNoteFpitch(nN),
                  m_pChordDescriptor[nC-1]->GetNoteFpitch(i), m_pChordDescriptor[nC]->GetNoteFpitch(i));


                lmFIntval nInterval = abs(
                         m_pChordDescriptor[nC]->GetNoteFpitch(nN)
                         - m_pChordDescriptor[nC]->GetNoteFpitch(i) );
                int nIntervalNumber = FIntval_GetNumber(nInterval);

                wxLogMessage(_T(" Notes: %s-->%s %s-->%s Movement type:%s  INTERVAL:%d (%s)")
                        , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(nN)).c_str()
                        , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).c_str()
                        , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(i)).c_str()
                        , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(i)).c_str()
                        , sMovTypes[nVoiceMovementType].c_str()
                        , nIntervalNumber, FIntval_GetIntvCode(nInterval).c_str());

                if ( nVoiceMovementType == lm_eDirectMovement && ( nIntervalNumber == 1 || nIntervalNumber == 5 )  )
                {
                    // Incorrect, unless: voice interval is 2th and voice is > 0 (not BASS)
                     lmFIntval nVoiceInterval = abs(
                         m_pChordDescriptor[nC]->GetNoteFpitch(nN)
                         - m_pChordDescriptor[nC]->GetNoteFpitch(i) ) ;
                     int nVoiceIntervalNumber = FIntval_GetNumber(nVoiceInterval);

                     if (  nVoiceIntervalNumber == 2 && nN > 0 )
                     {
                        wxLogMessage(_T(" Exception!, voice not BASS and voice interval is 2th!  "));
                     }
                     else
                     {
                        wxString sType;
                        if (nInterval > 80) // current limitation in FIntval_GetName
                           sType = _T("higher than 2 octaves");
                        else
                           sType =  FIntval_GetName(nInterval);

                        sResultDetails = wxString::Format(
               _T("Direct movement resulting %s. Chords:%d,%d. Voices:%d %s-->%s and %d %s-->%s. Interval: %s")
               , sType.c_str(), (nC-1)+1, (nC)+1
               , m_pChordDescriptor[nC]->GetNoteVoice(nN)
               , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(nN)).c_str()
               , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).c_str()
               , m_pChordDescriptor[nC]->GetNoteVoice(i)
               , FPitch_ToAbsLDPName(m_pChordDescriptor[nC-1]->GetNoteFpitch(i)).c_str()
               , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(i)).c_str()
               , FIntval_GetIntvCode(nInterval).c_str());

                        if (pBox && m_pChordDescriptor[nC-1]->HasLmNotes() && m_pChordDescriptor[nC]->HasLmNotes())
                        {
                            DrawArrow(
                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(nN),
                                 m_pChordDescriptor[nC]->GetNoteLmNote(nN),
                                 wxColour(*wxCYAN) );
                             DrawArrow(
                                 m_pChordDescriptor[nC-1]->GetNoteLmNote(i),
                                 m_pChordDescriptor[nC]->GetNoteLmNote(i),
                                 wxColour(*wxCYAN) );
                        }

                        wxLogMessage( sResultDetails );


                        if (pBox && m_pChordDescriptor[nC]->HasLmNotes() )
                        {
                           pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
                              , m_pChordDescriptor[nC], colour, sResultDetails);

                            // display failing notes in red  (TODO: improve error display?)
                            m_pChordDescriptor[nC]->GetNoteLmNote(nN)->SetColour(*wxRED);
                            m_pChordDescriptor[nC]->GetNoteLmNote(i)->SetColour(*wxRED);
                        }


                        m_pChordDescriptor[nC]->tChordErrors.SetError( this->GetRuleId(), true);
                        nErrCount++;
                    }
                }

            }
        }
    }

    wxLogMessage(_T(" Rule %d final error count %d"), this->GetRuleId(), nErrCount);
    return nErrCount;
}


// return number of errors
int lmRuleNoVoicesCrossing::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[]
                                     , ChordInfoBox* pBox)
{
    sResultDetails = _T("Rule: No voices crossing:");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmRuleNoVoicesCrossing: m_pChordDescriptor NULL "));
        return 0;
    }
    int nDifColour = this->GetRuleId() * 2;   //todo: consider to apply a different color for each rule
    int nTransp = 128; // todo: consider to user transparency
    wxColour colour( 200, 20+nDifColour, 20+nDifColour, nTransp);
    int nErrCount = 0;
    int nNumNotes;
    int nVoice[2];
    int nPitch[2];
    // Analyze all chords
    for (int nC=0; nC<m_nNumChords; nC++)
    {
        pNumFailuresInChord[nC] = 0;

        // Apply rule only if:
        //  chord in root position (o inversions)
        //  root note is duplicated
        if ( m_pChordDescriptor[nC]->GetInversion() != 0 )
        {
            wxLogMessage(_T(" Rule not applicable: not root position: %d inversions"), m_pChordDescriptor[nC]->GetInversion());
            //@@@ todo remove: return 0;
            continue;
        }
        if (  m_pChordDescriptor[nC]->GetInversion() == 0 && ! m_pChordDescriptor[nC]->IsBassDuplicated() )
        {
            wxLogMessage(_T(" Rule not applicable: not root position but root note not duplicated"));
            //@@@ todo remove: return 0;
            continue;
        }

        nNumNotes = m_pChordDescriptor[nC]->GetNumNotes() ;

        // for all the notes in the chord...
        for (int nN=0; nN<nNumNotes; nN++)
        {
            // check crossing  TODO: ENSURE VOICES HAVE A VALUE!!
            for (int i=1; i<nN; i++)
            {
                nVoice[1] = m_pChordDescriptor[nC]->GetNoteVoice(nN);
                nVoice[0] = m_pChordDescriptor[nC]->GetNoteVoice(i);
                nPitch[1] = m_pChordDescriptor[nC]->GetNoteFpitch(nN);
                nPitch[0] = m_pChordDescriptor[nC]->GetNoteFpitch(i);
                if (  nVoice[1] > nVoice[0] &&
                      nPitch[1] <= nPitch[0] )
                {
                    sResultDetails = wxString::Format(
                        _T("Chord:%d: Voice crossing.  Voice%d(%s) <= Voice%d(%s) ")
                    , (nC)+1
                    , nVoice[1], FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).c_str()
                    , nVoice[0], FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(i)).c_str()
                    );

                    wxLogMessage( sResultDetails );

                    if (pBox &&  m_pChordDescriptor[nC]->HasLmNotes())
                    {
                         pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
                            , m_pChordDescriptor[nC], colour, sResultDetails);

                         // display failing notes in red  (TODO: mejorar indicacion de errores)
                         m_pChordDescriptor[nC]->GetNoteLmNote(nN)->SetColour(*wxRED);
                         m_pChordDescriptor[nC]->GetNoteLmNote(i)->SetColour(*wxRED);
                    }

                     m_pChordDescriptor[nC]->tChordErrors.SetError( this->GetRuleId(), true);
                     nErrCount++;
                }
            }
        }
    }

    wxLogMessage(_T(" Rule %d final error count %d"), this->GetRuleId(), nErrCount);
    return nErrCount;
}



int lmNoIntervalHigherThanOctave::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[]
                                           , ChordInfoBox* pBox)
{
    sResultDetails = _T(" Rule: no interval higher than octave");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmNoIntervalHigherThanOctave:  m_pChordDescriptor NULL "));
        return 0;
    }
    int nDifColour = this->GetRuleId() * 2;
    int nTransp = 128;
    wxColour colour( 200, 20+nDifColour, 20+nDifColour, nTransp);
    int nErrCount = 0;
    int nNumNotes;
    int nInterval;

    // Analyze all chords
    for (int nC=0; nC<m_nNumChords; nC++)
    {
        wxLogMessage(_T("Check chord %d "), nC);

        pNumFailuresInChord[nC] = 0;

        // Apply rule only if:
        //  chord in root position (o inversions)
        //  root note is duplicated
        if ( m_pChordDescriptor[nC]->GetInversion() != 0 )
        {
            wxLogMessage(_T(" Rule not applicable: not root position: %d inversions"), m_pChordDescriptor[nC]->GetInversion());
            //@@@ todo remove: return 0;
            continue;
        }
        if (  m_pChordDescriptor[nC]->GetInversion() == 0 && ! m_pChordDescriptor[nC]->IsBassDuplicated() )
        {
            wxLogMessage(_T(" Rule not applicable: not root position bass note is not duplicated"));
            //@@@ todo remove: return 0;
            continue;
        }

        nNumNotes = m_pChordDescriptor[nC]->GetNumNotes() ;

        // TODO: confirm: only applicable to 4 voices
        if ( nNumNotes !=  4 )
        {
            wxLogMessage(_T(" Rule not applicable: not 4 notes (%d)"), nNumNotes);
            //@@@ todo remove: return 0;
            continue;
        }
        // for all the voices in the chord...
        for (int nN=1; nN<4; nN++)
        {
            lmFIntval nLimit;
            if (nN == 1)
                nLimit = lm_p8*2; // up to 2 octaves allowed for bass-tenor
            else
                nLimit = lm_p8; // only ine octave allowed for the rest

            // TODO: ensure correspondance VOICE - order
            nInterval = m_pChordDescriptor[nC]->GetNoteFpitch(nN)
                            - m_pChordDescriptor[nC]->GetNoteFpitch(nN-1);

            wxLogMessage(_T("  Notes %d - %d: interval: %d "), nN, nN-1, nInterval);

            if (  nInterval > nLimit )
            {
                sResultDetails = wxString::Format(
                _T("Chord %d: Interval %s higher than octave between voices %d (%s) and %d (%s)")
                , (nC)+1
                , FIntval_GetIntvCode(nInterval).c_str()
                , m_pChordDescriptor[nC]->GetNoteVoice(nN)
                , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN)).c_str()
                , m_pChordDescriptor[nC]->GetNoteVoice(nN-1)
                , FPitch_ToAbsLDPName(m_pChordDescriptor[nC]->GetNoteFpitch(nN-1)).c_str()
                );

                wxLogMessage( sResultDetails );

                if (pBox &&  m_pChordDescriptor[nC]->HasLmNotes())
                {
                    pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
                      , m_pChordDescriptor[nC], colour, sResultDetails);

                    // display failing notes in red  (TODO: mejorar indicacion de errores)
                    m_pChordDescriptor[nC]->GetNoteLmNote(nN)->SetColour(*wxRED);
                    m_pChordDescriptor[nC]->GetNoteLmNote(nN-1)->SetColour(*wxRED);

                    DrawArrow(
                         m_pChordDescriptor[nC]->GetNoteLmNote(nN-1),
                         m_pChordDescriptor[nC]->GetNoteLmNote(nN),
                         wxColour(*wxBLUE) );
                }


                 m_pChordDescriptor[nC]->tChordErrors.SetError( this->GetRuleId(), true);
                 nErrCount++;
                 wxLogMessage(_T(" Rule %d partial error count %d"), this->GetRuleId(), nErrCount);
            }
        }
    }

    wxLogMessage(_T(" Rule %d final error count %d"), this->GetRuleId(), nErrCount);
    return nErrCount;
}



//
// lmRuleList
//

lmRuleList::~lmRuleList()
{
    // Iterate over the map and delete lmRule
    std::map<int, lmRule*>::iterator it;
    for(it = m_Rules.begin(); it != m_Rules.end(); ++it)
    {
        delete it->second;
    }
    m_Rules.clear();
}


bool lmRuleList::AddRule(lmRule* pNewRule, const wxString& sDescription )
{
    int nRuleId = pNewRule->GetRuleId();
    pNewRule->SetDescription(sDescription);

    std::map<int, lmRule*>::iterator it = m_Rules.find(nRuleId);
    if(it != m_Rules.end())
    {
        wxLogMessage(_T(" AddRule: Rule %d already stored !"), nRuleId);
        return false;
    }
    if ( nRuleId >= lmCVR_FirstChordValidationRule && nRuleId <= lmCVR_LastChordValidationRule)
    {
        m_Rules.insert(std::pair<int, lmRule*>(nRuleId, pNewRule));
    }
    else
    {
        wxLogMessage(_T(" AddRule: INVALID rule id: %d"), nRuleId );
    }
    return true;
}

bool lmRuleList::DeleteRule(int nRuleId)
{
    std::map<int, lmRule*>::iterator it = m_Rules.find(nRuleId);
    if(it == m_Rules.end())
    {
        wxLogMessage(_T(" DeleteRule: Rule %d not stored !"), nRuleId);
        return false;
    }
    m_Rules.erase(it);
    return true;
}
lmRule* lmRuleList::GetRule(int nRuleId)
{
    std::map<int, lmRule*>::iterator it = m_Rules.find(nRuleId);
    if(it == m_Rules.end())
        return NULL;
    else
        return it->second;
}
void lmRuleList::SetChordDescriptor(lmScoreChord** pChD, int nNumChords)
{
    // Iterate over the map and set Chord Descriptor to each item
    //  Note: Use a const_iterator if we are not going to change the values
    //     for(mapType::const_iterator it = data.begin(); it != data.end(); ++it)
    std::map<int, lmRule*>::iterator it;
    for(it = m_Rules.begin(); it != m_Rules.end(); ++it)
    {
        it->second->SetChordDescriptor( pChD, nNumChords );
    }
}
