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

#include <sstream>
#include <algorithm>

#include "lenmus_injectors.h"
#include "lenmus_document.h"
#include "lenmus_user_command.h"
#include "lenmus_model_builder.h"

using namespace std;

namespace lenmus
{

//------------------------------------------------------------------
// UserCommandExecuter
//------------------------------------------------------------------

UserCommandExecuter::UserCommandExecuter(DocumentScope& documentScope, Document* pDoc)
    : m_pDoc(pDoc)
    , m_docCommandExecuter(pDoc)
    , m_pModelBuilder( Injector::inject_ModelBuilder(documentScope) )
{
}

UserCommandExecuter::UserCommandExecuter(Document* pDoc, ModelBuilder* pBuilder)
    : m_pDoc(pDoc)
    , m_docCommandExecuter(pDoc)
    , m_pModelBuilder(pBuilder)
{
    //dependency injection constructor, for unit testing
}

UserCommandExecuter::~UserCommandExecuter()
{
    delete m_pModelBuilder;
}

void UserCommandExecuter::execute(UserCommand& cmd)
{
    UserCommandData* data 
      = new UserCommandData(cmd.get_name(), 
                            m_docCommandExecuter.is_document_modified(),
                            static_cast<int>(m_docCommandExecuter.undo_stack_size()) );
    m_stack.push(data);
    cmd.do_actions(&m_docCommandExecuter);
    data->set_end_pos( static_cast<int>(m_docCommandExecuter.undo_stack_size()) );
    update_model();
}

void UserCommandExecuter::undo()
{
    UserCommandData* data = m_stack.pop();
    for (int i=0; i < data->get_num_actions(); ++i)
      m_docCommandExecuter.undo();
    update_model();
}

void UserCommandExecuter::redo()
{
    UserCommandData* data = m_stack.undo_pop();
    for (int i=0; i < data->get_num_actions(); ++i)
      m_docCommandExecuter.redo();
    update_model();
}

void UserCommandExecuter::update_model()
{
    m_pModelBuilder->update_model(m_pDoc->get_tree());
}


}  //namespace lenmus
