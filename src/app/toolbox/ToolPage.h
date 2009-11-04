//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_TOOLPAGE_H__
#define __LM_TOOLPAGE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolPage.cpp"
#endif

#include <list>

#include "ColorScheme.h"
#include "ToolsBox.h"
#include "ToolGroup.h"


class lmToolPage: public wxPanel
{
	DECLARE_ABSTRACT_CLASS(lmToolPage)

public:
    lmToolPage(wxWindow* parent, lmEToolPageID nPageID);
    lmToolPage();
    ~lmToolPage();
    virtual void CreatePage(wxWindow* parent, lmEToolPageID nPageID);
    void AddGroup(lmToolGroup* pGroup);

    //event handlers
    void OnPaintEvent(wxPaintEvent & event);

	inline wxBoxSizer* GetMainSizer() { return m_pMainSizer; }
	void CreateLayout();
	inline lmColorScheme* GetColors() { return GetToolBox()->GetColors(); }
    virtual lmToolGroup* GetToolGroup(lmEToolGroupID nGroupID) = 0;

    virtual wxString& GetPageToolTip() { return m_sPageToolTip; }
    virtual wxString& GetPageBitmapName() { return m_sPageBitmapName; }
    virtual wxMenu* GetContextualMenuForToolPage() { return (wxMenu*)NULL; }
    virtual void OnPopUpMenuEvent(wxCommandEvent& event) { event.Skip(); }

    virtual void CreateGroups() = 0;
    virtual bool DeselectRelatedGroups(lmEToolGroupID nGroupID) = 0;
    void ReconfigureForMouseMode(int nMode);

	//current selected group/tool and its options
    inline lmEToolGroupID GetCurrentGroupID() const { return m_nCurGroupID; }
    inline lmEToolID GetCurrentToolID() const { return m_nCurToolID; }
    virtual wxString GetToolShortDescription() = 0;

    //callbacks
    void OnToolChanged(lmEToolGroupID nGroupID, lmEToolID nToolID);


protected:
    inline lmToolBox* GetToolBox() { return (lmToolBox*)GetParent(); }

    wxString    m_sPageToolTip;         //tool tip text
    wxString    m_sPageBitmapName;      //bitmap to use
    bool        m_fGroupsCreated;       //to avoid handling events until groups created

    //info about current group/tool
    lmEToolGroupID  m_nCurGroupID;
    lmEToolID       m_nCurToolID;

private:
	wxBoxSizer*		m_pMainSizer;	    //the main sizer for the panel
	lmColorScheme	m_colors;
    lmEToolPageID   m_nPageID;          //this page ID
    std::list<lmToolGroup*> m_Groups;   //groups in this page

    DECLARE_EVENT_TABLE()
};

#endif    // __LM_TOOLPAGE_H__
