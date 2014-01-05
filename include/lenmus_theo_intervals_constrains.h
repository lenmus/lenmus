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

#ifndef __LENMUS_THEO_INTERVALS_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_THEO_INTERVALS_CONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_constrains.h"

namespace lenmus
{


class TheoIntervalsConstrains : public ExerciseOptions
{
protected:
    ClefConstrains  m_oClefs;       //allowed clefs
    KeyConstrains   m_oValidKeys;   //allowed key signatures
    int     m_nProblemType;
    int     m_nLedgerAbove;         //ledger lines above
    int     m_nLedgerBelow;         //ledger lines below
    int     m_nProblemLevel;        //Problem level:
                                        //  0-Just name the interval number
                                        //  1-Perfect, major and minor intervals
                                        //  2-Also augmented and diminished
                                        //  3-Also double augmented / diminished
    bool    m_fAccidentals;         //allow accidentals
    bool    m_fDoubleAccidentals;   //allow double accidentals
    bool    m_fTypeAllowed[3];      //intervals. Allowed types:
                                        //  0-harmonic
                                        //  1-melodic ascending
                                        //  2-melodic descending

public:
    TheoIntervalsConstrains(const wxString& sSection, ApplicationScope& appScope);
    ~TheoIntervalsConstrains() {}

    enum {
        k_deduce_interval = 0,      //WARNING: the enum values are used as indexes
        k_build_interval,           // in DlgCfgTheoIntervals. Do not alter
        k_both                      // neither order nor values. Must start in 0
    };

    inline bool IsValidClef(EClef nClef) { return m_oClefs.IsValid(nClef); }
    inline void SetClef(EClef nClef, bool fValid) { m_oClefs.SetValid(nClef, fValid); }

    inline int GetProblemType() { return m_nProblemType; }
    inline void SetProblemType(int nType) { m_nProblemType = nType; }

    inline bool IsTypeAllowed(int nType) { return m_fTypeAllowed[nType]; }
    inline void SetTypeAllowed(int nType, bool fValue) { m_fTypeAllowed[nType] = fValue; }

    inline bool get_accidentals() { return m_fAccidentals; }
    inline void SetAccidentals(bool fValue) { m_fAccidentals = fValue; }

    inline bool GetDoubleAccidentals() { return m_fDoubleAccidentals; }
    inline void SetDoubleAccidentals(bool fValue) { m_fDoubleAccidentals = fValue; }

    inline ClefConstrains* GetClefConstrains() { return &m_oClefs; }
    inline KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }

    inline int GetLedgerLinesAbove() { return m_nLedgerAbove; }
    inline int GetLedgerLinesBelow() { return m_nLedgerBelow; }
    inline void SetLedgerLinesAbove(int nLines) { m_nLedgerAbove = nLines; }
    inline void SetLedgerLinesBelow(int nLines) { m_nLedgerBelow = nLines; }

    inline int GetProblemLevel() { return m_nProblemLevel; }
    inline void SetProblemLevel(int nLevel) { m_nProblemLevel = nLevel; }

    inline void set_section(const string& sSection) {
                m_sSection = to_wx_string(sSection);
                load_settings();
            }

    void save_settings();

private:
    void load_settings();

};


}   // namespace lenmus

#endif  // __LENMUS_THEO_INTERVALS_CONSTRAINS_H__
