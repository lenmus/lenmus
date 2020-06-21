//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#include "lenmus_preferences.h"

#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/filename.h>

//// proxy settings
//lmProxySettings m_rProxySettings;


//
//
////-------------------------------------------------------------------------------------------
//// lmPgmOptions implementation
////-------------------------------------------------------------------------------------------
//
//lmPgmOptions* lmPgmOptions::m_pInstance = (lmPgmOptions*)nullptr;
//
//lmPgmOptions::lmPgmOptions()
//{
//    m_OptData.resize(lm_OP_END);        //allocate all the elements
//}
//
//lmPgmOptions::~lmPgmOptions()
//{
//    //delete the lmOptionData collection
//    std::vector<lmOptionData*>::iterator it;
//    for (it = m_OptData.begin(); it != m_OptData.end(); ++it)
//        delete *it;
//    m_OptData.clear();
//}
//
//lmPgmOptions* lmPgmOptions::GetInstance()
//{
//    if (!m_pInstance)
//    {
//        m_pInstance = LENMUS_NEW lmPgmOptions();
//        m_pInstance->LoadDefaultValues();
//    }
//    return m_pInstance;
//}
//
//void lmPgmOptions::DeleteInstance()
//{
//    if (m_pInstance)
//        delete m_pInstance;
//    m_pInstance = (lmPgmOptions*)nullptr;
//}
//
//
//void lmPgmOptions::Register(lmOptionId nId, long nUnits, float rValue, wxString sKey)
//{
//    lmOptionData* pData = LENMUS_NEW lmOptionData();
//    pData->rValue = rValue;
//    pData->sGroup = sKey;
//    pData->type = nUnits;
//
//    m_OptData[nId] = pData;
//}
//
//void lmPgmOptions::Register(lmOptionId nId, long nUnits, long nValue, wxString sKey)
//{
//    lmOptionData* pData = LENMUS_NEW lmOptionData();
//    pData->nValue = nValue;
//    pData->sGroup = sKey;
//    pData->type = nUnits;
//
//    m_OptData[nId] = pData;
//}
//
//void lmPgmOptions::Register(lmOptionId nId, long nUnits, wxString sValue, wxString sKey)
//{
//    lmOptionData* pData = LENMUS_NEW lmOptionData();
//    pData->sValue = sValue;
//    pData->sGroup = sKey;
//    pData->type = nUnits;
//
//    m_OptData[nId] = pData;
//}
//
//void lmPgmOptions::Register(lmOptionId nId, long nUnits, bool fValue, wxString sKey)
//{
//    lmOptionData* pData = LENMUS_NEW lmOptionData();
//    pData->fValue = fValue;
//    pData->sGroup = sKey;
//    pData->type = nUnits;
//
//    m_OptData[nId] = pData;
//}
//
//void lmPgmOptions::LoadDefaultValues()
//{
//    //Engraving options
//    //------------------------------------------------------------------------
//    //Instruments grouping: space after name/abbreviation
//    Register(lm_EO_GRP_SPACE_AFTER_NAME, lmOP_TENTHS, 10.0f, "InstrGroup/Space after name/");
//    //Instruments grouping: width of bracket
//    Register(lm_EO_GRP_BRACKET_WIDTH, lmOP_TENTHS, 12.5f, "InstrGroup/Width of bracket/");
//    //Instruments grouping: space after bracket
//    Register(lm_EO_GRP_BRACKET_GAP, lmOP_TENTHS, 5.0f, "InstrGroup/Space after bracket/");
//    //Instruments grouping: width of brace
//    Register(lm_EO_GRP_BRACE_WIDTH, lmOP_TENTHS, 6.0f, "InstrGroup/Width of brace/");
//    //Instruments grouping: space after brace
//    Register(lm_EO_GRP_BRACE_GAP, lmOP_TENTHS, 5.0f, "InstrGroup/Space after brace bar/");
//
//    //Beams
//    //thickness of beam line
//    Register(lm_EO_BEAM_THICKNESS, lmOP_TENTHS, 5.0f, "Beam/Thickness of beam line/");
//    //Space between beam lines
//    Register(lm_EO_BEAM_SPACING, lmOP_TENTHS, 3.0f, "Beam/Space between beam lines/");
//
//
//    //Edit options
//    //------------------------------------------------------------------------
//    //what to do when clef added?: 0=ask, 1=keep pitch, 2=keep position
//    Register(lm_DO_CLEF_CHANGE, lmOP_ENUM, 0L, "Clef/Insert behaviour/transpose");
//    //what to do when key added?: 0=ask, 1=keep pitch, 2=do nothing
//    Register(lm_DO_KS_CHANGE, lmOP_ENUM, 0L, "Key/Insert behaviour/transpose");
//    //AutoBar: automatically insert a barline when a measure is full
//    Register(lm_DO_AUTOBAR, lmOP_BOOL, true, "Barline/Insert barlines automatically");
//
//}
