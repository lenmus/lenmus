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

#ifndef __LM_ABOUTDLGH__        //to avoid nested includes
#define __LM_ABOUTDLGH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "AboutDlg.cpp"
#endif

// headers
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/html/htmlwin.h>


class lmAboutDialog : public wxDialog
{

public:
    lmAboutDialog(wxWindow* pParent);
    ~lmAboutDialog();

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

    DECLARE_EVENT_TABLE()
};

#endif    // __LM_ABOUTDLGH__
