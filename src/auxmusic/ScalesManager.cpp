//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

typedef struct lmScaleInfoStruct {
    int nNumNotes;
    lmFIntval nInterval[lmNOTES_IN_SCALE];
} lmScaleInfo;


static wxString m_sScaleName[est_Max];
static bool m_fStringsInitialized = false;

// AWARE: Array indexes are in correspondence with enum EScaleType
// - intervals are from previous note

#define lm_1s  -2
static const lmScaleInfo tData[est_Max] = {
        //Major scales
    { 8, { lm_M2,lm_M2,lm_m2,lm_M2,lm_M2,lm_M2,lm_m2} },    //Major natural
    { 8, { lm_M2,lm_M2,lm_m2,lm_M2,lm_m2,lm_a2,lm_m2} },    //Major TypeII
    { 8, { lm_M2,lm_M2,lm_m2,lm_M2,lm_m2,lm_M2,lm_M2} },    //Major TypeIII
    { 8, { lm_M2,lm_M2,lm_m2,lm_M2,lm_M2,lm_m2,lm_M2} },    //Major TypeIV
        // Minor scales
    { 8, { lm_M2,lm_m2,lm_M2,lm_M2,lm_m2,lm_M2,lm_M2} },    //Minor Natural,
    { 8, { lm_M2,lm_m2,lm_M2,lm_M2,lm_M2,lm_m2,lm_M2} },    //Minor Dorian,
    { 8, { lm_M2,lm_m2,lm_M2,lm_M2,lm_m2,lm_a2,lm_m2} },    //Minor Harmonic,
    { 8, { lm_M2,lm_m2,lm_M2,lm_M2,lm_M2,lm_M2,lm_m2} },    //Minor Melodic,
        // Gregorian modes
    { 8, { lm_M2,lm_M2,lm_m2,lm_M2,lm_M2,lm_M2,lm_m2} },    //Gregorian Ionian (major natural),
    { 8, { lm_M2,lm_m2,lm_M2,lm_M2,lm_M2,lm_m2,lm_M2} },    //Gregorian Dorian,
    { 8, { lm_m2,lm_M2,lm_M2,lm_M2,lm_m2,lm_M2,lm_M2} },    //Gregorian Phrygian,
    { 8, { lm_M2,lm_M2,lm_M2,lm_m2,lm_M2,lm_M2,lm_m2} },    //Gregorian Lydian,
    { 8, { lm_M2,lm_M2,lm_m2,lm_M2,lm_M2,lm_m2,lm_M2} },    //Gregorian Mixolydian,
    { 8, { lm_M2,lm_m2,lm_M2,lm_M2,lm_m2,lm_M2,lm_M2} },    //Gregorian Aeolian (minor natural),
    { 8, { lm_m2,lm_M2,lm_M2,lm_m2,lm_M2,lm_M2,lm_M2} },    //Gregorian Locrian,
        // Other scales
    { 6, { lm_m3,lm_M2,lm_M2,lm_m3,lm_M2} },                //Pentatonic Minor,
    { 6, { lm_M2,lm_M2,lm_m3,lm_M2,lm_m3} },                //Pentatonic Major,
    { 7, { lm_m3,lm_M2,lm_m2,lm_m2,lm_m3,lm_M2} },          //Blues,
        // Non-tonal scales
    { 7, { lm_M2,lm_M2,lm_M2,lm_M2,lm_M2,lm_M2} },          //WholeTones,
    {13, { lm_1s,lm_1s,lm_1s,lm_1s,lm_1s,lm_1s,lm_1s,lm_1s,lm_1s,lm_1s,lm_1s,lm_1s} },       //Chromatic,
};


//-------------------------------------------------------------------------------------
// Implementation of lmScalesManager class


lmScalesManager::lmScalesManager(wxString sRootNote, EScaleType nScaleType,
                                 lmEKeySignatures nKey)
{
    //save parameters
    m_nType = nScaleType;
    m_nKey = nKey;

    m_fpNote[0] = FPitch(sRootNote);

    //for non-tonal scales, key signature is not used
    bool fUseSharps = lmRandomGenerator::FlipCoin();

    //get notes that form the scale
    int nNumNotes = GetNumNotes();
    if (m_nType == est_Chromatic) 
    {
        for (int i=1; i < nNumNotes; i++)
        {
            m_fpNote[i] = FPitch_AddSemitone(m_fpNote[i-1], fUseSharps);
        }
    }
    else if (m_nType == est_WholeTones)
    {
        for (int i=1; i < nNumNotes; i++)
        {
            m_fpNote[i] = FPitch_AddSemitone(m_fpNote[i-1], fUseSharps);
            m_fpNote[i] = FPitch_AddSemitone(m_fpNote[i], fUseSharps);
        }
    }
    else 
    {
        for (int i=1; i < nNumNotes; i++) {
            m_fpNote[i] = m_fpNote[i-1] + tData[m_nType].nInterval[i-1];
        }
    }

}

lmScalesManager::~lmScalesManager()
{
}

int lmScalesManager::GetNumNotes()
{
    return tData[m_nType].nNumNotes;
}

wxString lmScalesManager::GetPattern(int i)
{
    // Returns relative LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < GetNumNotes());
    return FPitch_ToRelLDPName(m_fpNote[i], m_nKey);

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

        // Gregorian modes
        m_sScaleName[est_GreekIonian] = _("Ionian");
        m_sScaleName[est_GreekDorian] = _("Dorian");
        m_sScaleName[est_GreekPhrygian] = _("Phrygian");
        m_sScaleName[est_GreekLydian] = _("Lydian");
        m_sScaleName[est_GreekMixolydian] = _("Mixolydian");
        m_sScaleName[est_GreekAeolian] = _("Aeolian");
        m_sScaleName[est_GreekLocrian] = _("Locrian");

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
    return tData[nType].nNumNotes;
}

bool IsScaleMajor(EScaleType nType)
{
    return (nType <= est_LastMajor);
}

bool IsScaleMinor(EScaleType nType)
{
    return (nType > est_LastMajor && nType <= est_LastMinor);
}

bool IsScaleGregorian(EScaleType nType)
{
    return (nType > est_LastMinor && nType <= est_LastGreek);
}

