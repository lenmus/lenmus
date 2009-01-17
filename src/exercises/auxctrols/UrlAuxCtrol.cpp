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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "UrlAuxCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "UrlAuxCtrol.h"
#include "../../html/HtmlWindow.h"

//colors
#include "../../globals/Colors.h"
extern lmColors* g_pColors;


//-------------------------------------------------------------------------------------
// Implementation of lmUrlAuxCtrol

DEFINE_EVENT_TYPE(lmEVT_URL_CLICK)


BEGIN_EVENT_TABLE(lmUrlAuxCtrol, wxStaticText)
    EVT_LEFT_DOWN    (lmUrlAuxCtrol::OnClick)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmUrlAuxCtrol, wxStaticText)


lmUrlAuxCtrol::lmUrlAuxCtrol(wxWindow* parent, wxWindowID id, double rScale,
                             const wxString& sNormalLabel, 
                             const wxString& sAltLabel,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
    : wxStaticText(parent, id, sNormalLabel, pos, size, style, name)
{
    wxFont font = parent->GetFont();
    font.SetUnderlined(true);
    double rCurSize = (double)font.GetPointSize();
    font.SetPointSize( (int)(rCurSize * rScale) );
    SetFont(font);
    SetForegroundColour(g_pColors->HtmlLinks());
    SetCursor(wxCURSOR_HAND);

    m_sNormalLabel = sNormalLabel;
    m_sAltLabel = sAltLabel;
}

void lmUrlAuxCtrol::OnClick(wxMouseEvent& event)
{
    wxCommandEvent eventCustom(lmEVT_URL_CLICK);
    eventCustom.SetId(event.GetId());
    GetParent()->GetEventHandler()->ProcessEvent( eventCustom );

}

