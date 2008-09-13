//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_WELCOMEWND_H__        //to avoid nested includes
#define __LM_WELCOMEWND_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "WelcomeWnd.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// headers
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/html/htmlwin.h>

#include "../mdi/ChildFrame.h"

class wxHyperlinkCtrl;
class wxHyperlinkEvent;
class wxFileHistory;

class lmWelcomeWnd: public lmMDIChildFrame
{
   DECLARE_DYNAMIC_CLASS(lmWelcomeWnd)

public:
    lmWelcomeWnd(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~lmWelcomeWnd();

    //event handlers
    void OnNewInLenmus(wxHyperlinkEvent& event);
    void OnNewScore(wxHyperlinkEvent& event);
    void OnOpenEBooks(wxHyperlinkEvent& event);
    void OnOpenRecent(wxHyperlinkEvent& event);
    void OnCloseWindow(wxCloseEvent& event);


private:
    void CreateControls(int nRecentFiles, wxFileHistory* pHistory);

    //controls on dialog
	wxStaticBitmap*     m_pBmpLeftBanner;

	wxStaticText*       m_pTxtTitle;

	wxStaticText*       m_pLearnTitle;
	wxStaticBitmap*     m_pLearnIcon;
	wxHyperlinkCtrl*    m_pLinkNewInLenmus;
	wxHyperlinkCtrl*    m_pLinkVisitWebsite;
	wxStaticText*       m_pPhonascusTitle;
	wxStaticBitmap*     m_pPhonascusIcon;
	wxHyperlinkCtrl*    m_pLinkOpenEBooks;
	wxStaticText*       m_pScoreTitle;
	wxStaticBitmap*     m_pScoreIcon;
	wxHyperlinkCtrl*    m_pLinkNewScore;
	wxStaticText*       m_pRecentScoresTitle;

	wxHyperlinkCtrl*    m_pLinkRecent[9];

    //other variables
    wxString        m_sHeader;          //html code to start a page
    wxString        m_sVersionNumber;   //version number in format "x.x"

    DECLARE_EVENT_TABLE()
};

#endif    // __LM_WELCOMEWND_H__
