//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
#ifndef __LM_DLGCOMPILEBOOK_H__        //to avoid nested includes
#define __LM_DLGCOMPILEBOOK_H__

// GCC interface
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "DlgCompileBook.h"
#endif

// headers
#include "wx/dialog.h"


// class definition
class lmDlgCompileBook : public wxDialog {

public:
    lmDlgCompileBook(wxWindow* parent, wxString* pSrcPath, wxString* pDestPath);
    virtual ~lmDlgCompileBook();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));
    void OnBrowseSrc(wxCommandEvent& WXUNUSED(event));



private:
    bool VerifyData();

    //controls
    wxCheckBox*     m_pChkScale;
    wxTextCtrl*     m_pTxtSrcPath;

    //communication with parent
    wxString*   m_pSrcPath;
    wxString*   m_pDestPath;



    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGCOMPILEBOOK_H__
