//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "Preferences.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Preferences.h"

// to save config information into a file
#include "wx/confbase.h"
#include "wx/fileconf.h"
#include "wx/filename.h"



wxConfigBase* g_pPrefs = (wxConfigBase*) NULL;

// global values
bool g_fAnswerSoundsEnabled;    // Feedback right/wrong answer sounds are enabled
bool g_fTeamCounters;           // Exercises will have two counters to play in teams
bool g_fAutoNewProblem;         // If right answer, don't show solution and generate a new problem
bool g_fAutoBeam;               // Assign beam structure automatically


// proxy settings
lmProxySettings m_rProxySettings;



void InitPreferences()
{
    g_pPrefs = wxConfigBase::Get();

    g_pPrefs->Read(_T("/Options/EnableAnswerSounds"), &g_fAnswerSoundsEnabled, true);
    g_pPrefs->Read(_T("/Options/TeamCounters"), &g_fTeamCounters, false);
    g_pPrefs->Read(_T("/Options/AutoNewProblem"), &g_fAutoNewProblem, true);
    g_pPrefs->Read(_T("/Options/AutoBeam"), &g_fAutoBeam, true);
}

lmProxySettings* GetProxySettings()
{
    bool fUseProxy;
    g_pPrefs->Read(_T("/Internet/UseProxy"), &fUseProxy, false);
    m_rProxySettings.fUseProxy = fUseProxy;

    m_rProxySettings.sProxyHostname = g_pPrefs->Read(_T("/Internet/Hostname"), _T(""));
    long nPort = 0;
    wxString sPort = g_pPrefs->Read(_T("/Internet/PortNumber"), _T(""));
    if (sPort.IsNumber())
        sPort.ToLong(&nPort);
    m_rProxySettings.nProxyPort = nPort;

    bool fAuthentication;
    g_pPrefs->Read(_T("/Internet/ProxyAuthentication"), &fAuthentication, false);
    m_rProxySettings.fRequiresAuth = fAuthentication;
    m_rProxySettings.sProxyUsername = g_pPrefs->Read(_T("/Internet/Username"), _T(""));
    m_rProxySettings.sProxyPassword = g_pPrefs->Read(_T("/Internet/Password"), _T(""));

    return &m_rProxySettings;
}


//-------------------------------------------------------------------------------------------
// lmPgmOptions implementation
//-------------------------------------------------------------------------------------------

lmPgmOptions* lmPgmOptions::m_pInstance = (lmPgmOptions*)NULL;

lmPgmOptions::lmPgmOptions()
{
    m_OptData.resize(lm_OP_END);        //allocate all the elements
}

lmPgmOptions::~lmPgmOptions()
{
    //delete the lmOptionData collection
    std::vector<lmOptionData*>::iterator it;
    for (it = m_OptData.begin(); it != m_OptData.end(); ++it)
        delete *it;
    m_OptData.clear();
}

lmPgmOptions* lmPgmOptions::GetInstance()
{
    if (!m_pInstance)
    {
        m_pInstance = new lmPgmOptions();
        m_pInstance->LoadDefaultValues();
    }
    return m_pInstance;
}

void lmPgmOptions::DeleteInstance()
{
    if (m_pInstance)
        delete m_pInstance;
    m_pInstance = (lmPgmOptions*)NULL;
}


void lmPgmOptions::Register(lmOptionId nId, long nUnits, float rValue, wxString sKey)
{
    lmOptionData* pData = new lmOptionData();
    pData->rValue = rValue;
    pData->sGroup = sKey;
    pData->type = nUnits;

    m_OptData[nId] = pData;
}

void lmPgmOptions::Register(lmOptionId nId, long nUnits, long nValue, wxString sKey)
{
    lmOptionData* pData = new lmOptionData();
    pData->nValue = nValue;
    pData->sGroup = sKey;
    pData->type = nUnits;

    m_OptData[nId] = pData;
}

void lmPgmOptions::Register(lmOptionId nId, long nUnits, wxString sValue, wxString sKey)
{
    lmOptionData* pData = new lmOptionData();
    pData->sValue = sValue;
    pData->sGroup = sKey;
    pData->type = nUnits;

    m_OptData[nId] = pData;
}

void lmPgmOptions::Register(lmOptionId nId, long nUnits, bool fValue, wxString sKey)
{
    lmOptionData* pData = new lmOptionData();
    pData->fValue = fValue;
    pData->sGroup = sKey;
    pData->type = nUnits;

    m_OptData[nId] = pData;
}

void lmPgmOptions::LoadDefaultValues()
{
    //Engraving options
    //------------------------------------------------------------------------
    //Instruments grouping: space after name/abbreviation
    Register(lm_EO_GRP_SPACE_AFTER_NAME, lmOP_TENTHS, 10.0f, _T("InstrGroup/Space after name/"));
    //Instruments grouping: width of bracket
    Register(lm_EO_GRP_BRACKET_WIDTH, lmOP_TENTHS, 12.5f, _T("InstrGroup/Width of bracket/"));
    //Instruments grouping: space after bracket
    Register(lm_EO_GRP_BRACKET_GAP, lmOP_TENTHS, 5.0f, _T("InstrGroup/Space after bracket/"));
    //Instruments grouping: width of brace
    Register(lm_EO_GRP_BRACE_WIDTH, lmOP_TENTHS, 6.0f, _T("InstrGroup/Width of brace/"));
    //Instruments grouping: space after brace
    Register(lm_EO_GRP_BRACE_GAP, lmOP_TENTHS, 5.0f, _T("InstrGroup/Space after brace bar/"));

    //Beams
    //thickness of beam line
    Register(lm_EO_BEAM_THICKNESS, lmOP_TENTHS, 5.0f, _T("Beam/Thickness of beam line/"));
    //Space between beam lines
    Register(lm_EO_BEAM_SPACING, lmOP_TENTHS, 3.0f, _T("Beam/Space between beam lines/"));


    //Edit options
    //------------------------------------------------------------------------
    //what to do when clef added?: 0=ask, 1=keep pitch, 2=keep position
    Register(lm_DO_CLEF_CHANGE, lmOP_ENUM, 0L, _T("Clef/Insert behaviour/transpose"));
    //what to do when key added?: 0=ask, 1=keep pitch, 2=do nothing
    Register(lm_DO_KS_CHANGE, lmOP_ENUM, 0L, _T("Key/Insert behaviour/transpose"));
    //AutoBar: automatically insert a barline when a measure is full
    Register(lm_DO_AUTOBAR, lmOP_BOOL, true, _T("Barline/Insert barlines automatically"));

}
