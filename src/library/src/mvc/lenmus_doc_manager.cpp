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

#include "lenmus_doc_manager.h"
#include "lenmus_document.h"
#include "lenmus_user_command.h"
#include "lenmus_view.h"

using namespace std;

namespace lenmus
{

//MvcContainer. Helper class to store pointers to all related objects around a Document
class MvcContainer
{
protected:
    std::list<View*>    m_views;    //views observing the document

public:

    MvcContainer() {}
    ~MvcContainer() {
        delete pUserCmdExec;
        delete pCmdExec;
        delete pDoc;
    }

    void add_view(View* pView);
    inline int get_num_views() { return static_cast<int>(m_views.size()); }

    Document*               pDoc;
    DocCommandExecuter*     pCmdExec;
    UserCommandExecuter*    pUserCmdExec;
};

void MvcContainer::add_view(View* pView)
{
    m_views.push_back(pView);
}


//------------------------------------------------------------------------------
//MvcCollection implementation
//------------------------------------------------------------------------------

MvcCollection::MvcCollection()
{
} 

MvcCollection::~MvcCollection()
{
    //delete documents
    std::list<MvcContainer*>::iterator it;
    for (it=m_documents.begin(); it != m_documents.end(); ++it)
    {
        delete *it;
    }
    m_documents.clear();
}

Document* MvcCollection::get_document(int iDoc)
{
    MvcContainer* pData = get_document_data(iDoc);
    return pData->pDoc;
}

UserCommandExecuter* MvcCollection::get_command_executer(int iDoc)
{
    MvcContainer* pData = get_document_data(iDoc);
    return pData->pUserCmdExec;
}

MvcContainer* MvcCollection::get_document_data(int iDoc)
{
    std::list<MvcContainer*>::iterator it;
    int i = 0;
    for (it=m_documents.begin(); it != m_documents.end() && i != iDoc; ++it, ++i);
    if (i == iDoc)
        return *it;
    else
        throw "invalid index";
}

MvcContainer* MvcCollection::get_document_data(Document* pDoc)
{
    std::list<MvcContainer*>::iterator it;
    for (it=m_documents.begin(); it != m_documents.end(); ++it)
    {
        if (pDoc == (*it)->pDoc)
            return *it;
    }
    throw "invalid pointer";
}

void MvcCollection::add(MvcContainer* pData)
{
    m_documents.push_back(pData);
}

void MvcCollection::close_document(int iDoc)
{
    std::list<MvcContainer*>::iterator it;
    int i = 0;
    for (it=m_documents.begin(); it != m_documents.end() && i != iDoc; ++it, ++i);
    if (iDoc == i)
    {
        delete *it;
        m_documents.erase(it);
    }
    else
        throw "invalid index";
}

void MvcCollection::close_document(Document* pDoc)
{
    std::list<MvcContainer*>::iterator it;
    for (it=m_documents.begin(); it != m_documents.end(); ++it)
    {
        if (pDoc == (*it)->pDoc)
        {
            delete *it;
            m_documents.erase(it);
            break;
        }
    }
}

UserCommandExecuter* MvcCollection::get_command_executer(Document* pDoc)
{
    MvcContainer* pData = get_document_data(pDoc);
    return pData->pUserCmdExec;
}

void MvcCollection::add_view(Document* pDoc, View* pView)
{
    MvcContainer* pData = get_document_data(pDoc);
    pData->add_view(pView);
}

int MvcCollection::get_num_views(Document* pDoc)
{
    MvcContainer* pData = get_document_data(pDoc);
    return pData->get_num_views();
}


//------------------------------------------------------------------------------
//MvcBuilder implementation
//------------------------------------------------------------------------------

MvcBuilder::MvcBuilder(MvcCollection& docviews, ostream& reporter)
    : m_docviews(docviews)
    , m_reporter(reporter)
{
} 

MvcBuilder::~MvcBuilder()
{
}

Document* MvcBuilder::new_document()
{
    MvcContainer* pData = new MvcContainer;
    pData->pDoc = new Document(m_reporter);
    pData->pCmdExec = new DocCommandExecuter(pData->pDoc);
    pData->pUserCmdExec = new UserCommandExecuter(pData->pCmdExec);
    m_docviews.add(pData);
    return pData->pDoc;
}

Document* MvcBuilder::open_document(const std::string& filename)
{
    MvcContainer* pData = new MvcContainer;
    pData->pDoc = new Document(filename, m_reporter);
    pData->pCmdExec = new DocCommandExecuter(pData->pDoc);
    pData->pUserCmdExec = new UserCommandExecuter(pData->pCmdExec);
    m_docviews.add(pData);
    return pData->pDoc;
}


}  //namespace lenmus
