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

#ifndef __LENMUS_DLG_CFG_IDFY_SCALES_H__        //to avoid nested includes
#define __LENMUS_DLG_CFG_IDFY_SCALES_H__

//wxWidgets
#include <wx/dialog.h>
class wxCheckBox;
class wxRadioBox;
class wxStaticBitmap;
class wxStaticText;
class wxWindow;

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_scales_constrains.h"
#include "lenmus_scale.h"


namespace lenmus
{

//forward declarations;
class ScalesConstrains;


//---------------------------------------------------------------------------------------
class DlgCfgIdfyScale : public wxDialog
{
public:
    DlgCfgIdfyScale(wxWindow* parent, ScalesConstrains* pConstrains,
                    bool fTheoryMode);
    virtual ~DlgCfgIdfyScale();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));


private:
    bool VerifyData();

    ScalesConstrains*  m_pConstrains;          // the constraints to set up
    bool               m_fTheoryMode;

    //controls
    wxCheckBox*     m_pChkScale[est_Max];       // Allowed chords check boxes
    wxCheckBox*     m_pChkKeySign[k_key_F+1];    // Allowed key signatures check boxes
    wxRadioBox*     m_pBoxPlayModes;            // box with play mode radio buttons
    wxCheckBox*     m_pChkDisplayKey;           // Display key signature check box

    wxStaticBitmap* m_pBmpKeySignError;
    wxStaticText*   m_pLblKeySignError;
    wxStaticBitmap* m_pBmpAllowedScalesError;
    wxStaticText*   m_pLblAllowedScalesError;


    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif    // __LENMUS_DLG_CFG_IDFY_SCALES_H__
