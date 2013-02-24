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

#ifndef __LENMUS_TOOL_BARLINES_H__
#define __LENMUS_TOOL_BARLINES_H__

//lenmus
#include "lenmus_tool_group.h"
#include "lenmus_tool_page.h"
//#include "../../score/defs.h"

//lomse
#include "lomse_internal_model.h"
using namespace lomse;

class wxBitmapComboBox;
class wxRadioBox;
class wxListBox;


using namespace std;


namespace lenmus
{

class CheckButton;

enum EBarline
{
    k_barline_unknown = -1,
	k_barline_simple = ImoBarline::k_simple,
	k_barline_double = ImoBarline::k_double,
    k_barline_start = ImoBarline::k_start,
    k_barline_end = ImoBarline::k_end,
    k_barline_end_repetition = ImoBarline::k_end_repetition,
    k_barline_start_repetition = ImoBarline::k_start_repetition,
    k_barline_double_repetition = ImoBarline::k_double_repetition,

    k_max_barline,
};


//---------------------------------------------------------------------------------------
// global functions related to barlines
//---------------------------------------------------------------------------------------
const wxString& get_barline_name(EBarline barlineType);



//---------------------------------------------------------------------------------------
// Group for barlines type
//---------------------------------------------------------------------------------------
class GrpBarlines: public ToolGroup
{
public:
    GrpBarlines(ToolPage* pParent, wxBoxSizer* pMainSizer, int nValidMouseModes);
    ~GrpBarlines() {}

    //implement virtual methods
    inline EToolGroupID GetToolGroupID() { return k_grp_BarlineType; }
    EToolID GetCurrentToolID();

	//access to selected barline
	EBarline GetSelectedBarline();

	//event handlers
    void OnBarlinesList(wxCommandEvent& event);


private:
    void CreateGroupControls(wxBoxSizer* pMainSizer);

	wxBitmapComboBox*   m_pBarlinesList;

    DECLARE_EVENT_TABLE()
};


//---------------------------------------------------------------------------------------
// The page panel
//---------------------------------------------------------------------------------------
class ToolPageBarlines : public ToolPage
{
	DECLARE_DYNAMIC_CLASS(ToolPageBarlines)

public:
    ToolPageBarlines();
    ToolPageBarlines(wxWindow* parent);
    ~ToolPageBarlines();
    void Create(wxWindow* parent);

    //implementation of virtual methods
    void CreateGroups();

    //current tool/group info
    wxString GetToolShortDescription();

    //interface with groups
        //barlines
    inline EBarline GetSelectedBarline() { return m_pGrpBarlines->GetSelectedBarline(); }


private:

    //groups
    GrpBarlines*		m_pGrpBarlines;

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_BARLINES_H__
