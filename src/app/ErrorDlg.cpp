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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ErrorDlg.h"
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

#include "ErrorDlg.h"


BEGIN_EVENT_TABLE(lmErrorDlg, wxDialog)
    EVT_BUTTON( XRCID( "btnAccept" ), lmErrorDlg::OnAcceptClicked )
    EVT_BUTTON( XRCID( "btnCancel" ), lmErrorDlg::OnCancelClicked )

END_EVENT_TABLE()



lmErrorDlg::lmErrorDlg(wxWindow* pParent, wxString sErrorType, wxString sErrorMsg)
{
    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, pParent, _T("ErrorDlg"));

        //
        //get pointers to all controls
        //

    m_pTxtType = XRCCTRL(*this, "txtErrorType", wxStaticText);
    m_pTxtMsge = XRCCTRL(*this, "txtMsge", wxStaticText);

    //load error icon
    wxStaticBitmap* pBmpError = XRCCTRL(*this, "bmpErrorIcon", wxStaticBitmap);
    pBmpError->SetBitmap( wxArtProvider::GetIcon(_T("msg_error"), wxART_TOOLBAR, wxSize(32,32)) );

    //prepare information to display
    m_pTxtType->SetLabel(sErrorType);
    m_pTxtMsge->SetLabel(sErrorMsg);

    CentreOnScreen();

}

lmErrorDlg::~lmErrorDlg()
{
}

void lmErrorDlg::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

void lmErrorDlg::OnCancelClicked(wxCommandEvent& WXUNUSED(event))
{
    EndDialog(wxID_CANCEL);
}

