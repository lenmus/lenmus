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
#include "lenmus_parser.h"

using namespace std;

namespace lenmus
{

Document::Document()
    : m_pTree(NULL)
{
    create_empty();
} 

Document::Document(const std::string& filename)
    : m_pTree(NULL)
{
    load(filename);
}

Document::~Document()
{
    clear();
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

void Document::create_empty()
{
    clear();
    LdpParser parser(cout);
    m_pTree = parser.parse_text("(lenmusdoc (vers 0.0))");
} 

void Document::load(const std::string& filename)
{
    clear();
    LdpParser parser(cout);
    SpLdpTree tree = parser.parse_file(filename);

    if (tree->get_root()->get_type() == k_score)
    {
        create_empty();
        iterator it = begin();
        add_param(it, tree->get_root());
    }
    else
        m_pTree = tree;
}

/// inserts element before position 'it', that is, as previous sibling 
/// of node pointed by 'it'
Document::iterator Document::insert(iterator& it, LdpElement* node)
{
    return (*it)->insert(it.get_tree_iterator(), node);
}

/// adds a child to element referred by iterator 'it'.
void Document::add_param(iterator& it, LdpElement* node)
{
    (*it)->append_child(node);
}

/// removes element pointed by 'it'. Returns removed element
LdpElement* Document::remove(iterator& it)
{
    Tree<LdpElement>::depth_first_iterator itNode = it.get_tree_iterator();
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

/////
//LdpElement* Document::replace(iterator it, LdpElement* node)
//{
//    return NULL;
//}


//------------------------------------------------------------------
// Transitional, while moving from score to lenmusdoc
//------------------------------------------------------------------

Document::iterator Document::get_score()
{
    iterator it = begin();
    while (it != end() && (*it)->get_type() != k_score)
        ++it;
    return it;
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
    pDoc->remove(m_itInserted);
    m_applied = false;
}

void DocCommandInsert::redo(Document* pDoc)
{
    m_itInserted = pDoc->insert(m_position, m_added);
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
    pDoc->remove_last_param(m_position);
    m_applied = false;
}

void DocCommandPushBack::redo(Document* pDoc)
{
    pDoc->add_param(m_position, m_added);
    m_applied = true;
}


//------------------------------------------------------------------
// DocCommandRemove
//------------------------------------------------------------------

DocCommandRemove::DocCommandRemove(Document::iterator& it) 
    : DocCommand(it, NULL, *it), m_itParent(it)
{
    --m_itParent; //TO_FIX: this is not the parent!!!!!
}

DocCommandRemove::~DocCommandRemove()
{
    if (m_applied)
        delete m_removed;
}

void DocCommandRemove::undo(Document* pDoc)
{
    Document::iterator it = m_itParent;
    if (++it == pDoc->end())
        pDoc->add_param(m_itParent, m_removed);
    else
        pDoc->insert(it, m_removed);
    m_applied = false;
}

void DocCommandRemove::redo(Document* pDoc)
{
    Document::iterator it = m_itParent;
    pDoc->remove(++it);
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
