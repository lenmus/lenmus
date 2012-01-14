//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
#ifndef __LT_DLGRUNAGDOC_H__        //to avoid nested includes
#define __LT_DLGRUNAGDOC_H__

#include "wx/wx.h"
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

// class definition
class ltDlgRunAgdoc : public wxDialog
{
public:
    ltDlgRunAgdoc(wxWindow* parent, wxString* pProject);
    virtual ~ltDlgRunAgdoc();

    // event handlers
    void OnAcceptClicked(wxCommandEvent& WXUNUSED(event));
    void OnCancelClicked(wxCommandEvent& WXUNUSED(event));
    void OnSelectProject(wxCommandEvent& WXUNUSED(event));

protected:
    void Create();

    //controls
		wxStaticText* m_pLblProject;
		wxTextCtrl* m_pTxtProject;
		wxButton* m_pBtSelect;

		wxButton* m_pBtAccept;
		wxButton* m_pBtCancel;

    //variables
    wxString*       m_pProject;

    DECLARE_EVENT_TABLE()
};

#endif    // __LT_DLGRUNAGDOC_H__
