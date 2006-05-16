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
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file UpdaterDlg.cpp
    @brief Implementation file for all dialog classes used only by the updater
    @ingroup updates_management
*/
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "UpdaterDlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xrc/xmlres.h"
#include <wx/listctrl.h>




#include "UpdaterDlg.h"
//#include "Updater.h"



//-----------------------------------------------------------------------------
// Implementation of class lmUpdaterDlgStart
//-----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmUpdaterDlgStart, wxDialog)
    EVT_BUTTON( XRCID( "btnProceed" ), lmUpdaterDlgStart::OnProceedClicked )
    EVT_BUTTON( XRCID( "btnCancel" ), lmUpdaterDlgStart::OnCancelClicked )

END_EVENT_TABLE()



lmUpdaterDlgStart::lmUpdaterDlgStart(wxWindow* parent)
{
    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("UpdaterDlgStart"));

        //
        //get pointers to all controls
        //

    //load updater logo
    wxStaticBitmap* pBmpUpdaterLogo = XRCCTRL(*this, "bmpUpdaterLogo", wxStaticBitmap);
    pBmpUpdaterLogo->SetBitmap( wxArtProvider::GetIcon(_T("banner_updater"), wxART_OTHER) );

    CentreOnParent();

}




//-----------------------------------------------------------------------------
// Implementation of class lmUpdaterDlgInfo
//-----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmUpdaterDlgInfo, wxDialog)
    EVT_BUTTON( XRCID( "btnDownload" ), lmUpdaterDlgInfo::OnDownloadClicked )
    EVT_BUTTON( XRCID( "btnCancel" ), lmUpdaterDlgInfo::OnCancelClicked )

    // global UI updates
    EVT_UPDATE_UI(wxID_ANY, lmUpdaterDlgInfo::OnUpdateUI)

END_EVENT_TABLE()



lmUpdaterDlgInfo::lmUpdaterDlgInfo(wxWindow* parent, lmUpdater* pUpdater)
{
    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("UpdaterDlgInfo"));

        //
        //get pointers to all controls
        //
    m_pUpdates = XRCCTRL(*this, "lstUpdates", wxCheckListBox);
    m_pDescription = XRCCTRL(*this, "txtDescription", wxStaticText);
    m_pBtnDownload = XRCCTRL(*this, "btnDownload", wxButton);

    //load updater logo
    wxStaticBitmap* pBmpUpdaterLogo = XRCCTRL(*this, "bmpUpdaterLogo", wxStaticBitmap);
    pBmpUpdaterLogo->SetBitmap( wxArtProvider::GetIcon(_T("banner_updater"), wxART_OTHER) );

    //initializations
    m_nNumItems = 0;
    m_pUpdater = pUpdater;

    CentreOnParent();

}

void lmUpdaterDlgInfo::AddPackage(wxString sPackage, wxString sDescription)
{
    m_pUpdates->Set(1, &sPackage);
    m_pUpdates->Check(m_nNumItems, true);
    m_pDescription->SetLabel(sDescription);
    m_nNumItems++;

}

void lmUpdaterDlgInfo::OnDownloadClicked(wxCommandEvent& WXUNUSED(event))
{
    m_pUpdater->DownloadFiles();
    EndDialog(wxID_OK);

}

void lmUpdaterDlgInfo::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
    //enable / disable download button
    bool fEnableDownload = false;
    int i;
    for (i=0; i < m_nNumItems; i++) {
        if (m_pUpdates->IsChecked(i) ) {
            fEnableDownload = true;
            break;
        }
    }
    m_pBtnDownload->Enable(fEnableDownload);

}
