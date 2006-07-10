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
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file Interval.cpp
    @brief Implementation file for class lmInterval
    @ingroup auxmusic
*/
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

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;



static wxString sIntervalName[16];
static fStringsInitialized = false;

//-------------------------------------------------------------------------------------
// Implementation of lmInterval class


lmInterval::lmInterval(lmNote* pNote1, lmNote* pNote2, EKeySignatures nKey)
{
    if (!fStringsInitialized) InitializeStrings();

    //save parameters and compute the interval
    m_ntMidi1 = pNote1->GetMidiPitch();
    m_ntMidi2 = pNote2->GetMidiPitch();
    m_ntDiat1 = pNote1->GetPitch();
    m_ntDiat2 = pNote2->GetPitch();
    m_nKey = nKey;
    Analyze();
    
}

//Generate a random interval satisfying the received constrains.
lmInterval::lmInterval(bool fDiatonic, int ntDiatMin, int ntDiatMax,
        bool AllowedIntervals[], EIntervalDirection nDir, EKeySignatures nKey)
{
    if (!fStringsInitialized) InitializeStrings();

    bool fAscending = ((nDir == edi_Ascending || nDir == edi_Both) && (rand() & 0x01));
    //dbg------------------------------------------------------
    g_pLogger->LogTrace(_T("lmInterval"), _T("Posibles (nDir) = %s, %s, %s\n"), 
                    (nDir == edi_Ascending ? _T("Ascendentes") : 
                        (nDir==edi_Both ? _T("Ambos") : _T("Descendentes"))),
                    (fAscending ? _T("Ascendente") : _T("Descendente")),
                    (fDiatonic ? _T("Diatónico") : _T("Cromático") ) );
    //end dbg------------------------------------------------
    
    m_nKey = nKey;
    
    //compute max number of semitones in the allowed note range
    int ntMidiMin = lmConverter::PitchToMidiPitch(ntDiatMin);
    int ntMidiMax = lmConverter::PitchToMidiPitch(ntDiatMax);
    int nRange = ntMidiMax - ntMidiMin;
    wxArrayInt nValidNotes;       //to store the midi notes that can be used
    g_pLogger->LogTrace(_T("lmInterval"), _T("MidiMin=%d, MidiMax=%d\n"), 
                        ntMidiMin, ntMidiMax);
    
    //compute allowed intervals for that range of notes
    int nMaxIntv = nRange;
    int nAllowedIntv[25];
    wxString sDbgMsg = _T("Intv:");
    int i;
    int nNumIntv = 0;
    for (i=0; i < wxMin(nRange, 25); i++) {
        if (AllowedIntervals[i]) {
            nAllowedIntv[nNumIntv] = i;
            nNumIntv++;
            sDbgMsg += wxString::Format(_T(" %d"), i);
        }
    }
    sDbgMsg += _T("\n");
    g_pLogger->LogTrace(_T("lmInterval"), sDbgMsg);
    if (nNumIntv == 0) {
        wxMessageBox( _T("No puede generarse ningún intervalo. El ámbito es menor que el primer intervalo permitido") );
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
        wxMessageBox(_("It is not possible to generate an interval satisfying the constrains imposed by the choosen settings."));
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
    sDbgMsg = wxString::Format(_T("Notas válidas: nNumValidNotes=%d : "), nNumValidNotes);
    for (int kk = 0; kk < nNumValidNotes; kk++) {
        sDbgMsg += wxString::Format(_T(" %d,"), nValidNotes[kk] );
    }
    g_pLogger->LogTrace(_T("lmInterval"), sDbgMsg);
    //end dbg --------------------------------------------
        
    i = oGen.RandomNumber(0, nNumValidNotes - 1);
    m_ntMidi1 = nValidNotes.Item(i);
    
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
    m_sPattern[1] = MIDINoteToLDPPattern(m_ntMidi2, m_nKey, &m_ntDiat2);
//    if (Abs(m_ntDiat1 - m_ntDiat2) <> nSelIntv) {
//        //mala suerte. Estamos en el caso de no ajustar a notas propias de la tonalidad y
//        //el intervalo generado, aunque correcto, es aumentado o disminuido, con lo que
//        //su nombre no corresponde con los de los botones de respuesta. Generar otro.
//        MsgBox "mala suerte"
//    }
    Analyze();

}

//analize the interval and fill up internal variables (i.e. name) according the analysis
void lmInterval::Analyze()
{
    m_nSemi = abs(m_ntMidi1 - m_ntMidi2);
    m_nIntv = abs(m_ntDiat1 - m_ntDiat2) + 1;
    
    //trim to reduce intervals greater than one octave
    bool fMajor = (m_nIntv > 8);
    if (fMajor) {
        m_nSemi -= 12;
        m_nIntv -= 7;
    }
    
    //compute the number of semitones required to be perfect or major (p.84 Atlas)
    int nPerfect = 2 * (m_nIntv - 1);
    if (m_nIntv > 3) nPerfect--;            //intervals greater than 3rd loose a semitone 
    if (m_nIntv == 8) nPerfect = 12;        //the octave has 12 semitones
    
    //at this point:
    //   m_nSemi = num. of semitones in the interval
    //   m_nIntv = number of the interval
    //   nPerfect = num. of semitones that should have to be perfect or major

    //wxLogMessage( wxString::Format(
    //    _T("[lmInterval::Analyze]: Diat1=%d, Diat2=%d, MIDI1=%d, MIDI2=%d, ")
    //    _T("m_nIntv=%d, m_nSemi=%d, nPerfect=%d"),
    //    m_ntDiat1, m_ntDiat2, m_ntMidi1, m_ntMidi2, m_nIntv, m_nSemi, nPerfect) );
    
    //compute interval type
    int i;
    if (m_nIntv == 1 || m_nIntv == 4 || m_nIntv == 5 || m_nIntv == 8) {
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
    m_sName = sIntervalName[(fMajor ? m_nIntv + 7 : m_nIntv)];
    switch (m_nType) {
        case eti_Diminished:        m_sName += _(" diminished");        break;    
        case eti_Minor:                m_sName += _(" minor");                break;    
        case eti_Major:                m_sName += _(" mayor");                break;    
        case eti_Augmented:            m_sName += _(" augmented");            break;    
        case eti_Perfect:            m_sName += _(" perfect");            break;    
        case eti_DoubleAugmented:    m_sName += _(" double augmented");    break;    
        case eti_DoubleDiminished:    m_sName += _(" double diminished");    break;    
        default:
            wxASSERT(false);
    }
    
    //reduce intervals graater than a octave
    if (fMajor) {
        m_nSemi += 12;
        m_nIntv += 7;
    }
    
    //patterns with note names and accidentals (but no key signature accidentals)
    //! @todo
//    m_sPattern[0] = NotaMidiToPatron(m_ntMidi1, m_nKey, m_ntDiat1);
//    m_sPattern[1] = NotaMidiToPatron(m_ntMidi2, m_nKey, m_ntDiat2);
       
    
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

