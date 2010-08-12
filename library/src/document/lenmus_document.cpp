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
#include "lenmus_parser.h"
#include "lenmus_compiler.h"
#include "lenmus_injectors.h"
#include "lenmus_id_assigner.h"

using namespace std;

namespace lenmus
{


//------------------------------------------------------------------
// Document implementation
//------------------------------------------------------------------

Document::Document(LdpCompiler* pCompiler, IdAssigner* pIdAssigner) 
    : Observable()
    , m_pTree(NULL)
    , m_pCompiler(pCompiler)
    , m_pIdAssigner(pIdAssigner)
    , m_pDocScope(NULL)
{
}

Document::Document(LibraryScope& libraryScope, ostream& reporter) 
    : Observable()
    , m_pTree(NULL)
{
    m_pDocScope = new DocumentScope(reporter);
    m_pCompiler  = Injector::inject_LdpCompiler(libraryScope, *m_pDocScope);
    m_pIdAssigner = m_pDocScope->id_assigner();
}

Document::~Document()
{
    clear();
    delete m_pCompiler;
    if (m_pDocScope)
        delete m_pDocScope;
}
int Document::from_file(const std::string& filename)
{
    clear();
    m_pTree = m_pCompiler->compile_file(filename);
    return m_pCompiler->get_num_errors();
}

int Document::from_string(const std::string& source)
{
    clear();
    m_pTree = m_pCompiler->compile_string(source);
    return m_pCompiler->get_num_errors();
}

void Document::create_empty()
{
    clear();
    m_pTree = m_pCompiler->create_empty();
}

void Document::create_with_empty_score()
{
    clear();
    m_pTree = m_pCompiler->create_with_empty_score();
}

void Document::clear()
{
    if (m_pTree)
    {
        delete m_pTree->get_root();
        delete m_pTree;
        m_pTree = NULL;
    }
}

Document::iterator Document::content()
{
    iterator it = begin();
    while (it != end() && !(*it)->is_type(k_content))
        ++it;
    return it;
}

Document::iterator Document::insert(iterator& it, LdpElement* node)
{
    // inserts element before position 'it', that is, as previous sibling
    // of node pointed by 'it'

    m_pIdAssigner->reassign_ids(node);
    return (*it)->insert(it, node);
}

void Document::add_param(iterator& it, LdpElement* node)
{
    // adds a child to element referred by iterator 'it'.

    m_pIdAssigner->reassign_ids(node);
    (*it)->append_child(node);
}

LdpElement* Document::remove(iterator& it)
{
    // removes element pointed by 'it'. Returns removed element

    LdpTree::depth_first_iterator itNode = it;
    LdpElement* removed = *itNode;
    m_pTree->erase(itNode);
    return removed;
}

void Document::remove_last_param(iterator& it)
{
    // removes last param of element pointed by 'it'

    Tree<LdpElement>::depth_first_iterator itParm( (*it)->get_last_child() );
    m_pTree->erase(itParm);
}


//------------------------------------------------------------------
// Transitional, while moving from score to lenmusdoc
//------------------------------------------------------------------

Document::iterator Document::get_score()
{
    iterator it = begin();
    while (it != end() && !(*it)->is_type(k_score))
        ++it;
    return it;
}

void Document::create_score(ostream& reporter)
{
    create_with_empty_score();
}


//------------------------------------------------------------------
// DocCommandInsert
//------------------------------------------------------------------

DocCommandInsert::DocCommandInsert(Document::iterator& it, LdpElement* newElm)
    : DocCommand(it, newElm, NULL)
{
}

DocCommandInsert::~DocCommandInsert()
{
    if (!m_applied)
        delete m_added;
}

void DocCommandInsert::undo(Document* pDoc)
{
    (*m_itInserted)->reset_modified();
    pDoc->remove(m_itInserted);
    m_applied = false;
}

void DocCommandInsert::redo(Document* pDoc)
{
    m_itInserted = pDoc->insert(m_position, m_added);
    (*m_itInserted)->set_modified();
    m_applied = true;
}


//------------------------------------------------------------------
// DocCommandPushBack
//------------------------------------------------------------------

DocCommandPushBack::DocCommandPushBack(Document::iterator& it, LdpElement* added)
    : DocCommand(it, added, NULL)
{
}

DocCommandPushBack::~DocCommandPushBack()
{
    if (!m_applied)
        delete m_added;
}

void DocCommandPushBack::undo(Document* pDoc)
{
    (*m_position)->reset_modified();
    pDoc->remove_last_param(m_position);
    m_applied = false;
}

void DocCommandPushBack::redo(Document* pDoc)
{
    pDoc->add_param(m_position, m_added);
    (*m_position)->set_modified();
    m_applied = true;
}


//------------------------------------------------------------------
// DocCommandRemove
//------------------------------------------------------------------

DocCommandRemove::DocCommandRemove(Document::iterator& it)
    : DocCommand(it, NULL, *it)
{
    m_parent = (*it)->get_parent();
    m_nextSibling = (*it)->get_next_sibling();
}

DocCommandRemove::~DocCommandRemove()
{
    if (m_applied)
        delete m_removed;
}

void DocCommandRemove::undo(Document* pDoc)
{
    m_parent->reset_modified();
    if (!m_nextSibling)
    {
        Document::iterator it(m_parent);
        pDoc->add_param(it, m_removed);
    }
    else
    {
        Document::iterator it(m_nextSibling);
        pDoc->insert(it, m_removed);
    }
    m_applied = false;
}

void DocCommandRemove::redo(Document* pDoc)
{
    pDoc->remove(m_position);
    m_parent->set_modified();
    m_applied = true;
}


//------------------------------------------------------------------
// DocCommandExecuter
//------------------------------------------------------------------

DocCommandExecuter::DocCommandExecuter(Document* target)
    : m_pDoc(target)
{
}

void DocCommandExecuter::execute(DocCommand* pCmd)
{
    m_stack.push(pCmd);
    pCmd->redo(m_pDoc);
}

void DocCommandExecuter::undo()
{
    DocCommand* cmd = m_stack.pop();
    cmd->undo(m_pDoc);
}

void DocCommandExecuter::redo()
{
    DocCommand* cmd = m_stack.undo_pop();
    cmd->redo(m_pDoc);
}


}  //namespace lenmus
