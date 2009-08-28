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
#pragma implementation "Interval.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Interval.h"
#include "Conversion.h"
#include "../exercises/Generators.h"
#include "../score/KeySignature.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;




static wxString m_sIntervalName[16];
static bool m_fStringsInitialized = false;
//forward declaration
void FIntval_InitializeStrings();

//-------------------------------------------------------------------------------------
// Implementation of lmInterval class


//Generate a random interval satisfying the received constraints.
lmInterval::lmInterval(bool fDiatonic, int ntDiatMin, int ntDiatMax, bool fAllowedIntervals[],
             bool fAscending, lmEKeySignatures nKey, int nMidiStartNote)
{
    if (!m_fStringsInitialized) FIntval_InitializeStrings();

    //dbg------------------------------------------------------
    g_pLogger->LogTrace(_T("lmInterval"), _T("Direction = %s, type = %s\n"),
                    (fAscending ? _T("Ascending") : _T("Descending")),
                    (fDiatonic ? _T("Diatonic") : _T("Chromatic") ) );
    //end dbg------------------------------------------------

    m_nKey = nKey;

    //compute max number of semitones in the allowed note range
    int ntMidiMin = DPitch_ToMPitch(ntDiatMin);
    int ntMidiMax = DPitch_ToMPitch(ntDiatMax);
    int nRange = ntMidiMax - ntMidiMin;
    wxArrayInt nValidNotes;       //to store the midi notes that can be used
    g_pLogger->LogTrace(_T("lmInterval"), _T("MidiMin=%d, MidiMax=%d\n"),
                        ntMidiMin, ntMidiMax);

    //compute allowed intervals for that range of notes
    int nAllowedIntv[25];
    wxString sDbgMsg = _T("Intv:");
    int i;
    int nNumIntv = 0;
    for (i=0; i < wxMin(nRange, 25); i++) {
        if (fAllowedIntervals[i]) {
            nAllowedIntv[nNumIntv] = i;
            nNumIntv++;
            sDbgMsg += wxString::Format(_T(" %d"), i);
        }
    }
    sDbgMsg += _T("\n");
    g_pLogger->LogTrace(_T("lmInterval"), sDbgMsg);
    if (nNumIntv == 0) {
        wxMessageBox( _T("Can not generate any interval. Notes range is lower than first allowed interval") );
        m_MPitch1 = 60;
        m_MPitch2 = 65;
        m_sPattern[0] = MIDINoteToLDPPattern(m_MPitch1, m_nKey, &m_DPitch1);
        m_sPattern[1] = MIDINoteToLDPPattern(m_MPitch2, m_nKey, &m_DPitch2);
        Analyze();
        return;
    }

    int nSelIntv;                //selected interval (num semitones)
    int iSel;                    //index on nAllowedIntv. Points to the selected interval
    int ntAux;
    bool fComply;
    int ntAux2;
    int nNumValidNotes;
    lmRandomGenerator oGen;     //random generator
    while (nNumIntv != 0)
    {
        //dbg -----------------------------------------------
        sDbgMsg = wxString::Format(_T("Loop. Allowed intervals: nNumIntv=%d : "), nNumIntv);
        for (int kk = 0; kk < nNumIntv; kk++) {
            sDbgMsg += wxString::Format(_T(" %d,"), nAllowedIntv[kk] );
        }
        g_pLogger->LogTrace(_T("lmInterval"), sDbgMsg);
        //end dbg --------------------------------------------

        //Select a random interval. This is the first thing to do in order that all intervals
        // have the same probability. Other algorithms that I have tried didn't work because
        //gave more probability to certain intervals.
        //
        iSel = oGen.RandomNumber(0, nNumIntv - 1);
        nSelIntv = nAllowedIntv[iSel];
        g_pLogger->LogTrace(_T("lmInterval"), _T("Intv iSel: isel= %d, Intv= %d\n"),
                            iSel, nSelIntv);

        //Now determine valid notes to start the choosen interval
        nNumValidNotes = 0;
        sDbgMsg = _T("Generando notas: ");
        for (i=0; i <= nRange - nSelIntv; i++) {
            if (fAscending) {
                ntAux = ntMidiMin + i;
            } else {
                ntAux = ntMidiMax - i;
            }
            sDbgMsg += wxString::Format(_T(", %d"), ntAux);
            fComply = false;
            if (fDiatonic) {
                if (MPitch_IsNaturalNote((lmMPitch)ntAux, m_nKey)) {
                    ntAux2 = (fAscending ? ntAux + nSelIntv : ntAux - nSelIntv);
                    if (MPitch_IsNaturalNote((lmMPitch)ntAux2, m_nKey)) fComply = true;
                }
            } else {
                fComply = true;    //OK. This note can be used to start the choosen interval
            }

            if (fComply) {
                nValidNotes.Add(ntAux);
                nNumValidNotes++;
                sDbgMsg += _T("+");
            }
        }
        g_pLogger->LogTrace(_T("lmInterval"), sDbgMsg);

        if (nNumValidNotes > 0) {
            //There is at least one note starting note.
            //Exit the loop and proceed to choose the ending note
            break;
        } else {
            //The choosen interval can not be generated as there are no valid notes to
            //start it. Discard this interval and choose anoher one.
            if (iSel == nNumIntv) {
                nNumIntv--;
            } else {
                for (i=iSel; i < nNumIntv; i++) {
                    nAllowedIntv[i] = nAllowedIntv[i + 1];
                }
                nNumIntv--;
            }
        }

    }

    //The loop has been exited because two possible reasons:
    //1. nNumValidNotes > 0 (There are starting notes and we must proceed to choose the ending notes)
    //2. nNumIntv==0 (The interval can not be generated as there are no valid notes to start it)
    if (nNumIntv == 0) {
        wxMessageBox(_("It is not possible to generate an interval satisfying the constraints imposed by the chosen settings."));
        m_MPitch1 = 60;
        m_MPitch2 = 65;
        m_sPattern[0] = MIDINoteToLDPPattern(m_MPitch1, m_nKey, &m_DPitch1);
        m_sPattern[1] = MIDINoteToLDPPattern(m_MPitch2, m_nKey, &m_DPitch2);
        Analyze();
        return;
    }

    //Choose at random the starting note, between the valid starting notes, and compute
    //the final one.

    //dbg -----------------------------------------------
    sDbgMsg = wxString::Format(_T("Valid notes: nNumValidNotes=%d : "), nNumValidNotes);
    for (int kk = 0; kk < nNumValidNotes; kk++) {
        sDbgMsg += wxString::Format(_T(" %d,"), nValidNotes[kk] );
    }
    g_pLogger->LogTrace(_T("lmInterval"), sDbgMsg);
    //end dbg --------------------------------------------

    if (nMidiStartNote != 0)
        m_MPitch1 = nMidiStartNote;
    else {
        i = oGen.RandomNumber(0, nNumValidNotes - 1);
        m_MPitch1 = nValidNotes.Item(i);
    }

    if (fAscending) {
        m_MPitch2 = m_MPitch1 + nSelIntv;
    } else {
        m_MPitch2 = m_MPitch1 - nSelIntv;
    }
    g_pLogger->LogTrace(_T("lmInterval"), _T("i=%d, ntMidi1=%d, ntMidi2=%d\n"),
                        i, m_MPitch1, m_MPitch2);

    //Interval successfully generated. Prepare interval information
    wxASSERT( m_MPitch1 > 11 && m_MPitch2 > 11);
    m_sPattern[0] = MIDINoteToLDPPattern(m_MPitch1, m_nKey, &m_DPitch1);
    //m_sPattern[1] = MIDINoteToLDPPattern(m_MPitch2, m_nKey, &m_DPitch2);

    // MIDINoteToLDPPattern() always returns notes with sharps. Therefore, the
    // second diatonic note might not correspond to the interval. For example,
    // if the interval is 7th Major (semitones=11) first note could be c5 and
    // second note will be c4+ instead of the right value d4-.
    // To fix this problem the second note will be generated from the first one

    // As this constructor is only used for ear training exercises, some intervals
    // are not valid and must be replaced by its enarmonic. For example:
    // d8 --> M7

    g_pLogger->LogTrace(_T("lmInterval"),
            _T("Before: ntMidi1=%d, ntMidi2=%d, m_DPitch1=%d, m_DPitch2=%d, nKey=%d, nSelIntv=%d, sPattern[0]='%s', sPattern[1]='%s', name=%s\n"),
            m_MPitch1, m_MPitch2, m_DPitch1, m_DPitch2, m_nKey, nSelIntv, m_sPattern[0].c_str(), m_sPattern[1].c_str(), m_sName.c_str());
    // This table is in correspondence with enum EIntervalName
    static wxString sValidIntervals[] = {
        _T("p1"),      // unison
        _T("m2"),
        _T("M2"),
        _T("m3"),
        _T("M3"),
        _T("p4"),
        _T("a4"),
        _T("p5"),
        _T("m6"),
        _T("M6"),
        _T("m7"),
        _T("M7"),
        _T("p8"),
        _T("m9"),
        _T("M9"),
        _T("m10"),
        _T("M10"),
        _T("p11"),
        _T("a11"),
        _T("p12"),
        _T("m13"),
        _T("M13"),
        _T("m14"),
        _T("M14"),
        _T("p15") };

    wxString sIntvCode = sValidIntervals[nSelIntv];
    m_sPattern[1] = ComputeInterval(m_sPattern[0], sIntvCode,
                         (m_MPitch2 > m_MPitch1), m_nKey);

    //Get interval number
    wxString sIntvNum = sIntvCode.substr(1);
    long nIntvNum;
    sIntvNum.ToLong(&nIntvNum);

    //compute diatonic pitch of second note
    if (m_MPitch2 > m_MPitch1)
        m_DPitch2 = m_DPitch1 + nIntvNum -1;
    else
        m_DPitch2 = m_DPitch1 - nIntvNum +1;

    g_pLogger->LogTrace(_T("lmInterval"),
            _T("After: ntMidi1=%d, ntMidi2=%d, m_DPitch1=%d, m_DPitch2=%d, nKey=%d, nSelIntv=%d, sPattern[0]='%s', sPattern[1]='%s', nIntvNum=%d\n"),
            m_MPitch1, m_MPitch2, m_DPitch1, m_DPitch2, m_nKey, nSelIntv, m_sPattern[0].c_str(), m_sPattern[1].c_str(), nIntvNum);

//    if (Abs(m_DPitch1 - m_DPitch2) <> nSelIntv) {
//        //mala suerte. Estamos en el caso de no ajustar a notas propias de la tonalidad y
//        //el intervalo generado, aunque correcto, es aumentado o disminuido, con lo que
//        //su nombre no corresponde con los de los botones de respuesta. Generar otro.
//        MsgBox "mala suerte"
//    }
    Analyze();

    g_pLogger->LogTrace(_T("lmInterval"),
        _T("m_nNumIntv=%d, name=%s\n"), m_nNumIntv, m_sName.c_str());

}

//analize the interval and fill up internal variables (i.e. name) according the analysis
void lmInterval::Analyze()
{
    m_nSemi = abs(m_MPitch1 - m_MPitch2);
    m_nNumIntv = abs(m_DPitch1 - m_DPitch2) + 1;

    //trim to reduce intervals greater than one octave
    bool fMajor = (m_nNumIntv > 8);
    if (fMajor) {
        m_nSemi -= 12;
        m_nNumIntv -= 7;
    }

    //compute the number of semitones required to be perfect or major (p.84 Atlas)
    int nPerfect = 2 * (m_nNumIntv - 1);
    if (m_nNumIntv > 3) nPerfect--;            //intervals greater than 3rd loose a semitone
    if (m_nNumIntv == 8) nPerfect = 12;        //the octave has 12 semitones

    //at this point:
    //   m_nSemi = num. of semitones in the interval
    //   m_nNumIntv = number of the interval
    //   nPerfect = num. of semitones that should have to be perfect or major

    //wxLogMessage( wxString::Format(
    //    _T("[lmInterval::Analyze]: Diat1=%d, Diat2=%d, MIDI1=%d, MIDI2=%d, ")
    //    _T("m_nNumIntv=%d, m_nSemi=%d, nPerfect=%d"),
    //    m_DPitch1, m_DPitch2, m_MPitch1, m_MPitch2, m_nNumIntv, m_nSemi, nPerfect) );

    //compute interval type
    int i;
    if (m_nNumIntv == 1 || m_nNumIntv == 4 || m_nNumIntv == 5 || m_nNumIntv == 8) {
        //perfect intervals
        i = abs(m_nSemi - nPerfect);
        if (m_nSemi < nPerfect) {
            if (i == 1) {
                m_nType = eti_Diminished;
            } else {
                m_nType = eti_DoubleDiminished;
            }
        } else if (m_nSemi > nPerfect) {
            if (i == 1) {
                m_nType = eti_Augmented;
            } else {
                m_nType = eti_DoubleAugmented;
            }
        } else {
            m_nType = eti_Perfect;
        }
    } else {
        if (m_nSemi < nPerfect) {
            i = nPerfect - m_nSemi;
            if (i == 1) {
                m_nType = eti_Minor;
            } else if (i == 2) {
                m_nType = eti_Diminished;
            } else {
                m_nType = eti_DoubleDiminished;
            }
        } else {
            i = m_nSemi - nPerfect;
            if (i == 0) {
                m_nType = eti_Major;
            } else if (i == 1) {
                m_nType = eti_Augmented;
            } else {
                m_nType = eti_DoubleAugmented;
            }
        }
    }

    //prepare interval name
    m_sName = m_sIntervalName[(fMajor ? m_nNumIntv + 7 : m_nNumIntv)];
    if (m_nNumIntv != 1) {    //Exclude Unisons
        switch (m_nType) {
            case eti_Diminished:        m_sName += _(" diminished");          break;
            case eti_Minor:             m_sName += _(" minor");               break;
            case eti_Major:             m_sName += _(" mayor");               break;
            case eti_Augmented:         m_sName += _(" augmented");           break;
            case eti_Perfect:           m_sName += _(" perfect");             break;
            case eti_DoubleAugmented:   m_sName += _(" double augmented");    break;
            case eti_DoubleDiminished:  m_sName += _(" double diminished");   break;
            default:
                wxASSERT(false);
        }
    }

    //reduce intervals greater than a octave
    if (fMajor) {
        m_nSemi += 12;
        m_nNumIntv += 7;
    }

}
wxString lmInterval::GetIntervalCode()
{
    lmIntvBits tIntv;
    tIntv.nNum = m_nNumIntv;
    tIntv.nSemitones = m_nSemi;
    return IntervalBitsToCode(tIntv);
}

void lmInterval::GetNoteBits(int i, lmNoteBits* pBits)
{
    wxASSERT(i==0 || i==1);

    lmConverter::NoteToBits(m_sPattern[i], pBits);

    //now, add the accidentas implied by key signature
    int nAccidentals[7];
    lmComputeAccidentals(m_nKey, nAccidentals);
    pBits->nAccidentals = nAccidentals[pBits->nStep];

}


//-------------------------------------------------------------------------------------
// Global methods
//-------------------------------------------------------------------------------------

wxString ComputeInterval(wxString sRootNote, wxString sIntvCode,
                         bool fAscending, lmEKeySignatures nKey)
{
    //Root note elements. i.e.: '+d4' -> (1, 4, 1)
    lmNoteBits tRoot;
    if (lmConverter::NoteToBits(sRootNote, &tRoot)) {
        wxLogMessage(_T("[ComputeInterval] Unexpected error in lmConverter::NoteToBits coversion. Note: '%s'"),
                sRootNote.c_str() );
        wxASSERT(false);
    }

    lmNoteBits tNew;
    ComputeInterval(&tRoot, sIntvCode, fAscending, &tNew);

    return lmConverter::NoteBitsToName(tNew, nKey);

}

void ComputeInterval(lmNoteBits* pRoot, wxString sIntvCode,
                     bool fAscending, lmNoteBits* pNewNote)
{
    //interval elements. i.e.: '5a' -> (5, 8)
    lmIntvBits tIntval;
    if (IntervalCodeToBits(sIntvCode, &tIntval)) {
        wxLogMessage(_T("[ComputeInterval] Unexpected error in IntervalCodeToBits coversion. Interval: '%s'"),
                sIntvCode.c_str() );
        wxASSERT(false);
    }

    if (fAscending) {
        // Compute ascending interval

        // compute new step
        int nNewStepFull = pRoot->nStep + tIntval.nNum - 1;
        pNewNote->nStep = nNewStepFull % 7;

        //compute octave increment
        int nIncrOctave = (pNewNote->nStep == nNewStepFull ? 0 : (nNewStepFull - pNewNote->nStep)/7 );
        pNewNote->nOctave = pRoot->nOctave + nIncrOctave;

        //compute new step semitones
        pNewNote->nStepSemitones = lmConverter::StepToSemitones(pNewNote->nStep);

        //compute new accidentals
        pNewNote->nAccidentals = (pRoot->nStepSemitones + tIntval.nSemitones -
                            pNewNote->nStepSemitones) % 12 + pRoot->nAccidentals;
        if (pNewNote->nAccidentals > 5) pNewNote->nAccidentals -= 12;
    }
    else {
        //compute descending interval

        // compute new step and octave decrement
        int nNewStepFull = pRoot->nStep - (tIntval.nNum - 1);
        pNewNote->nStep = nNewStepFull % 7;
        int nIncrOctave = 0;    //assume no octave change
        if (nNewStepFull < 0) {
            if (pNewNote->nStep < 0) pNewNote->nStep += 7;
            nIncrOctave = (nNewStepFull - pNewNote->nStep)/7 ;
        }
        pNewNote->nOctave = pRoot->nOctave + nIncrOctave;

        //compute new step semitones
        pNewNote->nStepSemitones = lmConverter::StepToSemitones(pNewNote->nStep);

        //compute new accidentals
        pNewNote->nAccidentals = (pRoot->nStepSemitones - tIntval.nSemitones -
                            pNewNote->nStepSemitones) % 12 + pRoot->nAccidentals;
        if (pNewNote->nAccidentals < -5) pNewNote->nAccidentals += 12;
    }


}

bool IntervalCodeToBits(wxString sIntvCode, lmIntvBits* pBits)
{
    //return true if error
    //Restrictions: any interval , including greater that one octave, but limited to
    // d, m , p , M and a. That is, it is not allowed double augmented, double diminished, etc.

    //split interval number and type
    int i = 0;
    while (!(sIntvCode.Mid(i, 1)).IsNumber() ) {
        i++;
    }
    wxString sChar = sIntvCode.substr(0, i);
    wxString sNum = sIntvCode.substr(i);

    long nNum;
    sNum.ToLong(&nNum);
    pBits->nNum = (int)nNum;

    //reduce the interval
    int nOctaves = (nNum - 1) / 7;
    nNum = 1 + (nNum - 1) % 7;

    // compute semitones implied by interval type
    int nSemi;
    if (nNum == 1)      nSemi = -1;
    else if (nNum == 2) nSemi = 0;
    else if (nNum == 3) nSemi = 2;
    else if (nNum == 4) nSemi = 4;
    else if (nNum == 5) nSemi = 6;
    else if (nNum == 6) nSemi = 7;
    else if (nNum == 7) nSemi = 9;
    else
    {
        wxLogMessage(_T("[IntervalCodeToBits] Program error: Invalid interval '%s', nNum=%d"),
                sIntvCode.c_str(), nNum );
        wxASSERT(false);    //impossible
    }

    if ( nNum == 1 || nNum == 4 || nNum == 5) {
        if (sChar == _T("d"))       nSemi += 0;
        else if (sChar == _T("p"))  nSemi += 1;
        else if (sChar == _T("a"))  nSemi += 2;
        else
        {
            wxLogMessage(_T("[IntervalCodeToBits] Program error: Invalid interval '%s', nNum=%d, sChar='%s'"),
                    sIntvCode.c_str(), nNum, sChar.c_str() );
            wxASSERT(false);    //impossible
        }
    }
    else {  // 2, 3, 6, 7
        if (sChar == _T("d"))       nSemi += 0;
        else if (sChar == _T("m"))  nSemi += 1;
        else if (sChar == _T("M"))  nSemi += 2;
        else if (sChar == _T("a"))  nSemi += 3;
        else
        {
            wxLogMessage(_T("[IntervalCodeToBits] Program error: Invalid interval '%s', nNum=%d, sChar='%s'"),
                    sIntvCode.c_str(), nNum, sChar.c_str() );
            wxASSERT(false);    //impossible
        }
    }
    pBits->nSemitones = nSemi + 12 * nOctaves;

    return false;

}

wxString IntervalBitsToCode(lmIntvBits& tIntv)
{
    g_pLogger->LogTrace(_T("lmInterval"),
            _T("IntervalBitsToCode: nNum=%d, nSemitones=%d\n"),
            tIntv.nNum, tIntv.nSemitones );

    wxString sNormal = _T("dmMa");
    wxString sPerfect = _T("dpa");
    int nSemitones = tIntv.nSemitones % 12;
    int nNum = 1 + (tIntv.nNum - 1) % 7;
    wxString sResp;
    if (nNum == 1)  {
        // unison, octave
        // For unison nSemitones = -1, 0, 1
        // For octave nSemitones = 11, 12, 13 (reduced in previous step to 11,0,1)
        //    so we need to correct semitones
        if (nSemitones == 11) nSemitones = -1;
        sResp = sPerfect.substr(nSemitones + 1, 1);
    }
    else if (nNum == 2)
        sResp = sNormal.substr(nSemitones, 1);
    else if (nNum == 3)
        sResp = sNormal.substr(nSemitones - 2, 1);
    else if (nNum == 4)
        sResp = sPerfect.substr(nSemitones - 4, 1);
    else if (nNum == 5)
        sResp = sPerfect.substr(nSemitones - 6, 1);
    else if (nNum == 6)
        sResp = sNormal.substr(nSemitones - 7, 1);
    else if (nNum == 7)
        sResp = sNormal.substr(nSemitones - 9, 1);
    else
        wxASSERT(false);    //impossible


    sResp += wxString::Format(_T("%d"), tIntv.nNum);
    return sResp;

}




//-------------------------------------------------------------------------------------
// FIntval
// Interval implementation based on FIntval.
//-------------------------------------------------------------------------------------

// an entry for the table of intervals data
typedef struct {
    lmEIntervalType nType;
    int             nNumIntv;
} lmIntvalData;

static const lmIntvalData m_aIntvData[40] =
{
    {/*  0 - lm_p1  */  eti_Perfect,            1 },   
    {/*  1 - lm_a1  */  eti_Augmented,          1 },
    {/*  2 - lm_da1 */  eti_DoubleAugmented,    1 },
    {/*  3 - lm_dd2 */  eti_DoubleDiminished,   2 },
    {/*  4 - lm_d2  */  eti_Diminished,         2 },
    {/*  5 - lm_m2  */  eti_Minor,              2 },
    {/*  6 - lm_M2  */  eti_Major,              2 },
    {/*  7 - lm_a2  */  eti_Augmented,          2 },
    {/*  8 - lm_da2 */  eti_DoubleAugmented,    2 },
    {/*  9 - lm_dd3 */  eti_DoubleDiminished,   3 },
    {/* 10 - lm_d3  */  eti_Diminished,         3 },
    {/* 11 - lm_m3  */  eti_Minor,              3 },
    {/* 12 - lm_M3  */  eti_Major,              3 },
    {/* 13 - lm_a3  */  eti_Augmented,          3 },
    {/* 14 - lm_da3 */  eti_DoubleAugmented,    3 },
    {/* 15 - lm_dd4 */  eti_DoubleDiminished,   4 },
    {/* 16 - lm_d4  */  eti_Diminished,         4 },
    {/* 17 - lm_p4  */  eti_Perfect,            4 },
    {/* 18 - lm_a4  */  eti_Augmented,          4 },
    {/* 19 - lm_da4 */  eti_DoubleAugmented,    4 },
    {/*empty*/          (lmEIntervalType)0,     0 },
    {/* 21 - lm_dd5 */  eti_DoubleDiminished,   5 },
    {/* 22 - lm_d5  */  eti_Diminished,         5 },
    {/* 23 - lm_p5  */  eti_Perfect,            5 },
    {/* 24 - lm_a5  */  eti_Augmented,          5 },
    {/* 25 - lm_da5 */  eti_DoubleAugmented,    5 },
    {/* 26 - lm_dd6 */  eti_DoubleDiminished,   6 },
    {/* 27 - lm_d6  */  eti_Diminished,         6 },
    {/* 28 - lm_m6  */  eti_Minor,              6 },
    {/* 29 - lm_M6  */  eti_Major,              6 },
    {/* 30 - lm_a6  */  eti_Augmented,          6 },
    {/* 31 - lm_da6 */  eti_DoubleAugmented,    6 },
    {/* 32 - lm_dd7 */  eti_DoubleDiminished,   7 },
    {/* 33 - lm_d7  */  eti_Diminished,         7 },
    {/* 34 - lm_m7  */  eti_Minor,              7 },
    {/* 35 - lm_M7  */  eti_Major,              7 },
    {/* 36 - lm_a7  */  eti_Augmented,          7 },
    {/* 37 - lm_da7 */  eti_DoubleAugmented,    7 },
    {/* 38 - lm_dd8 */  eti_DoubleDiminished,   8 },
    {/* 39 - lm_d8  */  eti_Diminished,         8 },
};


void FIntval_InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

    m_sIntervalName[0] = _T("");
    m_sIntervalName[1] = _("Unison");
    m_sIntervalName[2] = _("2nd");
    m_sIntervalName[3] = _("3rd");
    m_sIntervalName[4] = _("4th");
    m_sIntervalName[5] = _("5th");
    m_sIntervalName[6] = _("6th");
    m_sIntervalName[7] = _("7th");
    m_sIntervalName[8] = _("octave");
    m_sIntervalName[9] = _("9th");
    m_sIntervalName[10] = _("10th");
    m_sIntervalName[11] = _("11th");
    m_sIntervalName[12] = _("12th");
    m_sIntervalName[13] = _("13th");
    m_sIntervalName[14] = _("14th");
    m_sIntervalName[15] = _("Two octaves");

    m_fStringsInitialized = true;
}

lmFIntval FIntval(wxString& sName)
{
    // unison
    if (sName == _T("p1")) return lm_p1;
    if (sName == _T("a1")) return lm_a1;
    // second
    if (sName == _T("d2")) return lm_d2;
    if (sName == _T("m2")) return lm_m2;
    if (sName == _T("M2")) return lm_M2;
    if (sName == _T("a2")) return lm_a2;
    // third
    if (sName == _T("d3")) return lm_d3;
    if (sName == _T("m3")) return lm_m3;
    if (sName == _T("M3")) return lm_M3;
    if (sName == _T("a3")) return lm_a3;
    // fourth
    if (sName == _T("d4")) return lm_d4;
    if (sName == _T("p4")) return lm_p4;
    if (sName == _T("a4")) return lm_a4;
    // fifth
    if (sName == _T("d5")) return lm_d5;
    if (sName == _T("p5")) return lm_p5;
    if (sName == _T("a5")) return lm_a5;
    //sixth
    if (sName == _T("d6")) return lm_d6;
    if (sName == _T("m6")) return lm_m6;
    if (sName == _T("M6")) return lm_M6;
    if (sName == _T("a6")) return lm_a6;
    // seventh
    if (sName == _T("d7")) return lm_d7;
    if (sName == _T("m7")) return lm_m7;
    if (sName == _T("M7")) return lm_M7;
    if (sName == _T("a7")) return lm_a7;
    // octave
    if (sName == _T("d8")) return lm_d8;
    if (sName == _T("p8")) return lm_p8;

    return lmNULL_FIntval;

}

lmFIntval FIntval_FromType(int nIntv, lmEIntervalType nType)
{
    for (int i=0; i < 40; i++)
    {
        if (m_aIntvData[i].nNumIntv == nIntv && m_aIntvData[i].nType == nType)
            return i;
    }
    wxASSERT(false);
}

int FIntval_GetNumber(lmFIntval fi)
{
    //returns interval number: 1=unison, 2=2nd, ..., 8=8ve, 9=9th, ..., 15=2 octaves, ...

    int nOctave = (fi / 40) * 7;
    fi %= 40;
    // Here fi= 0..39

    return m_aIntvData[fi].nNumIntv + nOctave;
}

wxString FIntval_GetIntvCode(lmFIntval fi)
{
    //returns interval code

    int nOctave = ((fi / 40) * 7);
    fi %= 40;
    // Here fi= 0..39
    nOctave += m_aIntvData[fi].nNumIntv;

    wxString sCode = _T("");
    switch (m_aIntvData[fi].nType)
    {
        case eti_Diminished:        return wxString::Format(_T("d%d"), nOctave);
        case eti_Minor:             return wxString::Format(_T("m%d"), nOctave);
        case eti_Major:             return wxString::Format(_T("M%d"), nOctave);
        case eti_Augmented:         return wxString::Format(_T("a%d"), nOctave);
        case eti_Perfect:           return wxString::Format(_T("p%d"), nOctave);
        case eti_DoubleAugmented:   return wxString::Format(_T("da%d"), nOctave);
        case eti_DoubleDiminished:  return wxString::Format(_T("dd%d"), nOctave);
        default:
            wxASSERT(false);
    }
    return wxEmptyString;
}

wxString FIntval_GetName(lmFIntval fi)
{
    //AWARE: This method is restricted to two octaves

    wxASSERT(fi < 81);      // 80 = two octaves

    int nOctave = (fi / 40) * 7;
    int fiIndex = fi % 40;
    // Here fi= 0..39

    int nNumIntv = m_aIntvData[fiIndex].nNumIntv + nOctave;              // 0..15
    if (!m_fStringsInitialized) FIntval_InitializeStrings();
    wxString sName = m_sIntervalName[nNumIntv];
    if (fi == 0)
        sName = _("Unison");
    else if (fi == 1)
        sName = _("Chromatic semitone");
    else if (fi == 2)
        sName = _("Chromatic tone");
    else
    {    
        switch (m_aIntvData[fiIndex].nType)
        {
            case eti_Diminished:        sName += _(" diminished");          break;
            case eti_Minor:             sName += _(" minor");               break;
            case eti_Major:             sName += _(" mayor");               break;
            case eti_Augmented:         sName += _(" augmented");           break;
            case eti_Perfect:           sName += _(" perfect");             break;
            case eti_DoubleAugmented:   sName += _(" double augmented");    break;
            case eti_DoubleDiminished:  sName += _(" double diminished");   break;
            default:
                wxASSERT(false);
        }
    }
    return sName;
}

lmEIntervalType FIntval_GetType(lmFIntval fi)
{
    fi %= 40;   //Now fi= 0..39
    return m_aIntvData[fi].nType;
}

