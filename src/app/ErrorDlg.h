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

#ifndef __LM_ERRORDLG_H__        //to avoid nested includes
#define __LM_ERRORDLG_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ErrorDlg.cpp"
#endif

// headers
#include <wx/dialog.h>
#include <wx/button.h>


class lmErrorDlg : public wxDialog
{

public:
    lmErrorDlg(wxWindow* pParent, wxString sErrorType, wxString sErrorMsg);
    ~lmErrorDlg();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event));


private:
    wxStaticText*   m_pTxtType;
    wxStaticText*   m_pTxtMsge;

    DECLARE_EVENT_TABLE()
};

#endif    // __LM_ERRORDLG_H__
