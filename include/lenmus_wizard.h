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

#ifndef __LENMUS_WIZARD_H__        //to avoid nested includes
#define __LENMUS_WIZARD_H__

#include <list>

//wxWidgets
#include <wx/wizard.h>



namespace lenmus
{

//forward declarations
class Wizard;

//---------------------------------------------------------------------------------------
// WizardPage: a page for a wizard
class WizardPage : public wxWizardPageSimple
{
public:
    WizardPage() : wxWizardPageSimple(), m_fOptional(false) {}
    WizardPage(wxWizard *parent);

    inline bool IsOptional() { return m_fOptional; }
    inline void SetOptional(bool fValue) { m_fOptional = fValue; }

    virtual void OnEnterPage() {}

    wxWizard* GetParentWizard() { return m_pParent; }

private:
    wxWizard*   m_pParent;
    bool        m_fOptional;

};


//---------------------------------------------------------------------------------------
// Wizard: A wizard with optional pages
class Wizard : public wxDialog
{
public:
    Wizard(wxWindow *parent, int id = wxID_ANY, const wxString& title = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~Wizard();

    //adding pages
    void AddPage(WizardPage* pPage, bool fOptional=false);

    void Run();

    virtual bool HasNextPage(WizardPage* page)
        { return page->GetNext() != nullptr; }

    virtual bool HasPrevPage(WizardPage* page)
        { return page->GetPrev() != nullptr; }

    virtual bool IsOptional(WizardPage* page)
        { return ((WizardPage*)page)->IsOptional(); }

    //buttons
    void EnableButtonNext(bool fEnable);
    void EnableButtonPrev(bool fEnable);


private:
    void CreateControls();
    void UpdateButtons();
    void FinishWizard();

    // event handlers
    void OnCancel(wxCommandEvent& event);
    void OnPageButton(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);

    void OnWizardEvent(wxWizardEvent& event);

    // show the prev/next page, but call TransferDataFromWindow on the current
    // page first and return false without changing the page if
    // TransferDataFromWindow() returns false - otherwise, returns true
    bool ShowPage(WizardPage* pPage, bool fGoingFwd = true);



    wxBoxSizer*     m_pWindowSizer;   //sizer item for page panel
    wxPoint         m_posWizard;    //dialog position from the ctor

    // wizard state
    WizardPage*   m_pCurPage;         //current page or nullptr

    // wizard controls
    wxButton*       m_pBtnPrev;      //"<Back" button
    wxButton*       m_pBtnNext;      //"Next>" button
    wxButton*       m_pBtnFinish;   //"Finish" button

    WizardPage*   m_pFirstPage;
    WizardPage*   m_pLastPage;

    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif  // __LENMUS_WIZARD_H__
