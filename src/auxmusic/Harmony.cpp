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

//
// Global functions
//
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


//
// Message box to display the results if the chord analysis
// 
// Remember:
//      x: relative to object; positive: right
//      y: relative to top line; positive: down
ChordInfoBox::ChordInfoBox(wxSize* pSize, lmFontInfo* pFontInfo, std::list<lmMarkup*>* pMarkup
                           , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement)
{
    Settings(pSize, pFontInfo, pMarkup, nBoxX, nBoxY, nLineX, nLineY, nBoxYIncrement);
}
void ChordInfoBox::Settings(wxSize* pSize, lmFontInfo* pFontInfo, std::list<lmMarkup*>* pMarkup
                            , int nBoxX, int nBoxY, int nLineX, int nLineY, int nBoxYIncrement)
{
    m_ntConstBoxXstart = nBoxX;  
    m_ntConstInitialBoxYStart = nBoxY;
    m_ntConstLineXstart = nLineX; 
    m_ntConstLineYStart = nLineY;
    m_ntConstBoxYIncrement = nBoxYIncrement;
    m_pMarkup = pMarkup;
    m_pFontInfo = pFontInfo;
    m_pSize = pSize;

    assert(m_pMarkup != NULL);
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

	lmMarkup* pError = new lmMarkup(cpSO, pTxtBox);
    m_pMarkup->push_back(pError);

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
    lmScoreLine* pLine = new lmScoreLine(xtStart, ytStart, xtEnd, ytEnd, 2, lm_eLineCap_None,
                                         lm_eLineCap_Arrowhead, lm_eLine_Solid,
                                         color);
    pNote1->SetColour(color);
    pNote2->SetColour(color);
	pNote1->AttachAuxObj(pLine);
}



//
// class lmChordDescriptor
//

wxString lmChordDescriptor::ToString()
{
    wxString sStr;
    int nNotes = nNumChordNotes;
    sStr = wxString::Format(_T(" Chord: %s, %d notes, %d invers, %d elis ")
        , pChord->GetNameFull().c_str()
        , pChord->GetNumNotes()
            , pChord->GetInversion()
            ,  pChord->GetElision());
    sStr += _T(" Notes:");
    for (int nN = 0; nN<nNotes; nN++)
    {
        sStr += _T(" ");
        sStr += NoteId( *pChordNotes[nN] );
    }
    return sStr;
}


//
// class lmActiveNotes
//

lmActiveNotes::lmActiveNotes()
{
    r_current_time = 0.0f;
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

void lmActiveNotes::SetTime(float r_new_current_time)
{
    r_current_time = r_new_current_time;
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
         if ( ! IsHigherTime(  (*it)->rEndTime, r_current_time ) )
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
    sRetStr = wxString::Format(_T(" [Time: %f, %d ActNotes: ") , r_current_time, nNumNotes);

    std::list<lmActiveNoteInfo*>::iterator it;
    for(it=m_ActiveNotesInfo.begin(); it != m_ActiveNotesInfo.end(); ++it)
    {
        auxStr = wxString::Format(_T(" %s  EndT: %f ")
            , NoteId( *(*it)->pNote).c_str(), (*it)->rEndTime  );
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

// TODO: ADD MORE HARMONY RULES !!
//        To add a rule:
//        1) Create the class (recommended to use the macro LM_CREATE_CHORD_RULE)
//        2) Add an instance in AddRule
//        3) Implement the Evaluate method
//////////////////////////////////////////////////////////////////////

// Todo: select the applicable rules somehow?  use IsEnabled?

//
// Add rules
//

LM_CREATE_CHORD_RULE(lmRuleNoParallelMotion, lmCVR_NoParallelMotion,
     "No parallel motion of perfect octaves, perfect fifths, and unisons")
LM_CREATE_CHORD_RULE(lmRuleNoResultingFifthOctaves, lmCVR_NoResultingFifthOctaves,
     "No resulting fifths and octaves")
LM_CREATE_CHORD_RULE(lmRuleNoVoicesCrossing, lmCVR_NoVoicesCrossing,
     "Do not allow voices crossing. No duplicates (only for root position and root duplicated)")
LM_CREATE_CHORD_RULE(lmNoIntervalHigherThanOctave, lmCVR_NoIntervalHigherThanOctave,
     "Voices interval not greater than one octave (except bass-tenor)")

void lmRuleList::CreateRules()
{
    AddRule( new lmRuleNoParallelMotion() );
    AddRule( new lmRuleNoResultingFifthOctaves() );
    AddRule( new lmRuleNoVoicesCrossing() );
    AddRule( new lmNoIntervalHigherThanOctave() );
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

lmRule::lmRule(int nRuleID, wxString sDescription)
{
    m_fEnabled = true;
    m_pChordDescriptor = NULL;
    m_sDescription = sDescription;
    m_sDetails = _T("nothing");
    m_nRuleId = nRuleID;
};


//
// Definition of rules of harmony
//

// return number of errors
int lmRuleNoParallelMotion::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[], ChordInfoBox* pBox )
{
    sResultDetails = _T("lmRule5ThNotDoubled::Evaluate");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmRuleNoParallelMotion Rule %d  m_pChordDescriptor NULL "));
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
                    int nDistance = m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                        - m_pChordDescriptor[nC].pChordNotes[i]->GetFPitch() ;

                     int nNormalizedDistance = abs(nDistance) % lm_p8;

                    wxLogMessage(_T(" >>> Parallel motion in chord %d, notes:%d %d, intv:%d, DIST:%d")
		               ,nC, i,  nN, nVoiceInterval[i],  nDistance );

                    if ( nNormalizedDistance == 0 || nNormalizedDistance == lm_p5 )
                    {
                        wxString sType =  ( nDistance == 0?  _T(" octave/unison"): _T(" fifth "));
                        pNumFailuresInChord[nC] = pNumFailuresInChord[nC]  +1;

                        int nFullVoiceInterval = abs ( m_pChordDescriptor[nC].pChordNotes[i]->GetFPitch() 
                              - m_pChordDescriptor[nC-1].pChordNotes[i]->GetFPitch() );

//TODO: ¿ACUMULAR MENSAJES?                        sResultDetails += wxString::Format(
                        sResultDetails = wxString::Format(
                            _T("Parallel motion of %s, chords: %d, %d; v%d %s-->%s, v%d %s-->%s, Distance: %s(%d), Interval: %s(%d)")
                            ,sType.c_str(),  (nC-1)+1, (nC)+1
                            ,(i)+1,  NoteId(*m_pChordDescriptor[nC-1].pChordNotes[i]).c_str(), NoteId(*m_pChordDescriptor[nC].pChordNotes[i]).c_str()
                            ,(nN)+1, NoteId(*m_pChordDescriptor[nC-1].pChordNotes[nN]).c_str(),  NoteId(*m_pChordDescriptor[nC].pChordNotes[nN]).c_str()
                            , FIntval_GetIntvCode(nDistance).c_str(), nDistance
                            , FIntval_GetIntvCode(nFullVoiceInterval).c_str(), nVoiceInterval[i]
                            );

                        wxLogMessage( sResultDetails );


                        pBox->DisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore(), &m_pChordDescriptor[nC], colour, sResultDetails);


                        // display failing notes in red   TODO: mejorar?
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

    return nErrCount;
}

// return number of errors
int lmRuleNoResultingFifthOctaves::Evaluate(wxString& sResultDetails
                                            , int pNumFailuresInChord[], ChordInfoBox* pBox)
{
    // Forbidden to arrive to a fifth or octave by means of a direct movement ( both same delta sign)
    // exceptions:
    //  - voice is soprano (TODO: tenor, contralto??) and distance is 2th
    //  - TODO: fifth and one sound existed??
    sResultDetails = _T("lmRuleNoResultingFifthOctaves::Evaluate");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmRuleNoResultingFifthOctaves Rule %d  m_pChordDescriptor NULL "));
        return 0;
    }

    int nDifColour = this->GetRuleId() * 2;   //todo: pensar forma de cambiar algo el color en cada regla?
    int nTransp = 128; // todo: ¿usar transparencia?
    wxColour colour( 200, 20+nDifColour, 20+nDifColour, nTransp);
    int nErrCount = 0;
    int nNumNotes;
    int nVoiceMovementType, nDistance, nInterval;
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

                nDistance  = abs (
                          m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                        - m_pChordDescriptor[nC].pChordNotes[i]->GetFPitch() );

                wxLogMessage(_T(" Notes: %s-->%s %s-->%s Mov tpye:%d  distance:%d")
 , NoteId(*m_pChordDescriptor[nC-1].pChordNotes[nN]), NoteId(*m_pChordDescriptor[nC].pChordNotes[nN]).c_str()
 , NoteId(*m_pChordDescriptor[nC-1].pChordNotes[i]), NoteId(*m_pChordDescriptor[nC].pChordNotes[i]).c_str()
 ,nVoiceMovementType, nDistance);

                if ( nVoiceMovementType == lm_eDirectMovement && ( nDistance == 0 || nDistance == lm_p5 )  )
                {
                    // Incorrect, unless: interval is 2th and voice is > 0 (not BASS)
                    nInterval  = abs (
                          m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                        - m_pChordDescriptor[nC-1].pChordNotes[nN]->GetFPitch() );

                    if (  (nInterval == lm_m2 || nInterval == lm_M2)
                          && nN > 0 )
                    {
                       wxLogMessage(_T(" Exception!, voice not BASS and interval is 2th!  "));
                    }
                    else
                    {
                        wxString sType =  ( nDistance == 0?  _T(" octave/unison"): _T(" fifth "));

                        sResultDetails = wxString::Format(
               _T("Direct movement resulting %s. Chords:%d,%d. Voices:%d %s-->%s and %d %s-->%s. Distance: %s(%d), Interval: %s(%d)")
               , sType.c_str(), (nC-1)+1, (nC)+1
               , (nN)+1, NoteId(*m_pChordDescriptor[nC-1].pChordNotes[nN]).c_str(), NoteId(*m_pChordDescriptor[nC].pChordNotes[nN]).c_str()
               , (i)+1, NoteId(*m_pChordDescriptor[nC-1].pChordNotes[i]).c_str(), NoteId(*m_pChordDescriptor[nC].pChordNotes[i]).c_str()
               , FIntval_GetIntvCode(nDistance).c_str(), nDistance
               , FIntval_GetIntvCode(nInterval).c_str(), nInterval);

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

                        // display failing notes in red  (TODO: mejorar indicacion de errores)
                        m_pChordDescriptor[nC].pChordNotes[nN]->SetColour(*wxRED);
                        m_pChordDescriptor[nC].pChordNotes[i]->SetColour(*wxRED);


                        m_pChordDescriptor[nC].tChordErrors.SetError( this->GetRuleId(), true);
                        nErrCount++;
                    }
                }

            }
        }
    }

    return nErrCount;
}


// return number of errors
int lmRuleNoVoicesCrossing::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[]
                                     , ChordInfoBox* pBox)
{
    sResultDetails = _T("lmRuleNoVoicesCrossing::Evaluate");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmRuleNoVoicesCrossing Rule %d  m_pChordDescriptor NULL "));
        return 0;
    }
    int nDifColour = this->GetRuleId() * 2;   //todo: pensar forma de cambiar algo el color en cada regla?
    int nTransp = 128; // todo: ¿usar transparencia?
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
            wxLogMessage(_T(" rule not applicable: not root position: %d inversions"), m_pChordDescriptor[nC].pChord->GetInversion());
            return 0;
        }
        if (  m_pChordDescriptor[nC].pChord->GetInversion() == 0 && ! m_pChordDescriptor[nC].pChord->IsRootDuplicated() )
        {
            wxLogMessage(_T(" rule not applicable: not root position but root note not duplicated"));
            return 0;
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
                    _T("Voice crossing. Chord:%d. Voice%d(%s) <= Voice%d(%s) ")
                    ,  (nC)+1
                    , nVoice[1], NoteId(*m_pChordDescriptor[nC].pChordNotes[nN]).c_str()
                    , nVoice[0], NoteId(*m_pChordDescriptor[nC].pChordNotes[i]).c_str()
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

    return nErrCount;
}



int lmNoIntervalHigherThanOctave::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[]
                                           , ChordInfoBox* pBox)
{
    sResultDetails = _T("lmNoIntervalHigherThanOctave::Evaluate");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmNoIntervalHigherThanOctave Rule %d  m_pChordDescriptor NULL "));
        return 0;
    }
    int nDifColour = this->GetRuleId() * 2;   //todo: think: change color in each rule?
    int nTransp = 128; // todo: use transparency?
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
        if ( m_pChordDescriptor[nC].pChord->GetInversion() != 0 )
        {
            wxLogMessage(_T(" rule not applicable: not root position: %d inversions"), m_pChordDescriptor[nC].pChord->GetInversion());
            return 0;
        }
        if (  m_pChordDescriptor[nC].pChord->GetInversion() == 0 && ! m_pChordDescriptor[nC].pChord->IsRootDuplicated() )
        {
            wxLogMessage(_T(" rule not applicable: not root position but root note not duplicated"));
            return 0;
        }

        nNumNotes = m_pChordDescriptor[nC].nNumChordNotes ;

        // TODO: confirm: only applicable to 4 voices
        if ( nNumNotes !=  4 )
        {
            wxLogMessage(_T(" rule not applicable: not 4 notes (%d)"), nNumNotes);
            return 0;
        }
        // for all the notes in the chord...
        for (int nN=2; nN<4; nN++)
        {
            // TODO: ensure correspondance VOICE - order
            nInterval = m_pChordDescriptor[nC].pChordNotes[nN]->GetFPitch()
                            - m_pChordDescriptor[nC].pChordNotes[nN-1]->GetFPitch();

            wxLogMessage(_T("  Notes %d - %d: interval: %d "), nN, nN-1, nInterval);

            if (  nInterval >= lm_p8 )
            {
                sResultDetails = wxString::Format(
                _T(" Interval %s higher than octave between notes %d (%s) and %d (%s). Chord %d")
                ,  FIntval_GetIntvCode(nInterval).c_str()
                , (nN)+1, NoteId(*m_pChordDescriptor[nC].pChordNotes[nN]).c_str()
                , (nN-1)+1, NoteId(*m_pChordDescriptor[nC].pChordNotes[nN-1]).c_str()
                ,(nC)+1
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
            }
        }
    }

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


bool lmRuleList::AddRule(lmRule* pNewRule )
{
    int nRuleId = pNewRule->GetRuleId();
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
        wxLogMessage(_T(" AddRule: INVALID lmChordValidationRules: %d"), nRuleId );
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
