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

#ifndef __LENMUS_PROPERTIES_GENERAL_H__        //to avoid nested includes
#define __LENMUS_PROPERTIES_GENERAL_H__

//lenmus
#include "lenmus_dlg_properties.h"

//wxWidgets
class wxControl;

//lomse
#include <lomse_im_attributes.h>
using namespace lomse;

//other
#include <list>
using namespace std;


namespace lenmus
{

//functions to get names for attributes' values
typedef const wxString (*pt2Function)(int i);

//---------------------------------------------------------------------------------------
// For identifying controls classes
enum ECtrlType
{
    k_ctrl_choice = 0,      //wxChoice
    k_ctrl_text,            //wxTextCtrl
    k_ctrl_colour,          //wxColourPickerCtrl
};

//---------------------------------------------------------------------------------------
// An item for the table with controls
struct ControlItem
{
    int         id;         //id assigned to this control
    wxControl*  pCtrl;      //pointer to control
    ECtrlType   ctrlType;   //control class
    int         attrb;      //attribute
    EDataType   attrbType;  //attribute data type

    ControlItem(int i, wxControl* c, ECtrlType ct, int a, EDataType at)
        : id(i), pCtrl(c), ctrlType(ct), attrb(a), attrbType(at) {}
};

//---------------------------------------------------------------------------------------
// GeneralProperties: property pages for barlines
class GeneralProperties : public PropertiesPage
{
protected:
    ImoObj* m_pImo;
    list<ControlItem> m_controls;

public:
	GeneralProperties(DlgProperties* parent, ImoObj* pImo);
	~GeneralProperties();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(CommandGenerator* pExecuter, bool fCurrentPage);

    // event handlers

protected:
    void CreateControls();
    pt2Function get_function_for_names(int attrb);

    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif //__LENMUS_PROPERTIES_GENERAL_H__
