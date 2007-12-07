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

    // allowed accidentals. Defaul: none
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/Accidentals"), m_sSection.c_str() );
    g_pPrefs->Read(sKey, &m_fAccidentals, false);
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/DoubleAccidentals"), m_sSection.c_str() );
    g_pPrefs->Read(sKey, &m_fDoubleAccidentals, false);

    // problem type
    sKey = wxString::Format(_T("/Constrains/TheoIntval/%s/ProblemType"), m_sSection.c_str() );
    m_nProblemType = (EProblemTheoIntervals) g_pPrefs->Read(sKey, (long) ePT_Both );


}
