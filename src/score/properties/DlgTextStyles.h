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

#ifndef __LM_DLGTEXTSTYLES_H__        //to avoid nested includes
#define __LM_DLGTEXTSTYLES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DlgTextStyles.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"

#else
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/dialog.h>

#endif


//--------------------------------------------------------------------------------------
/// Class lmDlgTextStyles
//--------------------------------------------------------------------------------------

class lmDlgTextStyles : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxGrid* m_pGrid;
		wxRadioBox* m_radioBox1;
		wxButton* m_button11;
		wxButton* m_button21;
		wxButton* m_button211;
		wxButton* m_button6;
		
		wxButton* m_button3;
		
		wxButton* m_button4;
		
	
	public:
		lmDlgTextStyles( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Text styles defined in current score"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 530,400 ), long style = wxDEFAULT_DIALOG_STYLE );
		~lmDlgTextStyles();

private:
    void CreateControls();
	
};

#endif //__LM_DLGTEXTSTYLES_H__
