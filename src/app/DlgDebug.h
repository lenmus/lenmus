//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_DLGDEBUG_H__        //to avoid nested includes
#define __LM_DLGDEBUG_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgDebug.cpp"
#endif


#include <wx/dialog.h>

#include <wx/html/htmlwin.h>        //GCC complains of forward declaration of wxHtmlWindow !
//class wxHtmlWindow;
class wxButton;
class wxTextCtrl;
class wxWindow;


class lmDlgDebug : public wxDialog
{
   DECLARE_DYNAMIC_CLASS(lmDlgDebug)

public:
    lmDlgDebug(wxWindow* parent, wxString sTitle, wxString sData);
    virtual ~lmDlgDebug();

    void OnOK(wxCommandEvent& WXUNUSED(event));
    void OnSave(wxCommandEvent& WXUNUSED(event));

    void AppendText(wxString sText);

private:
    wxTextCtrl*        m_pTxtData;

    DECLARE_EVENT_TABLE()
};

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

    DECLARE_EVENT_TABLE()
};


#endif    // __LM_DLGDEBUG_H__
