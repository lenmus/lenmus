//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#include "SplashFrame.h"

class wxDocManager;
class lmMainFrame;
class lmView;
class lmController;


// Class lmTheApp defines the lenmus application
class lmTheApp: public wxApp
{
  public:

      // event handlers
    bool OnInit();
    int OnExit();

    void UpdateCurrentDocViews();

    // operations
    void ChangeLanguage(wxString lang);

    // Accessors
    wxBitmap& GetBackgroundBitmap() const { return (wxBitmap&) m_background; }
    lmView* GetActiveView() { return (lmView*)m_pDocManager->GetCurrentView(); }
    wxDocManager* GetDocManager() { return m_pDocManager; }
    wxString GetLanguageCanonicalName() { return m_pLocale->GetCanonicalName(); }
    wxString GetLocaleName() { return m_pLocale->GetLocale(); }
    wxString GetLocaleSysName() { return m_pLocale->GetSysName(); }
    wxString GetVersionNumber();

	//overrides
	int FilterEvent(wxEvent& event);


private:
    void GetMainWindowPlacement(wxRect *frameRect, bool *fMaximized);
    void GetDefaultMainWindowRect(wxRect *defRect);
    wxString GetInstallerLanguage();
    wxString ChooseLanguage(wxWindow *parent);
    void SetUpLocale(wxString lang);
    lmSplashFrame* RecreateGUI(int nMilliseconds);
	void FindOutScreenDPI();


    wxDocManager*	m_pDocManager;
    wxLocale*       m_pLocale;          //locale we'll be using (user config)
    wxBitmap        m_background;       //background bitmap (user config)

    //object used to check if another instance of this program is running
    wxSingleInstanceChecker*    m_pInstanceChecker;

};

DECLARE_APP(lmTheApp)

extern lmMainFrame *GetMainFrame();


//extern bool singleWindowMode;
extern lmTheApp* g_pTheApp;
extern double	g_rScreenDPI;
extern double	g_rPixelsPerLU;

#endif    // __LM_THEAPP_H__
