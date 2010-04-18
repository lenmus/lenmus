//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_THEAPP_H__
#define __LM_THEAPP_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TheApp.cpp"
#endif

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/snglinst.h"


class lmMainFrame;
class lmView;
class lmController;
class lmDocManager;
class wxCmdLineParser;
class lmSplashFrame;

const int lmID_CHANGE_LANGUAGE = ::wxNewId();
DECLARE_EVENT_TYPE(lmEVT_CHANGE_LANGUAGE, -1)

// Class lmTheApp defines the lenmus application
class lmTheApp: public wxApp
{
public:
    lmTheApp();

      // event handlers
    virtual bool OnInit();
    virtual int OnExit();
    virtual int OnRun();
    void OnChangeLanguage(wxCommandEvent& WXUNUSED(event));

    // Accessors
    wxBitmap& GetBackgroundBitmap() const { return (wxBitmap&) m_background; }
    wxString GetLanguageCanonicalName() { return m_pLocale->GetCanonicalName(); }
    wxString GetLocaleName() { return m_pLocale->GetLocale(); }
    wxString GetLocaleSysName() { return m_pLocale->GetSysName(); }
    const wxString GetVersionNumber();
    const wxString GetCurrentUser();

	//overrides
	virtual int FilterEvent(wxEvent& event);
    virtual void OnFatalException();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

private:
    void GetMainWindowPlacement(wxRect *frameRect, bool *fMaximized);
    void GetDefaultMainWindowRect(wxRect *defRect);
    wxString GetInstallerLanguage();
    wxString ChooseLanguage(wxWindow *parent);
    void SetUpLocale(wxString lang);
    lmSplashFrame* RecreateGUI(int nMilliseconds, bool fFirstTime);
	void FindOutScreenDPI();
    void SendForensicLog(wxString& sLogFile, bool fHandlingCrash);
    void ParseCommandLine();
    bool DoApplicationSetUp();
    void DoApplicationCleanUp();
    void SetUpCurrentLanguage();
    void CreateMainFrame();
    void WaitAndDestroySplash();
    void OpenWelcomeWindow();
    void RecoverScoreIfPreviousCrash();
    void CheckForUpdates();
    void SetUpMidi();
    void CreateDocumentManager();
    void CreateDocumentTemplates();
    void InitializeXrcResources();
    void CreatePathsObject();
    void DefineTraceMasks();
    void OpenDataBase();

    lmDocManager*	m_pDocManager;
    wxLocale*       m_pLocale;          //locale we'll be using (user config)
    wxBitmap        m_background;       //background bitmap (user config)
    bool            m_fUseGui;
    lmSplashFrame*  m_pSplash;
    long            m_nSplashStartTime;
    long            m_nSplashVisibleMilliseconds;


    //object used to check if another instance of this program is running
    wxSingleInstanceChecker*    m_pInstanceChecker;

    DECLARE_EVENT_TABLE()
};

DECLARE_APP(lmTheApp)

extern lmMainFrame *GetMainFrame();
extern double	g_rScreenDPI;
extern double	g_rPixelsPerLU;

#endif    // __LM_THEAPP_H__
