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
#include "lenmus_model_builder.h"
#include "lenmus_internal_model.h"
#include "lenmus_staffobjs_table.h"
#include "lenmus_basic_model.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// ImObjectsBuilder. Helper class to build the ImoObj model from the basic model
//-------------------------------------------------------------------------------------

ImObjectsBuilder::ImObjectsBuilder(ostream& reporter)
    : m_reporter(reporter)
    , m_pBasicModel(NULL)
{
    if (m_pBasicModel)
        delete m_pBasicModel;
}

ImObjectsBuilder::~ImObjectsBuilder()
{
}

ImoDocument* ImObjectsBuilder::create_objects(BasicModel* pBasicModel)
{
    m_pBasicModel = pBasicModel;
    return dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
}



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

ImoDocument* ModelBuilder::build_model(BasicModel* pBasicModel)
{
    ImObjectsBuilder imb(m_reporter);
    ImoDocument* pDoc = imb.create_objects(pBasicModel);
    int numContent = pDoc->get_num_content_items();
    for (int i = 0; i < numContent; i++)
        structurize( pDoc->get_content_item(i) );
    return pDoc;
}


//void ModelBuilder::update_model(LdpTree* pTree)
//{
//    m_pTree = pTree;
//    DocIterator it(m_pTree);
//    for (it.start_of_content(); *it != NULL; ++it)
//    {
//        //Factory method ?
//        if ((*it)->is_modified())
//        {
//            if((*it)->is_type(k_score))
//            {
//                ImoScore* pScore = dynamic_cast<ImoScore*>( (*it)->get_imobj() );
//                ColStaffObjsBuilder builder(m_pTree);
//                builder.update(pScore);
//            }
//        }
//    }
//}

//void ModelBuilder::structurize(DocIterator it)
//{
//    //in future this should invoke a factory object
//
//    if ((*it)->is_type(k_score))
//    {
//        ColStaffObjsBuilder builder(m_pTree);
//        builder.build(*it);
//    }
//}

void ModelBuilder::structurize(ImoObj* pImo)
{
    //in future this should invoke a factory object

    ImoScore* pScore = dynamic_cast<ImoScore*>(pImo);
    if (pScore)
    {
        ColStaffObjsBuilder builder(m_pTree);
        builder.build(pScore);
    }
}



}  //namespace lenmus
