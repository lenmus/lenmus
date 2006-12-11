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

//for GCC
#ifdef __GNUG__
    #pragma implementation "DlgCompileBook.h"
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

#include "DlgCompileBook.h"



//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmDlgCompileBook, wxDialog)
    EVT_BUTTON( XRCID( "btOK" ), lmDlgCompileBook::OnAcceptClicked )
    EVT_BUTTON( XRCID( "btCancel" ), lmDlgCompileBook::OnCancelClicked )

    // Language check boxes
    EVT_CHECKBOX( XRCID( "chkLangEnglish" ), lmDlgCompileBook::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkLangFrench" ), lmDlgCompileBook::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkLangSpanish" ), lmDlgCompileBook::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkLangTurkish" ), lmDlgCompileBook::OnDataChanged )
    EVT_CHECKBOX( XRCID( "chkLangAll" ), lmDlgCompileBook::OnDataChanged )

    EVT_BUTTON( XRCID( "btBrowseSrc" ), lmDlgCompileBook::OnBrowseSrc )

END_EVENT_TABLE()



lmDlgCompileBook::lmDlgCompileBook(wxWindow* parent, lmCompileBookOptions* pOptions)
{
    //save parameters
    m_pOptions = pOptions;

    // create the dialog controls
    wxXmlResource::Get()->LoadDialog(this, parent, _T("DlgCompileBook"));

    //get pointers to all controls
    m_pChkLang[eLangEnglish] = XRCCTRL(*this, "chkLangEnglish", wxCheckBox);
    m_pChkLang[eLangFrench] = XRCCTRL(*this, "chkLangFrench", wxCheckBox);
    m_pChkLang[eLangSpanish] = XRCCTRL(*this, "chkLangSpanish", wxCheckBox);
    m_pChkLang[eLangTurkish] = XRCCTRL(*this, "chkLangTurkish", wxCheckBox);
    m_pChkLang[eLangLast] = XRCCTRL(*this, "chkLangAll", wxCheckBox);
    m_pTxtSrcPath = XRCCTRL(*this, "txtSrcPath", wxTextCtrl);     
    m_pTxtDestPath = XRCCTRL(*this, "txtDestPath", wxTextCtrl); 

    // initialize all controls with current data
    for(int i=0; i < eLangLast; i++) {
        m_pOptions->fLanguage[i] = false;
        m_pChkLang[i]->SetValue(false);
    }
    m_pChkLang[eLangLast]->SetValue(false);

    m_pTxtSrcPath->SetValue( m_pOptions->sSrcPath );
    m_pTxtDestPath->SetValue( m_pOptions->sDestPath );

    //center dialog on screen
    CentreOnScreen();

}

lmDlgCompileBook::~lmDlgCompileBook()
{
}

void lmDlgCompileBook::OnDataChanged(wxCommandEvent& WXUNUSED(event))
{
    VerifyData();
}

void lmDlgCompileBook::OnAcceptClicked(wxCommandEvent& WXUNUSED(event))
{
    // Accept button will be enabled only if all data have been validated and is Ok. So
    // when accept button is clicked we can proceed to save data.

    //move language options
    if (m_pChkLang[eLangLast]->GetValue()) {
        for(int i=0; i < eLangLast; i++) {
             m_pOptions->fLanguage[i] = true;
        }
    }
    else {
        for(int i=0; i < eLangLast; i++) {
            m_pOptions->fLanguage[i] = m_pChkLang[i]->GetValue();
        }
    }

    //terminate the dialog 
    EndModal(wxID_OK);      
}

bool lmDlgCompileBook::VerifyData()
{
    // Returns a boolean to enable or not a tab change. That is: returns true if there are
    // local errors (errors affecting only to the data in a tab) so not to enable a tab
    // change. If there are no tab local errors then returns false (althought it there might
    // be global errors -- coherence between data in different tabs --).
    //
    // Anyway, global errors al also checked. If there are no global neither local 
    // errors the Accept button is enabled. Otherwise it is disabled.

    //bool fAtLeastOne;
    //int i;

    ////assume no errors
    //bool fError = false;
    //bool fLocalError = false;
    //bool fGlobalError = false;

    //m_pLblKeySignError->Show(false);
    //m_pBmpKeySignError->Show(false);
    //m_pLblAllowedScalesError->Show(false);
    //m_pBmpAllowedScalesError->Show(false);
    //
    //// check that at least one scale is selected
    //fError = false;
    //fAtLeastOne = false;
    //for (i=0; i < est_Max; i++) {
    //    if (m_pChkScale[i]->GetValue()) {
    //        fAtLeastOne = true;
    //        break;
    //    }
    //}


    //m_pChkLang[eLangEnglish] = XRCCTRL(*this, "chkLangEnglish", wxCheckBox);
    //m_pChkLang[eLangFrench] = XRCCTRL(*this, "chkLangFrench", wxCheckBox);
    //m_pChkLang[eLangSpanish] = XRCCTRL(*this, "chkLangSpanish", wxCheckBox);
    //m_pChkLang[eLangTurkish] = XRCCTRL(*this, "chkLangTurkish", wxCheckBox);
    //m_pChkLang[eLangLast] = XRCCTRL(*this, "chkLangAll", wxCheckBox);


    //fError = !fAtLeastOne;
    //if (fError) {
    //    m_pLblAllowedScalesError->Show(true);
    //    m_pBmpAllowedScalesError->Show(true);
    //}
    //fLocalError |= fError;
    //
    //// check that at least one key signature has been choosen
    //fAtLeastOne = false;
    //for (i=0; i < earmFa+1; i++) {
    //    fAtLeastOne |= m_pChkKeySign[i]->GetValue();
    //}
    //fError = !fAtLeastOne;
    //if (fError) {
    //    m_pLblKeySignError->Show(true);
    //    m_pBmpKeySignError->Show(true);
    //}
    //fLocalError |= fError;

    // 
    //    //
    //    // Check for global errors
    //    //
    //
    //fGlobalError = false;   //no global checkings in this dlg

  
    ////enable / disable accept button
    //wxButton* pButtonOK = XRCCTRL(*this, "btOK", wxButton);
    //pButtonOK->Enable(!fLocalError && !fGlobalError);

    //return fLocalError;
    return false;
    
}

void lmDlgCompileBook::OnBrowseSrc(wxCommandEvent& WXUNUSED(event))
{
    // ask for the file to covert
    wxString sFilter = wxT("*.xml");
    wxString sPath = ::wxFileSelector(_T("Choose the file to convert"),
                                        wxT(""),    //default path
                                        wxT(""),    //default filename
                                        wxT("xml"),    //default_extension
                                        sFilter,
                                        wxOPEN,        //flags
                                        this);
    if ( sPath.IsEmpty() ) return;
    m_pOptions->sSrcPath = sPath;
    m_pTxtSrcPath->SetValue(sPath);

}

