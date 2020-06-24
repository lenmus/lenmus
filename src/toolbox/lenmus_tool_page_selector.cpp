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
#include "lenmus_tool_page_selector.h"
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/menu.h>
    #include <wx/log.h>
    #include <wx/dcbuffer.h>
    #include <wx/settings.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
    #include <wx/button.h>
#endif


namespace lenmus
{

//=======================================================================================
// PageSelector implementation

wxBEGIN_EVENT_TABLE(PageSelector, wxPanel)
    EVT_BUTTON(wxID_ANY, PageSelector::on_button_click)
    EVT_PAINT(PageSelector::on_paint_event)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
PageSelector::PageSelector()
    : wxPanel()
{
    initialize();
}

//---------------------------------------------------------------------------------------
PageSelector::PageSelector(wxWindow *parent, wxWindowID id,
                                 const wxString& WXUNUSED(value),
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style)
    : wxPanel(parent, id, pos, size, style)
{
    initialize();
    //Create(parent, id, value, pos, size, style);
    this->SetMinSize( wxSize(165, 40) );

	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxHORIZONTAL );

    //some space at start, for the tab slope
	pMainSizer->Add(0, 0, 0, wxEXPAND|wxLEFT, 8);

    //now the page title
	m_pPageTitle = new wxStaticText(this, wxID_ANY, _("Page title"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pPageTitle->Wrap(135);
	m_pPageTitle->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	pMainSizer->Add( m_pPageTitle, 1, wxALL, 5 );

    //finally, the selector button
	m_pSelector = new wxButton( this, wxID_ANY, "▾", wxDefaultPosition, wxSize( 20,25 ), 0 );
	m_pSelector->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	pMainSizer->Add( m_pSelector, 0, wxLEFT, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
PageSelector::~PageSelector()
{
    for (int id=0; id < k_page_max; ++id)
    {
        int idMenu = m_menuitems[id]->GetId();
        wxMenuItem* pItem = m_pMenu->FindItem(idMenu);
        if (!pItem)
            delete m_menuitems[id];
    }
    delete m_pMenu;
}

//---------------------------------------------------------------------------------------
void PageSelector::initialize()
{
    m_pMenu = new wxMenu;
    for (int i=0; i < k_page_max; ++i)
        m_menuitems[i] = nullptr;
}

//---------------------------------------------------------------------------------------
void PageSelector::add_page_selector(int idPage, wxWindowID idEvent, const wxString& name)
{
    m_pMenu->Append(idEvent, name);
    m_menuitems[idPage] = m_pMenu->FindItem(idEvent);
}

//---------------------------------------------------------------------------------------
void PageSelector::add_separator()
{
    m_pMenu->AppendSeparator();
}

//---------------------------------------------------------------------------------------
void PageSelector::on_button_click(wxCommandEvent& WXUNUSED(event))
{
    PopupMenu(m_pMenu, 10, 2);
}

//---------------------------------------------------------------------------------------
void PageSelector::set_page_title(const wxString& title)
{
    m_pPageTitle->SetLabel(title);
}

//---------------------------------------------------------------------------------------
void PageSelector::enable(bool fEnable)
{
    m_pSelector->Enable(fEnable);
}

//---------------------------------------------------------------------------------------
void PageSelector::selector_visible(bool fVisible)
{
    m_pSelector->Show(fVisible);
}

//---------------------------------------------------------------------------------------
void PageSelector::on_paint_event(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    //tab points
    wxCoord height = 40;
    wxCoord width = 145;
    wxCoord x = 0;
    wxCoord y = 0;
    wxCoord pageWidth = 200;
    int slop = 6;

    wxPoint tabPoints[7];
    tabPoints[0] = wxPoint(x,           y+height-4);
    tabPoints[1] = wxPoint(x+slop,         y+2);
    tabPoints[2] = wxPoint(x+slop+2,        y);
    tabPoints[3] = wxPoint(x+width-slop-2,  y);
    tabPoints[4] = wxPoint(x+width-slop,   y+2);
    tabPoints[5] = wxPoint(x+width,     y+height-4);
    tabPoints[6] = wxPoint(x+pageWidth, y+height-4);

    //use a rectangle for the clipping region
    dc.SetClippingRegion(x, y, pageWidth+1, height-3);

    //draw tab border
    wxColor tabColor = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
    wxColor borderColor = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
    wxPen borderPen = wxPen(borderColor);
    wxPen tabPen = wxPen(tabColor);
    dc.SetPen(borderPen);
    dc.SetBrush(wxBrush(tabColor)); //*wxTRANSPARENT_BRUSH);  //
    dc.DrawPolygon(WXSIZEOF(tabPoints), tabPoints);

    //erase horizontal line at the bottom of the tab
    dc.SetPen(tabColor);
    dc.DrawLine(tabPoints[0].x+1, tabPoints[0].y, tabPoints[5].x, tabPoints[5].y);

    //draw some hidden tabs if more than one
    if (m_pSelector && m_pSelector->IsShown())
    {
        dc.SetPen(borderPen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        int w=5;
        for (int i=0; i < 3; ++i, w+=5)
        {
            dc.DrawLine(tabPoints[3].x+2, tabPoints[3].y+2, tabPoints[3].x+w, tabPoints[3].y+2);
            dc.DrawLine(tabPoints[3].x+w, tabPoints[3].y+2, tabPoints[4].x+w, tabPoints[4].y+2);
            dc.DrawLine(tabPoints[4].x+w, tabPoints[4].y+2, tabPoints[5].x+w, tabPoints[5].y);
        }
    }

//    //draw color border on top of tab
//    dc.SetPen( wxPen( wxColour(141,192,255), 2) );
//    dc.DrawLine(tabPoints[2].x, tabPoints[2].y+2, tabPoints[3].x, tabPoints[3].y+2);

    dc.DestroyClippingRegion();
}

//---------------------------------------------------------------------------------------
void PageSelector::enable_page(int id, bool fEnable)
{
    int idMenu = m_menuitems[id]->GetId();
    wxMenuItem* pItem = m_pMenu->FindItem(idMenu);
    if (!pItem && fEnable)
        m_pMenu->Append( m_menuitems[id] );
    else if (pItem && !fEnable)
        m_pMenu->Remove(idMenu);
}


}   //namespace lenmus
