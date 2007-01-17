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
/*! @file ScalesManager.cpp
    @brief Implementation file for class lmScalesManager
    @ingroup auxmusic
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ScalesManager.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ScalesManager.h"
#include "Conversion.h"
#include "../ldp_parser/AuxString.h"
#include "../exercises/Generators.h"
#include "../score/KeySignature.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

typedef struct lmScaleDataStruct {
    bool fFromRootNote;
    wxString sIntervals;
} lmScaleData;

static wxString m_sScaleName[est_Max];
static m_fStringsInitialized = false;

//! @aware Array indexes are in correspondence with enum EScaleType
// - intervals are from root note if flag is 'true'. Otherwise, 
//   if flag is 'false', intervals are from previous note
//
static const lmScaleData tData[est_Max] = { 
        //Major scales
    { false, _T("M2,M2,m2,M2,M2,M2,m2,") },     //Major natural 
    { false, _T("M2,M2,m2,M2,m2,a2,m2,") },     //Major TypeII
    { false, _T("M2,M2,m2,M2,m2,M2,M2,") },     //Major TypeIII
    { false, _T("M2,M2,m2,M2,M2,m2,M2,") },     //Major TypeIV
        // Minor scales
    { false, _T("M2,m2,M2,M2,m2,M2,M2,") },     //Minor Natural,
    { false, _T("M2,m2,M2,M2,M2,m2,M2,") },     //Minor Dorian,
    { false, _T("M2,m2,M2,M2,m2,a2,m2,") },     //Minor Harmonic,
    { false, _T("M2,m2,M2,M2,M2,M2,m2,") },     //Minor Melodic,
        // Greek modes
    { false, _T("2s,2s,1s,2s,2s,2s,1s,") },     //Greek Ionian (major natural),
    { false, _T("2s,1s,2s,2s,2s,1s,2s,") },     //Greek Dorian,
    { false, _T("1s,2s,2s,2s,1s,2s,2s,") },     //Greek Phrygian,
    { false, _T("2s,2s,2s,1s,2s,2s,1s,") },     //Greek Lydian,
    { false, _T("2s,2s,1s,2s,2s,1s,2s,") },     //Greek Mixolydian,
    { false, _T("2s,1s,2s,2s,1s,2s,2s,") },     //Greek Aeolian (minor natural),
    { false, _T("1s,2s,2s,1s,2s,2s,2s,") },     //Greek Locrian,
        // Other scales
    { false, _T("3s,2s,2s,3s,2s,") },           //Pentatonic Minor,
    { false, _T("2s,2s,3s,2s,3s,") },           //Pentatonic Major,
    { false, _T("3s,2s,1s,1s,3s,2s,") },        //Blues,
    { false, _T("2s,2s,2s,2s,2s,2s,") },        //WholeTones,
    { false, _T("1s,1s,1s,1s,1s,1s,1s,1s,1s,1s,1s,1s,") },   //Chromatic,
};


//-------------------------------------------------------------------------------------
// Implementation of lmScalesManager class


lmScalesManager::lmScalesManager(wxString sRootNote, EScaleType nScaleType, 
                                 EKeySignatures nKey)
{
//    //save parameters
    m_nType = nScaleType;
    m_nKey = nKey;

    if (lmConverter::NoteToBits(sRootNote, &m_tBits[0])) {
        wxLogMessage(_T("[lmScalesManager::lmScalesManager] Unexpected error in lmConverter::NoteToBits coversion. Note: '%s'"),
                sRootNote );
        wxASSERT(false);
    }

    //get notes that form the scale
    int i;
    int nNumNotes = GetNumNotes();
    for (i=1; i < nNumNotes; i++) {
        //get the next interval
        wxString sIntval = (tData[m_nType].sIntervals).Mid((i-1)*3, 2);
        //compute next note
        if (sIntval.GetChar(1) == _T('s')) {
            //Non modal interval
            if (tData[m_nType].fFromRootNote)
                AddSemitonesToNote(&m_tBits[0], sIntval, m_nKey, edi_Ascending, &m_tBits[i]);
            else
                AddSemitonesToNote(&m_tBits[i-1], sIntval, m_nKey, edi_Ascending, &m_tBits[i]);
        }
        else {
            //Modal interval
            if (tData[m_nType].fFromRootNote)
                ComputeInterval(&m_tBits[0], sIntval, true, &m_tBits[i]);
            else
                ComputeInterval(&m_tBits[i-1], sIntval, true, &m_tBits[i]);
        }
        //wxLogMessage(_T("[lmScalesManager] Intval='%s', Note %d = %s, (Bits: Step=%d, Octave=%d, Accidentals=%d, StepSemitones=%d), key=%d"),
        //             sIntval, i, lmConverter::NoteBitsToName(m_tBits[i],m_nKey),
        //             m_tBits[i].nStep, m_tBits[i].nOctave, m_tBits[i].nAccidentals, m_tBits[i].nStepSemitones,
        //             m_nKey );
    }

}

lmScalesManager::~lmScalesManager()
{
}

int lmScalesManager::GetNumNotes()
{
    return 1 + ((tData[m_nType].sIntervals).Length() / 3);
}

wxString lmScalesManager::GetPattern(int i)
{
    // Returns LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < GetNumNotes());
    return lmConverter::NoteBitsToName(m_tBits[i], m_nKey);

}

//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

wxString ScaleTypeToName(EScaleType nType)
{
    wxASSERT(nType < est_Max);

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized)
    {
        // Major scales
        m_sScaleName[est_MajorNatural] = _("Major natural");
        m_sScaleName[est_MajorTypeII] = _("Major type II");
        m_sScaleName[est_MajorTypeIII] = _("Major type III");
        m_sScaleName[est_MajorTypeIV] = _("Major type IV");

        // Minor scales
        m_sScaleName[est_MinorNatural] = _("Minor natural");
        m_sScaleName[est_MinorDorian] = _("Minor Dorian");
        m_sScaleName[est_MinorHarmonic] = _("Minor Harmonic");
        m_sScaleName[est_MinorMelodic] = _("Minor Melodic");

        // Greek scales
        m_sScaleName[est_GreekIonian] = _("Greek Ionian");
        m_sScaleName[est_GreekDorian] = _("Greek Dorian");
        m_sScaleName[est_GreekPhrygian] = _("Greek Phrygian");
        m_sScaleName[est_GreekLydian] = _("Greek Lydian");
        m_sScaleName[est_GreekMixolydian] = _("Greek Mixolydian");
        m_sScaleName[est_GreekAeolian] = _("Greek Aeolian");
        m_sScaleName[est_GreekLocrian] = _("Greek Locrian");

        // Other scales
        m_sScaleName[est_PentatonicMinor] = _("Pentatonic minor");
        m_sScaleName[est_PentatonicMajor] = _("Pentatonic major");
        m_sScaleName[est_Blues] = _("Blues");
        m_sScaleName[est_WholeTones] = _("Whole tones");
        m_sScaleName[est_Chromatic] = _("Chromatic");

        m_fStringsInitialized = true;
    }
    
    return m_sScaleName[nType];
    
}

int NumNotesInScale(EScaleType nType)
{
    wxASSERT(nType < est_Max);
    return 1 + ((tData[nType].sIntervals).Length() / 3);
}
