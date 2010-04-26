//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
#pragma implementation "ScalesConstrains.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ScalesConstrains.h"
#include "Generators.h"

// the config object
extern wxConfigBase *g_pPrefs;


lmScalesConstrains::lmScalesConstrains(wxString sSection)
    : lmExerciseOptions(sSection)
{
    LoadSettings();
}

bool lmScalesConstrains::IsValidGroup(EScaleGroup nGroup)
{
    if (nGroup == esg_Major)
    {
        return (IsScaleValid( est_MajorNatural ) ||
                IsScaleValid( est_MajorTypeII ) ||
                IsScaleValid( est_MajorTypeIII ) ||
                IsScaleValid( est_MajorTypeIV ) );
    }
    else if(nGroup == esg_Minor)
    {
        return (IsScaleValid( est_MinorNatural ) ||
                IsScaleValid( est_MinorDorian ) ||
                IsScaleValid( est_MinorHarmonic ) ||
                IsScaleValid( est_MinorMelodic ) );
    }
    else if(nGroup == esg_Gregorian)
    {
        return (IsScaleValid( est_GreekIonian ) ||
                IsScaleValid( est_GreekDorian ) ||
                IsScaleValid( est_GreekPhrygian ) ||
                IsScaleValid( est_GreekLydian ) ||
                IsScaleValid( est_GreekMixolydian ) ||
                IsScaleValid( est_GreekAeolian ) ||
                IsScaleValid( est_GreekLocrian ) );
    }
    else if(nGroup == esg_Other)
    {
        return (IsScaleValid( est_PentatonicMinor ) ||
                IsScaleValid( est_PentatonicMajor ) ||
                IsScaleValid( est_Blues ) ||
                IsScaleValid( est_WholeTones ) ||
                IsScaleValid( est_Chromatic ) );
    }
    else {
        wxASSERT(false);    //impossible
        return false;
    }

}

void lmScalesConstrains::SaveSettings()
{
    //
    // save settings in user configuration data file
    //

    // allowed scales
    int i;
    wxString sKey;
    for (i=0; i < est_Max; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/Scale%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fValidScales[i]);
    }

    // key signatures
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        fValid = m_oValidKeys.IsValid((lmEKeySignatures)i);
        g_pPrefs->Write(sKey, fValid);
    }

    // other settings
    sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/DisplayKey"), m_sSection.c_str());
    g_pPrefs->Write(sKey, m_fDisplayKey);
    sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/PlayMode"), m_sSection.c_str());
    g_pPrefs->Write(sKey, m_nPlayMode);

}

void lmScalesConstrains::LoadSettings()
{
    //
    // load settings form user configuration data or default values
    //

    // allowed scales. Default: major natural, minor natural, harmonic and melodic
    int i;
    wxString sKey;
    for (i=0; i < est_Max; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/Scale%dAllowed"),
            m_sSection.c_str(), i );
        lmEScaleType nType = (lmEScaleType)i;
        bool fDefault = (nType == est_MajorNatural || nType == est_MinorNatural 
                         || nType == est_MinorHarmonic || nType == est_MinorMelodic);

        g_pPrefs->Read(sKey, &m_fValidScales[i], fDefault);
    }

    // key signatures. Default all
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &fValid, true);
        m_oValidKeys.SetValid((lmEKeySignatures)i, fValid);
    }

    // other settings:
    //      Display key - default: no
    sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/DisplayKey"), m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_fDisplayKey, false);
    // play modes. Default: ascending
    sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/PlayMode"), m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_nPlayMode, 0);   //0-ascending

}

lmEScaleType lmScalesConstrains::GetRandomScaleType()
{
    lmRandomGenerator oGenerator;
    int nWatchDog = 0;
    int nType = oGenerator.RandomNumber(0, est_Max-1);
    while (!IsScaleValid((lmEScaleType)nType)) {
        nType = oGenerator.RandomNumber(0, est_Max-1);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmScalesConstrains::GetRandomChordType."));
            return (lmEScaleType)0;
        }
    }
    return (lmEScaleType)nType;

}

bool lmScalesConstrains::GetRandomPlayMode()
{
    //return 'true' for ascending and 'false' for descending

    if (m_nPlayMode == 0) //ascending
        return true;
    else if (m_nPlayMode == 1) //descending
        return false;
    else {  // both modes allowed. Choose one at random
        lmRandomGenerator oGenerator;
        return oGenerator.FlipCoin();
   }
}

lmEScaleType lmScaleShortNameToType(const wxString& sName)
{
    // returns -1 if error
    //
    //  major: MN (natural), MH (harmonic), M3 (type III), MM (mixolydian)
    //  minor: mN (natural), mM (melodic), mD (dorian), mH (harmonic)
    //  medieval modes: Do (Dorian), Ph (Phrygian), Ly (Lydian),
    //                  Mx (Mixolydian), Ae (Aeolian), Io (Ionian),
    //                  Lo (Locrian)
    //  other: Pm (Pentatonic minor), PM (Pentatonic Major), Bl (Blues)
    //  non-tonal: WT (Whole Tones), Ch (Chromatic)


    // Major scales
    if      (sName == _T("MN")) return est_MajorNatural;
    else if (sName == _T("MH")) return est_MajorTypeII;
    else if (sName == _T("M3")) return est_MajorTypeIII;
    else if (sName == _T("MM")) return est_MajorTypeIV;

    // Minor scales
    else if (sName == _T("mN")) return est_MinorNatural;
    else if (sName == _T("mD")) return est_MinorDorian;
    else if (sName == _T("mH")) return est_MinorHarmonic;
    else if (sName == _T("mM")) return est_MinorMelodic;

    // Gregorian modes
    else if (sName == _T("Io")) return est_GreekIonian;
    else if (sName == _T("Do")) return est_GreekDorian;
    else if (sName == _T("Ph")) return est_GreekPhrygian;
    else if (sName == _T("Ly")) return est_GreekLydian;
    else if (sName == _T("Mx")) return est_GreekMixolydian;
    else if (sName == _T("Ae")) return est_GreekAeolian;
    else if (sName == _T("Lo")) return est_GreekLocrian;

    // Other scales
    else if (sName == _T("Pm")) return est_PentatonicMinor;
    else if (sName == _T("PM")) return est_PentatonicMajor;
    else if (sName == _T("Bl")) return est_Blues;

    //non-tonal scales 
    else if (sName == _T("WT")) return est_WholeTones; 
    else if (sName == _T("Ch")) return est_Chromatic;

    return (lmEScaleType)-1;  //error
}

bool lmIsMinorScale(lmEScaleType nType)
{
    return (nType >= est_MinorNatural && nType <= est_LastMinor);
}

