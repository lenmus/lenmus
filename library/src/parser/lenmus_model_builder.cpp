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

#include "lenmus_document.h"
#include "lenmus_document_iterator.h"
#include "lenmus_internal_model.h"
#include "lenmus_staffobjs_table.h"
#include "lenmus_model_builder.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// ModelBuilder implementation
//-------------------------------------------------------------------------------------

ModelBuilder::ModelBuilder(ostream& reporter)
    : m_reporter(reporter)
{
} 

ModelBuilder::~ModelBuilder()
{
}

void ModelBuilder::build_model(LdpTree* pTree)
{
    m_pTree = pTree;
    DocIterator it(m_pTree);
    for (it.start_of_content(); *it != NULL; ++it)
        structurize(it);
}

void ModelBuilder::update_model(LdpTree* pTree)
{
    m_pTree = pTree;
    DocIterator it(m_pTree);
    for (it.start_of_content(); *it != NULL; ++it)
    {
        //Factory method ?
        if ((*it)->is_modified())
        {
            if((*it)->is_type(k_score))
            {
                ColStaffObjsBuilder builder(m_pTree);
                builder.update(*it);
            }
        }
    }
}

void ModelBuilder::structurize(DocIterator it)
{
    //in future this should invoke a factory object

    if ((*it)->is_type(k_score))
    {
        ColStaffObjsBuilder builder(m_pTree);
        builder.build(*it);
    }
}



}  //namespace lenmus
