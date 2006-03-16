// RCS-ID: $Id: ScoreConstrains.cpp,v 1.12 2006/02/28 17:40:21 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
//    for (any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file ScoreConstrains.cpp
    @brief Implementation file for lmScoreConstrains class
    @ingroup generators
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EarIntvalConstrains.h"
#endif

// for (compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ScoreConstrains.h"
#include "../ldp_parser/AuxString.h"
#include "../ldp_parser/LDPParser.h"
#include "Generators.h"
#include "../score/Score.h"

//debug
//#include "../app/MainFrame.h"
//extern lmMainFrame *GetMainFrame();

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


// access to the config object
extern wxConfigBase *g_pPrefs;


lmScoreConstrains::lmScoreConstrains()
{
    /*  Score Constrains objec are only created in html ScoreMusicReading exercises.
        The exercise must provide all necessary values.
        When a 'section key' is provided - by invoking method SetSection() - this object
        valuaes are loaded from cofiguration file (or with default values if the key
        does not exists).
        If no 'section key' is provided, the exercise must be configure with the
        settings provided in the htm params.
        So, no initialization ob this object is needed (as long as we verify that
        all necessary html params are included)
    */

    //initializations
    m_sSection = _T("");

    //default values for html params
    m_nMaxInterval = 4;

}

void lmScoreConstrains::SaveSettings()
{
    /*
    save settings in user configuration data file
    */

    if (m_sSection == _T("")) return;

    // allowed clefs and notes range
    int i;
    wxString sKey;
    for (i = lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/Clef%d"), m_sSection, i); 
        g_pPrefs->Write(sKey, m_oClefs.IsValid( (EClefType)i ));
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/MinNote%d"), m_sSection, i);
        g_pPrefs->Write(sKey, m_oClefs.GetLowerPitch( (EClefType)i ));
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/MaxNote%d"), m_sSection, i);
        g_pPrefs->Write(sKey, m_oClefs.GetUpperPitch( (EClefType)i ));
    }

    // max interval in two consecutive notes
    sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/MaxInterval"), m_sSection);
    g_pPrefs->Write(sKey, (long)m_nMaxInterval);

    // flag to signal that this key has data
    sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/HasData"), m_sSection);
    g_pPrefs->Write(sKey, true);

    // allowed time signatures
    for (i = lmMIN_TIME_SIGN; i <= lmMAX_TIME_SIGN; i++) {
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/Time%d"), m_sSection, i); 
        g_pPrefs->Write(sKey, m_oValidTimeSign.IsValid( (ETimeSignature)i ));
    }

    // allowed key signatures
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/KeySignature%d"), m_sSection, i); 
        g_pPrefs->Write(sKey, m_oValidKeys.IsValid((EKeySignatures)i) );
    }

    //! @todo save remaining data: fragments

}

void lmScoreConstrains::LoadSettings()
{
    /*
    load settings form user configuration data. Default values are taken from html params. 
    
    When arriving here, default values are already stored in this object. So we must check
    if the section key is stored in the configuration file. If it is, then we have
    to load config data from there; otherwise, do nothing as default values are in place.

    */

    if (m_sSection == _T("")) {
        // leave default values
        return;
    }

    //check if the section key is stored in the configuration file.
    wxString sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/HasData"), m_sSection);
    bool fHasData = false;
    g_pPrefs->Read(sKey, &fHasData, false);

    //If no data saved, return. Default values are already in place.
    if (!fHasData) return;

    //
    //  Load confiuration data from config file. Default values are meaningless
    //  as they never should be used
    //

    // allowed clefs and notes range
    // default values: only G cleft allowed
    int i;
    bool fValue;
    for (i = lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/Clef%d"), m_sSection, i);
        g_pPrefs->Read(sKey, &fValue, ((EClefType)i == eclvSol));
        m_oClefs.SetValid((EClefType)i, fValue);
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/MinNote%d"), m_sSection, i);
        m_oClefs.SetLowerPitch((EClefType)i, g_pPrefs->Read(sKey, _T("a3")));
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/MaxNote%d"), m_sSection, i);
        m_oClefs.SetUpperPitch((EClefType)i, g_pPrefs->Read(sKey, _T("a5")));
    }

    // max interval in two consecutive notes
    sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/MaxInterval"), m_sSection);
    m_nMaxInterval = (int)g_pPrefs->Read(sKey, 4L);     //default value 4

    // allowed time signatures
    for (i = lmMIN_TIME_SIGN; i <= lmMAX_TIME_SIGN; i++) {
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/Time%d"), m_sSection, i); 
        g_pPrefs->Read(sKey, &fValue, true);
        m_oValidTimeSign.SetValid((ETimeSignature)i, fValue);
    }

    // allowed key signatures
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/ScoreConstrains/%s/KeySignature%d"), 
            m_sSection, i );
        g_pPrefs->Read(sKey, &fValue, (bool)((EKeySignatures)i == earmDo) );
        m_oValidKeys.SetValid((EKeySignatures)i, fValue);
    }


    //! @todo load remaining data: fragments

}

wxString lmScoreConstrains::Verify()
{
    wxString sError = _T("");
    int i;
    bool fAtLeastOne;

    // ensure that at least a Clef is selected
    fAtLeastOne = false;
    for (i=lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        fAtLeastOne = fAtLeastOne || m_oClefs.IsValid((EClefType)i);
        if (fAtLeastOne) break;
    }
    if (!fAtLeastOne) {
        sError += _("Global error: No clef constrains specified\n");
    }

    // ensure that at least a time signature is selected
    fAtLeastOne = false;
    for (i = lmMIN_TIME_SIGN; i <= lmMAX_TIME_SIGN; i++) {
        fAtLeastOne = fAtLeastOne || m_oValidTimeSign.IsValid((ETimeSignature)i);
        if (fAtLeastOne) break;
    }
    if (!fAtLeastOne) {
        sError += _("Global error: No time signature constrains specified\n");
    }

    // ensure that at least a key signature is selected
    fAtLeastOne = false;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        fAtLeastOne = fAtLeastOne || m_oValidKeys.IsValid((EKeySignatures)i);
        if (fAtLeastOne) break;
    }
    if (!fAtLeastOne) {
        sError += _("Global error: No key signature constrains specified\n");
    }

    //! @todo verify remaining data: fragments

    return sError;

}

//--------------------------------------------------------------------------------------
// lmFragmentsTable implementation
//--------------------------------------------------------------------------------------

lmFragmentsTable::lmFragmentsTable()
{
}

lmFragmentsTable::~lmFragmentsTable()
{
    int i;
    for (i=0; i < (int)m_aFragment.Count(); i++) {
        delete m_aFragment[i];
    }
}

/*! Receives a fragment pattern. Analyzes it, breaking it into segments and
    computing times tab, tam and ts, and stores the fragment in the fragments table
*/
void lmFragmentsTable::AddEntry(lmTimeSignConstrains* pValidTimeSigns, wxString sPattern)
{
    // create the fragment's entry
    lmFragmentEntry* pFragment = new lmFragmentEntry(pValidTimeSigns);
    m_aFragment.Add(pFragment);

    // split the pattern into segments and add each segment to the segments table
    int iEnd;
    wxString sSegment;
    float rSegmentDuration;             // ts
    float rTimeAlignMeasure = 0.0;      // tam
    float rTimeAlignBeat = 0.0;         // tab
    bool fFirstSegment = true;          //it is the firts segment
    bool fBarMark = false;              //bar alignment mark found
    bool fBarTimeComputed = false;      //computation of 'tam' finished
    wxString sSource = sPattern;

    while (sSource != _T("") )
    {
        //extract the segment and remove it from source
        iEnd = SplitFragment(sSource);
        if (iEnd == -1) {
            //last segment
            sSegment = sSource;
            sSource = _T("");
       }
        else {
            //intermediate segment
            sSegment = sSource.Mid(0, iEnd);
            if (!fBarMark) fBarMark = (sSource.Mid(iEnd, 1) == _T("|"));
            sSource = sSource.Mid(iEnd+1);
        }

        //compute segment duration and time to align to beat (tab)
        if (fFirstSegment) {
            sSegment = GetFirstSegmentDuracion(sSegment, &rSegmentDuration, &rTimeAlignBeat);
            fFirstSegment = false;
        }
        else {
            rSegmentDuration = GetPatternDuracion(sSegment, pValidTimeSigns);
            rTimeAlignBeat = 0.0;
        }

        //add up time to bar alignment
        if (!fBarTimeComputed) {
            rTimeAlignMeasure += rSegmentDuration;
            fBarTimeComputed = fBarMark;
        }

        // add segment to table
        lmSegmentEntry* pSegment = new lmSegmentEntry(sSegment, rTimeAlignBeat,
                                                    rSegmentDuration);
        pFragment->AddSegment(pSegment);

    }

    //update tam (time to align fragment to barline)
    if (fBarTimeComputed) {
        pFragment->SetTimeToAlignToBarline(rTimeAlignMeasure);
    }

}

/*! Receives an string formed by concatenated elements, for example:
    "(n * n)(n * s g+)(n * s)(n * c g-)"
    sSource must be normalized (lower case, no extra spaces)
    @return the index to the end (closing parenthesis) or first element
*/
int lmFragmentsTable::SplitPattern(wxString sSource)
{                    
    int i;                  //index to character being explored
    int iMax;               //sSource length
    int nAPar;              //open parenthesis counter

    iMax = sSource.Length();
    wxASSERT(iMax > 0);                         //sSource must not be empty
    wxASSERT(sSource.Mid(0, 1) == _T("(") );    //must start with parenthesis
    
    nAPar = 1;       //let//s count first parenthesis
    //look for the matching closing parenthesis
    bool fFound = false;
    for (i=1; i < iMax; i++) {
        if (sSource.Mid(i, 1) == _T("(") ) {
            nAPar++;
        } else if (sSource.Mid(i, 1) == _T(")") ) {
            nAPar--;
            if (nAPar == 0) {
                //matching parenthesis found. Exit loop
                fFound = true;
                break;
            }
        }
    }
    wxASSERT(fFound);
    return i;
        
}

/*! Select the fragments that are usable with requested time signature.
    A reference to the usable framents is stored in the fragment table
    variable m_aSelectionSet so that 
    @return The number of fragments selected
*/
int lmFragmentsTable::SelectFragments(ETimeSignature nTimeSign)
{
    m_aSelectionSet.Clear();
    int i;
    for (i=0; i < (int)m_aFragment.Count(); i++) {
        if ((m_aFragment[i]->m_pValidTimeSigns)->IsValid(nTimeSign)) {
            m_aSelectionSet.Add(i);
            //wxLogMessage(wxString::Format(_T("[lmFragmentsTable::SelectRows] entry=%d, nTimeSign =%d"),
            //        i, nTimeSign));
        }
    }
    m_nNextSegment = 0;
    return m_aSelectionSet.Count();

}

/*! Select one fragment, at random, from the set of selected fragments.
    Also, all variables related to retrieving the segments composing the 
    selected fragment are initialized.
*/
void lmFragmentsTable::ChooseRandom()
{
    lmRandomGenerator oGenerator;
    int i = oGenerator.RandomNumber(0, m_aSelectionSet.Count()-1);
    m_nSelItem = m_aSelectionSet.Item(i);
    lmFragmentEntry* pFragmentEntry = m_aFragment.Item( m_nSelItem );
    m_pSegments = pFragmentEntry->GetSegments();
    m_nNextSegment = 0;     //point to first one
}

//! Returns the next segment entry for the chosen fragment or NULL if no more available
lmSegmentEntry* lmFragmentsTable::GetNextSegment()
{
    if (m_nNextSegment < (int)m_pSegments->Count()) {
        return m_pSegments->Item(m_nNextSegment++);
    }
    else {
        return (lmSegmentEntry*) NULL;
    }

}

//wxString lmFragmentsTable::Old_GetFirstSegmentDuracion(wxString sSegment,
//                                float* pSegmentDuration, float* pTimeAlignBeat)
//{
//    /*
//        - Removes any rests at the beginig of the segment and returns the
//            remaining elements.
//        - Computes the duration of the removed rests and updates with this
//            value variable pointed by pTimeAlignBeat
//        - Computes the duration of the remaining elements and updates with this
//            value variable pointed by pSegmentDuration
//    */
//    int iEnd;
//    wxString sElement;
//    float rSegmentDuration = 0.0;
//    float rTimeToBeatAlign = 0.0;
//    float rElementDuration = 0.0;
//    bool fIsInitialRest;
//    bool fProcessingInitialRests = true;
//    wxString sFirstSegment = _T("");
//
//    wxString sSource = sSegment;
//    while (sSource != _T("") )
//    {
//        //extract the element and remove it from source
//        iEnd = SrcSplitPattern(sSource) + 1;
//        sElement = sSource.Mid(0, iEnd);
//        sSource = sSource.Mid(iEnd);
//
//        //compute element's duration
//        rElementDuration = SrcGetElementDuracion(sElement);
//
//        //determine element type
//        fIsInitialRest = (fProcessingInitialRests ? SrcIsRest(sElement) : false);
//
//        // accumulate element duration
//        if (fIsInitialRest) {
//            rTimeToBeatAlign += rElementDuration;
//        }
//        else {
//            fProcessingInitialRests = false;
//            sFirstSegment += sElement;
//            rSegmentDuration += rElementDuration;
//        }
//    }
//
//    //return results
//    *pSegmentDuration = rSegmentDuration;
//    *pTimeAlignBeat = rTimeToBeatAlign;
//    return sFirstSegment;
//
//}

int lmFragmentsTable::SplitFragment(wxString sSource)
{                    
    int i;                  //index to character being explored
    int iMax;               //sSource length

    iMax = sSource.Length();
    wxASSERT(iMax > 0);                         //sSource must not be empty
    wxASSERT(sSource.Mid(0, 1) == _T("(") );    //must start with parenthesis
    
    //look for a comma (,) or a barline (|)
    for (i=1; i < iMax; i++) {
        if (sSource.Mid(i, 1) == _T(",") || sSource.Mid(i, 1) == _T("|") ) {
            // found. Exit loop
            return i;
        }
    }
    
    return -1;      // not found
        
}

float lmFragmentsTable::GetPatternDuracion(wxString sPattern, lmTimeSignConstrains* pValidTimeSigns)
{
    /*
        return the total duration of the pattern
    */

    //prepare source with a measure and instatiate note pitches
    wxString sSource = _T("(c 1 ") + sPattern;
    sSource += _T("(Barra Final))");
    sSource.Replace(_T("*"), _T("a4"));

    // prepare and initialize the score
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmScore* pScore = new lmScore();
    pScore->AddInstrument(1,0,0);                  //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = pScore->GetVStaff(1, 1);   //get first vstaff of instr.1
    pVStaff->AddClef(eclvSol);
    pVStaff->AddKeySignature(earmDo);
    //pVStaff->AddTimeSignature( m_nTimeSign );
    pNode = parserLDP.ParseText(sSource);
    parserLDP.AnalyzeMeasure(pNode, pVStaff);

    //The score is built. Traverse it to get total duration
    lmStaffObj* pSO;
    lmStaffObjIterator* pIter = pVStaff->CreateIterator(eTR_ByTime);
    pIter->MoveLast();
    pSO = pIter->GetCurrent();
    wxASSERT(pSO->GetType() == eTPO_Barline);
    float rPatternDuration = pSO->GetTimePos();

    // iterator no longer needed. delete it
    delete pIter;

    //the score is no longer needed. Delete it
    delete pScore;

    return rPatternDuration;

}

wxString lmFragmentsTable::GetFirstSegmentDuracion(wxString sSegment,
                                float* pSegmentDuration, float* pTimeAlignBeat)
{
    /*
        - Removes any rests at the beginig of the segment and returns the
            remaining elements.
        - Computes the duration of the removed rests and updates with this
            value variable pointed by pTimeAlignBeat
        - Computes the duration of the remaining elements and updates with this
            value variable pointed by pSegmentDuration
    */
    g_pLogger->LogTrace(_T("lmFragmentsTable::GetFirstSegmentDuracion"),
            _T("[lmFragmentsTable::GetFirstSegmentDuracion] analyzing='%s'"),
            sSegment );

    //prepare source with a measure and instatiate note pitches
    wxString sSource = _T("(c 1 ") + sSegment;
    sSource += _T("(Barra Final))");
    sSource.Replace(_T("*"), _T("a4"));

    // prepare and initialize the score
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmScore* pScore = new lmScore();
    pScore->AddInstrument(1,0,0);                  //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = pScore->GetVStaff(1, 1);   //get first vstaff of instr.1
    pVStaff->AddClef(eclvSol);
    pVStaff->AddKeySignature(earmDo);
    //pVStaff->AddTimeSignature( m_nTimeSign );
    pNode = parserLDP.ParseText(sSource);
    parserLDP.AnalyzeMeasure(pNode, pVStaff);

    //The score is built. Traverse it to get total duration
    lmStaffObj* pSO;
    lmNoteRest* pNR;
    lmStaffObjIterator* pIter = pVStaff->CreateIterator(eTR_ByTime);

    // compute initial rests duration
    float rRestsDuration = 0.0;
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->GetType() == eTPO_NoteRest) {
            pNR = (lmNoteRest*) pSO;
            if (pNR->IsRest()) {
                rRestsDuration += pNR->GetDuration();        //add duration
            }
            else
                break;
        }
        pIter->MoveNext();
    }

    //now compute remaining duration
    float rSegmentDuration = 0.0;
    pIter->MoveLast();
    pSO = pIter->GetCurrent();
    wxASSERT(pSO->GetType() == eTPO_Barline);
    rSegmentDuration = pSO->GetTimePos() - rRestsDuration;

    // iterator no longer needed. delete it
    delete pIter;

    //the score is no longer needed. Delete it
    //GetMainFrame()->DumpScore(pScore);  //debug
    delete pScore;

    // Now remove any rests from the begining of the pattern
    int iEnd;

    wxString sElement;
    sSource = sSegment;
    while (sSource != _T("") ) 
    {
        //extract an element
        iEnd = SrcSplitPattern(sSource) + 1;
        sElement = sSource.Mid(0, iEnd);

        // if it is a rest remove it from pattern; otherwise finish loop
        if (SrcIsRest(sElement)) {
            sSource = sSource.Mid(iEnd);
        }
        else {
            break;
        }
    }

    //return results
    g_pLogger->LogTrace(_T("lmFragmentsTable::GetFirstSegmentDuracion"),
            _T("[lmFragmentsTable::GetFirstSegmentDuracion] sSource='%s', ts=%.2f, tab=%.2f"),
            sSource, rSegmentDuration, rRestsDuration );

    *pSegmentDuration = rSegmentDuration;
    *pTimeAlignBeat = rRestsDuration;
    return sSource;

}



