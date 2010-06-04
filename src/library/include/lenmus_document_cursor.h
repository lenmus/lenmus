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

#ifndef __LM_DOCUMENT_CURSOR_H__
#define __LM_DOCUMENT_CURSOR_H__

#include <stack>
#include "lenmus_document_iterator.h"
#include "lenmus_staffobjs_table.h"

using namespace std;

namespace lenmus
{

//forward declarations
class Document;
class LdpElement;
class ScoreIterator;
class ImScore;


//-------------------------------------------------------------------------------------
// ElementCursor: base class for any cursor
//-------------------------------------------------------------------------------------

class ElementCursor
{
public:
    ElementCursor() {}
    virtual ~ElementCursor() {}

    //positioning
    inline void operator ++() { next(); }
    inline void operator --() { prev(); }

    virtual ElementCursor* enter_element() { return this; }
    virtual LdpElement* get_pointee()=0;
    virtual void next()=0;
    virtual void prev()=0;

};


//-------------------------------------------------------------------------------------
// DocCursor 
//-------------------------------------------------------------------------------------

class DocCursor
{
protected:
    Document*       m_pDoc;
    ElementCursor*  m_pCursor;
    DocIterator     m_it;
    LdpElement*     m_pFirst;

public:
    DocCursor(Document* pDoc);
    virtual ~DocCursor();

    inline LdpElement* operator *() { return get_pointee(); }
    LdpElement* get_pointee();

    //positioning
    void enter_element();
    inline void operator ++() { next(); }
    inline void operator --() { prev(); }

protected:
    void next();
    void prev();
    void start_delegation();
    void stop_delegation();
    inline bool is_delegating() { return m_pCursor != NULL; }

};


//-------------------------------------------------------------------------------------
// ScoreCursor
//-------------------------------------------------------------------------------------

class ScoreCursor : public ElementCursor
{
protected:
    Document*           m_pDoc;
    ImScore*            m_pScore;
    ColStaffObjs*       m_pColStaffObjs;
    ColStaffObjs::iterator   m_it;

public:
    ScoreCursor(Document* pDoc, LdpElement* pScoreElm);
    virtual ~ScoreCursor();

    void next();
    void prev();
    LdpElement* get_pointee();

protected:
    void start();
};


}   //namespace lenmus

#endif      //__LM_DOCUMENT_CURSOR_H__
