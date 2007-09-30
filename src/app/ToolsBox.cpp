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
#pragma implementation "ToolsBox.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/cursor.h"

#include "ToolsBox.h"
#include "ArtProvider.h"        // to use ArtProvider for managing icons
#include "TheApp.h"
#include "ScoreCanvas.h"

//layout parameters
const int SPACING = 5;          //spacing (pixels) around each sizer
const int NUM_COLUMNS = 4;      //number of buttons per row
const int NUM_BUTTONS = 16;
const int ID_BUTTON = 2200;


BEGIN_EVENT_TABLE(lmToolBox, wxPanel)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmToolBox::OnButtonClicked)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmToolBox, wxPanel)


lmToolBox::lmToolBox(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id, wxPoint(0,0), wxSize(150, 400), wxNO_BORDER)
{
    //The Tool box panel has two areas:
    //1. Tool selection buttons are, in the middle
    //2. Selected tool options, in the bottom

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

    //the main sizer, to contain the three areas
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    //the buttons area
    wxGridSizer *buttonsSizer = new wxGridSizer(NUM_COLUMNS);
    mainSizer->Add(buttonsSizer, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, SPACING));

    //the options area
    wxBoxSizer* optsSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(optsSizer, 0, wxGROW|wxTOP, SPACING);
    m_pOptionsPanel = new wxPanel(this, id, wxDefaultPosition, wxSize(150, 300), wxSUNKEN_BORDER );
    optsSizer->Add(m_pOptionsPanel, 0, wxGROW|wxTOP, SPACING);

    //add the tool buttons
    int iB = 0;
    int nNumRows = (NUM_BUTTONS + NUM_COLUMNS - 1) / NUM_COLUMNS;
    wxBitmapButton* m_pButton[NUM_BUTTONS];
    for (int iRow=0; iRow < nNumRows; iRow++)
    {
        for (int iCol=0; iCol < NUM_COLUMNS; iCol++)
        {
            iB = iCol + iRow * NUM_COLUMNS;    // button index: 0 .. 24
            m_pButton[iB] = new wxBitmapButton(this, ID_BUTTON + iB, 
                wxArtProvider::GetBitmap(_T("tool_new"), wxART_TOOLBAR, wxSize(16,16) ));
            buttonsSizer->Add(m_pButton[iB],
                              wxSizerFlags(0).Border(wxALL, SPACING) );
        }
    }

}

void lmToolBox::OnButtonClicked(wxCommandEvent& event)
{
	lmController* pController = g_pTheApp->GetViewController();
	pController->SetCursor(*wxCROSS_CURSOR);
    //wxMessageBox(_T("Button clicked"));
}
