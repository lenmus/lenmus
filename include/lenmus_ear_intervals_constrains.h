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

#ifndef __LENMUS_EARINTERVALSCONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_EARINTERVALSCONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_constrains.h"
#include "lenmus_injectors.h"

//lomse
#include "lomse_pitch.h"
using namespace lomse;

namespace lenmus
{

//---------------------------------------------------------------------------------------
// EarIntervalsConstrains: options for EarIntervalsCtrol control
class EarIntervalsConstrains : public ExerciseOptions
{
public:
    EarIntervalsConstrains(wxString sSection, ApplicationScope& appScope);
    ~EarIntervalsConstrains() {}

    bool IsIntervalAllowed(int nInterval) { return m_fIntervalAllowed[nInterval]; }
    void SetIntervalAllowed(int nInterval, bool fValue) {
            m_fIntervalAllowed[nInterval] = fValue;
        }
    bool IsTypeAllowed(int nType) { return m_fTypeAllowed[nType]; }
    void SetTypeAllowed(int nType, bool fValue) {
            m_fTypeAllowed[nType] = fValue;
        }
    bool OnlyNatural() { return m_fOnlyNatural; }
    void SetOnlyNatural(bool fValue) { m_fOnlyNatural = fValue; }

    bool FirstNoteEqual() { return m_fFirstEqual; }
    void SetFirstNoteEqual(bool fValue) { m_fFirstEqual = fValue; }


    KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }

    bool* AllowedIntervals() { return m_fIntervalAllowed; }
    void SetMinNote(DiatonicPitch nPitch) { m_nMinPitch = nPitch; }
    DiatonicPitch MinNote() { return m_nMinPitch; }
    void SetMaxNote(DiatonicPitch nPitch) { m_nMaxPitch = nPitch; }
    DiatonicPitch MaxNote() { return m_nMaxPitch; }

    void save_settings();


private:
    void load_settings();


    wxString    m_sSection;         // section name to save the constraints

    bool    m_fIntervalAllowed[lmNUM_INTVALS];     //interval n allowed (0..24)
    int     m_nMinPitch;                // interval allowed range: min. diatonic pitch
    int     m_nMaxPitch;                    //      max diatonic pitch
    bool    m_fTypeAllowed[3];          // intervals: allowed types:
                                            //      0-harmonic
                                            //      1-melodic ascending
                                            //      2-melodic descending
    bool                m_fOnlyNatural; //use only natural intervals of the scale
    KeyConstrains     m_oValidKeys;   //allowed key signatures

    //for interval comparison exercises
    bool    m_fFirstEqual;     // first note equal in both intervals


};


}   // namespace lenmus

#endif  // __LENMUS_EARINTERVALSCONSTRAINS_H__
