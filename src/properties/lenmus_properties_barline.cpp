//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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
#include "lenmus_properties_barline.h"
#include "lenmus_standard_header.h"

#include "lenmus_utilities.h"
#include "lenmus_string.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_im_attributes.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bmpcbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/clrpicker.h>
#include <wx/choice.h>

namespace lenmus
{

//forward declarations
class CommandGenerator;

typedef int TIntAttribute;


//=======================================================================================
// Implementation of BarlineProperties
//=======================================================================================

const long k_id_barline = wxNewId();

static BarlinesDBEntry tBarlinesDB[k_max_barline+1];


//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(BarlineProperties, PropertiesPage)

wxEND_EVENT_TABLE()

////---------------------------------------------------------------------------------------
////AWARE: pScore is needed as parameter in the constructor for those cases in
////which the text is being created and is not yet included in the score. In this
////cases method GetScore() will fail, so we can not use it in the implementation
////of this class
BarlineProperties::BarlineProperties(DlgProperties* parent, ImoBarline* pBL)
    : PropertiesPage(parent)
    , m_pBL(pBL)
{

    //To avoid having to translate again barline names, we are going to load them
    //by using global function GetBarlineName()
    int i;
    for (i = 0; i < k_max_barline; i++)
    {
        tBarlinesDB[i].nBarlineType = EBarline(i);
        tBarlinesDB[i].sBarlineName = get_barline_name(EBarline(i));
    }
    //End of table item
    tBarlinesDB[i].nBarlineType = k_barline_unknown;
    tBarlinesDB[i].sBarlineName = "";

    CreateControls();
    ApplicationScope& appScope = parent->get_app_scope();
	load_barlines_bitmap_combobox(appScope, m_pBarlinesList, tBarlinesDB);
//	select_barline_in_bitmap_combobox(m_pBarlinesList, m_pBL->GetBarlineType() );
}

//---------------------------------------------------------------------------------------
void BarlineProperties::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pTxtBarline = new wxStaticText( this, wxID_ANY, _("Barline type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtBarline->Wrap( -1 );
	m_pTxtBarline->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Tahoma" ) );

	pMainSizer->Add( m_pTxtBarline, 0, wxALL, 5 );

	wxArrayString m_pBarlinesListChoices;
    m_pBarlinesList = new wxBitmapComboBox();
    m_pBarlinesList->Create(this, k_id_barline, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
							0, nullptr, wxCB_READONLY);
	pMainSizer->Add( m_pBarlinesList, 0, wxALL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
BarlineProperties::~BarlineProperties()
{
}

//---------------------------------------------------------------------------------------
void BarlineProperties::OnAcceptChanges(CommandGenerator* UNUSED(pExecuter), bool UNUSED(fCurrentPage))
{
//	int iB = m_pBarlinesList->GetSelection();
//    EBarline nType = tBarlinesDB[iB].nBarlineType;
//	if (nType == m_pBL->GetBarlineType())
//		return;		//nothing to change
//
//    if (pExecuter)
//    {
//        //Editing an existing object. Do changes by issuing edit commands
//        pExecuter->ChangeBarline(m_pBL, nType, m_pBL->IsVisible());
//    }
//  //  else
//  //  {
//  //      //Direct creation. Modify text object directly
//  //      m_pParentText->SetText( m_pTxtCtrl->GetValue() );
//  //      m_pParentText->SetStyle(pStyle);
//		//m_pParentText->SetAlignment(m_nHAlign);
//  //  }
}


}   //namespace lenmus
