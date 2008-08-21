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

#ifndef __LM_WIZARD_H__        //to avoid nested includes
#define __LM_WIZARD_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Wizard.cpp"
#endif

#include <list>

#include "wx/wizard.h"


class lmWizard;

//-----------------------------------------------------------------------------
// lmWizardPage: 
//-----------------------------------------------------------------------------

class lmWizardPage : public wxWizardPageSimple
{
public:
    lmWizardPage() : wxWizardPageSimple(), m_fOptional(false) {}
    lmWizardPage(wxWizard *parent);

    inline bool IsOptional() { return m_fOptional; }
    inline void SetOptional(bool fValue) { m_fOptional = fValue; }

    virtual void OnEnterPage() {}

    wxWizard* GetParentWizard() { return m_pParent; }

private:
    wxWizard*   m_pParent;
    bool        m_fOptional;

};


//-----------------------------------------------------------------------------------
// lmWizard: A wizard with optional pages
//-----------------------------------------------------------------------------------

class lmWizard : public wxDialog
{
public:
    lmWizard(wxWindow *parent, int id = wxID_ANY, const wxString& title = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition, long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~lmWizard();

    //adding pages
    void AddPage(lmWizardPage* pPage, bool fOptional=false);

    void Run();

    virtual bool HasNextPage(lmWizardPage* page)
        { return page->GetNext() != NULL; }

    virtual bool HasPrevPage(lmWizardPage* page)
        { return page->GetPrev() != NULL; }

    virtual bool IsOptional(lmWizardPage* page)
        { return ((lmWizardPage*)page)->IsOptional(); }

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
    bool ShowPage(lmWizardPage* pPage, bool fGoingFwd = true);


    
    wxBoxSizer*     m_pWindowSizer;   //sizer item for page panel
    wxPoint         m_posWizard;    //dialog position from the ctor

    // wizard state
    lmWizardPage*   m_pCurPage;         //current page or NULL

    // wizard controls
    wxButton*       m_pBtnPrev;      //"<Back" button
    wxButton*       m_pBtnNext;      //"Next>" button
    wxButton*       m_pBtnFinish;   //"Finish" button

    lmWizardPage*   m_pFirstPage;
    lmWizardPage*   m_pLastPage;

    DECLARE_EVENT_TABLE()
};


#endif  // __LM_WIZARD_H__
