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

#include "lenmus_elements.h"
#include "lenmus_core_table.h"
#include "lenmus_internal_model.h"

using namespace std;

namespace lenmus
{


//-------------------------------------------------------------------------------------
// ImBarline implementation
//-------------------------------------------------------------------------------------

ImBarline::ImBarline()
    : ImStaffObj()
    , m_type(kSimple)
{
} 


//-------------------------------------------------------------------------------------
// ImInstrument implementation
//-------------------------------------------------------------------------------------

ImInstrument::ImInstrument()
    : ImObj()
    , m_name("")
    , m_abbrev("")
    , m_midiInstr(0)
    , m_midiChannel(0)
    , m_nStaves(1)
{
//	m_midiChannel = g_pMidi->DefaultVoiceChannel();
//	m_midiInstr = g_pMidi->DefaultVoiceInstr();
} 


//-------------------------------------------------------------------------------------
// ImKeySignature implementation
//-------------------------------------------------------------------------------------

ImKeySignature::ImKeySignature()
    : ImStaffObj()
    , m_type(ImKeySignature::Undefined)
{
} 


//-------------------------------------------------------------------------------------
// ImScore implementation
//-------------------------------------------------------------------------------------

ImScore::ImScore()
    : ImObj()
    , m_version("")
    , m_nInstruments(0)
{
} 

CoreTable* ImScore::get_core_table()
{
    return NULL;
}


}  //namespace lenmus
