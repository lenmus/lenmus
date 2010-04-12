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

#include "lenmus_document.h"
#include "lenmus_elements.h"
#include "lenmus_user_command.h"

using namespace std;

namespace lenmus
{

class UserCommandData
{
protected:
    std::string     m_name;
    int             m_startPos;
    int             m_endPos;

public:
    UserCommandData(const std::string& name, int startPos) 
        : m_name(name), m_startPos(startPos), m_endPos(0) {}
        ~UserCommandData() {}

    inline void set_end_pos(int n) { m_endPos = n; }
    inline int get_num_actions() { return m_endPos - m_startPos; }
};

//------------------------------------------------------------------
// UserCommandExecuter
//------------------------------------------------------------------

UserCommandExecuter::UserCommandExecuter(DocCommandExecuter* target)
    : m_pDocCommandExecuter(target)
{
}

void UserCommandExecuter::execute(UserCommand& cmd)
{
    UserCommandData* data 
      = new UserCommandData(cmd.get_name(), 
                            static_cast<int>(m_pDocCommandExecuter->undo_stack_size()) );
    m_stack.push(data);
    cmd.do_actions(m_pDocCommandExecuter);
    data->set_end_pos( static_cast<int>(m_pDocCommandExecuter->undo_stack_size()) );
}

void UserCommandExecuter::undo()
{
    //UserCommandData* data = m_stack.pop();
    //for (int i=0; i < data->get_num_actions(); ++i)
    //  m_pDocCommandExecuter->undo();
}

void UserCommandExecuter::redo()
{
    //UserCommandData* data = m_stack.undo_pop();
    //for (int i=0; i < data->get_num_actions(); ++i)
    //  m_pDocCommandExecuter->redo();
}


}  //namespace lenmus
