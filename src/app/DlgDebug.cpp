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

#ifdef __GNUG__
#pragma implementation "DlgDebug.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <wx/html/htmlwin.h>
#include <wx/button.h>
#include <wx/sizer.h>


#include "DlgDebug.h"

BEGIN_EVENT_TABLE(lmDlgDebug, wxDialog)
   EVT_BUTTON(wxID_OK, lmDlgDebug::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmDlgDebug, wxDialog)

lmDlgDebug::lmDlgDebug(wxWindow * parent, wxString sTitle, wxString sData)
    :  wxDialog(parent, -1, sTitle,
            wxDefaultPosition, wxSize(800, 430), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
    Centre();

    wxBoxSizer * pMainSizer = new wxBoxSizer(wxVERTICAL);

    m_pTxtData = new wxTextCtrl(this, -1, sData,
                                wxPoint(10, 10),
                                wxSize(780, 380),
                                wxTE_MULTILINE | wxHSCROLL | wxTE_READONLY );
    m_pTxtData->SetFont(wxFont(10, wxFONTFAMILY_MODERN, wxNORMAL, wxNORMAL, FALSE, _T("Courier")));

    pMainSizer->Add(m_pTxtData,
                    1,            // make vertically stretchable
                    wxEXPAND |    // make horizontally stretchable
                    wxALL,        //   and make border all around
                    10 );         // set border width to 10


    wxButton *cmdOK = new wxButton(this, wxID_OK,
                                _("OK"),
                                wxPoint(150, 390),
                                wxSize(80, 25));
    cmdOK->SetDefault();
    cmdOK->SetFocus();
    pMainSizer->Add(cmdOK, 0, wxALIGN_CENTER | wxALL, 10);

    // set autolayout based on sizers
    SetAutoLayout(true);
    SetSizer(pMainSizer);

}

lmDlgDebug::~lmDlgDebug()
{
}

void lmDlgDebug::OnOK(wxCommandEvent& WXUNUSED(event))
{
   EndModal(wxID_OK);
}

void lmDlgDebug::AppendText(wxString sText)
{
    m_pTxtData->AppendText(sText);
}

