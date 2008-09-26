//--------------------------------------------------------------------------------------
//    LenMus project: free software for music theory and language
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#include "wx/fs_zip.h"
#include "wx/xrc/xmlres.h"          // use the xrc resource system
#include "wx/cmdline.h"             // to parse command line arguments

#include "MainFrame.h"
#include "Paths.h"
#include "command.h"

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
    virtual bool OnInit();
    virtual int MyApp::OnExit();

    // command line
    wxArrayString *m_fnames;
    bool ProcessCmdLine (wxChar** argv, int argc = 0);

    bool m_fUseGUI;

};

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
    // set information about this application
    const wxString sAppName = _T("langtool");
    SetVendorName(_T("LenMus"));
    SetAppName(sAppName);

    // Add support for zip files
    wxFileSystem::AddHandler(new wxZipFSHandler);

    // Get program directory and set up global paths object
    #ifdef __WXMSW__
    // On Windows, the path to the LenMus program is in argv[0]
    wxString sHomeDir = wxPathOnly(argv[0]);
    #endif
    #ifdef __MACOSX__
    // On Mac OS X, the path to the LenMus program is in argv[0]
    wxString sHomeDir = wxPathOnly(argv[0]);
    #endif
    #ifdef __MACOS9__
    // On Mac OS 9, the initial working directory is the one that
    // contains the program.
    wxString sHomeDir = wxGetCwd();
    #endif
    wxFileName oRootPath(sHomeDir);     //sHomeDir is 'build' folder
    //oRootPath.RemoveLastDir();          //now we are in the langtool root
    g_pPaths = new lmPaths(sHomeDir);


    // Load all of the XRC files that will be used. You can put everything
    // into one giant XRC file if you wanted, but then they become more
    // difficult to manage, and harder to reuse in later projects.
    wxFileName oFN(oRootPath);
    oFN.AppendDir(_T("xrc"));
        // Initialize all the XRC handlers.
    wxXmlResource::Get()->InitAllHandlers();
        // The score generation settings dialog
    oFN.SetFullName(_T("DlgCompileBook.xrc"));
    wxXmlResource::Get()->Load( oFN.GetFullPath() );

    // get and process command line
    //m_fnames = new wxArrayString();
    ProcessCmdLine(argv, argc);

        // Create GUI

    if (m_fUseGUI) {
        // create the main application window
        ltMainFrame *frame = new ltMainFrame(_T("LangTool - eMusicBooks and Lang files processor"), sHomeDir);

        // and show it (the frames, unlike simple controls, are not shown when
        // created initially)
        frame->Show(true);

        // success: wxApp::OnRun() will be called which will enter the main message
        // loop and the application will run. If we returned false here, the
        // application would exit immediately.
        return true;
    }
    else {
        OnExit();
        return false;
    }

}

int MyApp::OnExit()
{
    // delete all objects used by the App

    // path names
    delete g_pPaths;

    return 0;
}

bool MyApp::ProcessCmdLine (wxChar** argv, int argc) {

    // get and process command line

    // The structure wxCmdLineEntryDesc is used to describe the one command line switch, 
    // option or parameter. 
    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        {wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be verbose") },
        {wxCMD_LINE_SWITCH, _T("c"), NULL, _T("Command mode: GUI not activated") },
        {wxCMD_LINE_OPTION, _T("book"), NULL, _T("Source eBook to process (full path)") },
        {wxCMD_LINE_OPTION, _T("out"), NULL, _T("Path to store results") },
        {wxCMD_LINE_OPTION, _T("langs"), NULL, _T("languages") },
        //{wxCMD_LINE_PARAM,  NULL, NULL, _T("input files"),
        // wxCMD_LINE_VAL_STRING,
        // wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE},
        // End of command list
        {wxCMD_LINE_NONE}
    };
    
    // default values for params
    wxString sBook = _T("nil");
    wxString sOut = _T("nil");
    wxString sLangs = _T("nil");

    //analyse the command line
    wxCmdLineParser oParser(cmdLineDesc, argc, argv);
    int nResp = oParser.Parse();
    if (nResp == -1) {      //solicitado help. Ya ha sido impreso
    }
    else if (nResp > 0) {      //error
        wxLogMessage(_T("Syntax error detected, aborting."));
    }
    else {      //todo correcto. Procesar
        m_fUseGUI = !oParser.Found(_T("c"));
        oParser.Found(_T("book"), &sBook);
        oParser.Found(_T("out"), &sOut);
        oParser.Found(_T("langs"), &sLangs);
    }

    if (!m_fUseGUI) {
        CmdCompileBook(sBook, sOut, sLangs, g_pPaths->GetLocalePath() );
    }

    //// get filenames from the commandline
    //m_fnames->Clear();
    //if (parser.Parse() == 0) {
    //    for (size_t paramNr=0; paramNr < parser.GetParamCount(); ++paramNr) {
    //        m_fnames->Add (parser.GetParam (paramNr));
    //    }
    //}

    return true;
}

