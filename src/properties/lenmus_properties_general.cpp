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
#include "lenmus_properties_general.h"
#include "lenmus_standard_header.h"

#include "lenmus_utilities.h"
#include "lenmus_string.h"
#include "lenmus_command_event_handler.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_basic.h>
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
// Implementation of GeneralProperties
//=======================================================================================

const int k_id_ctrl = 2600;

wxBEGIN_EVENT_TABLE(GeneralProperties, PropertiesPage)

wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
GeneralProperties::GeneralProperties(DlgProperties* parent, ImoObj* pImo)
    : PropertiesPage(parent)
    , m_pImo(pImo)
{
    CreateControls();
}

//---------------------------------------------------------------------------------------
void GeneralProperties::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );
	int idCtrl = k_id_ctrl;

    list<TIntAttribute> attribs = m_pImo->get_supported_attributes();
    list<TIntAttribute>::reverse_iterator it;
    for (it = attribs.rbegin(); it != attribs.rend(); ++it)
    {
        AttributesData data = AttributesTable::get_data_for(*it);

        wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
        pMainSizer->Add( sizer, 0, wxALL, 5 );

        //label
        wxString label = wxGetTranslation( to_wx_string(data.label) );
        wxStaticText* text = new wxStaticText(this, wxID_ANY, label, wxDefaultPosition,
                                              wxDefaultSize, 0 );
        text->Wrap( -1 );
        text->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Tahoma" ) );
        sizer->Add( text, 0, wxALL, 5 );

        //value
        switch (data.type)
        {
            case k_type_int:
            {
                int value = m_pImo->get_int_attribute(data.attrb);
                pt2Function funct = get_function_for_names(data.attrb);
                if (funct != nullptr)
                {
                    wxChoice* ctrl = new wxChoice(this, idCtrl);
                    for (int i=data.minimum; i <= data.maximum; ++i)
                        ctrl->AppendString( funct(i) );
                    ctrl->Select(value);
                    sizer->Add( ctrl, 0, wxALL, 5 );
                    m_controls.push_back(
                        ControlItem(idCtrl++, ctrl, k_ctrl_choice, data.attrb, data.type) );
                }
                else
                {
                    wxString sValue = wxString::Format("%d", value);
                    wxTextCtrl* ctrl = new wxTextCtrl(this, idCtrl, sValue, wxDefaultPosition,
                                                       wxDefaultSize, 0 );
                    ctrl->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Tahoma" ) );
                    sizer->Add( ctrl, 0, wxALL, 5 );
                    m_controls.push_back(
                        ControlItem(idCtrl++, ctrl, k_ctrl_text, data.attrb, data.type) );
                }
                break;
            }
            case k_type_color:
            {
                Color color = m_pImo->get_color_attribute(data.attrb);
                wxColour value(color.r, color.g, color.b);
                wxColourPickerCtrl* ctrl = new wxColourPickerCtrl(this, idCtrl, value);
                sizer->Add( ctrl, 0, wxALL, 5 );
                m_controls.push_back(
                    ControlItem(idCtrl++, ctrl, k_ctrl_colour, data.attrb, data.type) );
                break;
            }
            case k_type_bool:
            {
                bool value = m_pImo->get_bool_attribute(data.attrb);
                wxChoice* ctrl = new wxChoice(this, idCtrl);
                ctrl->AppendString(_("false"));
                ctrl->AppendString(_("true"));
                ctrl->Select(value ? 1 : 0);
                sizer->Add( ctrl, 0, wxALL, 5 );
                m_controls.push_back(
                    ControlItem(idCtrl++, ctrl, k_ctrl_choice, data.attrb, data.type) );
                break;
            }
            case k_type_double:
            {
                double value = m_pImo->get_double_attribute(data.attrb);
                wxString sValue = wxString::Format("%.4f", value);
                wxTextCtrl* ctrl = new wxTextCtrl(this, idCtrl, sValue, wxDefaultPosition,
                                                   wxDefaultSize, 0 );
                ctrl->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Tahoma" ) );
                sizer->Add( ctrl, 0, wxALL, 5 );
                m_controls.push_back(
                    ControlItem(idCtrl++, ctrl, k_ctrl_text, data.attrb, data.type) );
                break;
            }
            case k_type_string:
            {
                wxString value = to_wx_string( m_pImo->get_string_attribute(data.attrb) );
                wxTextCtrl* ctrl = new wxTextCtrl(this, idCtrl, value, wxDefaultPosition,
                                                  wxDefaultSize, 0 );
                ctrl->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Tahoma" ) );
                sizer->Add( ctrl, 0, wxALL, 5 );
                m_controls.push_back(
                    ControlItem(idCtrl++, ctrl, k_ctrl_text, data.attrb, data.type) );
                break;
            }
            default:
                break;
        }

        //units
        if (!data.units.empty())
        {
            wxString label = wxGetTranslation( to_wx_string(data.units) );
            wxStaticText* text = new wxStaticText(this, wxID_ANY, label, wxDefaultPosition,
                                                  wxDefaultSize, 0 );
            text->Wrap( -1 );
            text->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Tahoma" ) );
            sizer->Add( text, 0, wxALL, 5 );
        }

    }

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
pt2Function GeneralProperties::get_function_for_names(int attrb)
{
    switch (attrb)
    {
        case k_attr_barline:        return &get_barline_name;
        case k_attr_stem_type:      return &get_stem_name;
        default:
            return nullptr;
    }
}

//---------------------------------------------------------------------------------------
GeneralProperties::~GeneralProperties()
{
}

//---------------------------------------------------------------------------------------
void GeneralProperties::OnAcceptChanges(CommandGenerator* pExecuter, bool WXUNUSED(fCurrentPage))
{
    list<ControlItem>::const_iterator it;
    for (it = m_controls.begin(); it != m_controls.end(); ++it)
    {
        //get current value
        wxString sValue;
        wxColour color;
        int iValue = 0;
        ControlItem item = *it;
        switch (item.ctrlType)
        {
            case k_ctrl_choice:
            {
                wxChoice* ctrl = static_cast<wxChoice*>(item.pCtrl);
                iValue = ctrl->GetCurrentSelection();
                sValue = ctrl->GetString(iValue);
                break;
            }
            case k_ctrl_text:
            {
                wxTextCtrl* ctrl = static_cast<wxTextCtrl*>(item.pCtrl);
                sValue = ctrl->GetValue();
                break;
            }
            case k_ctrl_colour:
            {
                wxColourPickerCtrl* ctrl = static_cast<wxColourPickerCtrl*>(item.pCtrl);
                color = ctrl->GetColour();
                break;
            }
        }

        //get old value
        switch (item.attrbType)
        {
            case k_type_int:
            {
                int oldInt = m_pImo->get_int_attribute(item.attrb);
                pt2Function funct = get_function_for_names(item.attrb);
                if (item.ctrlType == k_ctrl_choice && funct != nullptr)
                {
                    if (iValue != oldInt)
                        pExecuter->change_attribute(m_pImo, item.attrb, iValue);
                }
                else if (item.ctrlType == k_ctrl_text)
                {
                    long newValue;
                    bool fOk = sValue.ToLong(&newValue);
                    if (fOk && int(newValue) != oldInt)
                        pExecuter->change_attribute(m_pImo, item.attrb, int(newValue));
                }
                break;
            }
            case k_type_color:
            {
                Color oldColor = m_pImo->get_color_attribute(item.attrb);
                Color newColor( color.Red(), color.Green(), color.Blue() );
                if (!lomse::is_equal(oldColor, newColor))
                    pExecuter->change_attribute(m_pImo, item.attrb, newColor);
                break;
            }
            case k_type_bool:
            {
//                bool oldValue = m_pImo->get_bool_attribute(item.attrb);
//                bool newValue = (sValue == "true");
//                if (oldValue != newValue)
//                    pExecuter->change_attribute(m_pImo, item.attrb, newValue);
                //TODO Bool attributes are not considered in Lomse !!
            }
            case k_type_double:
            {
                double oldValue = m_pImo->get_double_attribute(item.attrb);
                double newValue;
                bool fOk = sValue.ToDouble(&newValue);
                if (fOk && newValue != oldValue)
                    pExecuter->change_attribute(m_pImo, item.attrb, newValue);
                break;
            }
            case k_type_string:
            {
                wxString oldValue = to_wx_string( m_pImo->get_string_attribute(item.attrb) );
                if (oldValue != sValue)
                {
                    string newValue = to_std_string(sValue);
                    pExecuter->change_attribute(m_pImo, item.attrb, newValue);
                }
                break;
            }
            default:
                break;
        }
    }

//		return;		//nothing to change
//
//    if (pExecuter)
//    {
//        //Editing an existing object. Do changes by issuing edit commands
//        pExecuter->ChangeBarline(m_pImo, nType, m_pImo->IsVisible());
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
