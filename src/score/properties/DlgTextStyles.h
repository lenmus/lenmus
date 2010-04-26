//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_DLGTEXTSTYLES_H__        //to avoid nested includes
#define __LM_DLGTEXTSTYLES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgTextStyles.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

class wxGrid;

//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//
//#else
//#include <wx/string.h>
//#include <wx/stattext.h>
//#include <wx/gdicmn.h>
//#include <wx/font.h>
//#include <wx/colour.h>
//#include <wx/settings.h>
#include <wx/grid.h>
//#include <wx/sizer.h>
//#include <wx/radiobox.h>
//#include <wx/button.h>
//#include <wx/dialog.h>
//
//#endif


class lmScore;

//--------------------------------------------------------------------------------------
/// Class lmDlgTextStyles
//--------------------------------------------------------------------------------------

class lmDlgTextStyles : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxGrid* m_pGrid;
		wxRadioBox* m_pRadShow;
		wxButton* m_pBtAddNew;
		wxButton* m_pBtRemove;
		wxButton* m_pBtAddDefault;
		wxButton* m_pBtEdit;

		wxButton* m_pBtAccept;

		wxButton* m_pBtCancel;


	public:
		lmDlgTextStyles( wxWindow* parent, lmScore* pScore);
		~lmDlgTextStyles();



    // event handlers
    void OnAccept(wxCommandEvent& WXUNUSED(event));
    void OnCancel(wxCommandEvent& WXUNUSED(event));
    void OnAddNew(wxCommandEvent& WXUNUSED(event));
    void OnAddDefault(wxCommandEvent& WXUNUSED(event));
    void OnRemove(wxCommandEvent& WXUNUSED(event));
    void OnEdit(wxGridEvent& event);

private:
    void CreateControls();
    void LoadStyles();


    lmScore*        m_pScore;

    DECLARE_EVENT_TABLE()
};

#endif //__LM_DLGTEXTSTYLES_H__
