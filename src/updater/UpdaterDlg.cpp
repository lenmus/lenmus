//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
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
//    for (any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file UpdaterDlg.cpp
    @brief Implementation file for class lmUpdaterDlg
    @ingroup updates_management
*/

//for GCC
#ifdef __GNUG__
    #pragma implementation "UpdaterDlg.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <wx/button.h>

#include "wx/xrc/xmlres.h"


#include "UpdaterDlg.h"


static wxSize m_nSize(24,24);

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmUpdaterDlg, wxDialog)
    EVT_BUTTON( XRCID( "btnDownload" ), lmUpdaterDlg::OnDownloadClicked )
    EVT_BUTTON( XRCID( "btnCancel" ), lmUpdaterDlg::OnCancelClicked )

END_EVENT_TABLE()



lmUpdaterDlg::lmUpdaterDlg(wxWindow * parent, lmUpdater* pUpdater)
{
    m_pUpdater = pUpdater;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("UpdaterDlg"));

        //
        //get pointers to all controls
        //

    m_pHtmlWindow = XRCCTRL(*this, "htmlDisplay", wxHtmlWindow);

    //prepare information to display
    wxString sPage = _T("<html><body><p><b>");
    sPage += _("There is a more recent version available");
    sPage += _T("</b></p><p>");
    sPage += _("Latest version: ");
    sPage += m_pUpdater->GetVersion();
    sPage += _T("</p><p><b>");
    sPage += _("Details:");
    sPage += _T("</b></p>");
    sPage += m_pUpdater->GetDescription();
    sPage += _T("</body></html>");

    //load it in the dialog
    m_pHtmlWindow->SetPage(sPage);

    CentreOnScreen();

}

lmUpdaterDlg::~lmUpdaterDlg()
{
}

void lmUpdaterDlg::OnDownloadClicked(wxCommandEvent& WXUNUSED(event))
{
    //show download page
    wxString sPage = _T("<html><body><p><b>");
    sPage += _("Downloading ....");
    sPage += _T("</b></p><p>");
    sPage += m_pUpdater->GetUrl();
    sPage += _T("</p></body></html>");

    //load it in the dialog
    m_pHtmlWindow->SetPage(sPage);

    //terminate the dialog 
    //EndModal(wxID_OK);      
}

