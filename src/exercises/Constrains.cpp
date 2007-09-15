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
#pragma implementation "Constrains.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Constrains.h"

// the config object
extern wxConfigBase *g_pPrefs;



//-------------------------------------------------------------------------------------------
// lmClefConstrain
//-------------------------------------------------------------------------------------------

lmClefConstrain::lmClefConstrain()
{
    for (int i = lmMIN_CLEF; i <= lmMAX_CLEF; i++) {
        m_fValidClefs[i-lmMIN_CLEF] = false;
        m_aLowerPitch[i-lmMIN_CLEF] = _T("c0");
        m_aUpperPitch[i-lmMIN_CLEF] = _T("c9");
    }
}


//-------------------------------------------------------------------------------------------
// lmKeyConstrains
//-------------------------------------------------------------------------------------------

lmKeyConstrains::lmKeyConstrains()
{
    for (int i = lmMIN_KEY; i <= lmMAX_KEY; i++) {
        m_fValidKeys[i-lmMIN_KEY] = false;
    }
}


//-------------------------------------------------------------------------------------------
// lmTimeSignConstrains
//-------------------------------------------------------------------------------------------

lmTimeSignConstrains::lmTimeSignConstrains()
{
    for (int i = lmMIN_TIME_SIGN; i <= lmMAX_TIME_SIGN; i++) {
        m_fValidTimes[i-lmMIN_TIME_SIGN] = false;
    }
}

/*! Initialize constraints from a comma separated list of values.
    i.e.: '24,34,44' (no spaces allowed)
    Returns true if error
*/
bool lmTimeSignConstrains::SetConstrains(wxString sTimeSign)
{
    int i;
    for (i = lmMIN_TIME_SIGN; i <= lmMAX_TIME_SIGN; i++) {
        m_fValidTimes[i-lmMIN_TIME_SIGN] = false;
    }

    wxString sData;
    int nTimeSign;

    //split the list into values
    i = sTimeSign.find(_T(","));
    sData = ((i > 0) ? sTimeSign.Left(i) : sTimeSign);
    while (sData != _T("")) {
        // 24,34,44,68,98,128,28,38,22,32
        if (sData == _T("24"))          nTimeSign = (int)emtr24;
        else if (sData == _T("34"))     nTimeSign = (int)emtr34;
        else if (sData == _T("44"))     nTimeSign = (int)emtr44;
        else if (sData == _T("68"))     nTimeSign = (int)emtr68;
        else if (sData == _T("98"))     nTimeSign = (int)emtr98;
        else if (sData == _T("128"))    nTimeSign = (int)emtr128;
        else if (sData == _T("28"))     nTimeSign = (int)emtr28;
        else if (sData == _T("38"))     nTimeSign = (int)emtr38;
        else if (sData == _T("22"))     nTimeSign = (int)emtr22;
        else if (sData == _T("32"))     nTimeSign = (int)emtr32;
        else {
            return true;
        }
        m_fValidTimes[nTimeSign-lmMIN_TIME_SIGN] = true;
        sTimeSign = ((i > 0) ? sTimeSign.substr(i+1) : _T(""));
        i = sTimeSign.find(_T(","));
        sData = ((i > 0) ? sTimeSign.Left(i) : sTimeSign);
    }
    return false;

}



//--------------------------------------------------------------------------------
// Implementation of abstract class lmEBookCtrolOptions
//--------------------------------------------------------------------------------
lmEBookCtrolOptions::lmEBookCtrolOptions(wxString sSection)
{
    m_sSection = sSection;
    m_fSettingsLink = false;
    m_sGoBackURL = _T("");
    m_fPlayLink = true;
    LoadSettings();
}


//--------------------------------------------------------------------------------
// Implementation of abstract class lmExerciseOptions
//--------------------------------------------------------------------------------
lmExerciseOptions::lmExerciseOptions(wxString sSection)
    : lmEBookCtrolOptions(sSection)
{
    m_fButtonsEnabledAfterSolution = true;
    m_fSolutionLink = true;
    m_fUseCounters = true;
}



//-------------------------------------------------------------------------------------------
// Implementation of lmScoreCtrolOptions
//-------------------------------------------------------------------------------------------

lmScoreCtrolOptions::lmScoreCtrolOptions(wxString sSection)
    : lmEBookCtrolOptions(sSection)
{
    //default values
    fPlayCtrol = false;
    fSolfaCtrol = false;
    fMeasuresCtrol = false;
    fBorder = false;
    fMusicBorder = false;
    sPlayLabel = _("Play");
    sStopPlayLabel = _("Stop");
    sSolfaLabel = _("Read");
    sStopSolfaLabel = _("Stop");
    sMeasuresLabel = _("Measure %d");
    sStopMeasureLabel = _("Stop %d");
    rScale = 1.0;
    m_nMM = 0;
}

void lmScoreCtrolOptions::SetLabels(wxString& sLabel, wxString* pStart, wxString* pStop)
{
    //find the bar
    int i = sLabel.Find(_T("|"));
    if (i != -1) {
        if (i > 1) *pStart = sLabel.substr(0, i-1);
        if (i < (int)sLabel.length()-1) *pStop = sLabel.substr(i+1);
    }
    else {
         *pStart = sLabel;
    }

}

