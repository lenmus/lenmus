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
#include "ArtProvider.h" 


//Status bar fields
enum lmEStatusBarField 
{
    lm_Field_Message = 0,
    lm_Field_MousePos,
    lm_Field_NumPage,
    lm_Field_AbsTime,
    lm_Field_RelTime,
    lm_Field_Caps,
    lm_Field_Nums,
    lm_Field_NUM_FIELDS       //MUST BE THE LAST ONE
};


// ----------------------------------------------------------------------------
// lmStatusBar implementation
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(lmStatusBar, wxStatusBar)
    EVT_SIZE(lmStatusBar::OnSize)
END_EVENT_TABLE()


lmStatusBar::lmStatusBar(wxFrame* pFrame, lmEStatusBarLayout nType, wxWindowID id)
           : wxStatusBar(pFrame, wxID_ANY, wxST_SIZEGRIP)
{
    m_pFrame = pFrame;
    m_nType = lm_eStatBar_ScoreEdit;
    m_nNumFields = lm_Field_NUM_FIELDS;
    int ch = GetCharWidth();

    const int widths[] = {-1, 15*ch, 15*ch, 15*ch, 10*ch, 4*ch, 4*ch};
    SetFieldsCount(m_nNumFields);
    SetStatusWidths(m_nNumFields, widths);

	//icon space
	wxSize size(16,16);
	int nWidth, nHeight;
	GetTextExtent(_T(" "), &nWidth, &nHeight);
	size_t nSpaces = size_t(0.5f + ((4.0f + float(size.x)) / (float)nWidth) );
	//wxLogMessage(wxString::Format(_T("nSpaces=%d"), nSpaces));
	m_sIconSpace.insert(size_t(0), nSpaces, _T(' '));

	//add bitmap for time
    m_pBmpClock = new wxStaticBitmap(this, wxID_ANY, 
									 wxArtProvider::GetIcon(_T("status_time"), wxART_TOOLBAR, size) );

	//add bitmap for page num
    m_pBmpPage = new wxStaticBitmap(this, wxID_ANY, 
									 wxArtProvider::GetIcon(_T("status_page"), wxART_TOOLBAR, size) );

	SetMinHeight(size.y);
}

lmStatusBar::~lmStatusBar()
{
}

void lmStatusBar::SetMsgText(const wxString& sMsg)
{
    SetStatusText(sMsg, lm_Field_Message);
}

void lmStatusBar::SetNumPage(int nPage)
{
    if (nPage > 0)
        SetStatusText(wxString::Format(_("%s%d"), m_sIconSpace, nPage), lm_Field_NumPage);
    else
        SetStatusText(_T(""), lm_Field_NumPage);
}

void lmStatusBar::SetMousePos(float x, float y)
{
    SetStatusText(wxString::Format(_T("%.2f, %.2f"), x, y), lm_Field_MousePos);
}

void lmStatusBar::SetCursorRelPos(float rTime)
{
    SetStatusText(wxString::Format(_T("%s%.2f"), m_sIconSpace, rTime), lm_Field_RelTime);
}

void lmStatusBar::OnSize(wxSizeEvent& event)
{
	//position bitmaps in the appropiate field area
    
	//page
	wxRect rect;
    GetFieldRect(lm_Field_NumPage, rect);
    wxSize size = m_pBmpPage->GetSize();
    m_pBmpPage->Move(rect.x,
                     rect.y + (rect.height - size.y) / 2);

	//clock
    GetFieldRect(lm_Field_RelTime, rect);
    size = m_pBmpClock->GetSize();
    m_pBmpClock->Move(rect.x,
                      rect.y + (rect.height - size.y) / 2);

    event.Skip();
}

