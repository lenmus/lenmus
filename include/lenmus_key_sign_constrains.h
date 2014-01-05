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

#ifndef __LENMUS_KEY_SIGN_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_KEY_SIGN_CONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_constrains.h"


namespace lenmus
{

//---------------------------------------------------------------------------------------
enum EProblemTheoKeySig
{
    eIdentifyKeySignature = 0,
    eWriteKeySignature,
    eBothKeySignProblems
};

//---------------------------------------------------------------------------------------
enum EScaleMode
{
    k_scale_major = 0,
    k_scale_minor,
    k_scale_both
};


//---------------------------------------------------------------------------------------
// TheoKeySignConstrains: Options for TheoKeySignCtrol control
class TheoKeySignConstrains : public ExerciseOptions
{
public:
    TheoKeySignConstrains(wxString sSection, ApplicationScope& appScope);
    ~TheoKeySignConstrains() {}

    EProblemTheoKeySig GetProblemType() { return m_nProblemType; }
    void SetProblemType(EProblemTheoKeySig nType) { m_nProblemType = nType; }

    int GetMaxAccidentals() { return m_nMaxAccidentals; }
    void SetMaxAccidentals(int nNum) { m_nMaxAccidentals = nNum; }

    EScaleMode GetScaleMode() { return m_nMode; }
    void SetScaleMode(EScaleMode nMode) { m_nMode = nMode; }

    bool IsValidClef(EClef nClef) { return m_oClefs.IsValid(nClef); }
    void SetClef(EClef nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }
    ClefConstrains* GetClefConstrains() { return &m_oClefs; }

    KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }

    void save_settings() {}


private:
    void load_settings() {}

    EProblemTheoKeySig  m_nProblemType;
    int                 m_nMaxAccidentals;
    EScaleMode          m_nMode;
    KeyConstrains       m_oValidKeys;           //allowed key signatures
    ClefConstrains    m_oClefs;               //allowed clefs

};


}   // namespace lenmus

#endif  // __LENMUS_KEY_SIGN_CONSTRAINS_H__
