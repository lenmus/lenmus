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

#ifndef __LML_MVC_BUILDER_H__
#define __LML_MVC_BUILDER_H__

#include <list>
#include <iostream>


using namespace std;

namespace lenmus
{

//forward declarations
class Document;
class DocCommandExecuter;
class UserCommandExecuter;
class Notification;
class MvcElement;
class View;
class LibraryScope;
class Controller;


//class MvcCollection. Responsible for managing the collection of MvcElemet objects.
class MvcCollection
{
protected:
    std::list<MvcElement*>    m_elements;

public:

    MvcCollection();
    ~MvcCollection();

    //add elements
    void add(MvcElement* pMvc);

    //remove elements
    void close_document(int iDoc);
    void close_document(Document* pDoc);

    //get elements
    MvcElement* get_mvc_element(int iDoc);
    MvcElement* get_mvc_element(Document* pDoc);

    //other
    void add_view(Document* pDoc, View* pView);
    void on_document_reloaded(Document* pDoc);

    //access to info
    //UserCommandExecuter* get_command_executer(Document* pDoc);
    int get_num_views(Document* pDoc);

    //for unit tests
    inline int get_num_documents() { return static_cast<int>(m_elements.size()); }
    Document* get_document(int iDoc);
    //UserCommandExecuter* get_command_executer(int iDoc);

};


// MvcBuilder: responsible for creating MvcElements
class MvcBuilder
{
protected:
    LibraryScope&   m_libScope;

public:
    MvcBuilder(LibraryScope& libraryScope);
    virtual ~MvcBuilder();

    enum { k_edit_view=0, };

    //MVC elements creation
    MvcElement* new_document(int viewType, const std::string& content="");
    MvcElement* open_document(int viewType, const std::string& filename);

};


//MvcElement: Facade object that define the library API to interact with the
//            MVC model for a Document
class MvcElement
{
protected:
    Document*           m_pDoc;
    std::list<View*>    m_views;
    void*               m_userData;
    UserCommandExecuter* m_pExec;
    void                (*m_callback)(Notification* event);

public:
    MvcElement(Document* pDoc, UserCommandExecuter* pExec, View* pView);
    virtual ~MvcElement();

    void close_document();
    void on_document_reloaded();

    //views management
    View* add_view();
    void delete_view(View* pView);
    inline int get_num_views() { return static_cast<int>( m_views.size() ); }
    View* get_view(int iView);

    //accessors
    inline Document* get_document() { return m_pDoc; }
    inline UserCommandExecuter* get_command_executer() { return m_pExec; }

    //to sent notifications to user application
    void set_callback( void (*pt2Func)(Notification* event) );
    void notify_user_application(Notification* event);

    //to save user data
    inline void set_user_data(void* pData) { m_userData = pData; }
    inline void* get_user_data() { return m_userData; }


        //score edition commands

    //insert a rest at current cursor position
    void insert_rest(View* pView, std::string source);


};

class Notification
{
protected:
    MvcElement* m_pMvcElement;
    Document*   m_pDoc;
    View*       m_pView;

public:
    Notification() 
        : m_pMvcElement(NULL), m_pDoc(NULL), m_pView(NULL)
    {
    }

    Notification(MvcElement* pMvc, Document* pDoc, View* pView) 
        : m_pMvcElement(pMvc), m_pDoc(pDoc), m_pView(pView) 
    {
    }

    //getters and setters
    inline View* get_view() { return m_pView; }
    inline Document* get_document() { return m_pDoc; }
    inline MvcElement* get_mvc_element() { return m_pMvcElement; }
    inline void set_view(View* pView) { m_pView = pView; }
    inline void set_document(Document* pDoc) { m_pDoc = pDoc; }
    inline void set_mvc_element(MvcElement* pMvc) { m_pMvcElement = pMvc; }

};


}   //namespace lenmus

#endif      //__LML_MVC_BUILDER_H__
