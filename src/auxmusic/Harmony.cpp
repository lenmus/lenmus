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

//
// Global functions
//


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


int GetHarmonicMovementType( lmNote* pVoice10, lmNote* pVoice11, lmNote* pVoice20, lmNote* pVoice21)
{
    int nMovType = -10;

    int nD1 = GetHarmonicDirection(pVoice11->GetFPitch() - pVoice10->GetFPitch());
    int nD2 = GetHarmonicDirection(pVoice21->GetFPitch() - pVoice20->GetFPitch());

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
    wxLogMessage(_("\t\t GetIntervalNumberFromFPitchDistance: %d-%d D:%d I:%d ")
        , n2, n1, nDistance, nIntervalNumber);
    return nIntervalNumber;
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
void  GetIntervalsFromNotes(int nNumNotes, lmNote** pInpChordNotes, lmChordInfo* tOutChordInfo)
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
            wxLogMessage(_(" Interval %d: IGNORED, already in %d")
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


/* TODO REMOVE WHEN NOT NEEDE ANY MORE. NOW THIS FUNCTIONALLLY IS IN ComputeTypeAndInversion
   TODO: ENSURE ELISIONS IMPLEMENTED IN ComputeTypeAndInversion

const bool CONSIDER_5TH_ELIDED = true;

// TODO: consider improvement: TryChordCreation might actually CREATE the lmChord
//       instead of just trying to do it
//
// Look for notes in the score that make up a valid chord
bool TryChordCreation(int nNumNotes, lmNote** pInpChordNotes, lmChordInfo* tOutChordInfo, wxString &sOutStatusStr)
{
    bool fOk = false;

    sOutStatusStr =  wxString::Format(_("%d notes: ") ,  nNumNotes);
    wxASSERT(pInpChordNotes != NULL);

    tOutChordInfo->Initalize();

    if (nNumNotes < 3)
    {
        sOutStatusStr +=  _(" at least 3 notes are required !");
        return false;
    }

    // Sort notes
    SortChordNotes(nNumNotes, pInpChordNotes);

    // Check whether root not is duplicated
    tOutChordInfo->fRootIsDuplicated = false;
    for (int i=1; i<nNumNotes; i++)
    {
        if ( pInpChordNotes[i]->GetFPitch() == pInpChordNotes[i]->GetFPitch())
            tOutChordInfo->fRootIsDuplicated = true;
    }

    for (int i=0; i<nNumNotes; i++)
    {
        sOutStatusStr +=  wxString::Format(_T(" %s") 
            ,pInpChordNotes[i]->GetPrintName().c_str() );
    }

    // Get intervals and Create lmChordInfo from notes
    GetIntervalsFromNotes(nNumNotes, pInpChordNotes, tOutChordInfo);

    for (int i=0; i<tOutChordInfo->nNumIntervals; i++)
    {
        sOutStatusStr +=  wxString::Format(_(", %d intervals")
            , tOutChordInfo->nIntervals[i]);
    }

    lmChordInfo tOriOutChordInfo; // TODO: this might be improved; we need to keep the original to check "fifth elided"
    //(now GetChordTypeAndInversionsFromIntervals changes tOutChordInfo a cause of the inversions)
    // TODO: IDEA: move the check "fifth elided" to a rule
    tOriOutChordInfo.Initalize();
    tOriOutChordInfo = *tOutChordInfo;

    lmEChordType nType = GetChordTypeAndInversionsFromIntervals(*tOutChordInfo);

    // If returned lmINVALID_CHORD_TYPE, the intervals do not make up a valid chord
    // ...but as a last resort: we consider a possible 5th elided
    if (CONSIDER_5TH_ELIDED && nType == lmINVALID_CHORD_TYPE)
    {
        //TODO: consider to improve by placing this inside GetChordTypeAndInversionsFromIntervals
        //TODO: consider to check elision BEFORE GetChordTypeAndInversionsFromIntervals
        //(now GetChordTypeAndInversionsFromIntervals changes tOutChordInfo a cause of the inversions)

        // Try with "fifth elided"
        nType = GetChordTypeFromIntervals( tOriOutChordInfo, true );
        if (nType != lmINVALID_CHORD_TYPE)
        {
    #ifdef __WXDEBUG__
            wxLogMessage(_T(" Checking fifth ellided, type:%d"), nType);
    #endif
            //  TODO: consider to improve by checking that root note is x2 or x3
            //  TODO: consider to improve by checking  other possible elisions
            *tOutChordInfo = tOriOutChordInfo; // TODO: might be improved; we need it to keep the original
            tOutChordInfo->nFifthElided = 1;
        }
    }


    if (nType == lmINVALID_CHORD_TYPE )
    {
    #ifdef __WXDEBUG__
        wxLogMessage(_T(" Invalid chord: Num notes %d, Ell:%d, i0:%d i1:%d i2:%d")
            ,  tOutChordInfo->nNumNotes, tOutChordInfo->nFifthElided, tOutChordInfo->nIntervals[0], tOutChordInfo->nIntervals[1]
        , tOutChordInfo->nIntervals[2] );
    #endif
        fOk = false;
        sOutStatusStr +=  _(", do not make up a valid chord!");
    }
    else
    {
        fOk = true;
        sOutStatusStr +=  _T(", chord OK ");

    }
    return fOk;
}




lmEChordType GetChordTypeFromIntervals(lmChordInfo& tChordInfo, bool fAllowFifthElided )
{
    // Search the chord type that matches the specified intervals
    // For each item, it must match:
    //   number of intervals
    //   every interval
    for (int nIntv = 0; nIntv < ect_Max; nIntv++)
    {
        if ( tChordInfo.nNumIntervals == (tChordData[nIntv].nNumNotes - 1))
        {
            bool fDifferent = false;
            for (int i = 0; i < tChordInfo.nNumIntervals && !fDifferent; i++)
            {
                if (tChordInfo.nIntervals[i] != tChordData[nIntv].nIntervals[i])
                  fDifferent = true;
            }
            if (!fDifferent)
              return (lmEChordType) nIntv;  // found matching item
        }
    }

    if ( fAllowFifthElided )
    {
        // Special case: Fifth elided
        //  TODO: improve?  ONLY ONE POSSIBLE CASE: SECOND INTERVAL MISSING
        for (int nIntv = 0; nIntv < ect_Max; nIntv++)
        {
            if ( tChordInfo.nNumIntervals == tChordData[nIntv].nNumNotes - 2)
            {
                bool fDifferent = false;
                // Interval 0 must match
                // Interval 1 is not checked!
                // For the rest... what TODO:??
                if (tChordInfo.nIntervals[0] != tChordData[nIntv].nIntervals[0])
                      fDifferent = true;
    #ifdef __WXDEBUG__
                else
                     wxLogMessage(_T(" Check fifth ellided, %d intv,  %d == %d, ITEM:%d")
                        , tChordInfo.nNumIntervals, tChordInfo.nIntervals[0], tChordData[nIntv].nIntervals[0], nIntv );
    #endif
                // For the rest... what TODO:??  for the moment: they must also be the same
                for (int i = 2; i < tChordData[nIntv].nNumNotes - 1 && !fDifferent; i++)
                {
                    if (tChordInfo.nIntervals[i] != tChordData[nIntv].nIntervals[i+1])
                    {
                      fDifferent = true;
                    }
                }
                if (!fDifferent)
                  return (lmEChordType) nIntv;  // found matching item
            }
        }
    }

    return lmINVALID_CHORD_TYPE; // invalid chord
}

// Perform n inversions/reversions over a chord with any number of intervals
//   Inversion: the lowest note is increased one octave.
//   Reversion: the highest note is reduced one octave.
// Return: number of inversions actually performed
int DoInversionsToChord( lmChordInfo* pInOutChordInfo, int nNumTotalInv)
{
    // The algoritm is simple; the inversion is performed by updating the intervals as follows:
    //  INCREMENT inversion:
    //   NewInterval[i] = OldInterval[i+1] - OldInterval[0]; // when i < N
    //   NewInterval[N] = lm_p8 - OldInterval[0]; // when i = N
    //  REVERSION (decrement inversion):
    //   NewInterval[0] = lm_p8 - OldInterval[N]; // when i = 0
    //   NewInterval[i] = OldInterval[i-1] + NewInterval[0]; // when i > 0

    lmFIntval fpIntv = 0;

    // Create a copy of the input
    lmChordInfo tInChordInfo;
    tInChordInfo =  *pInOutChordInfo;

    if (   pInOutChordInfo == NULL
        || tInChordInfo.nNumNotes > (lmFIntval)lmINTERVALS_IN_CHORD+1
        || tInChordInfo.nNumIntervals > tInChordInfo.nNumNotes-1
        || tInChordInfo.nNumIntervals < 1
        )
    {
        if (pInOutChordInfo == NULL)
           wxLogMessage(_T(" DoInversionsToChord: NULL DATA"));
        else
           wxLogMessage(_T(" DoInversionsToChord: wrong data Num Notes %d, Num Intvervals %d")
            ,tInChordInfo.nNumNotes, tInChordInfo.nNumIntervals);
        return 0;
    }

#ifdef __WXDEBUG__
        wxLogMessage(_T(" Before inversion: NumNotes %d, NumIntv %d, i0:%d i1:%d i2:%d")
            , tInChordInfo.nNumNotes, tInChordInfo.nNumIntervals
            , tInChordInfo.nIntervals[0], tInChordInfo.nIntervals[1],tInChordInfo.nIntervals[2]);
#endif

    // number of notes and intervals remains unchanged
    pInOutChordInfo->nNumNotes = tInChordInfo.nNumNotes;
    pInOutChordInfo->nNumIntervals = tInChordInfo.nNumIntervals;

    // aware:
    //  nNumNotes: number of notes
    //  nNumIntervals: number of intervals
    //  nNumIntervals-1: index of last inteval (N)
    //  nNumIntervals-2: index of penultimate interval

    if ( nNumTotalInv > 0)
    {
        for ( int nInv=0; nInv<nNumTotalInv; nInv++)
        {
            int i=0;
            for ( i=0; i<tInChordInfo.nNumIntervals-1; i++)
            {
                pInOutChordInfo->nIntervals[i] = tInChordInfo.nIntervals[i+1] - tInChordInfo.nIntervals[0];
            }
            pInOutChordInfo->nIntervals[tInChordInfo.nNumIntervals-1] = lm_p8 - tInChordInfo.nIntervals[0];

        #ifdef __WXDEBUG__
                wxLogMessage(_T(" After adding inversion %d : NumNotes %d, NumIntv %d, i0:%d i1:%d i2:%d")
                    ,nInv,tInChordInfo.nNumNotes,tInChordInfo.nNumIntervals
                    , tInChordInfo.nIntervals[0], tInChordInfo.nIntervals[1],tInChordInfo.nIntervals[2]);
        #endif
        }
    }
    else if ( nNumTotalInv < 0)
    {
        for ( int nInv=0; nInv<-nNumTotalInv; nInv++)
        {
            int i=0;
            pInOutChordInfo->nIntervals[0] = lm_p8 - tInChordInfo.nIntervals[tInChordInfo.nNumIntervals-1];
            for ( i=1; i<tInChordInfo.nNumIntervals; i++)
            {
                pInOutChordInfo->nIntervals[i] = tInChordInfo.nIntervals[i-1] + pInOutChordInfo->nIntervals[0];
            }
        #ifdef __WXDEBUG__
                wxLogMessage(_T(" After reducing inversion %d : NumNotes %d, NumIntv %d, i0:%d i1:%d i2:%d")
                    ,nInv,tInChordInfo.nNumNotes,tInChordInfo.nNumIntervals
                    , tInChordInfo.nIntervals[0], tInChordInfo.nIntervals[1],tInChordInfo.nIntervals[2]);
        #endif
        }
    }
    return nNumTotalInv;
}


// Improvement of GetChordTypeFromIntervals: also consider possible inversions
//
//    Algorithm:
//
// Repeat while not valid chord type found and inversions are possible
//    (possible inversions = number of intervals - 1 = number of notes - 2)
//   Search the chord type that matches the specified intervals
//    found: return chord type
//    not found: Do one inversion
//
lmEChordType GetChordTypeAndInversionsFromIntervals( lmChordInfo &tChordInfo)
{
    lmEChordType nType = lmINVALID_CHORD_TYPE;
    int nNumPossibleInversions = tChordInfo.nNumIntervals;
    tChordInfo.nNumInversions = 0;
    do
    {
        nType = GetChordTypeFromIntervals( tChordInfo );

        if (nType != lmINVALID_CHORD_TYPE)
             return nType;

        // Try with inversions
        if (tChordInfo.nNumInversions >= nNumPossibleInversions)
            return lmINVALID_CHORD_TYPE; // invalid chord
        else
        {
            // Remove one inversion from chord
            int nInvOk = DoInversionsToChord(&tChordInfo, -1);
            tChordInfo.nNumInversions++;
        }

    }while ( nType == lmINVALID_CHORD_TYPE );

    return nType;
}
--------------------------------*/



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
void ChordInfoBox::DisplayChordInfo(lmScore* pScore, lmChordDescriptor* pChordDsct, wxColour colour, wxString &sText)
{
    if (pChordDsct == NULL || pChordDsct->nNumChordNotes < 1)
    {
        wxLogMessage(_T(" DisplayChordInfo ERROR: NO notes to attach the textbox"));
        return;  // todo: improvement: in this case, display a box but not attached to any note
    }
    int nNumChordNotes  = pChordDsct->nNumChordNotes;
    lmTextStyle* pStyle = pScore->GetStyleName(*m_pFontInfo);

    // Display chord info in score with a line and text
    assert(nNumChordNotes > 0);
    assert(nNumChordNotes < 20);

    for (int i = 0; i<nNumChordNotes; i++)
    {
        assert(pChordDsct->pChordNotes[i] != NULL);
        pChordDsct->pChordNotes[i]->SetColour(colour);
    }

    // Line end: the first note
    lmStaffObj* cpSO = pChordDsct->pChordNotes[nNumChordNotes-1];
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



//
// class lmChordDescriptor
//
// Todo: consider to merge lmChordDescriptor and lmChord into a single class
//
wxString lmChordDescriptor::ToString()
{

    wxString sStr;
    int nNotes = nNumChordNotes;
    sStr += pChord->GetNameFull().c_str();
// Note that the number of notes and the number of inversions is already in the description from GetNameFull
//    sStr += wxString::Format(_T("%d"), pChord->GetNumNotes());
//    sStr += _(" notes, ");
//    sStr += wxString::Format(_T("%d"), pChord->GetInversion());
//    sStr += _(" inversions, ");
    if ( pChord->GetElision() > 0 )
    {
      sStr += wxString::Format(_T(", %d"), pChord->GetElision());
      sStr += _(" elisions");
    }
    sStr += _(", Notes:");
    for (int nN = 0; nN<nNotes; nN++)
    {
        sStr += _T(" ");
        sStr += pChordNotes[nN]->GetPrintName();
    }
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

void lmActiveNotes::GetChordDescriptor(lmChordDescriptor* ptChordDescriptor)
{
     std::list<lmActiveNoteInfo*>::iterator it;
     int nCount = 0;
     for(it=m_ActiveNotesInfo.begin(); it != m_ActiveNotesInfo.end(); ++it, nCount++)
     {
         ptChordDescriptor->pChordNotes[nCount] = (*it)->pNote;
     }
     ptChordDescriptor->nNumChordNotes = nCount;
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
    wxString sRetStr = _T("");
    wxString auxStr = _T("");
    int nNumNotes = GetNumActiveNotes();
    sRetStr = wxString::Format(_(" [Time: %f, %d Active notes: ") , m_rCurrentTime, nNumNotes);

    std::list<lmActiveNoteInfo*>::iterator it;
    for(it=m_ActiveNotesInfo.begin(); it != m_ActiveNotesInfo.end(); ++it)
    {
        auxStr = wxString::Format(_T(" %s  End time: %f ")
            , (*it)->pNote->GetPrintName().c_str(), (*it)->rEndTime  );
        sRetStr += auxStr;
    }
    sRetStr += _T(" ]");
    return sRetStr;
}


//
// class lmRuleList
//

lmRuleList::lmRuleList(lmChordDescriptor* pChD, int nNumChords)
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
        _("No parallel motion of perfect octaves, perfect fifths, and unisons") );
    AddRule( new lmRuleNoResultingFifthOctaves(),
        _("No resulting fifths and octaves") );
    AddRule( new lmRuleNoVoicesCrossing(),
        _("Do not allow voices crossing. No duplicates (only for root position and root duplicated)") );
    AddRule( new lmNoIntervalHigherThanOctave(),
        _("Voices interval not greater than one octave (except bass-tenor)") );
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
    sResultDetails = _("Rule: No parallel motion ");
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
        nNumNotes = (m_pChordDescriptor[nC].nNumChordNotes < m_pChordDescriptor[nC-1].nNumChordNotes?
                     m_pChordDescriptor[nC].nNumChordNotes:  m_pChordDescriptor[nC-1].nNumChordNotes);
        for (int nN=0; nN<nNumNotes; nN++)
        {
            nVoiceInterval[nN] =  ( m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                - m_pChordDescriptor[nC-1].pChordNotes[nN]->GetFPitch() ) % lm_p8 ;

            // check if it is parallel with any previous note
            for (int i=0; i<nN; i++)
            {
                if ( nVoiceInterval[i] == nVoiceInterval[nN])
                {
                     lmFIntval nInterval = abs( 
                         m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                         - m_pChordDescriptor[nC].pChordNotes[i]->GetFPitch() );
                     int nIntervalNumber = FIntval_GetNumber(nInterval);

                     wxLogMessage(_(" >>> Check parallel motion in chord %d, notes:%d %d, INTERVAL:%d(%s) {%d}")
		               ,nC, i,  nN,  nIntervalNumber
                       , FIntval_GetIntvCode(nInterval).c_str()
                       , nInterval);

                    if ( nIntervalNumber == 1 || nIntervalNumber == 5 )
                    {
                        wxString sType =  FIntval_GetName(nInterval);
                        pNumFailuresInChord[nC] = pNumFailuresInChord[nC]  +1;

                        int nFullVoiceInterval = abs ( m_pChordDescriptor[nC].pChordNotes[i]->GetFPitch()
                              - m_pChordDescriptor[nC-1].pChordNotes[i]->GetFPitch() );

//TODO: accumulate messages?                        sResultDetails += wxString::Format(
                        sResultDetails = wxString::Format(
                            _("Parallel motion of %s, chords: %d, %d; v%d %s-->%s, v%d %s-->%s, Interval: %s")
                            ,sType.c_str(),  (nC-1)+1, (nC)+1
                            , m_pChordDescriptor[nC].pChordNotes[i]->GetVoice()
                            , m_pChordDescriptor[nC-1].pChordNotes[i]->GetPrintName().c_str()
                            , m_pChordDescriptor[nC].pChordNotes[i]->GetPrintName().c_str()
                            , m_pChordDescriptor[nC].pChordNotes[nN]->GetVoice()
                            , m_pChordDescriptor[nC-1].pChordNotes[nN]->GetPrintName().c_str()
                            , m_pChordDescriptor[nC].pChordNotes[nN]->GetPrintName().c_str()
                            , FIntval_GetIntvCode(nInterval).c_str()
                            );

                        wxLogMessage( sResultDetails );


                        pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore(), &m_pChordDescriptor[nC], colour, sResultDetails);


                        // display failing notes in red   TODO: this could be improved...
                        m_pChordDescriptor[nC].pChordNotes[nN]->SetColour(*wxCYAN);
                        m_pChordDescriptor[nC].pChordNotes[i]->SetColour(*wxBLUE);
                        m_pChordDescriptor[nC-1].pChordNotes[nN]->SetColour(*wxCYAN);
                        m_pChordDescriptor[nC-1].pChordNotes[i]->SetColour(*wxBLUE);

                        DrawArrow(
                             m_pChordDescriptor[nC-1].pChordNotes[nN],
                             m_pChordDescriptor[nC].pChordNotes[nN],
                             wxColour(*wxRED) );
                        DrawArrow(
                             m_pChordDescriptor[nC-1].pChordNotes[i],
                             m_pChordDescriptor[nC].pChordNotes[i],
                             wxColour(*wxRED) );


                        m_pChordDescriptor[nC].tChordErrors.SetError( this->GetRuleId(), true);
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
    // Forbidden to arrive to a fifth or octave by means of a direct movement ( both: same delta sign)
    // exceptions:
    //  - voice is soprano (TODO: consider: tenor, contralto??) and distance is 2th
    //  - TODO: consider: fifth and one sound existed??
    sResultDetails = _("Rule: No resulting fifth/octaves ");
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
        wxLogMessage(_T("Check chord %d "), nC);

        pNumFailuresInChord[nC] = 0;

        // num notes: min of both chords
        nNumNotes = (m_pChordDescriptor[nC].nNumChordNotes < m_pChordDescriptor[nC-1].nNumChordNotes?
                     m_pChordDescriptor[nC].nNumChordNotes:  m_pChordDescriptor[nC-1].nNumChordNotes);

        // for all the notes in the chord...
        for (int nN=0; nN<nNumNotes; nN++)
        {
            // check type of movement with any previous note
            for (int i=0; i<nN; i++)
            {
                nVoiceMovementType = GetHarmonicMovementType(
                  m_pChordDescriptor[nC-1].pChordNotes[nN], m_pChordDescriptor[nC].pChordNotes[nN],
                  m_pChordDescriptor[nC-1].pChordNotes[i], m_pChordDescriptor[nC].pChordNotes[i]);


                lmFIntval nInterval = abs( 
                         m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                         - m_pChordDescriptor[nC].pChordNotes[i]->GetFPitch() );
                int nIntervalNumber = FIntval_GetNumber(nInterval);

                wxLogMessage(_(" Notes: %s-->%s %s-->%s Movement type:%d  INTERVAL:%d (%s)")
                        , m_pChordDescriptor[nC-1].pChordNotes[nN]->GetPrintName().c_str()
                        , m_pChordDescriptor[nC].pChordNotes[nN]->GetPrintName().c_str()
                        , m_pChordDescriptor[nC-1].pChordNotes[i]->GetPrintName().c_str()
                        , m_pChordDescriptor[nC].pChordNotes[i]->GetPrintName().c_str()
                        , nVoiceMovementType, nIntervalNumber, FIntval_GetIntvCode(nInterval).c_str());

                if ( nVoiceMovementType == lm_eDirectMovement && ( nIntervalNumber == 1 || nIntervalNumber == 5 )  )
                {
                    // Incorrect, unless: voice interval is 2th and voice is > 0 (not BASS)
                     lmFIntval nVoiceInterval = abs( 
                         m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                         - m_pChordDescriptor[nC].pChordNotes[i]->GetFPitch() ) ;
                     int nVoiceIntervalNumber = FIntval_GetNumber(nVoiceInterval);

                     if (  nVoiceIntervalNumber == 2 && nN > 0 )
                     {
                        wxLogMessage(_T(" Exception!, voice not BASS and voice interval is 2th!  "));
                     }
                     else
                     {
                        wxString sType;
                        if (nInterval > 80) // current limitation in FIntval_GetName
                           sType = _("higher than 2 octaves");
                        else
                           sType =  FIntval_GetName(nInterval);

                        sResultDetails = wxString::Format(
               _("Direct movement resulting %s. Chords:%d,%d. Voices:%d %s-->%s and %d %s-->%s. Interval: %s")
               , sType.c_str(), (nC-1)+1, (nC)+1
               , m_pChordDescriptor[nC].pChordNotes[nN]->GetVoice()
               , m_pChordDescriptor[nC-1].pChordNotes[nN]->GetPrintName().c_str()
               , m_pChordDescriptor[nC].pChordNotes[nN]->GetPrintName().c_str()
               , m_pChordDescriptor[nC].pChordNotes[i]->GetVoice()
               , m_pChordDescriptor[nC-1].pChordNotes[i]->GetPrintName().c_str()
               , m_pChordDescriptor[nC].pChordNotes[i]->GetPrintName().c_str()
               , FIntval_GetIntvCode(nInterval).c_str());

                        DrawArrow(
                             m_pChordDescriptor[nC-1].pChordNotes[nN],
                             m_pChordDescriptor[nC].pChordNotes[nN],
                             wxColour(*wxCYAN) );
                         DrawArrow(
                             m_pChordDescriptor[nC-1].pChordNotes[i],
                             m_pChordDescriptor[nC].pChordNotes[i],
                             wxColour(*wxCYAN) );

                        wxLogMessage( sResultDetails );


                        pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
                          , &m_pChordDescriptor[nC], colour, sResultDetails);

                        // display failing notes in red  (TODO: improve error display?)
                        m_pChordDescriptor[nC].pChordNotes[nN]->SetColour(*wxRED);
                        m_pChordDescriptor[nC].pChordNotes[i]->SetColour(*wxRED);


                        m_pChordDescriptor[nC].tChordErrors.SetError( this->GetRuleId(), true);
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
        if ( m_pChordDescriptor[nC].pChord->GetInversion() != 0 )
        {
            wxLogMessage(_(" Rule not applicable: not root position: %d inversions"), m_pChordDescriptor[nC].pChord->GetInversion());
            //@@@ todo remove: return 0;
            continue;
        }
        if (  m_pChordDescriptor[nC].pChord->GetInversion() == 0 && ! m_pChordDescriptor[nC].pChord->IsRootDuplicated() )
        {
            wxLogMessage(_(" Rule not applicable: not root position but root note not duplicated"));
            //@@@ todo remove: return 0;
            continue;
        }

        nNumNotes = m_pChordDescriptor[nC].nNumChordNotes ;

        // for all the notes in the chord...
        for (int nN=0; nN<nNumNotes; nN++)
        {
            // check crossing  TODO: ENSURE VOICES HAVE A VALUE!!
            for (int i=1; i<nN; i++)
            {
                nVoice[1] = m_pChordDescriptor[nC].pChordNotes[nN]->GetVoice();
                nVoice[0] = m_pChordDescriptor[nC].pChordNotes[i]->GetVoice();
                nPitch[1] = m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch();
                nPitch[0] = m_pChordDescriptor[nC].pChordNotes[i]->GetFPitch();
                if (  nVoice[1] > nVoice[0] &&
                      nPitch[1] <= nPitch[0] )
                {
                    sResultDetails = wxString::Format(
                        _("Chord:%d: Voice crossing.  Voice%d(%s) <= Voice%d(%s) ")
                    , (nC)+1
                    , nVoice[1], m_pChordDescriptor[nC].pChordNotes[nN]->GetPrintName().c_str()
                    , nVoice[0], m_pChordDescriptor[nC].pChordNotes[i]->GetPrintName().c_str()
                    );

                    wxLogMessage( sResultDetails );

                    pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
                      , &m_pChordDescriptor[nC], colour, sResultDetails);

                     // display failing notes in red  (TODO: mejorar indicacion de errores)
                     m_pChordDescriptor[nC].pChordNotes[nN]->SetColour(*wxRED);
                     m_pChordDescriptor[nC].pChordNotes[i]->SetColour(*wxRED);

                     m_pChordDescriptor[nC].tChordErrors.SetError( this->GetRuleId(), true);
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
    sResultDetails = _(" Rule: no interval higher than octave");
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
        wxLogMessage(_("Check chord %d "), nC);

        pNumFailuresInChord[nC] = 0;

        // Apply rule only if:
        //  chord in root position (o inversions)
        //  root note is duplicated
        if ( m_pChordDescriptor[nC].pChord->GetInversion() != 0 )
        {
            wxLogMessage(_T(" Rule not applicable: not root position: %d inversions"), m_pChordDescriptor[nC].pChord->GetInversion());
            //@@@ todo remove: return 0;
            continue;
        }
        if (  m_pChordDescriptor[nC].pChord->GetInversion() == 0 && ! m_pChordDescriptor[nC].pChord->IsRootDuplicated() )
        {
            wxLogMessage(_T(" Rule not applicable: not root position but root note not duplicated"));
            //@@@ todo remove: return 0;
            continue;
        }

        nNumNotes = m_pChordDescriptor[nC].nNumChordNotes ;

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
            nInterval = m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                            - m_pChordDescriptor[nC].pChordNotes[nN-1]->GetFPitch();

            wxLogMessage(_T("  Notes %d - %d: interval: %d "), nN, nN-1, nInterval);

            if (  nInterval > nLimit )
            {
                sResultDetails = wxString::Format(
                _("Chord %d: Interval %s higher than octave between voices %d (%s) and %d (%s)")
                , (nC)+1
                , FIntval_GetIntvCode(nInterval).c_str()
                , m_pChordDescriptor[nC].pChordNotes[nN]->GetVoice()
                , m_pChordDescriptor[nC].pChordNotes[nN]->GetPrintName().c_str()
                , m_pChordDescriptor[nC].pChordNotes[nN-1]->GetVoice()
                , m_pChordDescriptor[nC].pChordNotes[nN-1]->GetPrintName().c_str()
                );

                wxLogMessage( sResultDetails );

                pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
                  , &m_pChordDescriptor[nC], colour, sResultDetails);

                 // display failing notes in red  (TODO: mejorar indicacion de errores)
                m_pChordDescriptor[nC].pChordNotes[nN]->SetColour(*wxRED);
                m_pChordDescriptor[nC].pChordNotes[nN-1]->SetColour(*wxRED);

                DrawArrow(
                     m_pChordDescriptor[nC].pChordNotes[nN-1],
                     m_pChordDescriptor[nC].pChordNotes[nN],
                     wxColour(*wxBLUE) );

                 m_pChordDescriptor[nC].tChordErrors.SetError( this->GetRuleId(), true);
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
void lmRuleList::SetChordDescriptor(lmChordDescriptor* pChD, int nNumChords)
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
