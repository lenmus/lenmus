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
#pragma implementation "Pitch.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/utils.h"
#include "defs.h"
#include "Score.h"


static wxString m_sNoteName[7] = {
            _T("c"),  _T("d"), _T("e"), _T("f"), _T("g"), _T("a"), _T("b") };

//interval names. The index in this array is the FInterval value
wxString m_sFIntvalCode[41] = {
    _T("p1"), _T("a1"), _T("no"), _T("no"), _T("d2"), _T("m2"), _T("M2"), _T("a2"),
    _T("no"), _T("no"), _T("d3"), _T("m3"), _T("M3"), _T("a3"), _T("no"), _T("no"),
    _T("d4"), _T("p4"), _T("a4"), _T("no"), _T("no"), _T("no"), _T("d5"), _T("p5"),
    _T("a5"), _T("no"), _T("no"), _T("d6"), _T("m6"), _T("M6"), _T("a6"), _T("no"),
    _T("no"), _T("d7"), _T("m7"), _T("M7"), _T("a7"), _T("no"), _T("no"), _T("d8"),
    _T("p8")
};


//-------------------------------------------------------------------------------------
// implementation of lmFPitch: Absolute pitch, interval-invariant (base-40)
//-------------------------------------------------------------------------------------


lmFPitch FPitch(lmDPitch dp, int nAcc)
{
    return FPitch(DPitch_Step(dp), DPitch_Octave(dp), nAcc);
}

lmFPitch FPitch(lmAPitch ap)
{
    return FPitch(ap.Step(), ap.Octave(), ap.Accidentals());
}

lmFPitch FPitch(int nStep, int nOctave, int nAcc)
{
    //  Cbb Cb  C   C#  C## -   Dbb Db  D   D#  D## -   Ebb Eb  E   E#  E## Fbb Fb  F   F#  F## -
    //  1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23
    //  
    //  Gbb Gb  G   G#  G## -   Abb Ab  A   A#  A## -   Bbb Bb  B   B#  B##
    //  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40
    //
    //  Pitch generation:
    //  Note:       C=3, D=9, E=15, F=20, G=26, A=32, B=38
    //  Diff:           6    6     5     6     6    6     5
    //  Step:         0    1    2     3     4     5     6
    //  step*6+2:     2    8    14    20    26    32    38
    //  if s<3  ++    3    9    15    20    26    32    38
    //  +=Acc
    //
    //  Step extraction:
    //  Note:       C=1-5, D=7-11, E=13-17, F=18-22, G=24-28, A=30-34, B=36-40
    //  Step:         0    1       2        3        4        5        6
    //  n/6:          0-0  1-1     2-2      3-3      4-4      5-5      6-6
    //
    //  Octave extraction:
    //  Note (n):     0=1-40, 1=41-80, 2=81-120, m=(m*40)+1 - (m+1)*40
    //  (n-1)/40:     0       2        3         m
    //
    //  Accidentals extraction
    //  Note (n):        0=1-40, 1=41-80, 2=81-120, m=(m*40)+1 - (m+1)*40
    //  x=((n-1)%40)+1:  1-40    1-40     1-40      1-40
    //  step=n/6
    //  pitch approx. = step*6+2
    //  acc = x - pitch
    //  if step<3 acc--

    int fpPitch = (nOctave * 40) + (nStep * 6) + 2;
    if (nStep < 3) fpPitch++;
    return fpPitch + nAcc;

}

bool FPitch_IsValid(lmFPitch fp)
{
    //returns false for invalid pitches (6, 12, 23, 29 & 35)

    int x = ((fp - 1) % 40 ) + 1;
    return !(x == 6 || x == 12 || x == 23 || x == 29 || x == 35);
}

lmFPitch FPitch(wxString& sNote)
{
    // sNote must be letter followed by a number (i.e.: "c4" ) optionally precedeed by
    // accidentals (i.e.: "++c4")
    // It is assumed that sNote is trimmed (no spaces before or after data)
    // and lower case.

    //split the string: accidentals and name
    wxString sAccidentals;
    switch (sNote.length()) {
        case 2:
            sAccidentals = _T("");
            break;
        case 3:
            sAccidentals = sNote.substr(0, 1);
            sNote = sNote.substr(1, 2);
            break;
        case 4:
            sAccidentals = sNote.substr(0, 2);
            sNote = sNote.substr(2, 2);
            break;
        default:
            wxLogMessage(_T("[lmFPitch constructor] Bad note name '%s'"), sNote.c_str());
            return lmC4_FPITCH;
    }

    //compute step
    wxChar sStep = sNote.at(0);
    int nStep;
    if (sStep == _T('c')) nStep = 0;
    else if (sStep == _T('d')) nStep =  1;
    else if (sStep == _T('e')) nStep =  2;
    else if (sStep == _T('f')) nStep =  3;
    else if (sStep == _T('g')) nStep =  4;
    else if (sStep == _T('a')) nStep =  5;
    else if (sStep == _T('b')) nStep =  6;
    else {
        wxLogMessage(_T("[lmFPitch constructor] Bad note name '%s'"), sNote.c_str());
        return lmC4_FPITCH;
    }

    //compute octave
    wxString sOctave = sNote.substr(1, 1);
    long nOctave;
    if (!sOctave.ToLong(&nOctave)) {
        wxLogMessage(_T("[lmFPitch constructor] Bad note name '%s'"), sNote.c_str());
        return lmC4_FPITCH;
    }

    //compute accidentals
    int nAccidentals;
    if (sAccidentals == _T(""))          nAccidentals = 0;
    else if (sAccidentals == _T("-"))    nAccidentals = -1;
    else if (sAccidentals == _T("--"))   nAccidentals = -2;
    else if (sAccidentals == _T("+"))    nAccidentals = 1;
    else if (sAccidentals == _T("++"))   nAccidentals = 2;
    else if (sAccidentals == _T("x"))    nAccidentals = 2;
    else {
        wxLogMessage(_T("[lmFPitch constructor] Bad note name '%s'"), sNote.c_str());
        return lmC4_FPITCH;
    }

    return FPitch(nStep, nOctave, nAccidentals);

}

int FPitch_Accidentals(lmFPitch fp)
{
    //  Accidentals extraction
    //  Note (n):        0=1-40, 1=41-80, 2=81-120, m=(m*40)+1 - (m+1)*40
    //  x=((n-1)%40)+1:  1-40    1-40     1-40      1-40
    //  step=x/6
    //  compute pitch
    //  acc = x - pitch

    int nReduction = ((fp - 1) % 40 ) + 1;
    int nStep = nReduction / 6;
    int nPitch = (nStep * 6) + 2;
    if (nStep < 3) nPitch++;
    return nReduction - nPitch;
}

wxString FPitch_ToAbsLDPName(lmFPitch fp)
{
    // The absolute LDP none name is returned
    // If note is invalid (more than two accidentals) returns empty string

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
    sAnswer += wxString::Format(_T("%d"), FPitch_Octave(fp));
    return sAnswer;
}

wxString FPitch_ToRelLDPName(lmFPitch fp, lmEKeySignatures nKey)
{
    // The note LDP name, relative to received key signature, is returned

    // Get the accidentals implied by the key signature.
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
    int nAccidentals[7];
    ComputeAccidentals(nKey, nAccidentals);

    //compute note accidentals
    wxString sAnswer;
    int nAbsAcc = FPitch_Accidentals(fp);
    switch(nAbsAcc) {
        case -2: sAnswer =_T("--"); break;
        case -1: sAnswer =_T("-"); break;
        case 0:  sAnswer =_T(""); break;
        case 1:  sAnswer =_T("+"); break;
        case 2:  sAnswer =_T("x"); break;
        default:
            sAnswer = _T("");
    }

    //change note accidentals to take key into account
    int nStep = FPitch_Step(fp);
    if (nAccidentals[nStep] != 0) {
        if (nAbsAcc == nAccidentals[nStep])
            sAnswer = _T("");   //replace note accidental by key accidental
        else if (nAbsAcc == 0)
            sAnswer = _T("=");  //force a natural
        //else
            //leave note accidentals
    }

    // add step letter and octave number
    sAnswer += m_sNoteName[nStep];
    sAnswer += wxString::Format(_T("%d"), FPitch_Octave(fp));

    return sAnswer;
}

lmMPitch FPitch_ToMPitch(lmFPitch fp)
{
    wxASSERT(lmC4_DPITCH == 29);    //AWARE It's assumed that we start in C0
    lmMPitch nMidi = (lmMPitch)((FPitch_Octave(fp)+1) * 12);

    switch(FPitch_ToDPitch(fp) % 7)
    {
        case 0:  //si
            nMidi = nMidi + 11;
            break;
        case 1:  //do
            //nothing to add. The value is ok
            break;
        case 2:  //re
            nMidi = nMidi + 2;
            break;
        case 3:  //mi
            nMidi = nMidi + 4;
            break;
        case 4:  //fa
            nMidi = nMidi + 5;
            break;
        case 5:  //sol
            nMidi = nMidi + 7;
            break;
        case 6:  //la
            nMidi = nMidi + 9;
            break;
    }

    return nMidi + FPitch_Accidentals(fp);

}

lmDPitch FPitch_ToDPitch(lmFPitch fp)
{
    return DPitch(FPitch_Step(fp), FPitch_Octave(fp));
}

lmAPitch FPitch_ToAPitch(lmFPitch fp)
{
    return lmAPitch(FPitch_ToDPitch(fp), FPitch_Accidentals(fp));
}

lmFPitch FPitch_AddSemitone(lmFPitch fpNote, lmEKeySignatures nKey)
{
    // This function adds one semitone to note fpNote and returns the resulting
    // note.
    // Step and accidentals of new note are adjusted to fit 'naturally' in the key
    // signature received: one accidental at maximum, of the same type than the
    // accidentals in the key signature.
    return FPitch_AddSemitone(fpNote, (KeySignatureToNumFifths(nKey) >= 0));
}


lmFPitch FPitch_AddSemitone(lmFPitch fpNote, bool fUseSharps)
{
    // This function adds one semitone to note fpNote and returns the resulting
    // note.
    // Step and accidentals of new note are adjusted to use one accidental at
    // maximum, of the same type as requested by fUseSharps

    // step 0   0   1   1   2   3   3   4   4   5   5   6
        //  C   C#  D   D#  E   F   F#  G   G#  A   A#  B
        //  3   4   9   10  15  20  21  26  27  32  33  38

    // step 0   1   1   2   2   3   4   4   5   5   6   6
        //  C   Db  D   Eb  E   F   Gb  G   Ab  A   Bb  B
        //  3   8   9   14  15  20  25  26  31  32  37  38


    // extract components
    int nStep = FPitch_Step(fpNote);
    int nAcc = FPitch_Accidentals(fpNote);
    int nOctave = FPitch_Octave(fpNote);

    //Determine what type of accidentals to use for semitones: flats or sharps
    if (fUseSharps)
    {
        //Key signature is using sharps.
        //Only one accidental allowed and must be a sharp
        if (nAcc == 0)
        {
            //if no accidentals add one sharp or advance step, depending on current step
            if (nStep == 2 || nStep == 6)
                nStep++;
            else
                nAcc++;
        }
        else
        {
            //assume one sharp: advance to next natural step
            nStep++;
            nAcc=0;
        }

        // Adjust octave
        if (nStep==7) {
            nStep = 0;
            nOctave++;
        }
    }
    else
    {
        //Key signature is using flats.
        //Only one accidental allowed and must be a flat
        if (nAcc == 0)
        {
            //if no accidentals advance step and, depending on new step, add one flat
            nStep++;
            if (nStep == 7) {
                nStep = 0;
                nOctave++;
            }
            else if (nStep != 3)
                nAcc--;
        }
        else
        {
            //assume one flat: remove accidentals to advance to the natural step
            nAcc=0;
        }
    }

    return FPitch(nStep, nOctave, nAcc);

}


//-------------------------------------------------------------------------------------
// implementation of class lmAPitch: Absoulte pitch
//-------------------------------------------------------------------------------------

lmAPitch::lmAPitch(const wxString& sNote)
{
    // sNote must be letter followed by a number (i.e.: "c4" ) optionally precedeed by
    // accidentals (i.e.: "++c4")
    // It is assumed that sNote is trimmed (no spaces before or after data)
    // and lower case.

    //split the string: accidentals and name
    wxString sAccidentals;
    int iStepPos;
    switch (sNote.length()) {
        case 2:
            sAccidentals = _T("");
            iStepPos = 0;
            break;
        case 3:
            sAccidentals = sNote.substr(0, 1);
            iStepPos = 1;
            break;
        case 4:
            sAccidentals = sNote.substr(0, 2);
            iStepPos = 2;
            break;
        default:
            wxLogMessage(_T("[lmAPitch constructor] Bad note name '%s'"), sNote.c_str());
            m_nDPitch = lmC4_DPITCH;
            m_nAcc = lmNO_ACCIDENTAL;
            return;
    }

    //compute step
    wxChar sStep = sNote.at(iStepPos);
    int nStep;
    if (sStep == _T('c')) nStep = 0;
    else if (sStep == _T('d')) nStep =  1;
    else if (sStep == _T('e')) nStep =  2;
    else if (sStep == _T('f')) nStep =  3;
    else if (sStep == _T('g')) nStep =  4;
    else if (sStep == _T('a')) nStep =  5;
    else if (sStep == _T('b')) nStep =  6;
    else {
        wxLogMessage(_T("[lmAPitch constructor] Bad note name '%s'"), sNote.c_str());
        m_nDPitch = lmC4_DPITCH;
        m_nAcc = lmNO_ACCIDENTAL;
        return;
    }

    //compute octave
    wxString sOctave = sNote.substr(1, 1);
    long nOctave;
    if (!sOctave.ToLong(&nOctave)) {
        wxLogMessage(_T("[lmAPitch constructor] Bad note name '%s'"), sNote.c_str());
        m_nDPitch = lmC4_DPITCH;
        m_nAcc = lmNO_ACCIDENTAL;
        return;
    }

    //compute accidentals
    int nAccidentals;
    if (sAccidentals == _T(""))          nAccidentals = 0;
    else if (sAccidentals == _T("-"))    nAccidentals = -1;
    else if (sAccidentals == _T("--"))   nAccidentals = -2;
    else if (sAccidentals == _T("+"))    nAccidentals = 1;
    else if (sAccidentals == _T("++"))   nAccidentals = 2;
    else if (sAccidentals == _T("x"))    nAccidentals = 2;
    else {
        wxLogMessage(_T("[lmAPitch constructor] Bad note name '%s'"), sNote.c_str());
        m_nDPitch = lmC4_DPITCH;
        m_nAcc = lmNO_ACCIDENTAL;
        return;
    }

    m_nDPitch = (int)nOctave * 7 + nStep + 1;;
    m_nAcc = nAccidentals;

}

wxString lmAPitch::LDPName() const
{
    wxString sAnswer;
    switch(m_nAcc) {
        case -2: sAnswer =_T("--"); break;
        case -1: sAnswer =_T("-"); break;
        case 0:  sAnswer =_T(""); break;
        case 1:  sAnswer =_T("+"); break;
        case 2:  sAnswer =_T("++"); break;
        default:
            sAnswer = _T("");
    }
    sAnswer += m_sNoteName[(m_nDPitch - 1) % 7];
    sAnswer += wxString::Format(_T("%d"), (m_nDPitch - 1) / 7 );
    return sAnswer;
}

const lmMPitch lmAPitch::GetMPitch() const
{
    int nOctave = Octave() + 1;
    wxASSERT(lmC4_DPITCH == 29);    //AWARE It's assumed that we start in C0
    lmMPitch nMidi = (lmMPitch)(nOctave * 12);

    switch(m_nDPitch % 7)
    {
        case 0:  //si
            nMidi = nMidi + 11;
            break;
        case 1:  //do
            //nothing to add. The value is ok
            break;
        case 2:  //re
            nMidi = nMidi + 2;
            break;
        case 3:  //mi
            nMidi = nMidi + 4;
            break;
        case 4:  //fa
            nMidi = nMidi + 5;
            break;
        case 5:  //sol
            nMidi = nMidi + 7;
            break;
        case 6:  //la
            nMidi = nMidi + 9;
            break;
    }

    return nMidi + m_nAcc;

}

//-------------------------------------------------------------------------------------
// implementation of pseudo-class lmDPitch: Diatonic pitch
//-------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// lmDPitch: Diatonic pitch
//---------------------------------------------------------------------------------------

lmMPitch DPitch_ToMPitch(lmDPitch dp)
{
    int nOctave = (dp - 1) / 7;
    int nRemainder = dp % 7;

    int nMidiPitch = nOctave * 12;
    switch (nRemainder) {
        case 0:  //si
            nMidiPitch = nMidiPitch + 11;   break;
        case 1:  //do
            //do nothing. Value is OK
            break;
        case 2:  //re
            nMidiPitch += 2;   break;
        case 3:  //mi
            nMidiPitch += 4;   break;
        case 4:  //fa
            nMidiPitch += 5;   break;
        case 5:  //sol
            nMidiPitch += 7;   break;
        case 6:  //la
            nMidiPitch += 9;   break;
    }
    nMidiPitch += 12;

    return nMidiPitch;

}

wxString DPitch_ToLDPName(lmDPitch dp)
{
    // Returns the LDP note name that corresponds to the received pitch. For example,
    // pitch 29 will return "c4".

    int nOctave = (dp - 1) / 7;
    int nStep = (dp -1) % 7;
    return m_sNoteName[nStep] + wxString::Format(_T("%d"), nOctave);

}

wxString DPitch_GetEnglishNoteName(lmDPitch dp)
{
    int iNota = (dp - 1) % 7;
    int nOctava = (dp - 1) / 7;
    return wxString::Format(_T("%s%d"), m_sNoteName[iNota].c_str(), nOctava );

}

lmFPitch DPitch_ToFPitch(lmDPitch dp, lmEKeySignatures nKey)
{
    // Get the accidentals implied by the key signature.
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
    int nAccidentals[7];
    ComputeAccidentals(nKey, nAccidentals);

    int nStep = DPitch_Step(dp);
    return FPitch(nStep, DPitch_Octave(dp), nAccidentals[nStep]);
}


//---------------------------------------------------------------------------------------
// lmMPitch: MIDI pitch
//---------------------------------------------------------------------------------------

bool MPitch_IsNaturalNote(lmMPitch ntMidi, lmEKeySignatures nKey)
{
    // Returns true if the Midi note corresponds to natural note of the key signature scale

    //Prepare string with "1" in natural tones of the scale
    wxString sScale;
    switch (nKey) {
        case earmDo:
        case earmLam:
            //           C D EF G A B
            sScale = _T("101011010101");   break;

        //Sostenidos ---------------------------------------
        case earmSol:
        case earmMim:
            //           C D EF G A B
            sScale = _T("101010110101");   break;
        case earmRe:
        case earmSim:
            //           C D EF G A B
            sScale = _T("011010110101");   break;
        case earmLa:
        case earmFasm:
            //           C D EF G A B
            sScale = _T("011010101101");   break;
        case earmMi:
        case earmDosm:
            //           C D EF G A B
            sScale = _T("010110101101");   break;
        case earmSi:
        case earmSolsm:
            //           C D EF G A B
            sScale = _T("010110101011");   break;
        case earmFas:
        case earmResm:
            //           C D EF G A B
            sScale = _T("010101101011");   break;
        case earmDos:
        case earmLasm:
            //           C D EF G A B
            sScale = _T("110101101010");   break;

        //Bemoles -------------------------------------------
        case earmFa:
        case earmRem:
            //           C D EF G A B
            sScale = _T("101011010110");   break;
        case earmSib:
        case earmSolm:
            //           C D EF G A B
            sScale = _T("101101010110");   break;
        case earmMib:
        case earmDom:
            //           C D EF G A B
            sScale = _T("101101011010");   break;
        case earmLab:
        case earmFam:
            //           C D EF G A B
            sScale = _T("110101011010");   break;
        case earmReb:
        case earmSibm:
            //           C D EF G A B
            sScale = _T("110101101010");   break;
        case earmSolb:
        case earmMibm:
            //           C D EF G A B
            sScale = _T("010101101011");   break;
        case earmDob:
        case earmLabm:
            //           C D EF G A B
            sScale = _T("010110101011");   break;
        default:
            wxASSERT(false);
    }

    int nRemainder = ntMidi % 12;      //nRemainder goes from 0 (Do) to 11 (Si)
    return (sScale.substr(nRemainder, 1) == _T("1"));

}

wxString MPitch_ToLDPName(lmMPitch nMidiPitch)
{
    // Returns the LDP diatonic pitch that corresponds to the received MIDI pitch.
    // AWARE: It is assumed C major key signature.

    int nOctave = (nMidiPitch - 12) / 12;
    int nRemainder = nMidiPitch % 12;
    wxString sNote[] = {_T("c"), _T("+c"), _T("d"), _T("+d"), _T("e"), _T("f"), _T("+f"),
                        _T("g"), _T("+g"), _T("a"), _T("+a"), _T("b") };
    return sNote[nRemainder] + wxString::Format(_T("%d"), nOctave);

}


