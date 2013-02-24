//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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

////wxWidgets
//#include "wx/wxprec.h"
//#ifndef WX_PRECOMP
//    #include "wx/wx.h"
//#endif
//
//#include "lenmus_time_signature_properties.h"
//
//
//
//namespace lenmus
//{
//
//
////---------------------------------------------------------------------------------------
//PropTimeSignature::PropTimeSignature(wxWindow* parent, wxWindowID id,
//                                         const wxString& title, const wxPoint& pos,
//                                         const wxSize& size, long style)
//    : wxDialog( parent, id, title, pos, size, style )
//{
//    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
//
//    //TODO: Review translatable strings
//
//    wxBoxSizer* pMainSizer;
//    pMainSizer = new wxBoxSizer( wxVERTICAL );
//
//    wxString pFrqSizerChoices[] = {
//        _T("2/2"), _T("2/4"), _T("6/8"), _T("2/8"),
//        _T("3/2"), _T("3/4"), _T("9/8"), _T("3/8"),
//        _T("4/2"), _T("4/4"), _T("12/8"), _T("4/8"),
//        _T("other"), _T("none")
//    };
//    int pFrqSizerNChoices = sizeof( pFrqSizerChoices ) / sizeof( wxString );
//    pFrqSizer = new wxRadioBox( this, wxID_ANY, _T("Time signature"),
//                                wxDefaultPosition, wxDefaultSize, pFrqSizerNChoices,
//                                pFrqSizerChoices, 4, wxRA_SPECIFY_COLS );
//    pFrqSizer->SetSelection( 5 );
//    pMainSizer->Add( pFrqSizer, 0, wxALL|wxEXPAND, 5 );
//
//    this->SetSizer( pMainSizer );
//    this->Layout();
//}
//
////---------------------------------------------------------------------------------------
//PropTimeSignature::~PropTimeSignature()
//{
//}
//
//
//}   //namespace lenmus
