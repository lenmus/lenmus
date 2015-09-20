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

#ifndef __LENMUS_PROPERTIES_BARLINE_H__        //to avoid nested includes
#define __LENMUS_PROPERTIES_BARLINE_H__

//lenmus
#include "lenmus_dlg_properties.h"

//wxWidgets
class wxStaticText;
class wxBitmapComboBox;


namespace lenmus
{


//---------------------------------------------------------------------------------------
// BarlineProperties: property pages for barlines
class BarlineProperties : public PropertiesPage
{
public:
	BarlineProperties(DlgProperties* parent, ImoBarline* pBL);
	~BarlineProperties();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(CommandGenerator* pExecuter, bool fCurrentPage);

    // event handlers

protected:
    void CreateControls();

    //controls
	wxStaticText*		m_pTxtBarline;
	wxBitmapComboBox*	m_pBarlinesList;

    //other variables
    ImoBarline*			m_pBL;


    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif //__LENMUS_PROPERTIES_BARLINE_H__
