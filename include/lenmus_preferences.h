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

#ifndef __LENMUS_PREFERENCES_H__
#define __LENMUS_PREFERENCES_H__

//#include "lenmus_standard_header.h"
//#include <vector>
//#include <wx/config.h>
//
//
//// the config object
//extern wxConfigBase *pPrefs;
//
////program: user options
//extern bool g_fAutoBeam;
//
//
//
//
////define data type to contain option info and  value
//typedef struct lmOptionDataStruct
//{
//    long        id;
//    long        type;           //long, float, bool, ...
//    wxString    sValue;
//    long        nValue;
//    float       rValue;
//    bool        fValue;
//    wxString    sGroup;
//    wxString    sSubgroup;
//    wxString    sName;
//
//} lmOptionData;
//
////to define type of units
//enum {
//    lmOP_TENTHS = 0,    //tenths
//    lmOP_ENUM,          //enumeration
//    lmOP_BOOL,          //boolean
//};
//
////Add a value for each option. Must be consecutive, and are used as indexes to access a vector.
//enum lmOptionId
//{
//    //Engraving options
//    //------------------------------------------------------
//    lm_EO_FIRST_ENGRAVING = 0,              //marker
//    lm_EO_GRP_SPACE_AFTER_NAME,
//    lm_EO_GRP_BRACKET_WIDTH,
//    lm_EO_GRP_BRACKET_GAP,
//    lm_EO_GRP_BRACE_WIDTH,
//    lm_EO_GRP_BRACE_GAP,
//    //Beams
//    lm_EO_BEAM_THICKNESS,
//    lm_EO_BEAM_SPACING,
//    //
//    lm_EO_LAST_ENGRAVING,
//
//    //Edit options
//    //------------------------------------------------------
//    lm_DO_FIRST_EDIT = lm_EO_LAST_ENGRAVING,    //marker
//    //
//    lm_DO_CLEF_CHANGE,              //what to do when clef changed?: 0=ask, 1=keep pitch, 2=keep position
//    lm_DO_KS_CHANGE,                //what to do when key changed?: 0=ask, 1=keep pitch, 2=add accidentals
//    lm_DO_AUTOBAR,                  //AutoBar: automatically insert a barline when a measure is full
//    //
//    lm_DO_LAST_EDIT,
//
//    //End of lit
//    lm_OP_END
//};
//
//
//
////--------------------------------------------------------------------------------------------
//// Program options
////--------------------------------------------------------------------------------------------
//
//class lmPgmOptions
//{
//public:
//    ~lmPgmOptions();
//
//    static lmPgmOptions* GetInstance();
//    static void DeleteInstance();
//
//    //creation / load / save
//    void LoadDefaultValues();
//
//    //access
//    inline float GetFloatValue(lmOptionId nId) { return m_OptData[nId]->rValue; }
//    inline long GetLongValue(lmOptionId nId) { return m_OptData[nId]->nValue; }
//    inline wxString GetStringValue(lmOptionId nId) { return m_OptData[nId]->sValue; }
//    inline bool GetBoolValue(lmOptionId nId) { return m_OptData[nId]->fValue; }
//
//
//protected:
//    lmPgmOptions();
//
//private:
//    void Register(lmOptionId nId, long nUnits, float rValue, wxString sKey);
//    void Register(lmOptionId nId, long nUnits, long nValue, wxString sKey);
//    void Register(lmOptionId nId, long nUnits, wxString sValue, wxString sKey);
//    void Register(lmOptionId nId, long nUnits, bool fValue, wxString sKey);
//
//    static lmPgmOptions*  m_pInstance;    //the only instance of this class
//
//    std::vector<lmOptionData*>      m_OptData;
//
//};


#endif    // __LENMUS_PREFERENCES_H__

