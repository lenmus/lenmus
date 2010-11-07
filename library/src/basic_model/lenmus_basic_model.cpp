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

#include "lenmus_basic_model.h"

#include "lenmus_internal_model.h"


namespace lenmus
{


//-------------------------------------------------------------------------------------
// BasicModel implementation
//-------------------------------------------------------------------------------------

BasicModel::BasicModel()
    : m_pRoot(NULL)
{
}

BasicModel::~BasicModel()
{
    if (m_pRoot)
        delete m_pRoot;
    delete_beams();
    delete_tuplets();
}

void BasicModel::delete_beams()
{
    std::list<ImoBeam*>::iterator it;
    for (it = m_beams.begin(); it != m_beams.end(); ++it)
        delete *it;
    m_beams.clear();
}

void BasicModel::delete_tuplets()
{
    std::list<ImoTuplet*>::iterator it;
    for (it = m_tuplets.begin(); it != m_tuplets.end(); ++it)
        delete *it;
    m_tuplets.clear();
}


}  //namespace lenmus
