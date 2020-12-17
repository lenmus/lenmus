//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

#ifndef __LENMUS_WELCOME_WINDOW_H__        //to avoid nested includes
#define __LENMUS_WELCOME_WINDOW_H__

#if (0)   //0=old windows , 1=new window

//lenmus headers
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"
#include "lenmus_canvas.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/wxhtml.h>

//forward declarations
class wxHyperlinkCtrl;
class wxHyperlinkEvent;
class wxFileHistory;

namespace lenmus
{

//---------------------------------------------------------------------------------------
class WelcomeWindow : public wxScrolledWindow
                    , public CanvasInterface
{
protected:
    ApplicationScope& m_appScope;
    wxHtmlWindow*   m_pHtmlWindow;
    wxString        m_sHeader;          //html code to start a page
    wxString        m_sVersionNumber;   //version number in format "x.x"

public:
    WelcomeWindow(ContentWindow* parent, ApplicationScope& appScope,
                  wxFileHistory* pHistory, wxWindowID id = wxID_ANY);
    ~WelcomeWindow();

protected:
    //event handlers
    void OnNewInLenmus(wxHyperlinkEvent& event);
    void OnNewScore(wxHyperlinkEvent& event);
    void OnQuickGuide(wxHyperlinkEvent& event);
    void OnOpenEBooks(wxHyperlinkEvent& event);
    void OnOpenRecent(wxHyperlinkEvent& event);
    void OnInstructions(wxHyperlinkEvent& event);

private:
    void CreateControls(wxFileHistory* pHistory);
    void ShowDocument(wxString& sDocName);

    //controls on dialog
	wxStaticBitmap*     m_pBmpLeftBanner;
	wxStaticText*       m_pTxtTitle;
    wxStaticText*       m_pTxtVersion;
	wxStaticText*       m_pLearnTitle;
	wxStaticBitmap*     m_pLearnIcon;
	wxHyperlinkCtrl*    m_pLinkNewInLenmus;
	wxHyperlinkCtrl*    m_pLinkVisitWebsite;
	wxHyperlinkCtrl*    m_pLinkReportError;
	wxStaticText*       m_pPhonascusTitle;
	wxStaticBitmap*     m_pPhonascusIcon;
	wxHyperlinkCtrl*    m_pLinkInstructions;
	wxHyperlinkCtrl*    m_pLinkOpenEBooks;
    wxHyperlinkCtrl*    m_pLinkQuickGuide;
	wxStaticText*       m_pScoreTitle;
	wxStaticBitmap*     m_pScoreIcon;
	wxHyperlinkCtrl*    m_pLinkNewScore;
	wxStaticText*       m_pRecentScoresTitle;
	wxHyperlinkCtrl*    m_pLinkRecent[9];

    wxDECLARE_EVENT_TABLE();
};

#else
//lenmus headers
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"
#include "lenmus_canvas.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>

namespace lenmus
{

//---------------------------------------------------------------------------------------
class WelcomeWindow : public wxScrolledWindow
                    , public CanvasInterface
{
protected:
    ApplicationScope&   m_appScope;

public:
    WelcomeWindow(ContentWindow* parent, ApplicationScope& appScope,
                  wxWindowID id = wxID_ANY);
    ~WelcomeWindow();

protected:
    //event handlers
    void on_button_exercises(wxCommandEvent& event);
    void on_button_book_1(wxCommandEvent& UNUSED(event));
    void on_button_book_2(wxCommandEvent& UNUSED(event));
    void on_button_book_3(wxCommandEvent& UNUSED(event));
    void on_button_study_guide(wxCommandEvent& UNUSED(event));

    void open_book(const wxString& filename);

private:
    void create_controls();
    void show_document(const wxString& sDocName);

    //controls on dialog
    wxStaticBitmap* m_logo;
    wxStaticText* m_txtWelcome;
    wxStaticText* m_txtVersion;
    wxStaticText* m_txtWhatToDo;

    wxStaticBitmap* m_bmpGuide;
    wxStaticText* m_txtGuide;
    wxStaticText* m_txtGuideText;
    wxButton* m_btnGuide;

    wxStaticBitmap* m_bmpTrain;
    wxStaticText* m_txtTrain;
    wxStaticText* m_txtTrainText;
    wxButton* m_btnTrain;

    wxStaticBitmap* m_bmpCollaborate;
    wxStaticText* m_txtCollaborate;
    wxStaticText* m_txtCollaborateText;
    wxButton* m_btnCollaborate;

    wxStaticBitmap* m_bmpBooks;
    wxStaticText* m_txtBooks;
    wxStaticText* m_txtBooksText;
    wxStaticText* m_txtLevel1;
    wxButton* m_btnLevel1;
    wxStaticText* m_txtLevel2;
    wxButton* m_btnLevel2;
    wxStaticText* m_txtLevel3;
    wxButton* m_btnLevel3;

    wxStaticText* m_txtUCA;
    wxStaticBitmap* m_logoUCA;

    wxDECLARE_EVENT_TABLE();
};

#endif  //old or new version


}   //namespace lenmus

#endif    // __LENMUS_WELCOME_WINDOW_H__
