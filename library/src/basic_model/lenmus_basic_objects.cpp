//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#include "lenmus_basic_objects.h"

#include <algorithm>
#include "lenmus_internal_model.h"

using namespace std;

namespace lenmus
{


//-------------------------------------------------------------------------------------
// DtoComponentObj implementation
//-------------------------------------------------------------------------------------

void DtoComponentObj::set_color(rgba16 color)
{
    m_color = color;
}


//-------------------------------------------------------------------------------------
// DtoBarline implementation
//-------------------------------------------------------------------------------------

DtoBarline::DtoBarline(int barlineType)
    : DtoStaffObj()
    , m_barlineType(barlineType)
{
}

//-------------------------------------------------------------------------------------
// DtoFermata implementation
//-------------------------------------------------------------------------------------

DtoFermata::DtoFermata() 
    : DtoAuxObj()
    , m_placement(ImoFermata::k_above)
    , m_symbol(ImoFermata::k_normal) 
{
}

//-------------------------------------------------------------------------------------
// DtoMetronomeMark implementation
//-------------------------------------------------------------------------------------

DtoMetronomeMark::DtoMetronomeMark()
    : DtoStaffObj()
    , m_markType(ImoMetronomeMark::k_value)
    , m_ticksPerMinute(60)
    , m_leftNoteType(0)
    , m_leftDots(0)
    , m_rightNoteType(0)
    , m_rightDots(0)
    , m_fParenthesis(false)
{
}


//-------------------------------------------------------------------------------------
// DtoNoteRest implementation
//-------------------------------------------------------------------------------------

DtoNoteRest::DtoNoteRest()
    : DtoStaffObj()
    , m_nDots(0)
    , m_nVoice(1)
{
}

void DtoNoteRest::set_note_type_and_dots(int noteType, int dots)
{
    m_nNoteType = noteType;
    m_nDots = dots;
}


//-------------------------------------------------------------------------------------
// DtoNote implementation
//-------------------------------------------------------------------------------------

DtoNote::DtoNote()
    : DtoNoteRest()
    , m_step(0)
    , m_octave(4)
    , m_accidentals(0)
    , m_stemDirection(0)
    , m_inChord(false)
{
}


}  //namespace lenmus
