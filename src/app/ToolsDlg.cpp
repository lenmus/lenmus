// RCS-ID: $Id: ToolsDlg.cpp,v 1.5 2006/02/23 19:17:49 cecilios Exp $
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
/*! @file ToolsDlg.cpp
    @brief Implementation file for class lmToolsDlg
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma implementation
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
#include <wx/dcclient.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/intl.h>

#include "wx/statline.h"
#include "wx/stattext.h"
#include "wx/notebook.h"

#include "ToolsDlg.h"
#include "global.h"

lmToolsDlg::lmToolsDlg(wxFrame *frame, wxChar *title, int x, int y )
            : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(-1, -1), 
                wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR )
{
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxNotebook *notebook = new wxNotebook( this, -1 );
    wxNotebookSizer *nbs = new wxNotebookSizer( notebook );
    topsizer->Add( nbs, 1, wxGROW );

    wxButton *button = new wxButton( this, wxID_OK, _T("OK") );
    topsizer->Add( button, 0, wxALIGN_RIGHT | wxALL, 10 );

    // First page: one big text ctrl
    wxTextCtrl *multi = new wxTextCtrl( notebook, -1, _T("TextCtrl."), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    notebook->AddPage( multi, _("Notes") );

    // Second page: a text ctrl and a button
    wxPanel *panel = new wxPanel( notebook, -1 );
    notebook->AddPage( panel, _("Clefs") );

    wxSizer *panelsizer = new wxBoxSizer( wxVERTICAL );

    wxTextCtrl *text = new wxTextCtrl( panel, -1, _T("TextLine 1."), wxDefaultPosition, wxSize(250,-1) );
    panelsizer->Add( text, 0, wxGROW|wxALL, 30 );
    text = new wxTextCtrl( panel, -1, _T("TextLine 2."), wxDefaultPosition, wxSize(250,-1) );
    panelsizer->Add( text, 0, wxGROW|wxALL, 30 );
    wxButton *button2 = new wxButton( panel, -1, _T("Hallo") );
    panelsizer->Add( button2, 0, wxALIGN_RIGHT | wxLEFT|wxRIGHT|wxBOTTOM, 30 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( panelsizer );


  // don't allow frame to get smaller than what the sizers tell it and also set
  // the initial size as calculated by the sizers
  topsizer->SetSizeHints( this );

  SetSizer( topsizer );
}

