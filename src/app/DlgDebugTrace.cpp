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
//    for (any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DlgDebugTrace.h"
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
#include <wx/button.h>
#include <wx/sizer.h>

#include "wx/xrc/xmlres.h"

#include "DlgDebugTrace.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


// lmDlgDebugTrace events table
BEGIN_EVENT_TABLE(lmDlgDebugTrace, wxDialog)
    EVT_BUTTON( XRCID( "btnAccept" ), lmDlgDebugTrace::OnAcceptClicked )
    EVT_BUTTON( XRCID( "btnCancel" ), lmDlgDebugTrace::OnCancelClicked )

    // add/remove buttons
    EVT_BUTTON( XRCID( "btnAddAll" ), lmDlgDebugTrace::OnAddAll )
    EVT_BUTTON( XRCID( "btnAdd" ), lmDlgDebugTrace::OnAdd )
    EVT_BUTTON( XRCID( "btnRemoveAll" ), lmDlgDebugTrace::OnRemoveAll )
    EVT_BUTTON( XRCID( "btnRemove" ), lmDlgDebugTrace::OnRemove )

END_EVENT_TABLE()



lmDlgDebugTrace::lmDlgDebugTrace(wxWindow * parent)
{
    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("DlgDebugTrace"));

    //get pointers to all controls
    m_pLstAvailable = XRCCTRL(*this, "lstAvailable", wxListBox);
    m_pLstToTrace = XRCCTRL(*this, "lstToTrace", wxListBox);

    // initialize all controls with current data
    g_pLogger->PopulateWithDefinedTraceMasks(m_pLstAvailable);
    m_pLstToTrace->Clear();
    for (int i=0; (size_t)i < m_pLstAvailable->GetCount(); i++) {
        wxString sMask = m_pLstAvailable->GetString(i);
        if (g_pLogger->IsAllowedTraceMask(sMask)) 
            m_pLstToTrace->Append(sMask);
    }

    // show dialog
    this->SetSize(600, 400);
    CentreOnParent();

}

lmDlgDebugTrace::~lmDlgDebugTrace()
{
}

void lmDlgDebugTrace::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    g_pLogger->ClearTraceMasks();
    for (int i=0; (size_t)i < m_pLstToTrace->GetCount(); i++) {
        g_pLogger->AddTraceMask( m_pLstToTrace->GetString(i) );
    }
    //terminate the dialog
    EndModal(wxID_OK);
}

void lmDlgDebugTrace::OnAddAll(wxCommandEvent& WXUNUSED(event))
{
    m_pLstToTrace->Clear();
    for (int i=0; (size_t)i < m_pLstAvailable->GetCount(); i++) {
        m_pLstToTrace->Append( m_pLstAvailable->GetString(i) );
    }
}

void lmDlgDebugTrace::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    wxArrayInt anSelections;
    m_pLstAvailable->GetSelections(anSelections);
    for (int i=0; (size_t)i < anSelections.GetCount(); i++) {
        m_pLstToTrace->Append( m_pLstAvailable->GetString(anSelections[i]) );
    }
}

void lmDlgDebugTrace::OnRemoveAll(wxCommandEvent& WXUNUSED(event))
{
    m_pLstToTrace->Clear();
}

void lmDlgDebugTrace::OnRemove(wxCommandEvent& WXUNUSED(event))
{
    int iNdx = m_pLstToTrace->GetSelection();
    if (iNdx != wxNOT_FOUND) m_pLstToTrace->Delete(iNdx);
}

