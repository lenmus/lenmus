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

#include "lenmus_wizard.h"

//xwWidgets
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/statline.h"




namespace lenmus
{

//---------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// WizardPage
// ----------------------------------------------------------------------------

WizardPage::WizardPage(wxWizard* pParent)
    : wxWizardPageSimple(pParent, nullptr, nullptr, wxNullBitmap)     //wx2.8     , nullptr)
{
    m_pParent = pParent;
    m_fOptional = false;
}


// ----------------------------------------------------------------------------
// Wizard implementation
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(Wizard, wxDialog)
    EVT_BUTTON(wxID_CANCEL, Wizard::OnCancel)
    EVT_BUTTON(wxID_BACKWARD, Wizard::OnPageButton)
    EVT_BUTTON(wxID_FORWARD, Wizard::OnPageButton)
    EVT_BUTTON(wxID_HELP, Wizard::OnHelp)

    EVT_WIZARD_PAGE_CHANGED(wxID_ANY, Wizard::OnWizardEvent)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, Wizard::OnWizardEvent)
    EVT_WIZARD_CANCEL(wxID_ANY, Wizard::OnWizardEvent)
    EVT_WIZARD_FINISHED(wxID_ANY, Wizard::OnWizardEvent)
    EVT_WIZARD_HELP(wxID_ANY, Wizard::OnWizardEvent)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
Wizard::Wizard(wxWindow *parent, int id, const wxString& title,
                   const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    m_posWizard = pos;
    m_pFirstPage = (WizardPage*)nullptr;

    CreateControls();
}

//---------------------------------------------------------------------------------------
Wizard::~Wizard()
{
}

//---------------------------------------------------------------------------------------
void Wizard::Run()
{
    if (m_pFirstPage)
        ShowPage(m_pFirstPage, true);

    GetSizer()->SetSizeHints(this);
    if (m_posWizard == wxDefaultPosition)
        CentreOnScreen();
    ShowModal();
}

//---------------------------------------------------------------------------------------
void Wizard::CreateControls()
{
	m_pWindowSizer = new wxBoxSizer( wxVERTICAL );

    //the current page
	m_pCurPage = (WizardPage*)new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_pWindowSizer->Add( m_pCurPage, 1, wxEXPAND | wxALL, 5 );

    //add separation line
	wxStaticLine* pStatLine = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_pWindowSizer->Add( pStatLine, 0, wxEXPAND | wxALL, 5 );

    //Add buttons sizer and buttons
	wxBoxSizer* pButtonsSizer;
	pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	m_pWindowSizer->Add( pButtonsSizer, 0, wxEXPAND, 5 );

    //help button
    if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
    {
	    wxButton* pBtnHelp = new wxButton( this, wxID_HELP, _("Help"), wxDefaultPosition, wxDefaultSize, 0 );
	    pButtonsSizer->Add( pBtnHelp, 0, wxALL, 5 );
    }
    //spacer
	pButtonsSizer->Add( 20, 0, 1, wxEXPAND, 5 );

    //Back button
	m_pBtnPrev = new wxButton( this, wxID_BACKWARD, _("< Previous"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtnPrev, 0, wxALL, 5 );

    //Next button
	m_pBtnNext = new wxButton( this, wxID_FORWARD, _("Next >"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtnNext, 0, wxALL, 5 );

    //Finish button
	m_pBtnFinish = new wxButton( this, wxID_FORWARD, _("Finish"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( m_pBtnFinish, 0, wxALL, 5 );

	//spacer
	pButtonsSizer->Add( 20, 0, 1, wxEXPAND, 5 );

    //Cancel button
	wxButton* pBtnCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	pButtonsSizer->Add( pBtnCancel, 0, wxALL, 5 );

	SetSizer(m_pWindowSizer);
}

//---------------------------------------------------------------------------------------
void Wizard::OnCancel(wxCommandEvent& WXUNUSED(eventUnused))
{
    EndModal(wxID_CANCEL);

    //notify the user
    wxWizardEvent event(wxEVT_WIZARD_CANCEL, GetId(), false, 0);
    (void)GetEventHandler()->ProcessEvent(event);
}

//---------------------------------------------------------------------------------------
void Wizard::AddPage(WizardPage* pPage, bool fOptional)
{
    if (!m_pFirstPage)
        m_pFirstPage = pPage;
    else
        //chain pages
        wxWizardPageSimple::Chain(m_pLastPage, pPage);

    m_pLastPage = pPage;
    pPage->SetOptional(fOptional);
}

//---------------------------------------------------------------------------------------
bool Wizard::ShowPage(WizardPage* page, bool fGoingFwd)
{
    //if there is a page displayed ask it if no veto to change it
    if (m_pCurPage)
    {
        // send the event to the old page
        wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGING, GetId(), fGoingFwd, m_pCurPage);
        if (m_pCurPage->GetEventHandler()->ProcessEvent(event) && !event.IsAllowed())
        {
            //moving to this page is vetoed by previous page
            return false;
        }
        m_pCurPage->Hide();
    }

    //set the new page
    WizardPage* pOldPage = m_pCurPage;
    m_pCurPage = page;

    //if there is no page, this the end
    if (!m_pCurPage)
    {
        FinishWizard();
        return true;            // terminate successfully
    }

    //position and show the new page
    (void)m_pCurPage->TransferDataToWindow();

    //update buttons state
    UpdateButtons();

    //send the change event to the new page
    wxWizardEvent event(wxEVT_WIZARD_PAGE_CHANGED, GetId(), fGoingFwd, m_pCurPage);
    (void)m_pCurPage->GetEventHandler()->ProcessEvent(event);

    //and finally show it
    m_pWindowSizer->Replace(pOldPage, m_pCurPage);
    m_pCurPage->Show();
    m_pCurPage->SetFocus();
    GetSizer()->Layout();

    return true;
}

//---------------------------------------------------------------------------------------
void Wizard::FinishWizard()
{
    EndModal(wxID_OK);

    //notify the user
    wxWizardEvent event(wxEVT_WIZARD_FINISHED, GetId(), false, 0);
    (void)GetEventHandler()->ProcessEvent(event);
}

//---------------------------------------------------------------------------------------
void Wizard::UpdateButtons()
{
    m_pBtnPrev->Enable(HasPrevPage(m_pCurPage));

    bool fNext = HasNextPage(m_pCurPage);
    m_pBtnNext->Enable(fNext);
    m_pBtnFinish->Enable(!fNext || IsOptional(m_pCurPage));

    if (fNext)
        m_pBtnNext->SetDefault();
}

//---------------------------------------------------------------------------------------
void Wizard::OnPageButton(wxCommandEvent& event)
{
    wxASSERT_MSG( (event.GetEventObject() == m_pBtnNext) ||
                  (event.GetEventObject() == m_pBtnPrev) ||
                  (event.GetEventObject() == m_pBtnFinish),
                  _("unknown button") );

    wxCHECK_RET( m_pCurPage, "should have a valid current page" );

    // ask the current page first: notice that we do it before calling
    // GetNext/Prev() because the data transfered from the controls of the page
    // may change the value returned by these methods
    if ( !m_pCurPage->Validate() || !m_pCurPage->TransferDataFromWindow() )
    {
        // the page data is incorrect, don't do anything
        return;
    }

    bool fForward = event.GetEventObject() == m_pBtnNext;

    WizardPage* pNextPage;
    if (fForward)
    {
        pNextPage = (WizardPage*)m_pCurPage->GetNext();
    }
    else if ( event.GetEventObject() == m_pBtnFinish )
    {
        FinishWizard();      // terminate successfully
        return;
    }
    else // back
    {
        pNextPage = (WizardPage*)m_pCurPage->GetPrev();

        wxASSERT_MSG( pNextPage, "\"<Back\" button should have been disabled" );
    }

    // just pass to the new page (or maybe not - but we don't care here)
    (void)ShowPage(pNextPage, fForward);
}

//---------------------------------------------------------------------------------------
void Wizard::OnHelp(wxCommandEvent& WXUNUSED(event))
{
    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    if(m_pCurPage != nullptr)
    {
        // Create and send the help event to the specific page handler
        // event data contains the active page so that context-sensitive
        // help is possible
        wxWizardEvent eventHelp(wxEVT_WIZARD_HELP, GetId(), true, m_pCurPage);
        (void)m_pCurPage->GetEventHandler()->ProcessEvent(eventHelp);
    }
}

//---------------------------------------------------------------------------------------
void Wizard::OnWizardEvent(wxWizardEvent& event)
{
    // the dialogs have wxWS_EX_BLOCK_EVENTS style on by default but we want to
    // propagate wxEVT_WIZARD_XXX to the parent (if any), so do it manually
    if ( !(GetExtraStyle() & wxWS_EX_BLOCK_EVENTS) )
    {
        // the event will be propagated anyhow
        event.Skip();
    }
    else
    {
        wxWindow *parent = GetParent();

        if ( !parent || !parent->GetEventHandler()->ProcessEvent(event) )
        {
            event.Skip();
        }
    }
}

//---------------------------------------------------------------------------------------
void Wizard::EnableButtonNext(bool fEnable)
{
    if (fEnable)
        m_pBtnNext->SetDefault();
    else if (m_pBtnFinish->IsEnabled())
    {
        m_pBtnFinish->SetDefault();
        //TODO:
        //Looks like a wxWidgets bug:
        //SetDefault doesn't cause to remove the selection of button next. Probably it is
        //delayed to idle time. But when redraw is done, as button next
        //is disabled by next instruction, the selection is not removed.
    }

    m_pBtnNext->Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void Wizard::EnableButtonPrev(bool fEnable)
{
    m_pBtnPrev->Enable(fEnable);
}


}   // namespace lenmus
