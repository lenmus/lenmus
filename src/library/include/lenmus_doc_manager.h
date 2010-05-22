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

#ifndef __LM_DOC_MANAGER_H__
#define __LM_DOC_MANAGER_H__

#include <list>
#include <iostream>


using namespace std;

namespace lenmus
{

//forward declarations
class Document;
class DocCommandExecuter;
class UserCommandExecuter;
class MvcContainer;
class View;


//class MvcCollection. Responsible for managing the collection of MvcContainer objects.
class MvcCollection
{
protected:
    std::list<MvcContainer*>    m_documents;

public:

    MvcCollection();
    ~MvcCollection();

    void add(MvcContainer* pData);
    void close_document(int iDoc);
    void close_document(Document* pDoc);

    //collection management
    void add_view(Document* pDoc, View* pView);

    //access to info
    UserCommandExecuter* get_command_executer(Document* pDoc);
    int get_num_views(Document* pDoc);

    //for unit tests
    inline int get_num_documents() { return static_cast<int>(m_documents.size()); }
    Document* get_document(int iDoc);
    UserCommandExecuter* get_command_executer(int iDoc);

protected:
    MvcContainer* get_document_data(int iDoc);
    MvcContainer* get_document_data(Document* pDoc);

};


class MvcBuilder
{
protected:
    ostream&        m_reporter;
    MvcCollection&  m_docviews;

public:
    MvcBuilder(MvcCollection& docviews, ostream& reporter=cout);
    virtual ~MvcBuilder();

    //document creation
    Document* new_document();
    Document* open_document(const std::string& filename);

};


}   //namespace lenmus

#endif      //__LM_DOC_MANAGER_H__
