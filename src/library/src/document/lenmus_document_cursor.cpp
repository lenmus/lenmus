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

#include "lenmus_document.h"
#include "lenmus_document_cursor.h"
#include "lenmus_elements.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// ElementIterator implementation
//-------------------------------------------------------------------------------------

ElementIterator::ElementIterator(Document* pDoc)
    : m_pDoc(pDoc)
{
    m_it = pDoc->begin();
} 

ElementIterator::~ElementIterator()
{
}

void ElementIterator::to_begin()
{
    clear_stack();
    m_it = m_pDoc->begin();
}

void ElementIterator::clear_stack()
{
    while (!m_stack.empty())
        m_stack.pop();
}

void ElementIterator::next()
{
    if (*m_it != NULL)
        m_it = (*m_it)->get_next_sibling();
}

void ElementIterator::prev()
{
    if (*m_it != NULL)
        m_it = (*m_it)->get_prev_sibling();
}

bool ElementIterator::is_pointing_to(long elmType)
{
    return *m_it != NULL && (*m_it)->get_type() == elmType;
}

void ElementIterator::point_to(long elmType)
{
    while (*m_it != NULL && !is_pointing_to(elmType))
        next();
}

void ElementIterator::point_to(LdpElement* pElm)
{
    m_it = Document::iterator(pElm);
}

void ElementIterator::enter_element()
{
    if (*m_it != NULL)
    {
        m_stack.push(m_it);
        ++m_it;
    }
}

void ElementIterator::exit_element()
{
    if (!m_stack.empty())
    {
        m_it = m_stack.top();
        m_stack.pop();
    }
    else
        m_it == NULL;
}

void ElementIterator::exit_all_to(LdpElement* pElm)

{
     //exit elements until the received one

    while (*m_it != pElm && !m_stack.empty())
    {
        m_it = m_stack.top();
        if (*m_it == pElm)
            break;
        m_stack.pop();
    }
}

//void ElementIterator::start_of(long elmType, int num)
//{
//    //within the limits of current element finds the element #num [0..n-1] 
//    //of type 'elmType' and points to its first sub-element
//
//    to_begin();
//    enter_element();
//    point_to(k_content);
//    enter_element();
//}



//-------------------------------------------------------------------------------------
// DocIterator implementation
//-------------------------------------------------------------------------------------

DocIterator::DocIterator(Document* pDoc)
    : ElementIterator(pDoc)
    , m_pScoreCursor(NULL)
{
    m_it = m_pDoc->begin();
    enter_element();
} 

DocIterator::~DocIterator()
{
    if (m_pScoreCursor)
        delete m_pScoreCursor;
}

void DocIterator::enter_element()
{
    //if necessary, create specific cursor to delegate to it. 
    //TODO: This is a Factory method that violates the Open Close Principle
    if ((*m_it)->get_type() == k_score)
    {
        if (m_pScoreCursor)
            delete m_pScoreCursor;
        m_pScoreCursor = new ScoreElmIterator(this);
    }
    else
        ElementIterator::enter_element();
}

void DocIterator::start_of_content()
{
    //to first sub-element in 'content' element

    to_begin();
    enter_element();
    point_to(k_content);
    enter_element();
}



//-------------------------------------------------------------------------------------
// ScoreElmIterator implementation
//-------------------------------------------------------------------------------------

ScoreElmIterator::ScoreElmIterator(ElementIterator* pCursor)
    : m_pCursor(pCursor)
{
    m_pScore = **pCursor;
    m_pCursor->enter_element();
} 

ScoreElmIterator::~ScoreElmIterator()
{
}

void ScoreElmIterator::start()
{
    m_pCursor->exit_all_to(m_pScore);
    m_pCursor->enter_element();
}

void ScoreElmIterator::start_of_instrument(int instr)
{
    //to first staff obj of instr (0..n-1)

    find_instrument(instr);
    m_pCursor->enter_element();
    m_pCursor->point_to(k_musicData);
    m_pCursor->enter_element();
}

void ScoreElmIterator::find_instrument(int instr)
{
    //instr = 0..n

    start();
    m_pCursor->point_to(k_instrument);
    for (int i=0; i != instr && !m_pCursor->is_out_of_range(); i++)
    {
        ++(*m_pCursor);
        m_pCursor->point_to(k_instrument);
    }
}


}  //namespace lenmus
