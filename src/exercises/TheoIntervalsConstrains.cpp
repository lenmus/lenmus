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
#pragma implementation "TheoIntervalsConstrains.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "TheoIntervalsConstrains.h"
#include "Generators.h"

// the config object
extern wxConfigBase *g_pPrefs;


//-------------------------------------------------------------------------------------------
// implementation of lmTheoIntervalsConstrains
//-------------------------------------------------------------------------------------------

lmTheoIntervalsConstrains::lmTheoIntervalsConstrains(wxString sSection) 
    : lmExerciseOptions(sSection)
{
    LoadSettings();
}

void lmTheoIntervalsConstrains::SaveSettings()
{
    //
    // save settings in user configuration data file
    //

    // allowed clefs
    int i;
    wxString sKey;
    for (i = lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/Clef%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, IsValidClef((lmEClefType)i) );
    }

    // allowed key signatures
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        fValid = m_oValidKeys.IsValid((lmEKeySignatures)i);
        g_pPrefs->Write(sKey, fValid);
    }

    // allowed intervals types
    for (i=0; i < 3; i++) {
        sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/IntervalType%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fTypeAllowed[i]);
    }

    //ledger lines
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/LedgerAbove"),
                            m_sSection.c_str());
    g_pPrefs->Write(sKey, (long)m_nLedgerAbove);
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/LedgerBelow"),
                            m_sSection.c_str());
    g_pPrefs->Write(sKey, (long)m_nLedgerBelow);

    //problem level
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/ProblemLevel"),
                            m_sSection.c_str());
    g_pPrefs->Write(sKey, (long)m_nProblemLevel);

    // allowed accidentals
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/Accidentals"), m_sSection.c_str() );
    g_pPrefs->Write(sKey, m_fAccidentals);
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/DoubleAccidentals"), m_sSection.c_str() );
    g_pPrefs->Write(sKey, m_fDoubleAccidentals);

    // problem type
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/ProblemType"), m_sSection.c_str() );
    g_pPrefs->Write(sKey, (long) m_nProblemType );

}

void lmTheoIntervalsConstrains::LoadSettings()
{
    //
    // load settings form user configuration data or default values
    //

    // allowed clefs. Default G clef
    int i;
    wxString sKey;
    bool fValid;
    for (i = lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/Clef%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &fValid, (i == lmE_Sol) );
        SetClef((lmEClefType)i, fValid);
    }

    // allowed key signatures. Default: C major key signature
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &fValid, (bool)((lmEKeySignatures)i == earmDo) );
        m_oValidKeys.SetValid((lmEKeySignatures)i, fValid);
    }

    // intervals types. Default: melodic
    for (i=0; i < 3; i++) {
        sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/IntervalType%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &m_fTypeAllowed[i], (i != 0));
    }

    //ledger lines. Default: 1
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/LedgerAbove"),
                            m_sSection.c_str());
    m_nLedgerAbove = (int)g_pPrefs->Read(sKey, 1L);
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/LedgerBelow"),
                            m_sSection.c_str());
    m_nLedgerBelow = (int)g_pPrefs->Read(sKey, 1L);

    //problem level. Default: 2 - Also augmented and diminished
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/ProblemLevel"),
                            m_sSection.c_str());
    m_nProblemLevel = (int)g_pPrefs->Read(sKey, 2L);

    // allowed accidentals. Defaul: none
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/Accidentals"), m_sSection.c_str() );
    g_pPrefs->Read(sKey, &m_fAccidentals, false);
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/DoubleAccidentals"), m_sSection.c_str() );
    g_pPrefs->Read(sKey, &m_fDoubleAccidentals, false);

    // problem type
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/ProblemType"), m_sSection.c_str() );
    m_nProblemType = (EProblemTheoIntervals) g_pPrefs->Read(sKey, (long) ePT_Both );


}
