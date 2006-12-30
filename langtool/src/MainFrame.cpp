//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
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
#include "wx/filename.h"
#include "wx/dir.h"

#include "MainFrame.h"
#include "installer.h"
//#include "parser.h"
#include "html_converter.h"
#include "ebook_processor.h"
#include "DlgCompileBook.h"
#include "Paths.h"


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
    MENU_COMPILE_BOOK,
    MENU_GENERATE_PO,

};

// supported languages table
typedef struct lmLangDataStruct {
    wxString sLang;
    wxString sLangName;
    wxString sLangCode;
} lmLangData;

#define lmNUM_LANGUAGES 4
//table must be ordered by language name (in English) to
//ensure correspondence with table in DlgCompileBook.h
static const lmLangData tLanguages[lmNUM_LANGUAGES] = { 
    { _T("en"), _T("English"), _T("iso-8859-1") }, 
    { _T("fr"), _T("French"), _T("iso-8859-1") }, 
    { _T("es"), _T("Spanish"), _T("iso-8859-1") }, 
    { _T("tr"), _T("Turkish"), _T("iso-8859-9") }, 
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ltMainFrame, wxFrame)

    // File menu
    EVT_MENU(MENU_QUIT,  ltMainFrame::OnQuit)
    EVT_MENU(MENU_ABOUT, ltMainFrame::OnAbout)

    // eBooks menu
    EVT_MENU(MENU_COMPILE_BOOK, ltMainFrame::OnCompileBook)
    EVT_MENU(MENU_GENERATE_PO, ltMainFrame::OnGeneratePO)

    // Installer menu
    EVT_MENU(MENU_INSTALLER, ltMainFrame::OnInstaller)

END_EVENT_TABLE()

// ============================================================================
// main frame implementation
// ============================================================================

// frame constructor
ltMainFrame::ltMainFrame(const wxString& title, const wxString& sRootPath)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600,400))
{
    //save parameters
    m_sRootPath = sRootPath;
    m_sLenMusPath = _T("c:\\usr\\desarrollo_wx\\lenmus\\");

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



    // now append the freshly created menus to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(pFileMenu, _T("&File"));
    menuBar->Append(pBooksMenu, _T("&eBooks"));
    menuBar->Append(pInstMenu, _T("&Installer"));
    menuBar->Append(pHelpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

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
                wxT("Copyright (c) 2006 Cecilio Salmeron,\n")
                wxT("licenced under GNU GPL licence terms.\n"),
                wxVERSION_STRING, lmEbookProcessor::GetLibxml2Version().c_str() );

    wxMessageBox(msg, _T("About LangTool"), wxOK | wxICON_INFORMATION, this);
}

void ltMainFrame::OnInstaller(wxCommandEvent& WXUNUSED(event))
{
    // loop to generate the file for each language
    for (int i=0; i < lmNUM_LANGUAGES; i++) {
        GenerateLanguage(i);
    }
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

    ::wxBeginBusyCursor();
    lmEbookProcessor oEBP(0, m_pText);
    wxFileName oFSrc(sPath);
    oEBP.GenerateLMB(sPath, _T("en"), lmLANG_FILE);
    wxString sFolder = oFSrc.GetName();

    //create the PO files if they do not exist
    LogMessage(_T("Creating PO files:\n"));
    for (int i=0; i < lmNUM_LANGUAGES; i++)
    {
        wxString sLang =  tLanguages[i].sLang;
        wxFileName oFDest( g_pPaths->GetLocalePath() );
        oFDest.AppendDir(sLang);
        oFDest.SetName( oFSrc.GetName() + _T("_") + sLang );
        oFDest.SetExt(_T("po"));
        if (!oFDest.FileExists()) {     //if file does not exist
            LogMessage(_T("Creating PO file %s\n"), oFDest.GetFullName());
            wxString sCharset = tLanguages[i].sLangCode;
            wxString sLangName = tLanguages[i].sLangName;
            if (!oEBP.CreatePoFile(oFDest.GetFullPath(), sCharset, sLangName, sLang, sFolder)) {
                LogMessage(_T("Error: PO file can not be created\n"));
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
    wxString sLang = tLanguages[i].sLang;
    wxString sLangName = tLanguages[i].sLangName;

    pLocale->Init(_T(""), sLang, _T(""), true, true);
    pLocale->AddCatalogLookupPathPrefix( g_pPaths->GetLenMusLocalePath() + sLang );
    pLocale->AddCatalog(_T("lenmus_") + pLocale->GetName());

    wxString sContent = lmInstaller::GetInstallerStrings(sLang, sLangName);
    wxMessageBox(sContent);
    wxString sPath = sNil + _T(".\\") + sLang + _T(".nsh");
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

void ltMainFrame::OnCompileBook(wxCommandEvent& WXUNUSED(event))
{
    lmCompileBookOptions rOptions;
    rOptions.sSrcPath = wxEmptyString;
    rOptions.sDestPath = m_sLenMusPath + _T("books\\");

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
        if (rOptions.fLanguage[i]) {
            wxLocale* pLocale = (wxLocale*)NULL;
            wxString sLang = tLanguages[i].sLang;
            if (i != 0) {
                pLocale = new wxLocale();
                wxString sNil = _T("");
                wxString sLangName = tLanguages[i].sLangName;

                pLocale->Init(_T(""), sLang, _T(""), true, true);
                pLocale->AddCatalogLookupPathPrefix( g_pPaths->GetLocalePath() );
                wxString sCatalogName = sBookName + _T("_") + pLocale->GetName();
                pLocale->AddCatalog(sCatalogName);
                
                LogMessage(_T("Locale changed to %s language (using %s)."),
                            pLocale->GetName(), sCatalogName + _T(".mo\n") );
            }
            oEBP.GenerateLMB(rOptions.sSrcPath, sLang);

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

