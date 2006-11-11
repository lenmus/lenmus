// RCS-ID: $Id: DlgCfgTheoIntervals.cpp,v 1.3 2006/02/23 19:15:54 cecilios Exp $
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
/*! @file DlgCfgTheoIntervals.cpp
    @brief Implementation file for class lmDlgCfgTheoIntervals
    @ingroup app_gui
*/

//for GCC
#ifdef __GNUG__
    #pragma implementation "DlgCfgTheoIntervals.h"
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

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;



#include "DlgCfgTheoIntervals.h"
#include "../ldp_parser/AuxString.h"
#include "../auxmusic/Conversion.h"



//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmDlgCfgTheoIntervals, wxDialog)
    EVT_BUTTON( XRCID( "buttonAccept" ), lmDlgCfgTheoIntervals::OnAcceptClicked )
    EVT_BUTTON( XRCID( "buttonCancel" ), lmDlgCfgTheoIntervals::OnCancelClicked )

    // Clefs check boxes
    EVT_CHECKBOX( XRCID( "chkGClef" ), lmDlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkF4Clef" ), lmDlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkF3Clef" ), lmDlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkC1Clef" ), lmDlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkC2Clef" ), lmDlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkC3Clef" ), lmDlgCfgTheoIntervals::OnControlClicked )
    EVT_CHECKBOX( XRCID( "chkC4Clef" ), lmDlgCfgTheoIntervals::OnControlClicked )

    // Accidentals
    EVT_CHECKBOX( XRCID( "chkAccidentals" ), lmDlgCfgTheoIntervals::OnControlClicked )

END_EVENT_TABLE()



lmDlgCfgTheoIntervals::lmDlgCfgTheoIntervals(wxWindow * parent,
                           lmTheoIntervalsConstrains* pConstrains)
{
    // save received data
    m_pConstrains = pConstrains;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("DlgCfgTheoIntervals"));

        //
        //get pointers to all controls
        //

    // error messages and bitmaps
    m_pLblClefError = XRCCTRL(*this, "lblClefError", wxStaticText);
    m_pBmpClefError = XRCCTRL(*this, "bmpClefError", wxStaticBitmap);

    //// Allowed clefs
    m_pChkClef[0] = XRCCTRL(*this, "chkGClef", wxCheckBox);
    m_pChkClef[1] = XRCCTRL(*this, "chkF4Clef", wxCheckBox);
    m_pChkClef[2] = XRCCTRL(*this, "chkF3Clef", wxCheckBox);
    m_pChkClef[3] = XRCCTRL(*this, "chkC1Clef", wxCheckBox);
    m_pChkClef[4] = XRCCTRL(*this, "chkC2Clef", wxCheckBox);
    m_pChkClef[5] = XRCCTRL(*this, "chkC3Clef", wxCheckBox);
    m_pChkClef[6] = XRCCTRL(*this, "chkC4Clef", wxCheckBox);

    // allowed accidentals
    m_pChkDoubleAccidentals = XRCCTRL(*this, "chkDoubleAccidentals", wxCheckBox);
    m_pChkAccidentals = XRCCTRL(*this, "chkAccidentals", wxCheckBox);

    // problem type
    m_radProblemType = XRCCTRL(*this, "radProblemType", wxRadioBox);

    //set error icons
    wxBitmap bmpError =
         wxArtProvider::GetBitmap(_T("msg_error"), wxART_TOOLBAR, wxSize(16,16));
    m_pBmpClefError->SetBitmap(bmpError);

    //hide all error messages and their associated icons
    m_pLblClefError->Show(false);
    m_pBmpClefError->Show(false);

        //
        // initialize all controls with current constrains data
        //

    // check boxes for allowed clefs
    int i;
    for (i=0; i < 7; i++) {
        m_pChkClef[i]->SetValue( m_pConstrains->IsValidClef((EClefType)((int)lmMIN_CLEF+i) ));
    }

    // allowed accidentals.
    m_pChkAccidentals->SetValue( m_pConstrains->GetAccidentals() );
    if ( m_pChkAccidentals->GetValue() ) {
        m_pChkDoubleAccidentals->SetValue( m_pConstrains->GetDoubleAccidentals() );
        m_pChkDoubleAccidentals->Enable(true);
    }
    else
        m_pChkDoubleAccidentals->SetValue(false);

    // problem type
    m_radProblemType->SetSelection( (int)m_pConstrains->GetProblemType() );
   
    //center dialog on screen
    CentreOnScreen();

}

lmDlgCfgTheoIntervals::~lmDlgCfgTheoIntervals()
{
}

/*! Accept button will be enabled only if all data habe been validated and is Ok. So
    when accept button is clicked we can proceed to save data.
*/
void lmDlgCfgTheoIntervals::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // save allowed clefs
    int i;
    for (i=0; i < 7; i++) {
        m_pConstrains->SetClef((EClefType)((int)lmMIN_CLEF+i), m_pChkClef[i]->GetValue());
    }
    
    // save allowed accidentals
    m_pConstrains->SetAccidentals( m_pChkAccidentals->GetValue() );
    m_pConstrains->SetDoubleAccidentals( m_pChkDoubleAccidentals->GetValue() );

    // problem type
    m_pConstrains->SetProblemType((EProblemTheoIntervals)m_radProblemType->GetSelection() );
   
    //terminate the dialog 
    EndModal(wxID_OK);      
}

/*! Returns true if there are errors. If there are no  
    errors the Accept button is enabled. Otherwise it is disabled.
*/
bool lmDlgCfgTheoIntervals::VerifyData()
{
    int i;

    //assume no errors
    bool fError = false;
    bool fGlobalError = false;
    m_pLblClefError->Show(false);
    m_pBmpClefError->Show(false);
    
    // check that at least one clef is allowed
    bool fAtLeastOne = false;
    for (i=0; i < 7; i++) {
        if (m_pChkClef[i]->GetValue()) {
            fAtLeastOne = true;
            break;
        }
    }
    fError = !fAtLeastOne;
    if (fError) {
        m_pLblClefError->Show(true);
        m_pBmpClefError->Show(true);
    }
    fGlobalError |= fError;

    // enable double accidentals only if accidentals checked
    if (m_pChkAccidentals->GetValue()) {
        m_pChkDoubleAccidentals->Enable(true);
    }
    else {
        m_pChkDoubleAccidentals->SetValue(false);
        m_pChkDoubleAccidentals->Enable(false);
    }
    
    //enable / disable accept button
    wxButton* pButtonAccept = XRCCTRL(*this, "buttonAccept", wxButton);
    pButtonAccept->Enable(!fGlobalError);

    return fGlobalError;
    
}

void lmDlgCfgTheoIntervals::OnControlClicked(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}