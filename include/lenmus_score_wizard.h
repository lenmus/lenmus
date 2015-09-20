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

#ifndef __LENMUS_SCORE_WIZARD_H__
#define __LENMUS_SCORE_WIZARD_H__

//lenmus
#include "lenmus_wizard.h"

#include "lenmus_injectors.h"

//lomse
#include <lomse_internal_model.h>
using namespace lomse;

//wxWidgets
#include <wx/wizard.h>
#include <wx/statline.h>
#include <wx/bmpcbox.h>

class wxListBox;
class wxStaticText;
class wxChoice;
class wxRadioBox;
class wxStaticBitmap;
class wxStaticBoxSizer;
class wxPanel;
class wxTextCtrl;


namespace lenmus
{

//forward declarations
class ScoreWizardLayout;
class ScoreWizardKeyPage;
class ScoreWizardTimePage;


//---------------------------------------------------------------------------------------
// ScoreWizard: a wizard for configuring a score
class ScoreWizard : public Wizard
{
private:
    ApplicationScope& m_appScope;
    wxString m_score;

public:
    ScoreWizard(wxWindow* parent, ApplicationScope& appScope);
    ~ScoreWizard();

    //event handlers
    void OnWizardCancel( wxWizardEvent& event );
    void OnWizardFinished( wxWizardEvent& event );
    void OnPageChanged( wxWizardEvent& event );

    void PrepareScore();
    void UpdatePreview(wxStaticBitmap* pBmpPreview);
    inline wxString get_score() { return m_score; }

protected:
    wxDECLARE_EVENT_TABLE();

};


//---------------------------------------------------------------------------------------
// ScoreWizardLayout: a wizard page to define paper layout
class ScoreWizardLayout : public WizardPage
{
    wxDECLARE_DYNAMIC_CLASS( ScoreWizardLayout );

public:
    ScoreWizardLayout();
    ScoreWizardLayout(wxWizard* parent);

    //event handlers
    void OnEnsembleSelected(wxCommandEvent& event);

    bool TransferDataFromWindow();
    void OnEnterPage();

protected:
    bool Create(wxWizard* parent);
    void CreateControls();

    wxListBox* m_pLstEnsemble;
	wxStaticBitmap* m_pBmpPreview;

    wxDECLARE_EVENT_TABLE();
};

//---------------------------------------------------------------------------------------
// ScoreWizardKeyPage: a wizard page for choosing key signatures
class ScoreWizardKeyPage: public WizardPage
{
    wxDECLARE_DYNAMIC_CLASS( ScoreWizardKeyPage );

public:
	ScoreWizardKeyPage();
    ScoreWizardKeyPage(wxWizard* parent, ApplicationScope* pAppScope);

    //event handlers
    void OnKeyType(wxCommandEvent& event);
    void OnComboKey(wxCommandEvent& event);

    bool TransferDataFromWindow();
    void OnEnterPage();

protected:
    bool Create(wxWizard* parent);
    void CreateControls();
    void LoadKeyList(int nType);

    ApplicationScope* m_pAppScope;
	wxRadioBox* m_pKeyRadioBox;
	wxBitmapComboBox* m_pKeyList;
	wxStaticBitmap* m_pBmpPreview;

    wxDECLARE_EVENT_TABLE();
};


//---------------------------------------------------------------------------------------
// ScoreWizardTimePage: a wizard page for selecting time signature
class ScoreWizardTimePage: public WizardPage
{
    wxDECLARE_DYNAMIC_CLASS( ScoreWizardTimePage );

public:
	ScoreWizardTimePage();
    ScoreWizardTimePage(wxWizard* parent);

    //event handlers
    void OnTimeType(wxCommandEvent& event);

    bool TransferDataFromWindow();
    void OnEnterPage();

protected:
    bool Create(wxWizard* parent);
    void CreateControls();
    void EnableOtherTimeSignatures(bool fEnable);

    wxStaticBoxSizer*   m_pOtherTimeSizer;

		wxRadioBox* m_pTimeRadioBox;
		wxPanel* m_pOtherTimeBoxPanel;
		wxStaticText* m_pLblTopNumber;
		wxTextCtrl* m_pTxtTopNumber;
		wxStaticText* m_pLblBottomNum;
		wxTextCtrl* m_pTxtBottomNumber;
		wxStaticBitmap* m_pBmpPreview;

    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif  // __LENMUS_SCORE_WIZARD_H__
