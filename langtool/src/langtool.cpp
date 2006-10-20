//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wfstream.h"

#include "installer.h"
#include "ebook_processor.h"
#include "parser.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnInstaller(wxCommandEvent& event);
    void OnSplitFile(wxCommandEvent& WXUNUSED(event));

private:
    void PutContentIntoFile(wxString sPath, wxString sContent);
    void GenerateLanguage(int i);

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    MENU_QUIT = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    MENU_ABOUT = wxID_ABOUT,

    MENU_INSTALLER = wxID_HIGHEST + 100,
    MENU_SPLIT_FILE,
};

// supported languages table
typedef struct lmLangDataStruct {
    wxString sLang;
    wxString sLangName;
} lmLangData;

#define lmNUM_LANGUAGES 3
static const lmLangData tLanguages[lmNUM_LANGUAGES] = { 
    { _T("en"), _T("English") }, 
    { _T("es"), _T("Spanish") }, 
    { _T("fr"), _T("French") }, 
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(MENU_QUIT,  MyFrame::OnQuit)
    EVT_MENU(MENU_ABOUT, MyFrame::OnAbout)
    EVT_MENU(MENU_INSTALLER, MyFrame::OnInstaller)
    EVT_MENU(MENU_SPLIT_FILE, MyFrame::OnSplitFile)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("Minimal wxWidgets App"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(LangTool));

    // create a menu bar

    // the Help menu
    wxMenu* pHelpMenu = new wxMenu;
    pHelpMenu->Append(MENU_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

    // the File menu
    wxMenu* pFileMenu = new wxMenu;
    pFileMenu->Append(MENU_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // the Generate menu
    wxMenu* pGenMenu = new wxMenu;
    pGenMenu->Append(MENU_INSTALLER, _T("&Installer"), _T("Generate 'Installer' strings"));

    // the Split File menu
    wxMenu* pSplitMenu = new wxMenu;
    pSplitMenu->Append(MENU_SPLIT_FILE, _T("&Split"), _T("Split HTML ebook file"));


    // now append the freshly created menus to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(pFileMenu, _T("&File"));
    menuBar->Append(pGenMenu, _T("&Generate"));
    menuBar->Append(pSplitMenu, _T("&Split"));
    menuBar->Append(pHelpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the LangTool utility.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About LanfTool"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnInstaller(wxCommandEvent& WXUNUSED(event))
{
    // loop to generate the file for each language
    for (int i=0; i < lmNUM_LANGUAGES; i++) {
        GenerateLanguage(i);
    }
}

void MyFrame::OnSplitFile(wxCommandEvent& WXUNUSED(event))
{
    // ask for the file to split
    //wxString sFilter = wxT("*.*");
    //wxString sPath = ::wxFileSelector(_T("Choose the file to split"),
    //                                    wxT(""),    //default path
    //                                    wxT(""),    //default filename
    //                                    wxT("htm"),    //default_extension
    //                                    sFilter,
    //                                    wxOPEN,        //flags
    //                                    this);
    wxString sPath = ::wxDirSelector(_T("Choose a folder"));
    if ( sPath.IsEmpty() ) return;

    lmXmlParser* pParser = new lmXmlParser();
    pParser->ParseBook(sPath);
    delete pParser;

}


void MyFrame::GenerateLanguage(int i)
{
    wxLocale* pLocale = new wxLocale();
    wxString sNil = _T("");
    wxString sLang = tLanguages[i].sLang;
    wxString sLangName = tLanguages[i].sLangName;

    pLocale->Init(_T(""), sLang, _T(""), true, true);
    pLocale->AddCatalogLookupPathPrefix( _T("c:\\usr\\desarrollo_wx\\lenmus\\locale\\") + sLang );
    pLocale->AddCatalog(_T("lenmus_") + pLocale->GetName());

    wxString sContent = lmInstaller::GetInstallerStrings(sLang, sLangName);
    wxMessageBox(sContent);
    wxString sPath = sNil + _T(".\\") + sLang + _T(".nsh");
    PutContentIntoFile(sPath, sContent);

    delete pLocale;

}

void MyFrame::PutContentIntoFile(wxString sPath, wxString sContent)
{
    wxFile* pFile = new wxFile(sPath, wxFile::write);
    if (!pFile->IsOpened()) {
        //todo
        pFile = (wxFile*)NULL;
        return;
    }
    pFile->Write(sContent);
    pFile->Close();
    delete pFile;


}

