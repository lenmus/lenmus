//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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
//---------------------------------------------------------------------------------------

//lenmus
#include "lenmus_constrains.h"
#include "lenmus_generators.h"


namespace lenmus
{

//global function to return translated string
const wxString& get_generation_mode_name(long nMode)
{
    wxASSERT(nMode >= 0 && nMode < k_num_generation_modes);

    static wxString sNames[k_num_generation_modes];
    static bool fTranslated = false;

    if (!fTranslated)
    {
        //AWARE: In correspondence with enum
        sNames[0] = _("Learning");
        sNames[1] = _("Practise");
        sNames[2] = _("Exam");
        sNames[3] = _("Quiz");
        fTranslated = true;
    }

    return sNames[nMode];
}


//=======================================================================================
// ClefConstrains
//=======================================================================================
ClefConstrains::ClefConstrains()
{
    for (int i = lmMIN_CLEF; i <= lmMAX_CLEF; i++)
    {
        m_fValidClefs[i-lmMIN_CLEF] = false;
        m_aLowerPitch[i-lmMIN_CLEF] = _T("c0");
        m_aUpperPitch[i-lmMIN_CLEF] = _T("c9");
    }
}


//=======================================================================================
// KeyConstrains
//=======================================================================================
KeyConstrains::KeyConstrains()
{
    for (int i = k_min_key; i <= k_max_key; i++)
    {
        m_fValidKeys[i-k_min_key] = false;
    }
}

EKeySignature KeyConstrains::GetRandomKeySignature()
{
    RandomGenerator oGenerator;
    int nWatchDog = 0;
    EKeySignature nType =
        static_cast<EKeySignature>( oGenerator.random_number(k_min_key, k_max_key) );
    while (!IsValid(nType))
    {
        nType =
            static_cast<EKeySignature>( oGenerator.random_number(k_min_key, k_max_key) );
        if (nWatchDog++ == 1000)
        {
            wxLogMessage(_T("Program error: Loop detected in KeyConstrains::GetRandomKeySignature."));
            return k_min_key;
        }
    }
    return nType;
}

//=======================================================================================
// TimeSignConstrains
//=======================================================================================

TimeSignConstrains::TimeSignConstrains()
{
    for (int i = k_min_time_signature; i <= k_max_time_signature; i++) {
        m_fValidTimes[i-k_min_time_signature] = false;
    }
}

/*! Initialize constraints from a comma separated list of values.
    i.e.: '24,34,44' (no spaces allowed)
    Returns true if error
*/
bool TimeSignConstrains::SetConstrains(wxString sTimeSign)
{
    int i;
    for (i = k_min_time_signature; i <= k_max_time_signature; i++) {
        m_fValidTimes[i-k_min_time_signature] = false;
    }

    wxString sData;
    int nTimeSign;

    //split the list into values
    i = sTimeSign.find(_T(","));
    sData = ((i > 0) ? sTimeSign.Left(i) : sTimeSign);
    while (sData != _T("")) {
        // 24,34,44,68,98,128,28,38,22,32
        if (sData == _T("24"))          nTimeSign = (int)k_time_2_4;
        else if (sData == _T("34"))     nTimeSign = (int)k_time_3_4;
        else if (sData == _T("44"))     nTimeSign = (int)k_time_4_4;
        else if (sData == _T("68"))     nTimeSign = (int)k_time_6_8;
        else if (sData == _T("98"))     nTimeSign = (int)k_time_9_8;
        else if (sData == _T("128"))    nTimeSign = (int)k_time_12_8;
        else if (sData == _T("28"))     nTimeSign = (int)k_time_2_8;
        else if (sData == _T("38"))     nTimeSign = (int)k_time_3_8;
        else if (sData == _T("22"))     nTimeSign = (int)k_time_2_2;
        else if (sData == _T("32"))     nTimeSign = (int)k_time_3_2;
        else {
            return true;
        }
        m_fValidTimes[nTimeSign-k_min_time_signature] = true;
        sTimeSign = ((i > 0) ? sTimeSign.substr(i+1) : _T(""));
        i = sTimeSign.find(_T(","));
        sData = ((i > 0) ? sTimeSign.Left(i) : sTimeSign);
    }
    return false;

}



//=======================================================================================
// Implementation of abstract class EBookCtrolOptions
//=======================================================================================
EBookCtrolOptions::EBookCtrolOptions(const wxString& sSection, ApplicationScope& appScope)
    : m_appScope(appScope)
    , m_sSection(sSection)
    , m_sGoBackURL("")
    , m_fPlayLink(true)
    , m_fSettingsLink(false)
    , m_width(0.0f)
    , m_height(0.0f)
{
}

//---------------------------------------------------------------------------------------
void EBookCtrolOptions::set_section(const string& sSection)
{
    m_sSection = to_wx_string(sSection);
    load_settings();
}


//=======================================================================================
// Implementation of abstract class ExerciseOptions
//=======================================================================================
ExerciseOptions::ExerciseOptions(const wxString& sSection, ApplicationScope& appScope)
    : EBookCtrolOptions(sSection, appScope)
{
    m_fButtonsEnabledAfterSolution = true;
    m_fSolutionLink = true;
    m_fUseCounters = true;
	m_fTheoryMode = true;
    m_nGenerationMode = k_exam_mode;

    //Default suppoted generation/evaluation modes: exam & quiz
    for (long i=0; i < k_num_generation_modes; i++)
        m_fSupportedMode[i] = false;
    m_fSupportedMode[k_exam_mode] = true;
    m_fSupportedMode[k_quiz_mode] = true;
}



//=======================================================================================
// Implementation of ScoreCtrolOptions
//=======================================================================================
ScoreCtrolOptions::ScoreCtrolOptions(const wxString& sSection, ApplicationScope& appScope)
    : EBookCtrolOptions(sSection, appScope)
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
    rTopMargin = 0.0;       //millimeters
}

//---------------------------------------------------------------------------------------
void ScoreCtrolOptions::SetLabels(wxString& sLabel, wxString* pStart, wxString* pStop)
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


}   //namespace lenmus
