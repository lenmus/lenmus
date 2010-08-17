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

#include "lenmus_id_assigner.h"
#include "lenmus_ldp_elements.h"
#include "lenmus_basic.h"

namespace lenmus
{

IdAssigner::IdAssigner() 
    : m_idCounter(0L) 
{
}

void IdAssigner::reassign_ids(LdpElement* pElm)
{
    LdpTree tree(pElm);
    reassign_ids(&tree);
}

void IdAssigner::reassign_ids(LdpTree* pTree)
{

    long minId = find_min_id(pTree);
    if (minId != -1L)
    {
        long shift = m_idCounter - minId;
        shift_ids(pTree, shift);
    }
}

void IdAssigner::shift_ids(LdpTree* pTree, long shift)
{
    long maxId = 0L;
    LdpTree::depth_first_iterator it;
    for (it = pTree->begin(); it != pTree->end(); ++it)
    {
        if (!(*it)->is_simple())
        {
            long id = (*it)->get_id() + shift;
            maxId = max(maxId, id);
            (*it)->set_id(id);
        }
    }
    m_idCounter = ++maxId;
}

long IdAssigner::find_min_id(LdpTree* pTree)
{
    long minId = -1L;
    bool fFirstId = true;
    LdpTree::depth_first_iterator it;
    for (it = pTree->begin(); it != pTree->end(); ++it)
    {
        if (!(*it)->is_simple())
        {
            if (fFirstId)
            {
                fFirstId = false;
                minId = (*it)->get_id();
            }
            else
                minId = min(minId, (*it)->get_id());
        }
    }
    return minId;
}


}  //namespace lenmus
