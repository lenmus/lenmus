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

#ifndef __LENMUS_ABOUT_DIALOG_H__
#define __LENMUS_ABOUT_DIALOG_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/html/htmlwin.h>
#include <wx/event.h>

//forward declarations
class wxStaticBitmap;
class wxStaticText;
class wxStaticBitmap;
class wxStaticBitmap;

namespace lenmus
{

//---------------------------------------------------------------------------------------
class AboutDialog : public wxDialog
{
protected:
    ApplicationScope& m_appScope;

public:
    AboutDialog(wxWindow* pParent, ApplicationScope& appScope);
    ~AboutDialog();

    // event handlers
    void OnAccept(wxCommandEvent& WXUNUSED(event));
    void OnPurpose(wxCommandEvent& WXUNUSED(event));
    void OnLicense(wxCommandEvent& WXUNUSED(event));
    void OnDevelopers(wxCommandEvent& WXUNUSED(event));
    void OnArtCredits(wxCommandEvent& WXUNUSED(event));
    void OnSoftwareCredits(wxCommandEvent& WXUNUSED(event));
    //void OnTranslators(wxCommandEvent& WXUNUSED(event));
    void OnBuildInfo(wxCommandEvent& WXUNUSED(event));


private:
	void CreateControls();

    //controls on dialog
	wxStaticBitmap*		m_pBmpLogo;
	wxStaticText*		m_pTxtTitle;
	wxStaticText*		m_pTxtSubtitle;
	wxHtmlWindow*		m_pHtmlWindow;
	wxButton*			m_pBtnPurpose;
	wxButton*			m_pBtnLicense;
	wxButton*			m_pBtnDevelopers;
	wxButton*			m_pBtnArtCredits;
	wxButton*			m_pBtnSoftwareCredits;
	wxButton*			m_pBtnBuildInfo;
	wxButton*			m_pBtnAccept;

    //other variables
    wxString        m_sHeader;          //html code to start a page
    wxString        m_sVersionNumber;   //version number in format "x.x"

    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_ABOUT_DIALOG_H__
