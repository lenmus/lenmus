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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "StatusBar.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/statusbr.h"
#endif

#if !wxUSE_STATUSBAR
    #error "You need wxWidgets compiled with wxUSE_STATUSBAR to 1 in setup.h!"
#endif // wxUSE_STATUSBAR


#include "StatusBar.h"


//Status bar fields
enum lmEStatusBarField 
{
    lm_eSB_Message = 0,
    lm_eSB_MousePos,
    lm_eSB_NumPage,
    lm_eSB_AbsTime,
    lm_eSB_RelTime,
    lm_eSB_Caps,
    lm_eSB_Nums,
    lm_eSB_NUM_FIELDS       //MUST BE THE LAST ONE
};


// ----------------------------------------------------------------------------
// lmStatusBar implementation
// ----------------------------------------------------------------------------

lmStatusBar::lmStatusBar(wxFrame* pFrame, lmEStatusBarLayout nType, wxWindowID id)
           : wxStatusBar(pFrame, wxID_ANY, wxST_SIZEGRIP)
{
    m_pFrame = pFrame;
    m_nType = lm_eStatBar_ScoreEdit;
    m_nNumFields = lm_eSB_NUM_FIELDS;
    int ch = GetCharWidth();
    const int widths[] = {-1, 15*ch, 15*ch, 15*ch, 10*ch, 4*ch, 4*ch};
    SetFieldsCount(m_nNumFields);
    SetStatusWidths(m_nNumFields, widths);
}

lmStatusBar::~lmStatusBar()
{
}

void lmStatusBar::SetMsgText(const wxString& sMsg)
{
    SetStatusText(sMsg, lm_eSB_Message);
}

void lmStatusBar::SetNumPage(int nPage)
{
    if (nPage > 0)
        SetStatusText(wxString::Format(_("Page %d"), nPage), lm_eSB_NumPage);
    else
        SetStatusText(_T(""), lm_eSB_NumPage);
}

void lmStatusBar::SetMousePos(float x, float y)
{
    wxString sMsg = wxString::Format(_T("%.2f, %.2f"), x, y);
    SetStatusText(sMsg, lm_eSB_MousePos);
}

void lmStatusBar::SetCursorRelPos(float rTime)
{
    wxString sMsg = wxString::Format(_T("%.2f"), rTime);
    SetStatusText(sMsg, lm_eSB_RelTime);
}

