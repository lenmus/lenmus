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

#ifndef __LML_ID_ASSIGNER_H__ 
#define __LML_ID_ASSIGNER_H__

#include "lenmus_ldp_elements.h"

namespace lenmus
{

//IdAssigner definition
class IdAssigner
{
protected:
    long m_idCounter;

public:
    IdAssigner();
    ~IdAssigner() {}

    void reassign_ids(LdpElement* pElm);
    void reassign_ids(LdpTree* pTree);
    inline void set_last_id(long id) { m_idCounter = id + 1L; }
    inline long get_last_id() { return m_idCounter - 1L; }

protected:
    long find_min_id(LdpTree* pTree);
    void shift_ids(LdpTree* pTree, long shift);

};


} //namespace lenmus

#endif    //__LML_ID_ASSIGNER_H__
