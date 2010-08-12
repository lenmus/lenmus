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

#include "lenmus_injectors.h"
#include "lenmus_mvc_builder.h"
#include "lenmus_document.h"
#include "lenmus_user_command.h"
#include "lenmus_view.h"

using namespace std;

namespace lenmus
{

//------------------------------------------------------------------------------
//MvcCollection implementation
//------------------------------------------------------------------------------

MvcCollection::MvcCollection()
{
} 

MvcCollection::~MvcCollection()
{
    //delete elements
    std::list<MvcElement*>::iterator it;
    for (it=m_elements.begin(); it != m_elements.end(); ++it)
    {
        delete *it;
    }
    m_elements.clear();
}

Document* MvcCollection::get_document(int iDoc)
{
    MvcElement* pMvc = get_mvc_element(iDoc);
    return pMvc->get_document();
}

UserCommandExecuter* MvcCollection::get_command_executer(int iDoc)
{
    MvcElement* pMvc = get_mvc_element(iDoc);
    return pMvc->get_command_executer();
}

MvcElement* MvcCollection::get_mvc_element(int iDoc)
{
    std::list<MvcElement*>::iterator it;
    int i = 0;
    for (it=m_elements.begin(); it != m_elements.end() && i != iDoc; ++it, ++i);
    if (i == iDoc)
        return *it;
    else
        throw "invalid index";
}

MvcElement* MvcCollection::get_mvc_element(Document* pDoc)
{
    std::list<MvcElement*>::iterator it;
    for (it=m_elements.begin(); it != m_elements.end(); ++it)
    {
        if (pDoc == (*it)->get_document())
            return *it;
    }
    throw "invalid pointer";
}

void MvcCollection::add(MvcElement* pMvc)
{
    m_elements.push_back(pMvc);
}

void MvcCollection::close_document(int iDoc)
{
    std::list<MvcElement*>::iterator it;
    int i = 0;
    for (it=m_elements.begin(); it != m_elements.end() && i != iDoc; ++it, ++i);
    if (iDoc == i)
    {
        delete *it;
        m_elements.erase(it);
    }
    else
        throw "invalid index";
}

void MvcCollection::close_document(Document* pDoc)
{
    std::list<MvcElement*>::iterator it;
    for (it=m_elements.begin(); it != m_elements.end(); ++it)
    {
        if (pDoc == (*it)->get_document())
        {
            delete *it;
            m_elements.erase(it);
            break;
        }
    }
}

UserCommandExecuter* MvcCollection::get_command_executer(Document* pDoc)
{
    MvcElement* pMvc = get_mvc_element(pDoc);
    return pMvc->get_command_executer();
}

void MvcCollection::add_view(Document* pDoc, View* pView)
{
    //MvcElement* pMvc = get_mvc_element(pDoc);
    //pMvc->add_view(pView);
}

int MvcCollection::get_num_views(Document* pDoc)
{
    MvcElement* pMvc = get_mvc_element(pDoc);
    return pMvc->get_num_views();
}

void MvcCollection::on_document_reloaded(Document* pDoc)
{
    MvcElement* pMvc = get_mvc_element(pDoc);
    pMvc->on_document_reloaded();
}


//------------------------------------------------------------------------------
//MvcBuilder implementation
//------------------------------------------------------------------------------

MvcBuilder::MvcBuilder(LibraryScope& libraryScope)  //, MvcCollection& docviews)
    : m_libScope(libraryScope)
{
} 

MvcBuilder::~MvcBuilder()
{
}

MvcElement* MvcBuilder::new_document(int viewType, const std::string& content)
{
    Document* pDoc = Injector::inject_Document(m_libScope);
    if (content != "")
        pDoc->from_string(content);
    else
        pDoc->create_empty();

    EditView* pView = Injector::inject_EditView(pDoc);
    MvcElement* pMvc = new MvcElement(pDoc, pView);
    return pMvc;
}

MvcElement* MvcBuilder::open_document(int viewType, const std::string& filename)
{
    Document* pDoc = Injector::inject_Document(m_libScope);
    pDoc->from_file(filename);

    EditView* pView = Injector::inject_EditView(pDoc);
    MvcElement* pMvc = new MvcElement(pDoc, pView);
    return pMvc;
}


//------------------------------------------------------------------------------
//MvcElement implementation
//------------------------------------------------------------------------------

MvcElement::MvcElement(Document* pDoc, View* pView)
    : m_pDoc(pDoc)
    , m_callback(NULL)
    , m_userData(NULL)
{
    m_pUserCmdExec = Injector::inject_UserCommandExecuter(m_pDoc);
    m_views.push_back(pView);
    m_pDoc->add_observer(pView);
    pView->set_owner(this);
}

MvcElement::~MvcElement()
{
    std::list<View*>::iterator it;
    for (it=m_views.begin(); it != m_views.end(); ++it)
        delete *it;
    m_views.clear();

    delete m_pDoc;
    delete m_pUserCmdExec;
}

void MvcElement::close_document()
{
}

View* MvcElement::add_view()
{
    return NULL;
}

void MvcElement::delete_view(View* pView)
{
}

View* MvcElement::get_view(int iView)
{
    std::list<View*>::iterator it;
    int i = 0;
    for (it=m_views.begin(); it != m_views.end()&& i != iView; ++it, ++i);
    if (i == iView)
        return *it;
    else
        throw "invalid index";
}

void MvcElement::on_document_reloaded()
{
    std::list<View*>::iterator it;
    for (it=m_views.begin(); it != m_views.end(); ++it)
        (*it)->on_document_reloaded();
}

void MvcElement::notify_user_application(Notification* event)
{
    if (m_callback)
        m_callback(event);
}

void MvcElement::set_callback( void (*pt2Func)(Notification* event) )
{
    m_callback = pt2Func;
}

void MvcElement::insert_rest(View* pView, std::string source)
{
	EditController* pController = dynamic_cast<EditController*>( pView->get_controller() );
    if (pController)
        pController->insert_rest(pView->get_cursor(), source);
}

void EditController::insert_rest(DocCursor& cursor, std::string source)
{
    ////create rest
    //LdpParser* parser = Injector::inject_LdpParser( ? );
    //SpLdpTree tree = parser.parse_text(source);
    //Analyser a((cout, wxGetApp().app_scope().ldp_factory());
    //a.analyse_tree(tree);
    //LdpElement* pElm = tree->get_root();

    //create rest
    Document* pDoc = pMvcOwner->get_document();
    LdpElement* pElm = pDoc->parse_element(source);

    //prepare command and execute it
    UserCommandExecuter* pExec = pMvcOwner->get_command_executer();
    InsertUserCommand cmd(cursor, pElm);
    pExec->execute(cmd);

    //wxLogMessage( lmToWxString(pDoc->to_string_with_ids()) );
    //wxLogMessage( _T("ID=%d"), pElm->get_id()  );

    //place cursor after inserted object
    cursor.start_of_content();
    cursor.enter_element();         //enter score
    cursor.point_to( pElm->get_id() );
    cursor.move_next();

    pDoc->notify_that_document_has_been_modified();
}

}  //namespace lenmus
