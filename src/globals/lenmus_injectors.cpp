//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#include "lenmus_injectors.h"

#include "lenmus_config.h"
#include "lenmus_paths.h"
#include "lenmus_midi_server.h"

// to save config information into a file
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/filename.h>

#include <sstream>

using namespace std;

namespace lenmus
{

//=======================================================================================
// ApplicationScope implementation
//=======================================================================================
ApplicationScope::ApplicationScope(ostream& reporter)
    : m_reporter(reporter)
    , m_pPaths(NULL)            //lazzy instantiation. Singleton scope.
    , m_pPrefs(NULL)            //lazzy instantiation. Singleton scope.
    , m_pMidi(NULL)             //lazzy instantiation. Singleton scope.
    , m_sAppName(_T(LENMUS_APP_NAME))
    , m_sVendorName(_T(LENMUS_VENDOR_NAME))
{
    set_version_string();
}

//---------------------------------------------------------------------------------------
ApplicationScope::~ApplicationScope()
{
    delete m_pPaths;
    delete m_pMidi;

    //Must be the last object as any previous object could need it to save user options
    delete m_pPrefs;
}

//---------------------------------------------------------------------------------------
void ApplicationScope::set_version_string()
{
    //examples: "5.0.a0", "5.0.b2", "5.1", "5.1.2"

    int major = LENMUS_VERSION_MAJOR;
    int minor = LENMUS_VERSION_MINOR;
    int patch = LENMUS_VERSION_PATCH;
    wxString state = _T(LENMUS_VERSION_STATE);
    if (state.empty())
    {
        if (patch == 0)
            m_sVersionString = wxString::Format(_T("%d.%d"), major, minor);
        else
            m_sVersionString = wxString::Format(_T("%d.%d.%d"), major, minor, patch);
    }
    else
    {
        m_sVersionString = wxString::Format(_T("%d.%d.%s%d"), major, minor,
                                            state.c_str(), patch);
    }
}

//---------------------------------------------------------------------------------------
wxString ApplicationScope::get_app_full_name()
{
    //i.e. "Lenmus Phonascus v5.0 alpha 0"

    wxString name = get_app_name();
    name += _T(" v");

    int major = LENMUS_VERSION_MAJOR;
    int minor = LENMUS_VERSION_MINOR;
    int patch = LENMUS_VERSION_PATCH;
    wxString state = _T(LENMUS_VERSION_STATE);
    if (state.empty())
    {
        name += get_version_string();
    }
    else
    {
        if (state == _T("a"))
            name += wxString::Format(_T("%d.%d alpha %d"), major, minor, patch);
        else if (state == _T("b"))
            name += wxString::Format(_T("%d.%d beta %d"), major, minor, patch);
        else
            name += get_version_string();
    }
    return name;
}

//---------------------------------------------------------------------------------------
Paths* ApplicationScope::get_paths()
{
    if (!m_pPaths)
        m_pPaths = new Paths( wxGetCwd() );
    return m_pPaths;
}

//---------------------------------------------------------------------------------------
MidiServer* ApplicationScope::get_midi_server()
{
    if (!m_pMidi)
        m_pMidi = new MidiServer(*this);
    return m_pMidi;
}

//---------------------------------------------------------------------------------------
wxConfigBase* ApplicationScope::get_preferences()
{
    if (!m_pPrefs)
        create_preferences_object();
    return m_pPrefs;
}

//---------------------------------------------------------------------------------------
void ApplicationScope::set_bin_folder(const wxString& sBinPath)
{
    delete m_pPaths;
    m_pPaths = new Paths(sBinPath);

    create_preferences_object();
}

//---------------------------------------------------------------------------------------
void ApplicationScope::create_preferences_object()
{
    Paths* pPaths = get_paths();
    wxString path = pPaths->GetConfigPath();
    wxFileName filename(path, _T("lenmus"), _T("ini") );

    //wxString initfile = filename.GetFullPath();
    delete m_pPrefs;
    wxConfigBase::Set((wxConfigBase*) NULL);
    wxFileConfig *pConfig =
        new wxFileConfig(_T("lenmus"), _T("LenMus"), filename.GetFullPath(),
                         _T("lenmus"), wxCONFIG_USE_LOCAL_FILE );
    wxConfigBase::Set(pConfig);
    m_pPrefs = wxConfigBase::Get();

    //force writing back the default values just in case they're not present
    pConfig->SetRecordDefaults();
}




//=======================================================================================
// Injector implementation
//=======================================================================================
//LdpParser* Injector::inject_LdpParser(ApplicationScope& libraryScope,
//                                      DocumentScope& documentScope)
//{
//    return new LdpParser(documentScope.default_reporter(),
//                         libraryScope.ldp_factory());
//}
//
////---------------------------------------------------------------------------------------
//Analyser* Injector::inject_Analyser(ApplicationScope& libraryScope,
//                                    DocumentScope& documentScope)
//{
//    return new Analyser(documentScope.default_reporter(), libraryScope);
//}
//
////---------------------------------------------------------------------------------------
//ModelBuilder* Injector::inject_ModelBuilder(DocumentScope& documentScope)
//{
//    return new ModelBuilder(documentScope.default_reporter());
//}
//
////---------------------------------------------------------------------------------------
//LdpCompiler* Injector::inject_LdpCompiler(ApplicationScope& libraryScope,
//                                          DocumentScope& documentScope)
//{
//    return new LdpCompiler(inject_LdpParser(libraryScope, documentScope),
//                           inject_Analyser(libraryScope, documentScope),
//                           inject_ModelBuilder(documentScope),
//                           documentScope.id_assigner() );
//}
//
////---------------------------------------------------------------------------------------
//Document* Injector::inject_Document(ApplicationScope& libraryScope)
//{
//    return new Document(libraryScope);
//}
//
////---------------------------------------------------------------------------------------
//ScreenDrawer* Injector::inject_ScreenDrawer(ApplicationScope& libraryScope)
//{
//    return new ScreenDrawer(libraryScope);
//}
//
//////---------------------------------------------------------------------------------------
////UserCommandExecuter* Injector::inject_UserCommandExecuter(Document* pDoc)
////{
////    return new UserCommandExecuter(pDoc);
////}
//
////---------------------------------------------------------------------------------------
//SimpleView* Injector::inject_SimpleView(ApplicationScope& libraryScope, Document* pDoc)  //UserCommandExecuter* pExec)
//{
//    return dynamic_cast<SimpleView*>(
//                        inject_View(libraryScope,
//                                    ViewFactory::k_view_simple,
//                                    pDoc)
//                       );
//}
//
////---------------------------------------------------------------------------------------
//VerticalBookView* Injector::inject_VerticalBookView(ApplicationScope& libraryScope,
//                                                    Document* pDoc)  //UserCommandExecuter* pExec)
//{
//    return dynamic_cast<VerticalBookView*>(
//                        inject_View(libraryScope,
//                                    ViewFactory::k_view_vertical_book,
//                                    pDoc)
//                       );
//}
//
////---------------------------------------------------------------------------------------
//HorizontalBookView* Injector::inject_HorizontalBookView(ApplicationScope& libraryScope,
//                                                        Document* pDoc)  //UserCommandExecuter* pExec)
//{
//    return dynamic_cast<HorizontalBookView*>(
//                        inject_View(libraryScope,
//                                    ViewFactory::k_view_horizontal_book,
//                                    pDoc)
//                       );
//}
//
////---------------------------------------------------------------------------------------
//View* Injector::inject_View(ApplicationScope& libraryScope, int viewType, Document* pDoc)
//                            //UserCommandExecuter* pExec)
//{
//    ScreenDrawer* pDrawer = Injector::inject_ScreenDrawer(libraryScope);
//    return ViewFactory::create_view(libraryScope, viewType, pDrawer);
//}
//
////---------------------------------------------------------------------------------------
//Interactor* Injector::inject_Interactor(ApplicationScope& libraryScope,
//                                        Document* pDoc, View* pView) //, UserCommandExecuter* pExec)
//{
//    //factory method
//
//    return new EditInteractor(libraryScope, pDoc, pView);  //, pExec);
//}
//
////---------------------------------------------------------------------------------------
//Presenter* Injector::inject_Presenter(ApplicationScope& libraryScope,
//                                      int viewType, Document* pDoc)
//{
//    //UserCommandExecuter* pExec = Injector::inject_UserCommandExecuter(pDoc);
//    View* pView = Injector::inject_View(libraryScope, viewType, pDoc); //, pExec);
//    Interactor* pInteractor = Injector::inject_Interactor(libraryScope, pDoc, pView);
//    pView->set_interactor(pInteractor);
//    return new Presenter(pDoc, pInteractor);  //, pExec);
//}
//
////---------------------------------------------------------------------------------------
//Task* Injector::inject_Task(int taskType, Interactor* pIntor)
//{
//    return TaskFactory::create_task(taskType, pIntor);
//}



}  //namespace lenmus
