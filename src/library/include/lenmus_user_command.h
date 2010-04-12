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

#ifndef __LM_USER_COMMAND_H__
#define __LM_USER_COMMAND_H__

#include <sstream>
#include "lenmus_elements.h"
#include "lenmus_stack.h"

using namespace std;

namespace lenmus
{

//forward declarations
class UserCommand;
class DocCommandExecuter;
class UserCommandData;

/// A class to manage the undo/redo stack
typedef UndoableStack<UserCommandData*>     CmdDataUndoStack;



/// 
class UserCommandExecuter
{
private:
    DocCommandExecuter* m_pDocCommandExecuter;
    CmdDataUndoStack    m_stack;

public:
    UserCommandExecuter(DocCommandExecuter* target);
    virtual ~UserCommandExecuter() {}
    virtual void execute(UserCommand& cmd);
    virtual void undo();
    virtual void redo();

    virtual size_t undo_stack_size() { return m_stack.size(); }
};


/*!
\brief A class to store data for a command
*/
//------------------------------------------------------------------
class UserCommand
{
public:
    UserCommand(const std::string& name) : m_name(name) {}
    virtual ~UserCommand() {}

    inline std::string get_name() { return m_name; }

protected:
    friend class UserCommandExecuter;
    virtual bool do_actions(DocCommandExecuter* dce)=0;

    std::string     m_name;
};




}   //namespace lenmus

#endif      //__LM_USER_COMMAND_H__
