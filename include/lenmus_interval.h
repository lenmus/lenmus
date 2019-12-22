//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2019 LenMus project
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

#ifndef __LENMUS_INTERVAL_H__        //to avoid nested includes
#define __LENMUS_INTERVAL_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lenmus
#include "lenmus_standard_header.h"

//lomse
#include <lomse_pitch.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_interval.h>
using namespace lomse;


namespace lenmus
{


extern wxString get_FIntval_name(FIntval intv);     //i.e. "Minor 2nd", "Perfect 4th", Major 3rd"


//---------------------------------------------------------------------------------------
// Interval class is a totally defined interval: two notes
// It is oriented to aural training exercises, that require special constrains.
class Interval
{
protected:
    FPitch m_fp[2];
    EKeySignature m_nKey;

public:
    Interval(bool fDiatonic, DiatonicPitch dpMin, DiatonicPitch dpMax,
             bool fAllowedIntervals[], bool fAscending,
             EKeySignature nKey, FPitch fpStartNote=k_undefined_fpitch);

    ~Interval() {};

    int get_num_semitones();
    FIntval get_interval() { return FIntval( abs((int)m_fp[0] - (int)m_fp[1]) ); }
    inline FPitch get_pitch(int iNote) { return m_fp[iNote]; }
    wxString get_interval_name();

};


}   //namespace lenmus

#endif  // __LENMUS_INTERVAL_H__
