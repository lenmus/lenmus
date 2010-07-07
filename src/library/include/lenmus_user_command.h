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
#include <list>
#include "lenmus_elements.h"
#include "lenmus_stack.h"

using namespace std;

namespace lenmus
{

//forward declarations
class UserCommand;
class DocCommandExecuter;
class CmdActionData;
class ModelBuilder;
class DocumentScope;

// a helper class to store information about execution of a user command
class UserCommandData
{
protected:
    std::string     m_name;
    int             m_startPos;
    int             m_endPos;
    bool            m_docModified;

public:
    UserCommandData(const std::string& name, bool modified, int startPos) 
        : m_name(name), m_docModified(modified), m_startPos(startPos), m_endPos(0) {}
    ~UserCommandData() {}

    inline void set_end_pos(int n) { m_endPos = n; }
    inline int get_num_actions() { return m_endPos - m_startPos; }
    inline bool get_modified() { return m_docModified; }
};

// A class to manage the undo/redo stack of user commands
typedef UndoableStack<UserCommandData*>     CmdDataUndoStack;



/// 
class UserCommandExecuter
{
private:
    DocCommandExecuter  m_docCommandExecuter;
    CmdDataUndoStack    m_stack;
    Document*           m_pDoc;
    ModelBuilder*       m_pModelBuilder;

public:
    UserCommandExecuter(DocumentScope& documentScope, Document* pDoc);
    UserCommandExecuter(Document* pDoc, ModelBuilder* pBuilder);     //only for tests
    virtual ~UserCommandExecuter();
    virtual void execute(UserCommand& cmd);
    virtual void undo();
    virtual void redo();

    virtual size_t undo_stack_size() { return m_stack.size(); }

private:
    void update_model();
};



// A class to store data for a command
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

    std::string             m_name;
};




}   //namespace lenmus

#endif      //__LM_USER_COMMAND_H__
