//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "ChordManager.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ChordManager.h"
#include "Conversion.h"
#include "../ldp_parser/AuxString.h"
#include "../exercises/Generators.h"
#include "../score/KeySignature.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


static wxString m_sChordName[ect_Max];
static bool m_fStringsInitialized = false;

// AWARE: Array indexes are in correspondence with enum lmEChordType
// - intervals are from root note
//      number + type:   m=minor, M=major, p=perfect, a=augmented, d=diminished
#define lmNIL   lmNULL_FIntval
//TODO: @ CONSIDERAR AMPLIAR CON INVERSIONES....
//TODO: @@@ MEJORAR CREACION DE TABLA Y DE lmChordInfo: LOS INTERVALOS DEL 4 AL 6 NO SE INCIALIZAN
static lmChordInfo tData[ect_Max] = {
    { 3, 0,{ lm_M3, lm_p5, lmNIL }},      //MT        - MajorTriad
    { 3, 0,{ lm_m3, lm_p5, lmNIL }},      //mT        - MinorTriad
    { 3, 0,{ lm_M3, lm_a5, lmNIL }},      //aT        - AugTriad
    { 3, 0,{ lm_m3, lm_d5, lmNIL }},      //dT        - DimTriad
    { 3, 0,{ lm_p4, lm_p5, lmNIL }},      //I,IV,V    - Suspended_4th
    { 3, 0,{ lm_M2, lm_p5, lmNIL }},      //I,II,V    - Suspended_2nd
    { 4, 0,{ lm_M3, lm_p5, lm_M7 }},      //MT + M7   - MajorSeventh
    { 4, 0,{ lm_M3, lm_p5, lm_m7 }},      //MT + m7   - DominantSeventh
    { 4, 0,{ lm_m3, lm_p5, lm_m7 }},      //mT + m7   - MinorSeventh
    { 4, 0,{ lm_m3, lm_d5, lm_d7 }},      //dT + d7   - DimSeventh
    { 4, 0,{ lm_m3, lm_d5, lm_m7 }},      //dT + m7   - HalfDimSeventh
    { 4, 0,{ lm_M3, lm_a5, lm_M7 }},      //aT + M7   - AugMajorSeventh
    { 4, 0,{ lm_M3, lm_a5, lm_m7 }},      //aT + m7   - AugSeventh
    { 4, 0,{ lm_m3, lm_p5, lm_M7 }},      //mT + M7   - MinorMajorSeventh
    { 4, 0,{ lm_M3, lm_p5, lm_M6 }},      //MT + M6   - MajorSixth
    { 4, 0,{ lm_m3, lm_p5, lm_M6 }},      //mT + M6   - MinorSixth
    { 3, 0,{ lm_M3, lm_a4, lm_a6 }},      //          - AugSixth
};

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
    for (int nCount = 1; nCount < nNumNotes; nCount++)
    {
        wxASSERT(pInpChordNotes[nCount] != NULL);
        fpIntv = (lmFIntval) (pInpChordNotes[nCount]->GetFPitch() - pInpChordNotes[0]->GetFPitch());

        if (fpIntv > lm_p8)
        {
              fpIntv = fpIntv % lm_p8;  // todo: @ confirmar reducción de intervalos
        }
#ifdef __WXDEBUG__
        wxLogMessage(_T("[GetIntervalsFromNotes nota %d: %d  nota 0: %d] INTERVAL: %d")
            , nCount, pInpChordNotes[nCount]->GetFPitch(), pInpChordNotes[0]->GetFPitch(), fpIntv);
#endif
        // Update chord interval information
        tOutChordInfo->nIntervals[nCount-1] = fpIntv;
    }
    // For the rest of intervals...
    for (int i=nNumNotes; i<lmINTERVALS_IN_CHORD; i++)
    {
      tOutChordInfo->nIntervals[i] = lmNULL_FIntval;
    }

    tOutChordInfo->nNumNotes = nNumNotes;

}

// Search the chord type that matches the specified intervals
lmEChordType GetChordTypeFromIntervals( lmChordInfo tChordInfo )
{
    for (int i = 0; i < ect_Max; i++)
    {
       if (    tChordInfo.nNumNotes == tData[i].nNumNotes
           && tChordInfo.nIntervals[0] == tData[i].nIntervals[0]
           && tChordInfo.nIntervals[1] == tData[i].nIntervals[1]
           && tChordInfo.nIntervals[2] == tData[i].nIntervals[2]
           )
        return (lmEChordType) i;
    }
    return lmINVALID_CHORD_TYPE; // invalid chord
}

// Perform one inversion over a chord with any number of intervals
// Inversion: the lower note is increased one octave.  
void AddOneInversionToChord( lmChordInfo* pInOutChordInfo)
{
    // The algoritm is simple; the inversion is performed by updating the intervals as follows:
    //  INCREMENT inversion:
    //   NewInterval[i] = OldInterval[i+1] - OldInterval[0]; // when i < N
    //   NewInterval[N] = lm_p8 - OldInterval[0]; // when i = N

    lmFIntval fpIntv = 0;

    // Create a copy of the input
    lmChordInfo tInChordInfo;
    tInChordInfo =  *pInOutChordInfo;

    assert(pInOutChordInfo != NULL);
    assert(tInChordInfo.nNumNotes <= (lmFIntval)lmINTERVALS_IN_CHORD+1);

#ifdef __WXDEBUG__
        wxLogMessage(_T(" Before inversion: NumNotes %d, i0:%d i1:%d i2:%d")
          ,tInChordInfo.nNumNotes, tInChordInfo.nIntervals[0], tInChordInfo.nIntervals[1]
          ,tInChordInfo.nIntervals[2]);
#endif

    // number of intervals remains unchanged
    pInOutChordInfo->nNumNotes = tInChordInfo.nNumNotes; 

    // aware: 
    //  nNumNotes: number of notes
    //  nNumNotes-1: number of intervals
    //  nNumNotes-2: index of last inteval (N)
    //  nNumNotes-3: index of penultimate interval

    int i=0;
    for ( i=0; i<tInChordInfo.nNumNotes-2; i++) 
    {
        pInOutChordInfo->nIntervals[i] = tInChordInfo.nIntervals[i+1] - tInChordInfo.nIntervals[0];
    }
    pInOutChordInfo->nIntervals[tInChordInfo.nNumNotes-2] = lm_p8 - tInChordInfo.nIntervals[0];

 //TODO @@@quitar de aqui   pInOutChordInfo->nNumInversions++;

#ifdef __WXDEBUG__
        wxLogMessage(_T(" After inversion: NumNotes %d, i0:%d i1:%d i2:%d")
         ,  pInOutChordInfo->nNumNotes, pInOutChordInfo->nIntervals[0], pInOutChordInfo->nIntervals[1]
         ,  pInOutChordInfo->nIntervals[2]  );
#endif
}

// TODO: @@@ unificar con AddOneInversionToChord (poner un parámetro que indique suma o resta)
void RemoveOneInversionFromChord( lmChordInfo* pInOutChordInfo)
{
    // The algoritm is simple; the inversion is performed by updating the intervals as follows:
    //  INCREMENT inversion:
    //   NewInterval[i] = OldInterval[i+1] - OldInterval[0]; // when i < N
    //   NewInterval[N] = lm_p8 - OldInterval[0]; // when i = N
    //  DECREMENT inversion:
    //   NewInterval[0] = lm_p8 - OldInterval[N]; // when i = 0
    //   NewInterval[i] = OldInterval[i-1] + NewInterval[0]; // when i > 0

    lmFIntval fpIntv = 0;

    // Create a copy of the input
    lmChordInfo tInChordInfo;
    tInChordInfo =  *pInOutChordInfo;

    assert(pInOutChordInfo != NULL);
    assert(tInChordInfo.nNumNotes <= (lmFIntval)lmINTERVALS_IN_CHORD+1);

#ifdef __WXDEBUG__
    wxLogMessage(_T("RemoveOneInversionFromChord: Before inversion: NumNotes %d, i0:%d i1:%d i2:%d")
          ,tInChordInfo.nNumNotes, tInChordInfo.nIntervals[0], tInChordInfo.nIntervals[1]
          ,tInChordInfo.nIntervals[2]);
#endif

    // number of intervals remains unchanged
    pInOutChordInfo->nNumNotes = tInChordInfo.nNumNotes; 

    // aware: 
    //  nNumNotes: number of notes
    //  nNumNotes-1: number of intervals
    //  nNumNotes-2: index of last inteval (N)
    //  nNumNotes-3: index of penultimate interval

    int i=0;
    pInOutChordInfo->nIntervals[0] = lm_p8 - tInChordInfo.nIntervals[tInChordInfo.nNumNotes-2];
    for ( i=1; i<tInChordInfo.nNumNotes-1; i++) 
    {
        pInOutChordInfo->nIntervals[i] = tInChordInfo.nIntervals[i-1] + pInOutChordInfo->nIntervals[0];
    }

 //TODO @@@quitar de aqui   pInOutChordInfo->nNumInversions++;

#ifdef __WXDEBUG__
        wxLogMessage(_T(" After inversion: NumNotes %d, i0:%d i1:%d i2:%d")
         ,  pInOutChordInfo->nNumNotes, pInOutChordInfo->nIntervals[0], pInOutChordInfo->nIntervals[1]
         ,  pInOutChordInfo->nIntervals[2]  );
#endif
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
    int nNumPossibleInversions = tChordInfo.nNumNotes - 1;
    tChordInfo.nNumInversions = 0;
    do
    {
        nType = GetChordTypeFromIntervals( tChordInfo );

        wxLogMessage(_T(" @GTI T:%d num inv %d, max: %d, i0:%d i1:%d i2:%d")
         , nType, tChordInfo.nNumInversions, nNumPossibleInversions
         , tChordInfo.nIntervals[0], tChordInfo.nIntervals[1], tChordInfo.nIntervals[2] );

        if (nType != lmINVALID_CHORD_TYPE)
             return nType;

        if (tChordInfo.nNumInversions >= nNumPossibleInversions)
            return lmINVALID_CHORD_TYPE; // invalid chord
        else
        {
            RemoveOneInversionFromChord(&tChordInfo);
            tChordInfo.nNumInversions++;
        }

    }while ( nType == lmINVALID_CHORD_TYPE );

    return nType;
}



wxString lmChordManager::ToString()
{
    wxString sRetStr;
    if ( ! this->IsCreated() )
        sRetStr = _T(" NOT CREATED");
    else
    {
        //TODO: @@@In LDP ???
        int nNumNotes = GetNumNotes();
        sRetStr = wxString::Format(_T("[Chord: %s, %d notes, %d invers, pattern: ")  
            , GetNameFull().c_str()
            , nNumNotes
            , m_nInversion);
        for (int n=0; n<nNumNotes; n++)
        {
            sRetStr += _T(" "); 
            sRetStr += GetPattern(n); 
        }
        sRetStr += _T(" ]");
    }
    return sRetStr;
}
void lmChordManager::Create(lmNote* pRootNote, lmChordInfo* pChordInfo)
{
   lmKeySignature* pKey = pRootNote->GetApplicableKeySignature();
   lmEKeySignatures nKey = (pKey ? pKey->GetKeyType() : earmDo);

    assert(pChordInfo != NULL);

    m_nKey = nKey;
    m_nNumNotes = pChordInfo->nNumNotes;
    m_nInversion = pChordInfo->nNumInversions;
    m_fpNote[0] = pRootNote->GetFPitch();

    DoCreateChord(pChordInfo->nIntervals);

}

// Look for notes in the score that make up a valid chord
bool TryChordCreation(int nNumNotes, lmNote** pInpChordNotes, lmChordInfo* tOutChordInfo, wxString &sOutStatusStr)
{
    bool fOk = false;

    sOutStatusStr =  wxString::Format(_T("[Try: %d notes ") ,  nNumNotes);
    wxASSERT(pInpChordNotes != NULL);

    tOutChordInfo->Initalize();

    for (int i=0; i<nNumNotes; i++)
    {
        wxASSERT(pInpChordNotes[i] != NULL);
        sOutStatusStr +=  wxString::Format(_T("{%s, %d} ") 
                ,pInpChordNotes[i]->SourceLDP(0) ,  pInpChordNotes[i]->GetFPitch());
    }

    if (nNumNotes < 3)
    {
        sOutStatusStr +=  _T(" minimum 3 notes are required ! ]");
        return false;
    }

    // Sort notes
    SortChordNotes(nNumNotes, pInpChordNotes);

    for (int i=0; i<nNumNotes; i++)
    {
        sOutStatusStr +=  wxString::Format(_T("{%d} ") 
                , pInpChordNotes[i]->GetFPitch());
    }

    // Get intervals and Create lmChordInfo from notes
    GetIntervalsFromNotes(nNumNotes, pInpChordNotes, tOutChordInfo);

    for (int i=0; i<nNumNotes-1; i++)
    {
        sOutStatusStr +=  wxString::Format(_T("<I:%d> ") 
            , tOutChordInfo->nIntervals[i]);
    }

// TODO: quitar    lmEChordType lmType = GetChordTypeFromIntervals( *tOutChordInfo );
    lmEChordType nType = GetChordTypeAndInversionsFromIntervals(*tOutChordInfo);

    if (nType == lmINVALID_CHORD_TYPE )
    {
        wxLogMessage(_T(" @Invalid chord: Num notes %d, i0:%d i1:%d i2:%d")
         ,  tOutChordInfo->nNumNotes, tOutChordInfo->nIntervals[0], tOutChordInfo->nIntervals[1]
        , tOutChordInfo->nIntervals[2] );
        wxLogMessage(_T(" @@tData[0]: Num notes %d, i0:%d i1:%d i2:%d")
            ,  tData[0].nNumNotes, tData[0].nIntervals[0], tData[0].nIntervals[1]
             , tData[0].nIntervals[2]  );
        fOk = false;
    }
    else
    {
        fOk = true;
    }
    sOutStatusStr +=  _T("]");
    return fOk;
}

lmChordManager::lmChordManager(lmNote* pRootNote, lmChordInfo &tChordInfo)
{
    this->Initialize(); // call basic constructor for initialization

    m_nType = GetChordTypeFromIntervals( tChordInfo );
    if ( m_nType == lmINVALID_CHORD_TYPE )
    {
        //TODO: @@@manage invalid chord...
        wxLogMessage(_T(" lmChordManager Invalid chord: Num notes %d, i0:%d i1:%d i2:%d")
             ,  tChordInfo.nNumNotes, tChordInfo.nIntervals[0], tChordInfo.nIntervals[1]
             , tChordInfo.nIntervals[2]  );
        wxLogMessage(_T(" @@@ tData[0]: Num notes %d, i0:%d i1:%d i2:%d")
            ,  tData[0].nNumNotes, tData[0].nIntervals[0], tData[0].nIntervals[1]
             , tData[0].nIntervals[2]  );
    }
//todo: @remove??    else
    Create(pRootNote, &tChordInfo);


#ifdef __WXDEBUG__
    wxLogMessage(_T(" CREATED chord: %s"), this->ToString() );
#endif
}
void lmChordManager::Initialize()
{
    m_nNumNotes = 0;
    m_nInversion = 0;
    m_nType =  lmINVALID_CHORD_TYPE;
    m_nKey = earmDo; // TODO: @@@initalize with invalid value
}



//-------------------------------------------------------------------------------------
// Implementation of lmChordManager class

lmChordManager::lmChordManager()
{
    this->Initialize();
}

lmChordManager::lmChordManager(wxString sRootNote, lmEChordType nChordType,
                               int nInversion, lmEKeySignatures nKey)
{
    Create(sRootNote, nChordType, nInversion, nKey);
}

void lmChordManager::Create(wxString sRootNote, lmEChordType nChordType,
                            int nInversion, lmEKeySignatures nKey)
{
    //parameter 'nInversion' is encoded as follows:
    //  0 - root position
    //  1 - 1st inversion
    //  2 - 2nd inversion
    //  and so on

    //save parameters
    m_nType = nChordType;
    m_nKey = nKey;
    m_nInversion = nInversion;

    // Create root note data
    m_fpNote[0] = FPitch(sRootNote);

    //get the intervals that form the chord
    lmFIntval nIntval[3], nNewIntv[3];
    nIntval[0] = (lmFIntval)tData[m_nType].nIntervals[0];
    nIntval[1] = (lmFIntval)tData[m_nType].nIntervals[1];
    nIntval[2] = (lmFIntval)tData[m_nType].nIntervals[2];

    //correction for inversions
    if (m_nInversion == 1)
    {
        nNewIntv[0] = nIntval[1] - nIntval[0];

        if (nIntval[2] == lmNIL) {
            nNewIntv[1] = lm_p8 - nIntval[0];   //invert the interval
            nNewIntv[2] = lmNIL;
        }
        else {
            nNewIntv[1] = nIntval[2] - nIntval[0];
            nNewIntv[2] = lm_p8 - nIntval[0];   //invert the interval
        }
    }
    else if (m_nInversion == 2)
    {
        if (nIntval[2] == lmNIL) {
            nNewIntv[0] = lm_p8 - nIntval[1];   //invert the interval
            nNewIntv[1] = lm_p8 + nIntval[0] - nIntval[1];
            nNewIntv[2] = lmNIL;
        }
        else {
            nNewIntv[0] = nIntval[2] - nIntval[1];
            nNewIntv[1] = lm_p8 - nIntval[1];   //invert the interval
            nNewIntv[2] = lm_p8 + nIntval[0] - nIntval[1];
        }
    }
    else if (m_nInversion == 3)
    {
        nNewIntv[0] = lm_p8 - nIntval[2];   //invert the interval
        nNewIntv[1] = nNewIntv[0] + nIntval[0];
        nNewIntv[2] = nNewIntv[0] + nIntval[1];
    }
    if (m_nInversion != 0) {
        nIntval[0] = nNewIntv[0];
        nIntval[1] = nNewIntv[1];
        nIntval[2] = nNewIntv[2];
    }

    m_nNumNotes = tData[m_nType].nNumNotes;
    DoCreateChord(nIntval);

}

/*@@@ TODO: SI NO SE USA, QUITAR
void lmChordManager::Create(lmFPitch fpRootNote, int nNumNotes, lmFIntval nIntervals[], lmEKeySignatures nKey)
{
    // save data
    m_nKey = nKey;
    m_nNumNotes = nNumNotes;
    m_fpNote[0] = fpRootNote;

    DoCreateChord(nIntervals);
} */


void lmChordManager::Create(wxString sRootNote, wxString sIntervals, lmEKeySignatures nKey)
{
    // save data
    m_nKey = nKey;

    // prepare root note
    m_fpNote[0] = FPitch(sRootNote);

    // extract intervals
    lmFIntval nIntval[lmNOTES_IN_CHORD];
    int iT = 0;                 //index to interval

    int nSize = (int)sIntervals.length();
    int iStart = 0;
    int iEnd = sIntervals.find(_T(','), iStart);
    while (iEnd != (int)wxStringBase::npos && iT < lmNOTES_IN_CHORD-1)
    {
        wxString sIntval = sIntervals.substr(iStart, iEnd-iStart);
        // If first interval is "#" or "b" it refers to the root note
        if (iStart==0 && (sIntval == _T("#") || sIntval == _T("b")))
        {
            // modify root pitch
            if (sIntval == _T("#"))
                m_fpNote[0]++;
            else
                m_fpNote[0]--;
        }
        else
        {
            // convert interval name to value
            nIntval[iT++] = FIntval(sIntval);
        }

        // advance pointers
        iStart = iEnd + 1;
        if (iStart >= nSize) break;
        iEnd = sIntervals.find(_T(','), iStart);

    }
    if (iT < lmNOTES_IN_CHORD-1) {
        wxString sIntval = sIntervals.substr(iStart);
        nIntval[iT++] = FIntval(sIntval);
    }

    m_nNumNotes = iT+1;     //num notes = nim.intervals + 1

    DoCreateChord(nIntval);

}

void lmChordManager::DoCreateChord(lmFIntval nIntval[])
{
    // root note is created in m_fpNote[0]. Create the remaining notes
    for (int i=1; i < m_nNumNotes; i++) {
        m_fpNote[i] = m_fpNote[0] + nIntval[i-1];
    }

}

lmChordManager::~lmChordManager()
{
}

int lmChordManager::GetNumNotes()
{
    return m_nNumNotes;
}

lmMPitch lmChordManager::GetMidiNote(int i)
{
    wxASSERT(i < GetNumNotes());
    return FPitch_ToMPitch(m_fpNote[i]);
}

wxString lmChordManager::GetPattern(int i)
{
    // Returns Relative LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < GetNumNotes());
    return FPitch_ToRelLDPName(m_fpNote[i], m_nKey);
}

wxString lmChordManager::GetNameFull()
{
    wxString sName = ChordTypeToName( m_nType );
    sName += _T(", ");
    if (m_nInversion == 0)
        sName += _("root position");
    else if (m_nInversion == 1)
        sName += _("1st inversion");
    else if (m_nInversion == 2)
        sName += _("2nd inversion");
    else if (m_nInversion == 3)
        sName += _("3rd inversion");
    else
        wxASSERT(false);    //impossible

    return sName;

}


#ifdef __WXDEBUG__
void lmChordManager::UnitTests()
{
    int i, j;

    //lmConverter::NoteToBits and lmConverter::NoteBitsToName
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of lmConverter::NoteToBits() method:"));
    wxString sNote[8] = { _T("a4"), _T("+a5"), _T("--b2"), _T("-a4"),
        _T("+e4"), _T("++f6"), _T("b1"), _T("xc4") };
    lmNoteBits tNote;
    for(i=0; i < 8; i++) {
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
    for(i=0; i < 8; i++) {
        for (j=0; j < 8; j++) {
            wxString sNewNote = ComputeInterval(sNote[i], sIntv[j], true, m_nKey);
            wxLogMessage(_T("Note='%s' + Intv='%s' --> '%s'"),
                         sNote[i].c_str(), sIntv[j].c_str(), sNewNote.c_str() );
            wxString sStartNote = ComputeInterval(sNewNote, sIntv[j], false, m_nKey);
            wxLogMessage(_T("Note='%s' - Intv='%s' --> '%s'"),
                         sNewNote.c_str(), sIntv[j].c_str(), sStartNote.c_str() );
        }
    }

    //IntervalCodeToBits and IntervalBitsToCode
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of IntervalCodeToBits() method:"));
    lmIntvBits tIntv;
    for(i=0; i < 8; i++) {
        if (IntervalCodeToBits(sIntv[i], &tIntv))
            wxLogMessage(_T("Unexpected error in IntervalCodeToBits()"));
        else {
            wxLogMessage(_T("Intv: '%s'. Bits: num=%d, Semitones=%d --> '%s'"),
                sIntv[i].c_str(), tIntv.nNum,tIntv.nSemitones,
                IntervalBitsToCode(tIntv).c_str() );
        }
    }

    ////SubstractIntervals
    //wxLogMessage(_T("[lmChordManager::UnitTests] Test of SubstractIntervals() method:"));
    //wxString sIntv1[8] = { _T("p5"), _T("p5"), _T("M7"), _T("M6"), _T("m6"), _T("M7"), _T("M6"), _T("p4") };
    //wxString sIntv2[8] = { _T("M3"), _T("m3"), _T("p5"), _T("p5"), _T("a5"), _T("M3"), _T("m3"), _T("M2") };
    //for(i=0; i < 8; i++) {
    //    wxLogMessage(_T("Intv1='%s', intv2='%s' --> dif='%s'"),
    //        sIntv1[i], sIntv2[i], SubstractIntervals(sIntv1[i], sIntv2[i]) );
    //}

    ////AddIntervals
    //wxLogMessage(_T("[lmChordManager::UnitTests] Test of AddIntervals() method:"));
    //wxString sIntv1[8] = { _T("p5"), _T("p5"), _T("M6"), _T("M3"), _T("M3"), _T("M6"), _T("d4"), _T("p8") };
    //wxString sIntv2[8] = { _T("M3"), _T("m3"), _T("m2"), _T("m3"), _T("M3"), _T("M3"), _T("m7"), _T("p8") };
    //for(i=0; i < 8; i++) {
    //    wxLogMessage(_T("Intv1='%s', intv2='%s' --> sum='%s'"),
    //        sIntv1[i].c_str(), sIntv2[i].c_str(), AddIntervals(sIntv1[i], sIntv2[i]).c_str() );
    //}

}
#endif  // __WXDEBUG__

//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

wxString ChordTypeToName(lmEChordType nType)
{
    wxASSERT(nType < ect_Max);

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized) {
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

        m_fStringsInitialized = true;
    }

    return m_sChordName[nType];

}

int NumNotesInChord(lmEChordType nChordType)
{
    wxASSERT(nChordType < ect_Max);
    return tData[nChordType].nNumNotes;

}

lmEChordType ChordShortNameToType(wxString sName)
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
