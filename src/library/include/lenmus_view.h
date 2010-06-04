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

#ifndef __LM_VIEW_H__
#define __LM_VIEW_H__

#include <list>
#include <iostream>
#include "lenmus_document_iterator.h"

using namespace std;

namespace lenmus
{

//forward declarations
class Document;
class MvcContainer;


//Abstract class from which all views must derive
class View
{
protected:
    Document*   m_pDoc;     //the observed document

public:

    View(Document* pDoc);
    virtual ~View();

protected:

};


//A view to edit the score in full page
class EditView : public View
{
protected:
    DocIterator      m_cursor;

public:

    EditView(Document* pDoc);
    ~EditView();

    inline DocIterator& get_cursor() { return m_cursor; }
    Document::iterator get_cursor_position();

protected:

};



}   //namespace lenmus

#endif      //__LM_VIEW_H__
