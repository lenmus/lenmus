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
#include "lenmus_elements.h"
#include "lenmus_stack.h"

using namespace std;

namespace lenmus
{

//forward declarations
class DocCommand;
class DocCommandExecuter;

/// A class to manage the undo/redo stack
typedef UndoableStack<DocCommand*>     UndoStack;



/** Base class for lenmus document
    It provides the basic API for a document. Encapsulates the internal 
    representation structure and provides:
        - an iterator to traverse the document;
        - support for visitors;
        - serialization; and
        - atomic methods to modify the document (no undo/redo capabilities).
        - methods to set/check a 'document modified' flag (but no logic to
          manage this flag, only reset when the document is created/loaded)
*/ //------------------------------------------------------------------
class Document
{
protected:
    LdpTree*    m_pTree;
    bool        m_modified;

public:
    Document(ostream& reporter=cout);
    Document(const std::string& filename, ostream& reporter=cout);
    virtual ~Document();

    void set_command_executer(DocCommandExecuter* pCE);
    virtual int load(const std::string& filename, ostream& reporter=cout);
    virtual int from_string(const std::string& source, ostream& reporter=cout);
 //   virtual void save(const std::string& filename);
    inline void set_modified(bool value) { m_modified = value; }
    inline bool is_modified() { return m_modified; }


    //a cursor for the document
    class iterator
    {
        protected:
            friend class Document;
            LdpTree::depth_first_iterator m_it;
            LdpTree::depth_first_iterator& get_tree_iterator() { return m_it; }

        public:
            iterator() {}
			iterator(LdpTree::depth_first_iterator& it) { m_it = it; }
			iterator(LdpElement* elm) { m_it = LdpTree::depth_first_iterator(elm); }
            virtual ~iterator() {}

	        LdpElement* operator *() const { return *m_it; }
            iterator& operator ++() { ++m_it; return *this; }
            iterator& operator --() { --m_it; return *this; }
		    bool operator ==(const iterator& it) const { return m_it == it.m_it; }
		    bool operator !=(const iterator& it) const { return m_it != it.m_it; }

    };

	iterator begin() { return iterator( m_pTree->begin() ); }
	iterator end() { return iterator( m_pTree->end() ); }

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

    ///
    //LdpElement* replace(iterator it, LdpElement* node);

    //------------------------------------------------------------------
    // Transitional, while moving from score to lenmusdoc
    //------------------------------------------------------------------
    iterator get_score();
    void create_score(ostream& reporter=cout);

protected:

    void clear();
    void create_empty(ostream& reporter=cout);

};


/*!
\brief A class to store data for a command
*/
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
    DocCommandInsert(Document::iterator& it, LdpElement* newElm);
    ~DocCommandInsert();

    void undo(Document* pDoc);
    void redo(Document* pDoc);

protected:
    Document::iterator m_itInserted;
};


class DocCommandPushBack : public DocCommand
{
public:
    DocCommandPushBack(Document::iterator& it, LdpElement* added);
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


/// 
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
    virtual void set_document_modified(bool value) { m_pDoc->set_modified(value); }
    virtual size_t undo_stack_size() { return m_stack.size(); }
};


}   //namespace lenmus

#endif      //__LM_DOCUMENT_H__
