//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
//    Copyright (c) 2002-2009 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program. If not, see <http://www.gnu.org/licenses/>. 
//
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
#include "wx/filename.h"
#include "wx/dir.h"

#include "MainFrame.h"
#include "installer.h"
#include "ebook_processor.h"
#include "help_processor.h"
#include "DlgCompileBook.h"
#include "DlgRunAgdoc.h"
#include "Paths.h"
#include "agdoc/ProjectProcessor.h"
#include "agdoc/IndexStorage.h"

//global variable
ltMainFrame* g_pMainFrame = (ltMainFrame*)NULL;

// IDs for the controls and the menu commands
enum
{
    // menu items
    MENU_QUIT = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    MENU_ABOUT = wxID_ABOUT,

    // Installer menu
    MENU_INSTALLER = wxID_HIGHEST + 100,

    // eBooks menu
    MENU_COMPILE_HELP,
    MENU_COMPILE_BOOK,
    MENU_GENERATE_PO,
    MENU_RUN_AGDOC,

    // options menu
    MENU_UTF8,

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ltMainFrame, wxFrame)

    // File menu
    EVT_MENU(MENU_QUIT,  ltMainFrame::OnQuit)
    EVT_MENU(MENU_ABOUT, ltMainFrame::OnAbout)

    // eBooks menu
    EVT_MENU(MENU_COMPILE_HELP, ltMainFrame::OnCompileHelp)
    EVT_MENU(MENU_COMPILE_BOOK, ltMainFrame::OnCompileBook)
    EVT_MENU(MENU_GENERATE_PO, ltMainFrame::OnGeneratePO)
    EVT_MENU(MENU_RUN_AGDOC, ltMainFrame::OnRunAgdoc)

    // Installer menu
    EVT_MENU(MENU_INSTALLER, ltMainFrame::OnInstaller)

    // Options menu
    EVT_MENU(MENU_UTF8, ltMainFrame::OnToggleUTF8)

END_EVENT_TABLE()

// ============================================================================
// main frame implementation
// ============================================================================

// frame constructor
ltMainFrame::ltMainFrame(const wxString& title, const wxString& sRootPath)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600,400))
    , m_sLastAgdocProject(_T(""))
    , m_sRootPath(sRootPath)
    , m_sLenMusPath(_T("c:\\usr\\desarrollo_wx\\lenmus\\"))
{
    // set the frame icon
    SetIcon(wxICON(LangTool));

	// create the wxTextCtrl 
	m_pText = new wxTextCtrl(this, -1, 
        _T("This program is an utility to compile eMusicBooks and to\n")
        _T("create and to manage Lang translation files\n\n"),
		wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxHSCROLL | wxTE_DONTWRAP);
	m_pText->SetBackgroundColour(*wxWHITE);

    // create a menu bar

    // the Help menu
    wxMenu* pHelpMenu = new wxMenu;
    pHelpMenu->Append(MENU_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

    // the File menu
    wxMenu* pFileMenu = new wxMenu;
    pFileMenu->Append(MENU_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // the Installer menu
    wxMenu* pInstMenu = new wxMenu;
    pInstMenu->Append(MENU_INSTALLER, _T("&Generate installer"), _T("Generate 'Installer' strings"));

    // items in the eBooks menu
    wxMenu* pBooksMenu = new wxMenu;
    pBooksMenu->Append(MENU_COMPILE_BOOK, _T("&Compile eBook"), _T("Convert eBook to LMB format"));
    pBooksMenu->Append(MENU_GENERATE_PO, _T("&Generate .po file"), _T("Generate a Lang file for the eBook"));
    pBooksMenu->Append(MENU_COMPILE_HELP, _T("Compile &Help"), _T("Generate Help file"));
    pBooksMenu->Append(MENU_RUN_AGDOC, _T("Run &agdoc"), _T("Run agdoc"));

    // the Options menu
    m_pOptMenu = new wxMenu;
    m_pOptMenu->Append(MENU_UTF8, _T("&Generate utf-8"), 
                _T("Generate files in utf-8 encoding"), wxITEM_CHECK);


    // now append the freshly created menus to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(pFileMenu, _T("&File"));
    menuBar->Append(pBooksMenu, _T("&eBooks"));
    menuBar->Append(pInstMenu, _T("&Installer"));
    menuBar->Append(m_pOptMenu, _T("&Options"));
    menuBar->Append(pHelpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create status bar
    CreateStatusBar(2);
    SetStatusText((m_pOptMenu->IsChecked(MENU_UTF8) ? _T("utf-8") : _T("MB charset")), 1);

    //set global pointer
    g_pMainFrame = this;
}


// event handlers

void ltMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void ltMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("This program is part of the LenMus project (www.lenmus.org)\n\n")
                wxT("This program is an utility to compile eMusicBooks and to\n")
                wxT("create and to manage translation files\n\n")
                wxT("It uses the libxml2 library, a widely-used, powerful cross-platform\n")
                wxT("XML processor, written by Daniel Veillard for \n")
                wxT("the GNOME project. It is included in this program by using the\n")
                wxT("wxXml2 wrapper for wxWidgets, written by Francesco Montorsi.\n\n")
                wxT("This release is compiled with %s, \n")
                wxT("and libxml2 version %s.\n\n\n")
                wxT("Copyright (c) 2008 Cecilio Salmeron,\n")
                wxT("licenced under GNU GPL licence terms.\n"),
                wxVERSION_STRING, lmEbookProcessor::GetLibxml2Version().c_str() );

    wxMessageBox(msg, _T("About LangTool"), wxOK | wxICON_INFORMATION, this);
}

void ltMainFrame::OnToggleUTF8(wxCommandEvent& WXUNUSED(event))
{
    SetStatusText((m_pOptMenu->IsChecked(MENU_UTF8) ? _T("utf-8") : _T("MB charset")), 1);
}

void ltMainFrame::OnInstaller(wxCommandEvent& WXUNUSED(event))
{
    // loop to generate the file for each language
    for (int i=0; i < lmNUM_LANGUAGES; i++) {
        GenerateLanguage(i);
    }
}

void ltMainFrame::OnRunAgdoc(wxCommandEvent& WXUNUSED(event))
{
    //        printf("Usage: agdoc [-i] [-q] <agproj_file>\n"
    //               "       -i - Create Index Only\n"
    //               "       -q - Quick Mode\n");
    //        return 1;
    //    }
    //
    //    const char* project_name = find_file_name(argc, argv, 0);

    wxString sProjectName = m_sLastAgdocProject;
    ltDlgRunAgdoc oDlg(this, &sProjectName); 
    if ( oDlg.ShowModal() != wxID_OK || sProjectName.IsEmpty() )
        return;

    m_sLastAgdocProject = sProjectName;

    //extract source folder
    wxFileName oFN( sProjectName );
    wxString sProjectFolder = oFN.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    ::wxBeginBusyCursor();

    ::wxSetWorkingDirectory( sProjectFolder );
    LogMessage(_T("Moved to folder '%s'\n"), wxGetCwd());

    bool index_only = false;    //find_bool_arg(argc, argv, "i");
    bool quick_mode = false;    //find_bool_arg(argc, argv, "q");

    try
    {
        build_the_project(sProjectName.mb_str(wxConvUTF8), index_only, quick_mode);
    }
    catch(agdoc::file_not_found& e)
    {
        LogMessage(_T("FATAL ERROR: File not found: %s\n"), 
                   wxString::From8BitData( e.message().c_str() ));
    }
    catch(agdoc::parsing_exception& e)
    {
        LogMessage(_T("ERROR: Line %d: %s\n"), e.line_num(),
                    wxString::From8BitData( e.message().c_str() ));
        if(e.line_content().length())
        {
            LogMessage(_T("%s\n\n"), 
                wxString::From8BitData( e.line_content().c_str() ));
        }
    }
    LogMessage(_T("Project processed\n"));
    LogMessage(_T("=======================================================================\n\n"));
    ::wxEndBusyCursor();
}

void ltMainFrame::OnGeneratePO(wxCommandEvent& WXUNUSED(event))
{
    // ask for the file to covert
    wxString sFilter = wxT("*.xml");
    wxString sPath = ::wxFileSelector(_T("Choose the file to convert"),
                                        wxT(""),    //default path
                                        wxT(""),    //default filename
                                        wxT("xml"),    //default_extension
                                        sFilter,
                                        wxOPEN,        //flags
                                        this);
    if ( sPath.IsEmpty() ) return;

    //Create the .cpp file with all strings
    ::wxBeginBusyCursor();
    lmEbookProcessor oEBP(0, m_pText);
    wxFileName oFSrc(sPath);
    oEBP.GenerateLMB(sPath, _T("en"), _T("utf-8"), lmLANG_FILE);
    wxString sFolder = oFSrc.GetName();

    //Delete created .htm and .toc files
    wxFileName oFTmp( g_pPaths->GetLocalePath() );  
	oFTmp.AppendDir(_T("src"));	    // 'langtool/locale/src'
    oFTmp.SetName( sFolder );
    wxString sTmp = oFTmp.GetFullPath();
    //remove .htm files
    sTmp += _T("*.htm");
    wxString sHtmFile = wxFindFirstFile(sTmp);
    while ( !sHtmFile.empty() )
    {
        ::wxRemoveFile(sHtmFile);
        sHtmFile = wxFindNextFile();
    }
    //remove .toc file
    sTmp = oFTmp.GetFullPath();
    sTmp += _T(".toc");
    ::wxRemoveFile(sTmp);       

    //create the PO files if they do not exist
    LogMessage(_T("Creating PO files:\n"));
    for (int i=0; i < lmNUM_LANGUAGES; i++)
    {
        wxString sLang =  g_tLanguages[i].sLang;
        wxFileName oFDest( g_pPaths->GetLocalePath() );
        oFDest.AppendDir(sLang);
        oFDest.SetName( oFSrc.GetName() + _T("_") + sLang );
        oFDest.SetExt(_T("po"));
        if (!oFDest.FileExists()) 
        {   
            //if file does not exist
            LogMessage(_T("Creating PO file %s\n"), oFDest.GetFullName());
            wxString sCharset = g_tLanguages[i].sCharCode;
            wxString sLangName = g_tLanguages[i].sLangName;
            if (!oEBP.CreatePoFile(oFDest.GetFullPath(), sCharset, sLangName, sLang, sFolder)) {
                LogMessage(_T("*** Error: PO file can not be created\n"));
            }
        }
        else
            LogMessage(_T("Omitting: PO file %s already exists.\n"), oFDest.GetFullName());
    }
    LogMessage(_T("PO files created.\n\n"));

    ::wxEndBusyCursor();

}

void ltMainFrame::GenerateLanguage(int i)
{
    wxLocale* pLocale = new wxLocale();
    wxString sNil = _T("");
    wxString sLang = g_tLanguages[i].sLang;
    wxString sLangName = g_tLanguages[i].sLangName;

    pLocale->Init(_T(""), sLang, _T(""), true, true);
    wxString sPath = g_pPaths->GetLenMusLocalePath();
    pLocale->AddCatalogLookupPathPrefix( sPath );
    wxString sCatalog = _T("lenmus_") + pLocale->GetName();
    if (! pLocale->AddCatalog( sCatalog )) {
        wxMessageBox(wxString::Format(_T("Fail adding catalog %s"), sCatalog));
    }
    wxString sContent = lmInstaller::GetInstallerStrings(sLang, sLangName);
    wxMessageBox(sContent);
    sPath = sNil + _T(".\\") + sLang + _T(".nsh");
    PutContentIntoFile(sPath, sContent);

    delete pLocale;

}

void ltMainFrame::PutContentIntoFile(wxString sPath, wxString sContent)
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

void ltMainFrame::OnCompileHelp(wxCommandEvent& WXUNUSED(event))
{
    lmCompileBookOptions rOptions;
    rOptions.sSrcPath = wxEmptyString;
    rOptions.sDestPath = m_sLenMusPath + _T("locale\\");

    lmDlgCompileBook oDlg(this, &rOptions); 
    int retcode = oDlg.ShowModal();
    if (retcode != wxID_OK) {
        return;
    }

    if ( rOptions.sSrcPath.IsEmpty() ) return;
    //extract source folder
    wxFileName oFSrc( rOptions.sSrcPath );
    wxString sSrcFolder = oFSrc.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    //Get help file name
    wxFileName oFN(rOptions.sSrcPath);
    const wxString sBookName = oFN.GetName();
    LogMessage(_T("Preparing to process help file. Sources: '%s'\n"), rOptions.sSrcPath );

    ::wxBeginBusyCursor();
    int nDbgOpt = 0;
    if (rOptions.fDump) nDbgOpt |= eDumpTree;
    if (rOptions.fLogTree) nDbgOpt |= eLogTree;
    ltHelpProcessor oHP(nDbgOpt, m_pText);

    //Loop to use each selected language
    for(int i=0; i < eLangLast; i++) 
    {
        if (rOptions.fLanguage[i])
        {
            wxLocale* pLocale = (wxLocale*)NULL;
            wxString sLang = g_tLanguages[i].sLang;
            wxString sCharCode = g_tLanguages[i].sCharCode;
            if (i != 0)
            {
                pLocale = new wxLocale();
                wxString sNil = _T("");
                wxString sLangName = g_tLanguages[i].sLangName;

                pLocale->Init(_T(""), sLang, _T(""), true, true);
                pLocale->AddCatalogLookupPathPrefix( g_pPaths->GetLocalePath() );
                wxString sCatalogName = sBookName + _T("_") + pLocale->GetName();
                pLocale->AddCatalog(sCatalogName);
                
                LogMessage(_T("Locale changed to %s language (using %s)."),
                            pLocale->GetName(), sCatalogName + _T(".mo\n") );
            }
            if (m_pOptMenu->IsChecked(MENU_UTF8)) sCharCode = _T("utf-8");
            oHP.GenerateHelpFile(sSrcFolder, sLang, sCharCode);

            if (i != 0) delete pLocale;
        }
    }
    ::wxEndBusyCursor();

}

void ltMainFrame::OnCompileBook(wxCommandEvent& WXUNUSED(event))
{
    lmCompileBookOptions rOptions;
    rOptions.sSrcPath = wxEmptyString;
    rOptions.sDestPath = m_sLenMusPath + _T("locale\\");

    lmDlgCompileBook oDlg(this, &rOptions); 
    int retcode = oDlg.ShowModal();
    if (retcode != wxID_OK) {
        return;
    }

    if ( rOptions.sSrcPath.IsEmpty() ) return;

    //Get book name
    wxFileName oFN(rOptions.sSrcPath);
    const wxString sBookName = oFN.GetName();
    LogMessage(_T("Preparing to process eMusicBook %s\n"), rOptions.sSrcPath );

    ::wxBeginBusyCursor();
    int nDbgOpt = 0;
    if (rOptions.fDump) nDbgOpt |= eDumpTree;
    if (rOptions.fLogTree) nDbgOpt |= eLogTree;
    lmEbookProcessor oEBP(nDbgOpt, m_pText);
    //Loop to use each selected language
    for(int i=0; i < eLangLast; i++) 
    {
        if (rOptions.fLanguage[i])
        {
            wxLocale* pLocale = (wxLocale*)NULL;
            wxString sLang = g_tLanguages[i].sLang;
            wxString sCharCode = g_tLanguages[i].sCharCode;
            if (i != 0)
            {
                pLocale = new wxLocale();
                wxString sNil = _T("");
                wxString sLangName = g_tLanguages[i].sLangName;

                pLocale->Init(_T(""), sLang, _T(""), true, true);
                pLocale->AddCatalogLookupPathPrefix( g_pPaths->GetLocalePath() );
                wxString sCatalogName = sBookName + _T("_") + pLocale->GetName();
                pLocale->AddCatalog(sCatalogName);
                
                LogMessage(_T("Locale changed to %s language (using %s)."),
                            pLocale->GetName(), sCatalogName + _T(".mo\n") );
            }
            if (m_pOptMenu->IsChecked(MENU_UTF8)) sCharCode = _T("utf-8");
            oEBP.GenerateLMB(rOptions.sSrcPath, sLang, sCharCode);

            if (i != 0) delete pLocale;
        }
    }
    ::wxEndBusyCursor();

}

void ltMainFrame::LogMessage(const wxChar* szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxString sMsg = wxString::FormatV(szFormat, argptr);
    m_pText->AppendText(sMsg);
    va_end(argptr);

}


//----------------------------------------------------------------------
void ltMainFrame::write_index(const char* file_name, 
                 agdoc::content_storage& index, 
                 agdoc::log_file& log)
{
    agdoc::string_type index_name(file_name);
    index_name.append(agdoc::keyword_index_ext.name, agdoc::keyword_index_ext.len);
    agdoc::write_content_storage(index_name.c_str(), index);
    log.write("Wrote index file: '%s'", index_name.c_str());
}



////----------------------------------------------------------------------
//bool ltMainFrame::find_bool_arg(int argc, char* argv[], const char* arg)
//{
//    int i;
//    for(i = 1; i < argc; ++i)
//    {
//        if(argv[i][0] == '-' && strcmp(argv[i] + 1, arg) == 0) return true;
//    }
//    return false;
//}



//----------------------------------------------------------------------
const char* ltMainFrame::find_file_name(int argc, char* argv[], int idx)
{
    int i;
    int file_name_idx = 0;
    for(i = 1; i < argc; ++i)
    {
        if(argv[i][0] != '-')
        {
            if(file_name_idx == idx) return argv[i];
            ++file_name_idx;
        }
    }
    return 0;
}



//----------------------------------------------------------------------
void ltMainFrame::build_the_project(const char* project_name, 
                       bool index_only,
                       bool quick_mode)
{
    agdoc::log_file log;
    agdoc::string_type pname = agdoc::from_ascii(project_name);

    if(quick_mode)
    {
        agdoc::string_type index_name(pname);
        index_name.append(agdoc::keyword_index_ext.name, agdoc::keyword_index_ext.len);

        if(agdoc::name_stat(index_name.c_str()) == agdoc::file_exists)
        {
            agdoc::ifile index_src(index_name.c_str());
            agdoc::index_storage idx(index_src.elements());

            agdoc::project_processor prj(pname.c_str(), 
                                         log, 
                                         &idx, 
                                         &(index_src.elements()),
                                         true);
            return;
        }
    }

    agdoc::project_processor prj1(pname.c_str(), log);
    write_index(pname.c_str(), prj1.index_content(), log);

    agdoc::index_storage idx(prj1.index_content().elements());

    if(!index_only)
    {
        agdoc::project_processor prj2(pname.c_str(), 
                                      log, 
                                      &idx, 
                                      &(prj1.index_content().elements()),
                                      quick_mode);
    }
}

//=====================================================================================
// Global functions
//=====================================================================================

void ltLogMessage(const char* msg)
{
    g_pMainFrame->LogMessage( wxString::From8BitData( msg ) );
    //g_pMainFrame->LogMessage( _T("\n") );
}

void ltLogMessage(const char* msg, const char* arg1)
{
    g_pMainFrame->LogMessage( wxString::From8BitData( msg ),
                              wxString::From8BitData( arg1 ) );
    g_pMainFrame->LogMessage( _T("\n") );
}

void ltLogMessage(const char* msg, const char* arg1, const char* arg2)
{
    g_pMainFrame->LogMessage( wxString::From8BitData( msg ),
                              wxString::From8BitData( arg1 ),
                              wxString::From8BitData( arg2 ) );
    g_pMainFrame->LogMessage( _T("\n") );
}

void ltLogDbg(wxString& msg)
{
    wxLogMessage(msg);
}

