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
#pragma implementation "EarIntvalConstrains.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EarIntvalConstrains.h"

// the config object
extern wxConfigBase *g_pPrefs;


lmEarIntervalsConstrains::lmEarIntervalsConstrains(wxString sSection)
    : lmExerciseConstrains(sSection)
{
    m_sSection = sSection;
    LoadSettings();

}

void lmEarIntervalsConstrains::SaveSettings()
{
    //
    //save settings in user configuration data file
    //

    // allowed intervals
    int i;
    wxString sKey;
    for (i=0; i < lmNUM_INTVALS; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/Interval%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fIntervalAllowed[i]);
    }

    // notes range
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/MinPitch"), m_sSection.c_str());
    g_pPrefs->Write(sKey, (long)m_nMinPitch);
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/MaxPitch"), m_sSection.c_str());
    g_pPrefs->Write(sKey, (long)m_nMaxPitch);

    // intervals types
    for (i=0; i < 3; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/IntervalType%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fTypeAllowed[i]);
    }

    // accidentals and key signatures
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/OnlyNatural"),
                        m_sSection.c_str());
    g_pPrefs->Write(sKey, m_fOnlyNatural);
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        fValid = m_oValidKeys.IsValid((EKeySignatures)i);
        g_pPrefs->Write(sKey, fValid);
    }

    // for interval comparison exercises
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/FirstEqual"), m_sSection.c_str());
    g_pPrefs->Write(sKey, m_fFirstEqual);

}

void lmEarIntervalsConstrains::LoadSettings()
{
    /*
    load settings form user configuration data or default values
    */

    // allowed intervals. Default: all in one octave range
    int i;
    wxString sKey;
    for (i=0; i < lmNUM_INTVALS; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/Interval%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &m_fIntervalAllowed[i], (bool)(i < 13) );
    }

    // notes range. Default A3 to A5
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/MinPitch"),
                m_sSection.c_str());
    m_nMinPitch = (int) g_pPrefs->Read(sKey, 27L);      // 27 = A3
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/MaxPitch"),
                m_sSection.c_str());
    m_nMaxPitch = (int) g_pPrefs->Read(sKey, 41L);      // 41 = A5

    // intervals types. Default: all types allowed
    for (i=0; i < 3; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/IntervalType%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &m_fTypeAllowed[i], true);
    }

    // accidentals and key signatures. Default use only natual intervals from C major scale
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/OnlyNatural"), m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_fOnlyNatural, true);    //use only natural intervals
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &fValid, (bool)((EKeySignatures)i == earmDo) );
        m_oValidKeys.SetValid((EKeySignatures)i, fValid);
    }

    // for interval comparison exercises
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/FirstEqual"), m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_fFirstEqual, true);    // first note equal in both intervals

}

