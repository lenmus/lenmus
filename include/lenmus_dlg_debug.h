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

#ifndef __LENMUS_DLG_DEBUG_H__        //to avoid nested includes
#define __LENMUS_DLG_DEBUG_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/dialog.h>
#include <wx/html/htmlwin.h>        //GCC complains if forward declaration of wxHtmlWindow !
//class wxHtmlWindow;

//forward declarations
class wxButton;
class wxTextCtrl;
class wxWindow;
class wxStaticText;

namespace lenmus
{

//---------------------------------------------------------------------------------------
class DlgDebug : public wxDialog
{
   wxDECLARE_DYNAMIC_CLASS(DlgDebug);

public:
    DlgDebug(wxWindow* parent, wxString sTitle, wxString sData, bool fSave = true);
    virtual ~DlgDebug();

    void OnOK(wxCommandEvent& WXUNUSED(event));
    void OnSave(wxCommandEvent& WXUNUSED(event));

    void AppendText(wxString sText);

private:
    wxTextCtrl*     m_pTxtData;
    bool            m_fSave;        //true to include 'Save' button

    wxDECLARE_EVENT_TABLE();
};

//---------------------------------------------------------------------------------------
class lmHtmlDlg : public wxDialog
{
public:
	lmHtmlDlg(wxWindow* pParent, const wxString& sTitle, bool fSaveButton = false);
	~lmHtmlDlg();

    void CreateControls(bool fSaveButton);
    void SetContent(const wxString& sContent);

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnSaveClicked(wxCommandEvent& WXUNUSED(event));


private:
	wxHtmlWindow*   m_pHtmlWnd;
	wxButton*       m_pBtnAccept;
	wxButton*       m_pBtnSave;

    wxDECLARE_EVENT_TABLE();
};

//---------------------------------------------------------------------------------------
class DlgSpacingParams : public wxDialog
{
   wxDECLARE_DYNAMIC_CLASS(DlgSpacingParams);

protected:
    float m_force;
    float m_alpha;
    float m_dmin;

    wxStaticText* m_lblForce;
    wxTextCtrl* m_txtForce;
    wxStaticText* m_lblAlpha;
    wxTextCtrl* m_txtAlpha;
    wxStaticText* m_lblDmin;
    wxTextCtrl* m_txtDmin;

    wxButton* m_cmdUpdate;
    wxButton* m_cmdClose;

public:
    DlgSpacingParams(wxWindow* parent, float force, float alpha, float dmin);
    virtual ~DlgSpacingParams();

    void on_close(wxCommandEvent& WXUNUSED(event));
    void on_update(wxCommandEvent& WXUNUSED(event));

protected:
    void update_params();


    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif    // __LENMUS_DLG_DEBUG_H__
