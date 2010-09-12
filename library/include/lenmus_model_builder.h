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

#ifndef __LML_MODEL_BUILDER_H__
#define __LML_MODEL_BUILDER_H__

#include <vector>
#include "lenmus_document.h"
#include "lenmus_document_iterator.h"

using namespace std;

namespace lenmus
{

//forward declarations
class BasicModel;
class ImoDocument;

//-------------------------------------------------------------------------------------
// ImObjectsBuilder. Helper class to build the ImoObj model from the basic model
//-------------------------------------------------------------------------------------

class ImObjectsBuilder
{
protected:
    ostream&    m_reporter;
    BasicModel* m_pBasicModel;

public:
    ImObjectsBuilder(ostream& reporter);
    virtual ~ImObjectsBuilder();

    ImoDocument* create_objects(BasicModel* pBasicModel);

protected:

};


//-------------------------------------------------------------------------------------
// ModelBuilder. Implements the final step of LDP compiler: code generation.
// Traverses the parse tree and creates the internal model
//-------------------------------------------------------------------------------------

class ModelBuilder
{
protected:
    ostream&    m_reporter;
    LdpTree*    m_pTree;

public:
    ModelBuilder(ostream& reporter);
    virtual ~ModelBuilder();

    ImoDocument* build_model(BasicModel* pBasicModel);


protected:
    void structurize(DocIterator it);
    void structurize(ImoObj* pImo);

};


}   //namespace lenmus

#endif      //__LML_MODEL_BUILDER_H__
