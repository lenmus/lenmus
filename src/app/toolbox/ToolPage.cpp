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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ToolPage.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xrc/xmlres.h"
#include "wx/bmpcbox.h"
#include "wx/statline.h"
#include "wx/settings.h"

#include "ToolPage.h"


#define lmPANEL_WIDTH 150



IMPLEMENT_ABSTRACT_CLASS(lmToolPage, wxPanel)


lmToolPage::lmToolPage()
{
}

lmToolPage::lmToolPage(wxWindow* parent)
{
    Create(parent);
}

void lmToolPage::Create(wxWindow* parent)
{
    //base class
    wxPanel::Create(parent, -1, wxDefaultPosition, wxSize(lmPANEL_WIDTH, -1),
                    wxBORDER_SUNKEN|wxTAB_TRAVERSAL);

	//main sizer
    m_pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_pMainSizer);

	//set colors
	SetBackgroundColour(GetColors()->Bright());  //.Normal());

    //initializations
    m_sPageToolTip = _T("");
    m_sPageBitmapName = _T("");
}

lmToolPage::~lmToolPage()
{
}

void lmToolPage::CreateLayout()
{
    SetAutoLayout(true);
    m_pMainSizer->Fit(this);
    m_pMainSizer->SetSizeHints(this);
    m_pMainSizer->Layout();
}
