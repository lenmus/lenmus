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
#include "lenmus_score_iterator.h"
#include "lenmus_internal_model.h"

using namespace std;

namespace lenmus
{

//-------------------------------------------------------------------------------------
// DocCursor implementation
//-------------------------------------------------------------------------------------

DocCursor::DocCursor(Document* pDoc)
    : m_pDoc(pDoc)
    , m_it(pDoc)
    , m_pCursor(NULL)
{
    m_it.start_of_content();
    m_pFirst = *m_it;
} 

DocCursor::~DocCursor()
{
    if (m_pCursor)
        delete m_pCursor;
}

LdpElement* DocCursor::get_pointee()
{
    return (is_delegating() ? m_pCursor->get_pointee() : *m_it);
}

void DocCursor::enter_element()
{
    if (!is_delegating())
        start_delegation();
}

void DocCursor::start_delegation()
{
    //Factory method to create delegate cursors
    if ((*m_it)->is_type(k_score))
        m_pCursor = new ScoreCursor(m_pDoc, *m_it);
}

void DocCursor::stop_delegation()
{
    delete m_pCursor;
    m_pCursor = NULL;
}

void DocCursor::next()
{
    is_delegating() ? m_pCursor->next() : ++m_it;
}

void DocCursor::prev()
{
    if (is_delegating())
    {
        m_pCursor->prev();
        if (m_pCursor->get_pointee() == NULL)
            stop_delegation();
        --m_it;
    }
    else if (*m_it != m_pFirst)
    {
        if (*m_it != NULL)
            --m_it;
        else
            m_it.last_of_content();
    }
}


//-------------------------------------------------------------------------------------
// ScoreCursor implementation
//-------------------------------------------------------------------------------------

ScoreCursor::ScoreCursor(Document* pDoc, LdpElement* pScoreElm)
    : m_pDoc(pDoc)
    , m_pScore( dynamic_cast<ImScore*>(pScoreElm->get_imobj()) )
    , m_pColStaffObjs( m_pScore->get_staffobjs_table() )
{
    if (m_pColStaffObjs == NULL)
    {
        ColStaffObjsBuilder builder(m_pDoc);
        m_pColStaffObjs = builder.build(pScoreElm);
        m_pScore->set_staffobjs_table(m_pColStaffObjs);
    }
    start();
} 

ScoreCursor::~ScoreCursor()
{
}
void ScoreCursor::next() 
{
    ++m_it;
}

void ScoreCursor::prev() 
{
    if (m_it == m_pColStaffObjs->begin())
        m_it = m_pColStaffObjs->end();
    else
        --m_it;
}

void ScoreCursor::start() 
{
    m_it = m_pColStaffObjs->begin();
}

LdpElement* ScoreCursor::get_pointee() 
{ 
    ColStaffObjs::iterator itEnd = m_pColStaffObjs->end();
    if (m_it != itEnd)  //m_pColStaffObjs->end())
        return (*m_it)->element();
    else
        return NULL;
}



}  //namespace lenmus
