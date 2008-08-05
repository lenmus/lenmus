//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_SCOREWIZARD_H__
#define __LM_SCOREWIZARD_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScoreWizard.cpp"
#endif

#include "wx/wizard.h"
#include "wx/statline.h"
#include "wx/bmpcbox.h"

#include "../widgets/Wizard.h"
#include "../score/defs.h"

class lmScoreWizardLayout;
class lmScoreWizardKey;
class lmScoreWizardTimePage;
class lmScore;


// lmScoreWizard class declaration
class lmScoreWizard: public lmWizard
{
    DECLARE_EVENT_TABLE()

public:
    lmScoreWizard(wxWindow* parent, lmScore** pPtrScore);
    ~lmScoreWizard();

    //event handlers
    void OnWizardCancel( wxWizardEvent& event );
    void OnWizardFinished( wxWizardEvent& event );
    void OnPageChanged( wxWizardEvent& event );

private:
    lmScore**       m_pPtrScore;
};


//----------------------------------------------------------------------------------
// lmScoreWizardLayout class declaration
//----------------------------------------------------------------------------------

class lmScoreWizardLayout : public lmWizardPage 
{
    DECLARE_DYNAMIC_CLASS( lmScoreWizardLayout )

public:
    lmScoreWizardLayout();
    lmScoreWizardLayout(wxWizard* parent);

    //event handlers
    void OnEnsembleSelected(wxCommandEvent& event);

    bool TransferDataFromWindow();
    void OnEnterPage();

protected:
    bool Create(wxWizard* parent);
    void CreateControls();

    wxListBox* m_pLstEnsemble;
	wxStaticText* m_pLblPaper;
	wxChoice* m_pCboPaper;
	wxRadioBox* m_pRadOrientation;
	wxStaticBitmap* m_pBmpPreview;
	
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------------
// lmScoreWizardKey class declaration
//----------------------------------------------------------------------------------

class lmScoreWizardKey: public lmWizardPage
{
    DECLARE_DYNAMIC_CLASS( lmScoreWizardKey )

public:
	lmScoreWizardKey();
    lmScoreWizardKey(wxWizard* parent);

    //event handlers
    void OnKeyType(wxCommandEvent& event);

    bool TransferDataFromWindow();
    void OnEnterPage();

protected:
    bool Create(wxWizard* parent);
    void CreateControls();
    void LoadKeyList(int nType);
    wxBitmap GenerateBitmap(wxString sKeyName);

	wxRadioBox* m_pKeyRadioBox;
	wxBitmapComboBox* m_pKeyList;
	wxStaticBitmap* m_pBmpPreview;
	
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------------
// lmScoreWizardTime class declaration
//----------------------------------------------------------------------------------

class lmScoreWizardTime: public lmWizardPage
{
    DECLARE_DYNAMIC_CLASS( lmScoreWizardTime )

public:
	lmScoreWizardTime();
    lmScoreWizardTime(wxWizard* parent);

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
	
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------------
// lmScoreWizardTitles class declaration
//----------------------------------------------------------------------------------

class lmScoreWizardTitles: public lmWizardPage
{
    DECLARE_DYNAMIC_CLASS( lmScoreWizardTitles )

public:
	lmScoreWizardTitles();
    lmScoreWizardTitles(wxWizard* parent);

    //event handlers

    bool TransferDataFromWindow();

protected:
    bool Create(wxWizard* parent);
    void CreateControls();

	wxStaticText* m_pLblTitle;
	wxTextCtrl* m_pTxtTitle;
	wxStaticText* m_pLblSubtitle;
	wxTextCtrl* m_pTxtSubtitle;
	wxStaticText* m_pLblComposer;
	wxTextCtrl* m_pTxtComposer;
	wxStaticText* m_pLblArranger;
	wxTextCtrl* m_pTxtArranger;
	wxStaticText* m_pLblLyricist;
	wxTextCtrl* m_pTxtLyricist;
	
    DECLARE_EVENT_TABLE()
};


#endif  // __LM_SCOREWIZARD_H__
