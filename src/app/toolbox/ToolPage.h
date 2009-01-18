//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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

#ifndef __LM_TOOLPAGE_H__
#define __LM_TOOLPAGE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolPage.cpp"
#endif

#include "ColorScheme.h"
#include "ToolsBox.h"
#include "ToolGroup.h"


class lmToolPage: public wxPanel
{
public:
    lmToolPage(wxWindow* parent);
    ~lmToolPage();
	
	inline wxBoxSizer* GetMainSizer() { return m_pMainSizer; }
	void CreateLayout();
	inline lmColorScheme* GetColors() { return ((lmToolBox*)GetParent())->GetColors(); }
    virtual lmToolGroup* GetToolGroup(lmEToolGroupID nGroupID) = 0;

    virtual wxString& GetPageToolTip() { return m_sPageToolTip; }
    virtual wxString& GetPageBitmapName() { return m_sPageBitmapName; }

protected:
    wxString    m_sPageToolTip;         //tool tip text
    wxString    m_sPageBitmapName;      //bitmap to use

private:
	wxBoxSizer*		m_pMainSizer;	//the main sizer for the panel
	lmColorScheme	m_colors;

};

#endif    // __LM_TOOLPAGE_H__
