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
#pragma implementation "DlgPatternEditor.h"
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


#include "DlgPatternEditor.h"


static wxSize m_nSize(24,24);

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmDlgPatternEditor, wxDialog)
    EVT_BUTTON( XRCID( "btnAccept" ), lmDlgPatternEditor::OnAcceptClicked )
    EVT_BUTTON( XRCID( "btnCancel" ), lmDlgPatternEditor::OnCancelClicked )

    // attributes for notes buttons
    EVT_BUTTON( XRCID( "btnDotted" ), lmDlgPatternEditor::OnDottedClicked )
    EVT_BUTTON( XRCID( "btnTiedNext" ), lmDlgPatternEditor::OnTiedNextClicked )
    EVT_BUTTON( XRCID( "btnNotBeamed" ), lmDlgPatternEditor::OnNotBeamedClicked )
    EVT_BUTTON( XRCID( "btnStartBeam" ), lmDlgPatternEditor::OnStartBeamClicked )
    EVT_BUTTON( XRCID( "btnContinueBeam" ), lmDlgPatternEditor::OnContinueBeamClicked )
    EVT_BUTTON( XRCID( "btnEndBeam" ), lmDlgPatternEditor::OnEndBeamClicked )

    // notes buttons
    EVT_BUTTON( XRCID( "btnNote1" ), lmDlgPatternEditor::OnNoteClicked )
    EVT_BUTTON( XRCID( "btnNote2" ), lmDlgPatternEditor::OnNoteClicked )
    EVT_BUTTON( XRCID( "btnNote4" ), lmDlgPatternEditor::OnNoteClicked )
    EVT_BUTTON( XRCID( "btnNote8" ), lmDlgPatternEditor::OnNoteClicked )
    EVT_BUTTON( XRCID( "btnNote16" ), lmDlgPatternEditor::OnNoteClicked )
    EVT_BUTTON( XRCID( "btnNote32" ), lmDlgPatternEditor::OnNoteClicked )
    EVT_BUTTON( XRCID( "btnNote64" ), lmDlgPatternEditor::OnNoteClicked )

    // rests buttons
    EVT_BUTTON( XRCID( "btnRest1" ), lmDlgPatternEditor::OnRestClicked )
    EVT_BUTTON( XRCID( "btnRest2" ), lmDlgPatternEditor::OnRestClicked )
    EVT_BUTTON( XRCID( "btnRest4" ), lmDlgPatternEditor::OnRestClicked )
    EVT_BUTTON( XRCID( "btnRest8" ), lmDlgPatternEditor::OnRestClicked )
    EVT_BUTTON( XRCID( "btnRest16" ), lmDlgPatternEditor::OnRestClicked )
    EVT_BUTTON( XRCID( "btnRest32" ), lmDlgPatternEditor::OnRestClicked )
    EVT_BUTTON( XRCID( "btnRest64" ), lmDlgPatternEditor::OnRestClicked )

    // other buttons
    EVT_BUTTON( XRCID( "btnClearAll" ), lmDlgPatternEditor::OnClearAllClicked )

END_EVENT_TABLE()



lmDlgPatternEditor::lmDlgPatternEditor(wxWindow * parent)
{
    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("DlgPatternEditor"));

        //
        //get pointers to all controls
        //

    m_pTxtPattern = XRCCTRL(*this, "txtPattern", wxTextCtrl);

    // attributes for notes buttons
    m_pBtnDotted = XRCCTRL(*this,"btnDotted", wxBitmapButton);
    m_pBtnTiedNext = XRCCTRL(*this, "btnTiedNext", wxBitmapButton);
    m_pBtnNotBeamed = XRCCTRL(*this, "btnNotBeamed", wxBitmapButton);
    m_pBtnStartBeam = XRCCTRL(*this, "btnStartBeam", wxBitmapButton);
    m_pBtnContinueBeam = XRCCTRL(*this, "btnContinueBeam", wxBitmapButton);
    m_pBtnEndBeam = XRCCTRL(*this, "btnEndBeam", wxBitmapButton);

    // Notes buttons
    m_pBtnNote[0] = XRCCTRL(*this, "btnNote1", wxBitmapButton);
    m_pBtnNote[1] = XRCCTRL(*this, "btnNote2", wxBitmapButton);
    m_pBtnNote[2] = XRCCTRL(*this, "btnNote4", wxBitmapButton);
    m_pBtnNote[3] = XRCCTRL(*this, "btnNote8", wxBitmapButton);
    m_pBtnNote[4] = XRCCTRL(*this, "btnNote16", wxBitmapButton);
    m_pBtnNote[5] = XRCCTRL(*this, "btnNote32", wxBitmapButton);
    m_pBtnNote[6] = XRCCTRL(*this, "btnNote64", wxBitmapButton);

    // Rests buttons
    m_pBtnRest[0] = XRCCTRL(*this, "btnRest1", wxBitmapButton);
    m_pBtnRest[1] = XRCCTRL(*this, "btnRest2", wxBitmapButton);
    m_pBtnRest[2] = XRCCTRL(*this, "btnRest4", wxBitmapButton);
    m_pBtnRest[3] = XRCCTRL(*this, "btnRest8", wxBitmapButton);
    m_pBtnRest[4] = XRCCTRL(*this, "btnRest16", wxBitmapButton);
    m_pBtnRest[5] = XRCCTRL(*this, "btnRest32", wxBitmapButton);
    m_pBtnRest[6] = XRCCTRL(*this, "btnRest64", wxBitmapButton);

    // set buttons' icons
    m_pBtnNote[0]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("whole_note"), wxART_TOOLBAR, m_nSize) );
    m_pBtnNote[1]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("half_note"), wxART_TOOLBAR, m_nSize) );
    m_pBtnNote[2]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("quarter_note"), wxART_TOOLBAR, m_nSize) );
    m_pBtnNote[3]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("8th_note"), wxART_TOOLBAR, m_nSize) );
    m_pBtnNote[4]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("16th_note"), wxART_TOOLBAR, m_nSize) );
    m_pBtnNote[5]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("32th_note"), wxART_TOOLBAR, m_nSize) );
    m_pBtnNote[6]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("64th_note"), wxART_TOOLBAR, m_nSize) );

    m_pBtnRest[0]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("whole_rest"), wxART_TOOLBAR, m_nSize) );
    m_pBtnRest[1]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("half_rest"), wxART_TOOLBAR, m_nSize) );
    m_pBtnRest[2]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("quarter_rest"), wxART_TOOLBAR, m_nSize) );
    m_pBtnRest[3]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("8th_rest"), wxART_TOOLBAR, m_nSize) );
    m_pBtnRest[4]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("16th_rest"), wxART_TOOLBAR, m_nSize) );
    m_pBtnRest[5]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("32th_rest"), wxART_TOOLBAR, m_nSize) );
    m_pBtnRest[6]->SetBitmapLabel( wxArtProvider::GetBitmap(_T("64_rest"), wxART_TOOLBAR, m_nSize) );

    m_pBtnDotted->SetBitmapLabel( wxArtProvider::GetBitmap(_T("dotted"), wxART_TOOLBAR, m_nSize) );
    m_pBtnTiedNext->SetBitmapLabel( wxArtProvider::GetBitmap(_T("tied_next"), wxART_TOOLBAR, m_nSize) );
    m_pBtnNotBeamed->SetBitmapLabel( wxArtProvider::GetBitmap(_T("not_beamed"), wxART_TOOLBAR, m_nSize) );
    m_pBtnStartBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("start_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnContinueBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("continue_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnEndBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("end_beam"), wxART_TOOLBAR, m_nSize) );

    // initializations
    m_fDotted = false;
    m_fTiedNext = false;
    m_fStartBeam = false;
    m_fEndBeam = false;
    m_fInBeam = false;

    EnableButtons();

    //force re-layout to take into account bitmaps sizes and center dialog on screen
    Layout();           //doesn't work
    CentreOnScreen();

}

lmDlgPatternEditor::~lmDlgPatternEditor()
{
}

/*! Accept button will be enabled only if all data habe been validated and is Ok. So
    when accept button is clicked we can proceed to save data.
*/
void lmDlgPatternEditor::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
   
    //terminate the dialog 
    EndModal(wxID_OK);      
}

void lmDlgPatternEditor::OnNoteClicked(wxCommandEvent& event)
{
    wxString sNote = _T("");
    int nId = event.GetId();

    if (nId == XRCID( "btnNote1" ))       { sNote = _T("(n * r"); }
    else if (nId == XRCID( "btnNote2" ))  { sNote = _T("(n * b"); }
    else if (nId == XRCID( "btnNote4" ))  { sNote = _T("(n * n"); }
    else if (nId == XRCID( "btnNote8" ))  { sNote = _T("(n * c"); }
    else if (nId == XRCID( "btnNote16" )) { sNote = _T("(n * s"); }
    else if (nId == XRCID( "btnNote32" )) { sNote = _T("(n * f"); }
    else if (nId == XRCID( "btnNote64" )) { sNote = _T("(n * m"); }

    AddElement(sNote);

}

void lmDlgPatternEditor::OnRestClicked(wxCommandEvent& event)
{
    wxString sRest = _T("");
    int nId = event.GetId();

    if (nId == XRCID( "btnRest1" ))       { sRest = _T("(s r"); }
    else if (nId == XRCID( "btnRest2" ))  { sRest = _T("(s b"); }
    else if (nId == XRCID( "btnRest4" ))  { sRest = _T("(s n"); }
    else if (nId == XRCID( "btnRest8" ))  { sRest = _T("(s c"); }
    else if (nId == XRCID( "btnRest16" )) { sRest = _T("(s s"); }
    else if (nId == XRCID( "btnRest32" )) { sRest = _T("(s f"); }
    else if (nId == XRCID( "btnRest64" )) { sRest = _T("(s m"); }

    AddElement(sRest);

}

void lmDlgPatternEditor::AddElement(wxString sStart)
{
    wxString sElement = sStart;
    if (m_fDotted) sElement += _T(".");
    if (m_fStartBeam) {
        sElement += _T(" g+");
        m_fInBeam = true;
    }
    if (m_fEndBeam) {
        sElement += _T(" g-");
        m_fInBeam = false;
    }
    if (m_fTiedNext) sElement += _T(" l+");

    sElement += _T(")");

    m_pTxtPattern->AppendText(sElement);

    //reset attributes
    m_fDotted = false;
    m_fTiedNext = false;
    m_fStartBeam = false;
    m_fEndBeam = false;

    //reset button images
    m_pBtnDotted->SetBitmapLabel( wxArtProvider::GetBitmap(_T("dotted"), wxART_TOOLBAR, m_nSize) );
    m_pBtnTiedNext->SetBitmapLabel( wxArtProvider::GetBitmap(_T("tied_next"), wxART_TOOLBAR, m_nSize) );
    m_pBtnNotBeamed->SetBitmapLabel( wxArtProvider::GetBitmap(_T("not_beamed"), wxART_TOOLBAR, m_nSize) );
    m_pBtnStartBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("start_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnContinueBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("continue_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnEndBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("end_beam"), wxART_TOOLBAR, m_nSize) );

    EnableButtons();

}

void lmDlgPatternEditor::EnableButtons()
{
    //enable/disable buttons
    if (m_fInBeam) {
        m_pBtnNotBeamed->Enable(false);
        m_pBtnStartBeam->Enable(false);
        m_pBtnContinueBeam->Enable(true);
        m_pBtnEndBeam->Enable(true);
    }
    else {
        m_pBtnNotBeamed->Enable(true);
        m_pBtnStartBeam->Enable(true);
        m_pBtnContinueBeam->Enable(false);
        m_pBtnEndBeam->Enable(false);
    }

    //
    //set button images
    //

    // dotted
    if (m_fDotted) 
        m_pBtnDotted->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    else
        m_pBtnDotted->SetBitmapLabel( wxArtProvider::GetBitmap(_T("dotted"), wxART_TOOLBAR, m_nSize) );

    // tied next
    if (m_fTiedNext)
        m_pBtnTiedNext->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    else
        m_pBtnTiedNext->SetBitmapLabel( wxArtProvider::GetBitmap(_T("tied_next"), wxART_TOOLBAR, m_nSize) );

    // start beam
    if (m_fStartBeam)
        m_pBtnStartBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    else
        m_pBtnStartBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("start_beam"), wxART_TOOLBAR, m_nSize) );

    //end beam
    if (m_fEndBeam)
        m_pBtnEndBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    else 
        m_pBtnEndBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("end_beam"), wxART_TOOLBAR, m_nSize) );

    //not beamed
    if (!m_fStartBeam && !m_fEndBeam && !m_fInBeam)
        m_pBtnNotBeamed->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    else
        m_pBtnNotBeamed->SetBitmapLabel( wxArtProvider::GetBitmap(_T("not_beamed"), wxART_TOOLBAR, m_nSize) );

    //continue beam
    if (m_fInBeam && !m_fEndBeam)
        m_pBtnContinueBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    else
        m_pBtnContinueBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("continue_beam"), wxART_TOOLBAR, m_nSize) );

}

void lmDlgPatternEditor::OnClearAllClicked(wxCommandEvent& event)
{
    m_pTxtPattern->Clear();
}

void lmDlgPatternEditor::OnDottedClicked(wxCommandEvent& event)
{
    //togle button
    m_fDotted = !m_fDotted;
    if (m_fDotted) 
        m_pBtnDotted->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    else
        m_pBtnDotted->SetBitmapLabel( wxArtProvider::GetBitmap(_T("dotted"), wxART_TOOLBAR, m_nSize) );

}

void lmDlgPatternEditor::OnTiedNextClicked(wxCommandEvent& event)
{
    //togle button
    m_fTiedNext = !m_fTiedNext;
    if (m_fTiedNext)
        m_pBtnTiedNext->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    else
        m_pBtnTiedNext->SetBitmapLabel( wxArtProvider::GetBitmap(_T("tied_next"), wxART_TOOLBAR, m_nSize) );
}

void lmDlgPatternEditor::OnNotBeamedClicked(wxCommandEvent& event)
{
    m_pBtnNotBeamed->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );

    //unset related ones
    m_fStartBeam = false;
    m_fEndBeam = false;
    m_pBtnStartBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("start_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnContinueBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("continue_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnEndBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("end_beam"), wxART_TOOLBAR, m_nSize) );
}

void lmDlgPatternEditor::OnStartBeamClicked(wxCommandEvent& event)
{
    m_fStartBeam = true;
    m_pBtnStartBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );

    //unset related ones
    m_fEndBeam = false;
    m_pBtnNotBeamed->SetBitmapLabel( wxArtProvider::GetBitmap(_T("not_beamed"), wxART_TOOLBAR, m_nSize) );
    m_pBtnContinueBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("continue_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnEndBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("end_beam"), wxART_TOOLBAR, m_nSize) );
}

void lmDlgPatternEditor::OnContinueBeamClicked(wxCommandEvent& event)
{
    m_pBtnContinueBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );

    //unset related ones
    m_fStartBeam = false;
    m_fEndBeam = false;
    m_pBtnNotBeamed->SetBitmapLabel( wxArtProvider::GetBitmap(_T("not_beamed"), wxART_TOOLBAR, m_nSize) );
    m_pBtnStartBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("start_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnEndBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("end_beam"), wxART_TOOLBAR, m_nSize) );
}

void lmDlgPatternEditor::OnEndBeamClicked(wxCommandEvent& event)
{
    m_fEndBeam = true;
    m_pBtnEndBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, m_nSize) );
    
    //unset related ones
    m_fStartBeam = false;
    m_pBtnNotBeamed->SetBitmapLabel( wxArtProvider::GetBitmap(_T("not_beamed"), wxART_TOOLBAR, m_nSize) );
    m_pBtnStartBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("start_beam"), wxART_TOOLBAR, m_nSize) );
    m_pBtnContinueBeam->SetBitmapLabel( wxArtProvider::GetBitmap(_T("continue_beam"), wxART_TOOLBAR, m_nSize) );
}

