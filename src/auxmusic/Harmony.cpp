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


// TODO: ESTO ES SOLO PROVISIONAL; PENSAR CÓMO MOSTRAR LA INFORMACION
#include "../app/MainFrame.h"
extern lmMainFrame* GetMainFrame();
#include "../app/ScoreDoc.h"
static const int ntDisXstart = 0;
static const int ntDisXend = -200;
static const int ntDisYstart = 40;
static const int ntDisYend = -120;
void  HDisplayChordInfo(lmScore* pScore, lmChordDescriptor*  pChordDsct
                                           , wxColour colour, wxString &sText, bool reset)
{
    int nNumChordNotes  = pChordDsct->nNumChordNotes;
    // Remember: all 'y' positions are relative to top line (5th line of
    //   first staff). 'x' positions are relative to current object position.
    lmTenths ntxStart = ntDisXstart;  // fijo; relativo al usuario
    lmTenths ntxEnd = ntDisXend; // fijo
    static lmTenths ntyStart = ntDisYstart;  // relativo a top line; positivo: abajo
    static lmTenths ntyEnd = ntDisYend;  // negativo: arriba. Se baja en cada uso

	lmTenths nTxPos = ntxEnd + 10;
    lmTenths nTyPos = ntyEnd + 10;
    if ( reset )
    {
        // only reset
        ntyStart = ntDisYstart;  
        ntyEnd = ntDisYend; 
        return;
    }

    //define the font to use for texts
    lmFontInfo tFont = {_("Comic Sans MS"), 6, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL};
    lmTextStyle* pStyle = pScore->GetStyleName(tFont);

    // Display chord info in score with a line and text
    assert(nNumChordNotes > 0);
    assert(nNumChordNotes < 20);
    assert(pChordDsct != NULL);

    for (int i = 0; i<nNumChordNotes; i++)
    {
        assert(pChordDsct->pChordNotes[i] != NULL);
        if ( pChordDsct->pChordNotes[i]->GetComponentColour() == *wxGREEN )
          pChordDsct->pChordNotes[i]->SetColour(colour);
    }

    // Green line pointing to the chord
    // Remember: all 'y' positions are relative to top line (5th line of
    //   first staff). 'x' positions are relative to current object position.
    lmStaffObj* cpSO =pChordDsct->pChordNotes[nNumChordNotes-1];
//not good for linux!    lmAuxObj* pTxtBox = cpSO->AttachTextBox(lmTPoint(nTxPos, nTyPos), lmTPoint(ntxStart, ntyStart),
//                                            sText, pStyle,	wxSize(500, 60), colour);
    lmTPoint lmTP1(nTxPos, nTyPos);
    lmTPoint lmTP2(ntxStart, ntyStart);
    wxSize size(510, 60);
    lmAuxObj* pTxtBox = cpSO->AttachTextBox(lmTP1, lmTP2,
                                            sText, pStyle,	size, colour);

/*---- TODO: que hacer con esto???
	lmMarkup* pError = new lmMarkup(cpSO, pTxtBox);
    m_markup.push_back(pError);
---*/
    ntyEnd +=50; // y positions are NOT relative; change each time

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
         // AWARE: EQUAL time considered as finished  (TODO @@CONFIRM)
         if ( ! IsHigherTime(  (*it)->rEndTime, r_current_time ) )
         {
             delete *it;
             it = m_ActiveNotesInfo.erase(it);  // aware: "it = " needed to avoid crash in loop....
         }
         else 
             it++;
     }
}

// TODO: usado para debug; ver si vale la pena dejarlo...
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

// TODO: COMPLETAR LA LISTA DE REGLAS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//        Para crear una regla:
//        1) Crear la clase (recomendado usar la macro LM_CREATE_CHORD_RULE)
//        2) Añadir una instancia en AddRule
//        3) Implementar el metodo Evaluate
//////////////////////////////////////////////////////////////////////

//
// Add rules
//

LM_CREATE_CHORD_RULE(lmRuleNoParallelMotion, lmCVR_NoParallelMotion,
     "No parallel motion of perfect octaves, perfect fifths, and unisons")
LM_CREATE_CHORD_RULE(lmRuleNoResultingFifthOctaves, lmCVR_NoResultingFifthOctaves,
     "No resulting fifths and octaves")

void lmRuleList::CreateRules()
{
    AddRule( new lmRuleNoParallelMotion() );
    AddRule( new lmRuleNoResultingFifthOctaves() );
}



//
// lmChordError
//
// TODO: VER SI ES PRACTICO ESTE SISTEMA DE REPRESENTAR CADA ERROR EN UN BIT
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
//TODO: remove?    wxLogMessage(_T("SetError %d ,  ErrList:%u ,  %u"), nBrokenRule,  nErrList,   fVal );
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
int lmRuleNoParallelMotion::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[])
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
                            ,(i)+1,  NoteId(*m_pChordDescriptor[nC-1].pChordNotes[i]), NoteId(*m_pChordDescriptor[nC].pChordNotes[i])
                            ,(nN)+1, NoteId(*m_pChordDescriptor[nC-1].pChordNotes[nN]),  NoteId(*m_pChordDescriptor[nC].pChordNotes[nN])
                            , FIntval_GetIntvCode(nDistance), nDistance
                            , FIntval_GetIntvCode(nFullVoiceInterval), nVoiceInterval[i]
                            );

                        wxLogMessage( sResultDetails );


                        HDisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
                          , &m_pChordDescriptor[nC], colour, sResultDetails, false);


                         // display failing notes in red   TODO: mejorar?
  //                       if ( m_pChordDescriptor[nC].pChordNotes[nN]->GetComponentColour() == *wxGREEN )
                             m_pChordDescriptor[nC].pChordNotes[nN]->SetColour(*wxCYAN);
  //                       if ( m_pChordDescriptor[nC].pChordNotes[i]->GetComponentColour() == *wxGREEN )
                             m_pChordDescriptor[nC].pChordNotes[i]->SetColour(*wxBLUE);
  //                       if ( m_pChordDescriptor[nC-1].pChordNotes[nN]->GetComponentColour() == *wxGREEN )
                             m_pChordDescriptor[nC-1].pChordNotes[nN]->SetColour(*wxCYAN);
  //                       if ( m_pChordDescriptor[nC-1].pChordNotes[i]->GetComponentColour() == *wxGREEN )
                             m_pChordDescriptor[nC-1].pChordNotes[i]->SetColour(*wxBLUE);



                         m_pChordDescriptor[nC].tChordErrors.SetError( this->GetRuleId(), true);
                         nErrCount++;
                    }
                }
//todo:remove   else wxLogMessage(_T("    not Parallel chord %d, n:%d n:%d, intv1:%d != intv2:%d")
//		               ,nC, i,  nN, nVoiceInterval[i] , nVoiceInterval[nN]);

            }
        }
    }

    return nErrCount;
}

int lmRuleNoResultingFifthOctaves::Evaluate(wxString& sResultDetails, int pNumFailuresInChord[])
{
    // Forbidden to arrive to a fifth or octave by means of a directo movement ( both same delta sign)
    // exceptions:
    //  - voice is soprano (TODO: tenor, contralto??) and distance is 2th
    //  - TODO: fifth and one sound existed??
    sResultDetails = _T("lmRuleNoResultingFifthOctaves::Evaluate");
    if ( m_pChordDescriptor == NULL)
    {
        wxLogMessage(_T(" lmRuleNoResultingFifthOctaves Rule %d  m_pChordDescriptor NULL "));
        return false;
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
 , NoteId(*m_pChordDescriptor[nC-1].pChordNotes[nN]), NoteId(*m_pChordDescriptor[nC].pChordNotes[nN])
 , NoteId(*m_pChordDescriptor[nC-1].pChordNotes[i]), NoteId(*m_pChordDescriptor[nC].pChordNotes[i])
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
               , (nN)+1, NoteId(*m_pChordDescriptor[nC-1].pChordNotes[nN]), NoteId(*m_pChordDescriptor[nC].pChordNotes[nN])
               , (i)+1, NoteId(*m_pChordDescriptor[nC-1].pChordNotes[i]), NoteId(*m_pChordDescriptor[nC].pChordNotes[i])
               , FIntval_GetIntvCode(nDistance), nDistance
               , FIntval_GetIntvCode(nInterval), nInterval);

                        wxLogMessage( sResultDetails );

                        HDisplayChordInfo(GetMainFrame()->GetActiveDoc()->GetScore()
                          , &m_pChordDescriptor[nC], colour, sResultDetails, false);

                         // display failing notes in red  (TODO: mejorar indicacion de errores)
   //                      if ( m_pChordDescriptor[nC].pChordNotes[nN]->GetComponentColour() == *wxGREEN )
                            m_pChordDescriptor[nC].pChordNotes[nN]->SetColour(*wxRED);
   //                      if ( m_pChordDescriptor[nC].pChordNotes[i]->GetComponentColour() == *wxGREEN )
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
