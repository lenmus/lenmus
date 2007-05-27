//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ObjectParams.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ObjectParams.h"
#include "wx/html/winpars.h"



lmObjectParams::lmObjectParams(const wxHtmlTag& tag, int nWidth, int nHeight, int nPercent)
{
    // html object window attributes
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    m_nPercent = nPercent;

}

void lmObjectParams::LogError(const wxString& sMsg)
{
    //! @todo do something else with the error
    wxLogMessage(sMsg);
}

void lmObjectParams::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    wxWindow *wnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindow(), -1,
            _("Default <object> window: you MUST implement this virtual method!"),
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));

}
