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

#ifndef __LML_CONTROLLER_H__
#define __LML_CONTROLLER_H__

#include <list>
#include <iostream>
#include "lenmus_document_cursor.h"
#include "lenmus_observable.h"

using namespace std;

namespace lenmus
{

//forward declarations
class Document;
class MvcElement;


//Abstract class from which all controllers must derive
class Controller
{
protected:
    LibraryScope&           m_pLibScope;
    Document*               m_pDoc;
    UserCommandExecuter*    m_pExec;
    LdpCompiler*            m_pCompiler;

public:
    Controller(LibraryScope& libraryScope, Document* pDoc, UserCommandExecuter* pExec);
    virtual ~Controller();

    //abstract class implements all possible commands. Derived classes override
    //them as needed: to empty method those not allowed or to a different behaviour.
    virtual void insert_rest(DocCursor& cursor, const std::string& source);

};


//A view to edit the score in full page
class EditController : public Controller
{
protected:

public:

    EditController(LibraryScope& libraryScope, Document* pDoc, UserCommandExecuter* pExec);
    virtual ~EditController();


};



}   //namespace lenmus

#endif      //__LML_CONTROLLER_H__
