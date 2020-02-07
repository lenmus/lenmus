//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
//---------------------------------------------------------------------------------------

//lenmus
#include "lenmus_status_bar.h"
#include "lenmus_standard_header.h"

#include "lenmus_art_provider.h"
#include "lenmus_string.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/statusbr.h>

#if !wxUSE_STATUSBAR
    #error "You need wxWidgets compiled with wxUSE_STATUSBAR to 1 in setup.h!"
#endif


namespace lenmus
{


//Status bar fields
enum EStatusBarField
{
    lm_Field_Message = 0,
    lm_Field_MousePos,
    lm_Field_MouseCaret,
    lm_Field_NumPage,
    lm_Field_RelTime,
    lm_Field_NUM_FIELDS       //MUST BE THE LAST ONE
};

enum {
    lmID_BUTTON_MOUSE = 1300,
    lmID_BUTTON_CARET,
};



//=======================================================================================
// StatusBar implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(StatusBar, wxStatusBar)
    EVT_SIZE(StatusBar::OnSize)
    EVT_BUTTON(lmID_BUTTON_MOUSE, StatusBar::OnButtonMouse)
    EVT_BUTTON(lmID_BUTTON_CARET, StatusBar::OnButtonCaret)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
StatusBar::StatusBar(wxFrame* pFrame, EStatusBarLayout WXUNUSED(nType), wxWindowID id)
    : wxStatusBar(pFrame, id, wxST_SIZEGRIP)
    , StatusReporter()
    , m_pFrame(pFrame)
    , m_nNumFields(lm_Field_NUM_FIELDS)
    , m_nType(lm_eStatBar_ScoreEdit)
    , m_fMouseDisplay(true)
    , m_nMousePage(0)
    , m_rMouseTime(0.0)
    , m_nMouseMeasure(0)
    , m_nCaretPage(0)
    , m_rCaretTime(0.0)
    , m_nCaretMeasure(0)
{
    int ch = GetCharWidth();

    //                        Mouse  Mouse  Num   Rel
    //                    Msg Pos    Caret  Page  Time
    const int widths[] = {-1, 25*ch, 8*ch,  8*ch, 18*ch};
    SetFieldsCount(m_nNumFields);
    SetStatusWidths(m_nNumFields, widths);

	//icon space
	wxSize size(16,16);
	int nWidth, nHeight;
	GetTextExtent(" ", &nWidth, &nHeight);
	size_t nSpaces = size_t(0.5f + ((4.0f + float(size.x)) / (float)nWidth) );
	//wxLogMessage(wxString::Format("nSpaces=%d", nSpaces));
	m_sIconSpace.insert(size_t(0), nSpaces, ' ');

	//load bitmaps
    m_pBmpClock = LENMUS_NEW
        wxStaticBitmap(this, wxID_ANY, wxArtProvider::GetIcon("status_time",
        wxART_TOOLBAR, size) );
    m_pBmpPage = LENMUS_NEW
        wxStaticBitmap(this, wxID_ANY, wxArtProvider::GetIcon("status_page",
        wxART_TOOLBAR, size) );
    m_pBmpMouse = LENMUS_NEW
        wxStaticBitmap(this, wxID_ANY, wxArtProvider::GetIcon("status_mouse",
        wxART_TOOLBAR, size) );

    m_bmpMouseInfoNormal = wxArtProvider::GetBitmap("show_mouse_info",
                                                    wxART_TOOLBAR, size);
    m_bmpMouseInfoSel = wxArtProvider::GetBitmap("show_mouse_info_over",
                                                 wxART_TOOLBAR, size);
    m_bmpCaretInfoNormal = wxArtProvider::GetBitmap("show_caret_info",
                                                    wxART_TOOLBAR, size);
    m_bmpCaretInfoSel = wxArtProvider::GetBitmap("show_caret_info_over",
                                                 wxART_TOOLBAR, size);


    //create buttons
    m_pBtMouse = LENMUS_NEW wxBitmapButton(this, lmID_BUTTON_MOUSE, m_bmpMouseInfoSel,
                                    wxDefaultPosition, size, wxBU_EXACTFIT);
    m_pBtMouse->SetToolTip(_("Display page and time referred to mouse position"));

    m_pBtCaret = LENMUS_NEW wxBitmapButton(this, lmID_BUTTON_CARET, m_bmpCaretInfoNormal,
                                    wxDefaultPosition, size, wxBU_EXACTFIT);
    m_pBtCaret->SetToolTip(_("Display page and time referred to caret position"));

	SetMinHeight(size.y);
}

//---------------------------------------------------------------------------------------
StatusBar::~StatusBar()
{
}

//---------------------------------------------------------------------------------------
void StatusBar::OnButtonMouse(wxCommandEvent& WXUNUSED(event))
{
    DoSelectMouseInfo();
}

//---------------------------------------------------------------------------------------
void StatusBar::OnButtonCaret(wxCommandEvent& WXUNUSED(event))
{
    DoSelectCaretInfo();
}

//---------------------------------------------------------------------------------------
void StatusBar::DoSelectMouseInfo()
{
    m_pBtMouse->SetBitmapLabel(m_bmpMouseInfoSel);
    m_pBtMouse->Refresh();
    m_pBtCaret->SetBitmapLabel(m_bmpCaretInfoNormal);
    m_pBtMouse->Refresh();

    m_fMouseDisplay = true;
    UpdateTimeInfo();
}

//---------------------------------------------------------------------------------------
void StatusBar::DoSelectCaretInfo()
{
    m_pBtMouse->SetBitmapLabel(m_bmpMouseInfoNormal);
    m_pBtMouse->Refresh();
    m_pBtCaret->SetBitmapLabel(m_bmpCaretInfoSel);
    m_pBtMouse->Refresh();

    m_fMouseDisplay = false;
    UpdateTimeInfo();
}

//---------------------------------------------------------------------------------------
void StatusBar::report_status(const wxString& sMsg)
{
    SetStatusText(sMsg, lm_Field_Message);
}

//---------------------------------------------------------------------------------------
void StatusBar::SetNumPage(int nPage)
{
    if (nPage > 0)
        SetStatusText(wxString::Format("%s%d", m_sIconSpace.wx_str(), nPage), lm_Field_NumPage);
    else
        SetStatusText("", lm_Field_NumPage);
}

//---------------------------------------------------------------------------------------
void StatusBar::SetMousePos(float xPos, float yPos)
{
    float x = xPos/100.0f;
    float y = yPos/100.0f;
    SetStatusText(wxString::Format("%sx=%.2f y=%.2f mm",
                                    m_sIconSpace.wx_str(), x, y), lm_Field_MousePos);
}

//---------------------------------------------------------------------------------------
void StatusBar::SetTimePosInfo(TimeUnits rTime, int nMeasure, bool fEmpty)
{
    if (!fEmpty)
        SetStatusText(wxString::Format("%s%d:%.2f", m_sIconSpace.wx_str(),
                                       nMeasure, rTime),
                      lm_Field_RelTime);
    else
        SetStatusText("", lm_Field_RelTime);
}

//---------------------------------------------------------------------------------------
void StatusBar::report_mouse_data(int nPage, TimeUnits rTime, int nMeasure, UPoint uPos)
{
    //save mouse info
    m_nMousePage = nPage;
    m_rMouseTime = rTime;
    m_nMouseMeasure = nMeasure;

    //update displayed info
    UpdateTimeInfo();
    SetMousePos(uPos.x, uPos.y);
}

//---------------------------------------------------------------------------------------
void StatusBar::report_caret_data(int nPage, TimeUnits rTime, int nMeasure)
{
    //save caret info
    m_nCaretPage = nPage;
    m_rCaretTime = rTime;
    m_nCaretMeasure = nMeasure;

    //update displayed info
    UpdateTimeInfo();
}

//---------------------------------------------------------------------------------------
void StatusBar::report_caret_time(const string& timecode)
{
    SetStatusText(wxString::Format("%s%s", m_sIconSpace.wx_str(),
                                       to_wx_string(timecode).wx_str()),
                                   lm_Field_RelTime);
}

//---------------------------------------------------------------------------------------
void StatusBar::UpdateTimeInfo()
{
    if (m_fMouseDisplay)
    {
        SetNumPage(m_nMousePage);
        SetTimePosInfo(m_rMouseTime, m_nMouseMeasure, m_nMousePage < 1);
    }
    else
    {
        SetNumPage(m_nCaretPage);
        SetTimePosInfo(m_rCaretTime, m_nCaretMeasure, m_nCaretPage < 1);
    }
}

//---------------------------------------------------------------------------------------
void StatusBar::OnSize(wxSizeEvent& event)
{
	//position bitmaps in the appropiate field area

	wxRect rect;
    wxSize size;

	//mouse
    GetFieldRect(lm_Field_MousePos, rect);
    size = m_pBmpMouse->GetSize();
    m_pBmpMouse->Move(rect.x,
                      rect.y + (rect.height - size.y) / 2);

    //mouse/caret buttons
    GetFieldRect(lm_Field_MouseCaret, rect);
    size = wxSize(16,16);
    m_pBtMouse->Move(rect.x,
                     rect.y + (rect.height - size.y) / 2);
    m_pBtCaret->Move(rect.x + rect.width/2,
                     rect.y + (rect.height - size.y) / 2);

	//page
    GetFieldRect(lm_Field_NumPage, rect);
    size = m_pBmpPage->GetSize();
    m_pBmpPage->Move(rect.x,
                     rect.y + (rect.height - size.y) / 2);

	//clock
    GetFieldRect(lm_Field_RelTime, rect);
    size = m_pBmpClock->GetSize();
    m_pBmpClock->Move(rect.x,
                      rect.y + (rect.height - size.y) / 2);

    event.Skip();
}


}   //namespace lenmus
