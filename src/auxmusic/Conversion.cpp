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
#pragma implementation "Conversion.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/Score.h"
#include "Conversion.h"

static wxString sEnglishNoteName[7] = {
            _T("c"),  _T("d"), _T("e"), _T("f"), _T("g"), _T("a"), _T("b") };

static bool m_fStringsInitialized = false;
static wxString sNoteName[7];


lmConverter::lmConverter()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized) {
        sNoteName[0] = _("c");
        sNoteName[1] = _("d");
        sNoteName[2] = _("e");
        sNoteName[3] = _("f");
        sNoteName[4] = _("g");
        sNoteName[5] = _("a");
        sNoteName[6] = _("b");
        m_fStringsInitialized = true;
    }

}


//---------------------------------------------------------------------------------------
// lmDPitch: Diatonic pitch
//---------------------------------------------------------------------------------------

lmPitch lmConverter::DPitchToMPitch(lmDPitch nPitch)
{
    int nOctave = (nPitch - 1) / 7;
    int nRemainder = nPitch % 7;

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

int lmConverter::GetStepFromDPitch(lmDPitch nPitch)
{
    return ((int)nPitch - 1) % 7;
}

int lmConverter::GetOctaveFromDPitch(lmDPitch nPitch)
{
    return  ((int)nPitch - 1) / 7;
}

lmDPitch lmConverter::DPitch(int nStep, int nOctave)
{
    return nOctave * 7 + nStep + 1;
}

//---------------------------------------------------------------------------------------
// lmMPitch: MIDI pitch
//---------------------------------------------------------------------------------------

bool lmConverter::IsNaturalNote(lmMPitch ntMidi, EKeySignatures nKey)
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
    return (sScale.Mid(nRemainder, 1) == _T("1"));

}



lmPitch lmConverter::MPitchToDPitch(lmMPitch nMidiPitch)
{
    // Returns the diatonic pitch that corresponds to the received MIDI pitch.
    // It is assumed C major key signature.

    //! @aware: To make this algorithm indepedent from the choosen pitch scale,
    //!         we will use constant lmC4PITCH to determine the right value.

    int nOctave = (nMidiPitch - 12) / 12;
    int nRemainder = nMidiPitch % 12;
    int nPitch = nOctave * 7 + 1;      // C note
    int nShift = lmC4PITCH - 29;       // if nOctave is 4, nPicht will be 29
    int nScale[] = {0,0,1,1,2,3,3,4,4,5,5,6};
    return nPitch + nShift + nScale[nRemainder];

}

wxString lmConverter::MPitchToLDPName(lmMPitch nMidiPitch)
{
    // Returns the LDP diatonic pitch that corresponds to the received MIDI pitch.
    // It is assumed C major key signature.

    int nOctave = (nMidiPitch - 12) / 12;
    int nRemainder = nMidiPitch % 12;
    wxString sNote[] = {_T("c"), _T("+c"), _T("d"), _T("+d"), _T("e"), _T("f"), _T("+f"),
                        _T("g"), _T("+g"), _T("a"), _T("+a"), _T("b") };
    return sNote[nRemainder] + wxString::Format(_T("%d"), nOctave);

}

wxString lmConverter::DPitchToLDPName(lmDPitch nPitch)
{
    // Returns the LDP note name that corresponds to the received pitch. For exaplample,
    // pitch 29 will return "c4".

    int nOctave = (nPitch - 1) / 7;
    int nStep = (nPitch -1) % 7;
    wxString sNote[] = {_T("c"), _T("d"), _T("e"), _T("f"), _T("g"), _T("a"), _T("b") };
    return sNote[nStep] + wxString::Format(_T("%d"), nOctave);

}

//---------------------------------------------------------------------------------------
// Notes
//---------------------------------------------------------------------------------------

bool lmConverter::NoteToBits(wxString sNote, lmNoteBits* pBits)
{
    //Returns true if error
    //- sNote must be letter followed by a number (i.e.: "c4" ) optionally precedeed by
    //  accidentals (i.e.: "++c4")
    //- It is assumed that sNote is trimmed (no spaces before or after data)
    //  and lower case.

    //split the string: accidentals and name
    wxString sAccidentals;
    switch (sNote.Len()) {
        case 2:
            sAccidentals = _T("");
            break;
        case 3:
            sAccidentals = sNote.Mid(0, 1);
            sNote = sNote.Mid(1, 2);
            break;
        case 4:
            sAccidentals = sNote.Mid(0, 2);
            sNote = sNote.Mid(2, 2);
            break;
        default:
            return true;   //error
    }

    //compute step
    int nStep = StepToInt( sNote.Left(1) );
    if (nStep == -1) return true;   //error
    pBits->nStep = nStep;

    //compute octave
    wxString sOctave = sNote.Mid(1, 1);
    long nOctave;
    if (!sOctave.ToLong(&nOctave)) return true;    //error
    pBits->nOctave = (int)nOctave;

    //compute accidentals
    int nAccidentals = AccidentalsToInt(sAccidentals);
    if (nAccidentals == -999) return true;  //error
    pBits->nAccidentals = nAccidentals;

    //compute step semitones
    pBits->nStepSemitones = StepToSemitones( pBits->nStep );

    return false;  //no error

}

bool lmConverter::DPitchToBits(lmDPitch nPitch, lmNoteBits* pBits)
{
    //Returns true if error
    //Accidentals are set to 'none' (zero)

    //compute step
    pBits->nStep = GetStepFromDPitch(nPitch);

    //compute octave
    pBits->nOctave = GetOctaveFromDPitch(nPitch);

    //compute accidentals
    pBits->nAccidentals = 0;

    //compute step semitones
    pBits->nStepSemitones = StepToSemitones( pBits->nStep );

    return false;  //no error

}


wxString lmConverter::NoteBitsToName(lmNoteBits& tBits, EKeySignatures nKey)
{
    static wxString m_sSteps = _T("cdefgab");

    // Get the accidentals implied by the key signature.
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
    int nAccidentals[7];
    ComputeAccidentals(nKey, nAccidentals);

    //compute accidentals note accidentals
    wxString sResult = _T("");
    if (tBits.nAccidentals == 1)
        sResult = _T("+");
    else if (tBits.nAccidentals == 2)
        sResult = _T("x");
    else if (tBits.nAccidentals == -1)
        sResult = _T("-");
    else if (tBits.nAccidentals == -2)
        sResult = _T("--");

    //change note accidentals to take key into account
    if (nAccidentals[tBits.nStep] != 0) {
        if (tBits.nAccidentals == nAccidentals[tBits.nStep])
            sResult = _T("");   //replace note accidental by key accidental
        else if (tBits.nAccidentals == 0)
            sResult = _T("=");  //force a natural
        else
            ;   //leave note accidentals
    }

    // compute step letter
    sResult += m_sSteps.Mid(tBits.nStep, 1);

    // compute octave
    sResult += wxString::Format(_T("%d"), tBits.nOctave);

    return sResult;

}

int lmConverter::StepToSemitones(int nStep)
{
    //Returns -1 if error
    // 'c'=0, 'd'=2, 'e'=4, 'f'=5, 'g'=7, 'a'=9, 'b'=11
    if (nStep == 0)  return 0;
    if (nStep == 1)  return 2;
    if (nStep == 2)  return 4;
    if (nStep == 3)  return 5;
    if (nStep == 4)  return 7;
    if (nStep == 5)  return 9;
    if (nStep == 6)  return 11;
    return -1;  //error
}

int lmConverter::AccidentalsToInt(wxString sAccidentals)
{
    //Returns -999 if error
    // '--'=-1, '-'=-1, ''=0, '+'=+1, '++'=+2 'x'=2
    if (sAccidentals == _T(""))     return 0;
    if (sAccidentals == _T("-"))    return -1;
    if (sAccidentals == _T("--"))   return -2;
    if (sAccidentals == _T("+"))    return 1;
    if (sAccidentals == _T("++"))   return 2;
    if (sAccidentals == _T("x"))    return 2;
    return -999;
}

int lmConverter::StepToInt(wxString sStep)
{
    //Returns -1 if error
    // 'c'=0, 'd'=1, 'e'=2, 'f'=3, 'g'=4, 'a'=5, 'b'=6
    if (sStep == _T("c")) return 0;
    if (sStep == _T("d")) return 1;
    if (sStep == _T("e")) return 2;
    if (sStep == _T("f")) return 3;
    if (sStep == _T("g")) return 4;
    if (sStep == _T("a")) return 5;
    if (sStep == _T("b")) return 6;
    return -1;
}



//---------------------------------------------------------------------------------------
// lmNotePitch
//---------------------------------------------------------------------------------------

lmNotePitch lmConverter::NoteNameToNotePitch(wxString sNote)
{
    // sNote must be letter followed by a number (i.e.: "c4" ) optionally precedeed by
    // accidentals (i.e.: "++c4")
    // It is assumed that sNote is trimmed (no spaces before or after data)
    // and lower case.

    //split the string: accidentals and name
    wxString sAccidentals;
    switch (sNote.Len()) {
        case 2:
            sAccidentals = _T("");
            break;
        case 3:
            sAccidentals = sNote.Mid(0, 1);
            sNote = sNote.Mid(1, 2);
            break;
        case 4:
            sAccidentals = sNote.Mid(0, 2);
            sNote = sNote.Mid(2, 2);
            break;
        default:
            wxLogMessage(_T("[lmConverter::NoteNameToNotePitch] Bad note name '%s'"), sNote.c_str());
            return lmC_NOTE | lmOCTAVE_4 | lmNO_ACCIDENTAL;
    }

    //compute step
    int nStep = StepToInt( sNote.Left(1) );
    if (nStep == -1) {
        wxLogMessage(_T("[lmConverter::NoteNameToNotePitch] Bad note name '%s'"), sNote.c_str());
        return lmC_NOTE | lmOCTAVE_4 | lmNO_ACCIDENTAL;
    }

    //compute octave
    wxString sOctave = sNote.Mid(1, 1);
    long nOctave;
    if (!sOctave.ToLong(&nOctave)) {
        wxLogMessage(_T("[lmConverter::NoteNameToNotePitch] Bad note name '%s'"), sNote.c_str());
        return lmC_NOTE | lmOCTAVE_4 | lmNO_ACCIDENTAL;
    }

    //compute accidentals
    int nAccidentals = AccidentalsToInt(sAccidentals);
    if (nAccidentals == -999) {
        wxLogMessage(_T("[lmConverter::NoteNameToNotePitch] Bad note name '%s'"), sNote.c_str());
        return lmC_NOTE | lmOCTAVE_4 | lmNO_ACCIDENTAL;
    }

    return lmGET_PITCH(nStep, (int)nOctave, nAccidentals);

}

lmDPitch lmConverter::NotePitchToDPitch(lmNotePitch nPitch)
{
    int nStep = lmGET_STEP(nPitch);
    int nOctave = lmGET_OCTAVE(nPitch);

    return nOctave * 7 + nStep + 1;
}

//---------------------------------------------------------------------------------------
// Pitch name
//---------------------------------------------------------------------------------------

wxString lmConverter::GetNoteName(lmDPitch nPitch)
{
    //returns the name of the note translated to current language
    int iNote = (nPitch - 1) % 7;
    return sNoteName[iNote];

}

wxString lmConverter::GetEnglishNoteName(lmDPitch nPitch)
{
    int iNota = (nPitch - 1) % 7;
    int nOctava = (nPitch - 1) / 7;

    return wxString::Format(_T("%s%d"), sEnglishNoteName[iNota].c_str(), nOctava );

}


