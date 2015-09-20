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

#ifndef __LENMUS_DLG_CFG_IDFY_TONALITY_H__        //to avoid nested includes
#define __LENMUS_DLG_CFG_IDFY_TONALITY_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

//lenmus
#include "lenmus_standard_header.h"

//lomse
#include <lomse_internal_model.h>
using namespace lomse;

namespace lenmus
{

class TonalityConstrains;

//---------------------------------------------------------------------------------------
class DlgCfgIdfyTonality : public wxDialog
{
public:
    DlgCfgIdfyTonality(wxWindow* parent, TonalityConstrains* pConstrains);
    virtual ~DlgCfgIdfyTonality();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllMajor(wxCommandEvent& WXUNUSED(event));
    void OnCheckAllMinor(wxCommandEvent& WXUNUSED(event));

protected:
    bool VerifyData();
    void create_controls();


    TonalityConstrains*   m_pConstrains;      // the constraints to set up

    //controls
    wxCheckBox*     m_pChkKeySign[k_max_key+1];         // Allowed key signatures check boxes

		wxNotebook* m_pNoteBook;
		wxPanel* m_pPanelKeySignatures;
		wxCheckBox* m_pChkKeyC;
		wxCheckBox* m_pChkKeyG;
		wxCheckBox* m_pChkKeyD;
		wxCheckBox* m_pChkKeyA;
		wxCheckBox* m_pChkKeyE;
		wxCheckBox* m_pChkKeyB;
		wxCheckBox* m_pChkKeyFSharp;
		wxCheckBox* m_pChkKeyCSharp;
		wxCheckBox* m_pChkKeyCFlat;
		wxCheckBox* m_pChkKeyGFlat;
		wxCheckBox* m_pChkKeyDFlat;
		wxCheckBox* m_pChkKeyAFlat;
		wxCheckBox* m_pChkKeyEFlat;
		wxCheckBox* m_pChkKeyBFlat;
		wxCheckBox* m_pChkKeyF;
		wxCheckBox* m_pChkKeyAMinor;
		wxCheckBox* m_pChkKeyEMinor;
		wxCheckBox* m_pChkKeyBMinor;
		wxCheckBox* m_pChkKeyFSharpMinor;
		wxCheckBox* m_pChkKeyCSharpMinor;
		wxCheckBox* m_pChkKeyGSharpMinor;
		wxCheckBox* m_pChkKeyDSharpMinor;
		wxCheckBox* m_pChkKeyASharpMinor;
		wxCheckBox* m_pChkKeyAFlatMinor;
		wxCheckBox* m_pChkKeyEFlatMinor;
		wxCheckBox* m_pChkKeyBFlatMinor;
		wxCheckBox* m_pChkKeyFMinor;
		wxCheckBox* m_pChkKeyCMinor;
		wxCheckBox* m_pChkKeyGMinor;
		wxCheckBox* m_pChkKeyDMinor;

		wxButton* m_pBtCheckAllMajor;
		wxButton* m_pBtCheckAllMinor;

		wxStaticBitmap* m_pBmpKeySignError;
		wxStaticText* m_pKSErrorSpaces;
		wxStaticText* m_pLblKeySignError;

		wxPanel* m_pPanelOther;
		wxRadioBox* m_pRadAnswerType;

		wxStaticBitmap* m_pBmpGlobalError;
		wxStaticText* m_pSpace1;
		wxStaticText* m_pLblGlobalError;

		wxButton* m_pBtAccept;
		wxButton* m_pBtCancel;


    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_DLG_CFG_IDFY_TONALITY_H__
