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
#include "lenmus_edit_interface.h"

//lomse
#include <lomse_interactor.h>
using namespace lomse;

//other
#include <list>
using namespace std;


namespace lenmus
{

//=======================================================================================
// Helper class for translating keys, according to user preferences
//=======================================================================================

//context for interpreting a keystroke
enum EKeyContext
{
    k_key_context_none=0,
    k_key_context_any,
    k_key_context_clefs,
    k_key_context_notes,
    k_key_context_note_rest,    //GroupNoteRest
    k_key_context_barlines,
    k_key_context_symbols,
};


class KeyTranslator
{
private:
    ApplicationScope& m_appScope;
    map< pair<int, unsigned>, EKeyCommands> m_any;
    map< pair<int, unsigned>, EKeyCommands> m_notes;
    map< pair<int, unsigned>, EKeyCommands> m_clefs;
    map< pair<int, unsigned>, EKeyCommands> m_barlines;
    map<int, wxString> m_names;

public:
    KeyTranslator(ApplicationScope& appScope);
    ~KeyTranslator();

    int translate(int context, int key, unsigned flags);
    wxString get_key_name(int keyCmd);

};


//---------------------------------------------------------------------------------------
// ToolPage: a page in the ToolBox
class ToolPage: public wxPanel
{
	DECLARE_ABSTRACT_CLASS(ToolPage)

protected:
    wxString    m_sPageToolTip;         //tool tip text for page selection buttons
    wxString    m_sPageBitmapName;      //bitmap to use for page selection buttons

    //info about current group/tool
    EToolGroupID  m_nCurGroupID;
    EToolID       m_nCurToolID;

	wxBoxSizer*		m_pMainSizer;	    //the main sizer for the panel
	ToolboxTheme	m_colors;
    EToolPageID     m_nPageID;          //this page ID
    list<ToolGroup*> m_groups;          //groups in this page

    ToolPage(wxWindow* parent, EToolPageID nPageID);

public:
    ToolPage();
    ~ToolPage();
    virtual void CreatePage(wxWindow* parent, EToolPageID nPageID);
    void AddGroup(ToolGroup* pGroup);
    void add_group(ToolGroup* pGroup);

    //event handlers
    void OnPaintEvent(wxPaintEvent & event);

	inline wxBoxSizer* GetMainSizer() { return m_pMainSizer; }
	void create_layout();
	inline ToolboxTheme* GetColors() { return GetToolBox()->GetColors(); }
    virtual ToolGroup* GetToolGroup(EToolGroupID nGroupID);

    virtual wxString& GetPageToolTip() { return m_sPageToolTip; }
    virtual wxString& GetPageBitmapName() { return m_sPageBitmapName; }
    virtual wxMenu* GetContextualMenuForToolPage() { return (wxMenu*)NULL; }
//    virtual void OnPopUpMenuEvent(wxCommandEvent& event) { event.Skip(); }
    bool process_key(wxKeyEvent& event);
    virtual int translate_key(int key, unsigned keyFlags);

    virtual void create_tool_groups() = 0;
    void ReconfigureForMouseMode(int nMode);

	//current selected group/tool and its options
    inline void SelectGroup(ToolGroup* pGroup) { do_select_group(pGroup); }
    void select_group(EToolGroupID groupID);
    void select_group_and_notify(ToolGroup* pGroup);
    inline EToolGroupID GetCurrentGroupID() const { return m_nCurGroupID; }
    inline EToolID get_selected_tool_id() const { return m_nCurToolID; }
//    virtual wxString GetToolShortDescription() = 0;

    void update_tools_info(ToolsInfo* pInfo);

    //callbacks
    void on_tool_changed(EToolID nToolID, EToolGroupID groupID);

    //enable/disable tools
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

    inline ToolBox* GetToolBox() { return (ToolBox*)GetParent(); }

protected:
    void DeselectRelatedGroups(EToolGroupID nGroupID);
    void do_select_group(ToolGroup* pGroup);
	KeyTranslator* get_key_translator();
    virtual int get_key_translation_context()=0;


    DECLARE_EVENT_TABLE()
};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_PAGE_H__
