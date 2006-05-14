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
/*! @file UpdaterDlg.h
    @brief Header file for all dialog classes used only by the updater
    @ingroup updates_management
*/
#ifndef __UPDATERDLG_H__
#define __UPDATERDLG_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "lmUpdaterDlg.cpp"
#endif

#include <wx/checklst.h>

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
    lmUpdaterDlgInfo(wxWindow* parent);
    ~lmUpdaterDlgInfo() {}

    // event handlers
    void OnDownloadClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event)) { EndDialog(wxID_CANCEL); }

    void AddPackage(wxString sPackage, wxString sSize, wxString sDescription);

private:
    //controls
    wxCheckListBox*     m_pUpdates;
    wxStaticText*       m_pDescription;

    DECLARE_EVENT_TABLE()
};


#endif    // __UPDATERDLG_H__
