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

#ifndef __LENMUS_TOOL_PAGE_H__
#define __LENMUS_TOOL_PAGE_H__

//lenmus
#include "lenmus_tool_box_theme.h"
#include "lenmus_tool_box.h"
#include "lenmus_tool_group.h"

//other
#include <list>
using namespace std;


namespace lenmus
{


class ToolPage: public wxPanel
{
	DECLARE_ABSTRACT_CLASS(ToolPage)

public:
    ToolPage(wxWindow* parent, EToolPageID nPageID);
    ToolPage();
    ~ToolPage();
    virtual void CreatePage(wxWindow* parent, EToolPageID nPageID);
    void AddGroup(ToolGroup* pGroup);

    //event handlers
    void OnPaintEvent(wxPaintEvent & event);

	inline wxBoxSizer* GetMainSizer() { return m_pMainSizer; }
	void CreateLayout();
	inline ToolboxTheme* GetColors() { return GetToolBox()->GetColors(); }
    virtual ToolGroup* GetToolGroup(EToolGroupID nGroupID);

    virtual wxString& GetPageToolTip() { return m_sPageToolTip; }
    virtual wxString& GetPageBitmapName() { return m_sPageBitmapName; }
    virtual wxMenu* GetContextualMenuForToolPage() { return (wxMenu*)NULL; }
    virtual void OnPopUpMenuEvent(wxCommandEvent& event) { event.Skip(); }

    virtual void CreateGroups() = 0;
    void ReconfigureForMouseMode(int nMode);

	//current selected group/tool and its options
    void SelectGroup(ToolGroup* pGroup);
    inline EToolGroupID GetCurrentGroupID() const { return m_nCurGroupID; }
    inline EToolID GetCurrentToolID() const { return m_nCurToolID; }
    virtual wxString GetToolShortDescription() = 0;

    //callbacks
    void OnToolChanged(EToolGroupID nGroupID, EToolID nToolID);


protected:
    inline ToolBox* GetToolBox() { return (ToolBox*)GetParent(); }
    void DeselectRelatedGroups(EToolGroupID nGroupID);

    wxString    m_sPageToolTip;         //tool tip text
    wxString    m_sPageBitmapName;      //bitmap to use
    bool        m_fGroupsCreated;       //to avoid handling events until groups created

    //info about current group/tool
    EToolGroupID  m_nCurGroupID;
    EToolID       m_nCurToolID;

private:
	wxBoxSizer*		m_pMainSizer;	    //the main sizer for the panel
	ToolboxTheme	m_colors;
    EToolPageID     m_nPageID;          //this page ID
    std::list<ToolGroup*> m_Groups;   //groups in this page

    DECLARE_EVENT_TABLE()
};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_PAGE_H__
