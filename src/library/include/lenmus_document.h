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

#ifndef __LM_DOCUMENT_H__
#define __LM_DOCUMENT_H__

#include <sstream>
#include "lenmus_injectors.h"
#include "lenmus_observable.h"
#include "lenmus_elements.h"
#include "lenmus_stack.h"

using namespace std;

namespace lenmus
{

//forward declarations
class DocCommand;
class DocCommandExecuter;
class LdpCompiler;

/// A class to manage the undo/redo stack
typedef UndoableStack<DocCommand*>     UndoStack;


//------------------------------------------------------------------------------------
// Base class for lenmus document.
// Encapsulates all the library internals, providing the basic API for creating and
// using a document.
//      - an iterator to traverse the document;
//      - support for visitors;
//      - serialization; and
//      - atomic methods to modify the document (no undo/redo capabilities).
//      - methods to set/check a 'document modified' flag (but no logic to
//        manage this flag, only reset when the document is created/loaded)
//------------------------------------------------------------------------------------

class Document : public Observable
{
protected:
    LdpTree*        m_pTree;
    LdpCompiler*    m_pCompiler;

public:
    Document(LibraryScope& libraryScope, ostream& reporter=cout);    //default compiler
    Document(LdpCompiler* pCompiler);       //injected compiler
    virtual ~Document();

    //creation
    int from_file(const std::string& filename);
    int from_string(const std::string& source);
    void create_empty();
    void create_with_empty_score();

    inline bool is_modified() { return m_pTree->is_modified(); }
    inline void clear_modified() { m_pTree->clear_modified(); }
    inline LdpTree* get_tree() { return m_pTree; }

    //a low level cursor for the document
    typedef LdpTree::depth_first_iterator iterator;

	iterator begin() { LdpTree::depth_first_iterator it = m_pTree->begin(); return iterator(it); }
	iterator end() { LdpTree::depth_first_iterator it = m_pTree->end(); return iterator(it); }
    iterator content();

    std::string to_string(iterator& it) { return (*it)->to_string(); }
    std::string to_string() { return m_pTree->get_root()->to_string(); }

        //atomic commands to edit the document. No undo/redo capabilities.
        //In principle, to be used only by DocCommandExecuter

    /// inserts param before the element at position referred by iterator 'it'.
    /// Returns iterator pointing to the newly inserted element
    iterator insert(iterator& it, LdpElement* node);

    /// push back a param to element referred by iterator 'it'.
    void add_param(iterator& it, LdpElement* node);

    /// removes element pointed by 'it'.
    LdpElement* remove(iterator& it);

    /// removes last param of element pointed by 'it'.
    void remove_last_param(iterator& it);

    //------------------------------------------------------------------
    // Transitional, while moving from score to lenmusdoc
    //------------------------------------------------------------------
public:
    iterator get_score();
    void create_score(ostream& reporter=cout);

protected:

    void clear();

};


// A class to store data for a command
//------------------------------------------------------------------
class DocCommand
{
protected:
    Document::iterator m_position;
    LdpElement* m_added;
    LdpElement* m_removed;
    bool m_applied;

public:
    DocCommand(Document::iterator& it, LdpElement* added, LdpElement* removed)
        : m_position(it), m_added(added), m_removed(removed), m_applied(false) {}

    virtual ~DocCommand() {}

    //getters
    inline Document::iterator& get_position() { return m_position; }
    inline LdpElement* get_added() { return m_added; }
    inline LdpElement* get_removed() { return m_removed; }

    //actions
    virtual void undo(Document* pDoc)=0;
    virtual void redo(Document* pDoc)=0;
};


class DocCommandInsert : public DocCommand
{
public:
    DocCommandInsert(Document::iterator& it, LdpElement* pNewElm);
    ~DocCommandInsert();

    void undo(Document* pDoc);
    void redo(Document* pDoc);

protected:
    Document::iterator m_itInserted;
};


class DocCommandPushBack : public DocCommand
{
public:
    DocCommandPushBack(Document::iterator& it, LdpElement* pNewElm);
    ~DocCommandPushBack();

    void undo(Document* pDoc);
    void redo(Document* pDoc);
};


class DocCommandRemove : public DocCommand
{
public:
    DocCommandRemove(Document::iterator& it);
    ~DocCommandRemove();

    void undo(Document* pDoc);
    void redo(Document* pDoc);

protected:
    LdpElement*     m_parent;
    LdpElement*     m_nextSibling;
};


//
class DocCommandExecuter
{
private:
    Document*   m_pDoc;
    UndoStack   m_stack;

public:
    DocCommandExecuter(Document* target);
    virtual ~DocCommandExecuter() {}
    virtual void execute(DocCommand* pCmd);
    virtual void undo();
    virtual void redo();

    virtual bool is_document_modified() { return m_pDoc->is_modified(); }
    virtual size_t undo_stack_size() { return m_stack.size(); }
};


}   //namespace lenmus

#endif      //__LM_DOCUMENT_H__
