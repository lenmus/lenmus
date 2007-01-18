//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
/*! @file TheApp.h
    @brief Header file for class lmTheApp
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma interface "TheApp.h"
#endif

#ifndef __LENMUS_APP_H_
#define __LENMUS_APP_H_

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"

#include "SplashFrame.h"

class wxDocManager;
class lmMainFrame;
class lmEditFrame;
class lmScoreView;

// Class lmTheApp defines the lenmus application
class lmTheApp: public wxApp
{
  public:
    lmTheApp();

	// event handlers
    bool OnInit();
    int OnExit();


    lmEditFrame* CreateProjectFrame(wxDocument* doc, wxView* view);
    void UpdateCurrentDocViews();

    // operations
    void ChangeLanguage(wxString lang);

    // Accessors
    wxBitmap& GetBackgroundBitmap() const { return (wxBitmap&) m_background; }
    lmScoreView* GetActiveView() { return (lmScoreView *)m_pDocManager->GetCurrentView(); }
    wxDocManager* GetDocManager() { return m_pDocManager; }
    wxString GetLanguageCanonicalName() { return m_pLocale->GetCanonicalName(); }
    wxString GetLocaleName() { return m_pLocale->GetLocale(); }
    wxString GetLocaleSysName() { return m_pLocale->GetSysName(); }
    wxString GetVersionNumber();

private:
    void GetMainWindowPlacement(wxRect *frameRect, bool *fMaximized);
    void GetDefaultMainWindowRect(wxRect *defRect);
    wxString GetInstallerLanguage();
    wxString ChooseLanguage(wxWindow *parent);
    void SetUpLocale(wxString lang);
    lmSplashFrame* RecreateGUI(int nMilliseconds);


    wxDocManager*	m_pDocManager;
    wxLocale*       m_pLocale;          //locale we'll be using (user config)
    wxBitmap        m_background;       //background bitmap (user config)


};

DECLARE_APP(lmTheApp)

extern lmMainFrame *GetMainFrame();


//extern bool singleWindowMode;
extern lmTheApp* g_pTheApp;

#endif    // __LENMUS_APP_H_
