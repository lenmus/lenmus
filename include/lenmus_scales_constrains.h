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

#ifndef __LENMUS_SCALES_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_SCALES_CONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_generators.h"
#include "lenmus_constrains.h"
#include "lenmus_scale.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
enum EScaleGroup
{
    k_scale_group_major = 0,
    k_scale_group_minor,
    k_scale_group_gregorian,
    k_scale_group_other,
    //last element, to signal End Of Table
    k_scale_group_max
};


//---------------------------------------------------------------------------------------
class ScalesConstrains : public ExerciseOptions
{
public:
    ScalesConstrains(wxString sSection, ApplicationScope& appScope);
    ~ScalesConstrains() {}

    EScaleType GetRandomScaleType();
    bool GetRandomPlayMode();

    bool IsScaleValid(EScaleType nType) { return m_fValidScales[nType]; }
    void SetScaleValid(EScaleType nType, bool fValid) { m_fValidScales[nType] = fValid; }
    bool* GetValidScales() { return m_fValidScales; }

    bool IsValidGroup(EScaleGroup nGroup);

    int GetPlayMode() { return m_nPlayMode; }
    void SetPlayMode(int nPlayMode) { m_nPlayMode = nPlayMode; }

    void SetDisplayKey(bool fValue) { m_fDisplayKey = fValue; }
    bool DisplayKey() { return m_fDisplayKey; }

    void SetSection(wxString sSection) {
                m_sSection = sSection;
                load_settings();
            }

    void save_settings();

    KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }


private:
    void load_settings();

    KeyConstrains   m_oValidKeys;           //allowed key signatures
    bool            m_fValidScales[est_Max];
    bool            m_fDisplayKey;
    int             m_nPlayMode;            // 0-ascending
                                            // 1-descending
                                            // 2-both
};


}   //namespace lenmus

#endif  // __LENMUS_SCALES_CONSTRAINS_H__
