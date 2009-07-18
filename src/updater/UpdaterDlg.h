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

#ifndef __LM_UPDATERDLG_H__
#define __LM_UPDATERDLG_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "UpdaterDlg.cpp"
#endif

#include <wx/checklst.h>

#include "Updater.h"

class lmUpdaterDlgStart: public wxDialog
{
public:
    lmUpdaterDlgStart(wxWindow* parent);
    ~lmUpdaterDlgStart() {}

    // event handlers
    void OnProceedClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_OK); }
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }

    void AddPackage(wxString sPackage, wxString sSize, wxString sDescription);

private:

    DECLARE_EVENT_TABLE()
};

// lmUpdaterDlgInfo
class lmUpdaterDlgInfo: public wxDialog
{
public:
    lmUpdaterDlgInfo(wxWindow* parent, lmUpdater* pUpdater);
    ~lmUpdaterDlgInfo() {}

    // event handlers
    void OnDownloadClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }
    void OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event));

    void AddPackage(wxString sPackage, wxString sDescription);

private:
    lmUpdater*          m_pUpdater;     // the lmUpdater object owning this dialog

    //controls
    wxCheckListBox*     m_pUpdates;
    wxStaticText*       m_pDescription;
    wxButton*           m_pBtnDownload;
    int                 m_nNumItems;    //num items in CheckListBox

    DECLARE_EVENT_TABLE()
};


#endif    // __LM_UPDATERDLG_H__
