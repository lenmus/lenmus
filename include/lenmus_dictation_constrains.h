//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#ifndef __LENMUS_DICTATION_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_DICTATION_CONSTRAINS_H__

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

//forward declarations
class ScoreConstrains;

//---------------------------------------------------------------------------------------
class DictationConstrains : public ExerciseOptions
{
protected:
    ScoreConstrains* m_pScoreConstrains;        //for Composer

//    KeyConstrains   m_oValidKeys;           //allowed key signatures
//    bool            m_fDisplayKey;

public:
    DictationConstrains(wxString sSection, ApplicationScope& appScope);
    virtual ~DictationConstrains();

    //required overrides
    void save_settings();

//    //specific
//    void SetDisplayKey(bool fValue) { m_fDisplayKey = fValue; }
//    bool DisplayKey() { return m_fDisplayKey; }
//
//    void SetSection(wxString sSection) {
//                m_sSection = sSection;
//                load_settings();
//            }

    inline ScoreConstrains* get_score_constrains() { return m_pScoreConstrains; }
    inline void set_score_constrains(ScoreConstrains* pConstrains) {
        m_pScoreConstrains = pConstrains;
    }

//    KeyConstrains* get_key_constrains() { return &m_oValidKeys; }


private:
    void load_settings();
};


}   //namespace lenmus

#endif  // __LENMUS_DICTATION_CONSTRAINS_H__
