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

#ifndef __LML_DOCUMENT_ITERATOR_H__
#define __LML_DOCUMENT_ITERATOR_H__

#include <stack>

using namespace std;

namespace lenmus
{

//forward declarations
class Document;
class LdpElement;


//-------------------------------------------------------------------------------------
// interfaces for traversing specific elements
//-------------------------------------------------------------------------------------

class InterfaceScoreElmIterator
{
public:
    //positioning
    virtual void start_of_instrument(int instr)=0;    //to first staff obj in instr 0..n
    virtual void find_instrument(int instr)=0;        //instr = 0..n-1
};



//-------------------------------------------------------------------------------------
// ElementIterator: base class for any document iterator. It provides the basic
// capabilities for traversing elements, locating specific sub-elements, etc. 
//-------------------------------------------------------------------------------------

class ElementIterator
{
protected:
    LdpTree*                m_pTree;
    Document::iterator      m_it;
    std::stack<Document::iterator> m_stack; //to save m_it when entering into an element

public:
    ElementIterator(LdpTree* pTree);
    ElementIterator(Document* pDoc);
    virtual ~ElementIterator();

    inline LdpElement* operator *() { return *m_it; }
    inline Document::iterator get_iterator() { return m_it; }

    //positioning
    inline void operator ++() { next(); }
    inline void operator --() { prev(); }
    void point_to(long elmType);
    void point_to(LdpElement* pElm);
    void enter_element();
    void exit_element();
    void exit_all_to(LdpElement* pElm);     //exit elements until the received one
    void to_begin();
    //void start_of(long elmType, int num);   //to first sub-element in element #num [0..n-1] of type 'elmType'

    //information
    bool is_pointing_to(long elmType);
    inline bool is_out_of_range() { return *m_it == NULL; }

protected:
    virtual void next();    //to next sibling element
    virtual void prev();    //to prev sibling element
    void clear_stack();

};





//-------------------------------------------------------------------------------------
// ScoreElmIterator: adds score traversing capabilities to the received cursor.
// The cursor must be pointing to the score when invoking the constructor
//-------------------------------------------------------------------------------------

class ScoreElmIterator : public InterfaceScoreElmIterator
{
protected:
    ElementIterator* m_pCursor;
    LdpElement* m_pScore;

public:
    ScoreElmIterator(ElementIterator* pCursor);
    virtual ~ScoreElmIterator();

    //positioning
    void start_of_instrument(int instr);    //to first staff obj in instr 0..n
    void find_instrument(int instr);        //instr = 0..n-1

protected:
    void start();   //to first element in score (should be 'vers')

};



//-------------------------------------------------------------------------------------
// DocIterator: A cursor to traverse the document for interactive edition
// - It uses the facade pattern to hide the particularities of traversing each 
//   document element type
// - For traversing each element type it delegates on a specific cursor (adaptor
//   pattern) 
//------------------------------------------------------------------------------------

class DocIterator : public ElementIterator, public InterfaceScoreElmIterator
{
public:
    DocIterator(Document* pDoc);
    DocIterator(LdpTree* pTree);
    virtual ~DocIterator();

    //positioning
    void start_of_content();
    void last_of_content();

    //overrides
    void enter_element();

    //implement InterfaceScoreElmIterator by delegation
    void start_of_instrument(int instr) {   //to first staff obj in instr 0..n
            if (m_pScoreElmIterator)
                m_pScoreElmIterator->start_of_instrument(instr);
        }

    void find_instrument(int instr) {   //instr = 0..n-1
            if (m_pScoreElmIterator)
                m_pScoreElmIterator->find_instrument(instr);
        }

protected:
    ScoreElmIterator* m_pScoreElmIterator;

};


}   //namespace lenmus

#endif      //__LML_DOCUMENT_ITERATOR_H__
