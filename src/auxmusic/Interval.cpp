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




static wxString sIntervalName[16];
static bool fStringsInitialized = false;

//-------------------------------------------------------------------------------------
// Implementation of lmInterval class


lmInterval::lmInterval(lmNote* pNote1, lmNote* pNote2, EKeySignatures nKey)
{
    if (!fStringsInitialized) InitializeStrings();

    //save parameters and compute the interval
    m_ntMidi1 = pNote1->GetMidiPitch();
    m_ntMidi2 = pNote2->GetMidiPitch();
    m_ntDiat1 = pNote1->GetDiatonicPitch();
    m_ntDiat2 = pNote2->GetDiatonicPitch();
    m_nKey = nKey;
    Analyze();

}

//Generate a random interval satisfying the received constraints.
lmInterval::lmInterval(bool fDiatonic, int ntDiatMin, int ntDiatMax, bool fAllowedIntervals[],
             bool fAscending, EKeySignatures nKey, int nMidiStartNote)
{
    if (!fStringsInitialized) InitializeStrings();

    //dbg------------------------------------------------------
    g_pLogger->LogTrace(_T("lmInterval"), _T("Direction = %s, type = %s\n"),
                    (fAscending ? _T("Ascending") : _T("Descending")),
                    (fDiatonic ? _T("Diatonic") : _T("Chromatic") ) );
    //end dbg------------------------------------------------

    m_nKey = nKey;

    //compute max number of semitones in the allowed note range
    int ntMidiMin = lmConverter::DPitchToMPitch(ntDiatMin);
    int ntMidiMax = lmConverter::DPitchToMPitch(ntDiatMax);
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
        m_ntMidi1 = 60;
        m_ntMidi2 = 65;
        m_sPattern[0] = MIDINoteToLDPPattern(m_ntMidi1, m_nKey, &m_ntDiat1);
        m_sPattern[1] = MIDINoteToLDPPattern(m_ntMidi2, m_nKey, &m_ntDiat2);
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
                if (lmConverter::IsNaturalNote((lmPitch)ntAux, m_nKey)) {
                    ntAux2 = (fAscending ? ntAux + nSelIntv : ntAux - nSelIntv);
                    if (lmConverter::IsNaturalNote((lmPitch)ntAux2, m_nKey)) fComply = true;
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
        m_ntMidi1 = 60;
        m_ntMidi2 = 65;
        m_sPattern[0] = MIDINoteToLDPPattern(m_ntMidi1, m_nKey, &m_ntDiat1);
        m_sPattern[1] = MIDINoteToLDPPattern(m_ntMidi2, m_nKey, &m_ntDiat2);
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
        m_ntMidi1 = nMidiStartNote;
    else {
        i = oGen.RandomNumber(0, nNumValidNotes - 1);
        m_ntMidi1 = nValidNotes.Item(i);
    }

    if (fAscending) {
        m_ntMidi2 = m_ntMidi1 + nSelIntv;
    } else {
        m_ntMidi2 = m_ntMidi1 - nSelIntv;
    }
    g_pLogger->LogTrace(_T("lmInterval"), _T("i=%d, ntMidi1=%d, ntMidi2=%d\n"),
                        i, m_ntMidi1, m_ntMidi2);

    //Interval successfully generated. Prepare interval information
    wxASSERT( m_ntMidi1 > 11 && m_ntMidi2 > 11);
    m_sPattern[0] = MIDINoteToLDPPattern(m_ntMidi1, m_nKey, &m_ntDiat1);
    //m_sPattern[1] = MIDINoteToLDPPattern(m_ntMidi2, m_nKey, &m_ntDiat2);

    // MIDINoteToLDPPattern() always returns notes with sharps. Therefore, the
    // second diatonic note might not correspond to the interval. For example,
    // if the interval is 7th Major (semitones=11) first note could be c5 and
    // second note will be c4+ instead of the right value d4-.
    // To fix this problem the second note will be generated from the first one

    // As this constructor is only used for ear training exercises, some intervals
    // are not valid and must be replaced by its enarmonic. For example:
    // d8 --> M7

    g_pLogger->LogTrace(_T("lmInterval"),
            _T("Before: ntMidi1=%d, ntMidi2=%d, m_ntDiat1=%d, m_ntDiat2=%d, nKey=%d, nSelIntv=%d, sPattern[0]='%s', sPattern[1]='%s', name=%s\n"),
            m_ntMidi1, m_ntMidi2, m_ntDiat1, m_ntDiat2, m_nKey, nSelIntv, m_sPattern[0].c_str(), m_sPattern[1].c_str(), m_sName.c_str());
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
                         (m_ntMidi2 > m_ntMidi1), m_nKey);

    //Get interval number
    wxString sIntvNum = sIntvCode.Mid(1);
    long nIntvNum;
    sIntvNum.ToLong(&nIntvNum);

    //compute diatonic pitch of second note
    if (m_ntMidi2 > m_ntMidi1)
        m_ntDiat2 = m_ntDiat1 + nIntvNum -1;
    else
        m_ntDiat2 = m_ntDiat1 - nIntvNum +1;

    g_pLogger->LogTrace(_T("lmInterval"),
            _T("After: ntMidi1=%d, ntMidi2=%d, m_ntDiat1=%d, m_ntDiat2=%d, nKey=%d, nSelIntv=%d, sPattern[0]='%s', sPattern[1]='%s', nIntvNum=%d\n"),
            m_ntMidi1, m_ntMidi2, m_ntDiat1, m_ntDiat2, m_nKey, nSelIntv, m_sPattern[0].c_str(), m_sPattern[1].c_str(), nIntvNum);

//    if (Abs(m_ntDiat1 - m_ntDiat2) <> nSelIntv) {
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
    m_nSemi = abs(m_ntMidi1 - m_ntMidi2);
    m_nNumIntv = abs(m_ntDiat1 - m_ntDiat2) + 1;

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
    //    m_ntDiat1, m_ntDiat2, m_ntMidi1, m_ntMidi2, m_nNumIntv, m_nSemi, nPerfect) );

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
    m_sName = sIntervalName[(fMajor ? m_nNumIntv + 7 : m_nNumIntv)];
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

    //reduce intervals greater than a octave
    if (fMajor) {
        m_nSemi += 12;
        m_nNumIntv += 7;
    }

}

void lmInterval::InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    sIntervalName[0] = _T("");
    sIntervalName[1] = _("Unison");
    sIntervalName[2] = _("2nd");
    sIntervalName[3] = _("3rd");
    sIntervalName[4] = _("4th");
    sIntervalName[5] = _("5th");
    sIntervalName[6] = _("6th");
    sIntervalName[7] = _("7th");
    sIntervalName[8] = _("octave");
    sIntervalName[9] = _("9th");
    sIntervalName[10] = _("10th");
    sIntervalName[11] = _("11th");
    sIntervalName[12] = _("12th");
    sIntervalName[13] = _("13th");
    sIntervalName[14] = _("14th");
    sIntervalName[15] = _("Two octaves");

    fStringsInitialized = true;
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
    ComputeAccidentals(m_nKey, nAccidentals);
    pBits->nAccidentals = nAccidentals[pBits->nStep];

}


//-------------------------------------------------------------------------------------
// Global methods
//-------------------------------------------------------------------------------------

wxString ComputeInterval(wxString sRootNote, wxString sIntvCode,
                         bool fAscending, EKeySignatures nKey)
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

void AddSemitonesToNote(lmNoteBits* pRoot, wxString sIntvCode, EKeySignatures nKey,
                               EIntervalDirection nDirection, lmNoteBits* pNewNote)
{
    // This function adds the requested semitones to note *pRoot and returns the resulting
    // note in *pNewNote.
    // Step and accidentals of new note are adjusted to fit 'naturally' in the key
    // signature received: one accidental at maximum, of the same type than the
    // accidentals in the key signature.
    // sIntvCode is one of '1s', '2s', ..., '9s' meaning '1 semitone', '2 semitones' , etc.


    //Determine what type of accidentals to use for semitones: flats or sharps
    bool fUseSharps = (KeySignatureToNumFifths(nKey) >= 0);

    //Get the number of semitones to add/substract
    wxString sSemitones = sIntvCode.Left(1);
    long nSemitones;
    sSemitones.ToLong(&nSemitones);
    if (nDirection == edi_Descending) nSemitones = -nSemitones;

    // compute new note total semitones and octave
    nSemitones += pRoot->nStepSemitones + pRoot->nAccidentals;
    pNewNote->nOctave = pRoot->nOctave;
    while (nSemitones > 11) {
        nSemitones -= 12;
        pNewNote->nOctave ++;
    }
    while (nSemitones < 0) {
        nSemitones += 12;
        pNewNote->nOctave --;
    }
    //here octave is adjusted and step semitones is >=0 and < 12.
    //compute step and accidentals
    switch (nSemitones) {
        case 0:
            pNewNote->nStep = 0;            // c
            pNewNote->nAccidentals = 0;
            break;
        case 1:
            if (fUseSharps) {
                pNewNote->nStep = 0;        // +c
                pNewNote->nAccidentals = 1;
            }
            else {
                pNewNote->nStep = 1;        // -d
                pNewNote->nAccidentals = -1;
            }
            break;
        case 2:
            pNewNote->nStep = 1;            // d
            pNewNote->nAccidentals = 0;
            break;
        case 3:
            if (fUseSharps) {
                pNewNote->nStep = 1;        // +d
                pNewNote->nAccidentals = 1;
            }
            else {
                pNewNote->nStep = 2;        // -e
                pNewNote->nAccidentals = -1;
            }
            break;
        case 4:
            pNewNote->nStep = 2;            // e
            pNewNote->nAccidentals = 0;
            break;
        case 5:
            pNewNote->nStep = 3;            // f
            pNewNote->nAccidentals = 0;
            break;
        case 6:
            if (fUseSharps) {
                pNewNote->nStep = 3;        // +f
                pNewNote->nAccidentals = 1;
            }
            else {
                pNewNote->nStep = 4;        // -g
                pNewNote->nAccidentals = -1;
            }
            break;
        case 7:
            pNewNote->nStep = 4;            // g
            pNewNote->nAccidentals = 0;
            break;
        case 8:
            if (fUseSharps) {
                pNewNote->nStep = 4;        // +g
                pNewNote->nAccidentals = 1;
            }
            else {
                pNewNote->nStep = 5;        // -a
                pNewNote->nAccidentals = -1;
            }
            break;
        case 9:
            pNewNote->nStep = 5;            // a
            pNewNote->nAccidentals = 0;
            break;
        case 10:
            if (fUseSharps) {
                pNewNote->nStep = 5;        // +a
                pNewNote->nAccidentals = 1;
            }
            else {
                pNewNote->nStep = 6;        // -b
                pNewNote->nAccidentals = -1;
            }
            break;
        case 11:
            pNewNote->nStep = 6;            // b
            pNewNote->nAccidentals = 0;
            break;
    }

    //compute step semitones
    pNewNote->nStepSemitones = lmConverter::StepToSemitones(pNewNote->nStep);

    //done

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
    wxString sChar = sIntvCode.Mid(0, i);
    wxString sNum = sIntvCode.Mid(i);

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

wxString InvertInterval(wxString sIntvCode)
{
    wxASSERT(sIntvCode.Length() == 2);

    if      (sIntvCode == _T("d2")) return _T("a7");
    else if (sIntvCode == _T("m2")) return _T("M7");
    else if (sIntvCode == _T("M2")) return _T("m7");
    else if (sIntvCode == _T("a2")) return _T("d7");
    else if (sIntvCode == _T("d3")) return _T("a6");
    else if (sIntvCode == _T("m3")) return _T("M6");
    else if (sIntvCode == _T("M3")) return _T("m6");
    else if (sIntvCode == _T("a3")) return _T("d6");
    else if (sIntvCode == _T("d4")) return _T("a4");
    else if (sIntvCode == _T("p4")) return _T("p4");
    else if (sIntvCode == _T("a4")) return _T("d4");
    else if (sIntvCode == _T("d5")) return _T("a5");
    else if (sIntvCode == _T("p5")) return _T("p5");
    else if (sIntvCode == _T("a5")) return _T("d5");
    else if (sIntvCode == _T("d6")) return _T("a3");
    else if (sIntvCode == _T("m6")) return _T("M3");
    else if (sIntvCode == _T("M6")) return _T("m3");
    else if (sIntvCode == _T("a6")) return _T("d3");
    else if (sIntvCode == _T("d7")) return _T("a2");
    else if (sIntvCode == _T("m7")) return _T("M2");
    else if (sIntvCode == _T("M7")) return _T("m2");
    else if (sIntvCode == _T("a7")) return _T("d2");

    wxASSERT(false);
    return sIntvCode; //error

}

wxString AddIntervals(wxString sIntvCode1, wxString sIntvCode2)
{
    wxASSERT(sIntvCode1.Length() == 2 && sIntvCode2.Length() == 2);

    // get interval elements
    lmIntvBits tIntv1, tIntv2;
    if (IntervalCodeToBits(sIntvCode1, &tIntv1)) {
        wxLogMessage(_T("[AddIntervals] Unexpected error in IntervalCodeToBits coversion. Interval: '%s'"),
                sIntvCode1.c_str() );
        wxASSERT(false);
    }

    if (IntervalCodeToBits(sIntvCode2, &tIntv2)) {
        wxLogMessage(_T("[AddIntervals] Unexpected error in IntervalCodeToBits coversion. Interval: '%s'"),
                sIntvCode1.c_str() );
        wxASSERT(false);
    }

    //compute addition
    lmIntvBits tSum;
    tSum.nNum = tIntv1.nNum + tIntv2.nNum - 1;
    tSum.nSemitones = tIntv1.nSemitones + tIntv2.nSemitones;

    //rebuild interval name
    return IntervalBitsToCode(tSum);

}

wxString SubstractIntervals(wxString sIntvCode1, wxString sIntvCode2)
{
    wxASSERT(sIntvCode1.Length() == 2 && sIntvCode2.Length() == 2);

    //It is assumed that intv1 > int2

    // get interval elements
    lmIntvBits tIntv1, tIntv2;
    if (IntervalCodeToBits(sIntvCode1, &tIntv1)) {
        wxLogMessage(_T("[SubstractIntervals] Unexpected error in IntervalCodeToBits coversion. Interval: '%s'"),
                sIntvCode1.c_str() );
        wxASSERT(false);
    }

    if (IntervalCodeToBits(sIntvCode2, &tIntv2)) {
        wxLogMessage(_T("[SubstractIntervals] Unexpected error in IntervalCodeToBits coversion. Interval: '%s'"),
                sIntvCode2.c_str() );
        wxASSERT(false);
    }

    //compute difference
    lmIntvBits tDif;
    tDif.nNum = tIntv1.nNum - tIntv2.nNum + 1;
    tDif.nSemitones = tIntv1.nSemitones - tIntv2.nSemitones;

    //rebuild interval name
    return IntervalBitsToCode(tDif);

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
        sResp = sPerfect.Mid(nSemitones + 1, 1);
    }
    else if (nNum == 2)
        sResp = sNormal.Mid(nSemitones, 1);
    else if (nNum == 3)
        sResp = sNormal.Mid(nSemitones - 2, 1);
    else if (nNum == 4)
        sResp = sPerfect.Mid(nSemitones - 4, 1);
    else if (nNum == 5)
        sResp = sPerfect.Mid(nSemitones - 6, 1);
    else if (nNum == 6)
        sResp = sNormal.Mid(nSemitones - 7, 1);
    else if (nNum == 7)
        sResp = sNormal.Mid(nSemitones - 9, 1);
    else
        wxASSERT(false);    //impossible


    sResp += wxString::Format(_T("%d"), tIntv.nNum);
    return sResp;

}

#if 0
//analize the interval and compute its name
wxString GetIntvName(lmIntvBits* pBits)
{
    int nSemitones = pBits->nSemitones;
    int nNum = pBits->nNum;
    EIntervalType nType;

    //trim to reduce intervals greater than one octave
    bool fMajor = (nNum > 8);
    if (fMajor) {
        nSemitones -= 12;
        nNum -= 7;
    }

    //compute the number of semitones required to be perfect or major (p.84 Atlas)
    int nPerfect = 2 * (nNum - 1);
    if (nNum > 3) nPerfect--;            //intervals greater than 3rd loose a semitone
    if (nNum == 8) nPerfect = 12;        //the octave has 12 semitones

    //at this point:
    //   nSemitones = num. of semitones in the interval
    //   nNum = number of the interval
    //   nPerfect = num. of semitones that should have to be perfect or major

    //compute interval type
    int i;
    if (nNum == 1 || nNum == 4 || nNum == 5 || nNum == 8) {
        //perfect intervals
        i = abs(nSemitones - nPerfect);
        if (nSemitones < nPerfect) {
            if (i == 1) {
                nType = eti_Diminished;
            } else {
                nType = eti_DoubleDiminished;
            }
        } else if (nSemitones > nPerfect) {
            if (i == 1) {
                nType = eti_Augmented;
            } else {
                nType = eti_DoubleAugmented;
            }
        } else {
            nType = eti_Perfect;
        }
    } else {
        if (nSemitones < nPerfect) {
            i = nPerfect - nSemitones;
            if (i == 1) {
                nType = eti_Minor;
            } else if (i == 2) {
                nType = eti_Diminished;
            } else {
                nType = eti_DoubleDiminished;
            }
        } else {
            i = nSemitones - nPerfect;
            if (i == 0) {
                nType = eti_Major;
            } else if (i == 1) {
                nType = eti_Augmented;
            } else {
                nType = eti_DoubleAugmented;
            }
        }
    }

    //prepare interval name
    wxString sName = sIntervalName[(fMajor ? nNum + 7 : nNum)];
    switch (nType) {
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

    //rebuild intervals greater than a octave
    if (fMajor) {
        nSemitones += 12;
        nNum += 7;
    }

    return sName;

}

#endif
