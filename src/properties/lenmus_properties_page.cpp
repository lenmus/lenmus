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

//lenmus
#include "lenmus_properties_page.h"
#include "lenmus_dlg_properties.h"
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//#include "GeneralProperties.h"
//#include "DlgTextStyles.h"
//#include "../Text.h"
//#include "../../app/ArtProvider.h"
//#include "../../app/ScoreCanvas.h"
//#include "../../app/ScoreDoc.h"         // lmDELAY_REPAINT

namespace lenmus
{


//=======================================================================================
// Implementation of lmGeneralProperties
//=======================================================================================

//---------------------------------------------------------------------------------------
//enum {
//    lmEDIT_CUT = 2100,
//    lmEDIT_COPY,
//    lmEDIT_PASTE,
//    lmEDIT_LEFT,
//    lmEDIT_CENTER,
//    lmEDIT_RIGHT,
//    lmEDIT_STYLE,
//    lmEDIT_CHANGE_STYLE,
//};

//---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmGeneralProperties, lmPropertiesPage)
    //EVT_BUTTON  (lmEDIT_CUT, lmGeneralProperties::OnCut)
    //EVT_BUTTON  (lmEDIT_COPY, lmGeneralProperties::OnCopy)
    //EVT_BUTTON  (lmEDIT_PASTE, lmGeneralProperties::OnPaste)
    //EVT_BUTTON  (lmEDIT_LEFT, lmGeneralProperties::OnLeft)
    //EVT_BUTTON  (lmEDIT_CENTER, lmGeneralProperties::OnCenter)
    //EVT_BUTTON  (lmEDIT_RIGHT, lmGeneralProperties::OnRight)
    //EVT_CHOICE  (lmEDIT_STYLE, lmGeneralProperties::OnStyle)
    //EVT_BUTTON  (lmEDIT_CHANGE_STYLE, lmGeneralProperties::OnEditStyles)
END_EVENT_TABLE()


//---------------------------------------------------------------------------------------
//AWARE: pScore is needed as parameter in the constructor for those cases in
//wich the text is being created and is not yet included in the score. In this
//cases method GetScore() will fail, so we can not use it in the implementation
//of this class
lmGeneralProperties::lmGeneralProperties(lmDlgProperties* parent, lmScoreObj* pSCO,
                                   lmScore* pScore)
    : lmPropertiesPage(parent)
{
    m_pScore = pScore;
    m_pSCO = pSCO;
    CreateControls();
}

//---------------------------------------------------------------------------------------
void lmGeneralProperties::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );


	pMainSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_pTxtTodo = new wxStaticText( this, wxID_ANY, "Not yet implemented", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_pTxtTodo->Wrap( -1 );
	m_pTxtTodo->SetFont( wxFont( 14, 74, 90, 92, false, "Tahoma" ) );

	pMainSizer->Add( m_pTxtTodo, 1, wxALL|wxEXPAND, 5 );


	pMainSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
lmGeneralProperties::~lmGeneralProperties()
{
}

//---------------------------------------------------------------------------------------
void lmGeneralProperties::OnAcceptChanges(lmController* pController, bool fCurrentPage)
{
}


}   //namespace lenmus
