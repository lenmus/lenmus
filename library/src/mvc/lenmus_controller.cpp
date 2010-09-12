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

#include "lenmus_controller.h"
#include "lenmus_user_command.h"
#include "lenmus_command.h"
#include "lenmus_compiler.h"
#include "lenmus_document.h"
#include "lenmus_document_cursor.h"
#include "lenmus_basic.h"

using namespace std;
//#define TRT(a) (a)

namespace lenmus
{

//-------------------------------------------------------------------------------------
// Controller implementation
//-------------------------------------------------------------------------------------

Controller::Controller(LibraryScope& libraryScope, Document* pDoc,
                       UserCommandExecuter* pExec)
    : m_libScope(libraryScope)
    , m_pDoc(pDoc)
    , m_pExec(pExec)
{
    DocumentScope* pDocScope = pDoc->get_scope();
    m_pCompiler  = Injector::inject_LdpCompiler(m_libScope, *pDocScope);
}

Controller::~Controller()
{
    delete m_pCompiler;
}

void Controller::insert_rest(DocCursor& cursor, const std::string& source)
{
    LdpElement* pElm = m_pCompiler->create_element(source);
    CmdInsertElement cmd(TRT("Insert rest"), cursor, pElm, m_pCompiler);
    m_pExec->execute(cmd);

    //place cursor after inserted object
    cursor.reset_and_point_to( pElm->get_id() );
    cursor.move_next();

    m_pDoc->notify_that_document_has_been_modified();
}




//-------------------------------------------------------------------------------------
// EditController implementation
//-------------------------------------------------------------------------------------

EditController::EditController(LibraryScope& libraryScope, Document* pDoc,
                               UserCommandExecuter* pExec)
    : Controller(libraryScope, pDoc, pExec)
{
}

EditController::~EditController()
{
}



}  //namespace lenmus
