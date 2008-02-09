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


//table must be ordered by language name (in English) to ensure
//correspondence with table in MainFrame.cpp
enum ELanguages
{   
    eLangEnglish = 0,
    eLangFrench,
    eLangSpanish,
    eLangTurkish,
    eLangDutch,
    eLangBasque,
    eLangLast
};

typedef struct lmCompileBookOptionsStruct {
    wxString sSrcPath;
    wxString sDestPath;
    bool fLanguage[eLangLast];
    //debug options
    bool fLogTree;
    bool fDump;
} lmCompileBookOptions;


// class definition
class lmDlgCompileBook : public wxDialog {

public:
    lmDlgCompileBook(wxWindow* parent, lmCompileBookOptions* pOptions);
    virtual ~lmDlgCompileBook();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnDataChanged(wxCommandEvent& WXUNUSED(event));
    void OnBrowseSrc(wxCommandEvent& WXUNUSED(event));



private:
    bool VerifyData();

    //controls
    wxCheckBox*     m_pChkLang[eLangLast+1];        //the last one is for option 'All languages'
    wxTextCtrl*     m_pTxtSrcPath;
    wxTextCtrl*     m_pTxtDestPath;

    wxCheckBox*     m_pChkDumpTree;
    wxCheckBox*     m_pChkLogTree;


    //communication with parent
    lmCompileBookOptions*   m_pOptions;


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_DLGCOMPILEBOOK_H__
