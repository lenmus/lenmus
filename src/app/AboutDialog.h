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
/*! @file AboutDialog.h
    @brief Header file for class lmAboutDialog
    @ingroup app_gui
*/
#ifndef __ABOUTDLGH__        //to avoid nested includes
#define __ABOUTDLGH__

// GCC interface
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "ErrorDlg.h"
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
    void OnTranslators(wxCommandEvent& WXUNUSED(event));
    void OnBuildInfo(wxCommandEvent& WXUNUSED(event));


private:
    //controls on dialog
    wxHtmlWindow*   m_pHtmlWindow;
    wxStaticText*   m_pTxtTitle;
    wxStaticText*   m_pTxtSubtitle;

    //other variables
    wxString        m_sHeader;          //html code to start a page
    wxString        m_sVersionNumber;   //version number in format "x.x"

    DECLARE_EVENT_TABLE()
};

#endif    // __ABOUTDLGH__
