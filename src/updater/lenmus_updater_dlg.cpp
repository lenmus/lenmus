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

//lenmus headers
#include "lenmus_updater_dlg.h"

//wxWidgets headers
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/listctrl.h>


namespace lenmus
{

//-----------------------------------------------------------------------------
// Implementation of class UpdaterDlgStart
//-----------------------------------------------------------------------------


wxBEGIN_EVENT_TABLE(UpdaterDlgStart, wxDialog)
    EVT_BUTTON( XRCID( "btnProceed" ), UpdaterDlgStart::OnProceedClicked )
    EVT_BUTTON( XRCID( "btnCancel" ), UpdaterDlgStart::OnCancelClicked )

wxEND_EVENT_TABLE()



UpdaterDlgStart::UpdaterDlgStart(wxWindow* parent)
{
    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, "UpdaterDlgStart");

        //
        //get pointers to all controls
        //

    //load updater logo
    wxStaticBitmap* pBmpUpdaterLogo = XRCCTRL(*this, "bmpUpdaterLogo", wxStaticBitmap);
    pBmpUpdaterLogo->SetBitmap( wxArtProvider::GetIcon("banner_updater", wxART_OTHER) );

    CentreOnParent();

}




//-----------------------------------------------------------------------------
// Implementation of class UpdaterDlgInfo
//-----------------------------------------------------------------------------


wxBEGIN_EVENT_TABLE(UpdaterDlgInfo, wxDialog)
    EVT_BUTTON( XRCID( "btnDownload" ), UpdaterDlgInfo::OnDownloadClicked )
    EVT_BUTTON( XRCID( "btnCancel" ), UpdaterDlgInfo::OnCancelClicked )

    // global UI updates
    EVT_UPDATE_UI(wxID_ANY, UpdaterDlgInfo::OnUpdateUI)

wxEND_EVENT_TABLE()



UpdaterDlgInfo::UpdaterDlgInfo(wxWindow* parent, Updater* pUpdater)
{
    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, "UpdaterDlgInfo");

        //
        //get pointers to all controls
        //
    m_pUpdates = XRCCTRL(*this, "lstUpdates", wxCheckListBox);
    m_pDescription = XRCCTRL(*this, "txtDescription", wxStaticText);
    m_pBtnDownload = XRCCTRL(*this, "btnDownload", wxButton);

    //load updater logo
    wxStaticBitmap* pBmpUpdaterLogo = XRCCTRL(*this, "bmpUpdaterLogo", wxStaticBitmap);
    pBmpUpdaterLogo->SetBitmap( wxArtProvider::GetIcon("banner_updater", wxART_OTHER) );

    //initializations
    m_nNumItems = 0;
    m_pUpdater = pUpdater;

    CentreOnParent();

}

void UpdaterDlgInfo::AddPackage(wxString sPackage, wxString sDescription)
{
    m_pUpdates->Set(1, &sPackage);
    m_pUpdates->Check(m_nNumItems, true);
    m_pDescription->SetLabel(sDescription);
    m_nNumItems++;

}

void UpdaterDlgInfo::OnDownloadClicked(wxCommandEvent& WXUNUSED(event))
{
    m_pUpdater->DownloadFiles();
    EndDialog(wxID_OK);

}

void UpdaterDlgInfo::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
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


}   //namespace lenmus
